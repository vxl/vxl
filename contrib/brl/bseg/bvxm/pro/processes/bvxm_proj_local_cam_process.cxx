// This is brl/bseg/bvxm/pro/processes/bvxm_proj_local_cam_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A class for clipping and image based on a 3D bounding box.
//        -  Input:
//             - Image path (string)
//             - bvxm_voxel_world_sptr
//
//        -  Output:
//             - modified rational camera "vpgl_camera_double_sptr"
//             - clipped image area (NITF) "vil_image_view_base_sptr"
//
//        -  Params:
//             -geographic uncertainty (error) in meters
//
// \author  Gamze D. Tunali
// \date    Feb 19, 2008
// \verbatim
//  Modifications
//   Brandon Mayer - Jan 28, 2009 - converted process-class to function to conform with new bvxm_process architecture.
// \endverbatim

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_world_params.h>
#include <bvxm/bvxm_util.h>

#include <vsol/vsol_box_2d_sptr.h>
#include <vsol/vsol_box_2d.h>

#include <brip/brip_roi.h>

#include <vpgl/vpgl_local_rational_camera.h>

#include <bprb/bprb_parameters.h>

//: globals variables and functions
namespace bvxm_proj_local_cam_process_globals
{
  constexpr unsigned n_inputs_ = 4;
  constexpr unsigned n_outputs_ = 2;
}

//: set input and output types
bool bvxm_proj_local_cam_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_proj_local_cam_process_globals;

  //this process takes 4 inputs:
  std::vector<std::string> input_types_(n_inputs_);
  unsigned  i=0;
  input_types_[i++] = "vpgl_camera_double_sptr";   // rational camera
  input_types_[i++] = "float";   // x
  input_types_[i++] = "float";   // y
  input_types_[i++] = "float";   // z

  //output
  unsigned j = 0;
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[j++] = "float"; // u
  output_types_[j++] = "float"; // v

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


bool bvxm_proj_local_cam_process(bprb_func_process& pro)
{
  using namespace bvxm_proj_local_cam_process_globals;
  //static const parameters
  static const std::string error = "error";

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << " The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  // get the inputs:
  unsigned i = 0;
  // camera
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(i++);
  float X=0.0,Y=0.0,Z=0.0;
  //voxel_world
  X = pro.get_input<float>(i++);
  Y = pro.get_input<float>(i++);
  Z = pro.get_input<float>(i++);

  vpgl_local_rational_camera<double>* rat_camera =
    dynamic_cast<vpgl_local_rational_camera<double>*> (camera.as_pointer());
  if (!rat_camera) {
    std::cerr << "The camera input is not a rational camera\n";
    return false;
  }

  double u=0,v=0;
  rat_camera->project((double)X,(double)Y,(double)Z,u,v);
  //Store outputs
  unsigned j = 0;
  // update the camera and store
  pro.set_output_val<float>(j++, u);
  pro.set_output_val<float>(j++, v);

  std::cout<<"(u,v):"<<u<<','<<v<<'\n';
  return true;
}
