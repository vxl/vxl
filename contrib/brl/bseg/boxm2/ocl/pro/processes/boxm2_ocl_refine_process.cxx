// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_refine_process.cxx
//:
// \file
// \brief  A process for rendering depth map of a scene.
//
// \author Vishal Jain
// \date Mar 10, 2011
// \verbatim
//  Modifications
//   Andrew Miller, 6 Jan 2012: factored code out to boxm2/ocl/algo/
// \endverbatim
#include <bprb/bprb_func_process.h>
#include <boxm2/ocl/algo/boxm2_ocl_refine.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <bocl/bocl_device.h>
namespace boxm2_ocl_refine_process_globals
{
    constexpr unsigned n_inputs_ = 4;
    constexpr unsigned n_outputs_ = 1;
}

bool boxm2_ocl_refine_process_cons(bprb_func_process& pro)
{
    using namespace boxm2_ocl_refine_process_globals;

    //process takes 1 input
    std::vector<std::string> input_types_(n_inputs_);
    input_types_[0] = "bocl_device_sptr";
    input_types_[1] = "boxm2_scene_sptr";
    input_types_[2] = "boxm2_opencl_cache_sptr";
    input_types_[3] = "float";

    // process has 1 output:
    std::vector<std::string>  output_types_(n_outputs_);
    output_types_[0] = "int";  //numcells
    return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_ocl_refine_process(bprb_func_process& pro)
{
    using namespace boxm2_ocl_refine_process_globals;
    if ( pro.n_inputs() < n_inputs_ ) {
        std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
        return false;
    }

    //get the inputs
    unsigned i = 0;
    bocl_device_sptr        device = pro.get_input<bocl_device_sptr>(i++);
    boxm2_scene_sptr        scene = pro.get_input<boxm2_scene_sptr>(i++);
    boxm2_opencl_cache_sptr opencl_cache = pro.get_input<boxm2_opencl_cache_sptr>(i++);
    auto                   thresh = pro.get_input<float>(i++);

    unsigned num_cells = boxm2_ocl_refine::refine_scene(device, scene, opencl_cache, thresh);
    std::cout<<"boxm2_ocl_refine_process num split: "<<num_cells<<std::endl;

    //set output
    pro.set_output_val<int>(0, num_cells);

    return true;
}
