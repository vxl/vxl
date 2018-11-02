#ifndef boxm2_vecf_fit_orbit_h_
#define boxm2_vecf_fit_orbit_h_
//:
// \file
// \brief  Fit parameters of the orbit model to data
//
// \author J.L. Mundy
// \date   28 May 2015
//
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "boxm2_vecf_labeled_point.h"
#include "boxm2_vecf_orbit_params.h"
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_sphere_3d.h>

class boxm2_vecf_fit_orbit{
 public:
  // ids for orbit measurements
  enum mids {LEFT_EYE_MEDIAL_CANTHUS, LEFT_EYE_LATERAL_CANTHUS, LEFT_EYE_INFERIOR_MARGIN,
             LEFT_EYE_SUPERIOR_MARGIN, LEFT_EYE_SUPERIOR_CREASE, LEFT_SCLERA, LEFT_INFERIOR_LID_SURFACE, LEFT_Nz,LEFT_IRIS_RADIUS,
             RIGHT_EYE_MEDIAL_CANTHUS, RIGHT_EYE_LATERAL_CANTHUS, RIGHT_EYE_INFERIOR_MARGIN,
             RIGHT_EYE_SUPERIOR_MARGIN, RIGHT_EYE_SUPERIOR_CREASE, RIGHT_SCLERA, RIGHT_INFERIOR_LID_SURFACE, RIGHT_Nz, RIGHT_IRIS_RADIUS};
 boxm2_vecf_fit_orbit(): nominal_canthus_angle_rad_(0.110498), has_inferior_surface_pts_(true), from_image_data_(false), image_height_(0.0)
    {fill_smid_map();}

  //: read a 3-d anchor file:
  // lateral, medial canthi, inferior/superior margins, superior crease
  bool read_anchor_file(std::string const& path);

  //: read a 2-d dlib part file (uses tree regression)
  bool read_dlib_part_file(std::string const& path, bool image_data = true);

  //: find initial orbit parameters for the left orbit
  bool fit_left();
  //: find initial orbit parameters for the right orbit
  bool fit_right();

  // in the case of image coordinates ===>
  // revert coordinates to approximate millimeters using the
  // iris radius
  bool from_image_data(){return from_image_data_;}
  void normalize_eye_data(float fixed_amount = 0.0f);
  // set the z values of the margin curve points and the crease curve points
  bool set_left_z_values();
  bool set_right_z_values();
  // <=======

  // Left orbit parameter fitting
  // see .cxx file for additional comments
  bool set_left_iris_radius();
  bool left_eye_radius( double& rad);
  void set_left_eye_radius(double rad){left_params_.eye_radius_=rad;}

  bool left_eye_socket_radii_coefs( double& lateral_radius_coef, double& medial_radius_coef);
  void set_left_eye_socket_radii_coefs(double lat_rad_coef, double med_rad_coef){
    left_params_.lateral_socket_radius_coef_ = lat_rad_coef;
    left_params_.medial_socket_radius_coef_ = med_rad_coef;
  }

  bool left_eye_inferior_lid_thickness(std::string const& data_desc, double& dr);
  void set_left_eye_inferior_lid_thickness(double dr){left_params_.inferior_lid_radius_offset_=dr;}

  bool left_trans_y_from_lateral_canthus( double& tr_y);
  void set_left_trans_y(double tr_y){left_params_.trans_y_=tr_y;}

  bool left_trans_z_from_sclera(std::string const& data_desc, double& tr_z);
  void set_left_trans_z(double tr_z){left_params_.trans_z_=tr_z;}

  bool left_trans_x_from_lateral_canthus(double& trx);
  void set_left_trans_x(double trx){left_params_.trans_x_=trx;}

  bool left_eye_x_scale(double& left_x_scale);
  void set_left_eye_x_scale(double& left_x_scale)
  {left_params_.scale_x_coef_ = left_x_scale*right_params_.scale_x_coef_;}

  // currently not used to estimate y_scale, nonlinear optimizer instead
  bool left_eye_y_scale(double& left_y_scale);
  void set_left_eye_y_scale(double& left_y_scale)
  {left_params_.scale_y_coef_ = left_y_scale*left_params_.scale_y_coef_;}

  // currently not used, nominal tmin is used instead
  bool left_eye_inferior_margin_t(double& left_inf_t);
  void set_left_eye_inferior_margin_t(double& left_inf_t){
    left_params_.lower_eyelid_tmin_ = left_inf_t;
  }

