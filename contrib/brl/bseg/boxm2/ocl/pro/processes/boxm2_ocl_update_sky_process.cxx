// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_update_sky_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for updating the sky voxels in the scene given a sky labeled image.
//
// \author Vishal Jain
// \date Mar 25, 2011

#include <vcl_fstream.h>
#include <vcl_algorithm.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/boxm2_util.h>
#include <vil/vil_image_view.h>

#include <vil/vil_new.h>


#include <boxm2/ocl/algo/boxm2_ocl_camera_converter.h>
#include <boxm2/ocl/algo/boxm2_ocl_update_sky.h>

//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <vul/vul_timer.h>

namespace boxm2_ocl_update_sky_process_globals
{
    const unsigned int n_inputs_  = 5;
    const unsigned int n_outputs_ = 0;
}

bool boxm2_ocl_update_sky_process_cons(bprb_func_process& pro)
{
    using namespace boxm2_ocl_update_sky_process_globals;

    //process takes 9 inputs (of which the four last ones are optional):
    vcl_vector<vcl_string> input_types_(n_inputs_);
    input_types_[0] = "bocl_device_sptr";
    input_types_[1] = "boxm2_scene_sptr";
    input_types_[2] = "boxm2_opencl_cache_sptr";
    input_types_[3] = "vpgl_camera_double_sptr";      //input camera
    input_types_[4] = "vil_image_view_base_sptr";     //input image
    // process has no outputs
    vcl_vector<vcl_string>  output_types_(n_outputs_);
    bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);


    return good;
}

bool boxm2_ocl_update_sky_process(bprb_func_process& pro)
{
    using namespace boxm2_ocl_update_sky_process_globals;

    //sanity check inputs
    if ( pro.n_inputs() < n_inputs_ ) {
        vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
        return false;
    }
    //get the inputs
    unsigned int i = 0;
    bocl_device_sptr         device       = pro.get_input<bocl_device_sptr>(i++);
    boxm2_scene_sptr         scene        = pro.get_input<boxm2_scene_sptr>(i++);
    boxm2_opencl_cache_sptr  opencl_cache = pro.get_input<boxm2_opencl_cache_sptr>(i++);
    vpgl_camera_double_sptr  cam          = pro.get_input<vpgl_camera_double_sptr>(i++);
    vil_image_view_base_sptr img          = pro.get_input<vil_image_view_base_sptr>(i++);

    vul_timer t;
    t.mark();
    bool flag =  boxm2_ocl_update_sky::update_sky(scene, device, opencl_cache, cam, img);
    vcl_cout<<"Total time taken is "<<t.all()<<vcl_endl;
    return flag;
}
