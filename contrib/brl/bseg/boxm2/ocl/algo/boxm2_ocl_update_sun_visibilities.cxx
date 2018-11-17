#include <iostream>
#include <algorithm>
#include "boxm2_ocl_update_sun_visibilities.h"
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/ocl/algo/boxm2_ocl_camera_converter.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_timer.h>

std::map<std::string, std::vector<bocl_kernel*> > boxm2_ocl_update_sun_visibilities::kernels_;

void boxm2_ocl_update_sun_visibilities::compile_kernel(const bocl_device_sptr& device,std::vector<bocl_kernel*> & vec_kernels,const std::string& opts)
{
    std::vector<std::string> src_paths;
    std::string source_dir = boxm2_ocl_util::ocl_src_root();
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "pixel_conversion.cl");
    src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(source_dir + "backproject.cl");
    src_paths.push_back(source_dir + "statistics_library_functions.cl");
    src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
    src_paths.push_back(source_dir + "bit/sun_visibilities_kernel.cl");
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

    std::vector<std::string> src_paths2;
    src_paths2.push_back(source_dir + "scene_info.cl");
    src_paths2.push_back(source_dir + "bit/sun_visibilities_kernel.cl");

    //compilation options
    std::string options ="" + opts;
    //seg len pass
    auto* seg_len = new bocl_kernel();
    std::string seg_opts = options + " -D SEGLENVIS -D STEP_CELL=step_cell_seglen_vis(aux_args,data_ptr,llid,d*linfo->block_len) ";
    seg_len->create_kernel(&device->context(), device->device_id(), src_paths, "seg_len_and_vis_main", seg_opts, "update::seg_len_vis");
    vec_kernels.push_back(seg_len);

    //may need DIFF LIST OF SOURCES FOR THSI GUY TOO
    auto* update = new bocl_kernel();
    update->create_kernel(&device->context(), device->device_id(), src_paths2, "update_visibilities_main", " -D UPDATE_SUN_VIS", "update::update_visibilities_main");
    vec_kernels.push_back(update);

    return ;
}

