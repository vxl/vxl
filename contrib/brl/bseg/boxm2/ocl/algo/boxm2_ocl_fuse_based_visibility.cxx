// This is brl/bseg/boxm2/ocl/algo/boxm2_ocl_fuse_based_visibility.cxx
#include <fstream>
#include <iostream>
#include <algorithm>
#include "boxm2_ocl_fuse_based_visibility.h"
//:
// \file
// \brief  A process for fusing models based on visibility ( fusion of 3-d models ( zeroth order ) )
//
// \author Vishal Jain
// \date Nov 13, 2013

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
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
#include <boct/boct_bit_tree.h>
#include <vnl/vnl_numeric_traits.h>

//: Map of kernels should persist between process executions
std::map<std::string,std::vector<bocl_kernel*> > boxm2_ocl_fuse_based_visibility::kernels_;

//Main public method, updates color model
bool boxm2_ocl_fuse_based_visibility::fuse_based_visibility(boxm2_scene_sptr         sceneA,
                                                            const boxm2_scene_sptr&         sceneB,
                                                            const bocl_device_sptr&         device,
                                                            const boxm2_opencl_cache_sptr&  opencl_cache)
{


  float transfer_time=0.0f;
  float gpu_time=0.0f;
  std::size_t local_threads[1]={64};
  std::size_t global_threads[1]={64};

  bocl_mem_sptr centerX = new bocl_mem(device->context(), boct_bit_tree::centerX, sizeof(cl_float)*585, "centersX lookup buffer");
  bocl_mem_sptr centerY = new bocl_mem(device->context(), boct_bit_tree::centerY, sizeof(cl_float)*585, "centersY lookup buffer");
  bocl_mem_sptr centerZ = new bocl_mem(device->context(), boct_bit_tree::centerZ, sizeof(cl_float)*585, "centersZ lookup buffer");
  centerX->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
  centerY->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
  centerZ->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
  // output buffer for debugging
  float output_buff[1000];
  bocl_mem_sptr output = new bocl_mem(device->context(), output_buff, sizeof(float)*1000, "output" );
  output->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR );
  // bit lookup buffer
  cl_uchar lookup_arr[256];
  boxm2_ocl_util::set_bit_lookup(lookup_arr);
  bocl_mem_sptr lookup=new bocl_mem(device->context(), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
  lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
  int status = 0;
  cl_command_queue queue = clCreateCommandQueue(device->context(),*(device->device_id()),CL_QUEUE_PROFILING_ENABLE,&status);
  //cache size sanity check
  std::size_t binCache = opencl_cache.ptr()->bytes_in_cache();
  std::cout<<"Update MBs in cache: "<<binCache/(1024.0*1024.0)<<std::endl;

  std::string options = "";
  // compile the kernel if not already compiled
  std::vector<bocl_kernel*>& kernels = get_kernels(device, options);
  std::vector<boxm2_block_id> blocks_A = sceneA->get_block_ids();
  std::vector<boxm2_block_id> blocks_B = sceneB->get_block_ids();
  std::cout<<sceneA->data_path()<<" "<<sceneB->data_path()<<std::endl;
  auto iter_blks_A = blocks_A.begin();
  auto iter_blks_B = blocks_B.begin();

  int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());

  bocl_kernel * kern = boxm2_ocl_fuse_based_visibility::get_kernels(device,"")[0];
  for (;iter_blks_A!=blocks_A.end() || iter_blks_B!=blocks_B.end(); iter_blks_A++,iter_blks_B++)
  {
      if((*iter_blks_A) != (*iter_blks_B))
      {
          std::cout<<"Blocks do  not match "<<(*iter_blks_A)<<" "<<(*iter_blks_B)<<std::endl;
          return false;
      }
      bocl_mem* blk_A       = opencl_cache->get_block(sceneA, *iter_blks_A);
      bocl_mem* alpha_A     = opencl_cache->get_data<BOXM2_ALPHA>(sceneA, *iter_blks_A,0,false);
      bocl_mem* vis_score_A     = opencl_cache->get_data<BOXM2_VIS_SCORE>(sceneA, *iter_blks_A,0,true);
      bocl_mem* app_A     = opencl_cache->get_data<BOXM2_GAUSS_RGB>(sceneA, *iter_blks_A,0,false);
      boxm2_scene_info* info_buffer_A = sceneA->get_blk_metadata(*iter_blks_A);
      info_buffer_A->data_buffer_length = (int) (alpha_A->num_bytes()/alphaTypeSize);
      bocl_mem* blk_info_A  = new bocl_mem(device->context(), info_buffer_A, sizeof(boxm2_scene_info), " Scene Info" );
      blk_info_A->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

      bocl_mem* blk_B       = opencl_cache->get_block(sceneB, *iter_blks_B);
      bocl_mem* alpha_B     = opencl_cache->get_data<BOXM2_ALPHA>(sceneB, *iter_blks_B,0,false);
      bocl_mem* app_B       = opencl_cache->get_data<BOXM2_GAUSS_RGB>(sceneB, *iter_blks_A,0,false);
      bocl_mem* vis_score_B = opencl_cache->get_data<BOXM2_VIS_SCORE>(sceneB, *iter_blks_B,0,true);
      boxm2_scene_info* info_buffer_B = sceneB->get_blk_metadata(*iter_blks_B);
      info_buffer_B->data_buffer_length = (int) (alpha_B->num_bytes()/alphaTypeSize);
      bocl_mem* blk_info_B  = new bocl_mem(device->context(), info_buffer_B, sizeof(boxm2_scene_info), " Scene Info" );
      blk_info_B->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
      global_threads[0] = (unsigned) RoundUp(info_buffer_A->scene_dims[0]*info_buffer_A->scene_dims[1]*info_buffer_A->scene_dims[2],(int)local_threads[0]);

      std::cout<<alpha_A->num_bytes()<<" "<<alpha_B->num_bytes()<<std::endl;
      kern->set_arg(centerX.ptr());
      kern->set_arg(centerY.ptr());
      kern->set_arg(centerZ.ptr());
      kern->set_arg(lookup.ptr());
      kern->set_arg(blk_info_A);
      kern->set_arg(blk_info_B);
      kern->set_arg(blk_A);
      kern->set_arg(alpha_A);
      kern->set_arg(vis_score_A);
      kern->set_arg(app_A);
      kern->set_arg(blk_B);
      kern->set_arg(alpha_B);
      kern->set_arg(vis_score_B);
      kern->set_arg(app_B);
      kern->set_arg(output.ptr());
      kern->set_local_arg(local_threads[0]*10*sizeof(cl_uchar) );    // cumsum buffer,
      kern->set_local_arg(16*local_threads[0]*sizeof(unsigned char)); // local trees
      kern->set_local_arg(16*local_threads[0]*sizeof(unsigned char)); // local trees
      if(!kern->execute(queue, 1, local_threads, global_threads))
      {
          std::cout<<"Kernel Failed to Execute "<<std::endl;
          return false;
      }
      int status = clFinish(queue);
      check_val(status, MEM_FAILURE, "Fusion ( Based on Visibility ) EXECUTE FAILED: " + error_to_string(status));
      gpu_time += kern->exec_time();
      //clear render kernel args so it can reset em on next execution
      kern->clear_args();
      clFinish(queue);
      alpha_A->read_to_buffer(queue);
      app_A->read_to_buffer(queue);
      clFinish(queue);

      opencl_cache->get_cpu_cache()->remove_data_base(sceneA,(*iter_blks_A),boxm2_data_traits<BOXM2_ALPHA>::prefix());
       opencl_cache->get_cpu_cache()->remove_data_base(sceneA,(*iter_blks_A),boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix());

       blk_info_B->release_memory();
      delete info_buffer_B;
      blk_info_A->release_memory();
      delete info_buffer_A;

  }
  std::cout<<"Gpu time "<<gpu_time<<" transfer time "<<transfer_time<<std::endl;
  clReleaseCommandQueue(queue);
  return true;
}


