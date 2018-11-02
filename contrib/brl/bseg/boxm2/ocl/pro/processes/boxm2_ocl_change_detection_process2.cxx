// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_change_detection_process2.cxx
//:
// \file
// \brief  A process for change detection
//
// \author Vishal Jain
// \date Mar 10, 2011

#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <bprb/bprb_func_process.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/ocl/algo/boxm2_ocl_change_detection.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/boxm2_util.h>
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

namespace boxm2_ocl_change_detection_process2_globals
{
  constexpr unsigned n_inputs_ = 9;
  constexpr unsigned n_outputs_ = 2;
}

bool boxm2_ocl_change_detection_process2_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_change_detection_process2_globals;

  // process takes 9 inputs and two outputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";
  input_types_[4] = "vil_image_view_base_sptr";
   input_types_[5] = "vcl_string";
  input_types_[6] = "bool";   // is max mode on ?
  input_types_[7] = "float";                        // near factor ( # of pixels should map to the finest voxel )
  input_types_[8] = "float";                        // far factor (  # of pixels should map to the finest voxel )

  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";  // prob of change image
  output_types_[1] = "vil_image_view_base_sptr";  // vis image
  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
  brdb_value_sptr identifier = new brdb_value_t<std::string>("");
  pro.set_input(5, identifier);
  brdb_value_sptr idx = new brdb_value_t<bool>(false);
  pro.set_input(6, idx);
  brdb_value_sptr tnearfactor = new brdb_value_t<float>(100000.0f);  //by default update alpha
  brdb_value_sptr tfarfactor = new brdb_value_t<float>(0.0001f);  //by default update alpha

  pro.set_input(7, tnearfactor);
  pro.set_input(8, tfarfactor);
  return good;
}

bool boxm2_ocl_change_detection_process2(bprb_func_process& pro)
{
  using namespace boxm2_ocl_change_detection_process2_globals;
  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  // get the inputs
  unsigned i = 0;
  bocl_device_sptr          device = pro.get_input<bocl_device_sptr>(i++);
  boxm2_scene_sptr          scene = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_opencl_cache_sptr   opencl_cache = pro.get_input<boxm2_opencl_cache_sptr>(i++);
  vpgl_camera_double_sptr   cam = pro.get_input<vpgl_camera_double_sptr>(i++);
  vil_image_view_base_sptr  img = pro.get_input<vil_image_view_base_sptr>(i++);
  std::string identifier = pro.get_input<std::string>(i++);
   bool  max_density = pro.get_input<bool>(i++);
  auto                    nearfactor = pro.get_input<float>(i++);
  auto                    farfactor = pro.get_input<float>(i++);

  // img dims
  unsigned ni=img->ni();
  unsigned nj=img->nj();

  // allocate two output images
  auto*    change_img = new vil_image_view<float>(ni, nj);
  auto*    vis_img = new vil_image_view<float>(ni, nj);

  // check to see which type of change detection to do, either two pass, or regular
  vul_timer t;
  //
  boxm2_ocl_aux_pass_change::change_detect( *change_img,
      *vis_img,
                                              device,
                                              scene,
                                              opencl_cache,
                                              cam,
                                              img, identifier,
                                              max_density, nearfactor,  farfactor );
  std::cout<<" change time: "<<t.all()<<" ms"<<std::endl;
  // set outputs
  i=0;
  pro.set_output_val<vil_image_view_base_sptr>(i++, change_img);
  pro.set_output_val<vil_image_view_base_sptr>(i++, vis_img);
  return true;
}
