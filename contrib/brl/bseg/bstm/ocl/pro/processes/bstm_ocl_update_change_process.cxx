
// This is brl/bseg/bstm/ocl/pro/processes/bstm_ocl_update_change_process.cxx
//:
// \file
// \brief  A process for change detection
//
// \author Ali Osman Ulusoy
// \date May 15, 2013

#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <bprb/bprb_func_process.h>
#include <bstm/ocl/bstm_opencl_cache.h>
#include <bstm/ocl/algo/bstm_ocl_change_detection.h>
#include <bstm/bstm_scene.h>
#include <bstm/bstm_block.h>
#include <bstm/bstm_data_base.h>
#include <bstm/ocl/bstm_ocl_util.h>
#include <bstm/bstm_util.h>
#include <vil/vil_image_view.h>

// brdb stuff
#include <brdb/brdb_value.h>

// directory utility
#include <vul/vul_timer.h>
#include <vcl_where_root_dir.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

namespace bstm_ocl_update_change_process_globals
{
  constexpr unsigned n_inputs_ = 7;
  constexpr unsigned n_outputs_ = 1;
}

bool bstm_ocl_update_change_process_cons(bprb_func_process& pro)
{
  using namespace bstm_ocl_update_change_process_globals;

  // process takes 9 inputs and two outputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "bstm_scene_sptr";
  input_types_[2] = "bstm_opencl_cache_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";
  input_types_[4] = "vil_image_view_base_sptr"; //img
  input_types_[5] = "vil_image_view_base_sptr"; //mask
  input_types_[6] = "float";                    // time

  brdb_value_sptr empty_mask = new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<unsigned char>(1,1));
  pro.set_input(5, empty_mask);

  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";  // prob of change image
  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

  return good;
}

bool bstm_ocl_update_change_process(bprb_func_process& pro)
{
  using namespace bstm_ocl_update_change_process_globals;
  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  // get the inputs
  unsigned i = 0;
  bocl_device_sptr         device = pro.get_input<bocl_device_sptr>(i++);
  bstm_scene_sptr          scene = pro.get_input<bstm_scene_sptr>(i++);
  bstm_opencl_cache_sptr   opencl_cache = pro.get_input<bstm_opencl_cache_sptr>(i++);
  vpgl_camera_double_sptr  cam = pro.get_input<vpgl_camera_double_sptr>(i++);
  vil_image_view_base_sptr img = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr mask_img = pro.get_input<vil_image_view_base_sptr>(i++);
  auto                   time = pro.get_input<float>(i++);

  // img dims
  unsigned ni=img->ni();
  unsigned nj=img->nj();

  // allocate two output images
  auto*    vis_img = new vil_image_view<float>(ni, nj);

  // check to see which type of change detection to do, either two pass, or regular
  vul_timer t;

  // store scene smaprt pointer
  bstm_ocl_update_change::update_change(   *vis_img,
                                            device,
                                             scene,
                                             opencl_cache,
                                             cam,
                                             img,
                                             mask_img,
                                             time);
  std::cout<<" change time: "<<t.all()<<" ms"<<std::endl;

  // set outputs
  i=0;
  pro.set_output_val<vil_image_view_base_sptr>(i++, vis_img);
  return true;
}
