// This is brl/bseg/boxm/algo/rt/pro/processes/boxm_rpc_registration_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for registering the RPC camera based on the edges in the world
// \author Gamze Tunali
// \date   Feb 1st, 2010

#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>
#include <boxm/algo/rt/boxm_rpc_registration.h>
#include <vil/vil_image_view.h>
#include <vcl_cstdio.h>

//: globals
namespace boxm_rpc_registration_process_globals
{
  const unsigned n_inputs_ = 8;
  const unsigned n_outputs_ = 1;
}

//: set input and output types
bool boxm_rpc_registration_process_cons(bprb_func_process& pro)
{
  using namespace boxm_rpc_registration_process_globals;

  // process takes 8 inputs:
  // input[0]: The scene
  // input[1]: The current camera
  // input[2]: The current edge image
  // input[3]: The expected edge image
  // input[4]: The flag indicating whether to align the 3D voxel world along with image
  // input[5]: Uncertainty in meters
  // input[6]: n_normal
  // input[7]: num_observation
  vcl_vector<vcl_string> input_types_(n_inputs_);
  unsigned i = 0;
  input_types_[i++] = "boxm_scene_base_sptr";
  input_types_[i++] = "vpgl_camera_double_sptr";
  input_types_[i++] = "vil_image_view_base_sptr";
  input_types_[i++] = "vil_image_view_base_sptr";
  input_types_[i++] = "bool";
  input_types_[i++] = "float";
  input_types_[i++] = "float";
  input_types_[i++] = "unsigned";

  // process has 1 output:
  // output[0]: The optimized camera
  vcl_vector<vcl_string> output_types_(n_outputs_);
  unsigned j = 0;
  output_types_[j++] = "vpgl_camera_double_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

//:  optimizes rpc camera parameters based on edges
bool boxm_rpc_registration_process(bprb_func_process& pro)
{
  using namespace boxm_rpc_registration_process_globals;

  //check number of inputs
  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << " The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  // get the inputs
  unsigned i = 0;

  // scene
  boxm_scene_base_sptr scene = pro.get_input<boxm_scene_base_sptr>(i++);

  // camera
  vpgl_camera_double_sptr camera_inp = pro.get_input<vpgl_camera_double_sptr>(i++);

  // image
  vil_image_view_base_sptr edge_image_sptr = pro.get_input<vil_image_view_base_sptr>(i++);

  vil_image_view_base_sptr expected_edge_image_sptr = pro.get_input<vil_image_view_base_sptr>(i++);

  // boolean parameter specifying the voxel world alignment state
  bool rpc_shift_3d_flag = pro.get_input<bool>(i++);

  // uncertainty in meters
  float uncertainty = pro.get_input<float>(i++);

  // n_normal
  float n_normal = pro.get_input<float>(i++);

  // number of observations
  unsigned num_observation = pro.get_input<unsigned>(i++);

  vcl_string edge_type="subpixel";//pro.get_input<vcl_string>(i++);

  vpgl_camera_double_sptr camera_out;

  if (edge_type=="subpixel")
  {
    vil_image_view<float > expected_edge_image(expected_edge_image_sptr);
    vil_image_view<float> edge_image(edge_image_sptr);
    boxm_rpc_registration<short,boxm_inf_line_sample<float> >(scene,edge_image,expected_edge_image,camera_inp, camera_out, rpc_shift_3d_flag,uncertainty, num_observation);
  }
  else {
    vil_image_view<vxl_byte> expected_edge_image(expected_edge_image_sptr);
    vil_image_view<vxl_byte> edge_image(edge_image_sptr);
    boxm_rpc_registration<short,boxm_edge_sample<float> >(scene,edge_image,expected_edge_image,camera_inp, camera_out, rpc_shift_3d_flag,uncertainty,n_normal, num_observation);
  }

  // output
  unsigned j = 0;
  // update the camera and store
  pro.set_output_val<vpgl_camera_double_sptr>(j++, camera_out);

  return true;
}
