#include <iostream>
#include <fstream>
#include "boxm2_ocl_kernel_vector_filter.h"
#include <boct/boct_bit_tree.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/boxm2_block.h>
#include <vul/vul_timer.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

boxm2_ocl_kernel_vector_filter::
boxm2_ocl_kernel_vector_filter( const bocl_device_sptr& device,bool optimize_transfers) :
  device_(device),optimize_transfers_(optimize_transfers)
{
  compile_filter_kernel();
}


bool boxm2_ocl_kernel_vector_filter::compile_filter_kernel()
{
  std::vector<std::string> src_paths;
  std::string source_dir = boxm2_ocl_util::ocl_src_root();
  src_paths.push_back(source_dir + "scene_info.cl");
  src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(source_dir + "bit/kernel_filter_block.cl");

  //compilation options
  std::string options("");

  return kernel_.create_kernel(  &device_->context(), device_->device_id(),
                                 src_paths, "kernel_filter_block", options ,
                                 "boxm2 ocl kernel filter kernel");
}


bool boxm2_ocl_kernel_vector_filter::run(const boxm2_scene_sptr& scene, const boxm2_opencl_cache_sptr& opencl_cache, const bvpl_kernel_vector_sptr& filter_vector)
{
  float gpu_time=0.0f;

  // create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device_->context(),
                                                *(device_->device_id()),
                                                CL_QUEUE_PROFILING_ENABLE,&status);
  if (status!=0) {
    std::cerr<<"ERROR in initializing a queue\n";
    return false;
  }
  std::string identifier = device_->device_identifier();

  //cache size sanity check
  long binCache = opencl_cache->bytes_in_cache();
  std::cout<<"Filtering: Start MBs in cache: "<<binCache/(1024.0*1024.0)<<std::endl;

  // bit lookup buffer
  cl_uchar lookup_arr[256];
  boxm2_ocl_util::set_bit_lookup(lookup_arr);
  bocl_mem_sptr lookup=new bocl_mem(device_->context(), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
  lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  //center buffers
  bocl_mem_sptr centerX = new bocl_mem(device_->context(), boct_bit_tree::centerX, sizeof(cl_float)*585, "centersX lookup buffer");
  bocl_mem_sptr centerY = new bocl_mem(device_->context(), boct_bit_tree::centerY, sizeof(cl_float)*585, "centersY lookup buffer");
  bocl_mem_sptr centerZ = new bocl_mem(device_->context(), boct_bit_tree::centerZ, sizeof(cl_float)*585, "centersZ lookup buffer");
  centerX->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
  centerY->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
  centerZ->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  //cache size sanity check
  binCache = opencl_cache->bytes_in_cache();
  std::cout<<"Filtering: Bits and centers MBs in cache: "<<binCache/(1024.0*1024.0)<<std::endl;

  //iterate though the filters in the vector

  for (const auto& filter : filter_vector->kernels_)
  {
    std::stringstream filter_ident; filter_ident << filter->name() << '_' << filter->id();
    std::cout<<"Computing Filter: " << filter_ident.str() << " of size: " << filter->float_kernel_.size() <<std::endl;
    //filter->print();

    //set up the filter, filter buffer and other related filter variables
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
    bocl_mem * filter_buffer=new bocl_mem(device_->context(), filter_coeff, sizeof(cl_float4)*filter->float_kernel_.size(), "filter coefficient buffer");
    filter_buffer->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    unsigned int filter_size[1];
    filter_size[0]=filter->float_kernel_.size();
    bocl_mem_sptr filter_size_buffer = new bocl_mem(device_->context(), filter_size, sizeof(unsigned int), "filter_size buffer");
    filter_size_buffer->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

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

      //set up input data (currently hard-coded to be alpha)
      vul_timer transfer;
      bocl_mem* data_in = opencl_cache->get_data<BOXM2_ALPHA>(scene,id, 0, false);
      std::size_t dataSize = data_in->num_bytes();

      //set up output_data
      bocl_mem* filter_response = opencl_cache->get_data_new(scene,id, boxm2_data_traits<BOXM2_FLOAT>::prefix(filter_ident.str()), dataSize, false);

      //grab the block out of the cache as well
      bocl_mem* blk = opencl_cache->get_block(scene,id);
      bocl_mem* blk_info = opencl_cache->loaded_block_info();

      //set workspace
      std::size_t lThreads[] = {4, 4, 4};
      std::size_t gThreads[] = { RoundUp(data.sub_block_num_.x(), lThreads[0]),
        RoundUp(data.sub_block_num_.y(), lThreads[1]),
        RoundUp(data.sub_block_num_.z(), lThreads[2]) };

      binCache = opencl_cache->bytes_in_cache();
      std::cout<<"Filtering: Ready to execute MBs in cache: "<<binCache/(1024.0*1024.0)<<std::endl;

      //make it a reference so the destructor isn't called at the end...
      kernel_.set_arg( blk_info );
      kernel_.set_arg( blk );
      kernel_.set_arg( data_in );
      kernel_.set_arg( filter_response );
      kernel_.set_arg( filter_buffer );
      kernel_.set_arg( filter_size_buffer.ptr() );
      kernel_.set_arg( lookup.ptr() );
      kernel_.set_arg( centerX.ptr() );
      kernel_.set_arg( centerY.ptr() );
      kernel_.set_arg( centerZ.ptr() );
      kernel_.set_local_arg( lThreads[0]*lThreads[1]*lThreads[2]*sizeof(cl_uchar16) );  //local trees (uchar16 per local thread)
      kernel_.set_local_arg( lThreads[0]*lThreads[1]*lThreads[2]*sizeof(cl_uchar16) );  //neighbor trees (uchar16 per local thread)

      //execute kernel
      kernel_.execute( queue, 3, lThreads, gThreads);
      int status = clFinish(queue);
      if (!check_val(status, CL_SUCCESS, "KERNEL FILTER EXECUTE FAILED: " + error_to_string(status)))
        return false;

      gpu_time += kernel_.exec_time();

      //clear render kernel args so it can reset em on next execution
      kernel_.clear_args();

      if (!optimize_transfers_ ){
        //read filter response from gpu to cpu
        filter_response->read_to_buffer(queue);
        status = clFinish(queue);
        if (!check_val(status, CL_SUCCESS, "READ FILTER RESPONSE FAILED: " + error_to_string(status)))
          return false;

        //shallow remove from ocl cache unnecessary items from ocl cache.
        opencl_cache->shallow_remove_data(scene,id,boxm2_data_traits<BOXM2_FLOAT>::prefix(filter_ident.str()));
      }
      std::cout<<"Filtering: After execute MBs in cache: "<<binCache/(1024.0*1024.0)<<std::endl;
    }  //end block iter for

    delete [] filter_coeff;
    delete filter_buffer;
    std::cout<<"For filter: " << filter_ident.str() << "gpu_time:  " << gpu_time << " ms" <<std::endl;
  }
  return true;
}
