#ifndef boxm2_vecf_orbit_params_h_
#define boxm2_vecf_orbit_params_h_
//:
// \file
// \brief  Parameters for the orbit model
//
// \author J.L. Mundy
// \date   27 Mar 2015
//
#include <iostream>
#include "boxm2_vecf_articulated_params.h"
#include <vnl/vnl_vector_fixed.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_sphere_3d.h>

class boxm2_vecf_orbit_params
: public boxm2_vecf_articulated_params
{
public:
  boxm2_vecf_orbit_params() :
    y_off_(0.0),
    x_off_coef_(0.1),
    x_min_(-1.0634),
    x_max_(0.99),
    x_marg_(0.1),
    trans_x_(0.0),
    trans_y_(0.0),
    trans_z_(0.0),
    superior_margin_t(0),
    eye_radius_(12.0),
    mid_inferior_margin_z_(eye_radius_+1.0),
    mid_superior_margin_z_(eye_radius_+1.0),
    mid_eyelid_crease_z_(eye_radius_),
    dphi_rad_(0.0),
    scale_x_coef_(0.0833333),
    scale_y_coef_(0.0833333),
    offset_(vgl_vector_3d<double>(0.0, 0.0, 0.0)),
    eye_pointing_dir_(vgl_vector_3d<double>(0.0, 0.0, 1.0)),
    eyelid_dt_(0.0),
    medial_socket_radius_coef_(0.75),
    lateral_socket_radius_coef_(0.9),
    iris_radius_(5.5),
    avg_iris_radius_(5.93),
    pupil_radius_(1.75),
    sclera_intensity_(static_cast<unsigned char>(250)),
    iris_intensity_(static_cast<unsigned char>(100)),
    pupil_intensity_(static_cast<unsigned char>(20)),
    eyelid_intensity_(160),
    eyelid_radius_offset_(3.0),
    inferior_lid_radius_offset_(eyelid_radius_offset_),
    eyelid_tmin_(0.0),
    eyelid_tmax_(0.95),
    lower_eyelid_intensity_(140),
    lower_eyelid_tmin_(1.05),
    lower_eyelid_tmax_(1.5),
    eyelid_crease_lower_intensity_(150),
    eyelid_crease_upper_intensity_(180),
    eyelid_crease_tmin_(-0.85),
    eyelid_crease_tmax_(1.05),
    eyelid_crease_scale_y_coef_(0.083333),
    eyelid_crease_ct_(0.0),
    brow_angle_rad_(0.5),
    brow_z_limit_(0.5),
    inferior_margin_xy_error_(0.0),
    inferior_margin_xyz_error_(0.0),
    superior_margin_xy_error_(0.0),
    superior_margin_xyz_error_(0.0),
    superior_crease_xy_error_(0.0),
    superior_crease_xyz_error_(0.0),
    mm_per_pix_(0.1),
    image_height_(1000.0)
  {
    app_.fill(static_cast<unsigned char>(0));
    app_[1]=static_cast<unsigned char>(32); app_[2] = static_cast<unsigned char>(255);
    init_sphere();
  }

  boxm2_vecf_orbit_params(double xmin,
                          double xmax,
                          double x_margin,
                          double eye_radius,
                          double iris_radius,
                          double pupil_radius,
                          double scale_x_coef,
                          double scale_y_coef,
                          unsigned char sclera_intensity,
                          unsigned char iris_intensity,
                          unsigned char pupil_intensity,
                          double eyelid_tmin,
                          double eyelid_tmax,
                          unsigned char eyelid_intensity,
                          double eyelid_dt,
                          double lower_eyelid_tmin,
                          double lower_eyelid_tmax,
                          unsigned char lower_eyelid_intensity,
                          double eyelid_crease_tmin,
                          double eyelid_crease_tmax,
                          unsigned char eyelid_crease_lower_intensity,
                          unsigned char eyelid_crease_upper_intensity,
                          double eyelid_crease_ct,
                          double brow_angle_rad) :
    x_min_(xmin),
    x_max_(xmax),
    x_marg_(x_margin),
    eye_radius_(eye_radius),
    scale_x_coef_(scale_x_coef),
    scale_y_coef_(scale_y_coef),
    eyelid_dt_(eyelid_dt),
    iris_radius_(iris_radius),
    pupil_radius_(pupil_radius),
    sclera_intensity_(sclera_intensity),
    iris_intensity_(iris_intensity),
    pupil_intensity_(pupil_intensity),
    eyelid_intensity_(eyelid_intensity),
    eyelid_tmin_(eyelid_tmin),
    eyelid_tmax_(eyelid_tmax),
    lower_eyelid_intensity_(lower_eyelid_intensity),
    lower_eyelid_tmin_(lower_eyelid_tmin),
    lower_eyelid_tmax_(lower_eyelid_tmax),
    eyelid_crease_lower_intensity_(eyelid_crease_lower_intensity),
    eyelid_crease_upper_intensity_(eyelid_crease_upper_intensity),
    eyelid_crease_tmin_(eyelid_crease_tmin),
    eyelid_crease_tmax_(eyelid_crease_tmax),
    eyelid_crease_ct_(eyelid_crease_ct),
    brow_angle_rad_(brow_angle_rad)
  {
    init_sphere();
  }

  void
  init_sphere()
  {
    //                                  eyelid/crease structure is positioned below the center of the iris by y_off_
    // ------------------------------------------V------------------------
    vgl_point_3d<double> c(trans_x_, trans_y_ - y_off_, trans_z_);
    sph_.set_centre(c);
    sph_.set_radius(eye_radius_);
    lid_sph_.set_centre(c);
    lid_sph_.set_radius(eyelid_radius());
  }

  //offset of eyelid with respect to the center of the pupil
  double y_off_;

  //x offset relative to radial distance to lateral canthus( a kludge factor to be eliminated at some point)
  double x_off_coef_;

  // horizontal limits of the inner and outer eye cusps, scaled relative to the eye radius
  double x_min_;
  double x_max_;
  // the additiona x margin to define the horizontal model extent
  double x_marg_;
  double x_min() const {return (1.0+x_marg_)*eye_radius_*x_min_;}
  double x_max() const {return (1.0+x_marg_)*eye_radius_*x_max_;}
  // translation parameters
  double trans_x_;
  double x_trans() const {return trans_x_;}
  double trans_y_;
  double y_trans() const {return trans_y_;}
  double trans_z_;
  double z_trans() const {return trans_z_;}

  double superior_margin_t;

  // the eyeball radius (may include the thickness of lateral and medial muscles)
  double eye_radius_;

  // the z coordinates at the middle of the margins (if 3-d data is available)
  double mid_inferior_margin_z_;
  double mid_superior_margin_z_;
  double mid_eyelid_crease_z_;

  // rotation about the z axis of the orbit model
  double dphi_rad_;
  double dphi_rad() const{ return dphi_rad_;}
  // for now, assume the crease and eyelid margins rotate together
  double crease_dphi_rad() const{ return dphi_rad_;}

  // scale parameters, the coeficients are normalized with respect to the eye_radius
  double scale_x_coef_;
  double scale_x() const {return scale_x_coef_*eye_radius_;}
  double scale_y_coef_;
  double scale_y() const {return scale_y_coef_*eye_radius_;}

  // the inferior-superior margin opening at x == 0 relative to (xmax_-xmin_)*eye_radius_
  // aspect ratio of the palpebral fissure
  double height_to_width_ratio();
  double height_to_width_ratio_;

  // vector to location of orbit in the head model
  vgl_vector_3d<double> offset_;

  // eye (pupil/iris)  pointing direction.
  vgl_vector_3d<double> eye_pointing_dir_;

  //movement of the upper eyelid opening (produces a vector field to move eyelid material)
  // dt is measured from the closed position (t = 1)
  double eyelid_dt_;

  // distance from pupil center to flat region surrounding the orbit
  ///start of planar region adjacent to nose
  double medial_socket_radius_coef_;
  double medial_socket_radius() const {return lid_sph_.radius()*medial_socket_radius_coef_;}

  ///start of planar region on side away from nose
  double lateral_socket_radius_coef_;
  double lateral_socket_radius() const {return lid_sph_.radius()*lateral_socket_radius_coef_;}

  // radius of the iris
  double iris_radius_;
  // the typical iris radius
  double avg_iris_radius_;
  // radius of the pupil
  double pupil_radius_;

  //intensities
  unsigned char sclera_intensity_;
  unsigned char iris_intensity_;
  unsigned char pupil_intensity_;
  unsigned char eyelid_intensity_;

  // essentially the thickness of the eyelid skin for superior lid
  double eyelid_radius_offset_;
  double eyelid_radius() const{ return eye_radius_ + eyelid_radius_offset_;}

  // offset for inferior lid
  double inferior_lid_radius_offset_;
  double inferior_lid_thickness() const {return inferior_lid_radius_offset_;}
  double inferior_lid_radius() const {return eyelid_radius_offset_+ eye_radius_;}

  // max and min limits of eyelid opening
  double eyelid_tmin_;
  double eyelid_tmax_;

  unsigned char lower_eyelid_intensity_;
  double lower_eyelid_tmin_;
  double lower_eyelid_tmax_;

  unsigned char eyelid_crease_lower_intensity_;
  unsigned char eyelid_crease_upper_intensity_;

  // limits of the position of the crease above the superior margin
  double eyelid_crease_tmin_;
  double eyelid_crease_tmax_;

  // scale factor for crease
  double eyelid_crease_scale_y_coef_;
  double eyelid_crease_scale_y() const
  {return eyelid_crease_scale_y_coef_*eye_radius_;}

  // the t value of the crease, that separates the upper and lower crease regions
  // i.e. the end of the spherical portion of the lid and beginning of the brow surface
  double eyelid_crease_ct_;

  //inclication angle of the brow above the upper eyelid
  double brow_angle_rad_;
  // the max z extent of the brow plane (needed to limit z range for high brow tilt)
  double brow_z_limit_;

  //outward normal to planar brow region
  vgl_vector_3d<double> upper_socket_normal() const
  {return {0.0,-std::sin(brow_angle_rad_), std::cos(brow_angle_rad_)};}
  boxm2_data_traits<BOXM2_MOG3_GREY>::datatype app_;

  //: vector of  monomials to form the polynomial when multiplied by the coefficients
  vnl_vector_fixed<double, 5> m(double xp) const{
    vnl_vector_fixed<double, 5> q;
    q[0]=1.0; q[1] = xp; q[2]=q[1]*q[1]; q[3]=q[2]*q[1]; q[4]=q[3]*q[1];
    return q;
  }
  //: lid and crease polynomial coefficients
  // more recently it was found that the cubic coefficient produced some
  // distorted results and is set to 0
  vnl_vector_fixed<double, 5> eyelid_coefs_t0() const{
    //double coefs[5]={5.70955, 0.128996, -0.0332313, 0.000466373, -0.00008886};
    double coefs[5]={5.70955, 0.128996, -0.0332313, 0.0, -0.00008886};//remove cubic term
    vnl_vector_fixed<double, 5> temp = vnl_vector_fixed<double, 5>(coefs);
    return temp;}

  vnl_vector_fixed<double, 5> eyelid_coefs_t1() const{
    //double coefs[5]={-3.85648, -0.0379334, 0.0140258, 0.00110097,0.00002418};
    double coefs[5]={-3.85648, -0.0379334, 0.0140258, 0.000,0.00002418};//remove cubic term
    vnl_vector_fixed<double, 5> temp = vnl_vector_fixed<double, 5>(coefs);
    return temp;}

  vnl_vector_fixed<double, 5> crease_coefs_t0() const{
    //double coefs[5]={8.91894, 0.0897925, -0.0264352, -0.000149888, -0.0001056};
    double coefs[5]={8.91894, 0.0897925, -0.0264352, 0.0, -0.0001056};//remove cubic term
    vnl_vector_fixed<double, 5> temp = vnl_vector_fixed<double, 5>(coefs);
    return temp;}
  // accuracy of fitting the orbit model to 2-d and 3-d data
  double inferior_margin_xy_error_;
  double inferior_margin_xyz_error_;
  double superior_margin_xy_error_;
  double superior_margin_xyz_error_;
  double superior_crease_xy_error_;
  double superior_crease_xyz_error_;
  // image properties
  double mm_per_pix_;
  double image_height_;
  vgl_sphere_3d<double> sph_;
  vgl_sphere_3d<double> lid_sph_;
};
std::ostream&  operator << (std::ostream& s, boxm2_vecf_orbit_params const& pr);
std::istream&  operator >> (std::istream& s, boxm2_vecf_orbit_params& pr);

#endif// boxm2_vecf_orbit_params
