// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_aggregate_normal_from_filter_response_process.cxx
#include <bprb/bprb_func_process.h>
#include <fstream>
#include <iostream>
#include <utility>
//:
// \file
// \brief A process to take in filter responses (from boxm2CppFilterResponseProcess) and aggregate them to a gradient direction.
// Currently, the cl code takes in 6 filter responses and the process supplies
// filter orientations to the cl code via a look-up table. It is hardcoded to
// take in dodecahedron face orientations currently.
// TODO: make filter orientations an input.
//
// \author Ali Osman Ulusoy
// \date Feb 13, 2011

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
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

#include <vul/vul_timer.h>

namespace boxm2_ocl_aggregate_normal_from_filter_process_globals
{
  constexpr unsigned n_inputs_ = 4;
  constexpr unsigned n_outputs_ = 0;

  //declare the response data type used to store in boxm2CppFilterResponseProcess.
  typedef boxm2_data_traits<BOXM2_FLOAT> RESPONSE_DATATYPE;

  void compile_kernel(const bocl_device_sptr& device,std::vector<bocl_kernel*> & vec_kernels,std::string opts)
  {
    std::vector<std::string> src_paths;
    std::string source_dir = boxm2_ocl_util::ocl_src_root();
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "aggregate_filter_response.cl");

    //compilation options
    const std::string& options = std::move(opts);

    auto* compute_vis = new bocl_kernel();
    std::string seg_opts = options + " -D DODECAHEDRON";
    compute_vis->create_kernel(&device->context(),device->device_id(), src_paths, "aggregate", seg_opts, "aggregate");
    vec_kernels.push_back(compute_vis);

    return ;
  }

  static std::map<std::string,std::vector<bocl_kernel*> > kernels;
}

bool boxm2_ocl_aggregate_normal_from_filter_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_aggregate_normal_from_filter_process_globals;

  // process has 4 inputs and no outputs:
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "unsigned";   //number of filters
  std::vector<std::string>  output_types_(n_outputs_);
  return pro.set_input_types(input_types_)
      && pro.set_output_types(output_types_);
}

bool boxm2_ocl_aggregate_normal_from_filter_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_aggregate_normal_from_filter_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;

  bocl_device_sptr         device = pro.get_input<bocl_device_sptr>(i++);
  boxm2_scene_sptr         scene = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_opencl_cache_sptr  opencl_cache = pro.get_input<boxm2_opencl_cache_sptr>(i++);
  auto num_kernels = pro.get_input<unsigned>(i++);

  //cache size sanity check
  long binCache = opencl_cache.ptr()->bytes_in_cache();
  std::cout<<"Update MBs in cache: "<<binCache/(1024.0*1024.0)<<std::endl;

  std::size_t local_threads[2]={8,8};
  std::size_t global_threads[2]={8,8};

  // create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue( device->context(),
                                                 *(device->device_id()),
                                                 CL_QUEUE_PROFILING_ENABLE,
                                                 &status);
  if (status!=0)
    return false;

  // compile the kernel if not already compiled
  std::string identifier=device->device_identifier();
  if (kernels.find(identifier)==kernels.end()) {
    std::cout<<"===========Compiling kernels==========="<<std::endl;
    std::vector<bocl_kernel*> ks;
    compile_kernel(device,ks,"");
    kernels[identifier]=ks;
  }

  // bit lookup buffer
  cl_uchar lookup_arr[256];
  boxm2_ocl_util::set_bit_lookup(lookup_arr);
  bocl_mem_sptr lookup=new bocl_mem(device->context(), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
  lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  // dodecahedron orientations lookup buffer
  cl_float4 dodecahedron_dir[6];
  boxm2_ocl_util::set_dodecahedron_orientations_lookup(dodecahedron_dir);
  bocl_mem_sptr dodecahedron_dir_lookup=new bocl_mem(device->context(), dodecahedron_dir, sizeof(cl_float4)*6, "dodecahedron directions lookup buffer");
  dodecahedron_dir_lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  //timers
  float transfer_time=0.0f;
  float gpu_time=0.0f;

  std::map<boxm2_block_id, boxm2_block_metadata> blocks = scene->blocks();
  std::cout << "Running boxm2_ocl_aggregate_normal_from_filter_process ..." << std::endl;

  //zip through each block
  std::map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;
  for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter)
  {
    boxm2_block_id id = blk_iter->first;
    std::cout << "Processing block: " << id << std::endl;

    //get kernel
    bocl_kernel* kern =  kernels[identifier][0];

    //load tree and alpha
    boxm2_block_metadata data = blk_iter->second;
    vul_timer transfer;
    /* bocl_mem* blk = */ opencl_cache->get_block(scene,blk_iter->first);
    bocl_mem* blk_info = opencl_cache->loaded_block_info();
    bocl_mem* alpha = opencl_cache->get_data<BOXM2_ALPHA>(scene,blk_iter->first,0,true);
    auto* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
    int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
    info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);
    blk_info->write_to_buffer((queue));

    //store normals locations
    std::size_t normalsTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_NORMAL>::prefix());
    bocl_mem * normals = opencl_cache->get_data(scene,id,boxm2_data_traits<BOXM2_NORMAL>::prefix(), info_buffer->data_buffer_length*normalsTypeSize,false);

#if 0 // unused
    //get response type
    std::size_t responseTypeSize = boxm2_data_info::datasize(RESPONSE_DATATYPE::prefix());
#endif

    transfer_time += (float) transfer.all();

    //set global and local threads
    local_threads[0] = 128;
    local_threads[1] = 1;
    global_threads[0] = RoundUp((normals->num_bytes()/normalsTypeSize), local_threads[0]);
    global_threads[1]=1;

    kern->set_arg( blk_info );
    kern->set_arg( dodecahedron_dir_lookup.ptr());
    kern->set_arg( normals );
    for (unsigned i = 0; i < num_kernels; i++) {
      std::stringstream ss; ss << i;
      bocl_mem * response = opencl_cache->get_data(scene,id,RESPONSE_DATATYPE::prefix(ss.str()), 0, true);
      kern->set_arg( response );
    }

    //execute kernel
    kern->execute(queue, 2, local_threads, global_threads);
    int status = clFinish(queue);
    check_val(status, MEM_FAILURE, "AGGREGATE NORMAL EXECUTE FAILED: " + error_to_string(status));
    gpu_time += kern->exec_time();

    //clear render kernel args so it can reset em on next execution
    kern->clear_args();

    //read normals and vis from gpu
    normals->read_to_buffer(queue);
    status = clFinish(queue);
    check_val(status, MEM_FAILURE, "READ NORMALS FAILED: " + error_to_string(status));
  }
  return true;
}
