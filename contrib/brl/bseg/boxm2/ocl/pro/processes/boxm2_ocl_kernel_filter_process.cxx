//:
// \file
// \brief A process to filter a boxm2 scene with a bvpl_kernel
// \author Isabel Restrepo
// \date April 12, 2012

#include "boxm2_ocl_kernel_filter_process.h"
#include <boct/boct_bit_tree.h>
#include <fstream>
#include <iostream>
#include <utility>

#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>

#include <vul/vul_timer.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


bool boxm2_ocl_kernel_filter_process_globals::compile_filter_kernel(const bocl_device_sptr& device, bocl_kernel * filter_kernel, std::string opts)
{
  std::vector<std::string> src_paths;
  std::string source_dir = boxm2_ocl_util::ocl_src_root();
  src_paths.push_back(source_dir + "scene_info.cl");
  src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(source_dir + "bit/kernel_filter_block.cl");

  //compilation options
  const std::string& options = std::move(opts);

  return filter_kernel->create_kernel(  &device->context(), device->device_id(),
                                        src_paths, "kernel_filter_block", options ,
                                        "boxm2 ocl kernel filter kernel");
}


bool boxm2_ocl_kernel_filter_process_globals::process(const bocl_device_sptr& device, const boxm2_scene_sptr& scene, const boxm2_opencl_cache_sptr& opencl_cache, const bvpl_kernel_sptr& filter)
{
  float gpu_time=0.0f;

  // create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device->context(),
                                                *(device->device_id()),
                                                CL_QUEUE_PROFILING_ENABLE,&status);
  if (status!=0) {
    std::cerr<<"ERROR in initializing a queue\n";
    return false;
  }
  std::string identifier = device->device_identifier();

  // compile the kernel
  if (kernels.find(identifier)==kernels.end())
  {
    std::cout<<"===========Compiling kernels==========="<<std::endl;
    auto* filter_kernel = new bocl_kernel();
    if (!compile_filter_kernel(device,filter_kernel, ""))
      return false;
    kernels[identifier]=filter_kernel;
  }

  // bit lookup buffer
  cl_uchar lookup_arr[256];
  boxm2_ocl_util::set_bit_lookup(lookup_arr);
  bocl_mem_sptr lookup=new bocl_mem(device->context(), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
  lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  //center buffers
  bocl_mem_sptr centerX = new bocl_mem(device->context(), boct_bit_tree::centerX, sizeof(cl_float)*585, "centersX lookup buffer");
  bocl_mem_sptr centerY = new bocl_mem(device->context(), boct_bit_tree::centerY, sizeof(cl_float)*585, "centersY lookup buffer");
  bocl_mem_sptr centerZ = new bocl_mem(device->context(), boct_bit_tree::centerZ, sizeof(cl_float)*585, "centersZ lookup buffer");
  centerX->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
  centerY->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
  centerZ->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  //set up the filter and filter buffer
  auto kit = filter->float_kernel_.begin();
  unsigned ci=0;
  auto* filter_coeff = new cl_float4 [filter->float_kernel_.size()];
  for (; kit!= filter->float_kernel_.end(); kit++, ci++)
  {
    vgl_point_3d<float> loc = kit->first;
    float w = kit->second.c_;
#ifdef CL_ALIGNED
    filter_coeff[ci].s[0] = loc.x();
    filter_coeff[ci].s[1] = loc.y();
    filter_coeff[ci].s[2] = loc.z();
    filter_coeff[ci].s[3] = w;
#else // assuming cl_float4 is a typedef for float[4]
    float* f_c = static_cast<float*>(filter_coeff[ci]);
    f_c[0] = loc.x();
    f_c[1] = loc.y();
    f_c[2] = loc.z();
    f_c[3] = w;
#endif // CL_ALIGNED
  }
  bocl_mem_sptr filter_buffer=new bocl_mem(device->context(), filter_coeff, sizeof(cl_float4)*filter->float_kernel_.size(), "filter coefficient buffer");
  filter_buffer->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  unsigned int filter_size[1];
  filter_size[0]=filter->float_kernel_.size();
  bocl_mem_sptr filter_size_buffer = new bocl_mem(device->context(), filter_size, sizeof(unsigned int), "filter_size buffer");
  filter_size_buffer->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //iterate through all blocks
  std::map<boxm2_block_id, boxm2_block_metadata> blocks = scene->blocks();
  std::map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;
  for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter)
  {
    //clear cache
    opencl_cache->clear_cache();
    boxm2_block_metadata data = blk_iter->second;
    boxm2_block_id id = blk_iter->first;

    std::cout<<"Filtering Block"<< id << std::endl;

    //grab appropriate kernel
    bocl_kernel* kern = kernels[identifier];

    //set up input data (currently hard-coded to be alpha)
    vul_timer transfer;
    bocl_mem* data_in = opencl_cache->get_data<BOXM2_ALPHA>(scene, id, 0, false);
    std::size_t dataSize = data_in->num_bytes();

    //set up output_data
    std::stringstream filter_ident; filter_ident << filter->name() << '_' << filter->id();
    bocl_mem* filter_response = opencl_cache->get_data_new(scene, id, boxm2_data_traits<BOXM2_FLOAT>::prefix(filter_ident.str()), dataSize, false);

    //grab the block out of the cache as well
    bocl_mem* blk = opencl_cache->get_block(scene, id);
    bocl_mem* blk_info = opencl_cache->loaded_block_info();

    //set workspace
    std::size_t lThreads[] = {4, 4, 4};
    std::size_t gThreads[] = { RoundUp(data.sub_block_num_.x(), lThreads[0]),
                              RoundUp(data.sub_block_num_.y(), lThreads[1]),
                              RoundUp(data.sub_block_num_.z(), lThreads[2]) };

    //make it a reference so the destructor isn't called at the end...
    kern->set_arg( blk_info );
    kern->set_arg( blk );
    kern->set_arg( data_in );
    kern->set_arg( filter_response );
    kern->set_arg( filter_buffer.ptr() );
    kern->set_arg( filter_size_buffer.ptr() );
    kern->set_arg( lookup.ptr() );
    kern->set_arg( centerX.ptr() );
    kern->set_arg( centerY.ptr() );
    kern->set_arg( centerZ.ptr() );
    kern->set_local_arg( lThreads[0]*lThreads[1]*lThreads[2]*sizeof(cl_uchar16) );  //local trees (uchar16 per local thread)
    kern->set_local_arg( lThreads[0]*lThreads[1]*lThreads[2]*sizeof(cl_uchar16) );  //neighbor trees (uchar16 per local thread)

    //execute kernel
    kern->execute( queue, 3, lThreads, gThreads);
    int status = clFinish( queue);
    if (!check_val(status, CL_SUCCESS, "KERNEL FILTER EXECUTE FAILED: " + error_to_string(status)))
      return false;

    gpu_time += kern->exec_time();

    //clear render kernel args so it can reset em on next execution
    kern->clear_args();

    //read filter response from gpu
    filter_response->read_to_buffer(queue);
    status = clFinish(queue);
    if (!check_val(status, MEM_FAILURE, "READ FILTER RESPONSE FAILED: " + error_to_string(status)))
      return false;

    //shallow remove from ocl cache unnecessary items from ocl cache.
    opencl_cache->shallow_remove_data(scene,id,boxm2_data_traits<BOXM2_FLOAT>::prefix(filter_ident.str()));
  }  //end block iter for
  delete [] filter_coeff;

  std::cout<<"Scene kernel filter time: "<< gpu_time <<" ms"<<std::endl;

  return true;
}

//:sets input and output types
bool boxm2_ocl_kernel_filter_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_kernel_filter_process_globals ;

  std::vector<std::string> input_types_(n_inputs_);
  unsigned i=0;
  input_types_[i++] = "bocl_device_sptr";
  input_types_[i++] = "boxm2_scene_sptr";
  input_types_[i++] = "boxm2_opencl_cache_sptr";
  input_types_[i++] = "bvpl_kernel_sptr";

  std::vector<std::string> output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool boxm2_ocl_kernel_filter_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_kernel_filter_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  bocl_device_sptr device= pro.get_input<bocl_device_sptr>(i++);
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_opencl_cache_sptr opencl_cache= pro.get_input<boxm2_opencl_cache_sptr>(i++);
  bvpl_kernel_sptr filter = pro.get_input<bvpl_kernel_sptr>(i++);

  bool status = process(device, scene, opencl_cache, filter);

  return status;
}
