#include "breg3d_ekf_camera_optimizer_state.h"

#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/io/vnl_io_matrix_fixed.h>
#include <vnl/io/vnl_io_vector_fixed.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vpgl/algo/vpgl_interpolate.h>
#include <vsl/vsl_binary_io.h>


  // init constructor
breg3d_ekf_camera_optimizer_state::breg3d_ekf_camera_optimizer_state(
  double t_scale, 
  vgl_point_3d<double> base_point, vgl_rotation_3d<double> base_rot, 
  double init_translation_var, double init_rotation_var)
  : base_point_(base_point),base_rotation_(base_rot),k_(0),t_scale_(t_scale),xk_(0.0)
{
  set_error_covariance(init_translation_var,init_rotation_var);
}


vgl_rotation_3d<double> breg3d_ekf_camera_optimizer_state::get_rotation()
{
  return base_rotation_;
}


vgl_point_3d<double> breg3d_ekf_camera_optimizer_state::get_point()
{
  return base_point_;
}

void breg3d_ekf_camera_optimizer_state::set_error_covariance(double center_var, double rot_var)
{
  Pk_.fill(0.0);
  // translation components
  Pk_(0,0) = center_var;
  Pk_(1,1) = center_var;
  Pk_(2,2) = center_var;
  // rotation components
  Pk_(3,3) = rot_var;
  Pk_(4,4) = rot_var;
  Pk_(5,5) = rot_var;

  return;
}



//: output description of state to stream.
vcl_ostream& operator << (vcl_ostream& os, breg3d_ekf_camera_optimizer_state const& ekf_state)
{
  os << "k = " << ekf_state.k_ << vcl_endl;
  os << "Pk = " << ekf_state.Pk_ << vcl_endl;
  os << "xk = " << ekf_state.xk_ << vcl_endl;
  return os;
}

bool breg3d_ekf_camera_optimizer_state::operator == (breg3d_ekf_camera_optimizer_state const& other) const
{
  return (other.k_ == this->k_);
}


bool breg3d_ekf_camera_optimizer_state::operator < (breg3d_ekf_camera_optimizer_state const& other) const
{
  return (this->k_ < other.k_);
}


//: Binary save parameters to stream.
void vsl_b_write(vsl_b_ostream & os, breg3d_ekf_camera_optimizer_state const& ekf_state)
{
  vsl_b_write(os,ekf_state.k_);
  vsl_b_write(os,ekf_state.Pk_);
  vsl_b_write(os,ekf_state.xk_);
}


//: Binary load parameters from stream.
void vsl_b_read(vsl_b_istream & is, breg3d_ekf_camera_optimizer_state &ekf_state)
{
  vsl_b_read(is,ekf_state.k_);
  vsl_b_read(is,ekf_state.Pk_);
  vsl_b_read(is,ekf_state.xk_);
}
