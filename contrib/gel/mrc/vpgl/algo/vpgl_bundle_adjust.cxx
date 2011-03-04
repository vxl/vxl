// This is gel/mrc/vpgl/algo/vpgl_bundle_adjust.cxx
#include "vpgl_bundle_adjust.h"
//:
// \file



#include <vnl/algo/vnl_sparse_lm.h>
#include <vpgl/algo/vpgl_ba_fixed_k_lsqr.h>
#include <vgl/algo/vgl_rotation_3d.h>

#include <vcl_fstream.h>
#include <vcl_algorithm.h>



vpgl_bundle_adjust::vpgl_bundle_adjust()
  : ba_func_(NULL),
    use_weights_(false),
    use_gradient_(true),
    start_error_(0.0),
    end_error_(0.0)
{
}

vpgl_bundle_adjust::~vpgl_bundle_adjust()
{
  delete ba_func_;
}

//: Bundle Adjust
bool
vpgl_bundle_adjust::optimize(vcl_vector<vpgl_perspective_camera<double> >& cameras,
                             vcl_vector<vgl_point_3d<double> >& world_points,
                             const vcl_vector<vgl_point_2d<double> >& image_points,
                             const vcl_vector<vcl_vector<bool> >& mask)
{
  // Extract the camera and point parameters
  vcl_vector<vpgl_calibration_matrix<double> > K;
  a_ = vpgl_ba_fixed_k_lsqr::create_param_vector(cameras);
  b_ = vpgl_ba_fixed_k_lsqr::create_param_vector(world_points);
  for (unsigned int i=0; i<cameras.size(); ++i){
    K.push_back(cameras[i].get_calibration());
  }

  // do the bundle adjustment
  delete ba_func_;
  ba_func_ = new vpgl_ba_fixed_k_lsqr(K,image_points,mask,use_weights_);
  vnl_sparse_lm lm(*ba_func_);
  lm.set_trace(true);
  lm.set_verbose(true);
  vnl_vector<double> c;
  if (!lm.minimize(a_,b_,c,use_gradient_))
    return false;

  start_error_ = lm.get_start_error();
  end_error_ = lm.get_end_error();
  num_iterations_ = lm.get_num_iterations();

  // Update the camera parameters
  for (unsigned int i=0; i<cameras.size(); ++i)
    cameras[i] = ba_func_->param_to_cam(i,a_,c);
  // Update the point locations
  for (unsigned int j=0; j<world_points.size(); ++j)
    world_points[j] = ba_func_->param_to_point(j,b_,c);

  return true;
}


//: Write cameras and points to a file in VRML 2.0 for debugging
void
vpgl_bundle_adjust::write_vrml(const vcl_string& filename,
                               vcl_vector<vpgl_perspective_camera<double> >& cameras,
                               vcl_vector<vgl_point_3d<double> >& world_points)
{
  vcl_ofstream os(filename.c_str());
  os << "#VRML V2.0 utf8\n\n";

  for (unsigned int i=0; i<cameras.size(); ++i){
    vnl_double_3x3 K = cameras[i].get_calibration().get_matrix();

    const vgl_rotation_3d<double>& R = cameras[i].get_rotation();
    //R.set_row(1,-1.0*R.get_row(1));
    //R.set_row(2,-1.0*R.get_row(2));
    vgl_point_3d<double> ctr = cameras[i].get_camera_center();
    double fov = 2.0*vcl_max(vcl_atan(K[1][2]/K[1][1]), vcl_atan(K[0][2]/K[0][0]));
    os  << "Viewpoint {\n"
        << "  position    "<< ctr.x() << ' ' << ctr.y() << ' ' << ctr.z() << '\n'
        << "  orientation "<< R.axis() << ' '<< R.angle() << '\n'
        << "  fieldOfView "<< fov << '\n'
        << "  description \"Camera" << i << "\"\n}\n";
  }

  os << "Shape {\n  appearance NULL\n    geometry PointSet {\n"
     << "      color Color { color [1 0 0] }\n      coord Coordinate{\n"
     << "       point[\n";

  for (unsigned int j=0; j<world_points.size(); ++j){
    os  << world_points[j].x() << ' '
        << world_points[j].y() << ' '
        << world_points[j].z() << '\n';
  }
  os << "   ]\n  }\n }\n}\n";

  os.close();
}

