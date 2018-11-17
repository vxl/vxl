// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_batch_probability_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for updating the histogram given an image.
//
// \author Vishal Jain
// \date Mar 10, 2011

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vul/vul_timer.h>
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

namespace boxm2_ocl_batch_probability_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 0;
  void compile_kernel(const bocl_device_sptr& device,std::vector<bocl_kernel*> & vec_kernels)
  {
    std::vector<std::string> src_paths;
    std::string source_dir = boxm2_ocl_util::ocl_src_root();
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "cell_utils.cl");
    src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(source_dir + "backproject.cl");
    src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
    src_paths.push_back(source_dir + "bit/batchkernels.cl");

    //compilation options

    auto* update_prob = new bocl_kernel();
    std::string prob_opts = " -D UPDATE_PROB ";

    update_prob->create_kernel(&device->context(), device->device_id(), src_paths, "update_prob_main", prob_opts, "update::update_prob");
    vec_kernels.push_back(update_prob);
  }
  static std::map<cl_device_id*,std::vector<bocl_kernel*> > kernels;
}

bool boxm2_ocl_batch_probability_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_batch_probability_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";


  std::vector<std::string>  output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_ocl_batch_probability_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_batch_probability_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  float transfer_time=0.0f;
  float gpu_time=0.0f;
  //get the inputs
  unsigned i = 0;
  bocl_device_sptr device= pro.get_input<bocl_device_sptr>(i++);
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_opencl_cache_sptr opencl_cache= pro.get_input<boxm2_opencl_cache_sptr>(i++);
  // create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device->context(),
                                                *(device->device_id()),
                                                CL_QUEUE_PROFILING_ENABLE,
                                                &status);
  if (status!=0) return false;
  // compile the kernel
  if (kernels.find((device->device_id()))==kernels.end())
  {
    std::cout<<"===========Compiling kernels==========="<<std::endl;
    std::vector<bocl_kernel*> ks;
    compile_kernel(device,ks);
    kernels[(device->device_id())]=ks;
  }
  // create all buffers
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

  //2. set workgroup size

  // set arguments
  std::vector<boxm2_block_id> vis_order = scene->get_block_ids();
  std::vector<boxm2_block_id>::iterator id;
  for (id = vis_order.begin(); id != vis_order.end(); ++id)
  {
    //choose correct render kernel
    boxm2_block_metadata mdata = scene->get_block_metadata(*id);
    bocl_kernel* kern =  kernels[(device->device_id())][0];
    //write the image values to the buffer
    vul_timer transfer;
    bocl_mem * blk = opencl_cache->get_block(scene,*id);
    std::cout<<" Block Num: "<<(*id)<<std::endl;
    std::cout.flush();
    bocl_mem * blk_info = opencl_cache->loaded_block_info();
    bocl_mem * alpha = opencl_cache->get_data<BOXM2_ALPHA>(scene,*id);
    bocl_mem * hist = opencl_cache->get_data<BOXM2_BATCH_HISTOGRAM>(scene,*id);
    bocl_mem * intensity = opencl_cache->get_data<BOXM2_MOG3_GREY>(scene,*id);

    transfer_time += (float) transfer.all();

    ////3. SET args
    kern->set_arg( blk_info );
    kern->set_arg( lookup.ptr() );
    kern->set_arg( blk );
    kern->set_arg( alpha );
    kern->set_arg( intensity );
    kern->set_arg( hist );
    kern->set_arg( cl_output.ptr() );
    std::size_t lThreads[] = {1, 1};

    //local tree , cumsum buffer
    kern->set_local_arg( lThreads[0]*lThreads[1]*sizeof(cl_uchar16) );
    kern->set_local_arg( lThreads[0]*lThreads[1]*10*sizeof(cl_uchar) );

    // TOFIX:::note that this for fixed grid.
    auto* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();

    std::size_t numtrees = info_buffer->scene_dims[0]*
                          info_buffer->scene_dims[1]*
                          info_buffer->scene_dims[2];
    std::size_t gThreads[] = {numtrees,1};
    //execute kernel
    kern->execute(queue, 2, lThreads, gThreads);
    clFinish(queue);
    gpu_time += kern->exec_time();

    //clear render kernel args so it can reset em on next execution
    kern->clear_args();

    alpha->read_to_buffer(queue);
    hist->read_to_buffer(queue);
    intensity->read_to_buffer(queue);
  }
  clReleaseCommandQueue(queue);
  return true;
}
