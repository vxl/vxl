#include <vector>
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include "boxm2_ocl_update_vis_score.h"

#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/ocl/algo/boxm2_ocl_camera_converter.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_timer.h>

bool
boxm2_ocl_update_vis_score
::compile_kernels()
{
    std::vector<std::string> src_paths;
    std::string source_dir = boxm2_ocl_util::ocl_src_root();
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "pixel_conversion.cl");
    src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(source_dir + "backproject.cl");
    src_paths.push_back(source_dir + "statistics_library_functions.cl");
    src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
    src_paths.push_back(source_dir + "bit/vis_score_kernel.cl");
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

    std::vector<std::string> src_paths2;
    src_paths2.push_back(source_dir + "scene_info.cl");
    src_paths2.push_back(source_dir + "bit/vis_score_kernel.cl");

    //compilation options
    std::string options = "";
    //seg len pass
    std::string seg_opts = options + " -D RAYTRACE_PASS -D STEP_CELL=step_cell_vis_score(aux_args,data_ptr,llid,d*linfo->block_len) ";
    if (use_surface_normals_) {
      seg_opts += "-D USE_SURFACE_NORMALS";
    }
    bool status = seg_len_kernel_.create_kernel(&device_->context(), device_->device_id(), src_paths, "vis_score_raytrace_main", seg_opts, "update::vis_score_raytrace");

    //may need DIFF LIST OF SOURCES FOR THSI GUY TOO
    status &= update_kernel_.create_kernel(&device_->context(), device_->device_id(), src_paths2, "update_vis_score_main", " -D UPDATE_PASS", "update::vis_score");

    return status;
}

boxm2_ocl_update_vis_score
::boxm2_ocl_update_vis_score(const boxm2_scene_sptr& scene,
                             const bocl_device_sptr& device,
                             const boxm2_opencl_cache_sptr& ocl_cache,
                             bool use_surface_normals,
                             bool optimize_transfers) :
  use_surface_normals_(use_surface_normals),
  scene_(scene),
  device_(device),
  ocl_cache_(ocl_cache),
  optimize_transfers_(optimize_transfers)
{
  if (!compile_kernels()) {
    throw std::runtime_error("boxm2_ocl_update_vis_score: Failed to compile opencl kernels");
  }
}

