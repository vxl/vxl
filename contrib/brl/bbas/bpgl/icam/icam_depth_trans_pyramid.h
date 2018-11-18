// This is bbas/bpgl/icam/icam_depth_trans_pyramid.h
#ifndef icam_depth_trans_pyramid_h_
#define icam_depth_trans_pyramid_h_
//:
// \file
// \brief A pyramid of icam_depth_transforms
// \author J.L. Mundy
// \date Sept 09, 2010
//
// \verbatim
//  Modifications
//   None
// \endverbatim

#include <iostream>
#include <vector>
#include <icam/icam_depth_transform.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_vector.h>
#include <vil/vil_image_view.h>
#include <vil/vil_pyramid_image_view.h>
#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class icam_depth_trans_pyramid
{
 public:
  icam_depth_trans_pyramid(): n_levels_(0) {}
  icam_depth_trans_pyramid(icam_depth_transform & base_transform,
                           unsigned n_levels);
  icam_depth_trans_pyramid(vnl_matrix_fixed<double, 3, 3> const& K,
                           vil_image_view<double> const& depth,
                           vgl_rotation_3d<double> const& rot,
                           vgl_vector_3d<double> const& trans,
                           unsigned n_levels,
                           bool adjust_to_fl = false);
  icam_depth_trans_pyramid(vnl_matrix_fixed<double, 3, 3> const& K_from,
                           vnl_matrix_fixed<double, 3, 3> const& K_to,
                           vil_image_view<double> const& depth,
                           vgl_rotation_3d<double> const& rot,
                           vgl_vector_3d<double> const& trans,
                           unsigned n_levels,
                           bool adjust_to_fl = false);
  icam_depth_trans_pyramid(vnl_matrix_fixed<double, 3, 3> const& K_from,
                           double to_fl, double to_pu, double to_pv,
                           vil_image_view<double> const& depth,
                           vgl_rotation_3d<double> const& rot,
                           vgl_vector_3d<double> const& trans,
                           unsigned n_levels,
                           bool adjust_to_fl = false);
  ~icam_depth_trans_pyramid() = default;
  //: determines the number of pyramid levels to reduce the smallest image dimension to greater than or equal to \a min_size
  static unsigned required_levels(unsigned ni,unsigned nj,
                                  unsigned min_size = 127);
  unsigned n_levels() const {return n_levels_;}
  void set_scale_factors(vnl_vector<double> const& scales)
  { scale_factors_ = scales; }
  vnl_matrix_fixed<double, 3, 3>& calibration_matrix(unsigned level)
  { return K_from_[level]; }
  vnl_matrix_fixed<double, 3, 3>& from_calibration_matrix(unsigned level)
  { return K_from_[level]; }
  vnl_matrix_fixed<double, 3, 3> to_calibration_matrix(unsigned level)
  { return K_to(level); }
  vil_image_view<double>& depth_map(unsigned level)
  { return depth_pyramid_(level); }
  void set_to_fl(double to_fl) {to_fl_ = to_fl;}
  void set_to_pu(double to_pu) {to_pu_ = to_pu;}
  void set_to_pv(double to_pv) {to_pv_ = to_pv;}
  double to_fl() {return to_fl_;}
  double to_pu() {return to_pu_;}
  double to_pv() {return to_pv_;}
  void set_rotation(vgl_rotation_3d<double> const& rot) {rot_ = rot;}
  vgl_rotation_3d<double>& rotation() {
    return rot_;}
  void set_translation(vgl_vector_3d<double> const& trans) {trans_ = trans;}
  vgl_vector_3d<double>& translation() {return trans_;}
  icam_depth_transform depth_trans(unsigned level, bool smooth_map = false,
                                   double smooth_sigma = 1.0);
  unsigned n_params() {return n_params_;}
  void set_params(vnl_vector<double> const& params);
  vnl_vector<double> params();
  vil_image_view<double> depth(unsigned level)
    {return depth_pyramid_(level);}
 private:
  void init(icam_depth_transform& base_transform,
            unsigned n_levels);
  vnl_matrix_fixed<double, 3, 3> K_to(unsigned level);
  bool adjust_to_fl_;
  unsigned n_params_;
  unsigned n_levels_;
  vil_pyramid_image_view<double> depth_pyramid_;
  std::vector<vnl_matrix_fixed<double, 3, 3> > K_from_;
  //elements of the 'to' calibration matrix
  std::vector<double> to_scales_;
  double to_fl_;
  double to_pu_;
  double to_pv_;
  vgl_rotation_3d<double> rot_;
  vgl_vector_3d<double> trans_;
  vnl_vector<double> scale_factors_;
};

#endif // icam_depth_trans_pyramid_h_
