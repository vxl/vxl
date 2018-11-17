// This is brl/bseg/boxm2/ocl/algo/boxm2_ocl_view_based_update.cxx
#include <fstream>
#include <iostream>
#include <algorithm>
#include "boxm2_ocl_update_view_dep_app_color.h"
//:
// \file

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
#include <vil/vil_convert.h>

//directory utility
#include <vul/vul_timer.h>
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

//: Map of kernels should persist between process executions
std::map<std::string,std::vector<bocl_kernel*> > boxm2_ocl_update_view_dep_app_color::kernels_;

//Main public method, updates color model
bool boxm2_ocl_update_view_dep_app_color::update(const boxm2_scene_sptr&         scene,
                              bocl_device_sptr         device,
                              const boxm2_opencl_cache_sptr&  opencl_cache,
                              vpgl_camera_double_sptr  cam,
                              const vil_image_view_base_sptr& img,
                              const std::string&               ident,
                              const vil_image_view_base_sptr& mask_sptr,
                              bool                     update_alpha,
                              float                    mog_var,
                              std::size_t               startI,
                              std::size_t               startJ)
{
  enum {
      UPDATE_SEGLEN = 0,
      UPDATE_COMPRESS = 1,
      UPDATE_PREINF = 2,
      UPDATE_PROC   = 3,
      UPDATE_BAYES  = 4,
      UPDATE_CELL   = 5
  };
  float transfer_time=0.0f;
  float gpu_time=0.0f;
  std::size_t local_threads[2]={8,8};
  std::size_t global_threads[2]={8,8};

  //catch a "null" mask
  bool use_mask = false;
  if (mask_sptr) {
    if ( mask_sptr->ni() == img->ni() && mask_sptr->nj() == img->nj() ) {
      std::cout<<"Update using mask."<<std::endl;
      use_mask = true;
    }
    else {
      std::cout << "WARNING: received mask, but different size than image - ignoring." << std::endl;
    }
  }
  vil_image_view<float> mask;
  if (use_mask) {
    if (auto* mask_char = dynamic_cast<vil_image_view<unsigned char> *>(mask_sptr.ptr())) {
      mask.set_size(mask_char->ni(), mask_char->nj());
      vil_convert_stretch_range_limited(*mask_char, mask, (unsigned char)0, (unsigned char)255, 0.0f, 1.0f);
    }
    else if (auto* mask_flt = dynamic_cast<vil_image_view<float>*>(mask_sptr.ptr())) {
      mask = *mask_flt;
    }
    else {
      std::cout<<"boxm2_ocl_update_view_dep_app_color: mask map is not an unsigned char or float image" << std::endl;
      return false;
    }
  }
  else {
    mask.set_size(img->ni(), img->nj());
    mask.fill(1.0f);
  }

  //cache size sanity check
  std::size_t binCache = opencl_cache.ptr()->bytes_in_cache();
  std::cout<<"Update MBs in cache: "<<binCache/(1024.0*1024.0)<<std::endl;

  //make correct data types are here
  std::string data_type, num_obs_type,options;
  int appTypeSize;
  if (!validate_appearances(scene, ident, data_type, appTypeSize, num_obs_type, options)) {
    return false;
  }

  // create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue( device->context(),
                                                 *(device->device_id()),
                                                 CL_QUEUE_PROFILING_ENABLE,
                                                 &status);
  if (status!=0)
    return false;

  // compile the kernel if not already compiled
  std::vector<bocl_kernel*>& kernels = get_kernels(device, options);

  //grab input image, establish cl_ni, cl_nj (so global size is divisible by local size)
  vil_image_view_base_sptr byte_img = boxm2_util::prepare_input_image(img, false);
  if ( byte_img->pixel_format() != VIL_PIXEL_FORMAT_RGBA_BYTE ) {
    std::cout<<"boxm2_ocl_update_color_process::using a non RGBA image!!"<<std::endl;
    return false;
  }
  auto* img_view = static_cast<vil_image_view<vil_rgba<vxl_byte> >* >(byte_img.ptr());
  auto cl_ni=(unsigned)RoundUp(img_view->ni(),(int)local_threads[0]);
  auto cl_nj=(unsigned)RoundUp(img_view->nj(),(int)local_threads[1]);
  global_threads[0]=cl_ni;
  global_threads[1]=cl_nj;

  //set generic cam
  auto* ray_origins    = new cl_float[4*cl_ni*cl_nj];
  auto* ray_directions = new cl_float[4*cl_ni*cl_nj];
  bocl_mem_sptr ray_o_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_origins, "ray_origins buffer");
  bocl_mem_sptr ray_d_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_directions, "ray_directions buffer");
  boxm2_ocl_camera_converter::compute_ray_image( device, queue, cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff, startI, startJ);

  //Visibility, Preinf, Norm, and input image buffers
  auto* vis_buff = new float[cl_ni*cl_nj];
  auto* pre_buff = new float[cl_ni*cl_nj];
  auto* norm_buff = new float[cl_ni*cl_nj];
  std::fill(vis_buff, vis_buff+cl_ni*cl_nj, 1.0f);
  std::fill(pre_buff, pre_buff+cl_ni*cl_nj, 0.0f);
  std::fill(norm_buff, norm_buff+cl_ni*cl_nj, 0.0f);

  //copy input vals into image
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
        input_buff[numFloats*count + 3] = (float) 1.0f; //mask(i,j);
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
  int img_dim_buff[4] = {0, 0, (int)img_view->ni(), (int) img_view->nj() };
  bocl_mem_sptr img_dim=new bocl_mem(device->context(), img_dim_buff, sizeof(int)*4, "image dims");
  img_dim->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // Output Array
  float output_arr[100];
  for (float & i : output_arr) i = 0.0f;
  bocl_mem_sptr  cl_output=new bocl_mem(device->context(), output_arr, sizeof(float)*100, "output buffer");
  cl_output->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // bit lookup buffer
  cl_uchar lookup_arr[256];
  boxm2_ocl_util::set_bit_lookup(lookup_arr);
  bocl_mem_sptr lookup=new bocl_mem(device->context(), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
  lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  // app density used for proc_norm_image
  //Set app_buffer.x = 1.0, if you want uniform background model
  //Set app_buffer.x = 0, app_buffer.y and app_buffer.z to the mean and sigma of the gaussian for the background model
  float app_buffer[4]={1.0,0.0,0.0,0.0};
  bocl_mem_sptr app_density = new bocl_mem(device->context(), app_buffer, sizeof(cl_float4), "app density buffer");
  app_density->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // set arguments
  std::vector<boxm2_block_id> vis_order = scene->get_vis_blocks(cam);
  std::vector<boxm2_block_id>::iterator id;
  for (unsigned int i=0; i<kernels.size(); ++i)
  {
    if ( i == UPDATE_PROC ) {
      bocl_kernel * proc_kern=kernels[i];

      proc_kern->set_arg( norm_image.ptr() );
      proc_kern->set_arg( vis_image.ptr() );
      proc_kern->set_arg( pre_image.ptr());
      proc_kern->set_arg( img_dim.ptr() );
      proc_kern->set_arg( in_image.ptr() );
      proc_kern->set_arg( app_density.ptr() );

      //execute kernel
      proc_kern->execute( queue, 2, local_threads, global_threads);
      int status = clFinish(queue);
      if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)))
        return false;
      proc_kern->clear_args();
      norm_image->read_to_buffer(queue);

      continue;
    }

    //set masked values
    vis_image->read_to_buffer(queue);
