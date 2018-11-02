#ifndef bpgl_camera_estimator_hxx_
#define bpgl_camera_estimator_hxx_

#include <iostream>
#include <limits>
#include "bpgl_camera_estimator.h"
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_3x3.h>
#include <vil/vil_math.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// Default constructor
template <class exp_edge_func>
bpgl_camera_estimator<exp_edge_func>::bpgl_camera_estimator(const exp_edge_func& func)
: func_(func)
{
  // world_params_set_ = false;
  estimation_params_set_ = false;
}

template <class exp_edge_func>
void bpgl_camera_estimator<exp_edge_func>::set_estimation_params(double theta_range, double theta_step, double phi_range, double phi_step, double rot_range, double rot_step, int max_iter_rot_angle)
{
  theta_range_ = theta_range;
  theta_step_ = theta_step;
  phi_range_ = phi_range;
  phi_step_ = phi_step;
  rot_range_ = rot_range;
  rot_step_ = rot_step;
  max_iter_rot_angle_ = max_iter_rot_angle;
  estimation_params_set_ = true;
}

template <class exp_edge_func>
vil_image_view<float> bpgl_camera_estimator<exp_edge_func>::convert_to_spherical_coordinates(const vil_image_view<float> &img,
                                                                                              const vpgl_perspective_camera<double> &cam,
                                                                                              const double rotate)
{
  int theta_size = 2*vnl_math::ceil(theta_range_/theta_step_);
  int phi_size = 2*vnl_math::ceil(phi_range_/phi_step_);

  vil_image_view<float> imgs(theta_size,phi_size,1);
  imgs.fill(0.0f);

  vnl_double_3x3 K = cam.get_calibration().get_matrix();

  vnl_double_3x3 R(0.0);
  R(0,0) = 1.0;
  R(1,1) = std::cos(rotate);
  R(1,2) = std::sin(rotate);
  R(2,1) = -std::sin(rotate);
  R(2,2) = std::cos(rotate);

  for (unsigned i=0; i<imgs.ni(); i++) {
    for (unsigned j=0; j<imgs.nj(); j++) {
      double curr_theta = (0.5*vnl_math::pi) - theta_range_ + (theta_step_*(double)i);
      double curr_phi = -phi_range_ + (phi_step_*(double)j);

      double x = std::sin(curr_theta)*std::cos(curr_phi);
      double y = std::sin(curr_theta)*std::sin(curr_phi);
      double z = std::cos(curr_theta);

      vnl_double_3 curr_vector(x,y,z);
      curr_vector = R*curr_vector;
      x = curr_vector[0];
      y = curr_vector[1];
      z = curr_vector[2];

      curr_vector[0] = -z;
      curr_vector[1] = y;
      curr_vector[2] = x;
      vnl_double_3 curr_pixel = K*curr_vector;

      int u = vnl_math::rnd(curr_pixel[0]/curr_pixel[2]);
      int v = vnl_math::rnd(curr_pixel[1]/curr_pixel[2]);

      if (img.in_range(u,v)) {
        imgs(i,j) = img(u,v);
      }
    }
  }

  return imgs;
}


template <class exp_edge_func>
void bpgl_camera_estimator<exp_edge_func>::convert_angles_to_vector(const double theta,
                                                                     const double phi,
                                                                     double &vx,
                                                                     double &vy,
                                                                     double &vz)
{
  vx = std::sin(theta)*std::cos(phi);
  vy = std::sin(theta)*std::sin(phi);
  vz = std::cos(theta);
}

template <class exp_edge_func>
double bpgl_camera_estimator<exp_edge_func>::edge_prob_cross_correlation(const vil_image_view<float> &img1,
                                                                          const vil_image_view<float> &img2)
{
  vil_image_view<float> img1n; img1n.deep_copy(img1);
  vil_image_view<float> img2n; img2n.deep_copy(img2);

  vil_math_normalise<float>(img1n);
  vil_math_normalise<float>(img2n);

  vil_image_view<float> img_product;
  vil_math_image_product(img1n,img2n,img_product);

  double img_product_sum;
  vil_math_sum<float,double>(img_product_sum,img_product,0);

  return img_product_sum;
}


