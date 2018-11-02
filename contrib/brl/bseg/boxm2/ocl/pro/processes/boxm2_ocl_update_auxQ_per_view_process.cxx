// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_update_auxQ_per_view_process.cxx
#include <fstream>
#include <iostream>
#include <algorithm>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for updating the scene.
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
#include <vpl/vpl.h> // vpl_unlink()

#include <boxm2/ocl/algo/boxm2_ocl_camera_converter.h>
#include <boxm2/ocl/algo/boxm2_ocl_update_auxQ.h>

//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <vul/vul_timer.h>

namespace boxm2_ocl_update_auxQ_per_view_process_globals
{
  constexpr unsigned int n_inputs_ = 7;
  constexpr unsigned int n_outputs_ = 0;
}

bool boxm2_ocl_update_auxQ_per_view_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_update_auxQ_per_view_process_globals;

  //process takes 9 inputs (of which the four last ones are optional):
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";      //input camera
  input_types_[4] = "vil_image_view_base_sptr";     //input image
  input_types_[5] = "vcl_string";                   //illumination identifier
  input_types_[6] = "vcl_string";                   //mask image view
  // process has no outputs
  std::vector<std::string>  output_types_(n_outputs_);
  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
  // default 5, 6 and 7 and 8 inputs
  brdb_value_sptr idx5        = new brdb_value_t<std::string>("");
  pro.set_input(5, idx5);
  brdb_value_sptr idx6        = new brdb_value_t<std::string>("");
  pro.set_input(6, idx6);
  return good;
}

bool boxm2_ocl_update_auxQ_per_view_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_update_auxQ_per_view_process_globals;
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
  std::string               ident        = pro.get_input<std::string>(i++);
  std::string               view_ident   = pro.get_input<std::string>(i++);
  vul_timer t;
  t.mark();
  //TODO Factor this out to a utility function
  //make sure this image small enough (or else carve it into image pieces)
  boxm2_ocl_update_auxQ::update_auxQ(scene, device, opencl_cache, cam, img, ident,view_ident,8.0,8.0);
  return true;
}