#if 1
    if (use_mask)
    {
      int count = 0;
      for (unsigned int j=0;j<cl_nj;++j) {
        for (unsigned int i=0;i<cl_ni;++i) {
          if ( i<mask.ni() && j<mask.nj() ) {
            if (mask(i,j) == 0) {
              // input_buff[count+0] = -1.0f;
              // input_buff[count+1] = -1.0f;
              // input_buff[count+2] = -1.0f;
              // input_buff[count+3] = -1.0f;
              input_buff[numFloats*count+0]=-1.0f;
              vis_buff[count] = 0.0f;
            }
          }
          ++count;
        }
      }
      in_image->write_to_buffer(queue);
      vis_image->write_to_buffer(queue);
      clFinish(queue);
    }
#endif

    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
      //choose correct render kernel
      boxm2_block_metadata mdata = scene->get_block_metadata(*id);
      bocl_kernel* kern = kernels[i];

      //write the image values to the buffer
      vul_timer transfer;
      bocl_mem* blk       = opencl_cache->get_block(scene,*id);
      bocl_mem* blk_info  = opencl_cache->loaded_block_info();
      bocl_mem* alpha     = opencl_cache->get_data<BOXM2_ALPHA>(scene,*id,0,false);
      auto* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
      int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
      info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);
      blk_info->write_to_buffer((queue));


      // data type string may contain an identifier so determine the buffer size
      int nobsTypeSize = (int)boxm2_data_info::datasize(num_obs_type);
      // alpha->num_bytes()/alphaTypeSize*appTypeSize
      bocl_mem* mog       = opencl_cache->get_data(scene,*id,data_type,info_buffer->data_buffer_length*appTypeSize,false);    //info_buffer->data_buffer_length*boxm2_data_info::datasize(data_type));
      // alpha->num_bytes()/alphaTypeSize*nobsTypeSize
      bocl_mem* num_obs   = opencl_cache->get_data(scene,*id,num_obs_type,info_buffer->data_buffer_length*nobsTypeSize,false);//,info_buffer->data_buffer_length*boxm2_data_info::datasize(num_obs_type));

      //grab an appropriately sized AUX data buffer
      int auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX0>::prefix());
      bocl_mem *aux0   = opencl_cache->get_data<BOXM2_AUX0>(scene,*id, info_buffer->data_buffer_length*auxTypeSize);

      auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX1>::prefix());
      bocl_mem *aux1   = opencl_cache->get_data<BOXM2_AUX1>(scene,*id, info_buffer->data_buffer_length*auxTypeSize);

      auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX2>::prefix());
      bocl_mem *aux2   = opencl_cache->get_data<BOXM2_AUX2>(scene,*id, info_buffer->data_buffer_length*auxTypeSize);

      auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX3>::prefix());
      bocl_mem *aux3   = opencl_cache->get_data<BOXM2_AUX3>(scene,*id, info_buffer->data_buffer_length*auxTypeSize);

      auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX>::prefix());
      bocl_mem *aux   = opencl_cache->get_data<BOXM2_AUX>(scene,*id, info_buffer->data_buffer_length*auxTypeSize);

      transfer_time += (float) transfer.all();
      if (i==UPDATE_SEGLEN)
      {

        local_threads[0] = 8; local_threads[1] = 8;
        global_threads[0]=cl_ni; global_threads[1]=cl_nj;
        aux0->zero_gpu_buffer(queue);
        aux1->zero_gpu_buffer(queue);
        kern->set_arg( blk_info );
        kern->set_arg( blk );
        kern->set_arg( alpha );
        kern->set_arg( aux0 );
        kern->set_arg( aux1 );
        kern->set_arg( aux2 );
        kern->set_arg( aux3 );
        kern->set_arg( lookup.ptr() );

        // kern->set_arg( persp_cam.ptr() );
        kern->set_arg( ray_o_buff.ptr() );
        kern->set_arg( ray_d_buff.ptr() );

        kern->set_arg( img_dim.ptr() );
        kern->set_arg( in_image.ptr() );
        kern->set_arg( cl_output.ptr() );
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar16) );//local tree,
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar4) ); //ray bundle,
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_int) );    //cell pointers,
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_float4) ); //cached aux,
        kern->set_local_arg( local_threads[0]*local_threads[1]*10*sizeof(cl_uchar) ); //cumsum buffer, imindex buffer

        //execute kernel
        kern->execute(queue, 2, local_threads, global_threads);
        int status = clFinish(queue);
        if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)))
          return false;
        gpu_time += kern->exec_time();

        //clear render kernel args so it can reset em on next execution
        kern->clear_args();


      }        else if (i==UPDATE_COMPRESS)
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

        kern->execute(queue, 2, local_threads, global_threads);
        int status = clFinish(queue);
        if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)))
          return false;
        gpu_time += kern->exec_time();
        kern->clear_args();

      }
      else if (i==UPDATE_PREINF)
      {
        aux->zero_gpu_buffer(queue);
        local_threads[0] = 8; local_threads[1] = 8;
        global_threads[0]=cl_ni; global_threads[1]=cl_nj;

        kern->set_arg( blk_info );
        kern->set_arg( blk );
        kern->set_arg( alpha );
        kern->set_arg( mog );
        kern->set_arg( num_obs );
        kern->set_arg( aux );
        kern->set_arg( aux0 );
        kern->set_arg( aux1 );
        kern->set_arg( lookup.ptr() );
        kern->set_arg( ray_o_buff.ptr() );
        kern->set_arg( ray_d_buff.ptr() );
        //kern->set_arg(app_model_view_dirs_lookup.ptr());
        kern->set_arg( img_dim.ptr() );
        kern->set_arg( vis_image.ptr() );
        kern->set_arg( pre_image.ptr() );
        kern->set_arg( cl_output.ptr() );
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar16) );//local tree,
        kern->set_local_arg( local_threads[0]*local_threads[1]*10*sizeof(cl_uchar) ); //cumsum buffer, imindex buffer
        //execute kernel
        kern->execute(queue, 2, local_threads, global_threads);
        int status = clFinish(queue);
        if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)))
          return false;
        gpu_time += kern->exec_time();
        //clear render kernel args so it can reset em on next execution
        kern->clear_args();

      }
      else if (i==UPDATE_BAYES)
      {
        auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX2>::prefix());
        bocl_mem *aux2   = opencl_cache->get_data<BOXM2_AUX2>(scene,*id, info_buffer->data_buffer_length*auxTypeSize);
        aux2->zero_gpu_buffer(queue);

        auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX3>::prefix());
        bocl_mem *aux3   = opencl_cache->get_data<BOXM2_AUX3>(scene,*id, info_buffer->data_buffer_length*auxTypeSize);
        aux3->zero_gpu_buffer(queue);

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
        //kern->set_arg(app_model_view_dirs_lookup.ptr());

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
                //execute kernel
        kern->execute(queue, 2, local_threads, global_threads);
        int status = clFinish(queue);
        if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)))
          return false;
        gpu_time += kern->exec_time();

        //clear render kernel args so it can reset em on next execution
        kern->clear_args();
        aux2->read_to_buffer(queue);
        aux3->read_to_buffer(queue);
      }
      else if (i==UPDATE_CELL)
      {
        auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX2>::prefix());
        bocl_mem *aux2   = opencl_cache->get_data<BOXM2_AUX2>(scene,*id, info_buffer->data_buffer_length*auxTypeSize);

        auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX3>::prefix());
        bocl_mem *aux3   = opencl_cache->get_data<BOXM2_AUX3>(scene,*id, info_buffer->data_buffer_length*auxTypeSize);

        // update_alpha boolean buffer
        cl_int up_alpha[1];
        up_alpha[0] = update_alpha ? 1 : 0;
        bocl_mem_sptr up_alpha_mem = new bocl_mem(device->context(), up_alpha, sizeof(up_alpha), "update alpha bool buffer");
        up_alpha_mem->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

        //mog variance, if 0.0f or less, then var will be learned
        bocl_mem_sptr mog_var_mem = new bocl_mem(device->context(), &mog_var, sizeof(mog_var), "update gauss variance");
        mog_var_mem->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

        local_threads[0] = 16;
        local_threads[1] = 1 ;
        global_threads[0]=RoundUp(info_buffer->data_buffer_length,local_threads[0]);
        global_threads[1]=1;

        kern->set_arg( blk_info );
        kern->set_arg( alpha );
        kern->set_arg( mog );
        kern->set_arg( num_obs );
        kern->set_arg( aux );
        kern->set_arg( aux0 );
        kern->set_arg( aux1 );
        kern->set_arg( aux2 );
        kern->set_arg( aux3 );
        //kern->set_arg(app_model_view_dirs_lookup.ptr());
        kern->set_arg( up_alpha_mem.ptr() );
        kern->set_arg( mog_var_mem.ptr() );
        kern->set_arg( cl_output.ptr() );

        //execute kernel
        kern->execute(queue, 2, local_threads, global_threads);
        int status = clFinish(queue);
        if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)))
          return false;
        gpu_time += kern->exec_time();

        //clear render kernel args so it can reset em on next execution
        kern->clear_args();

        //write info to disk
        alpha->read_to_buffer(queue);
        mog->read_to_buffer(queue);
        num_obs->read_to_buffer(queue);
        status = clFinish(queue);
        if (!check_val(status, MEM_FAILURE, "READ MODEL BUFFERS FAILED: " + error_to_string(status)))
          return false;
      }

      //read image out to buffer (from gpu)
      in_image->read_to_buffer(queue);
      vis_image->read_to_buffer(queue);
      pre_image->read_to_buffer(queue);
      cl_output->read_to_buffer(queue);
      status = clFinish(queue);
      if (!check_val(status, MEM_FAILURE, "READ IMAGE BUFFERS FAILED: " + error_to_string(status)))
        return false;
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
std::vector<bocl_kernel*>& boxm2_ocl_update_view_dep_app_color::get_kernels(const bocl_device_sptr& device, const std::string& opts)
{
  // compile kernels if not already compiled
  std::string identifier = device->device_identifier() + opts;
  if (kernels_.find(identifier) != kernels_.end())
    return kernels_[identifier];

  bool UV_only = false;
  if (opts ==" -D MOG_VIEW_DEP_UV " ){
          UV_only = true;
          std::cout<<"updating UV only"<<std::endl;
  }


  //otherwise compile the kernels
  std::cout<<"=== boxm2_ocl_view_based_update::compiling kernels on device "<<identifier<<"==="<<std::endl;

  std::vector<std::string> src_paths;
  std::string source_dir = boxm2_ocl_util::ocl_src_root();
  src_paths.push_back(source_dir + "scene_info.cl");
  src_paths.push_back(source_dir + "pixel_conversion.cl");
  src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(source_dir + "backproject.cl");
  src_paths.push_back(source_dir + "statistics_library_functions.cl");
  src_paths.push_back(source_dir + "view_dep_app_common_functions.cl");
  src_paths.push_back(source_dir + "view_dep_app_color_helper_functions.cl");
  src_paths.push_back(source_dir + "bit/update_view_dep_app_color_kernels.cl");
  std::vector<std::string> non_ray_src = std::vector<std::string>(src_paths);

  //push ray trace files
  src_paths.push_back(source_dir + "view_dep_app_update_color_functors.cl");
  src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

  //compilation options
  std::string options = opts;
  if (UV_only){
    options += " -D YUV ";
  }

  //populate vector of kernels
  std::vector<bocl_kernel*> vec_kernels;

  //seg len pass
  auto* seg_len = new bocl_kernel();
  std::string seg_opts = options + " -D SEGLEN -D STEP_CELL=step_cell_seglen(aux_args,data_ptr,llid,d)";
  seg_len->create_kernel(&device->context(), device->device_id(), src_paths, "seg_len_main", seg_opts, "update::seg_len");
  vec_kernels.push_back(seg_len);

  auto* comp = new bocl_kernel();
  std::string comp_opts = options + " -D COMPRESS_RGB ";
  comp->create_kernel(&device->context(), device->device_id(), non_ray_src, "compress_rgb", comp_opts, "update_color::compress_rgb");
  vec_kernels.push_back(comp);

  auto* pre_inf = new bocl_kernel();
  std::string pre_opts = options + " -D PREINF -D STEP_CELL=step_cell_preinf(aux_args,data_ptr,llid,d)";
  pre_inf->create_kernel(&device->context(), device->device_id(), src_paths, "pre_inf_main", pre_opts, "update_color::pre_inf");
  vec_kernels.push_back(pre_inf);

  //may need DIFF LIST OF SOURCES FOR THIS GUY
  auto* proc_img = new bocl_kernel();
  std::string proc_opts = options + " -D PROC_NORM ";
  proc_img->create_kernel(&device->context(), device->device_id(), non_ray_src, "proc_norm_image", proc_opts, "update_color::proc_norm_image");
  vec_kernels.push_back(proc_img);

  //push back cast_ray_bit
  auto* bayes_main = new bocl_kernel();
  std::string bayes_opt = options + " -D BAYES -D STEP_CELL=step_cell_bayes(aux_args,data_ptr,llid,d)";
  bayes_main->create_kernel(&device->context(), device->device_id(), src_paths, "bayes_main", bayes_opt, "update_color::bayes_main");
  vec_kernels.push_back(bayes_main);

  //may need DIFF LIST OF SOURCES FOR THSI GUY TOO
  auto* update = new bocl_kernel();
  std::string update_opts = options + " -D UPDATE_BIT_SCENE_MAIN";
  update->create_kernel(&device->context(), device->device_id(), non_ray_src, "update_bit_scene_main", update_opts, "update::update_main");
  vec_kernels.push_back(update);

  //store and return
  kernels_[identifier] = vec_kernels;
  return kernels_[identifier];
}


