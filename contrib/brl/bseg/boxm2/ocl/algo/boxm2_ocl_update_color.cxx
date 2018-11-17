// This is brl/bseg/boxm2/ocl/algo/boxm2_ocl_update_color.cxx
#include "boxm2_ocl_update_color.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <utility>
//:
// \file
// \brief  A process for updating a color model
//
// \author Vishal Jain
// \date Mar 25, 2011

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/ocl/algo/boxm2_ocl_camera_converter.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>

//directory utility
#include <vul/vul_timer.h>
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

//: Map of kernels should persist between process executions
std::map<std::string,std::vector<bocl_kernel*> > boxm2_ocl_update_color::kernels_;

bool boxm2_ocl_update_color::update_color(const boxm2_scene_sptr&         scene,
                                          const bocl_device_sptr&         device,
                                          const boxm2_opencl_cache_sptr&  opencl_cache,
                                          const vpgl_camera_double_sptr&  cam,
                                          const vil_image_view_base_sptr& img,
                                          std::string               in_identifier,
                                          const std::string&    mask_filename,
                                          bool                     update_alpha,
                                          std::size_t               startI,
                                          std::size_t               startJ)
{
  vil_image_view_base_sptr mask_img;
  if (mask_filename!="")
  {
    std::cout<<"MASK FOUND"<<std::endl;
    mask_img=vil_load(mask_filename.c_str());
  }
  return update_color(scene, device, opencl_cache, cam, img, std::move(in_identifier), mask_img, update_alpha, startI, startJ);
}