//Returns vector of color update kernels (and caches them per device
std::vector<bocl_kernel*>& boxm2_ocl_fuse_based_visibility::get_kernels(const bocl_device_sptr& device, const std::string& opts)
{
  // compile kernels if not already compiled
  std::string identifier = device->device_identifier() + opts;
  if (kernels_.find(identifier) != kernels_.end())
    return kernels_[identifier];

  //otherwise compile the kernels
  std::cout<<"=== boxm2_ocl_fuse_based_visibility_process::compiling kernels on device "<<identifier<<"==="<<std::endl;

  std::vector<std::string> src_paths;
  std::string source_dir = boxm2_ocl_util::ocl_src_root();
  src_paths.push_back(source_dir + "scene_info.cl");
  src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(source_dir + "pixel_conversion.cl");
  src_paths.push_back(source_dir + "statistics_library_functions.cl");
  src_paths.push_back(source_dir + "atomics_util.cl");
  src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
  src_paths.push_back(source_dir + "fusion/fusion_kernels.cl");
  //compilation options
  const std::string& options = opts;
  //populate vector of kernels
  std::vector<bocl_kernel*> vec_kernels;
  //may need DIFF LIST OF SOURCES FOR
  auto* fuse = new bocl_kernel();
  std::string update_opts = options + " -D VISIBILITY_BASED";
  fuse->create_kernel(&device->context(), device->device_id(), src_paths, "fuse_blockwise_based_visibility", update_opts, "fusion::fuse_blockwise_based_visibility");
  vec_kernels.push_back(fuse);
  //store and return
  kernels_[identifier] = vec_kernels;
  return kernels_[identifier];
}


