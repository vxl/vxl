// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_merge_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
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
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#define ALPHA_BEHAVIOR 0
#define COPY_PARENT_BEHAVIOR 1
#define COPY_INIT_BEHAVIOR 2
#define ZERO_BEHAVIOR 3


namespace boxm2_ocl_merge_process_globals
{
    constexpr unsigned n_inputs_ = 4;
    constexpr unsigned n_outputs_ = 0;
    void compile_merge_tree_kernel(const bocl_device_sptr& device, bocl_kernel* merge_kernel)
    {
        //gather all render sources... seems like a lot for rendering...
        std::vector<std::string> src_paths;
        std::string source_dir = boxm2_ocl_util::ocl_src_root();
        src_paths.push_back(source_dir + "scene_info.cl");
        src_paths.push_back(source_dir + "basic/linked_list.cl");
        src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
        src_paths.push_back(source_dir + "bit/merge_bit_scene.cl");

        //create refine trees kernel (refine trees deterministic.  MOG type is necessary
        // to define, but not used by the kernel - using default value here
        merge_kernel->create_kernel( &device->context(),
                                     device->device_id(),
                                     src_paths,
                                     "merge_trees",
                                     " -D MOG_TYPE_8 ",
                                     "boxm2 opencl merge trees (pass one)"); //kernel identifier (for error checking)
    }
    void compile_merge_data_kernel(const bocl_device_sptr& device, bocl_kernel* merge_data_kernel, const std::string& option)
    {
        std::vector<std::string> src_paths;
        std::string source_dir = boxm2_ocl_util::ocl_src_root();
        src_paths.push_back(source_dir + "scene_info.cl");
        src_paths.push_back(source_dir + "basic/linked_list.cl");
        src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
        src_paths.push_back(source_dir + "bit/merge_bit_scene.cl");
        merge_data_kernel->create_kernel( &device->context(),
                                           device->device_id(),
                                           src_paths,
                                           "merge_data",
                                           option,
                                           "boxm2 opencl merge data: " + option);
    }
    std::string get_option_string(int datasize)
    {
        std::string options="";
        switch (datasize)
        {
          case 2:
            options= "-D MOG_TYPE_2 ";break;
          case 4:
            options= "-D MOG_TYPE_4 ";break;
          case 6:
            options= "-D MOG_TYPE_6 ";break;
          case 8:
            options= "-D MOG_TYPE_8 ";break;
          case 16:
            options= "-D MOG_TYPE_16 ";break;
          default:
            break;
        }
        return options;
    }

    //map of compiled kernels, organized by data type
    static std::map<std::string,bocl_kernel* > kernels;
}