//Main public method, updates color model
bool boxm2_ocl_update_color::update_color(const boxm2_scene_sptr&         scene,
                                          bocl_device_sptr         device,
                                          const boxm2_opencl_cache_sptr&  opencl_cache,
                                          vpgl_camera_double_sptr  cam,
                                          const vil_image_view_base_sptr& img,
                                          const std::string&               in_identifier,
                                          const vil_image_view_base_sptr& mask_img,
                                          bool                     update_alpha,
                                          std::size_t               startI,
                                          std::size_t               startJ)
{
  float transfer_time=0.0f;
  float gpu_time=0.0f;

  //setup local/global size
  std::size_t local_threads[2]={8,8};
  std::size_t global_threads[2]={8,8};
  enum {
      UPDATE_SEGLEN = 0,
      UPDATE_COMPRESS = 1,
      UPDATE_PREINF = 2,
      UPDATE_PROC   = 3,
      UPDATE_BAYES  = 4,
      UPDATE_CELL   = 5
  };

  //make sure the scene corresponds to this datatype
  bool foundDataType = false, foundNumObsType = false;
  std::string data_type, num_obs_type, options;
  if ( scene->has_data_type(boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix(in_identifier)) ) {
    data_type = boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix(in_identifier);
    foundDataType = true;
    options=" -D MOG_TYPE_8 ";
  }
  if ( scene->has_data_type(boxm2_data_traits<BOXM2_NUM_OBS_SINGLE>::prefix(in_identifier)) ) {
    num_obs_type = boxm2_data_traits<BOXM2_NUM_OBS_SINGLE>::prefix(in_identifier);
    foundNumObsType = true;
  }
  if (!foundDataType) {
    std::cout<<"boxm2_ocl_update_color: ERROR: scene doesn't have BOXM2_GAUSS_RGB data type"<<std::endl;
    return false;
  }
  if (!foundNumObsType) {
    std::cout<<"boxm2_ocl_update_color: ERROR: scene doesn't have BOXM2_NUM_OBS_SINGLE type"<<std::endl;
    return false;
  }

  // create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device->context(),*(device->device_id()),
                                                CL_QUEUE_PROFILING_ENABLE,&status);
  if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)) )
    return false;

  // compile kernels if not already compiled
  std::vector<bocl_kernel*>& kernels = get_kernels(device, options);

  //prepare input image
  vil_image_view_base_sptr float_img = boxm2_util::prepare_input_image(img, false);
  if ( float_img->pixel_format() != VIL_PIXEL_FORMAT_RGBA_BYTE ) {
    std::cout<<"boxm2_ocl_update_color::using a non RGBA image!!"<<std::endl;
    return false;
  }
  auto* img_view = static_cast<vil_image_view<vil_rgba<vxl_byte> >* >(float_img.ptr());

  //grab input image dimensions - round up to the nearest 8 for OPENCL
  unsigned cl_ni=RoundUp(img_view->ni(),local_threads[0]);
  unsigned cl_nj=RoundUp(img_view->nj(),local_threads[1]);
  global_threads[0]=cl_ni;
  global_threads[1]=cl_nj;

  // create all buffers
  auto* ray_origins = new cl_float[4*cl_ni*cl_nj];
  auto* ray_directions = new cl_float[4*cl_ni*cl_nj];
  bocl_mem_sptr ray_o_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_origins, "ray_origins buffer");
  bocl_mem_sptr ray_d_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_directions, "ray_directions buffer");
  boxm2_ocl_camera_converter::compute_ray_image( device, queue, cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff, startI, startJ);

  //create vis, pre, norm and input image buffers
  auto* vis_buff = new float[cl_ni*cl_nj];
  auto* pre_buff = new float[cl_ni*cl_nj];
  auto* norm_buff = new float[cl_ni*cl_nj];
  std::fill(vis_buff, vis_buff+cl_ni*cl_nj, 1.0f);
  std::fill(pre_buff, pre_buff+cl_ni*cl_nj, 0.0f);
  std::fill(norm_buff, norm_buff+cl_ni*cl_nj, 0.0f);

  float tnearfar[2] = { 0.0f, 1000000.0f} ;
  bocl_mem_sptr tnearfar_mem_ptr = opencl_cache->alloc_mem(2*sizeof(float), tnearfar, "tnearfar  buffer");
  tnearfar_mem_ptr->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  //initialize input image buffer
  int numFloats = 4;
  auto* input_buff = new float[numFloats*cl_ni*cl_nj];  //need to store RGB (or YUV values)
  int count=0;
  for (unsigned int j=0;j<cl_nj;++j) {
    for (unsigned int i=0;i<cl_ni;++i) {
      //rgba values
      input_buff[numFloats*count] = 0.0f;
      input_buff[numFloats*count + 1] = 0.0f;
      input_buff[numFloats*count + 2] = 0.0f;
      input_buff[numFloats*count + 3] = 1.0f;
      if (i<img_view->ni() && j< img_view->nj()) {
        vil_rgba<vxl_byte> rgba = (*img_view)(i,j);
        input_buff[numFloats*count + 0] = (float) rgba.R() / 255.0f;
        input_buff[numFloats*count + 1] = (float) rgba.G() / 255.0f;
        input_buff[numFloats*count + 2] = (float) rgba.B() / 255.0f;
        input_buff[numFloats*count + 3] = (float) 1.0f;
      }
      ++count;
    }
  }

  //bocl_mem_sptr in_image=new bocl_mem(device->context(),input_buff,cl_ni*cl_nj*sizeof(float),"input image buffer");
  bocl_mem_sptr in_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), input_buff, "input image buffer");
  in_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //bocl_mem_sptr vis_image=new bocl_mem(device->context(),vis_buff,cl_ni*cl_nj*sizeof(float),"vis image buffer");
  bocl_mem_sptr vis_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), vis_buff, "vis image buffer");
  vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //bocl_mem_sptr pre_image=new bocl_mem(device->context(),pre_buff,cl_ni*cl_nj*sizeof(float),"pre image buffer");
  bocl_mem_sptr pre_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), pre_buff, "pre image buffer");
  pre_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //bocl_mem_sptr norm_image=new bocl_mem(device->context(),norm_buff,cl_ni*cl_nj*sizeof(float),"pre image buffer");
  bocl_mem_sptr norm_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), norm_buff, "norm image buffer");
  norm_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // Image Dimensions
  unsigned int img_dim_buff[4] = {0, 0, img_view->ni(), img_view->nj() };
  bocl_mem_sptr img_dim=new bocl_mem(device->context(), img_dim_buff, sizeof(unsigned int)*4, "image dims");
  img_dim->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // Output Array
  float output_arr[100];
  for (float & i : output_arr) i = 0.0f;
  bocl_mem_sptr  cl_output=new bocl_mem(device->context(), output_arr, sizeof(float)*100, "output buffer");
  cl_output->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //update alpha boolean
  int upAlpha = update_alpha ? 1 : 0;
  bocl_mem_sptr upAlphaMem = new bocl_mem(device->context(), &upAlpha, sizeof(upAlpha), "update alpha bool buffer");
  upAlphaMem->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // bit lookup buffer
  cl_uchar lookup_arr[256];
  boxm2_ocl_util::set_bit_lookup(lookup_arr);
  bocl_mem_sptr lookup=new bocl_mem(device->context(), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
  lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  // app density used for proc_norm_image
  float app_buffer[4]={1.0,0.0,0.0,0.0};
  bocl_mem_sptr app_density = new bocl_mem(device->context(), app_buffer, sizeof(cl_float4), "app density buffer");
  app_density->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // set arguments
  std::vector<boxm2_block_id> vis_order;
  if(cam->type_name() == "vpgl_perspective_camera")
      vis_order= scene->get_vis_blocks_opt((vpgl_perspective_camera<double>*)cam.ptr(),img_view->ni(),img_view->nj());
  else
      vis_order= scene->get_vis_blocks(cam);

  std::vector<boxm2_block_id>::iterator id;
  for (unsigned int i=0; i<kernels.size(); ++i)
  {
    if ( i == UPDATE_PROC ) {
      local_threads[0] = 8; local_threads[1] = 8;
      global_threads[0]=cl_ni; global_threads[1]=cl_nj;
      bocl_kernel * proc_kern=kernels[i];

      proc_kern->set_arg( norm_image.ptr() );
      proc_kern->set_arg( vis_image.ptr() );
      proc_kern->set_arg( pre_image.ptr());
      proc_kern->set_arg( img_dim.ptr() );

      //execute kernel
      proc_kern->execute( queue, 2, local_threads, global_threads);
      int status = clFinish(queue);
      if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)) )
        return false;
      proc_kern->clear_args();
      norm_image->read_to_buffer(queue);
      continue;
    }
    vis_image->read_to_buffer(queue);
    if (mask_img) {
      // boolean mask
      if (auto * mask_char
          =dynamic_cast<vil_image_view<unsigned char> * >(mask_img.ptr()))
      {
        int count = 0;
        for (unsigned int j=0;j<cl_nj;++j)
          for (unsigned int i=0;i<cl_ni;++i)
          {
            if (i<mask_char->ni() && j<mask_char->nj()  )
            {
              if ( (*mask_char)(i,j)>0 )
              {
                input_buff[numFloats*count+0]=-1.0f;
                vis_buff[count] =-1.0f;
              }
            }
            ++count;
          }
      }
      else if (auto * mask_float =
               dynamic_cast<vil_image_view<float> * >(mask_img.ptr()) ) {
        int count = 0;
        for (unsigned int j=0;j<cl_nj;++j) {
          for (unsigned int i=0;i<cl_ni;++i) {
            if (i<mask_float->ni() && j<mask_float->nj()  )
            {
              float mask_val = (*mask_float)(i,j);
              if (mask_val < 0.0f) { mask_val = 0.0f;}
              if (mask_val > 1.0f) { mask_val = 1.0f;}

              if ( mask_val == 0 )
              {
                input_buff[numFloats*count+0]=-1.0f;
                vis_buff[count] =-1.0f;
              }
              else {
                vis_buff[count] = mask_val;
              }
              ++count;
            }
          }
        }
      }
      else {
        std::cerr << "ERROR: boxm2_ocl_update_color: upsupported mask type! " << std::endl;
        return false;
      }
      in_image->write_to_buffer(queue);
    }
    else {
      // no mask
      int count = 0;
      for (unsigned int j=0;j<cl_nj;++j) {
        for (unsigned int i=0;i<cl_ni;++i)
        {
          vis_buff[count] =1.0f;
        }
      }
      ++count;
    }
    vis_image->write_to_buffer(queue);
    clFinish(queue);
    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
      //choose correct render kernel
      boxm2_block_metadata mdata = scene->get_block_metadata(*id);
      bocl_kernel* kern =  kernels[i];

      //write the image values to the buffer
      vul_timer transfer;
      bocl_mem* blk       = opencl_cache->get_block(scene,*id);
      bocl_mem* alpha     = opencl_cache->get_data<BOXM2_ALPHA>(scene,*id, 0, false);
      bocl_mem * blk_info  = opencl_cache->loaded_block_info();

      //make sure the scene info data size reflects the real data size
      auto* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
      int alphaTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
      info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);
      blk_info->write_to_buffer((queue));

      // data type string may contain an identifier so determine the buffer size
      int nobsTypeSize = (int)boxm2_data_info::datasize(num_obs_type);
      int appTypeSize = (int) boxm2_data_info::datasize(data_type);
      bocl_mem* mog       = opencl_cache->get_data(scene,*id, data_type, info_buffer->data_buffer_length*appTypeSize, false);
      bocl_mem* num_obs   = opencl_cache->get_data(scene,*id, num_obs_type, info_buffer->data_buffer_length*nobsTypeSize, false);

      //grab an appropriately sized AUX data buffer
      int auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX0>::prefix());
      bocl_mem *aux0   = opencl_cache->get_data<BOXM2_AUX0>(scene,*id, info_buffer->data_buffer_length*auxTypeSize);
      auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX1>::prefix());
      bocl_mem *aux1   = opencl_cache->get_data<BOXM2_AUX1>(scene,*id, info_buffer->data_buffer_length*auxTypeSize);
      auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX2>::prefix());
      bocl_mem *aux2   = opencl_cache->get_data<BOXM2_AUX2>(scene,*id, info_buffer->data_buffer_length*auxTypeSize);
      auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX3>::prefix());
      bocl_mem *aux3   = opencl_cache->get_data<BOXM2_AUX3>(scene,*id, info_buffer->data_buffer_length*auxTypeSize);

      transfer_time += (float) transfer.all();
      if (i==UPDATE_SEGLEN)
      {
        aux0->zero_gpu_buffer(queue, true);
        aux1->zero_gpu_buffer(queue, true);
        aux2->zero_gpu_buffer(queue, true);
        aux3->zero_gpu_buffer(queue, true);
        cl_int init_status = clFinish(queue);

        if (!check_val(init_status, MEM_FAILURE, "initializaiton of aux buffers failed!: " + error_to_string(status)) ) {
          return false;
        }
        local_threads[0] = 8; local_threads[1] = 8;
        global_threads[0]=cl_ni; global_threads[1]=cl_nj;
        kern->set_arg( blk_info );
        kern->set_arg( blk );
        kern->set_arg( alpha );
        kern->set_arg( aux0 );
        kern->set_arg( aux1 );
        kern->set_arg( aux2 );
        kern->set_arg( aux3 );
        kern->set_arg( lookup.ptr() );
        kern->set_arg( ray_o_buff.ptr() );
        kern->set_arg( ray_d_buff.ptr() );
        kern->set_arg( tnearfar_mem_ptr.ptr() );
        kern->set_arg( img_dim.ptr() );
        kern->set_arg( in_image.ptr() );
        kern->set_arg( cl_output.ptr() );
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar16) );//local tree,
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar4) ); //ray bundle,
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_int) );    //cell pointers,
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_float4) ); //cached aux,
        kern->set_local_arg( local_threads[0]*local_threads[1]*10*sizeof(cl_uchar) ); //cumsum buffer, imindex buffer
      }
      else if (i==UPDATE_COMPRESS)
      {
        local_threads[0] = 64;
        local_threads[1] = 1 ;
        global_threads[0]=RoundUp(info_buffer->data_buffer_length,local_threads[0]);
        global_threads[1]=1;
        kern->set_arg( blk_info );
        kern->set_arg( aux0 );
        kern->set_arg( aux1 );
        kern->set_arg( aux2 );
        kern->set_arg( aux3 );
      }
      else if (i==UPDATE_PREINF)
      {
        local_threads[0] = 8; local_threads[1] = 8;
        global_threads[0]=cl_ni; global_threads[1]=cl_nj;
        kern->set_arg( blk_info );
        kern->set_arg( blk );
        kern->set_arg( alpha );
        kern->set_arg( mog );
        kern->set_arg( num_obs );
        kern->set_arg( aux0 );
        kern->set_arg( aux1 );
        kern->set_arg( lookup.ptr() );

        kern->set_arg( ray_o_buff.ptr() );
        kern->set_arg( ray_d_buff.ptr() );
        kern->set_arg( tnearfar_mem_ptr.ptr() );
        kern->set_arg( img_dim.ptr() );
        kern->set_arg( vis_image.ptr() );
        kern->set_arg( pre_image.ptr() );
        kern->set_arg( cl_output.ptr() );
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar16) );//local tree,
        kern->set_local_arg( local_threads[0]*local_threads[1]*10*sizeof(cl_uchar) ); //cumsum buffer, imindex buffer
      }
      else if (i==UPDATE_BAYES)
      {
        local_threads[0] = 8; local_threads[1] = 8;
        global_threads[0]=cl_ni; global_threads[1]=cl_nj;
        kern->set_arg( blk_info );
        kern->set_arg( blk );
        kern->set_arg( alpha );
        kern->set_arg( mog );
        kern->set_arg( num_obs );
        kern->set_arg( aux0 );
        kern->set_arg( aux1 );
        kern->set_arg( aux2 );
        kern->set_arg( aux3 );
        kern->set_arg( lookup.ptr() );

        kern->set_arg( ray_o_buff.ptr() );
        kern->set_arg( ray_d_buff.ptr() );
        kern->set_arg( tnearfar_mem_ptr.ptr() );
        kern->set_arg( img_dim.ptr() );
        kern->set_arg( vis_image.ptr() );
        kern->set_arg( pre_image.ptr() );
        kern->set_arg( norm_image.ptr() );
        kern->set_arg( cl_output.ptr() );
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar16) );//local tree,
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_short2) ); //ray bundle,
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_int) );    //cell pointers,
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_float) ); //cached aux,
        kern->set_local_arg( local_threads[0]*local_threads[1]*10*sizeof(cl_uchar) ); //cumsum buffer, imindex buffer
      }
      else if (i==UPDATE_CELL)
      {
        //set space and execute
        local_threads[0] = 64;
        local_threads[1] = 1 ;
        global_threads[0]=RoundUp(info_buffer->data_buffer_length,local_threads[0]);
        global_threads[1]=1;

        kern->set_arg( blk_info );
        kern->set_arg( alpha );
        kern->set_arg( mog );
        kern->set_arg( num_obs );
        kern->set_arg( aux0 );
        kern->set_arg( aux1 );
        kern->set_arg( aux2 );
        kern->set_arg( aux3 );
        kern->set_arg( upAlphaMem.ptr() );
        kern->set_arg( cl_output.ptr() );
      }
      //execute kernel
      kern->execute(queue, 2, local_threads, global_threads);
      int status = clFinish(queue);
      if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)) )
        return false;
      gpu_time += kern->exec_time();

      //clear render kernel args so it can reset em on next execution
      kern->clear_args();

      //read info back to host memory
      blk->read_to_buffer(queue);
      alpha->read_to_buffer(queue);
      mog->read_to_buffer(queue);
      num_obs->read_to_buffer(queue);
      aux0->read_to_buffer(queue);
      aux1->read_to_buffer(queue);
      aux2->read_to_buffer(queue);
      aux3->read_to_buffer(queue);

      //read image out to buffer (from gpu)
      in_image->read_to_buffer(queue);
      vis_image->read_to_buffer(queue);
      pre_image->read_to_buffer(queue);
      cl_output->read_to_buffer(queue);
      clFinish(queue);
    }
  }

  delete [] vis_buff;
  delete [] pre_buff;
  delete [] norm_buff;
  delete [] input_buff;
  delete [] ray_origins;
  delete [] ray_directions;
  opencl_cache->unref_mem(in_image.ptr());
  opencl_cache->unref_mem(vis_image.ptr());
  opencl_cache->unref_mem(pre_image.ptr());
  opencl_cache->unref_mem(norm_image.ptr());
  opencl_cache->unref_mem(ray_o_buff.ptr());
  opencl_cache->unref_mem(ray_d_buff.ptr());
    opencl_cache->unref_mem(tnearfar_mem_ptr.ptr());
  //reset local threads to 8/8 (default);
  local_threads[0] = 8;
  local_threads[1] = 8;
  global_threads[0]=cl_ni;
  global_threads[1]=cl_nj;

  std::cout<<"Gpu time "<<gpu_time<<" transfer time "<<transfer_time<<std::endl;
  clReleaseCommandQueue(queue);
  return true;
}


