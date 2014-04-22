// This is brl/bpro/core/vpgl_pro/processes/vpgl_rational_camera_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief Operations related to vpgl_rational_camera
//
// \author Yi Dong
// \date April 22, 2014
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//

#include <vcl_iostream.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>

//: Process to backproject an image point to a world point (wgs84 coords), using vpgl_backproject algo
//  Note that if initial gauss is not given, process will use camera offset as initial gauss
#include <vpgl/algo/vpgl_backproject.h>
#include <vgl/vgl_plane_3d.h>
#include <vpgl/vpgl_lvcs.h>
//: global variables and functions
namespace vpgl_rational_cam_img_to_global_process_globals
{
  const unsigned n_inputs_ = 8;
  const unsigned n_outputs_ = 3;
}
//: initialization
bool vpgl_rational_cam_img_to_global_process_cons(bprb_func_process& pro)
{
  using namespace vpgl_rational_cam_img_to_global_process_globals;
  // process takes 6 inputs
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vpgl_camera_double_sptr";  // rational camera
  input_types_[1] = "unsigned";                      // image point i
  input_types_[2] = "unsigned";                      // image point j
  input_types_[3] = "double";                   // initial gauss longitude (if given)
  input_types_[4] = "double";                   // initial gauss latitude  (if given)
  input_types_[5] = "double";                   // initial gauss elevation (also use as input plane elevation)
  input_types_[6] = "double";                   // plane elevation
  input_types_[7] = "double";                   // error tolerance
  // process takes 3 outputs
  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "double";                  // world point lon
  output_types_[1] = "double";                  // world point lat
  output_types_[2] = "double";                  // world point elev
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}
//: execute the process
bool vpgl_rational_cam_img_to_global_process(bprb_func_process& pro)
{
  using namespace vpgl_rational_cam_img_to_global_process_globals;

  // sanity check
  if (!pro.verify_inputs()){
    vcl_cerr << pro.name() << ": there should be " << n_inputs_ << " inputs" << vcl_endl;
    return false;
  }
  // get the inputs
  unsigned i = 0;
  vpgl_camera_double_sptr cam_sptr = pro.get_input<vpgl_camera_double_sptr>(i++);  // rational camera
  unsigned img_i   = pro.get_input<unsigned>(i++);                              // image point i
  unsigned img_j   = pro.get_input<unsigned>(i++);                              // image point j
  double init_lon  = pro.get_input<double>(i++);                                // initial gauss lon
  double init_lat  = pro.get_input<double>(i++);                                // initial gauss lat
  double init_elev = pro.get_input<double>(i++);                                // initial gauss elev
  double pl_elev   = pro.get_input<double>(i++);                                // point plane height
  double error_tol = pro.get_input<double>(i++);                                // error tolerance

  // get rational camera
  vpgl_rational_camera<double>* rat_cam = dynamic_cast<vpgl_rational_camera<double>*>(cam_sptr.as_pointer());
  if (!rat_cam) {
    vcl_cerr << pro.name() << ": the input camera is not a rational camera" << vcl_endl;
    return false;
  }

  vcl_cout << " rational camera type: " << rat_cam->type_name() << vcl_endl;
  
  // use camera offset if initial gauss is not given
  if (init_lon < 0.0) {
    // generate the initial from camera offset
    init_lon  = rat_cam->offset(vpgl_rational_camera<double>::X_INDX);
    init_lat  = rat_cam->offset(vpgl_rational_camera<double>::Y_INDX);
    init_elev = rat_cam->offset(vpgl_rational_camera<double>::Z_INDX);
  }
  // use init_elev as point plane height is pl_elev is not given
  if (pl_elev < 0.0)
    pl_elev = init_elev;

  vcl_cout << " initial gauss point: lon = " << init_lon << ", lat = " << init_lat << ", elev = " << init_elev << vcl_endl;
  vcl_cout << " elevation of the world point plane: " << pl_elev << vcl_endl;

  // start the back projection at given error tolerance
  vgl_point_2d<double> image_pt((double)img_i, (double)img_j);
  vgl_point_3d<double> world_pt(0.0, 0.0, 0.0);
  vgl_plane_3d<double> pl(0, 0, 1, -pl_elev);
  vgl_point_3d<double> initial_pt(init_lon, init_lat, init_elev);
  if (!vpgl_backproject::bproj_plane(rat_cam, image_pt, pl, initial_pt, world_pt, error_tol)) {
    vcl_cerr << pro.name() << ": back projection failed at given error initial gauss and error tolerance: " << error_tol << vcl_endl;
    return false;
  }
  double lon, lat, elev;
  lon = world_pt.x();  lat = world_pt.y();  elev = world_pt.z();
  // transfer to global wgs84 coordinates if input camera is a local rational camera
  if (rat_cam->type_name() == "vpgl_local_rational_camera") {
    vpgl_local_rational_camera<double>* local_cam = dynamic_cast<vpgl_local_rational_camera<double>*>(cam_sptr.as_pointer());
    vpgl_lvcs lvcs = local_cam->lvcs();
    lvcs.local_to_global(world_pt.x(), world_pt.y(), world_pt.z(), vpgl_lvcs::wgs84, lon, lat, elev);
  }

  // output
  i = 0;
  pro.set_output_val<double>(i++, lon);
  pro.set_output_val<double>(i++, lat);
  pro.set_output_val<double>(i++, elev);
  
  return true;
}


