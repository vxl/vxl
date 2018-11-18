// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_compute_height_factor_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for ingesting heightmap to fuse with images in a 3-d scene.
//
// \author Vishal Jain
// \date Mar 30, 2015

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vul/vul_timer.h>
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <vnl/vnl_random.h>
#include <boxm2/ocl/algo/boxm2_ocl_update_heightmap_factor.h>

namespace boxm2_ocl_compute_height_factor_process_globals
{
    constexpr unsigned n_inputs_ = 8;
    constexpr unsigned n_outputs_ = 0;


}

bool boxm2_ocl_compute_height_factor_process_cons(bprb_func_process& pro)
{
    using namespace boxm2_ocl_compute_height_factor_process_globals;

    //process takes 7 inputs
    std::vector<std::string> input_types_(n_inputs_);
    input_types_[0] = "bocl_device_sptr";
    input_types_[1] = "boxm2_scene_sptr";
    input_types_[2] = "boxm2_opencl_cache_sptr";
    input_types_[3] = "vil_image_view_base_sptr"; // hmap
    input_types_[4] = "vil_image_view_base_sptr"; // var
    input_types_[5] = "vil_image_view_base_sptr"; // ximg
    input_types_[6] = "vil_image_view_base_sptr"; // yimg
    input_types_[7] = "int"; // smoothing radius
    // process has no outputs
    std::vector<std::string> output_types_(n_outputs_);

    return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_ocl_compute_height_factor_process(bprb_func_process& pro)
{
    using namespace boxm2_ocl_compute_height_factor_process_globals;

    if (pro.n_inputs() < n_inputs_) {
        std::cout << pro.name() << ": The input number should be " << n_inputs_ << std::endl;
        return false;
    }
    float transfer_time = 0.0f;
    float gpu_time = 0.0f;
    //get the inputs
    unsigned i = 0;
    bocl_device_sptr device = pro.get_input<bocl_device_sptr>(i++);
    boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(i++);
    boxm2_opencl_cache_sptr opencl_cache = pro.get_input<boxm2_opencl_cache_sptr>(i++);
    vil_image_view_base_sptr z_img = pro.get_input<vil_image_view_base_sptr>(i++);
    vil_image_view_base_sptr z_var_img = pro.get_input<vil_image_view_base_sptr>(i++);
    vil_image_view_base_sptr x_img = pro.get_input<vil_image_view_base_sptr>(i++);
    vil_image_view_base_sptr y_img = pro.get_input<vil_image_view_base_sptr>(i++);
    int sradius = pro.get_input<int>(i++);
    boxm2_ocl_compute_heightmap_pre_post::compute_pre_post(scene, device, opencl_cache, z_img, z_var_img, x_img, y_img,sradius);
    return true;
}
