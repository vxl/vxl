// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_update_sky_process.cxx
#include <fstream>
#include <iostream>
#include <algorithm>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for updating the sky voxels in the scene given a sky labeled image.
//
// \author Vishal Jain
// \date Mar 25, 2011

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
    constexpr unsigned int n_inputs_ = 5;
    constexpr unsigned int n_outputs_ = 0;
}

bool boxm2_ocl_update_sky_process_cons(bprb_func_process& pro)
{
    using namespace boxm2_ocl_update_sky_process_globals;

    //process takes 9 inputs (of which the four last ones are optional):
    std::vector<std::string> input_types_(n_inputs_);
    input_types_[0] = "bocl_device_sptr";
    input_types_[1] = "boxm2_scene_sptr";
    input_types_[2] = "boxm2_opencl_cache_sptr";
    input_types_[3] = "vpgl_camera_double_sptr";      //input camera
    input_types_[4] = "vil_image_view_base_sptr";     //input image
    // process has no outputs
    std::vector<std::string>  output_types_(n_outputs_);
    bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);


    return good;
}

bool boxm2_ocl_update_sky_process(bprb_func_process& pro)
{
    using namespace boxm2_ocl_update_sky_process_globals;

    //sanity check inputs
    if ( pro.n_inputs() < n_inputs_ ) {
        std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
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
    std::cout<<"Total time taken is "<<t.all()<<std::endl;
    return flag;
}


namespace boxm2_ocl_update_sky2_process_globals
{
    constexpr unsigned int n_inputs_ = 6;
    constexpr unsigned int n_outputs_ = 0;
}

bool boxm2_ocl_update_sky2_process_cons(bprb_func_process& pro)
{
    using namespace boxm2_ocl_update_sky2_process_globals;
    //process takes 9 inputs (of which the four last ones are optional):
    std::vector<std::string> input_types_(n_inputs_);
    input_types_[0] = "bocl_device_sptr";
    input_types_[1] = "boxm2_scene_sptr";
    input_types_[2] = "boxm2_opencl_cache_sptr";
    input_types_[3] = "vpgl_camera_double_sptr";      //input camera
    input_types_[4] = "vil_image_view_base_sptr";     //input image
    input_types_[5] = "int"; // ( 0- accum, 1- for update )
    brdb_value_sptr empty_img = new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<unsigned char>());
    brdb_value_sptr empty_camera   = new brdb_value_t<vpgl_camera_double_sptr>(new vpgl_perspective_camera<double>());  //by default update alpha
    brdb_value_sptr default_step   = new brdb_value_t<int>(0);  //by default update alpha
    pro.set_input(3, empty_camera);
    pro.set_input(4, empty_img);
    pro.set_input(5, default_step);
    // process has no outputs
    std::vector<std::string>  output_types_(n_outputs_);
    bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
    return good;
}

bool boxm2_ocl_update_sky2_process(bprb_func_process& pro)
{
    using namespace boxm2_ocl_update_sky2_process_globals;
    //sanity check inputs
    if ( pro.n_inputs() < n_inputs_ ) {
        std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
        return false;
    }
    //get the inputs
    unsigned int i = 0;
    bocl_device_sptr         device       = pro.get_input<bocl_device_sptr>(i++);
    boxm2_scene_sptr         scene        = pro.get_input<boxm2_scene_sptr>(i++);
    boxm2_opencl_cache_sptr  opencl_cache = pro.get_input<boxm2_opencl_cache_sptr>(i++);
    vpgl_camera_double_sptr  cam          = pro.get_input<vpgl_camera_double_sptr>(i++);
    vil_image_view_base_sptr img          = pro.get_input<vil_image_view_base_sptr>(i++);
    int step          = pro.get_input<int>(i++);
    bool flag =  false;
    vul_timer t;
    t.mark();
    if(step == 0 )
         flag =  boxm2_ocl_update_sky2::accumulate_sky_evidence(scene, device, opencl_cache, cam, img);
    else if(step == 1)
         flag =  boxm2_ocl_update_sky2::update_sky2(scene, device, opencl_cache);
    else
    {
        std::cout<<"Wrong Step #"<<std::endl;
        flag = false;
    }
    std::cout<<"Total time taken is "<<t.all()<<std::endl;
    return flag;
}
