// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_filter_process.cxx
#include <iostream>
#include <fstream>
#include "boxm2_ocl_filter_process.h"
//:
// \file
// \brief  A process for rendering depth map of a scene.
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
#include <boct/boct_bit_tree.h>

void boxm2_ocl_filter_process_globals::compile_filter_kernel(const bocl_device_sptr& device,bocl_kernel * refine_data_kernel)
{
  std::vector<std::string> src_paths;
  std::string source_dir = boxm2_ocl_util::ocl_src_root();
  src_paths.push_back(source_dir + "scene_info.cl");
  src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(source_dir + "basic/linked_list.cl");
  src_paths.push_back(source_dir + "basic/sort_vector.cl");
  src_paths.push_back(source_dir + "bit/filter_block.cl");

  std::string options = " -D LIST_TYPE=float4 ";
  refine_data_kernel->create_kernel( &device->context(), device->device_id(),
                                     src_paths, "filter_block", options,
                                     "boxm2 ocl filter kernel");
}

bool boxm2_ocl_filter_process_cons(bprb_func_process& pro)
{
    using namespace boxm2_ocl_filter_process_globals;

    //process takes 1 input
    std::vector<std::string> input_types_(n_inputs_);
    input_types_[0] = "bocl_device_sptr";
    input_types_[1] = "boxm2_scene_sptr";
    input_types_[2] = "boxm2_opencl_cache_sptr";

    // process has 1 output:
    // output[0]: scene sptr
    std::vector<std::string>  output_types_(n_outputs_);

    return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_ocl_filter_process(bprb_func_process& pro)
{
    using namespace boxm2_ocl_filter_process_globals;

    if ( pro.n_inputs() < n_inputs_ ) {
        std::cout << pro.name() << ": The number of inputs should be " << n_inputs_<< std::endl;
        return false;
    }
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
        compile_filter_kernel(device,filter_kernel);
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

    //2. set workgroup size
    std::map<boxm2_block_id, boxm2_block_metadata> blocks = scene->blocks();
    std::map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;
    for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter)
    {
        //clear cache
        opencl_cache->clear_cache();
        boxm2_block_metadata data = blk_iter->second;
        boxm2_block_id id = blk_iter->first;

        std::cout<<"Filtering Block"<<id<<std::endl;

        //grab appropriate kernel
        bocl_kernel* kern = kernels[identifier];

        ////////////////////////////////////////////////////////////////////////////
        // Step One... currently mimics C++ implementation

        //get current and copy of alpha
        vul_timer transfer;
        bocl_mem* alphas = opencl_cache->get_data<BOXM2_ALPHA>(scene,id, 0, false);
        std::size_t dataSize = alphas->num_bytes();

        bocl_mem* new_alphas = new bocl_mem(device->context(), nullptr, dataSize, "new alpha buffer ");
        new_alphas->create_buffer(CL_MEM_READ_WRITE, queue);

        //grab the block out of the cache as well
        bocl_mem* blk = opencl_cache->get_block(scene,id);
        bocl_mem* blk_info = opencl_cache->loaded_block_info();


        //set workspace
        std::size_t lThreads[] = {4, 4, 4};
        std::size_t gThreads[] = { RoundUp(data.sub_block_num_.x(), lThreads[0]),
                                  RoundUp(data.sub_block_num_.y(), lThreads[1]),
                                  RoundUp(data.sub_block_num_.z(), lThreads[2]) };

        //make it a reference so the destructor isn't called at the end...
        kern->set_arg( blk_info );
        kern->set_arg( blk );
        kern->set_arg( alphas );
        kern->set_arg( new_alphas );
        kern->set_arg( lookup.ptr() );
        kern->set_arg( centerX.ptr() );
        kern->set_arg( centerY.ptr() );
        kern->set_arg( centerZ.ptr() );
        kern->set_local_arg( lThreads[0]*lThreads[1]*lThreads[2]*10*sizeof(cl_uchar) ); //cumsum (10 bytes per local thread
        kern->set_local_arg( lThreads[0]*lThreads[1]*lThreads[2]*sizeof(cl_uchar16) );  //local trees (uchar16 per local thread)
        kern->set_local_arg( lThreads[0]*lThreads[1]*lThreads[2]*sizeof(cl_uchar16) );  //neighbor trees (uchar16 per local thread)
        kern->set_local_arg( lThreads[0]*lThreads[1]*lThreads[2]*sizeof(cl_float4)*6 );  //local neighbor points 6*float4 per local thread)

        //execute kernel
        kern->execute( queue, 3, lThreads, gThreads);
        clFinish( queue);
        kern->clear_args();
        gpu_time += kern->exec_time();

        //write the data to buffer
        opencl_cache->deep_replace_data(scene,id, boxm2_data_traits<BOXM2_ALPHA>::prefix(), new_alphas);

    }  //end block iter for

    std::cout<<"Scene median filter time: "<<gpu_time<<" ms"<<std::endl;

    return true;
}
