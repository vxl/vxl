//This is brl/bseg/bvxm/pro/processes/bvxm_create_local_rpc_process.cxx

//:
// \file
// \brief // A process that takes a world model and rational camera and returns a local rational camera
//           Inputs:
//              0: The voxel world
//              1: The current camera
//           Outputs:
//              0: The local camera
//
// \author Ibrahim Eden
// \date March 14, 2008
// \verbatim
//
// \Modifications
//   Isabel Restrepo - Jan 27, 2009 - converted process-class to functions which is the new design for bvxm_processes.
// \endverbatim

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_image_metadata.h>

#include <vpgl/vpgl_local_rational_camera.h>

//: global variables
namespace bvxm_create_local_rpc_process_globals
{
  const unsigned n_inputs_ = 2;
  const unsigned n_outputs_ = 1;
}

//: initialize input and output types
bool bvxm_create_local_rpc_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_create_local_rpc_process_globals;
  // process takes 2 inputs:
  //input[0]: The voxel world
  //input[1]: The current camera
  vcl_vector<vcl_string>  input_types_(n_inputs_);
  input_types_[0] = "bvxm_voxel_world_sptr";
  input_types_[1] = "vpgl_camera_double_sptr";
  if (!pro.set_input_types(input_types_))
    return false;
      

  // process has 1 output:
  // output[0]: The local camera
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "vpgl_camera_double_sptr";
  
  if (!pro.set_output_types(output_types_))
    return false;
  
  return true;
}

//: process that takes a world model and rational camera and returns a local rational camera
bool bvxm_create_local_rpc_process(bprb_func_process& pro)
{
  using namespace bvxm_create_local_rpc_process_globals;

  //check number of inputs
  if (pro.n_inputs()<n_inputs_)
  {
    vcl_cout << pro.name() <<" : The input number should be "<< n_inputs_ << vcl_endl;
    return false;
  }

  //get inputs:
  //voxel world
  unsigned i = 0;
  bvxm_voxel_world_sptr vox_world = pro.get_input<bvxm_voxel_world_sptr>(i++);
  vpgl_camera_double_sptr camera_inp = pro.get_input<vpgl_camera_double_sptr>(i++);

  //check input's validity
  i = 0;
  if (!vox_world) {
    vcl_cout << pro.name() <<" :--  Input " << i++ << " is not valid!\n";
    return false;
  }

  if (!camera_inp) {
    vcl_cout << pro.name() <<" :--  Input " << i++ << " is not valid!\n";
    return false;
  }

  vpgl_rational_camera<double> *cam_inp;
  if (!(cam_inp = dynamic_cast<vpgl_rational_camera<double>*>(camera_inp.ptr()))) {
    vcl_cerr << "error: process expects camera to be a vpgl_rational_camera.\n";
    return false;
  }

  // create the local rational camera using the voxel model and the rational camera
  bgeo_lvcs_sptr lvcs = vox_world->get_params()->lvcs();
  vpgl_local_rational_camera<double> cam_out(*lvcs,*cam_inp);

  //Set and Store outputs
  int j = 0;
  vcl_vector<vcl_string> output_types_(1);

  // updated camera
  pro.set_output_val<vpgl_camera_double_sptr>(j++, (new vpgl_local_rational_camera<double>(cam_out)));

  return true;
}
