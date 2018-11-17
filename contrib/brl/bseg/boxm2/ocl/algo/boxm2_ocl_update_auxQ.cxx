// This is brl/bseg/boxm2/ocl/algo/boxm2_ocl_update_auxQ.cxx
#include <fstream>
#include <iostream>
#include <algorithm>
#include "boxm2_ocl_update_auxQ.h"
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

//directory utility
#include <vul/vul_timer.h>
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

//: Map of kernels should persist between process executions
std::map<std::string,std::vector<bocl_kernel*> > boxm2_ocl_update_auxQ::kernels_;

//Main public method, updates color model
bool boxm2_ocl_update_auxQ::update_auxQ(const boxm2_scene_sptr&         scene,
                              bocl_device_sptr         device,
                              const boxm2_opencl_cache_sptr&  opencl_cache,
                              vpgl_camera_double_sptr  cam,
                              const vil_image_view_base_sptr& img,
                              const std::string&               app_ident,
                              std::string               view_ident,
                              float resnearfactor ,
                              float resfarfactor )
{
  enum {
    UPDATE_SEGLEN = 0,
    UPDATE_PREINF = 1,
    UPDATE_PROC   = 2,
    UPDATE_BAYES  = 3,
    CONVERT_AUX_INT_FLOAT = 4,
  };
  float transfer_time=0.0f;
  float gpu_time=0.0f;
  std::size_t local_threads[2]={8,8};
  std::size_t global_threads[2]={8,8};
  view_ident = view_ident +"_curr";
  //cache size sanity check
  std::size_t binCache = opencl_cache.ptr()->bytes_in_cache();
  std::cout<<"Update MBs in cache: "<<binCache/(1024.0*1024.0)<<std::endl;

  //make correct data types are here
  std::string data_type, num_obs_type,options;

  int appTypeSize;
  bool isRGB = false;
  if (!validate_appearances(scene, data_type, appTypeSize, num_obs_type, options, isRGB))
      return false;
  if (app_ident.size() > 0) {
      data_type += "_" + app_ident;
      num_obs_type += "_" + app_ident;
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
  vil_image_view_base_sptr float_img = boxm2_util::prepare_input_image(img, true);
  auto* img_view = static_cast<vil_image_view<float>* >(float_img.ptr());
  auto cl_ni=(unsigned)RoundUp(img_view->ni(),(int)local_threads[0]);
  auto cl_nj=(unsigned)RoundUp(img_view->nj(),(int)local_threads[1]);
  global_threads[0]=cl_ni;
  global_threads[1]=cl_nj;
  //set generic cam
  auto* ray_origins    = new cl_float[4*cl_ni*cl_nj];
  auto* ray_directions = new cl_float[4*cl_ni*cl_nj];
  bocl_mem_sptr ray_o_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_origins, "ray_origins buffer");
  bocl_mem_sptr ray_d_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_directions, "ray_directions buffer");
  boxm2_ocl_camera_converter::compute_ray_image( device, queue, cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff);

  float tnearfar[2] = { 0.0f, 1000000} ;

  if(cam->type_name() == "vpgl_perspective_camera")
  {

      float f  = ((vpgl_perspective_camera<double> *)cam.ptr())->get_calibration().focal_length()*((vpgl_perspective_camera<double> *)cam.ptr())->get_calibration().x_scale();
      std::cout<<"Focal Length " << f<<std::endl;
      tnearfar[0] = f* scene->finest_resolution()/resnearfactor ;
      tnearfar[1] = f* scene->finest_resolution()*resfarfactor ;
      std::cout<<"Near and Far Clipping planes "<<tnearfar[0]<<" "<<tnearfar[1]<<std::endl;
  }
  bocl_mem_sptr tnearfar_mem_ptr = opencl_cache->alloc_mem(2*sizeof(float), tnearfar, "tnearfar  buffer");
  tnearfar_mem_ptr->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  //Visibility, Preinf, Norm, and input image buffers
  auto* vis_buff = new float[cl_ni*cl_nj];
  auto* pre_buff = new float[cl_ni*cl_nj];
  auto* norm_buff = new float[cl_ni*cl_nj];
  auto* input_buff=new float[cl_ni*cl_nj];
  for (unsigned i=0;i<cl_ni*cl_nj;i++)
  {
    vis_buff[i]=1.0f;
    pre_buff[i]=0.0f;
    norm_buff[i]=0.0f;
  }
  //copy input vals into image
  int count=0;
  for (unsigned int j=0;j<cl_nj;++j) {
    for (unsigned int i=0;i<cl_ni;++i) {
      input_buff[count] = 0.0f;
      if ( i<img_view->ni() && j< img_view->nj() )
        input_buff[count] = (*img_view)(i,j);
      ++count;
    }
  }
  //bocl_mem_sptr in_image=new bocl_mem(device->context(),input_buff,cl_ni*cl_nj*sizeof(float),"input image buffer");
  bocl_mem_sptr in_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), input_buff, "input image buffer");
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
  int img_dim_buff[4];
  img_dim_buff[0] = 0;
  img_dim_buff[1] = 0;
  img_dim_buff[2] = img_view->ni();
  img_dim_buff[3] = img_view->nj();

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
  float app_buffer[4]={1.0,0.0,0.0,0.0};
  bocl_mem_sptr app_density = new bocl_mem(device->context(), app_buffer, sizeof(cl_float4), "app density buffer");
  app_density->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // set arguments
  std::vector<boxm2_block_id> vis_order;
  //if(cam->type_name() == "vpgl_perspective_camera")
  //    vis_order= scene->get_vis_blocks_opt((vpgl_perspective_camera<double>*)cam.ptr(),img_view->ni(),img_view->nj());
  //else
      vis_order= scene->get_vis_blocks(cam);
  std::vector<boxm2_block_id>::iterator id;
  for (unsigned int i=0; i<kernels.size(); ++i)
  {
    if ( i == UPDATE_PROC ) {
      bocl_kernel * proc_kern=kernels[i];

      proc_kern->set_arg( norm_image.ptr() );
      proc_kern->set_arg( vis_image.ptr() );
      proc_kern->set_arg( pre_image.ptr());
      proc_kern->set_arg( img_dim.ptr() );

      //execute kernel
      proc_kern->execute( queue, 2, local_threads, global_threads);
      int status = clFinish(queue);
      if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)))
        return false;
      proc_kern->clear_args();
      norm_image->read_to_buffer(queue);
      vil_image_view<float> norm(img_view->ni(),img_view->nj());

      int count=0;
      for (unsigned int j=0;j<cl_nj;++j) {
          for (unsigned int i=0;i<cl_ni;++i) {
              if ( i<img_view->ni() && j< img_view->nj() )
                  norm(i,j) = norm_buff[count] ;
              ++count;
          }
      }
      continue;
    }

    //set masked values
    vis_image->read_to_buffer(queue);
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

      int nobsTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_NUM_OBS>::prefix());
      int appTypeSize =  (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
      // data type string may contain an identifier so determine the buffer size
      bocl_mem* mog       = opencl_cache->get_data(scene,*id,data_type,alpha->num_bytes()/alphaTypeSize*appTypeSize,false);    //info_buffer->data_buffer_length*boxm2_data_info::datasize(data_type));
      bocl_mem* num_obs   = opencl_cache->get_data(scene,*id,num_obs_type,alpha->num_bytes()/alphaTypeSize*nobsTypeSize,false);//,info_buffer->data_buffer_length*boxm2_data_info::datasize(num_obs_type));

      //grab an appropriately sized AUX data buffer
      int auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX0>::prefix());
      bocl_mem *aux0   = opencl_cache->get_data(scene,*id, boxm2_data_traits<BOXM2_AUX0>::prefix(view_ident),info_buffer->data_buffer_length*auxTypeSize,false);
      auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX1>::prefix());
      bocl_mem *aux1   = opencl_cache->get_data(scene,*id, boxm2_data_traits<BOXM2_AUX1>::prefix(view_ident),info_buffer->data_buffer_length*auxTypeSize,false);
      auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX2>::prefix());
      bocl_mem *aux2   = opencl_cache->get_data(scene,*id, boxm2_data_traits<BOXM2_AUX2>::prefix(view_ident),info_buffer->data_buffer_length*auxTypeSize,false);

      auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX3>::prefix());
      bocl_mem *aux3   = opencl_cache->get_data(scene,*id, boxm2_data_traits<BOXM2_AUX3>::prefix(view_ident),info_buffer->data_buffer_length*auxTypeSize,false);

      transfer_time += (float) transfer.all();
      if (i==UPDATE_SEGLEN)
      {
        aux0->zero_gpu_buffer(queue);
        aux1->zero_gpu_buffer(queue);
        kern->set_arg( blk_info );
        kern->set_arg( blk );
        kern->set_arg( alpha );
        kern->set_arg( aux0 );
        kern->set_arg( aux1 );
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
        //execute kernel
        kern->execute(queue, 2, local_threads, global_threads);
        int status = clFinish(queue);
        if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)))
          return false;
        gpu_time += kern->exec_time();
        //clear render kernel args so it can reset em on next execution
        kern->clear_args();
        aux0->read_to_buffer(queue);
        aux1->read_to_buffer(queue);
      }
      else if (i==UPDATE_PREINF)
      {

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
        //execute kernel
        kern->execute(queue, 2, local_threads, global_threads);
        int status = clFinish(queue);
        if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)))
          return false;
        gpu_time += kern->exec_time();
        //clear render kernel args so it can reset em on next execution
        kern->clear_args();
        //write info to disk
      }
      else if (i==UPDATE_BAYES)
      {
      aux2->zero_gpu_buffer(queue);
      aux3->zero_gpu_buffer(queue);
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
                //execute kernel
        kern->execute(queue, 2, local_threads, global_threads);
        int status = clFinish(queue);
        if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)))
          return false;
        gpu_time += kern->exec_time();
        //clear render kernel args so it can reset em on next execution
        kern->clear_args();


      }
      else if (i==CONVERT_AUX_INT_FLOAT)
      {
          std::size_t ltr[2];
          ltr[0] = 64;
          ltr[1] = 1;
          std::size_t gt[2];
          gt[0] = RoundUp(info_buffer->data_buffer_length,ltr[0]);
          gt[1] = 1;
          kern->set_arg( blk_info );
          kern->set_arg( aux0 );
          kern->set_arg( aux1 );
          kern->set_arg( aux2 );
          kern->set_arg( aux3 );

          //execute kernel
          kern->execute(queue, 2, ltr, gt);
          int status = clFinish(queue);
          check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status));
          gpu_time += kern->exec_time();

          //clear render kernel args so it can reset em on next execution
          kern->clear_args();
          //write info to disk
          aux0->read_to_buffer(queue);
          aux1->read_to_buffer(queue);
          aux2->read_to_buffer(queue);
          aux3->read_to_buffer(queue);

          opencl_cache->deep_remove_data(scene,*id,boxm2_data_traits<BOXM2_AUX0>::prefix(view_ident),true);
          opencl_cache->deep_remove_data(scene,*id,boxm2_data_traits<BOXM2_AUX1>::prefix(view_ident),true);
          opencl_cache->deep_remove_data(scene,*id,boxm2_data_traits<BOXM2_AUX2>::prefix(view_ident),true);
          opencl_cache->deep_remove_data(scene,*id,boxm2_data_traits<BOXM2_AUX3>::prefix(view_ident),true);

      }
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
  std::cout<<"Gpu time "<<gpu_time<<" transfer time "<<transfer_time<<std::endl;
  clReleaseCommandQueue(queue);
  return true;
}