//: Map of kernels should persist between process executions
std::map<std::string,std::vector<bocl_kernel*> > boxm2_ocl_fuse_based_orientation::kernels_;

//Main public method, updates color model
bool boxm2_ocl_fuse_based_orientation::fuse_based_orientation(boxm2_scene_sptr         sceneA,
                                                                            const boxm2_scene_sptr&         sceneB,
                                                                            const bocl_device_sptr&         device,
                                                                            const boxm2_opencl_cache_sptr&  opencl_cache)
{


  float transfer_time=0.0f;
  float gpu_time=0.0f;
  std::size_t local_threads[1]={64};
  std::size_t global_threads[1]={64};

  bocl_mem_sptr centerX = new bocl_mem(device->context(), boct_bit_tree::centerX, sizeof(cl_float)*585, "centersX lookup buffer");
  bocl_mem_sptr centerY = new bocl_mem(device->context(), boct_bit_tree::centerY, sizeof(cl_float)*585, "centersY lookup buffer");
  bocl_mem_sptr centerZ = new bocl_mem(device->context(), boct_bit_tree::centerZ, sizeof(cl_float)*585, "centersZ lookup buffer");
  centerX->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
  centerY->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
  centerZ->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
  // output buffer for debugging
  float output_buff[1000];
  bocl_mem_sptr output = new bocl_mem(device->context(), output_buff, sizeof(float)*1000, "output" );
  output->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR );
  // bit lookup buffer
  cl_uchar lookup_arr[256];
  boxm2_ocl_util::set_bit_lookup(lookup_arr);
  bocl_mem_sptr lookup=new bocl_mem(device->context(), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
  lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
  int status = 0;
  cl_command_queue queue = clCreateCommandQueue(device->context(),*(device->device_id()),CL_QUEUE_PROFILING_ENABLE,&status);
  //cache size sanity check
  std::size_t binCache = opencl_cache.ptr()->bytes_in_cache();
  std::cout<<"Update MBs in cache: "<<binCache/(1024.0*1024.0)<<std::endl;

  std::string options = "";
  // compile the kernel if not already compiled
  std::vector<bocl_kernel*>& kernels = get_kernels(device, options);
  std::vector<boxm2_block_id> blocks_A = sceneA->get_block_ids();
  std::vector<boxm2_block_id> blocks_B = sceneB->get_block_ids();
  std::cout<<sceneA->data_path()<<" "<<sceneB->data_path()<<std::endl;
  auto iter_blks_A = blocks_A.begin();
  auto iter_blks_B = blocks_B.begin();

  int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());

  bocl_kernel * kern = boxm2_ocl_fuse_based_orientation::get_kernels(device,"")[0];
  for (;iter_blks_A!=blocks_A.end() || iter_blks_B!=blocks_B.end(); iter_blks_A++,iter_blks_B++)
  {
      if((*iter_blks_A) != (*iter_blks_B))
      {
          std::cout<<"Blocks do  not match "<<(*iter_blks_A)<<" "<<(*iter_blks_B)<<std::endl;
          return false;
      }
      bocl_mem* blk_A       = opencl_cache->get_block(sceneA, *iter_blks_A);
      bocl_mem* alpha_A     = opencl_cache->get_data<BOXM2_ALPHA>(sceneA, *iter_blks_A,0,false);
      bocl_mem* vis_A     = opencl_cache->get_data<BOXM2_AUX3>(sceneA, *iter_blks_A,0,true,"normaldot");
      bocl_mem* exp_A     = opencl_cache->get_data<BOXM2_EXPECTATION>(sceneA, *iter_blks_A,0,true,"normaldot");
      bocl_mem* app_A     = opencl_cache->get_data<BOXM2_GAUSS_RGB>(sceneA, *iter_blks_A,0,false);
      boxm2_scene_info* info_buffer_A = sceneA->get_blk_metadata(*iter_blks_A);
      info_buffer_A->data_buffer_length = (int) (alpha_A->num_bytes()/alphaTypeSize);
      bocl_mem* blk_info_A  = new bocl_mem(device->context(), info_buffer_A, sizeof(boxm2_scene_info), " Scene Info" );
      blk_info_A->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

      bocl_mem* blk_B       = opencl_cache->get_block(sceneB, *iter_blks_B);
      bocl_mem* alpha_B     = opencl_cache->get_data<BOXM2_ALPHA>(sceneB, *iter_blks_B,0,false);
      bocl_mem* app_B       = opencl_cache->get_data<BOXM2_GAUSS_RGB>(sceneB, *iter_blks_B,0,false);
      bocl_mem* vis_B     = opencl_cache->get_data<BOXM2_AUX3>(sceneB, *iter_blks_B,0,true,"normaldot");
      bocl_mem* exp_B     = opencl_cache->get_data<BOXM2_EXPECTATION>(sceneB, *iter_blks_B,0,true,"normaldot");
      boxm2_scene_info* info_buffer_B = sceneB->get_blk_metadata(*iter_blks_B);
      info_buffer_B->data_buffer_length = (int) (alpha_B->num_bytes()/alphaTypeSize);
      bocl_mem* blk_info_B  = new bocl_mem(device->context(), info_buffer_B, sizeof(boxm2_scene_info), " Scene Info" );
      blk_info_B->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
      global_threads[0] = (unsigned) RoundUp(info_buffer_A->scene_dims[0]*info_buffer_A->scene_dims[1]*info_buffer_A->scene_dims[2],(int)local_threads[0]);

      std::cout<<alpha_A->num_bytes()<<" "<<alpha_B->num_bytes()<<std::endl;
      kern->set_arg(centerX.ptr());
      kern->set_arg(centerY.ptr());
      kern->set_arg(centerZ.ptr());
      kern->set_arg(lookup.ptr());
      kern->set_arg(blk_info_A);
      kern->set_arg(blk_info_B);
      kern->set_arg(blk_A);
      kern->set_arg(alpha_A);
      kern->set_arg(exp_A);
      kern->set_arg(vis_A);
      kern->set_arg(app_A);
      kern->set_arg(blk_B);
      kern->set_arg(alpha_B);
      kern->set_arg(exp_B);
      kern->set_arg(vis_B);
      kern->set_arg(app_B);
      kern->set_arg(output.ptr());
      kern->set_local_arg(local_threads[0]*10*sizeof(cl_uchar) );    // cumsum buffer,
      kern->set_local_arg(16*local_threads[0]*sizeof(unsigned char)); // local trees
      kern->set_local_arg(16*local_threads[0]*sizeof(unsigned char)); // local trees
      if(!kern->execute(queue, 1, local_threads, global_threads))
      {
          std::cout<<"Kernel Failed to Execute "<<std::endl;
          return false;
      }
      int status = clFinish(queue);
      check_val(status, MEM_FAILURE, "Fusion ( Based on Visibility ) EXECUTE FAILED: " + error_to_string(status));
      gpu_time += kern->exec_time();
      //clear render kernel args so it can reset em on next execution
      kern->clear_args();
      clFinish(queue);
      alpha_A->read_to_buffer(queue);
      app_A->read_to_buffer(queue);
      clFinish(queue);

      opencl_cache->get_cpu_cache()->remove_data_base(sceneA,(*iter_blks_A),boxm2_data_traits<BOXM2_ALPHA>::prefix());
       opencl_cache->get_cpu_cache()->remove_data_base(sceneA,(*iter_blks_A),boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix());

       blk_info_B->release_memory();
      delete info_buffer_B;
      blk_info_A->release_memory();
      delete info_buffer_A;

  }
  std::cout<<"Gpu time "<<gpu_time<<" transfer time "<<transfer_time<<std::endl;
  clReleaseCommandQueue(queue);
  return true;
}


