#include "breg3d_ekf_camera_optimizer_state.h"
//:
// \file

#include <vnl/io/vnl_io_matrix_fixed.h>
#include <vnl/io/vnl_io_vector_fixed.h>
#include <vsl/vsl_binary_io.h>


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
std::ostream& operator << (std::ostream& os, breg3d_ekf_camera_optimizer_state const& ekf_state)
{
  os << "k = " << ekf_state.k_ << '\n'
     << "Pk = " << ekf_state.Pk_ << '\n'
     << "xk = " << ekf_state.xk_ << std::endl;
  return os;
}

bool breg3d_ekf_camera_optimizer_state::operator == (breg3d_ekf_camera_optimizer_state const& other) const
{
  return other.k_ == this->k_;
}


bool breg3d_ekf_camera_optimizer_state::operator < (breg3d_ekf_camera_optimizer_state const& other) const
{
  return this->k_ < other.k_;
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
