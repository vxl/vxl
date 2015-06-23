// This is brl/bseg/boxm2/ocl/algo/boxm2_ocl_aggregate_normal_from_filter_vector.cxx
#include "boxm2_ocl_aggregate_normal_from_filter_vector.h"

#include <vcl_stdexcept.h>
#include <vcl_map.h>
#include <vcl_iostream.h>

#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>

//utilities
#include <vcl_fstream.h>
#include <vul/vul_timer.h>


boxm2_ocl_aggregate_normal_from_filter_vector::
boxm2_ocl_aggregate_normal_from_filter_vector(boxm2_scene_sptr scene, boxm2_opencl_cache_sptr ocl_cache, bocl_device_sptr device, bvpl_kernel_vector_sptr filter_vector) :
  scene_(scene), ocl_cache_(ocl_cache), device_(device), filter_vector_(filter_vector)
{
  unsigned num_filters = filter_vector->kernels_.size();

  vcl_cout<<"===========Compiling kernels==========="<<vcl_endl;
  bool status = false;
  if (num_filters == 3 ) {
    status = compile_kernel(kernel_, "-D XYZ");
  }
  else if (num_filters == 6 ) {
    status = compile_kernel(kernel_, "-D DODECAHEDRON");
  }
  else {
    vcl_cerr << "Aggregate kernel is not available for the requested number of responses\n";
    throw vcl_runtime_error("Unexpected number of filters");
  }
  if (!status) {
    throw vcl_runtime_error("ERROR compiling kernel in boxm2_ocl_aggregate_normal_from_filter_vector");
    
  }
}


bool boxm2_ocl_aggregate_normal_from_filter_vector::compile_kernel(bocl_kernel &aggregate_kernel ,vcl_string opts)
{
  vcl_vector<vcl_string> src_paths;
  vcl_string source_dir = boxm2_ocl_util::ocl_src_root();
  src_paths.push_back(source_dir + "scene_info.cl");
  src_paths.push_back(source_dir + "aggregate_filter_response.cl");

  return aggregate_kernel.create_kernel(&device_->context(),device_->device_id(), src_paths, "aggregate", opts, "aggregate");
}


bool boxm2_ocl_aggregate_normal_from_filter_vector::run()
{

  vcl_size_t local_threads[2]={8,8};
  vcl_size_t global_threads[2]={8,8};

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
  bocl_mem_sptr lookup=new bocl_mem(device_->context(), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
  lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  // set up directions buffer
  //cl_float4* directions = new cl_float4[num_filters];
  unsigned num_filters = filter_vector_->kernels_.size();
  cl_float* directions = new float[4*num_filters];

  //for (unsigned k = 0; k < num_filters; k++) {
  for (unsigned k = 0, count = 0; k < num_filters; k++, count += 4) {
    bvpl_kernel_sptr filter = filter_vector_->kernels_[k];
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
    directions[count+0] = dir[0];
    directions[count+1] = dir[1];
    directions[count+2] = dir[2];
    directions[count+3] = 0.0f;
  }
  bocl_mem_sptr directions_buffer=new bocl_mem(device_->context(), directions, sizeof(cl_float4)*num_filters, "directions buffer");
  directions_buffer->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  //timers
  float transfer_time=0.0f;
  float gpu_time=0.0f;

  vcl_map<boxm2_block_id, boxm2_block_metadata> blocks = scene_->blocks();

  //zip through each block
  vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;
  for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter)
  {
    boxm2_block_id id = blk_iter->first;
    vcl_cout << "Processing block: " << id << vcl_endl;

    //load tree and alpha
    boxm2_block_metadata data = blk_iter->second;
    vul_timer transfer;
    ocl_cache_->get_block(scene_,blk_iter->first);
    bocl_mem* blk_info  = ocl_cache_->loaded_block_info();
    bocl_mem* alpha     = ocl_cache_->get_data<BOXM2_ALPHA>(scene_,blk_iter->first,0,true);
    boxm2_scene_info* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
    int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
    info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);
    blk_info->write_to_buffer((queue));

    //store normals locations
    vcl_size_t normalsTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_NORMAL>::prefix());
    bocl_mem * normals    = ocl_cache_->get_data(scene_,id,boxm2_data_traits<BOXM2_NORMAL>::prefix(), info_buffer->data_buffer_length*normalsTypeSize,false);

    //set global and local threads
    local_threads[0] = 128;
    local_threads[1] = 1;
    global_threads[0] = RoundUp((normals->num_bytes()/normalsTypeSize), local_threads[0]);
    global_threads[1]=1;

    kernel_.set_arg( blk_info );
    kernel_.set_arg( directions_buffer.ptr());
    kernel_.set_arg( normals );
    for (unsigned i = 0; i < num_filters; i++) {
      bvpl_kernel_sptr filter = filter_vector_->kernels_[i];
      vcl_stringstream filter_ident;
      filter_ident << filter->name() << '_' << filter->id();
      bocl_mem * response = ocl_cache_->get_data(scene_,id,RESPONSE_DATATYPE::prefix(filter_ident.str()), 0, true);
      kernel_.set_arg( response );
    }

    transfer_time += (float) transfer.all();

    //execute kernel
    bool good_exec = kernel_.execute(queue, 2, local_threads, global_threads);
    if (!good_exec) {
      vcl_cerr << "ERROR: boxm2_ocl_aggregate_normal_from_filter_vector::run() kernel_.execute() returned false" << vcl_endl;
    }
    int status = clFinish(queue);
    check_val(status, MEM_FAILURE, "AGGREGATE NORMAL EXECUTE FAILED: " + error_to_string(status));
    gpu_time += kernel_.exec_time();

    //clear render kernel args so it can reset em on next execution
    kernel_.clear_args();

    //read normals and vis from gpu
    normals->read_to_buffer(queue);
    status = clFinish(queue);
    check_val(status, MEM_FAILURE, "READ NORMALS FAILED: " + error_to_string(status));
  }

  vcl_cout<<"Gpu time "<<gpu_time<<" transfer time "<<transfer_time<<vcl_endl;

  return true;
}
