// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_update_view_dep_app_process.cxx
#include <fstream>
#include <iostream>
#include <algorithm>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief   A process for updating a scene with BOXM2_MOG6_VIEW appearance datatype. This datatype allows for modeling view-dependent appearance.
//          As an approximation to a representation of the appearance on the entire view sphere, it models appearance of 6 discrete directions(x,y,z,-x,-y,-z).
//          The appearance of each direction is modeled using a Gaussian distribution. The parameters for each gaussian is learned in an online manner using this process.
//          The appearance from any view point can then be computed by interpolating the mean appearance from the three directions on the hemisphere facing the view point.
//          The resulting model can be visualized using boxm2_ocl_render_view.
//
// \author Ali Osman Ulusoy
// \date Nov 20, 2012

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

#include <vpl/vpl.h> // vpl_unlink()

#include <boxm2/ocl/algo/boxm2_ocl_camera_converter.h>
#include <boxm2/ocl/algo/boxm2_ocl_update_view_dep_app.h>

//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

namespace boxm2_ocl_update_view_dep_app_process_globals
{
  constexpr unsigned int n_inputs_ = 9;
  constexpr unsigned int n_outputs_ = 0;
}

bool boxm2_ocl_update_view_dep_app_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_update_view_dep_app_process_globals;

  //process takes 9 inputs (of which the four last ones are optional):
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";      //input camera
  input_types_[4] = "vil_image_view_base_sptr";     //input image
  input_types_[5] = "vcl_string";                   //illumination identifier
  input_types_[6] = "vil_image_view_base_sptr";     //mask image view
  input_types_[7] = "bool";                         //do_update_alpha/don't update alpha
  input_types_[8] = "float";                        //variance value? if 0.0 or less, then use variable variance

  // process has no outputs
  std::vector<std::string>  output_types_(n_outputs_);
  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

  // default 5, 6 and 7 and 8 inputs
  brdb_value_sptr idx        = new brdb_value_t<std::string>("");
  brdb_value_sptr empty_mask = new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<unsigned char>(1,1));
  brdb_value_sptr up_alpha   = new brdb_value_t<bool>(true);  //by default update alpha
  brdb_value_sptr def_var    = new brdb_value_t<float>(-1.0f);
  pro.set_input(5, idx);
  pro.set_input(6, empty_mask);
  pro.set_input(7, up_alpha);
  pro.set_input(8, def_var);
  return good;
}

bool boxm2_ocl_update_view_dep_app_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_update_view_dep_app_process_globals;
#if 0 // unused!
  std::size_t local_threads[2]={8,8};
  std::size_t global_threads[2]={8,8};
#endif
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
  vil_image_view_base_sptr mask_sptr    = pro.get_input<vil_image_view_base_sptr>(i++);
  bool                     update_alpha = pro.get_input<bool>(i++);
  auto                    mog_var      = pro.get_input<float>(i++);


  return boxm2_ocl_update_view_dep_app::update(scene, device, opencl_cache, cam, img, ident, mask_sptr, update_alpha, mog_var);
}
