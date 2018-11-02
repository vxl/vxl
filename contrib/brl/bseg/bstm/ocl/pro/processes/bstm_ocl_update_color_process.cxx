// This is brl/bseg/bstm/ocl/pro/processes/bstm_ocl_update_color_process.cxx
#include <fstream>
#include <iostream>
#include <algorithm>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for updating the 4-d scene.
//
// \author Ali Osman Ulusoy
// \date May 10, 2013

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bstm/ocl/bstm_opencl_cache.h>
#include <bstm/bstm_scene.h>
#include <bstm/bstm_block.h>
#include <bstm/bstm_data_base.h>
#include <bstm/ocl/bstm_ocl_util.h>
#include <bstm/bstm_util.h>
#include <vil/vil_image_view.h>

#include <vil/vil_new.h>
#include <vpl/vpl.h> // vpl_unlink()

#include <boxm2/ocl/algo/boxm2_ocl_camera_converter.h>
#include <bstm/ocl/algo/bstm_ocl_update_color.h>

//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

namespace bstm_ocl_update_color_process_globals
{
  constexpr unsigned int n_inputs_ = 10;
  constexpr unsigned int n_outputs_ = 0;
}

bool bstm_ocl_update_color_process_cons(bprb_func_process& pro)
{
  using namespace bstm_ocl_update_color_process_globals;

  //process takes 9 inputs (of which the four last ones are optional):
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "bstm_scene_sptr";
  input_types_[2] = "bstm_opencl_cache_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";      //input camera
  input_types_[4] = "vil_image_view_base_sptr";     //input image
  input_types_[5] = "float";                        //time
  input_types_[6] = "float";                        //mog var
  input_types_[7] = "vil_image_view_base_sptr";     //mask image view
  input_types_[8] = "bool";                         //update alpha
  input_types_[9] = "bool";                         //update changed voxels only

  //initialize empty mask if no input is given
  brdb_value_sptr empty_mask = new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<unsigned char>(1,1));
  brdb_value_sptr def_var    = new brdb_value_t<float>(-1.0f);
  brdb_value_sptr def_update_alpha    = new brdb_value_t<bool>(true);

  // process has no outputs
  std::vector<std::string>  output_types_(n_outputs_);
  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

  pro.set_input(6, def_var);
  pro.set_input(7, empty_mask);
  pro.set_input(8, def_update_alpha);

  return good;
}

bool bstm_ocl_update_color_process(bprb_func_process& pro)
{
  using namespace bstm_ocl_update_color_process_globals;

  //sanity check inputs
  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned int i = 0;
  bocl_device_sptr         device       = pro.get_input<bocl_device_sptr>(i++);
  bstm_scene_sptr         scene         = pro.get_input<bstm_scene_sptr>(i++);
  bstm_opencl_cache_sptr  opencl_cache  = pro.get_input<bstm_opencl_cache_sptr>(i++);
  vpgl_camera_double_sptr  cam          = pro.get_input<vpgl_camera_double_sptr>(i++);
  vil_image_view_base_sptr img          = pro.get_input<vil_image_view_base_sptr>(i++);
  auto time                           = pro.get_input<float>(i++);
  auto mog_var                        = pro.get_input<float>(i++);
  vil_image_view_base_sptr mask_img     = pro.get_input<vil_image_view_base_sptr>(i++);
  bool                     update_alpha = pro.get_input<bool>(i++);
  bool                     update_changes_only = pro.get_input<bool>(i++);

  return bstm_ocl_update_color::update(scene, device, opencl_cache, cam, img, time, mog_var,  update_alpha, update_changes_only, mask_img);
}
