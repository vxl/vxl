#include "icam_depth_trans_pyramid.h"
//
#include <vil/vil_pyramid_image_view.h>
#include <vil/algo/vil_gauss_filter.h>
#include <vnl/vnl_inverse.h>

unsigned icam_depth_trans_pyramid::
required_levels(unsigned ni,unsigned nj,unsigned min_size)
{
  //find minimum image dimension
  double m = ni, mns = min_size;
  if (nj<ni) m = nj;
  //required levels
  double nlevls = 1.0+ std::log(mns/m)/std::log(0.5);
  auto ncheck = static_cast<unsigned>(nlevls);
  double temp = static_cast<unsigned>(std::pow(0.5, double(ncheck-1))*m);
  if (temp<mns&&ncheck>=2)
    return ncheck-1;
  return ncheck;
}

vnl_matrix_fixed<double, 3, 3> icam_depth_trans_pyramid::
K_to(unsigned level)
{
  vnl_matrix_fixed<double,3,3> ret(0.0);
  double s = to_scales_[level];
  ret[0][0] = s*to_fl_;
  ret[0][2] = s*to_pu_;
  ret[1][1] = s*to_fl_;
  ret[1][2] = s*to_pv_;
  ret[2][2] = 1.0;
  return ret;
}

void icam_depth_trans_pyramid::init(icam_depth_transform& base_transform,
                                    unsigned n_levels)
{
  adjust_to_fl_ = base_transform.adjust_to_fl();
  n_params_ = base_transform.n_params();
  n_levels_ = n_levels;
  rot_ = base_transform.rotation();
  trans_ = base_transform.translation();
  vnl_matrix_fixed<double, 3, 3> K_inv =
    base_transform.from_calibration_matrix_inv();
  K_from_.push_back(vnl_inverse(K_inv));
  to_fl_ = base_transform.to_fl();
  to_pu_ = base_transform.to_pu();
  to_pv_ = base_transform.to_pv();
  vil_image_view_base_sptr depth_sptr =
    new vil_image_view<double>(base_transform.depth_map());
  depth_pyramid_=vil_pyramid_image_view<double>(depth_sptr,n_levels);
  vnl_matrix_fixed<double, 3, 3> s(0.0);
  s[0][0] = 0.5;  s[1][1] = 0.5;   s[2][2] = 1.0;
  to_scales_.push_back(1.0);
  for (unsigned i = 1; i<n_levels; ++i) {
    K_from_.push_back(s*K_from_[i-1]);
    to_scales_.push_back(0.5*to_scales_[i-1]);
  }
  scale_factors_=base_transform.scale_factors();
}

icam_depth_trans_pyramid::
icam_depth_trans_pyramid(icam_depth_transform& base_transform,
                         unsigned n_levels)
{
  this->init(base_transform, n_levels);
}

icam_depth_trans_pyramid::
icam_depth_trans_pyramid(vnl_matrix_fixed<double, 3, 3> const& K,
                         vil_image_view<double> const& depth,
                         vgl_rotation_3d<double> const& rot,
                         vgl_vector_3d<double> const& trans,
                         unsigned n_levels,
                         bool adjust_to_fl
                        )
{
  icam_depth_transform base_transform(K, depth, rot, trans, adjust_to_fl);
  this->init(base_transform, n_levels);
  scale_factors_.set_size(n_params());
  scale_factors_.fill(1.0);
}

icam_depth_trans_pyramid::
  icam_depth_trans_pyramid(vnl_matrix_fixed<double, 3, 3> const& K_from,
                           vnl_matrix_fixed<double, 3, 3> const& K_to,
                           vil_image_view<double> const& depth,
                           vgl_rotation_3d<double> const& rot,
                           vgl_vector_3d<double> const& trans,
                           unsigned n_levels,
                           bool adjust_to_fl
                          )
{
  icam_depth_transform base_transform(K_from, K_to, depth, rot,
                                      trans, adjust_to_fl);
  this->init(base_transform, n_levels);
  scale_factors_.set_size(n_params());
  scale_factors_.fill(1.0);
}

icam_depth_trans_pyramid::
icam_depth_trans_pyramid(vnl_matrix_fixed<double, 3, 3> const& K_from,
                         double to_fl, double to_pu, double to_pv,
                         vil_image_view<double> const& depth,
                         vgl_rotation_3d<double> const& rot,
                         vgl_vector_3d<double> const& trans,
                         unsigned n_lev,
                         bool adjust_to_fl
                        )
{
  icam_depth_transform base_transform(K_from, to_fl, to_pu, to_pv, depth, rot,
                                      trans, adjust_to_fl);
  this->init(base_transform, n_lev);
  scale_factors_.set_size(n_params());
  scale_factors_.fill(1.0);
}

void icam_depth_trans_pyramid::set_params(vnl_vector<double> const& params)
{
  vnl_vector<double> unscl_params = element_quotient(params, scale_factors_);
  vnl_vector_fixed<double, 3> rodv;
  for (unsigned i = 0; i<3; ++i)
    rodv[i]=unscl_params[i];
  rot_ = vgl_rotation_3d<double>(rodv);
  trans_ = vgl_vector_3d<double>(unscl_params[3], unscl_params[4],
                                 unscl_params[5]);
  if (adjust_to_fl_) to_fl_ = unscl_params[6];
}

vnl_vector<double> icam_depth_trans_pyramid::params()
{
  vnl_vector<double> res(n_params_), scl_res;
  vnl_vector_fixed<double, 3> rodv = rot_.as_rodrigues();
  for (unsigned i = 0; i<3; ++i)
    res[i]=rodv[i];
  res[3]=trans_.x();   res[4]=trans_.y();   res[5]=trans_.z();
  if (adjust_to_fl_)
    res[6]=to_fl_;
  scl_res = element_product(res, scale_factors_);
  return scl_res;
}

icam_depth_transform icam_depth_trans_pyramid::depth_trans(unsigned level,
                                                           bool smooth_map,
                                                           double smooth_sigma)
{
  vil_image_view<double>& dmap = this->depth_pyramid_(level);
  if (smooth_map) {
    vil_image_view<double> dmap_sm(dmap.ni(), dmap.nj());
    vil_gauss_filter_5tap(dmap,dmap_sm,
                          vil_gauss_filter_5tap_params(smooth_sigma));
    icam_depth_transform dt(this->calibration_matrix(level),
                            dmap_sm, rot_, trans_);
    dt.set_scale_factors(scale_factors_);
    return dt;
  }
  double s = to_scales_[level];
  icam_depth_transform dt(this->from_calibration_matrix(level),
                          s*to_fl_, s*to_pu_, s*to_pv_,
                          dmap, rot_, trans_, adjust_to_fl_);
  dt.set_scale_factors(scale_factors_);
  return dt;
}
