//This is brl/bbas/bpgl/bpgl_camera_estimator.h
#ifndef bpgl_camera_estimator_h_
#define bpgl_camera_estimator_h_
//:
// \file
// \brief A class for estimating perspective camera models
//
// \author Ibrahim Eden
// \date March 15, 2010
// \verbatim
//  Modifications
// \endverbatim


#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vpgl/vpgl_perspective_camera.h>

#include <brip/brip_vil_float_ops.h>

#include <vil/vil_flip.h>

template <class exp_edge_func>
class bpgl_camera_estimator
{
 public:

  bpgl_camera_estimator(const exp_edge_func& func);

  void set_estimation_params(double theta_range, double theta_step, double phi_range, double phi_step, double rot_range, double rot_step, int max_iter_rot_angle);

  vil_image_view<float> convert_to_spherical_coordinates(const vil_image_view<float> &img, const vpgl_perspective_camera<double> &cam, const double rotate);

  static void convert_angles_to_vector(const double theta, const double phi, double &vx, double &vy, double &vz);

  static double edge_prob_cross_correlation(const vil_image_view<float> &img1, const vil_image_view<float> &img2);

  template <class T1,class T2,class TR>
  vil_image_view<TR> estimate_offsets_fd(const vil_image_view<T1> &img1,
                                         const vil_image_view<T2> &img2,
                                         int &offset_x,
                                         int &offset_y,
                                         float &score);

  double estimate_rotation_angle(const vil_image_view<float> &img1c,
                                 const vil_image_view<float> &img2c);

  void estimate_rotation_iterative(const vil_image_view<float>& img_e,
                                   vpgl_perspective_camera<double> *cam);

  bool get_expected_edge_image(const vpgl_camera_double_sptr& curr_cam,
                               vil_image_view<float> *img_eei) { return func_.apply(curr_cam, img_eei); }

 private:
  exp_edge_func func_;

  //store parameters as global variables to be used across functions
  double theta_range_;
  double theta_step_;
  double phi_range_;
  double phi_step_;
  double rot_range_;
  double rot_step_;
  int max_iter_rot_angle_;
  bool estimation_params_set_;
};


// offset values needs to be used as following:
//   img1(i,j) = img2(i+offset_x,j+offset_y)
template <class exp_edge_func>
template <class T1,class T2,class TR>
vil_image_view<TR> bpgl_camera_estimator<exp_edge_func>::estimate_offsets_fd(const vil_image_view<T1> &img1,
                                                                             const vil_image_view<T2> &img2,
                                                                             int &offset_x,
                                                                             int &offset_y,
                                                                             float &score)
{
  vil_image_view<float> img_1;
  brip_vil_float_ops::normalize_to_interval<T1,float>(img1,img_1,0.0f,1.0f);
  vil_image_view<float> img_2;
  brip_vil_float_ops::normalize_to_interval<T2,float>(img2,img_2,0.0f,1.0f);

  vil_image_view<float> img_1_p;
  brip_vil_float_ops::resize_to_power_of_two(img_1, img_1_p);

  vil_image_view<float> img_2_p;
  brip_vil_float_ops::resize_to_power_of_two(img_2, img_2_p);

  vil_image_view<float> img_2_h = vil_flip_lr(img_2_p);
  vil_image_view<float> img_2_hv = vil_flip_ud(img_2_h);

  vil_image_view<float> img_1_n(img_1_p);
  vil_image_view<float> img_2_n(img_2_hv);

  vil_math_normalise<float>(img_1_n);
  vil_math_normalise<float>(img_2_n);

  vil_image_view<float> img_1_mag;
  vil_image_view<float> img_1_phase;
  brip_vil_float_ops::fourier_transform(img_1_n,img_1_mag,img_1_phase);

  vil_image_view<float> img_2_mag;
  vil_image_view<float> img_2_phase;
  brip_vil_float_ops::fourier_transform(img_2_n,img_2_mag,img_2_phase);

  vil_image_view<float> img_cc;
  vil_image_view<float> img_cc_mag(img_1_n.ni(),img_1_n.nj(),1);
  vil_image_view<float> img_cc_phase(img_1_n.ni(),img_1_n.nj(),1);

  for (unsigned i=0; i<img_cc_mag.ni(); i++) {
    for (unsigned j=0; j<img_cc_mag.nj(); j++) {
      float a = img_1_mag(i,j)*std::cos(img_1_phase(i,j));
      float b = img_1_mag(i,j)*std::sin(img_1_phase(i,j));
      float c = img_2_mag(i,j)*std::cos(img_2_phase(i,j));
      float d = img_2_mag(i,j)*std::sin(img_2_phase(i,j));
      float res_real = (a*c)-(b*d);
      float res_img = (b*c)+(a*d);
      float res_mag = std::sqrt((res_real*res_real)+(res_img*res_img));
      float res_phase = std::atan2(res_img, res_real);

      img_cc_mag(i,j) = res_mag;
      img_cc_phase(i,j) = res_phase;
    }
  }

  brip_vil_float_ops::inverse_fourier_transform(img_cc_mag,img_cc_phase,img_cc);

  score = std::numeric_limits<float>::min();

  for (unsigned i=0; i<img_cc.ni(); i++) {
    for (unsigned j=0; j<img_cc.nj(); j++) {
      if (img_cc(i,j)>score) {
        score = img_cc(i,j);
        offset_x = i;
        offset_y = j;
      }
    }
  }
  if (offset_x > (int)img_1_mag.ni()/2) {
    offset_x = offset_x - (int)img_1_mag.ni();
  }
  if (offset_y > (int)img_1_mag.nj()/2) {
    offset_y = offset_y - (int)img_1_mag.nj();
  }

  vil_image_view<TR> img_2_to_1(img_2.ni(),img_2.nj(),1);
  img_2_to_1.fill((TR)0);

  for (unsigned i=0; i<img_2_to_1.ni(); i++) {
    for (unsigned j=0; j<img_2_to_1.nj(); j++) {
      int curr_i = (int)i + offset_x;
      int curr_j = (int)j + offset_y;
      if (img_2_to_1.in_range(curr_i,curr_j)) {
        img_2_to_1(curr_i,curr_j) = img_2(i,j);
      }
    }
  }

  return img_2_to_1;
}

#endif // bpgl_camera_estimator_h_
