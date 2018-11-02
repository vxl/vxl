//This is brl/bseg/bvxm/pro/processes/bvxm_estimate_camera_utils.h
#ifndef bvxm_estimate_camera_utils_h_
#define bvxm_estimate_camera_utils_h_
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
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>

#include <vpgl/vpgl_perspective_camera.h>

#include <bvxm/bvxm_util.h>
#include <bvxm/grid/bvxm_voxel_grid.h>
#include <bvxm/grid/bvxm_voxel_slab.h>

#include <vnl/vnl_vector.h>
#include <vnl/vnl_cost_function.h>

#include <brip/brip_vil_float_ops.h>

#include <vil/vil_flip.h>

class bvxm_camera_estimator
{
 public:

  bvxm_camera_estimator();

  void set_world_params(int nx, int ny, int nz, double sx, double sy, double sz, double dx, double dy, double dz);
  void set_estimation_params(double theta_range, double theta_step, double phi_range, double phi_step, double rot_range, double rot_step, int max_iter_rot_angle);

  vil_image_view<float> convert_to_spherical_coordinates(const vil_image_view<float> &img, const vpgl_perspective_camera<double> &cam, const double rotate);

  static std::vector<vgl_point_3d<double> > convert_3d_box_to_3d_points(const vgl_box_3d<double> box_3d);
  static vgl_polygon<double> convert_3d_box_to_2d_polygon(const vgl_box_3d<double> box_3d, const vpgl_perspective_camera<double> *cam);

  static void convert_angles_to_vector(const double theta, const double phi, double &vx, double &vy, double &vz);

  static double edge_prob_cross_correlation(const vil_image_view<float> &img1, const vil_image_view<float> &img2);

  template <class T_from,class T_to>
  static void normalize_to_interval(const vil_image_view<T_from>& img_inp,
                                    vil_image_view<T_to>& img_out,
                                    float min,
                                    float max);

  void get_expected_edge_image(const bvxm_voxel_slab<float> &data,
                               const vpgl_perspective_camera<double> *cam,
                               vil_image_view<float> *img_eei);

  template <class T1,class T2,class TR>
  vil_image_view<TR> estimate_offsets_fd(const vil_image_view<T1> &img1,
                                         const vil_image_view<T2> &img2,
                                         int &offset_x,
                                         int &offset_y,
                                         float &score);

  double estimate_rotation_angle(const vil_image_view<float> &img1c,
                                 const vil_image_view<float> &img2c);

  void estimate_rotation_iterative(const bvxm_voxel_slab<float> &data,
                                   const vil_image_view<float>& img_e,
                                   vpgl_perspective_camera<double> *cam);
 private:
  //store parameters as global variables to be used across functions
  int nx_;
  int ny_;
  int nz_;
  double sx_;
  double sy_;
  double sz_;
  double dx_;
  double dy_;
  double dz_;
  bool world_params_set_;

  double theta_range_;
  double theta_step_;
  double phi_range_;
  double phi_step_;
  double rot_range_;
  double rot_step_;
  int max_iter_rot_angle_;
  bool estimation_params_set_;
};

template <class T_from,class T_to>
void bvxm_camera_estimator::normalize_to_interval(const vil_image_view<T_from>& img_inp,
                                                  vil_image_view<T_to>& img_out,
                                                  float min,
                                                  float max)
{
  assert(min<max);
  vil_image_view<float> img_temp;
  vil_convert_cast<T_from,float>(img_inp,img_temp);
  float min_img,max_img;
  vil_math_value_range<float>(img_temp,min_img,max_img);
  assert(min_img<max_img);

  float scale = (max-min)/(max_img-min_img);
  float offset = (min*max_img - min_img*max)/(max_img-min_img);

  vil_math_scale_and_offset_values<float,float>(img_temp,scale,offset);

  vil_convert_cast<float,T_to>(img_temp,img_out);
}