bool boxm2_ocl_update_sun_visibilities::update( const boxm2_scene_sptr&         scene,
                                                bocl_device_sptr         device,
                                                const boxm2_opencl_cache_sptr&  opencl_cache,
                                                const boxm2_cache_sptr&          /*cache*/,
                                                vpgl_camera_double_sptr  sun_cam,
                                                unsigned ni,
                                                unsigned nj,
                                                const std::string&               prefix_name
                                              )
{
  float transfer_time=0.0f;
  float gpu_time=0.0f;

  // create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device->context(),*(device->device_id()),
                                                CL_QUEUE_PROFILING_ENABLE,&status);
  if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)) ) {
    return false;
  }

  // compile kernels if not already compiled
  std::string identifier = device->device_identifier();
  if (kernels_.find(identifier)==kernels_.end())
  {
    std::cout<<"boxm2_ocl_update_sun_visibilities_process::compiling kernels on device" << identifier << "===" << std::endl;
    std::vector<bocl_kernel*> ks;
    compile_kernel(device,ks,"");
    kernels_[identifier]=ks;
  }
  //grab input image dimensions - round up to the nearest 8 for OPENCL
  std::size_t local_threads[2]={8,8};
  unsigned cl_ni=RoundUp(ni,local_threads[0]);
  unsigned cl_nj=RoundUp(nj,local_threads[1]);

  std::size_t global_threads[2]={cl_ni, cl_nj};

  // create all buffers
  auto* ray_origins    = new cl_float[4*cl_ni*cl_nj];
  auto* ray_directions = new cl_float[4*cl_ni*cl_nj];
  bocl_mem_sptr ray_o_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_origins,"ray_origins buffer");
  bocl_mem_sptr ray_d_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_directions,"ray_directions buffer");
  boxm2_ocl_camera_converter::compute_ray_image( device, queue, sun_cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff);

  //create vis, pre, norm and input image buffers
  auto* vis_buff  = new float[cl_ni*cl_nj];
  std::fill(vis_buff, vis_buff+cl_ni*cl_nj, 1.0f);

  bocl_mem_sptr vis_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), vis_buff,"vis image buffer");
  vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  auto* last_vis_buff  = new float[cl_ni*cl_nj];
  std::fill(last_vis_buff, last_vis_buff+cl_ni*cl_nj, 1.0f);

  bocl_mem_sptr last_vis_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), last_vis_buff, "last vis image buffer");
  last_vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // Image Dimensions
  int img_dim_buff[4];
  img_dim_buff[0] = 0;
  img_dim_buff[1] = 0;
  img_dim_buff[2] = ni;
  img_dim_buff[3] = nj;
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

  // set arguments
  std::vector<boxm2_block_id> vis_order = scene->get_vis_blocks((vpgl_generic_camera<double>*)sun_cam.ptr());
  std::vector<boxm2_block_id>::iterator id;

  for (id = vis_order.begin(); id != vis_order.end(); ++id)
  {
      std::cout << "update_sun_vis0: id = " << id->to_string() << std::endl;
      //choose correct render kernel
      boxm2_block_metadata mdata = scene->get_block_metadata(*id);
      bocl_kernel* kern =  kernels_[identifier][0];

      //write the image values to the buffer
      vul_timer transfer;
      bocl_mem* blk       = opencl_cache->get_block(scene,*id);
      bocl_mem* alpha     = opencl_cache->get_data<BOXM2_ALPHA>(scene,*id);
      bocl_mem * blk_info  = opencl_cache->loaded_block_info();

      //make sure the scene info data size reflects the real data size
      auto* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
      int alphaTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
      info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);
      blk_info->write_to_buffer((queue));

      //grab an appropriately sized AUX data buffer
      int auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX0>::prefix());
      bocl_mem *aux0   = opencl_cache->get_data<BOXM2_AUX0>(scene,*id, info_buffer->data_buffer_length*auxTypeSize,false);
      auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX1>::prefix());
      bocl_mem *aux1   = opencl_cache->get_data<BOXM2_AUX1>(scene,*id, info_buffer->data_buffer_length*auxTypeSize,false);
      transfer_time += (float) transfer.all();
      //local_threads[0] = 8; local_threads[1] = 8;
      //global_threads[0]=cl_ni; global_threads[1]=cl_nj;
      // zero out aux buffers
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
      kern->set_arg( img_dim.ptr() );
      kern->set_arg( vis_image.ptr() );
      kern->set_arg( last_vis_image.ptr() );
      kern->set_arg( cl_output.ptr() );
      kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar16) );//local tree,
      kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_short2) ); //ray bundle,
      kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_int) );    //cell pointers,
      kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_float) ); //cached aux,
      kern->set_local_arg( local_threads[0]*local_threads[1]*10*sizeof(cl_uchar) ); //cumsum buffer, imindex buffer

      //execute kernel
      kern->execute(queue, 2, local_threads, global_threads);
      int status = clFinish(queue);
      if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)) )
          return false;
      gpu_time += kern->exec_time();
      clFinish(queue);

      //clear render kernel args so it can reset em on next execution
      kern->clear_args();
      //read info back to host memory
      blk->read_to_buffer(queue);
      aux0->read_to_buffer(queue);
      aux1->read_to_buffer(queue);
      vis_image->read_to_buffer(queue);
      last_vis_image->read_to_buffer(queue);
      cl_output->read_to_buffer(queue);
      clFinish(queue);
  }

  opencl_cache->unref_mem(ray_o_buff.ptr());
  opencl_cache->unref_mem(ray_d_buff.ptr());
  opencl_cache->unref_mem(vis_image.ptr());
  opencl_cache->unref_mem(last_vis_image.ptr());

  delete [] vis_buff;
  delete [] last_vis_buff;
  delete [] ray_origins;
  delete [] ray_directions;

  //reset local threads to 8/8 (default);
  local_threads[0] = 64;
  local_threads[1] = 1;
  for (id = vis_order.begin(); id != vis_order.end(); ++id)
  {
      std::cout << "update_sun_vis2 : id = " << id->to_string() << std::endl;
      //choose correct render kernel
      boxm2_block_metadata mdata = scene->get_block_metadata(*id);
      //std::cout << "got metadata " << std::endl;
      bocl_kernel* kern =  kernels_[identifier][1];

      //write the image values to the buffer
      vul_timer transfer;
      bocl_mem* blk       = opencl_cache->get_block(scene,*id);
      bocl_mem * blk_info  = opencl_cache->loaded_block_info();

      bocl_mem* alpha     = opencl_cache->get_data<BOXM2_ALPHA>(scene,*id);
      auto* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
      int alphaTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
      info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);
      blk_info->write_to_buffer((queue));

      //std::cout << "getting aux0 and aux1 data " << std::endl;
      //grab an appropriately sized AUX data buffer
      int auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX0>::prefix());
      bocl_mem *aux0   = opencl_cache->get_data<BOXM2_AUX0>(scene,*id, info_buffer->data_buffer_length*auxTypeSize,true);
      auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX1>::prefix());
      bocl_mem *aux1   = opencl_cache->get_data<BOXM2_AUX1>(scene,*id, info_buffer->data_buffer_length*auxTypeSize,true);

      //std::cout << "getting output data: prefix_name = " << prefix_name << std::endl;
      bocl_mem *aux_out  = opencl_cache->get_data(scene,*id, boxm2_data_traits<BOXM2_AUX0>::prefix(prefix_name),
                                                  info_buffer->data_buffer_length*auxTypeSize,false);

      //std::cout << "got data. " << std::endl;

      transfer_time += (float) transfer.all();
      kern->set_arg( blk_info );
      kern->set_arg( aux0 );
      kern->set_arg( aux1 );
      kern->set_arg( aux_out );
      global_threads[0]=RoundUp(info_buffer->data_buffer_length,local_threads[0]);
      global_threads[1]=1;
      //std::cout << "executing kernel " << std::endl;

      //execute kernel
      kern->execute(queue, 2, local_threads, global_threads);
      int status = clFinish(queue);
      if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)) )
          return false;
      gpu_time += kern->exec_time();
      clFinish(queue);
      //std::cout << "kernel completed" << std::endl;

      //clear render kernel args so it can reset em on next execution
      kern->clear_args();
      //std::cout << "1" << std::endl;
      //read info back to host memory
      blk->read_to_buffer(queue);
      //std::cout << "2" << std::endl;
      //std::cout << "aux_out cpu_buffer= " << aux_out->cpu_buffer() << std::endl;
      //std::cout << "aux_out nbytes = " << aux_out->num_bytes() << std::endl;
      aux_out->read_to_buffer(queue);
      //std::cout << "3" << std::endl;
      clFinish(queue);
      //std::cout << "4" << std::endl;
      //cache->remove_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX0>::prefix(prefix_name));
      //std::cout << "5" << std::endl;
  }
  std::cout<<"Gpu time "<<gpu_time<<" transfer time "<<transfer_time<<std::endl;
  clReleaseCommandQueue(queue);
  return true;
}