//Returns vector of color update kernels (and caches them per device
std::vector<bocl_kernel*>& boxm2_ocl_update_color::get_kernels(const bocl_device_sptr& device, const std::string& opts)
{
  // compile kernels if not already compiled
  std::string identifier = device->device_identifier() + opts;
  if (kernels_.find(identifier) != kernels_.end())
    return kernels_[identifier];

  //otherwise compile the kernels
  std::cout<<"=== boxm2_ocl_update_color::compiling kernels on device "<<identifier<<"==="<<std::endl;

  std::vector<std::string> src_paths;
  std::string source_dir = boxm2_ocl_util::ocl_src_root();
  src_paths.push_back(source_dir + "scene_info.cl");
  src_paths.push_back(source_dir + "pixel_conversion.cl");
  src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(source_dir + "backproject.cl");
  src_paths.push_back(source_dir + "statistics_library_functions.cl");
  src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
  src_paths.push_back(source_dir + "bit/update_rgb_kernels.cl");
  std::vector<std::string> non_ray_src = std::vector<std::string>(src_paths);
  src_paths.push_back(source_dir + "update_rgb_functors.cl");
  src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

  //compilation options
  std::string options = " -D INTENSITY ";
  options += " -D YUV -D DETERMINISTIC -D MOG_TYPE_8 ";
  options += opts;

  //populate vector of kernels
  std::vector<bocl_kernel*> vec_kernels;

  //seg len pass
  auto* seg_len = new bocl_kernel();
  std::string seg_opts = options + " -D SEGLEN -D STEP_CELL=step_cell_seglen(aux_args,data_ptr,llid,d) ";
  seg_len->create_kernel(&device->context(), device->device_id(), src_paths, "seg_len_main", seg_opts, "update_color::seg_len");
  vec_kernels.push_back(seg_len);

  //create  compress rgb pass
  auto* comp = new bocl_kernel();
  std::string comp_opts = options + " -D COMPRESS_RGB ";
  comp->create_kernel(&device->context(), device->device_id(), non_ray_src, "compress_rgb", comp_opts, "update_color::compress_rgb");
  vec_kernels.push_back(comp);

  auto* pre_inf = new bocl_kernel();
  std::string pre_opts = options + " -D PREINF -D STEP_CELL=step_cell_preinf(aux_args,data_ptr,llid,d) ";
  pre_inf->create_kernel(&device->context(), device->device_id(), src_paths, "pre_inf_main", pre_opts, "update_color::pre_inf");
  vec_kernels.push_back(pre_inf);

  //may need DIFF LIST OF SOURCES FOR THIS GUY
  auto* proc_img = new bocl_kernel();
  proc_img->create_kernel(&device->context(), device->device_id(), non_ray_src, "proc_norm_image", options, "update_color::proc_norm_image");
  vec_kernels.push_back(proc_img);

  //push back cast_ray_bit
  auto* bayes_main = new bocl_kernel();
  std::string bayes_opt = options + " -D BAYES -D STEP_CELL=step_cell_bayes(aux_args,data_ptr,llid,d) ";
  bayes_main->create_kernel(&device->context(), device->device_id(), src_paths, "bayes_main", bayes_opt, "update_color::bayes_main");
  vec_kernels.push_back(bayes_main);

  //may need DIFF LIST OF SOURCES FOR THSI GUY TOO
  auto* update = new bocl_kernel();
  update->create_kernel(&device->context(), device->device_id(), non_ray_src, "update_bit_scene_main", options, "update_color::update_main");
  vec_kernels.push_back(update);

  //store and return
  kernels_[identifier] = vec_kernels;
  return kernels_[identifier];
}
