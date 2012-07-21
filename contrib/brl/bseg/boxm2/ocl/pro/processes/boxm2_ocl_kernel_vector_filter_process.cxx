//:
// \file
// \brief A process to filter a boxm2 scene with a vectors of bvpl_kernels
// \author Isabel Restrepo
// \date April 12, 2012

#include "boxm2_ocl_kernel_vector_filter_process.h"
#include <boct/boct_bit_tree.h>

#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>

#include <vul/vul_timer.h>
#include <vcl_fstream.h>


bool boxm2_ocl_kernel_vector_filter_process_globals::compile_filter_kernel(bocl_device_sptr device, bocl_kernel * filter_kernel, vcl_string opts)
{
  vcl_vector<vcl_string> src_paths;
  vcl_string source_dir = boxm2_ocl_util::ocl_src_root();
  src_paths.push_back(source_dir + "scene_info.cl");
  src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(source_dir + "bit/kernel_filter_block.cl");

  //compilation options
  vcl_string options = opts;

  return filter_kernel->create_kernel(  &device->context(), device->device_id(),
                                        src_paths, "kernel_filter_block", options ,
                                        "boxm2 ocl kernel filter kernel");
}


bool boxm2_ocl_kernel_vector_filter_process_globals::process(bocl_device_sptr device, boxm2_scene_sptr scene, boxm2_opencl_cache_sptr opencl_cache, bvpl_kernel_vector_sptr filter_vector)
{
  float gpu_time=0.0f;

  // create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device->context(),
                                                *(device->device_id()),
                                                CL_QUEUE_PROFILING_ENABLE,&status);
  if (status!=0) {
    vcl_cerr<<"ERROR in initializing a queue\n";
    return false;
  }
  vcl_string identifier = device->device_identifier();

  // compile the kernel
  if (kernels.find(identifier)==kernels.end())
  {
    vcl_cout<<"===========Compiling kernels==========="<<vcl_endl;
    bocl_kernel* filter_kernel = new bocl_kernel();
    if (!compile_filter_kernel(device,filter_kernel, ""))
      return false;
    kernels[identifier]=filter_kernel;
  }

  //cache size sanity check
  long binCache = opencl_cache.ptr()->bytes_in_cache();
  vcl_cout<<"Filtering: Start MBs in cache: "<<binCache/(1024.0*1024.0)<<vcl_endl;

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

  //cache size sanity check
  binCache = opencl_cache.ptr()->bytes_in_cache();
  vcl_cout<<"Filtering: Bits and centers MBs in cache: "<<binCache/(1024.0*1024.0)<<vcl_endl;

  //iterate though the filters in the vector

  for (unsigned k= 0; k< filter_vector->kernels_.size(); k++)
  {
    bvpl_kernel_sptr filter = filter_vector->kernels_[k];

    vcl_stringstream filter_ident; filter_ident << filter->name() << '_' << filter->id();
    vcl_cout<<"Computing Filter: " << filter_ident.str() << " of size: " << filter->float_kernel_.size() <<vcl_endl;
    filter->print();

    //set up the filter, filter buffer and other related filter variables
    vcl_vector<vcl_pair<vgl_point_3d<float>, bvpl_kernel_dispatch> >::iterator kit = filter->float_kernel_.begin();
    unsigned ci=0;
    cl_float4* filter_coeff = new cl_float4 [filter->float_kernel_.size()];
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
    bocl_mem * filter_buffer=new bocl_mem(device->context(), filter_coeff, sizeof(cl_float4)*filter->float_kernel_.size(), "filter coefficient buffer");
    filter_buffer->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    unsigned int filter_size[1];
    filter_size[0]=filter->float_kernel_.size();
    bocl_mem_sptr filter_size_buffer = new bocl_mem(device->context(), filter_size, sizeof(unsigned int), "filter_size buffer");
    filter_size_buffer->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

    //iterate through all blocks
    vcl_map<boxm2_block_id, boxm2_block_metadata> blocks = scene->blocks();
    vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;
    for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter)
    {
      //clear cache
      opencl_cache->clear_cache();
      boxm2_block_metadata data = blk_iter->second;
      boxm2_block_id id = blk_iter->first;

      vcl_cout<<"Filtering Block"<< id << vcl_endl;

      //grab appropriate kernel
      bocl_kernel* kern = kernels[identifier];

      //set up input data (currently hard-coded to be alpha)
      vul_timer transfer;
      bocl_mem* data_in = opencl_cache->get_data<BOXM2_ALPHA>(id, 0, false);
      vcl_size_t dataSize = data_in->num_bytes();

      //set up output_data
      bocl_mem* filter_response = opencl_cache->get_data_new(id, boxm2_data_traits<BOXM2_FLOAT>::prefix(filter_ident.str()), dataSize, false);

      //grab the block out of the cache as well
      bocl_mem* blk = opencl_cache->get_block(id);
      bocl_mem* blk_info = opencl_cache->loaded_block_info();

      //set workspace
      vcl_size_t lThreads[] = {4, 4, 4};
      vcl_size_t gThreads[] = { RoundUp(data.sub_block_num_.x(), lThreads[0]),
        RoundUp(data.sub_block_num_.y(), lThreads[1]),
        RoundUp(data.sub_block_num_.z(), lThreads[2]) };

      binCache = opencl_cache.ptr()->bytes_in_cache();
      vcl_cout<<"Filtering: Ready to execute MBs in cache: "<<binCache/(1024.0*1024.0)<<vcl_endl;

      //make it a reference so the destructor isn't called at the end...
      kern->set_arg( blk_info );
      kern->set_arg( blk );
      kern->set_arg( data_in );
      kern->set_arg( filter_response );
      kern->set_arg( filter_buffer );
      kern->set_arg( filter_size_buffer.ptr() );
      kern->set_arg( lookup.ptr() );
      kern->set_arg( centerX.ptr() );
      kern->set_arg( centerY.ptr() );
      kern->set_arg( centerZ.ptr() );
      kern->set_local_arg( lThreads[0]*lThreads[1]*lThreads[2]*sizeof(cl_uchar16) );  //local trees (uchar16 per local thread)
      kern->set_local_arg( lThreads[0]*lThreads[1]*lThreads[2]*sizeof(cl_uchar16) );  //neighbor trees (uchar16 per local thread)

      //execute kernel
      kern->execute( queue, 3, lThreads, gThreads);
      int status = clFinish(queue);
      if (!check_val(status, CL_SUCCESS, "KERNEL FILTER EXECUTE FAILED: " + error_to_string(status)))
        return false;

      gpu_time += kern->exec_time();

      //clear render kernel args so it can reset em on next execution
      kern->clear_args();

      //read filter response from gpu to cpu
      filter_response->read_to_buffer(queue);
      status = clFinish(queue);
      if (!check_val(status, CL_SUCCESS, "READ FILTER RESPONSE FAILED: " + error_to_string(status)))
        return false;

      //shallow remove from ocl cache unnecessary items from ocl cache.
      vcl_cout<<"Filtering: After execute MBs in cache: "<<binCache/(1024.0*1024.0)<<vcl_endl;
      opencl_cache->shallow_remove_data(id,boxm2_data_traits<BOXM2_FLOAT>::prefix(filter_ident.str()));
    }  //end block iter for

    delete [] filter_coeff;
    delete filter_buffer;
    vcl_cout<<"For filter: " << filter_ident.str() << "gpu_time:  " << gpu_time << " ms" <<vcl_endl;
  }
  return true;
}

//:sets input and output types
bool boxm2_ocl_kernel_vector_filter_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_kernel_vector_filter_process_globals ;

  vcl_vector<vcl_string> input_types_(n_inputs_);
  unsigned i=0;
  input_types_[i++] = "bocl_device_sptr";
  input_types_[i++] = "boxm2_scene_sptr";
  input_types_[i++] = "boxm2_opencl_cache_sptr";
  input_types_[i++] = "bvpl_kernel_vector_sptr";

  vcl_vector<vcl_string> output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool boxm2_ocl_kernel_vector_filter_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_kernel_vector_filter_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  bocl_device_sptr device= pro.get_input<bocl_device_sptr>(i++);
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_opencl_cache_sptr opencl_cache= pro.get_input<boxm2_opencl_cache_sptr>(i++);
  bvpl_kernel_vector_sptr filter_vector = pro.get_input<bvpl_kernel_vector_sptr>(i++);

  vcl_cout<<"Using the following gpu device:\n" << *(device.ptr());

  bool status = process(device, scene, opencl_cache, filter_vector);

  return status;
}