template <class exp_edge_func>
double bpgl_camera_estimator<exp_edge_func>::estimate_rotation_angle(const vil_image_view<float> &img1c,
                                                                      const vil_image_view<float> &img2c)
{
  assert(img1c.ni()==img2c.ni());
  assert(img1c.nj()==img2c.nj());

  double center_x = 0.5*(double)img1c.ni();
  double center_y = 0.5*(double)img1c.nj();

  vil_image_view<float> img1;
  brip_vil_float_ops::normalize_to_interval(img1c,img1,0.0f,1.0f);
  vil_image_view<float> img2;
  brip_vil_float_ops::normalize_to_interval(img2c,img2,0.0f,1.0f);

  int rot_size = vnl_math::ceil(rot_range_/rot_step_);

  double best_score = std::numeric_limits<double>::min();
  double best_rot = 0.0;

  for (int r=-rot_size; r<rot_size+1; r++) {
    double curr_rot = rot_step_*(double)r;

    vgl_h_matrix_2d<double> H;
    vgl_h_matrix_2d<double> H_temp;

    H.set_identity().set_translation(-center_x,-center_y);

    H_temp.set_identity().set_rotation(curr_rot);
    H = H_temp*H;

    H_temp.set_identity().set_translation(center_x,center_y);
    H = H_temp*H;

    vil_image_view<float> img2_rot(img2.ni(),img2.nj());

    brip_vil_float_ops::homography(img2, H, img2_rot,true,0.0f);

    double curr_score = edge_prob_cross_correlation(img1,img2_rot);

    if (curr_score > best_score) {
      best_score = curr_score;
      best_rot = curr_rot;
    }
  }

  return best_rot;
}

template <class exp_edge_func>
void bpgl_camera_estimator<exp_edge_func>::estimate_rotation_iterative(const vil_image_view<float>& img_e,
                                                                        vpgl_perspective_camera<double> *cam)
{
  if (!estimation_params_set_) {
    std::cerr << "Error: estimation parameters are not set\n";
    return;
  }

  int ni = img_e.ni();
  int nj = img_e.nj();

  for (int iter=0; iter<max_iter_rot_angle_; iter++) {
    std::cout << '.';
    vil_image_view<float> img_eei(ni,nj,1);
    vil_image_view<vxl_byte> img_temp(ni,nj,1);

    func_.apply(cam,&img_eei); // Gamze

    vil_image_view<float> img_eei_s = convert_to_spherical_coordinates(img_eei,*cam,0.0);
    vil_image_view<float> img_e_s = convert_to_spherical_coordinates(img_e,*cam,0.0);

    int offset_x, offset_y;
    float offset_score;
    estimate_offsets_fd<float,float,float>(img_e_s,img_eei_s,offset_x,offset_y,offset_score);

    if (offset_x==0 && offset_y==0) {
      break;
    }

    double theta = (0.5*vnl_math::pi) + (theta_step_*(double)offset_x);
    double phi = (phi_step_*(double)offset_y);

    double vx,vy,vz;
    bpgl_camera_estimator<exp_edge_func>::convert_angles_to_vector(theta,phi,vx,vy,vz);

    vgl_vector_3d<double> vec_from(0.0,0.0,1.0);
    vgl_vector_3d<double> vec_to(-vz,vy,vx);

    vgl_rotation_3d<double> rot_offsets(vec_from,vec_to);

    cam->set_rotation(rot_offsets*cam->get_rotation());

    // Estimating the rotation angle around the axis

    func_.apply(cam,&img_eei);

    double rot_ang = estimate_rotation_angle(img_e,img_eei);

    if (rot_ang==0.0) {
      break;
    }

    vnl_double_3 rot_rot_vec(0.0, 0.0, rot_ang);
    vgl_rotation_3d<double> rot_rot(rot_rot_vec);

    cam->set_rotation(rot_rot*cam->get_rotation());
  }
}

// Code for easy instantiation.
#undef BPGL_CAMERA_ESTIMATOR_INSTANTIATE
#define BPGL_CAMERA_ESTIMATOR_INSTANTIATE(T) \
template class bpgl_camera_estimator<T >

#endif
