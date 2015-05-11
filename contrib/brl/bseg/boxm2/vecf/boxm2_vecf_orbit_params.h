#ifndef boxm2_vecf_orbit_params_h_
#define boxm2_vecf_orbit_params_h_
//:
// \file
// \brief  Parameters for the orbit model
//
// \author J.L. Mundy
// \date   27 Mar 2015
//
#include <boxm2/boxm2_data_traits.h>

struct boxm2_vecf_orbit_params{
boxm2_vecf_orbit_params(): x_mid_(11.88), y_off_(0.88),
    eye_radius_(12.0), iris_radius_(5.5),
    pupil_radius_(1.75), sclera_intensity_(static_cast<unsigned char>(250)),
    iris_intensity_(static_cast<unsigned char>(100)),
    pupil_intensity_(static_cast<unsigned char>(20)), eyelid_radius_(13.0),
    eyelid_tmin_(0.1), eyelid_tmax_(0.95), eyelid_intensity_(160), eyelid_dt_(0.0),
    lower_eyelid_tmin_(1.05), lower_eyelid_tmax_(1.5),lower_eyelid_intensity_(160),
    eyelid_crease_tmin_(0.0), eyelid_crease_tmax_(1.05),eyelid_crease_lower_intensity_(150),
    eyelid_crease_upper_intensity_(180),eyelid_crease_ct_(0.6),brow_angle_rad_(0.87259),
    offset_(vgl_vector_3d<double>(0.0, 0.0, 0.0)),
  //eye_pointing_dir_(vgl_vector_3d<double>(-0.26, 0.0, 0.968)){
    eye_pointing_dir_(vgl_vector_3d<double>(0.0, 0.0, 1.0)){
    app_.fill(static_cast<unsigned char>(0));
    app_[1]=static_cast<unsigned char>(32); app_[2] = static_cast<unsigned char>(255);
  }

boxm2_vecf_orbit_params(double eye_radius, double iris_radius, double pupil_radius,
                        unsigned char sclera_intensity, unsigned char iris_intensity,
                        unsigned char pupil_intensity,
                        double eyelid_tmin, double eyelid_tmax, unsigned char eyelid_intensity, double eyelid_dt,
                        double lower_eyelid_tmin, double lower_eyelid_tmax, unsigned char lower_eyelid_intensity,
                        double eyelid_crease_tmin, double eyelid_crease_tmax, unsigned char eyelid_crease_lower_intensity,
                        unsigned char eyelid_crease_upper_intensity, double eyelid_crease_ct, double brow_angle_rad):
  eye_radius_(eye_radius), iris_radius_(iris_radius),
    pupil_radius_(pupil_radius), sclera_intensity_(sclera_intensity), iris_intensity_(iris_intensity),
    pupil_intensity_(pupil_intensity),
    eyelid_tmin_(eyelid_tmin), eyelid_tmax_(eyelid_tmax), eyelid_intensity_(eyelid_intensity), eyelid_dt_(eyelid_dt),
    lower_eyelid_tmin_(lower_eyelid_tmin), lower_eyelid_tmax_(lower_eyelid_tmax),lower_eyelid_intensity_(lower_eyelid_intensity),
    eyelid_crease_tmin_(eyelid_crease_tmin), eyelid_crease_tmax_(eyelid_crease_tmax),eyelid_crease_lower_intensity_(eyelid_crease_lower_intensity),
    eyelid_crease_upper_intensity_(eyelid_crease_upper_intensity), eyelid_crease_ct_(eyelid_crease_ct), brow_angle_rad_(brow_angle_rad){
    app_.fill(static_cast<unsigned char>(0));
    app_[1]=static_cast<unsigned char>(32); app_[2] = static_cast<unsigned char>(255);
  }  
  
  double x_mid_;
  double y_off_;
  double eye_radius_;
  double iris_radius_;
  double pupil_radius_;
  unsigned char sclera_intensity_;
  unsigned char iris_intensity_;
  unsigned char pupil_intensity_;
  unsigned char eyelid_intensity_;

  double eyelid_radius_;
  double eyelid_dt_;
  double eyelid_tmin_;
  double eyelid_tmax_;

  unsigned char lower_eyelid_intensity_;
  double lower_eyelid_tmin_;
  double lower_eyelid_tmax_;

  unsigned char eyelid_crease_lower_intensity_;
  unsigned char eyelid_crease_upper_intensity_;
  double eyelid_crease_tmin_;
  double eyelid_crease_tmax_;
  double eyelid_crease_ct_;
  double brow_angle_rad_;

  boxm2_data_traits<BOXM2_MOG3_GREY>::datatype app_;
  vgl_vector_3d<double> offset_;
  vgl_vector_3d<double> eye_pointing_dir_;
  // internal parameters
  double neighbor_radius() const {return 1.7320508075688772;}
  double gauss_sigma() const {return 0.75;}
};
#endif// boxm2_vecf_orbit_params