bool
boxm2_ocl_update_vis_score
::run(vpgl_camera_double_sptr cam,
      unsigned ni,
      unsigned nj,
      const std::string& prefix_name )
{
  float transfer_time=0.0f;
  float gpu_time=0.0f;

  // create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device_->context(),*(device_->device_id()),
                                                CL_QUEUE_PROFILING_ENABLE,&status);
  if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)) ) {
    return false;
  }

  //grab input image dimensions - round up to the nearest 8 for OPENCL
  std::size_t local_threads[2]={8,8};
  unsigned cl_ni=RoundUp(ni,local_threads[0]);
  unsigned cl_nj=RoundUp(nj,local_threads[1]);

  std::size_t global_threads[2]={cl_ni, cl_nj};

  // create all buffers
  auto* ray_origins    = new cl_float[4*cl_ni*cl_nj];
  auto* ray_directions = new cl_float[4*cl_ni*cl_nj];
  bocl_mem_sptr ray_o_buff = ocl_cache_->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_origins,"ray_origins buffer");
  bocl_mem_sptr ray_d_buff = ocl_cache_->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_directions,"ray_directions buffer");
  boxm2_ocl_camera_converter::compute_ray_image( device_, queue, cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff);

  //create vis, pre, norm and input image buffers
  auto* vis_buff  = new float[cl_ni*cl_nj];
  std::fill(vis_buff, vis_buff+cl_ni*cl_nj, 1.0f);

  bocl_mem_sptr vis_image = ocl_cache_->alloc_mem(cl_ni*cl_nj*sizeof(float), vis_buff,"vis image buffer");
  vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // Image Dimensions
  int img_dim_buff[4];
  img_dim_buff[0] = 0;
  img_dim_buff[1] = 0;
  img_dim_buff[2] = ni;
  img_dim_buff[3] = nj;
  bocl_mem_sptr img_dim= ocl_cache_->alloc_mem(sizeof(int)*4, img_dim_buff, "image dims");
  img_dim->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // Output Array
  float output_arr[100];
  for (float & i : output_arr) i = 0.0f;
  bocl_mem_sptr cl_output= ocl_cache_->alloc_mem(sizeof(float)*100, output_arr, "output buffer");
  cl_output->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // bit lookup buffer
  cl_uchar lookup_arr[256];
  boxm2_ocl_util::set_bit_lookup(lookup_arr);
  bocl_mem_sptr lookup = ocl_cache_->alloc_mem(sizeof(cl_uchar)*256, lookup_arr, "bit lookup buffer");
  lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  // set arguments
  std::vector<boxm2_block_id> vis_order = scene_->get_vis_blocks(cam);
  std::vector<boxm2_block_id>::iterator id;

  for (id = vis_order.begin(); id != vis_order.end(); ++id)
  {
      std::cout << "update_vis_score: id = " << id->to_string() << std::endl;
      //choose correct render kernel
      boxm2_block_metadata mdata = scene_->get_block_metadata(*id);

      //write the image values to the buffer
      vul_timer transfer;
      bocl_mem* blk       = ocl_cache_->get_block(scene_,*id);
      bocl_mem* alpha     = ocl_cache_->get_data<BOXM2_ALPHA>(scene_,*id);
      bocl_mem* normals = nullptr;
      if (use_surface_normals_) {
        normals   = ocl_cache_->get_data<BOXM2_NORMAL>(scene_,*id);
      }
      bocl_mem* blk_info  = ocl_cache_->loaded_block_info();

      //make sure the scene info data size reflects the real data size
      auto* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
      int alphaTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
      info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);
      blk_info->write_to_buffer((queue));

      //grab an appropriately sized AUX data buffer
      int auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX0>::prefix());
      bocl_mem *aux0   = ocl_cache_->get_data<BOXM2_AUX0>(scene_,*id, info_buffer->data_buffer_length*auxTypeSize,false);
      auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX1>::prefix());
      bocl_mem *aux1   = ocl_cache_->get_data<BOXM2_AUX1>(scene_,*id, info_buffer->data_buffer_length*auxTypeSize,false);
      transfer_time += (float) transfer.all();
      //local_threads[0] = 8; local_threads[1] = 8;
      //global_threads[0]=cl_ni; global_threads[1]=cl_nj;
      // zero out aux buffers
      aux0->zero_gpu_buffer(queue);
      aux1->zero_gpu_buffer(queue);
      seg_len_kernel_.set_arg( blk_info );
      seg_len_kernel_.set_arg( blk );
      seg_len_kernel_.set_arg( alpha );
      if (use_surface_normals_) {
        seg_len_kernel_.set_arg( normals );
      }
      seg_len_kernel_.set_arg( aux0 );
      seg_len_kernel_.set_arg( aux1 );
      seg_len_kernel_.set_arg( lookup.ptr() );
      seg_len_kernel_.set_arg( ray_o_buff.ptr() );
      seg_len_kernel_.set_arg( ray_d_buff.ptr() );
      seg_len_kernel_.set_arg( img_dim.ptr() );
      seg_len_kernel_.set_arg( vis_image.ptr() );
      seg_len_kernel_.set_arg( cl_output.ptr() );
      seg_len_kernel_.set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar16) );//local tree,
      seg_len_kernel_.set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_short2) ); //ray bundle,
      seg_len_kernel_.set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_int) );    //cell pointers,
      seg_len_kernel_.set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_float) ); //cached aux,
      seg_len_kernel_.set_local_arg( local_threads[0]*local_threads[1]*10*sizeof(cl_uchar) ); //cumsum buffer, imindex buffer

      //execute kernel
      seg_len_kernel_.execute(queue, 2, local_threads, global_threads);
      int status = clFinish(queue);
      if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)) ) {
          return false;
      }
      gpu_time += seg_len_kernel_.exec_time();
      clFinish(queue);

      //clear render kernel args so it can reset em on next execution
      seg_len_kernel_.clear_args();
      //read info back to host memory
      if(!optimize_transfers_){
        blk->read_to_buffer(queue);
        aux0->read_to_buffer(queue);
        aux1->read_to_buffer(queue);
        vis_image->read_to_buffer(queue);
        cl_output->read_to_buffer(queue);
        clFinish(queue);
      }
  }

  ocl_cache_->unref_mem(ray_o_buff.ptr());
  ocl_cache_->unref_mem(ray_d_buff.ptr());
  ocl_cache_->unref_mem(vis_image.ptr());
  ocl_cache_->unref_mem(cl_output.ptr());
  ocl_cache_->unref_mem(img_dim.ptr());
  ocl_cache_->unref_mem(lookup.ptr());

  delete [] vis_buff;
  delete [] ray_origins;
  delete [] ray_directions;

  //reset local threads to 8/8 (default);
  local_threads[0] = 64;
  local_threads[1] = 1;
  for (id = vis_order.begin(); id != vis_order.end(); ++id)
  {
      std::cout << "update_vis_score2 : id = " << id->to_string() << std::endl;
      //choose correct render kernel
      boxm2_block_metadata mdata = scene_->get_block_metadata(*id);

      //write the image values to the buffer
      vul_timer transfer;
      bocl_mem* blk       = ocl_cache_->get_block(scene_,*id);
      bocl_mem * blk_info  = ocl_cache_->loaded_block_info();

      bocl_mem* alpha     = ocl_cache_->get_data<BOXM2_ALPHA>(scene_,*id);
      auto* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
      int alphaTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
      info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);
      blk_info->write_to_buffer((queue));

      //std::cout << "getting aux0 and aux1 data " << std::endl;
      //grab an appropriately sized AUX data buffer
      int auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX0>::prefix());
      bocl_mem *aux0   = ocl_cache_->get_data<BOXM2_AUX0>(scene_,*id, info_buffer->data_buffer_length*auxTypeSize,true);
      auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX1>::prefix());
      bocl_mem *aux1   = ocl_cache_->get_data<BOXM2_AUX1>(scene_,*id, info_buffer->data_buffer_length*auxTypeSize,true);

      //std::cout << "getting output data: prefix_name = " << prefix_name << std::endl;
      int vis_score_size = boxm2_data_traits<BOXM2_VIS_SCORE>::datasize();
      bocl_mem *vis_score  = ocl_cache_->get_data(scene_,*id, boxm2_data_traits<BOXM2_VIS_SCORE>::prefix(prefix_name),
                                                  info_buffer->data_buffer_length*vis_score_size,false);

      //std::cout << "got data. " << std::endl;

      transfer_time += (float) transfer.all();
      update_kernel_.set_arg( blk_info );
      update_kernel_.set_arg( aux0 );
      update_kernel_.set_arg( aux1 );
      update_kernel_.set_arg( vis_score );
      global_threads[0]=RoundUp(info_buffer->data_buffer_length,local_threads[0]);
      global_threads[1]=1;
      //std::cout << "executing kernel " << std::endl;

      //execute kernel
      update_kernel_.execute(queue, 2, local_threads, global_threads);
      int status = clFinish(queue);
      if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)) )
          return false;
      gpu_time += update_kernel_.exec_time();
      clFinish(queue);
      //std::cout << "kernel completed" << std::endl;

      //clear render kernel args so it can reset em on next execution
      update_kernel_.clear_args();
      //std::cout << "1" << std::endl;
      //read info back to host memory
      if( ! optimize_transfers_){
        blk->read_to_buffer(queue);
        //std::cout << "2" << std::endl;
        //std::cout << "vis_score cpu_buffer= " << vis_score->cpu_buffer() << std::endl;
        //std::cout << "vis_score nbytes = " << vis_score->num_bytes() << std::endl;
        vis_score->read_to_buffer(queue);
        //std::cout << "3" << std::endl;
        clFinish(queue);
      }
      //std::cout << "4" << std::endl;
      //cache->remove_data_base(scene_,*id,boxm2_data_traits<BOXM2_AUX0>::prefix(prefix_name));
      //std::cout << "5" << std::endl;
  }
  std::cout<<"Gpu time "<<gpu_time<<" transfer time "<<transfer_time<<std::endl;
  clReleaseCommandQueue(queue);
  return true;
}

void
boxm2_ocl_update_vis_score
::reset(const std::string& prefix_name){
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device_->context(),*(device_->device_id()),
                                                CL_QUEUE_PROFILING_ENABLE,&status);
  if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)) ) {
    return ;
  }

  std::vector<boxm2_block_id> block_ids = scene_->get_block_ids();
  std::vector<boxm2_block_id>::iterator id;

  for(id = block_ids.begin(); id!=block_ids.end(); id++){
    bocl_mem *vis_score  = ocl_cache_->get_data(scene_,*id, boxm2_data_traits<BOXM2_VIS_SCORE>::prefix(prefix_name));
    //    bocl_mem * alpha  = ocl_cache_->get_data(scene_,*id, boxm2_data_traits<BOXM2_ALPHA>::prefix());
    vis_score->zero_gpu_buffer(queue);
    //alpha->write_to_buffer(queue);
  }
}
