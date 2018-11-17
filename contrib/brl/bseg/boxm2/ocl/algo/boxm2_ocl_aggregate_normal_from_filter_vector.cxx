// This is brl/bseg/boxm2/ocl/algo/boxm2_ocl_aggregate_normal_from_filter_vector.cxx
#include "boxm2_ocl_aggregate_normal_from_filter_vector.h"
#include <fstream>
#include <iostream>
#include <map>
#include <stdexcept>
#include <utility>


#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>

//utilities
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_timer.h>


boxm2_ocl_aggregate_normal_from_filter_vector::
boxm2_ocl_aggregate_normal_from_filter_vector(const boxm2_scene_sptr& scene, const boxm2_opencl_cache_sptr& ocl_cache, const bocl_device_sptr& device, const bvpl_kernel_vector_sptr& filter_vector,bool optimize_transfers) :
  scene_(scene), ocl_cache_(ocl_cache), device_(device), filter_vector_(filter_vector),optimize_transfers_(optimize_transfers)
{
  unsigned num_filters = filter_vector->kernels_.size();

  std::cout<<"===========Compiling kernels==========="<<std::endl;
  std::cout << "device name = " << device->info().device_name_ << std::endl;
  bool status = false;
  if (num_filters == 3 ) {
    status = compile_kernel(kernel_, "-D XYZ");
  }
  else if (num_filters == 6 ) {
    status = compile_kernel(kernel_, "-D DODECAHEDRON");
  }
  else {
    std::cerr << "Aggregate kernel is not available for the requested number of responses\n";
    throw std::runtime_error("Unexpected number of filters");
  }
  if (!status) {
    throw std::runtime_error("ERROR compiling kernel in boxm2_ocl_aggregate_normal_from_filter_vector");

  }
}


bool boxm2_ocl_aggregate_normal_from_filter_vector::compile_kernel(bocl_kernel &aggregate_kernel ,std::string opts)
{
  std::vector<std::string> src_paths;
  std::string source_dir = boxm2_ocl_util::ocl_src_root();
  src_paths.push_back(source_dir + "scene_info.cl");
  src_paths.push_back(source_dir + "aggregate_filter_response.cl");

  return aggregate_kernel.create_kernel(&device_->context(),device_->device_id(), src_paths, "aggregate", std::move(opts), "aggregate");
}