bool boxm2_ocl_merge_process_cons(bprb_func_process& pro)
{
    using namespace boxm2_ocl_merge_process_globals;

    //process takes 1 input
    std::vector<std::string> input_types_(n_inputs_);
    input_types_[0] = "bocl_device_sptr";
    input_types_[1] = "boxm2_scene_sptr";
    input_types_[2] = "boxm2_opencl_cache_sptr";
    input_types_[3] = "float";

    // process has 1 output:
    // output[0]: scene sptr
    std::vector<std::string>  output_types_(n_outputs_);
    return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_ocl_merge_process(bprb_func_process& pro)
{
    using namespace boxm2_ocl_merge_process_globals;
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
    auto thresh  =pro.get_input<float>(i++);

    std::string identifier=device->device_identifier();
    // create a command queue.
    int status=0;
    cl_command_queue queue = clCreateCommandQueue(device->context(),
                                                  *(device->device_id()),
                                                  CL_QUEUE_PROFILING_ENABLE,&status);
    if (status!=0) {
        std::cout<<" ERROR in initializing a queue"<<std::endl;
        return false;
    }

    //set tree identifier and compile (indexes into merge tree kernel)
    std::string tree_identifier=identifier+"tree";
    if (kernels.find(tree_identifier)==kernels.end())
    {
        std::cout<<"===========Compiling kernels==========="<<std::endl;
        auto * tree_kernel=new bocl_kernel();
        compile_merge_tree_kernel(device,tree_kernel);
        kernels[tree_identifier]=tree_kernel;
    }

    //compile the move data kernel for each data type
    std::vector<std::string> data_types = scene->appearances();
    data_types.push_back(boxm2_data_traits<BOXM2_ALPHA>::prefix());
    for (const auto & data_type : data_types)
    {
        std::string options=get_option_string(boxm2_data_info::datasize(data_type));
        std::string data_identifier= identifier+options;
        if (kernels.find(data_identifier)==kernels.end())
        {
            auto * data_kernel=new bocl_kernel();
            compile_merge_data_kernel(device,data_kernel,options);
            kernels[data_identifier]=data_kernel;
        }
    }

    //set up buffers
    float prob_buff[1];
    prob_buff[0]=thresh;
    bocl_mem_sptr prob_thresh = new bocl_mem(device->context(), prob_buff, sizeof(float), "prob_thresh buffer");
    prob_thresh->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
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
    opencl_cache->clear_cache();
    std::map<boxm2_block_id, boxm2_block_metadata> blocks = scene->blocks();
    std::map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;
    for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter)
    {
        boxm2_block_id currId = blk_iter->first;
        std::cout<<"Merging block "<<currId<<std::endl;
        // Merge Method Summary:
        //  - NEED TO CLEAR OUT THE GPU CACHE BEFORE YOU START.. so you don't overwrite stuff accidentally...
        //  - Create Block Copy, refine trees into that copy, maintaining old copy and array of new tree sizes
        //  - Do scan on size vector (cum sum)
        //  - Swap data into new buffers: For each data type
        //    - get BOCL_MEM* data independent of cpu pointer (from cache)
        //    - remove the BOCL_MEM* from the gpu cache (don't delete it)
        //    - do a deep delete (delete CPU buffer from CPU cache)
        //    - get a new data pointer (with newSize), will create CPU buffer and GPU buffer
        //    - Run refine_data_kernel with the two buffers
        //    - delete the old BOCL_MEM*, and that's it...

        //clear cache
        opencl_cache->clear_cache();
        boxm2_block_metadata data = blk_iter->second;
        bocl_kernel* kern=kernels[tree_identifier];

        ////////////////////////////////////////////////////////////////////////////
        // Step One... currently mimics C++ implementation
        //get id and refine block into tree copy, and calc vector of new tree sizes
        boxm2_block_id id = blk_iter->first;
        int numTrees = data.sub_block_num_.x() * data.sub_block_num_.y() * data.sub_block_num_.z();

        //set up tree copy
        std::cout<<"  creating tree copy"<<std::endl;
        bocl_mem_sptr blk_copy = new bocl_mem(device->context(), nullptr, numTrees*sizeof(cl_uchar16), "merge trees block copy buffer");
        blk_copy->create_buffer(CL_MEM_READ_WRITE| CL_MEM_ALLOC_HOST_PTR, (queue));

        //set up tree size (first find num trees)
        std::cout<<"  creating tree sizes buff"<<std::endl;
        bocl_mem_sptr tree_sizes = new bocl_mem(device->context(), nullptr, sizeof(cl_int)*numTrees, "merge tree sizes buffer");
        tree_sizes->create_buffer(CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, (queue));
        clFinish((queue));

        //write the image values to the buffer
        vul_timer transfer;
        bocl_mem* blk = opencl_cache->get_block(scene,id);
        bocl_mem* alpha = opencl_cache->get_data<BOXM2_ALPHA>(scene,id);
        bocl_mem* blk_info = opencl_cache->loaded_block_info();
        transfer_time += (float) transfer.all();
        std::size_t lThreads[] = {64, 1};
        std::size_t gThreads[] = {RoundUp(numTrees,lThreads[0]), 1};
        float alphasize=(float)alpha->num_bytes()/1024/1024;
        if (alphasize >= (float)blk_iter->second.max_mb_/10.0) {
            std::cout<<"  Merge STOP !!!"<<std::endl;
            continue;
        }

        //set first kernel args
        kern->set_arg( blk_info );
        kern->set_arg( blk );
        kern->set_arg( blk_copy.ptr() );
        kern->set_arg( alpha );
        kern->set_arg( tree_sizes.ptr() );
        kern->set_arg( prob_thresh.ptr() );
        kern->set_arg( lookup.ptr() );
        kern->set_arg( cl_output.ptr() );
        kern->set_local_arg( lThreads[0]*16*sizeof(cl_uchar) );
        kern->set_local_arg( lThreads[0]*sizeof(cl_uchar16) );
        kern->set_local_arg( lThreads[0]*sizeof(cl_uchar16) );
        kern->set_local_arg( lThreads[0]*73*sizeof(cl_char) );

        //execute kernel
        kern->execute( queue, 2, lThreads, gThreads);
        clFinish(queue);
        gpu_time += kern->exec_time();

        //clear render kernel args so it can reset em on next execution
        kern->clear_args();
        blk_copy->read_to_buffer(queue);

        /////////////////////////////////////////////////////////////////////////
        //STEP TWO
        //read out tree_sizes and do cumulative sum on it
        vul_timer scan_time;
        tree_sizes->read_to_buffer(queue);
        int* sizebuff = (int*) tree_sizes->cpu_buffer();
        for (int i=1; i<numTrees; ++i)
            sizebuff[i] += sizebuff[i-1];
        int newDataSize = sizebuff[numTrees-1];
        for (int i=numTrees-1; i>0; --i)
            sizebuff[i] = sizebuff[i-1];
        sizebuff[0] = 0;
        tree_sizes->write_to_buffer((queue));

        int dataLen = alpha->num_bytes()/sizeof(float);
        std::cout<<"  New data size: "<<newDataSize<<", old data: "<<dataLen<<'\n'
                <<"  Num Merged: "<<(dataLen-newDataSize)/8<<'\n'
                <<"  Scan data sizes time: "<<scan_time.all()<<std::endl;
        transfer_time += scan_time.all();
        /////////////////////////////////////////////////////////////////////////

        /////////////////////////////////////////////////////////////////////////
        //STEP Three
        //  - Swap data into new buffers: For each data type
        //    - get BOCL_MEM* data independent of cpu pointer (from cache)
        //    - remove the BOCL_MEM* from the gpu cache (don't delete it)
        //    - do a deep delete (delete CPU buffer from CPU cache)
        //    - get a new data pointer (with newSize), will create CPU buffer and GPU buffer
        //    - Run refine_data_kernel with the two buffers
        //    - delete the old BOCL_MEM*, and that's it...
        // POSSIBLE PROBLEMS: data may not exist in cache and may need to be initialized...
        //this vector will be passed in (listing data types to refine)
        std::vector<std::string> data_types = scene->appearances();
        data_types.push_back(boxm2_data_traits<BOXM2_ALPHA>::prefix());
        for (const auto & data_type : data_types)
        {
            std::cout<<"  Swapping data of type: "<<data_type<<std::endl;
            std::string options=get_option_string(boxm2_data_info::datasize(data_type));
            std::string data_identifier= identifier+options;
            if (kernels.find(data_identifier)==kernels.end())
            {
                std::cout<<"  boxm2_opencl_refine::Kernel for swapping datatype: "
                        <<data_type<<" and size "<<boxm2_data_info::datasize(data_type)
                        <<" is not compiled."<<std::endl;
                return false;
            }
            bocl_kernel* kern=kernels[data_identifier];

            //get bocl_mem data independent of CPU pointer
            bocl_mem* dat = opencl_cache->get_data(scene,id, data_type);

            //get a new data pointer (with newSize), will create CPU buffer and GPU buffer
            std::cout<<"  new data size of "<<data_type<<" is "<<newDataSize<<std::endl;
            int dataBytes = boxm2_data_info::datasize(data_type) * newDataSize;
            bocl_mem* new_dat = new bocl_mem(device->context(), nullptr, dataBytes, "new data buffer " + data_type);
            new_dat->create_buffer(CL_MEM_READ_WRITE, queue);

            //grab the block out of the cache as well
            bocl_mem* blk = opencl_cache->get_block(scene,id);
            bocl_mem* blk_info = opencl_cache->loaded_block_info();

            //determine swapping behavior
            int behavior_buffer[1];
            if(data_type == boxm2_data_traits<BOXM2_ALPHA>::prefix())
              behavior_buffer[0] = ALPHA_BEHAVIOR;
            else
              behavior_buffer[0] = ZERO_BEHAVIOR;
            bocl_mem behavior(device->context(), behavior_buffer, sizeof(int), "swap data behavior buffer");
            behavior.create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

            //set workspace
            std::size_t lThreads[] = {64, 1};
            std::size_t gThreads[] = {RoundUp(numTrees,lThreads[0]), 1};

            //make it a reference so the destructor isn't called at the end...
            kern->set_arg( blk_info );
            kern->set_arg( blk );
            kern->set_arg( blk_copy.ptr() );
            kern->set_arg( tree_sizes.ptr() );
            kern->set_arg( dat );
            kern->set_arg( new_dat );
            kern->set_arg( prob_thresh.ptr());
            kern->set_arg( &behavior );
            kern->set_arg( lookup.ptr() );
            kern->set_arg( cl_output.ptr() );
            kern->set_local_arg( lThreads[0]*10*sizeof(cl_uchar) );
            kern->set_local_arg( lThreads[0]*73*sizeof(cl_char) );
            kern->set_local_arg( lThreads[0]*sizeof(cl_uchar16) );
            kern->set_local_arg( lThreads[0]*sizeof(cl_uchar16) );

            //execute kernel
            kern->execute( queue, 2, lThreads, gThreads);
            clFinish( queue);
            kern->clear_args();
            gpu_time += kern->exec_time();

            ////write the data to buffer
            opencl_cache->deep_replace_data(scene,id, data_type, new_dat);
            if (data_type == boxm2_data_traits<BOXM2_ALPHA>::prefix()) {
                std::cout<<"   Writing refined trees."<<std::endl;
                blk->read_to_buffer(queue);
            }
        }
    } //end block_iter for
    std::cout<<" Merge GPU Time: "<<gpu_time<<", transfer time: "<<transfer_time<<std::endl;
    return true;
}