//Returns vector of color update kernels (and caches them per device
std::vector<bocl_kernel*>& boxm2_ocl_fuse_based_orientation::get_kernels(const bocl_device_sptr& device, const std::string& opts)
{
  // compile kernels if not already compiled
  std::string identifier = device->device_identifier() + opts;
  if (kernels_.find(identifier) != kernels_.end())
    return kernels_[identifier];

  //otherwise compile the kernels
  std::cout<<"=== boxm2_ocl_fuse_based_visibility_process::compiling kernels on device "<<identifier<<"==="<<std::endl;

  std::vector<std::string> src_paths;
  std::string source_dir = boxm2_ocl_util::ocl_src_root();
  src_paths.push_back(source_dir + "scene_info.cl");
  src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(source_dir + "pixel_conversion.cl");
  src_paths.push_back(source_dir + "statistics_library_functions.cl");
  src_paths.push_back(source_dir + "atomics_util.cl");
  src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
  src_paths.push_back(source_dir + "fusion/fusion_kernels.cl");
  //compilation options
  const std::string& options = opts;
  //populate vector of kernels
  std::vector<bocl_kernel*> vec_kernels;
  //may need DIFF LIST OF SOURCES FOR
  auto* fuse = new bocl_kernel();
  std::string update_opts = options + " -D ORIENTATION_BASED";
  fuse->create_kernel(&device->context(), device->device_id(), src_paths, "fuse_blockwise_based_orientation", update_opts, "fusion::fuse_blockwise_based_orientation");
  vec_kernels.push_back(fuse);
  //store and return
  kernels_[identifier] = vec_kernels;
  return kernels_[identifier];
}




