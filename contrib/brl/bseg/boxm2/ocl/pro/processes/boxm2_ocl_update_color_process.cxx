// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_update_color_process.cxx
//:
// \file
// \brief  A process for updating a color model
//
// \author Vishal Jain
// \date Mar 25, 2011

#include <bprb/bprb_func_process.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/ocl/algo/boxm2_ocl_update_color.h>
#include <bocl/bocl_device.h>
#include <brdb/brdb_value.h>

namespace boxm2_ocl_update_color_process_globals
{
  const unsigned n_inputs_  = 8;
  const unsigned n_outputs_ = 0;
}

bool boxm2_ocl_update_color_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_update_color_process_globals;

  //process takes 1 input
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";
  input_types_[4] = "vil_image_view_base_sptr";
  input_types_[5] = "vcl_string";
  input_types_[6] = "vcl_string";
  input_types_[7] = "bool";

  // process has 1 output:
  // output[0]: scene sptr
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
  
  //set defaults inputs
  brdb_value_sptr idx         = new brdb_value_t<vcl_string>("");
  brdb_value_sptr updateAlpha = new brdb_value_t<bool>(true); 
  pro.set_input(5, idx);
  pro.set_input(6, idx);
  pro.set_input(7, updateAlpha);
  return good;
}

bool boxm2_ocl_update_color_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_update_color_process_globals;
  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  float transfer_time=0.0f;
  float gpu_time=0.0f;

  //get the inputs: device, scene, opencl_cache, cam, img
  unsigned argIdx = 0;
  bocl_device_sptr          device = pro.get_input<bocl_device_sptr>(argIdx++);
  boxm2_scene_sptr          scene = pro.get_input<boxm2_scene_sptr>(argIdx++);
  boxm2_opencl_cache_sptr   opencl_cache = pro.get_input<boxm2_opencl_cache_sptr>(argIdx++);
  vpgl_camera_double_sptr   cam = pro.get_input<vpgl_camera_double_sptr>(argIdx++);
  vil_image_view_base_sptr  img = pro.get_input<vil_image_view_base_sptr>(argIdx++);
  vcl_string in_identifier = pro.get_input<vcl_string>(argIdx++);
  vcl_string mask_filename = pro.get_input<vcl_string>(argIdx++);
  bool       updateAlpha   = pro.get_input<bool>(argIdx++);

  //call ocl update
  boxm2_ocl_update_color::update_color(scene,device,opencl_cache,cam,img,in_identifier,mask_filename,updateAlpha);
  
  //no outputs
  return true;
}