  // currently not used, nominal tmin is used instead
  bool left_eye_superior_margin_t(double& superior_margin_t);
  void set_left_eye_superior_margin_t(double superior_margin_t){
    left_params_.eyelid_dt_ = superior_margin_t-1.0;}

  bool left_eyelid_crease_scale_y(double& crease_scale_y);
  void set_left_eyelid_crease_scale_y(double& left_crease_y_scale){
    left_params_.eyelid_crease_scale_y_coef_ = left_crease_y_scale*left_params_.eyelid_crease_scale_y_coef_;
  }
  bool left_mid_inferior_margin_z(double& marg_z);
  void set_left_inferior_margin_z(double& marg_z){
    left_params_.mid_inferior_margin_z_= marg_z;
  }
  bool left_mid_superior_margin_z(double& marg_z);
  void set_left_superior_margin_z(double& marg_z){
    left_params_.mid_superior_margin_z_= marg_z;
  }
  bool left_mid_eyelid_crease_z(double& crease_z);
  void set_left_mid_eyelid_crease_z(double& crease_z){
    left_params_.mid_eyelid_crease_z_= crease_z;
  }
 // currently not used, nominal crease_ct is used instead
  bool left_eye_superior_margin_crease_t(double& left_sup_crease_t);
  void set_left_eye_superior_margin_crease_t(double& left_sup_crease_t){
    left_params_.eyelid_crease_ct_ = left_sup_crease_t;
  }
  // just for initialization
  bool left_ang_rad(double& ang_rad);
  void set_left_ang_rad(double& ang_rad);

  // determine the canthus angles (either left or right) from inf and sup polynomial crossing points
  void set_canthus_angle(bool is_right);

  bool fit_sclera(std::string const& data_desc);
  bool max_sclera_z(std::string const& data_desc, double r, double& max_z);


  //: Right orbit parameter fitting
  // see .cxx file for additional comments
  bool set_right_iris_radius();
  bool right_eye_radius( double& rad);
  void set_right_eye_radius(double rad){right_params_.eye_radius_=rad;}

  bool right_eye_socket_radii_coefs( double& lateral_radius_coef, double& medial_radius_coef);
  void set_right_eye_socket_radii_coefs(double lat_rad_coef, double med_rad_coef){
    right_params_.lateral_socket_radius_coef_ = lat_rad_coef;
    right_params_.medial_socket_radius_coef_ = med_rad_coef;
  }

  bool right_eye_inferior_lid_thickness(std::string const& data_desc, double& dr);
  void set_right_eye_inferior_lid_thickness(double dr){right_params_.inferior_lid_radius_offset_=dr;}

  bool right_trans_x_from_lateral_canthus(double& trx);

  //: can be determined from outer cusp or from the spherical fit to the sclera
  void set_right_trans_x(double trx){right_params_.trans_x_=trx;}

  //: determined from outer cusp or from the spherical fit to the sclera
  bool right_trans_y_from_lateral_canthus( double& tr_y);
  void set_right_trans_y(double tr_y){right_params_.trans_y_=tr_y;}

  bool right_trans_z_from_sclera(std::string const& data_desc, double& tr_z);
  void set_right_trans_z(double tr_z){right_params_.trans_z_=tr_z;}

  bool right_eye_x_scale(double& right_x_scale);
  void set_right_eye_x_scale(double& right_x_scale)
  {right_params_.scale_x_coef_ = right_x_scale*right_params_.scale_x_coef_;}

  // currently not used to estimate y_scale, nonlinear optimizer instead
  bool right_eye_y_scale(double& right_y_scale);
  void set_right_eye_y_scale(double& right_y_scale)
  {right_params_.scale_y_coef_ = right_y_scale*right_params_.scale_y_coef_;}

  // currently not used, nominal tmin is used instead
  bool right_eye_inferior_margin_t(double& right_inf_t);
  void set_right_eye_inferior_margin_t(double& right_inf_t){
    right_params_.lower_eyelid_tmin_ = right_inf_t;
  }

  // currently not used, nominal tmin is used instead
  bool right_eye_superior_margin_t(double& superior_margin_t);
  void set_right_eye_superior_margin_t(double superior_margin_t){
    right_params_.eyelid_dt_ = superior_margin_t-1.0;}

  bool right_eyelid_crease_scale_y(double& crease_scale_y);
  void set_right_eyelid_crease_scale_y(double& right_crease_y_scale){
    right_params_.eyelid_crease_scale_y_coef_ = right_crease_y_scale*right_params_.eyelid_crease_scale_y_coef_;
  }

  bool right_mid_inferior_margin_z(double& marg_z);
  void set_right_inferior_margin_z(double& marg_z){
    right_params_.mid_inferior_margin_z_= marg_z;
  }