//: Map of kernels should persist between process executions
std::map<std::string,std::vector<bocl_kernel*> > boxm2_ocl_fuse_surface_density::kernels_;

//Main public method, updates color model
bool boxm2_ocl_fuse_surface_density::fuse_surface_density(boxm2_scene_sptr         sceneA,
                                                          const boxm2_scene_sptr&         sceneB,
                                                          const bocl_device_sptr&         device,
                                                          const boxm2_opencl_cache_sptr&  opencl_cache)
{


  float transfer_time=0.0f;
  float gpu_time=0.0f;
  std::size_t local_threads[1]={64};
  std::size_t global_threads[1]={64};

  bocl_mem_sptr centerX = new bocl_mem(device->context(), boct_bit_tree::centerX, sizeof(cl_float)*585, "centersX lookup buffer");
  bocl_mem_sptr centerY = new bocl_mem(device->context(), boct_bit_tree::centerY, sizeof(cl_float)*585, "centersY lookup buffer");
  bocl_mem_sptr centerZ = new bocl_mem(device->context(), boct_bit_tree::centerZ, sizeof(cl_float)*585, "centersZ lookup buffer");
  centerX->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
  centerY->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
  centerZ->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
  // output buffer for debugging
  float output_buff[1000];
  bocl_mem_sptr output = new bocl_mem(device->context(), output_buff, sizeof(float)*1000, "output" );
  output->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR );
  // bit lookup buffer
  cl_uchar lookup_arr[256];
  boxm2_ocl_util::set_bit_lookup(lookup_arr);
  bocl_mem_sptr lookup=new bocl_mem(device->context(), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
  lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
  int status = 0;
  cl_command_queue queue = clCreateCommandQueue(device->context(),*(device->device_id()),CL_QUEUE_PROFILING_ENABLE,&status);
  //cache size sanity check
  std::size_t binCache = opencl_cache.ptr()->bytes_in_cache();
  std::cout<<"Update MBs in cache: "<<binCache/(1024.0*1024.0)<<std::endl;

  std::string options = "";
  // compile the kernel if not already compiled
  std::vector<bocl_kernel*>& kernels = get_kernels(device, options);
  std::vector<boxm2_block_id> blocks_A = sceneA->get_block_ids();
  std::vector<boxm2_block_id> blocks_B = sceneB->get_block_ids();
  std::cout<<sceneA->data_path()<<" "<<sceneB->data_path()<<std::endl;
  auto iter_blks_A = blocks_A.begin();
  auto iter_blks_B = blocks_B.begin();

  int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());

  bocl_kernel * kern = boxm2_ocl_fuse_surface_density::get_kernels(device,"")[0];
  for (;iter_blks_A!=blocks_A.end() || iter_blks_B!=blocks_B.end(); iter_blks_A++,iter_blks_B++)
  {
      if((*iter_blks_A) != (*iter_blks_B))
      {
          std::cout<<"Blocks do  not match "<<(*iter_blks_A)<<" "<<(*iter_blks_B)<<std::endl;
          return false;
      }
      bocl_mem* blk_A       = opencl_cache->get_block(sceneA, *iter_blks_A);
      bocl_mem* alpha_A     = opencl_cache->get_data<BOXM2_ALPHA>(sceneA, *iter_blks_A,0,false);
      bocl_mem* vis_A     = opencl_cache->get_data<BOXM2_AUX3>(sceneA, *iter_blks_A,0,true,"surfacedensity");
      bocl_mem* exp_A     = opencl_cache->get_data<BOXM2_EXPECTATION>(sceneA, *iter_blks_A,0,true,"surfacedensity");
      bocl_mem* app_A     = opencl_cache->get_data<BOXM2_GAUSS_RGB>(sceneA, *iter_blks_A,0,false);
      boxm2_scene_info* info_buffer_A = sceneA->get_blk_metadata(*iter_blks_A);
      info_buffer_A->data_buffer_length = (int) (alpha_A->num_bytes()/alphaTypeSize);
      bocl_mem* blk_info_A  = new bocl_mem(device->context(), info_buffer_A, sizeof(boxm2_scene_info), " Scene Info" );
      blk_info_A->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

      bocl_mem* blk_B       = opencl_cache->get_block(sceneB, *iter_blks_B);
      bocl_mem* alpha_B     = opencl_cache->get_data<BOXM2_ALPHA>(sceneB, *iter_blks_B,0,false);
      bocl_mem* app_B       = opencl_cache->get_data<BOXM2_GAUSS_RGB>(sceneB, *iter_blks_B,0,false);
      bocl_mem* vis_B     = opencl_cache->get_data<BOXM2_AUX3>(sceneB, *iter_blks_B,0,true,"surfacedensity");
      bocl_mem* exp_B     = opencl_cache->get_data<BOXM2_EXPECTATION>(sceneB, *iter_blks_B,0,true,"surfacedensity");
      boxm2_scene_info* info_buffer_B = sceneB->get_blk_metadata(*iter_blks_B);
      info_buffer_B->data_buffer_length = (int) (alpha_B->num_bytes()/alphaTypeSize);
      bocl_mem* blk_info_B  = new bocl_mem(device->context(), info_buffer_B, sizeof(boxm2_scene_info), " Scene Info" );
      blk_info_B->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
      global_threads[0] = (unsigned) RoundUp(info_buffer_A->scene_dims[0]*info_buffer_A->scene_dims[1]*info_buffer_A->scene_dims[2],(int)local_threads[0]);

      std::cout<<alpha_A->num_bytes()<<" "<<alpha_B->num_bytes()<<std::endl;
      kern->set_arg(centerX.ptr());
      kern->set_arg(centerY.ptr());
      kern->set_arg(centerZ.ptr());
      kern->set_arg(lookup.ptr());
      kern->set_arg(blk_info_A);
      kern->set_arg(blk_info_B);
      kern->set_arg(blk_A);
      kern->set_arg(alpha_A);
      kern->set_arg(exp_A);
      kern->set_arg(vis_A);
      kern->set_arg(app_A);
      kern->set_arg(blk_B);
      kern->set_arg(alpha_B);
      kern->set_arg(exp_B);
      kern->set_arg(vis_B);
      kern->set_arg(app_B);
      kern->set_arg(output.ptr());
      kern->set_local_arg(local_threads[0]*10*sizeof(cl_uchar) );    // cumsum buffer,
      kern->set_local_arg(16*local_threads[0]*sizeof(unsigned char)); // local trees
      kern->set_local_arg(16*local_threads[0]*sizeof(unsigned char)); // local trees
      if(!kern->execute(queue, 1, local_threads, global_threads))
      {
          std::cout<<"Kernel Failed to Execute "<<std::endl;
          return false;
      }
      int status = clFinish(queue);
      check_val(status, MEM_FAILURE, "Fusion ( Based on Visibility ) EXECUTE FAILED: " + error_to_string(status));
      gpu_time += kern->exec_time();
      //clear render kernel args so it can reset em on next execution
      kern->clear_args();
      clFinish(queue);
      alpha_A->read_to_buffer(queue);
      app_A->read_to_buffer(queue);
      clFinish(queue);

      opencl_cache->get_cpu_cache()->remove_data_base(sceneA,(*iter_blks_A),boxm2_data_traits<BOXM2_ALPHA>::prefix());
       opencl_cache->get_cpu_cache()->remove_data_base(sceneA,(*iter_blks_A),boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix());

       blk_info_B->release_memory();
      delete info_buffer_B;
      blk_info_A->release_memory();
      delete info_buffer_A;

  }
  std::cout<<"Gpu time "<<gpu_time<<" transfer time "<<transfer_time<<std::endl;
  clReleaseCommandQueue(queue);
  return true;
}