//Returns vector of color update kernels (and caches them per device
std::vector<bocl_kernel*>& boxm2_ocl_update_auxQ::get_kernels(const bocl_device_sptr& device, const std::string& opts)
{
  // compile kernels if not already compiled
  std::string identifier = device->device_identifier() + opts;
  if (kernels_.find(identifier) != kernels_.end())
    return kernels_[identifier];

  //otherwise compile the kernels
  std::cout<<"=== boxm2_ocl_update_process::compiling kernels on device "<<identifier<<"==="<<std::endl;

  std::vector<std::string> src_paths;
  std::string source_dir = boxm2_ocl_util::ocl_src_root();
  src_paths.push_back(source_dir + "scene_info.cl");
  src_paths.push_back(source_dir + "pixel_conversion.cl");
  src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(source_dir + "backproject.cl");
  src_paths.push_back(source_dir + "atomics_util.cl");
  src_paths.push_back(source_dir + "statistics_library_functions.cl");
  src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
    src_paths.push_back(source_dir + "bit/update_kernels.cl");
  std::vector<std::string> non_ray_src = std::vector<std::string>(src_paths);

    src_paths.push_back(source_dir + "update_functors.cl");
  src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

  //compilation options
  const std::string& options = /*"-D ATOMIC_FLOAT " +*/ opts;

  //populate vector of kernels
  std::vector<bocl_kernel*> vec_kernels;

  //seg len pass
  auto* seg_len = new bocl_kernel();
  std::string seg_opts = options + " -D SEGLEN  -D STEP_CELL=step_cell_seglen(aux_args,data_ptr,llid,d)";
  seg_len->create_kernel(&device->context(), device->device_id(), src_paths, "seg_len_main", seg_opts, "update::seg_len");
  vec_kernels.push_back(seg_len);


  auto* pre_inf = new bocl_kernel();
  std::string pre_opts = options + " -D PREINF  -D STEP_CELL=step_cell_preinf(aux_args,data_ptr,llid,d)";
  pre_inf->create_kernel(&device->context(), device->device_id(), src_paths, "pre_inf_main", pre_opts, "update::pre_inf");
  vec_kernels.push_back(pre_inf);

  //may need DIFF LIST OF SOURCES FOR THIS GUY
  auto* proc_img = new bocl_kernel();
  std::string proc_opts = options + " -D PROC_NORM ";
  proc_img->create_kernel(&device->context(), device->device_id(), non_ray_src, "proc_norm_image", proc_opts, "update::proc_norm_image");
  vec_kernels.push_back(proc_img);

  //push back cast_ray_bit
  auto* bayes_main = new bocl_kernel();
  std::string bayes_opt = options + " -D BAYES  -D STEP_CELL=step_cell_bayes(aux_args,data_ptr,llid,d)";
  bayes_main->create_kernel(&device->context(), device->device_id(), src_paths, "bayes_main", bayes_opt, "update::bayes_main");
  vec_kernels.push_back(bayes_main);

  std::vector<std::string> src_paths_4;
  src_paths_4.push_back(source_dir + "scene_info.cl");
  src_paths_4.push_back(source_dir + "bit/batch_update_kernels.cl");
  //convert aux buffer int values to float (just divide by SEGLENFACTOR
  auto* convert_aux_int_float = new bocl_kernel();
  convert_aux_int_float->create_kernel(&device->context(),device->device_id(), src_paths_4, "convert_aux_int_to_float", opts+" -D CONVERT_AUX ", "batch_update::convert_aux_int_to_float");
  vec_kernels.push_back(convert_aux_int_float);

  //store and return
  kernels_[identifier] = vec_kernels;
  return kernels_[identifier];
}
//makes sure appearance types correspond correctly
bool boxm2_ocl_update_auxQ::validate_appearances(const boxm2_scene_sptr& scene,
                                            std::string& data_type,
                                            int& appTypeSize,
                                            std::string& num_obs_type,
                                            std::string& options,
                                            bool&  /*isRGB*/)
{
  std::vector<std::string> apps = scene->appearances();
  bool foundDataType = false, foundNumObsType = false;
  for (const auto & app : apps) {
    if ( app == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() )
    {
      data_type = app;
      foundDataType = true;
      options=" -D MOG_TYPE_8";
      appTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
    }
    else if ( app == boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix() )
    {
      data_type = app;
      foundDataType = true;
      options=" -D MOG_TYPE_16";
      appTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix());
    }
    else if ( app == boxm2_data_traits<BOXM2_NUM_OBS>::prefix() )
    {
      num_obs_type = app;
      foundNumObsType = true;
    }
  }
  if (!foundDataType) {
    std::cout<<"BOXM2_OPENCL_UPDATE_PROCESS ERROR: scene doesn't have BOXM2_MOG3_GREY or BOXM2_MOG3_GREY_16 data type"<<std::endl;
    return false;
  }
  if (!foundNumObsType) {
    std::cout<<"BOXM2_OPENCL_UPDATE_PROCESS ERROR: scene doesn't have BOXM2_NUM_OBS type"<<std::endl;
    return false;
  }
  return true;
}


