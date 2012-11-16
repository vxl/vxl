// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_aggregate_normal_from_filter_vector_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process to interpolate the responses of first order derivatives filters into a normal
// \author Ali Osman Ulusoy
// \verbatim
//  Modifications
//   April 17, 2012  Isabel Restrepo: Take the vector of filters an input. This provides filter names, number and orientation
// \endverbatim
// \date Feb 13, 2011

//process utilities
#include <brdb/brdb_value.h>

//OpenCL
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <bocl/bocl_cl.h>

//boxm2
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>

//filters
#include <bvpl/kernels/bvpl_kernel.h>

//utilities
#include <vcl_fstream.h>
#include <vul/vul_timer.h>

namespace boxm2_ocl_aggregate_normal_from_filter_vector_process_globals
{
  const unsigned n_inputs_ =  4;
  const unsigned n_outputs_ = 0;

  typedef boxm2_data_traits<BOXM2_FLOAT> RESPONSE_DATATYPE;

  bool compile_kernel(bocl_device_sptr device, bocl_kernel*  aggregate_kernel ,vcl_string opts)
  {
    vcl_vector<vcl_string> src_paths;
    vcl_string source_dir = boxm2_ocl_util::ocl_src_root();
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "aggregate_filter_response.cl");

    return aggregate_kernel->create_kernel(&device->context(),device->device_id(), src_paths, "aggregate", opts, "aggregate");
  }

  static vcl_map<vcl_string, bocl_kernel* > kernels;
}

bool boxm2_ocl_aggregate_normal_from_filter_vector_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_aggregate_normal_from_filter_vector_process_globals;

  // process has 4 inputs and no outputs:
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "bvpl_kernel_vector_sptr";

  vcl_vector<vcl_string>  output_types_(n_outputs_);
  return pro.set_input_types(input_types_)
      && pro.set_output_types(output_types_);
}

bool boxm2_ocl_aggregate_normal_from_filter_vector_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_aggregate_normal_from_filter_vector_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The number of inputs should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;

  bocl_device_sptr device = pro.get_input<bocl_device_sptr>(i++);
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_opencl_cache_sptr opencl_cache = pro.get_input<boxm2_opencl_cache_sptr>(i++);
  bvpl_kernel_vector_sptr filter_vector = pro.get_input<bvpl_kernel_vector_sptr>(i++);
  unsigned num_filters = filter_vector->kernels_.size();

  //cache size sanity check
  long binCache = opencl_cache.ptr()->bytes_in_cache();
  vcl_cout<<"Update MBs in cache: "<<binCache/(1024.0*1024.0)<<vcl_endl;

  vcl_size_t local_threads[2]={8,8};
  vcl_size_t global_threads[2]={8,8};

  // create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue( device->context(),
                                                 *(device->device_id()),
                                                 CL_QUEUE_PROFILING_ENABLE,
                                                 &status);
  if (status!=0)
    return false;

  // compile the kernel if not already compiled
  vcl_string identifier=device->device_identifier();

  if (kernels.find(identifier)==kernels.end()) {
    vcl_cout<<"===========Compiling kernels==========="<<vcl_endl;
    bocl_kernel* aggregate_kernel = new bocl_kernel();
    if (num_filters == 3 ) {
      compile_kernel(device,aggregate_kernel,"-D XYZ");
      kernels[identifier]=aggregate_kernel;
    }
    else if (num_filters == 6 ) {
      compile_kernel(device,aggregate_kernel,"-D DODECAHEDRON");
      kernels[identifier]=aggregate_kernel;
    }
    else {
      vcl_cerr << "Aggregate kernel is not available for the requested number of responses\n";
      return false;
    }
  }

  // bit lookup buffer
  cl_uchar lookup_arr[256];
  boxm2_ocl_util::set_bit_lookup(lookup_arr);
  bocl_mem_sptr lookup=new bocl_mem(device->context(), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
  lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  // set up directions buffer
  cl_float4* directions = new cl_float4[num_filters];

  for (unsigned k = 0; k < num_filters; k++) {
    bvpl_kernel_sptr filter = filter_vector->kernels_[k];
    vnl_float_3 dir = filter->axis();
    dir.normalize();
    if ( vcl_abs(dir.magnitude() - 1.0f) > 1e-7 )
      vcl_cout << "Warning: In aggregate, direction doesn't have unit magnitude" << vcl_endl;
#if 0
    directions[k].s0 = dir[0];
    directions[k].s1 = dir[1];
    directions[k].s2 = dir[2];
    directions[k].s3 = 0.0f;
#endif
  }
  bocl_mem_sptr directions_buffer=new bocl_mem(device->context(), directions, sizeof(cl_float4)*num_filters, "directions buffer");
  directions_buffer->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  //timers
  float transfer_time=0.0f;
  float gpu_time=0.0f;

  vcl_map<boxm2_block_id, boxm2_block_metadata> blocks = scene->blocks();
  vcl_cout << "Running boxm2_ocl_aggregate_normal_from_filter_vector_process ..." << vcl_endl;

  //zip through each block
  vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;
  for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter)
  {
    boxm2_block_id id = blk_iter->first;
    vcl_cout << "Processing block: " << id << vcl_endl;

    //grab appropriate kernel
    bocl_kernel* kern = kernels[identifier];

    //load tree and alpha
    boxm2_block_metadata data = blk_iter->second;
    vul_timer transfer;
    /* bocl_mem* blk = */ opencl_cache->get_block(blk_iter->first);
    bocl_mem* blk_info  = opencl_cache->loaded_block_info();
    bocl_mem* alpha     = opencl_cache->get_data<BOXM2_ALPHA>(blk_iter->first,0,true);
    boxm2_scene_info* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
    int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
    info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);
    blk_info->write_to_buffer((queue));

    //store normals locations
    vcl_size_t normalsTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_NORMAL>::prefix());
    bocl_mem * normals    = opencl_cache->get_data(id,boxm2_data_traits<BOXM2_NORMAL>::prefix(), info_buffer->data_buffer_length*normalsTypeSize,false);

    //set global and local threads
    local_threads[0] = 128;
    local_threads[1] = 1;
    global_threads[0] = RoundUp((normals->num_bytes()/normalsTypeSize), local_threads[0]);
    global_threads[1]=1;

    kern->set_arg( blk_info );
    kern->set_arg( directions_buffer.ptr());
    kern->set_arg( normals );
    for (unsigned i = 0; i < num_filters; i++) {
      bvpl_kernel_sptr filter = filter_vector->kernels_[i];
      vcl_stringstream filter_ident; filter_ident << filter->name() << '_' << filter->id();
      bocl_mem * response = opencl_cache->get_data(id,RESPONSE_DATATYPE::prefix(filter_ident.str()), 0, true);
      kern->set_arg( response );
    }

    transfer_time += (float) transfer.all();

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

  vcl_cout<<"Gpu time "<<gpu_time<<" transfer time "<<transfer_time<<vcl_endl;

  return true;
}