//Returns vector of color update kernels (and caches them per device
std::vector<bocl_kernel*>& boxm2_ocl_fuse_surface_density::get_kernels(const bocl_device_sptr& device, const std::string& opts)
{
  // compile kernels if not already compiled
  std::string identifier = device->device_identifier() + opts;
  if (kernels_.find(identifier) != kernels_.end())
    return kernels_[identifier];

  //otherwise compile the kernels
  std::cout<<"=== boxm2_ocl_fuse_based_visibility_process::compiling kernels on device "<<identifier<<"==="<<std::endl;

  std::vector<std::string> src_paths;
  std::string source_dir = boxm2_ocl_util::ocl_src_root();
  src_paths.push_back(source_dir + "scene_info.cl");
  src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(source_dir + "pixel_conversion.cl");
  src_paths.push_back(source_dir + "statistics_library_functions.cl");
  src_paths.push_back(source_dir + "atomics_util.cl");
  src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
  src_paths.push_back(source_dir + "fusion/fusion_kernels.cl");
  //compilation options
  const std::string& options = opts;
  //populate vector of kernels
  std::vector<bocl_kernel*> vec_kernels;
  //may need DIFF LIST OF SOURCES FOR
  auto* fuse = new bocl_kernel();
  std::string update_opts = options + " -D SURFACE_DENSITY_BASED";
  fuse->create_kernel(&device->context(), device->device_id(), src_paths, "fuse_blockwise_based_surface_density", update_opts, "fusion::fuse_blockwise_based_surface_density");
  vec_kernels.push_back(fuse);
  //store and return
  kernels_[identifier] = vec_kernels;
  return kernels_[identifier];
}
