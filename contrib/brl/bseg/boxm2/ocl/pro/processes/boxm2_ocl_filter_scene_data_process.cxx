// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_filter_scene_data_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for filtering scene data
//
// \author Octi Biris
// \date Jun 16, 2015

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/ocl/algo/boxm2_ocl_filter_scene_data.h>
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
#include <boxm2/boxm2_util.h>

namespace boxm2_ocl_filter_scene_data_process_globals
{
    constexpr unsigned n_inputs_ = 5;
    constexpr unsigned n_outputs_ = 0;


}

bool boxm2_ocl_filter_scene_data_process_cons(bprb_func_process& pro)
{
    using namespace boxm2_ocl_filter_scene_data_process_globals;

    //process takes 1 input
    std::vector<std::string> input_types_(n_inputs_);
    input_types_[0] = "bocl_device_sptr";
    input_types_[1] = "boxm2_scene_sptr";
    input_types_[2] = "boxm2_opencl_cache_sptr";
    input_types_[3] = "bvpl_kernel_vector_sptr";
    input_types_[4] = "int";


    // process has 1 output:
    // output[0]: scene sptr
    std::vector<std::string>  output_types_(n_outputs_);
    return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_ocl_filter_scene_data_process(bprb_func_process& pro)
{
    using namespace boxm2_ocl_filter_scene_data_process_globals;
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
    bvpl_kernel_vector_sptr filter_vector = pro.get_input<bvpl_kernel_vector_sptr>(i++);
    int filter_index = pro.get_input<int>(i++);
    std::string identifier=device->device_identifier();
    // create a command queue.
    int status=0;
    std::vector<std::string> valid_types;
    int appTypeSize;
        std::string appType;
        valid_types.emplace_back("boxm2_mog3_grey");
        valid_types.emplace_back("boxm2_mog6_view_compact");
        if (!boxm2_util::verify_appearance(*scene,valid_types,appType,appTypeSize)){
                std::cout<<"scene doesn't have the correct appearance type - only mog3_grey and mog6_view compact allowed!!"<<std::endl;
        }
    cl_command_queue queue = clCreateCommandQueue(device->context(),
                                                  *(device->device_id()),
                                                  CL_QUEUE_PROFILING_ENABLE,&status);
    if (status!=0) {
        std::cout<<" ERROR in initializing a queue"<<std::endl;
        return false;
    }
    opencl_cache->clear_cache();
    boxm2_ocl_filter_scene_data engine(opencl_cache, device,
                filter_vector,appType,appTypeSize,&queue);
   bool sm = engine.apply_filter(filter_index);
    clReleaseCommandQueue(queue);
    return sm ;
}