// offset values needs to be used as following:
//   img1(i,j) = img2(i+offset_x,j+offset_y)
template <class T1,class T2,class TR>
vil_image_view<TR> bvxm_camera_estimator::estimate_offsets_fd(const vil_image_view<T1> &img1,
                                                              const vil_image_view<T2> &img2,
                                                              int &offset_x,
                                                              int &offset_y,
                                                              float &score)
{
  vil_image_view<float> img_1;
  bvxm_camera_estimator::normalize_to_interval<T1,float>(img1,img_1,0.0f,1.0f);
  vil_image_view<float> img_2;
  bvxm_camera_estimator::normalize_to_interval<T2,float>(img2,img_2,0.0f,1.0f);

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

class bvxm_camera_estimator_amoeba : public vnl_cost_function
{
 public:
  bvxm_camera_estimator_amoeba(const bvxm_voxel_slab<float> &data,
                               bvxm_camera_estimator &cest,
                               const vil_image_view<float> &img_e,
                               const vpgl_perspective_camera<double> *cam)
  : vnl_cost_function(2), data_(data), cest_(cest), img_e_(img_e), cam_(cam)
  {
    best_score = 0.0;
    best_camera.set_calibration(cam->get_calibration());
    best_camera.set_rotation(cam->get_rotation());
    best_camera.set_camera_center(cam->get_camera_center());

    vpgl_perspective_camera<double> cam_centered(cam->get_calibration(),vgl_point_3d<double>(0.0,0.0,0.0),cam->get_rotation());
    double im_center_x = 0.5*(double)img_e.ni();
    double im_center_y = 0.5*(double)img_e.nj();

    vgl_line_3d_2_points<double> ln_center = cam_centered.backproject(vgl_point_2d<double>(im_center_x,im_center_y));
    vgl_line_3d_2_points<double> ln_center_x = cam_centered.backproject(vgl_point_2d<double>(im_center_x+1.0,im_center_y));
    vgl_line_3d_2_points<double> ln_center_y = cam_centered.backproject(vgl_point_2d<double>(im_center_x,im_center_y-1.0));

    vec_x = (ln_center_x.point2() - ln_center.point2());
    vec_y = (ln_center_y.point2() - ln_center.point2());

    normalize(vec_x);
    normalize(vec_y);

    cam_center = cam->get_camera_center();
    cam_center = cam_center - 2.0*vec_x;
    cam_center = cam_center - 2.0*vec_y;
  }

  double get_result(vnl_vector<double>& x, vpgl_perspective_camera<double> *cam)
  {
    cam->set_calibration(best_camera.get_calibration());
    cam->set_camera_center(best_camera.get_camera_center());
    cam->set_rotation(best_camera.get_rotation());
    std::cout << '\n';
    return best_score;
  }

  double f(const vnl_vector<double>& x) override
  {
    vgl_point_3d<double> curr_center = (cam_center + (x[0]*vec_x)) + (x[1]*vec_y);

    vpgl_perspective_camera<double> curr_cam(cam_->get_calibration(),curr_center,cam_->get_rotation());

    cest_.estimate_rotation_iterative(data_,img_e_,&curr_cam);
    vil_image_view<float> img_eei(img_e_.ni(),img_e_.nj());
    cest_.get_expected_edge_image(data_,&curr_cam,&img_eei);

    double curr_score = cest_.edge_prob_cross_correlation(img_e_,img_eei);

    if (curr_score > best_score) {
      best_score = curr_score;
      best_camera.set_camera_center(curr_cam.get_camera_center());
      best_camera.set_rotation(curr_cam.get_rotation());
    }
    return -curr_score;
  }

  const bvxm_voxel_slab<float> &data_;
  bvxm_camera_estimator &cest_;
  const vil_image_view<float> &img_e_;
  const vpgl_perspective_camera<double> *cam_;
  double best_score;
  vpgl_perspective_camera<double> best_camera;
  vgl_vector_3d<double> vec_x;
  vgl_vector_3d<double> vec_y;
  vgl_point_3d<double> cam_center;
};

#endif // bvxm_estimate_camera_utils_h_
