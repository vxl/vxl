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
#include <vnl/vnl_vector_fixed.h>
struct boxm2_vecf_orbit_params{
boxm2_vecf_orbit_params(): x_min_(-1.167), x_max_(1.0),y_off_(-0.88), eye_radius_(12.0), iris_radius_(5.5),
    pupil_radius_(1.75), socket_radius_coef_(0.875), sclera_intensity_(static_cast<unsigned char>(250)),
    iris_intensity_(static_cast<unsigned char>(100)),
    pupil_intensity_(static_cast<unsigned char>(20)), eyelid_radius_offset_(1.0),
    eyelid_tmin_(0.1), eyelid_tmax_(0.95), eyelid_intensity_(160), eyelid_dt_(0.0),
    lower_eyelid_tmin_(1.05), lower_eyelid_tmax_(1.5),lower_eyelid_intensity_(160),
    eyelid_crease_tmin_(0.0), eyelid_crease_tmax_(1.05),eyelid_crease_lower_intensity_(150),
    eyelid_crease_upper_intensity_(180),eyelid_crease_ct_(0.6),brow_angle_rad_(1.3),  //brow_angle_rad_(0.87259),
    brow_z_limit_(0.25), scale_x_coef_(0.0833333), scale_y_coef_(0.0833333), offset_(vgl_vector_3d<double>(0.0, 0.0, 0.0)),
    trans_x_(1.0) , trans_y_(2.0), rot_ang_rad_(0.0),
                           //eye_pointing_dir_(vgl_vector_3d<double>(-0.26, 0.0, 0.968)){
    eye_pointing_dir_(vgl_vector_3d<double>(0.0, 0.0, 1.0)){
    app_.fill(static_cast<unsigned char>(0));
    app_[1]=static_cast<unsigned char>(32); app_[2] = static_cast<unsigned char>(255);
                                                                             }

    boxm2_vecf_orbit_params(double xmin, double xmax, double eye_radius, double iris_radius, double pupil_radius,
                        double socket_radius_coef, double scale_x_coef, double scale_y_coef,
                        unsigned char sclera_intensity, unsigned char iris_intensity,
                        unsigned char pupil_intensity, double eyelid_tmin, double eyelid_tmax,
                        unsigned char eyelid_intensity, double eyelid_dt, double lower_eyelid_tmin,
                        double lower_eyelid_tmax, unsigned char lower_eyelid_intensity,
                        double eyelid_crease_tmin, double eyelid_crease_tmax, unsigned char eyelid_crease_lower_intensity,
                        unsigned char eyelid_crease_upper_intensity, double eyelid_crease_ct, double brow_angle_rad):
  x_min_(xmin), x_max_(xmax), eye_radius_(eye_radius), iris_radius_(iris_radius),
    pupil_radius_(pupil_radius), socket_radius_coef_(socket_radius_coef),
    scale_x_coef_(scale_x_coef), scale_y_coef_(scale_y_coef),
    sclera_intensity_(sclera_intensity), iris_intensity_(iris_intensity), pupil_intensity_(pupil_intensity),
    eyelid_tmin_(eyelid_tmin), eyelid_tmax_(eyelid_tmax), eyelid_intensity_(eyelid_intensity), eyelid_dt_(eyelid_dt),
    lower_eyelid_tmin_(lower_eyelid_tmin), lower_eyelid_tmax_(lower_eyelid_tmax),lower_eyelid_intensity_(lower_eyelid_intensity),
    eyelid_crease_tmin_(eyelid_crease_tmin), eyelid_crease_tmax_(eyelid_crease_tmax),eyelid_crease_lower_intensity_(eyelid_crease_lower_intensity),
    eyelid_crease_upper_intensity_(eyelid_crease_upper_intensity), eyelid_crease_ct_(eyelid_crease_ct), brow_angle_rad_(brow_angle_rad){
    app_.fill(static_cast<unsigned char>(0));
    app_[1]=static_cast<unsigned char>(32); app_[2] = static_cast<unsigned char>(255);
  }  
  double rot_ang_rad_;//currently not implemented

  //offset of eyelid with respect to the center of the pupil
  double y_off_;