std::map<std::string, std::vector<bocl_kernel*> > boxm2_ocl_update_PusingQ::kernels_;

bool boxm2_ocl_update_PusingQ::init_product(boxm2_scene_sptr scene, const boxm2_cache_sptr& cache)
{
    std::vector<boxm2_block_id> vis_order = scene->get_block_ids();
    std::vector<boxm2_block_id>::iterator id;
    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
        boxm2_data_base *  aux3 = cache->get_data_base(scene, *id,boxm2_data_traits<BOXM2_AUX3>::prefix(),0,false);
        auto *   aux3_data = reinterpret_cast<boxm2_data_traits<BOXM2_AUX3>::datatype*> ( aux3->data_buffer());
        std::fill_n(aux3_data,aux3->buffer_length()/boxm2_data_traits<BOXM2_AUX3>::datasize(),1);
        cache->remove_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX3>::prefix());
    }
    return true;
}

bool boxm2_ocl_update_PusingQ::accumulate_product(const boxm2_scene_sptr&         scene,
                                                  const bocl_device_sptr&         device,
                                                  const boxm2_opencl_cache_sptr&  opencl_cache,
                                                  const std::string& identifier)
{
  float transfer_time=0.0f;
  float gpu_time=0.0f;
  //cache size sanity check
  std::size_t binCache = opencl_cache.ptr()->bytes_in_cache();
  std::cout<<"Update MBs in cache: "<<binCache/(1024.0*1024.0)<<std::endl;
  // create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue( device->context(),*(device->device_id()),CL_QUEUE_PROFILING_ENABLE,&status);
  if (status!=0)
      return false;

  // compile the kernel if not already compiled
  std::vector<bocl_kernel*>& kernels = get_kernels(device,"");
  // bit lookup buffer
  cl_uchar lookup_arr[256];
  boxm2_ocl_util::set_bit_lookup(lookup_arr);
  bocl_mem_sptr lookup=new bocl_mem(device->context(), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
  lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
  // set arguments
  std::vector<boxm2_block_id> vis_order = scene->get_block_ids();
  std::vector<boxm2_block_id>::iterator id;
  bocl_kernel * kern=kernels[0];
  for (id = vis_order.begin(); id != vis_order.end(); ++id)
  {
      //choose correct render kernel
      boxm2_block_metadata mdata = scene->get_block_metadata(*id);
      //write the image values to the buffer
      vul_timer transfer;
      bocl_mem* blk       = opencl_cache->get_block(scene,*id);
      bocl_mem* blk_info  = opencl_cache->loaded_block_info();
      bocl_mem* alpha     = opencl_cache->get_data<BOXM2_ALPHA>(scene,*id,0,false);
      auto* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
      int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
      info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);
      blk_info->write_to_buffer((queue));
      //grab an appropriately sized AUX data buffer
      int auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX3>::prefix());
      bocl_mem *aux3_curr   = opencl_cache->get_data(scene,*id, boxm2_data_traits<BOXM2_AUX3>::prefix(identifier+"_curr"),info_buffer->data_buffer_length*auxTypeSize,false);
      bocl_mem *aux3_prev   = opencl_cache->get_data(scene,*id, boxm2_data_traits<BOXM2_AUX3>::prefix(identifier+"_prev"),info_buffer->data_buffer_length*auxTypeSize,false);
      bocl_mem *aux0_curr   = opencl_cache->get_data(scene,*id, boxm2_data_traits<BOXM2_AUX0>::prefix(identifier+"_curr"),info_buffer->data_buffer_length*auxTypeSize,false);
      transfer_time += (float) transfer.all();
      //set workspace
      std::size_t ltr[] = {64};
      std::size_t gtr[] = { RoundUp(info_buffer->data_buffer_length, ltr[0]) };
      kern->set_arg( blk_info );
      kern->set_arg( aux3_prev );
      kern->set_arg( aux3_curr );
      kern->set_arg( aux0_curr );
      kern->set_arg( alpha );
      //execute kernel
      kern->execute(queue, 1, ltr, gtr);
      int status = clFinish(queue);
      check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status));
      gpu_time += kern->exec_time();
      //clear render kernel args so it can reset em on next execution
      kern->clear_args();
      aux3_prev->read_to_buffer(queue);
      alpha->read_to_buffer(queue);
      opencl_cache->deep_remove_data(scene,*id,boxm2_data_traits<BOXM2_AUX3>::prefix(identifier+"_prev"),true);
      opencl_cache->deep_remove_data(scene,*id,boxm2_data_traits<BOXM2_AUX3>::prefix(identifier+"_curr"),false);
  }
  //read image out to buffer (from gpu)
  clFinish(queue);
  std::cout<<"Gpu time "<<gpu_time<<" transfer time "<<transfer_time<<std::endl;
  clReleaseCommandQueue(queue);
  return true;
}
bool boxm2_ocl_update_PusingQ::compute_probability(const boxm2_scene_sptr&         scene,
                                                   const bocl_device_sptr&         device,
                                                   const boxm2_opencl_cache_sptr&  opencl_cache)