  bool right_mid_superior_margin_z(double& marg_z);
  void set_right_superior_margin_z(double& marg_z){
    right_params_.mid_superior_margin_z_= marg_z;
  }

  bool right_mid_eyelid_crease_z(double& crease_z);
  void set_right_mid_eyelid_crease_z(double& crease_z){
    right_params_.mid_eyelid_crease_z_= crease_z;
  }

 // currently not used, nominal crease_ct is used instead
  bool right_eye_superior_margin_crease_t(double& right_sup_crease_t);
  void set_right_eye_superior_margin_crease_t(double& right_sup_crease_t){
    right_params_.eyelid_crease_tmin_ = right_sup_crease_t;
  }
  bool right_ang_rad(double& ang_rad);
  void set_right_ang_rad(double& ang_rad);

  // Testing the fit
  bool load_orbit_data(std::string const& data_desc, std::string const& path, bool error_msg = true);
  bool plot_orbit_data(std::string const& data_desc, std::vector<vgl_point_3d<double> >& data);
  bool display_anchors(std::ofstream& ostr, bool is_right);
  bool display_orbit_vrml(std::ofstream& ostr, bool is_right, bool show_model = true);
  bool display_left_right_orbit_model_vrml(std::ofstream& os);


  bool fitting_error(std::string const& data_desc);

  // Accessors
  //: add a labeled point to lpts_
  bool add_labeled_point(boxm2_vecf_labeled_point lp);

  boxm2_vecf_orbit_params left_params() const  {return left_params_;}
  void set_left_params(boxm2_vecf_orbit_params const& params);


  boxm2_vecf_orbit_params right_params() const {return right_params_;}
  void set_right_params(boxm2_vecf_orbit_params const& params);


  double left_dphi_rad() const {return left_dphi_rad_;}
  void set_left_dphi_rad(double dphi_rad){left_dphi_rad_ = dphi_rad;}

  double right_dphi_rad() const {return right_dphi_rad_;}
  void set_right_dphi_rad(double dphi_rad){right_dphi_rad_ = dphi_rad;}

  std::vector<vgl_point_3d<double> >& orbit_data(std::string const& data_desc){
    return orbit_data_[smid_map_[data_desc]];}

  bool lab_point(std::string const& data_desc, vgl_point_3d<double>& pt){
    std::map<mids, boxm2_vecf_labeled_point>::iterator lit;
    lit = lpts_.find(smid_map_[data_desc]);
    if(lit == lpts_.end())
      return false;
    pt = lit->second.p3d_;
    return true;
  }
  void set_has_inferior_surface(bool has_surface){has_inferior_surface_pts_ = has_surface;}
  //: helpers for parsing the dlib part file

  bool add_dlib_anchor_part(std::map<std::string, std::vector<vgl_point_2d<double> > > const& parts,
                            std::string const& dlabel, std::string const& olabel);
  bool add_dlib_orbit_data(std::map<std::string, std::vector<vgl_point_2d<double> > > const& parts,
                           std::string const& dlabel, std::string const& olabel);

  //setters required for dlib fit.
  void set_only_2d_data(bool is_from_dlib){this->only_2d_data_= is_from_dlib; }

  void set_image_height(double height){this->image_height_= height; }
 private:
  //: the map between string and enum
  void fill_smid_map();


  //: map a string label to the correspondingenum value
  std::map<std::string, mids> smid_map_;
  //: a map of labeled points, e.g. left_eye_lateral_canthus
  std::map<mids, boxm2_vecf_labeled_point> lpts_;

  //: a map of contour point data,
  // e.g. points along the inferior margin
  std::map<mids, std::vector<vgl_point_3d<double> > > orbit_data_;

  //: the current paramter estimates
  boxm2_vecf_orbit_params left_params_;
  boxm2_vecf_orbit_params right_params_;

  //: the typical canthus angle for estimating ang_rad from canthi (model rotation)
  double nominal_canthus_angle_rad_;

  //: estimates of the rotation of the
  // entire orbit model to best fit the data
  // should not be interpreted as the canthus angle
  double left_dphi_rad_;
  double right_dphi_rad_;

  //: are points on the inferior lid pouch available
  bool has_inferior_surface_pts_;
  //: are only 2-d data available (e.g. from dlib)
  bool only_2d_data_;
  //: are the data in image coordinates
  bool from_image_data_;
  //: if so the image height is needed for processing
  double image_height_;

};
#endif// boxm2_vecf_fit_orbit