  // horizontal limits of the orbit domain, scaled relative to the eye radius
  double x_min_;
  double x_max_;
  double x_min() const {return eye_radius_*x_min_+x_trans();}
  double x_max() const {return eye_radius_*x_max_+y_trans();}

  // translation parameters
  double trans_x_;
  double x_trans() const {return trans_x_;}
  double trans_y_;
  double y_trans() const {return trans_y_ + y_off_;}
  double eye_radius_;

  // scale parameters
  double scale_x_coef_;
  double scale_x() const {return scale_x_coef_*eye_radius_;}
  double scale_y_coef_;
  double scale_y() const {return scale_y_coef_*eye_radius_;}

  // vector to location of orbit in the head model
  vgl_vector_3d<double> offset_;

  // eye (pupil/iris)  pointing direction.
  vgl_vector_3d<double> eye_pointing_dir_;

  //movement of the upper eyelid opening (produces a vector field to move eyelid material)
  // dt is measured from the closed position (t = 1) 
  double eyelid_dt_;

  // distance from pupil center to flat region surrounding eye sphere
  double socket_radius_coef_;
  double socket_radius() const {return eye_radius_*socket_radius_coef_;}

  double iris_radius_;
  double pupil_radius_;
  unsigned char sclera_intensity_;
  unsigned char iris_intensity_;
  unsigned char pupil_intensity_;
  unsigned char eyelid_intensity_;

  // essentially 1/2 the thickness of the eyelid skin
  double eyelid_radius_offset_;
  double eyelid_radius() const{ return eye_radius_ + eyelid_radius_offset_;}

  // max and min limits of eyelid opening
  double eyelid_tmin_;
  double eyelid_tmax_;

  unsigned char lower_eyelid_intensity_;
  double lower_eyelid_tmin_;
  double lower_eyelid_tmax_;

  unsigned char eyelid_crease_lower_intensity_;
  unsigned char eyelid_crease_upper_intensity_;
  double eyelid_crease_tmin_;
  double eyelid_crease_tmax_;

  // the t value of the crease, separated upper and lower crease regions
  double eyelid_crease_ct_;

  //inclication angle of the brow above the upper eyelid
  double brow_angle_rad_;
  // the max z extent of the brow plane (needed to limit z range for high brow tilt)
  double brow_z_limit_;

  //outward normal to planar brow region
  vgl_vector_3d<double> upper_socket_normal() const
  {return vgl_vector_3d<double>(0.0,-vcl_sin(brow_angle_rad_), vcl_cos(brow_angle_rad_));}
  boxm2_data_traits<BOXM2_MOG3_GREY>::datatype app_;

  //: vector of  monomials (includes x translation and scale)
  vnl_vector_fixed<double, 5> m(double xp) const{
    vnl_vector_fixed<double, 5> q;
    q[0]=1.0; q[1] = (xp-x_trans())/scale_x(); q[2]=q[1]*q[1]; q[3]=q[2]*q[1]; q[4]=q[3]*q[1];
    return q;
  }
  //: lid and crease polynomial coeficients
  // includes y translation and scale
  vnl_vector_fixed<double, 5> eyelid_coefs_t0() const{
    double coefs[5]={5.70955, 0.128996, -0.0332313, 0.000466373, -0.00008886};
    vnl_vector_fixed<double, 5> temp = scale_y()*vnl_vector_fixed<double, 5>(coefs);
    temp[0]+= y_trans(); return temp;}
  
  vnl_vector_fixed<double, 5> eyelid_coefs_t1() const{
    double coefs[5]={-3.85648, -0.0379334, 0.0140258, 0.00110097,0.00002418};
    vnl_vector_fixed<double, 5> temp = scale_y()*vnl_vector_fixed<double, 5>(coefs);
    temp[0] += y_trans();    return temp;}

  vnl_vector_fixed<double, 5> crease_coefs_t0() const{
    double coefs[5]={8.91894, 0.0897925, -0.0264352, -0.000149888, -0.0001056};
    vnl_vector_fixed<double, 5> temp = scale_y()*vnl_vector_fixed<double, 5>(coefs);
    temp[0] += y_trans(); return temp;}
  
  // internal voxel processing parameters
  double neighbor_radius() const {return 1.7320508075688772;}
  double gauss_sigma() const {return 0.75;}

};
#endif// boxm2_vecf_orbit_params