{
  float transfer_time=0.0f;
  float gpu_time=0.0f;
  //cache size sanity check
  std::size_t binCache = opencl_cache.ptr()->bytes_in_cache();
  std::cout<<"Update MBs in cache: "<<binCache/(1024.0*1024.0)<<std::endl;
  // create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue( device->context(),*(device->device_id()),CL_QUEUE_PROFILING_ENABLE,&status);
  if (status!=0)
      return false;



  // compile the kernel if not already compiled
  std::vector<bocl_kernel*>& kernels = get_kernels(device,"");
  // bit lookup buffer
  cl_uchar lookup_arr[256];
  boxm2_ocl_util::set_bit_lookup(lookup_arr);
  bocl_mem_sptr lookup=new bocl_mem(device->context(), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
  lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
  // set arguments
  std::vector<boxm2_block_id> vis_order = scene->get_block_ids();
  std::vector<boxm2_block_id>::iterator id;
  bocl_kernel * kern=kernels[1];
  for (id = vis_order.begin(); id != vis_order.end(); ++id)
  {
      //choose correct render kernel
      boxm2_block_metadata mdata = scene->get_block_metadata(*id);
      float pinit_buf[1];
      pinit_buf[0] = mdata.p_init_;
      bocl_mem * pinit=new bocl_mem(device->context(), pinit_buf, sizeof(float), "pinit");
      pinit->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
      //write the image values to the buffer
      vul_timer transfer;
      bocl_mem* blk       = opencl_cache->get_block(scene,*id);
      bocl_mem* blk_info  = opencl_cache->loaded_block_info();
      bocl_mem* alpha     = opencl_cache->get_data<BOXM2_ALPHA>(scene,*id,0,false);
      auto* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
      int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
      // check for invalid parameters
      if( alphaTypeSize == 0 ) //This should never happen, it will result in division by zero later
      {
          std::cout << "ERROR: alphaTypeSize == 0 in " << __FILE__ << __LINE__ << std::endl;
          return false;
      }

      info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);
      blk_info->write_to_buffer((queue));
      //grab an appropriately sized AUX data buffer
      int auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX3>::prefix());
      bocl_mem *aux3_product   = opencl_cache->get_data(scene,*id, boxm2_data_traits<BOXM2_AUX3>::prefix(),info_buffer->data_buffer_length*auxTypeSize,true);
      transfer_time += (float) transfer.all();

      //set workspace
      std::size_t ltr[] = {4, 4, 4};
      std::size_t gtr[] = { RoundUp(mdata.sub_block_num_.x(), ltr[0]),
                           RoundUp(mdata.sub_block_num_.y(), ltr[1]),
                           RoundUp(mdata.sub_block_num_.z(), ltr[2])};

      kern->set_arg( blk_info );
      kern->set_arg( blk );
      kern->set_arg( alpha );
      kern->set_arg( aux3_product );
      kern->set_arg( pinit );
      kern->set_arg( lookup.ptr() );
      kern->set_local_arg( ltr[0]*ltr[1]*ltr[2]*10*sizeof(cl_uchar) );
      kern->set_local_arg( ltr[0]*ltr[1]*ltr[2]*sizeof(cl_uchar16) );
      //execute kernel
      kern->execute(queue, 3, ltr, gtr);
      int status = clFinish(queue);
      check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status));
      gpu_time += kern->exec_time();

      //clear render kernel args so it can reset em on next execution
      kern->clear_args();

      alpha->read_to_buffer(queue);
      clFinish(queue);
      //pinit->release_memory();

      //delete pinit;
      //opencl_cache->deep_remove_data(*id,boxm2_data_traits<BOXM2_ALPHA>::prefix(),true);
  }
  //read image out to buffer (from gpu)
  clFinish(queue);
  std::cout<<"Gpu time "<<gpu_time<<" transfer time "<<transfer_time<<std::endl;
  clReleaseCommandQueue(queue);

  return true;
}

