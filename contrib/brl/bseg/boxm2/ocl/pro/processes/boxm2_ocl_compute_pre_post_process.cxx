// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_compute_pre_post_process.cxx
#include <fstream>
#include <iostream>
#include <algorithm>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for updating the pre cellwise to compute image factor for BP formulation
//
// \author Vishal Jain
// \date Nov 11, 2014

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


#include <boxm2/ocl/algo/boxm2_ocl_camera_converter.h>
#include <boxm2/ocl/algo/boxm2_ocl_update_image_factor.h>

//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <vul/vul_timer.h>

namespace boxm2_ocl_compute_pre_post_process_globals
{
    constexpr unsigned int n_inputs_ = 8;
    constexpr unsigned int n_outputs_ = 0;
}

bool boxm2_ocl_compute_pre_post_process_cons(bprb_func_process& pro)
{
    using namespace boxm2_ocl_compute_pre_post_process_globals;
    //process takes 9 inputs (of which the four last ones are optional):
    std::vector<std::string> input_types_(n_inputs_);
    unsigned int i = 0;
    input_types_[i++] = "bocl_device_sptr";
    input_types_[i++] = "boxm2_scene_sptr";
    input_types_[i++] = "boxm2_opencl_cache_sptr";
    input_types_[i++] = "vpgl_camera_double_sptr";      //input camera
    input_types_[i++] = "vil_image_view_base_sptr";     //input image
    input_types_[i++] = "vcl_string";     //input image
    input_types_[i++] = "float";                        // near factor ( maximum # of pixels should map to the finest voxel )
    input_types_[i++] = "float";                        // far factor ( minimum # of pixels should map to the finest voxel )
    // process has no outputs
    std::vector<std::string>  output_types_(n_outputs_);
    bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
    brdb_value_sptr tnearfactor = new brdb_value_t<float>(1e6f);
    brdb_value_sptr tfarfactor = new brdb_value_t<float>(1e6f);
    pro.set_input(6, tnearfactor);
    pro.set_input(7, tfarfactor);
    return good;
}

bool boxm2_ocl_compute_pre_post_process(bprb_func_process& pro)
{
    using namespace boxm2_ocl_compute_pre_post_process_globals;
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
    vpgl_camera_double_sptr  cam = pro.get_input<vpgl_camera_double_sptr>(i++);
    vil_image_view_base_sptr img = pro.get_input<vil_image_view_base_sptr>(i++);
    std::string view_identifier = pro.get_input<std::string>(i++);
    auto                    nearfactor = pro.get_input<float>(i++);
    auto                    farfactor = pro.get_input<float>(i++);
    vul_timer t;
    t.mark();
    boxm2_ocl_compute_pre_post::compute_pre_post(scene, device, opencl_cache, cam, img, view_identifier, nearfactor, farfactor);
    std::cout << "Total time taken is " << t.all() << std::endl;
    return true;
}
