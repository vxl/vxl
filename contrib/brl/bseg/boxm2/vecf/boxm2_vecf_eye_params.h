#ifndef boxm2_vecf_eye_params_h_
#define boxm2_vecf_eye_params_h_
//:
// \file
// \brief  Parameters for the eye model
//
// \author J.L. Mundy
// \date   9 Dec 2014
//
#include <boxm2/boxm2_data_traits.h>
struct boxm2_vecf_eye_params{
boxm2_vecf_eye_params():
  eye_radius_(12.0), iris_radius_(6.0),
    pupil_radius_(1.75), sclera_intensity_(static_cast<unsigned char>(250)),
    iris_intensity_(static_cast<unsigned char>(80)),
    pupil_intensity_(static_cast<unsigned char>(20)),offset_(vgl_vector_3d<double>(36.6, 31.4, 65.7)),
    eye_pointing_dir_(vgl_vector_3d<double>(0.707, 0.0, 0.707)){
    app_.fill(static_cast<unsigned char>(0));
    app_[1]=static_cast<unsigned char>(32); app_[2] = static_cast<unsigned char>(255);
  }

boxm2_vecf_eye_params(double eye_radius, double iris_radius, double pupil_radius,
           unsigned char sclera_intensity, unsigned char iris_intensity,
           unsigned char pupil_intensity):
  eye_radius_(eye_radius), iris_radius_(iris_radius),
    pupil_radius_(pupil_radius), sclera_intensity_(sclera_intensity), iris_intensity_(iris_intensity),
    pupil_intensity_(pupil_intensity){
    app_.fill(static_cast<unsigned char>(0));
    app_[1]=static_cast<unsigned char>(32); app_[2] = static_cast<unsigned char>(255);
  }

  double eye_radius_;
  double iris_radius_;
  double pupil_radius_;
  unsigned char sclera_intensity_;
  unsigned char iris_intensity_;
  unsigned char pupil_intensity_;
  boxm2_data_traits<BOXM2_MOG3_GREY>::datatype app_;
  vgl_vector_3d<double> offset_;
  vgl_vector_3d<double> eye_pointing_dir_;
  // internal parameters
  double sphere_neighbor_radius() const {return 1.73205;}
  double gauss_sigma() const {return 0.75;}
  double angle_inc() const {return 0.0075;}
};
#endif// boxm2_vecf_eye_params