std::vector<bocl_kernel*>& boxm2_ocl_update_PusingQ::get_kernels(const bocl_device_sptr& device, const std::string& opts)
{
  // compile kernels if not already compiled
  std::string identifier = device->device_identifier() + opts;
  if (kernels_.find(identifier) != kernels_.end())
    return kernels_[identifier];

  //otherwise compile the kernels
  std::cout<<"=== boxm2_ocl_update_auxQ_process::compiling kernels on device "<<identifier<<"==="<<std::endl;
  std::vector<std::string> src_paths;
  std::string source_dir = boxm2_ocl_util::ocl_src_root();
  src_paths.push_back(source_dir + "scene_info.cl");
  src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(source_dir + "bit/update_kernels.cl");
  std::vector<std::string> non_ray_src = std::vector<std::string>(src_paths);

  //populate vector of kernels
  std::vector<bocl_kernel*> vec_kernels;


  //push back cast_ray_bit
  auto* apply_beta = new bocl_kernel();
  std::string apply_beta_opts = opts + " -D APPLYBETA";
  apply_beta->create_kernel(&device->context(), device->device_id(), non_ray_src, "apply_beta", apply_beta_opts, "update::apply_beta");
  vec_kernels.push_back(apply_beta);

  auto* compute_product_Q = new bocl_kernel();
  std::string product_q = opts + " -D PRODUCTQ";
  compute_product_Q->create_kernel(&device->context(), device->device_id(), non_ray_src, "compute_product_Q", product_q, "update::compute_product_Q");
  vec_kernels.push_back(compute_product_Q);
  auto* update_P = new bocl_kernel();
  std::string update_q_opts = opts + " -D UPDATEP";
  update_P->create_kernel(&device->context(), device->device_id(), non_ray_src, "update_P_using_Q", update_q_opts, "update::update_P_using_Q");
  vec_kernels.push_back(update_P);

  //store and return
  kernels_[identifier] = vec_kernels;
  return kernels_[identifier];
}