bool boxm2_ocl_aggregate_normal_from_filter_vector::run(bool clear_cache)
{
  // dec: not sure why cache was being cleared, so default clear_cache to true to keep old behavior.
  if (clear_cache) {
    ocl_cache_->clear_cache();
    ocl_cache_->get_cpu_cache()->clear_cache();
  }

  long bytes_in_cache = ocl_cache_->bytes_in_cache();
  std::cout<<"MBs in cache: "<<bytes_in_cache/(1024.0*1024.0)<<std::endl;

  std::size_t local_threads[2]={8,8};
  std::size_t global_threads[2]={8,8};

  // create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue( device_->context(),
                                                 *(device_->device_id()),
                                                 CL_QUEUE_PROFILING_ENABLE,
                                                 &status);
  if (status!=0)
    return false;

  // bit lookup buffer
  cl_uchar lookup_arr[256];
  boxm2_ocl_util::set_bit_lookup(lookup_arr);
  bocl_mem_sptr lookup= ocl_cache_->alloc_mem(sizeof(cl_uchar)*256, lookup_arr, "bit lookup buffer");
  lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  // set up directions buffer (previous code tried to create an array on the stack with variable dimension tsk tsk)
  unsigned num_filters = filter_vector_->kernels_.size();
  auto* directions = new cl_float[4*num_filters];

  //for (unsigned k = 0; k < num_filters; k++) {
  for (unsigned k = 0, count = 0; k < num_filters; k++, count += 4) {
    bvpl_kernel_sptr filter = filter_vector_->kernels_[k];
    vnl_float_3 dir = filter->axis();
    dir.normalize();
    if ( std::abs(dir.magnitude() - 1.0f) > 1e-7 )
      std::cout << "Warning: In aggregate, direction doesn't have unit magnitude" << std::endl;

    directions[count+0] = dir[0];
    directions[count+1] = dir[1];
    directions[count+2] = dir[2];
    directions[count+3] = 0.0f;
  }
  bocl_mem_sptr directions_buffer= ocl_cache_->alloc_mem(sizeof(cl_float4)*num_filters, directions, "directions buffer");
  directions_buffer->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  //timers
  float transfer_time=0.0f;
  float gpu_time=0.0f;

  std::map<boxm2_block_id, boxm2_block_metadata> blocks = scene_->blocks();

  //zip through each block
  std::map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;
  for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter)
  {
    boxm2_block_id id = blk_iter->first;
    std::cout << "Processing block: " << id << std::endl;

    //load tree and alpha
    boxm2_block_metadata data = blk_iter->second;
    vul_timer transfer;
    ocl_cache_->get_block(scene_,blk_iter->first);
    bocl_mem* blk_info  = ocl_cache_->loaded_block_info();
    bocl_mem* alpha     = ocl_cache_->get_data<BOXM2_ALPHA>(scene_,blk_iter->first,0,true);
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

    //store normals locations
    std::size_t normalsTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_NORMAL>::prefix());
    // check for invalid parameters
    if( normalsTypeSize == 0 ) //This should never happen, it will result in division by zero later
    {
      std::cout << "ERROR: normalsTypeSize == 0 in " << __FILE__ << __LINE__ << std::endl;
      return false;
    }

    bocl_mem * normals    = ocl_cache_->get_data(scene_,id,boxm2_data_traits<BOXM2_NORMAL>::prefix(), info_buffer->data_buffer_length*normalsTypeSize,false);

    std::cout<<"MBs in cache: "<< (ocl_cache_->bytes_in_cache()/(1024.0*1024.0)) << std::endl;

    //set global and local threads
    local_threads[0] = 64;
    local_threads[1] = 1;
    global_threads[0] = RoundUp((normals->num_bytes()/normalsTypeSize), local_threads[0]);
    global_threads[1]=1;

    kernel_.set_arg( blk_info );
    kernel_.set_arg( directions_buffer.ptr());
    kernel_.set_arg( normals );
    for (unsigned i = 0; i < num_filters; i++) {
      bvpl_kernel_sptr filter = filter_vector_->kernels_[i];
      std::stringstream filter_ident;
      filter_ident << filter->name() << '_' << filter->id();
      std::string response_data_type = RESPONSE_DATATYPE::prefix(filter_ident.str());
      std::cout << "reponse_data_type = " << response_data_type << std::endl;
      bocl_mem * response = ocl_cache_->get_data(scene_,id, response_data_type, 0, true);
      kernel_.set_arg( response );
    }
    std::cout<<"MBs in cache: "<< (ocl_cache_->bytes_in_cache()/(1024.0*1024.0)) << std::endl;

    transfer_time += (float) transfer.all();

    //execute kernel
    bool good_exec = kernel_.execute(queue, 2, local_threads, global_threads);
    if (!good_exec) {
      std::cerr << "ERROR: boxm2_ocl_aggregate_normal_from_filter_vector::run() kernel_.execute() returned false" << std::endl;
    }
    int status = clFinish(queue);
    check_val(status, MEM_FAILURE, "AGGREGATE NORMAL EXECUTE FAILED: " + error_to_string(status));
    gpu_time += kernel_.exec_time();

    //clear render kernel args so it can reset em on next execution
    kernel_.clear_args();
    //read normals and vis from gpu
    if (!optimize_transfers_){
      normals->read_to_buffer(queue);
      status = clFinish(queue);
      check_val(status, MEM_FAILURE, "READ NORMALS FAILED: " + error_to_string(status));
    }
  }

  std::cout<<"Gpu time "<<gpu_time<<" transfer time "<<transfer_time<<std::endl;

  // these buffers should be cleared as soon as they go out of scope
  ocl_cache_->unref_mem(lookup.ptr());
  ocl_cache_->unref_mem(directions_buffer.ptr());
  delete [] directions;
  return true;
}
bool boxm2_ocl_aggregate_normal_from_filter_vector::reset(){
  int status=0;
  cl_command_queue queue = clCreateCommandQueue( device_->context(),
                                                 *(device_->device_id()),
                                                 CL_QUEUE_PROFILING_ENABLE,
                                                 &status);
  if (status!=0)
    return false;

  std::map<boxm2_block_id, boxm2_block_metadata> blocks = scene_->blocks();

  //zip through each block
  std::map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;
  for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter){
    boxm2_block_id id = blk_iter->first;
    bocl_mem * normals  = ocl_cache_->get_data(scene_,id,boxm2_data_traits<BOXM2_NORMAL>::prefix(),false);
    bocl_mem * alpha    = ocl_cache_->get_data(scene_,id,boxm2_data_traits<BOXM2_ALPHA >::prefix(),false);
    normals->zero_gpu_buffer(queue);
    alpha->write_to_buffer(queue);
    unsigned num_filters = filter_vector_->size();
    for (unsigned i = 0; i < num_filters; i++) {
      bvpl_kernel_sptr filter = filter_vector_->kernels_[i];
      std::stringstream filter_ident;
      filter_ident << filter->name() << '_' << filter->id();
      std::string response_data_type = RESPONSE_DATATYPE::prefix(filter_ident.str());
      bocl_mem * response = ocl_cache_->get_data(scene_,id, response_data_type, 0, true);
      response->zero_gpu_buffer(queue);
      status = clFinish(queue);
      check_val(status, MEM_FAILURE, "READ NORMALS FAILED: " + error_to_string(status));
    }
  }
  return true;
}