//makes sure appearance types correspond correctly
bool boxm2_ocl_update_view_dep_app_color::validate_appearances(const boxm2_scene_sptr& scene,
                                                               std::string const& ident,
                                                               std::string& data_type,
                                                               int& appTypeSize,
                                                               std::string& num_obs_type,
                                                               std::string& options)
{
  std::vector<std::string> apps = scene->appearances();
  bool foundDataType = false, foundNumObsType = false;
  for (const auto & app : apps) {
    if ( app == boxm2_data_traits<BOXM2_GAUSS_RGB_VIEW>::prefix(ident)&& !foundDataType)
    {
      data_type = app;
      foundDataType = true;
      options=" -D MOG_VIEW_DEP_COLOR";
      appTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_GAUSS_RGB_VIEW>::prefix());
    }
    else if (app == boxm2_data_traits<BOXM2_GAUSS_UV_VIEW>::prefix(ident)&& !foundDataType){
      data_type = app;
      foundDataType = true;
      options=" -D MOG_VIEW_DEP_UV ";
      appTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_MOG6_VIEW>::prefix());

    }
    else if ( app == boxm2_data_traits<BOXM2_NUM_OBS_VIEW>::prefix(ident) )
    {
      num_obs_type = app;
      foundNumObsType = true;
    }
  }
  if (!foundDataType) {
    std::cout<<"BOXM2_OPENCL_VIEW_BASED_UPDATE_COLOR_PROCESS ERROR: scene doesn't have BOXM2_GAUSS_RGB_VIEW or BOXM2_GAUSS_UV_VIEW data type with ident = <" << ident << ">" << std::endl;
    return false;
  }
  if (!foundNumObsType) {
    std::cout<<"BOXM2_OPENCL_VIEW_BASED_UPDATE_PROCESS ERROR: scene doesn't have BOXM2_NUM_OBS_VIEW type with ident = <" << ident << ">" <<std::endl;
    return false;
  }
  return true;
}
