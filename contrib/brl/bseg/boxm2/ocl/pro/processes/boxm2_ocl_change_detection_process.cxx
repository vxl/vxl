// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_change_detection_process.cxx
//:
// \file
// \brief  A process for change detection
//
// \author Vishal Jain
// \date Mar 10, 2011

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
#include <vcl_fstream.h>
#include <vcl_algorithm.h>
#include <vcl_sstream.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

namespace boxm2_ocl_change_detection_process_globals
{
  const unsigned n_inputs_     = 9;
  const unsigned n_outputs_    = 2;
}

bool boxm2_ocl_change_detection_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_change_detection_process_globals;

  // process takes 9 inputs and two outputs
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";
  input_types_[4] = "vil_image_view_base_sptr";
  input_types_[5] = "vil_image_view_base_sptr";
  input_types_[6] = "int";   // n for nxn ray casting
  input_types_[7] = "vcl_string"; // "raybelief" string for using raybelief
  input_types_[8] = "bool";       // true to use max mode probability
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";  // prob of change image
  output_types_[1] = "vil_image_view_base_sptr";  // Red Green change image
  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

  // default is 1x1, with no ray belief
  brdb_value_sptr nxn  = new brdb_value_t<int>(1);
  brdb_value_sptr rayb = new brdb_value_t<vcl_string>(""); // use ray belief?
  brdb_value_sptr pmax = new brdb_value_t<bool>(false);    // use max-mode probability instead of mixture?
  pro.set_input(6, nxn);
  pro.set_input(7, rayb);
  pro.set_input(8, pmax);
  return good;
}

bool boxm2_ocl_change_detection_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_change_detection_process_globals;
  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  // get the inputs
  unsigned i = 0;
  bocl_device_sptr          device        = pro.get_input<bocl_device_sptr>(i++);
  boxm2_scene_sptr          scene         = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_opencl_cache_sptr   opencl_cache  = pro.get_input<boxm2_opencl_cache_sptr>(i++);
  vpgl_camera_double_sptr   cam           = pro.get_input<vpgl_camera_double_sptr>(i++);
  vil_image_view_base_sptr  img           = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr  exp_img       = pro.get_input<vil_image_view_base_sptr>(i++);
  int                       n             = pro.get_input<unsigned>(i++);                 // nxn
  vcl_string                norm_type     = pro.get_input<vcl_string>(i++);
  bool                      pmax          = pro.get_input<bool>(i++);

  // img dims
  unsigned ni=img->ni();
  unsigned nj=img->nj();

  // allocate two output images
  vil_image_view<float>*    change_img     = new vil_image_view<float>(ni, nj);
  vil_image_view<vxl_byte>* rgb_change_img = new vil_image_view<vxl_byte>(ni,nj,4);

  // check to see which type of change detection to do, either two pass, or regular
  vul_timer t;
  if ( norm_type == "twopass" ) {
    boxm2_ocl_two_pass_change::change_detect( *change_img,
                                              device,
                                              scene,
                                              opencl_cache,
                                              cam,
                                              img,
                                              exp_img,
                                              n,
                                              norm_type,
                                              pmax );
  }
  else {
    // store scene smaprt pointer
    boxm2_ocl_change_detection::change_detect( *change_img,
                                               *rgb_change_img,
                                               device,
                                               scene,
                                               opencl_cache,
                                               cam,
                                               img,
                                               exp_img,
                                               n,
                                               norm_type,
                                               pmax );
  }
  vcl_cout<<" change time: "<<t.all()<<" ms"<<vcl_endl;

  // store rgb change image
  vcl_cout<<" preparing rgb output image"<<vcl_endl;
  vil_image_view_base_sptr  float_img = boxm2_util::prepare_input_image(img, true); // true for force gray scale
  vil_image_view<float>&    inImg     = *static_cast<vil_image_view<float>* >(float_img.ptr());
  vil_image_view<float>&    change    = *change_img;
  vil_image_view<vxl_byte>& rgb       = *rgb_change_img;
  for (unsigned c=0; c<nj; c++) {
    for (unsigned r=0; r<ni; r++) {
      rgb(r,c,0) = (vxl_byte) ( change(r,c) * 255.0f);
      rgb(r,c,1) = (vxl_byte) ( inImg(r,c) * 255.0f/8.0f );
      rgb(r,c,2) = (vxl_byte) 0;
      rgb(r,c,3) = (vxl_byte) 255;
    }
  }

  // set outputs
  i=0;
  pro.set_output_val<vil_image_view_base_sptr>(i++, change_img);
  pro.set_output_val<vil_image_view_base_sptr>(i++, rgb_change_img);
  return true;
}
