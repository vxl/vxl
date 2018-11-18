#include <utility>
#include "icam_depth_transform.h"
//
#include <vnl/vnl_inverse.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_numeric_traits.h>


void icam_depth_transform::cache_k()
{
  k00_ = static_cast<float>(K_from_inv_[0][0]);
  k02_ = static_cast<float>(K_from_inv_[0][2]);
  k11_ = static_cast<float>(K_from_inv_[1][1]);
  k12_ = static_cast<float>(K_from_inv_[1][2]);
}

void icam_depth_transform::cache_r()
{
  vnl_matrix_fixed<double, 3, 3> R = rot_.as_matrix();
  r00_ = static_cast<float>(R[0][0]);
  r01_ = static_cast<float>(R[0][1]);
  r02_ = static_cast<float>(R[0][2]);
  r10_ = static_cast<float>(R[1][0]);
  r11_ = static_cast<float>(R[1][1]);
  r12_ = static_cast<float>(R[1][2]);
  r20_ = static_cast<float>(R[2][0]);
  r21_ = static_cast<float>(R[2][1]);
  r22_ = static_cast<float>(R[2][2]);
}

void icam_depth_transform::set_k(vnl_matrix_fixed<double, 3, 3> const& K)
{
  K_from_inv_ = vnl_inverse(K);
  this->cache_k();
  to_fl_ = K[0][0];
  to_pu_ = K[0][2];
  to_pv_ = K[1][2];
}

void icam_depth_transform::
set_k(vnl_matrix_fixed<double, 3, 3> const& K_from,
      vnl_matrix_fixed<double, 3, 3> const& K_to)
{
  K_from_inv_ = vnl_inverse(K_from);
  this->cache_k();
  to_fl_ = K_to[0][0];
  to_pu_ = K_to[0][2];
  to_pv_ = K_to[1][2];
}

vnl_matrix_fixed<double,3,3> icam_depth_transform::K_to()
{
  vnl_matrix_fixed<double,3,3> ret(0.0);
  ret[0][0] = to_fl_;
  ret[0][2] = to_pu_;
  ret[1][1] = to_fl_;
  ret[1][2] = to_pv_;
  ret[2][2] = 1.0;
  return ret;
}

void icam_depth_transform::invert_depth(vil_image_view<double> const& depth)
{
  unsigned ni = depth.ni(), nj = depth.nj();
  float mval = vnl_numeric_traits<float>::maxval;
  inv_depth_.set_size(ni, nj);
  for (unsigned j = 0; j<nj; ++j)
    for (unsigned i = 0; i<ni; ++i) {
      auto z = static_cast<float>(depth(i,j));
      if (z<1.0e-6f) {
        inv_depth_(i,j) = mval;
        continue;
      }
      inv_depth_(i,j) = 1.0f/z;
    }
}

icam_depth_transform::
icam_depth_transform(vnl_matrix_fixed<double, 3, 3> const& K,
                     vil_image_view<double> const& depth,
                     vgl_rotation_3d<double>  rot,
                     vgl_vector_3d<double> const& trans,
                     bool adjust_to_fl)
  : adjust_to_fl_(adjust_to_fl), depth_(depth),rot_(std::move(rot)), trans_(trans)
{
  this->set_k(K);
  this->cache_r();
  scale_factors_.set_size(n_params());
  scale_factors_.fill(1.0);
  this->invert_depth(depth);
}

icam_depth_transform::
icam_depth_transform(vnl_matrix_fixed<double, 3, 3> const& K_from,
                     vnl_matrix_fixed<double, 3, 3> const& K_to,
                     vil_image_view<double> const& depth,
                     vgl_rotation_3d<double>  rot,
                     vgl_vector_3d<double> const& trans,
                     bool adjust_to_fl)
  : adjust_to_fl_(adjust_to_fl), depth_(depth),rot_(std::move(rot)), trans_(trans)
{
  this->set_k(K_from, K_to);
  this->cache_r();
  scale_factors_.set_size(n_params());
  scale_factors_.fill(1.0);
  this->invert_depth(depth);
}

icam_depth_transform::
icam_depth_transform(vnl_matrix_fixed<double, 3, 3> const& K_from,
                     double to_fl, double to_pu, double to_pv,
                     vil_image_view<double> const& depth,
                     vgl_rotation_3d<double>  rot,
                     vgl_vector_3d<double> const& trans,
                     bool adjust_to_fl)
  : adjust_to_fl_(adjust_to_fl), depth_(depth),
    to_fl_(to_fl), to_pu_(to_pu), to_pv_(to_pv),
    rot_(std::move(rot)), trans_(trans)
{
  this->set_k(K_from);
  this->cache_r();
  scale_factors_.set_size(n_params());
  scale_factors_.fill(1.0);
  this->invert_depth(depth);
}

bool icam_depth_transform::transform(double from_u, double from_v,
                                     double& to_u, double& to_v) const
{
  float mval = vnl_numeric_traits<float>::maxval;
  int ni = depth_.ni(), nj = depth_.nj();
  if (from_u<0||from_v<0||from_u>=ni||from_v>=nj) {
    to_u = 0.0; to_v = 0.0;
    return false;
  }
  // the inverse depth at location (from_u, from_v)
  float Zinv = inv_depth_(static_cast<unsigned>(from_u),
                          static_cast<unsigned>(from_v));
  if (Zinv==mval) {
    to_u = 0.0; to_v = 0.0;
    return false;
  }

  //undo calibraion matrix to get the world image position of the from location
  // note that kij_ are the elements of the inverse matrix
  float fw0, fw1;
  fw0 = k00_*static_cast<float>(from_u)+ k02_;
  fw1 = k11_*static_cast<float>(from_v)+ k12_;
  // trans/Z (note Zinv is reciprocal depth)
  float t0=Zinv*static_cast<float>(trans_.x()),
    t1 = Zinv*static_cast<float>(trans_.y()),
    t2 = Zinv*static_cast<float>(trans_.z());

  float den = r20_*fw0 + r21_*fw1 + r22_+ t2;
  if (den<1e-6) {
    to_u = 0.0; to_v = 0.0;
    return false;
  }
  den = 1.0f/den;
  float ut = (r00_*fw0 + r01_*fw1 +r02_ + t0)*den;
  float vt = (r10_*fw0 + r11_*fw1 +r12_ + t1)*den;
  to_u = ut*to_fl_ + to_pu_;
  to_v = vt*to_fl_ + to_pv_;
  return true;
}

void icam_depth_transform::set_params(vnl_vector<double> const& params)
{
  vnl_vector<double> unscl_params = element_quotient(params, scale_factors_);
  vnl_vector_fixed<double, 3> rodv;
  for (unsigned i = 0; i<3; ++i)
    rodv[i]=unscl_params[i];
  rot_ = vgl_rotation_3d<double>(rodv);
  this->cache_r();
  trans_ = vgl_vector_3d<double>(unscl_params[3], unscl_params[4],
                                 unscl_params[5]);
  if (adjust_to_fl_) to_fl_ = unscl_params[6];
}

vnl_vector<double> icam_depth_transform::params()
{
  unsigned np = this->n_params();
  vnl_vector<double> res(np),scl_res;
  vnl_vector_fixed<double, 3> rodv = rot_.as_rodrigues();
  for (unsigned i = 0; i<3; ++i)
    res[i]=rodv[i];
  res[3]=trans_.x();   res[4]=trans_.y();   res[5]=trans_.z();
  if (adjust_to_fl_) res[6] = to_fl_;
  scl_res = element_product(res, scale_factors_);
  return scl_res;
}
