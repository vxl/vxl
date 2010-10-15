// This is brl/bpro/core/icam_pro/processes/icam_create_view_sphere_process.cxx

//:
// \file
// \brief A process for generating cameras that view a scene
//
// \author Gamze D. Tunali
// \date January 23, 2010
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <vpgl/vpgl_perspective_camera.h>
#include <vcl_string.h>
#include <vcl_iostream.h>

#include <icam/icam_view_sphere.h>
#include <icam/icam_view_metadata.h>

// global variables
namespace icam_create_view_sphere_process_globals
{
  // this process takes 10 inputs and 2 outputs
  const unsigned n_inputs_ = 12;
  const unsigned n_outputs_ = 2;
}

//: sets input and output types
bool icam_create_view_sphere_process_cons(bprb_func_process& pro)
{
  using namespace icam_create_view_sphere_process_globals;
  unsigned i=0;
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[i++] = "double";   // sphere elevation angle
  input_types_[i++] = "double";   // max degrees of view points that can be apart
  input_types_[i++] = "int";      // dimension of x of box
  input_types_[i++] = "int";      // dimension of y of box
  input_types_[i++] = "int";      // dimension of z of box
  input_types_[i++] = "double";   // origin_x of the box
  input_types_[i++] = "double";   // origin_y of the box
  input_types_[i++] = "double";   // origin_z of the box
  input_types_[i++] = "double";   // radius of the view sphere
  input_types_[i++] = "unsigned"; // ni of image
  input_types_[i++] = "unsigned"; // nj of image
  input_types_[i++] = "vcl_string"; // the folder path to save the camera files and the txt file inluding the names

  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "vcl_string"; // the text file that contains the paths for the view point cameras
  output_types_[1] = "icam_view_sphere_sptr"; // view sphere smart pointer

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

//: creates a scene from parameters
bool icam_create_view_sphere_process(bprb_func_process& pro)
{
  if (!pro.verify_inputs()) {
    vcl_cout << pro.name() << "icam_create_view_sphere_process: invalid inputs" << vcl_endl;
    return false;
  }
  using namespace icam_create_view_sphere_process_globals;

  int i=0;
  double elevation = pro.get_input<double>(i++);
  double view_angle = pro.get_input<double>(i++);
  int dim_x = pro.get_input<int>(i++);
  int dim_y = pro.get_input<int>(i++);
  int dim_z = pro.get_input<int>(i++);
  double orig_x = pro.get_input<double>(i++);
  double orig_y = pro.get_input<double>(i++);
  double orig_z = pro.get_input<double>(i++);
  double radius = pro.get_input<double>(i++);
  unsigned ni = pro.get_input<unsigned>(i++);
  unsigned nj = pro.get_input<unsigned>(i++);
  vcl_string path = pro.get_input<vcl_string>(i++);

  vgl_box_3d<double> world_bb(orig_x, orig_y, orig_z, orig_x+dim_x, orig_y+dim_y, orig_z+dim_z);
  icam_view_sphere_sptr view_sphere =new icam_view_sphere(world_bb, radius);

  // generate the view points-cameras
  view_sphere->create_view_points(elevation, view_angle, ni, nj);

  vcl_map<unsigned, vpgl_camera_double_sptr> cameras;
  view_sphere->cameras(cameras);

  // save the cameras and write the path
  vcl_stringstream fpath;
  fpath << path << "cameras.txt";
  vcl_ofstream file(fpath.str().c_str());
  if (!file.is_open()) {
    vcl_cerr << "Failed to open file " << fpath.str() << '\n';
    return false;
  }

  vcl_map<unsigned, vpgl_camera_double_sptr>::iterator it=cameras.begin();
  while (it != cameras.end()) {
    unsigned uid=it->first;
    vpgl_camera_double_sptr cam = it->second;
    vpgl_perspective_camera<double>* pers_cam = dynamic_cast<vpgl_perspective_camera<double>*>(cam.as_pointer());

    vcl_stringstream cam_path;
    cam_path << /*path << */"camera" << uid << ".txt";
    vcl_ofstream ofs(cam_path.str().c_str());
    if (!file.is_open()) {
      vcl_cerr << "Failed to open file " << cam_path.str() << '\n';
      return false;
    }
    ofs << *pers_cam;
    ofs.close();

    file << uid << ' ' << cam_path.str() << vcl_endl;
    it++;
  }
  file.close();

  // return the txt file (the lidt of camera paths)
  pro.set_output_val<vcl_string>(0, fpath.str());
  pro.set_output_val<icam_view_sphere_sptr>(1, view_sphere);
  return true;
}
