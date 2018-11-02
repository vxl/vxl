// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_remove_low_nobs_process.cxx
#include <fstream>
#include <iostream>
#include <algorithm>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for setting alpha to 0 for cells with low numbers of observations
//
// \author J. L. Mundy
// \date March 12, 2016

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/boxm2_util.h>
#include <vil/vil_image_view.h>

#include <vil/vil_new.h>


#include <boxm2/ocl/algo/boxm2_ocl_remove_low_nobs.h>

//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <vul/vul_timer.h>
#include <bpro/core/bbas_pro/bbas_1d_array_string.h>
#include <bpro/core/bbas_pro/bbas_1d_array_float.h>
namespace boxm2_ocl_remove_low_nobs_process_globals
{
    constexpr unsigned int n_inputs_ = 5;
    constexpr unsigned int n_outputs_ = 0;
}

bool boxm2_ocl_remove_low_nobs_process_cons(bprb_func_process& pro)
{
    using namespace boxm2_ocl_remove_low_nobs_process_globals;
    //process takes 9 inputs (of which the four last ones are optional):
    std::vector<std::string> input_types_(n_inputs_);
    unsigned int i = 0;
    input_types_[i++] = "bocl_device_sptr";
    input_types_[i++] = "boxm2_scene_sptr";
    input_types_[i++] = "boxm2_opencl_cache_sptr";
    input_types_[i++] = "float";      // threshold on number of observations

    // process has no outputs
    std::vector<std::string>  output_types_(n_outputs_);
    bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

    return good;
}

bool boxm2_ocl_remove_low_nobs_process(bprb_func_process& pro)
{
    using namespace boxm2_ocl_remove_low_nobs_process_globals;
    //sanity check inputs
    if (pro.n_inputs() < n_inputs_) {
        std::cout << pro.name() << ": The input number should be " << n_inputs_ << std::endl;
        return false;
    }
    //get the inputs
    unsigned int i = 0;
    bocl_device_sptr         device = pro.get_input<bocl_device_sptr>(i++);
    boxm2_scene_sptr         scene = pro.get_input<boxm2_scene_sptr>(i++);
    boxm2_opencl_cache_sptr  opencl_cache = pro.get_input<boxm2_opencl_cache_sptr>(i++);
    auto                    num_obs_thresh_multiplier = pro.get_input<float>(i);

    vul_timer t;
    t.mark();
    boxm2_ocl_remove_low_nobs::remove_low_nobs(scene, device, opencl_cache, num_obs_thresh_multiplier);
    std::cout << "Total time taken is " << t.all() << std::endl;
    return true;
}
