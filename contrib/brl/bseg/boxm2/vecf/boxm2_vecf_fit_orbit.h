#ifndef boxm2_vecf_fit_orbit_h_
#define boxm2_vecf_fit_orbit_h_
//:
// \file
// \brief  Fit parameters of the orbit model to data
//
// \author J.L. Mundy
// \date   28 May 2015
//
#include <vcl_string.h>
#include <vcl_iostream.h>
#include "boxm2_vecf_orbit_params.h"
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_sphere_3d.h>
#include <vcl_map.h>
struct labeled_point{
public:
  labeled_point(){}
  labeled_point(double x, double y, double z, vcl_string const& label):
  p3d_(vgl_point_3d<double>(x,y,z)), label_(label){}
  vgl_point_3d<double> p3d_;
  vcl_string label_;
};
class boxm2_vecf_fit_orbit{
 public:
  // ids for orbit measurements
  enum mids {LEFT_EYE_INNER_CUSP, LEFT_EYE_OUTER_CUSP, LEFT_EYE_MID_LOWER_LID,
             LEFT_EYE_MID_UP_LID, LEFT_EYE_MID_CREASE, LEFT_SCLERA, LEFT_Nz,LEFT_IRIS_RADIUS,
             RIGHT_EYE_INNER_CUSP, RIGHT_EYE_OUTER_CUSP, RIGHT_EYE_MID_LOWER_LID,
             RIGHT_EYE_MID_UP_LID, RIGHT_EYE_MID_CREASE, RIGHT_SCLERA, RIGHT_Nz, RIGHT_IRIS_RADIUS};
  boxm2_vecf_fit_orbit(){fill_smid_map();};
  boxm2_vecf_orbit_params fitted_params();
  bool add_labeled_point(labeled_point lp);
  bool read_anchor_file(vcl_string const& path);
  
  bool fit_left();
  bool fit_right();


  // Left orbit parameter fitting
  bool left_eye_radius( double& rad);
  void set_left_eye_radius(double rad){left_params_.eye_radius_=rad;}

  bool left_trans_y_from_outer_cusp( double& tr_y);
  void set_left_trans_y(double tr_y){left_params_.trans_y_=tr_y;}

  void set_left_trans_z(double tr_z){left_params_.trans_z_=tr_z;}

  bool left_trans_x_from_outer_cusp(double& trx);
  void set_left_trans_x(double trx){left_params_.trans_x_=trx;}

  bool left_eye_x_scale(double& left_x_scale);
  void set_left_eye_x_scale(double& left_x_scale)
  {left_params_.scale_x_coef_ = left_x_scale*right_params_.scale_x_coef_;}

  bool left_eye_y_scale(double& left_y_scale);
  void set_left_eye_y_scale(double& left_y_scale)
  {left_params_.scale_y_coef_ = left_y_scale*left_params_.scale_y_coef_;}
  
  bool left_eye_upper_lid_t(double& upper_lid_t);
  void set_left_eye_upper_lid_t(double upper_lid_t){
    left_params_.eyelid_dt_ = upper_lid_t-1.0;}

  bool left_eyelid_crease_scale_y(double& crease_scale_y);
  void set_left_eyelid_crease_scale_y(double& left_crease_y_scale){
    left_params_.eyelid_crease_scale_y_coef_ = left_crease_y_scale*left_params_.eyelid_crease_scale_y_coef_;
  }

  bool fit_sclera(vcl_string const& data_desc);


  //: Right orbit parameter fitting
  bool right_eye_radius( double& rad);
  void set_right_eye_radius(double rad){right_params_.eye_radius_=rad;}

  bool right_trans_x_from_outer_cusp(double& trx);

  //: can be determined from outer cusp or from the spherical fit to the sclera
  void set_right_trans_x(double trx){right_params_.trans_x_=trx;}

  //: determined from outer cusp or from the spherical fit to the sclera
  bool right_trans_y_from_outer_cusp( double& tr_y);
  void set_right_trans_y(double tr_y){right_params_.trans_y_=tr_y;}

  void set_right_trans_z(double tr_z){right_params_.trans_z_=tr_z;}

  bool right_eye_x_scale(double& right_x_scale);
  void set_right_eye_x_scale(double& right_x_scale)
  {right_params_.scale_x_coef_ = right_x_scale*right_params_.scale_x_coef_;}

  bool right_eye_y_scale(double& right_y_scale);
  void set_right_eye_y_scale(double& right_y_scale)
  {right_params_.scale_y_coef_ = right_y_scale*right_params_.scale_y_coef_;}
  
  bool right_eye_upper_lid_t(double& upper_lid_t);
  void set_right_eye_upper_lid_t(double upper_lid_t){
    right_params_.eyelid_dt_ = upper_lid_t-1.0;}

  bool right_eyelid_crease_scale_y(double& crease_scale_y);
  void set_right_eyelid_crease_scale_y(double& right_crease_y_scale){
    right_params_.eyelid_crease_scale_y_coef_ = right_crease_y_scale*right_params_.eyelid_crease_scale_y_coef_;
  }


  // Testing the fit
  bool load_orbit_data(vcl_string const& data_desc, vcl_string const& path);
  bool plot_orbit_data(vcl_string const& data_desc);
  
 private:
  void fill_smid_map();
  void plot_lower_lid(vcl_ostream& ostr, bool is_right);
  void plot_upper_lid(vcl_ostream& ostr, bool is_right);
  void plot_crease(vcl_ostream& ostr, bool is_right);
  vcl_map<vcl_string, mids> smid_map_;
  vcl_map<mids, labeled_point> lpts_;
  boxm2_vecf_orbit_params left_params_;
  boxm2_vecf_orbit_params right_params_;
  vcl_map<mids, vcl_vector<vgl_point_3d<double> > > orbit_data_;
  vgl_sphere_3d<double> left_sphere_;
  vgl_sphere_3d<double> right_sphere_;
};
#endif// boxm2_vecf_fit_orbit
