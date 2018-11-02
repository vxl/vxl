#include <iostream>
#include "boxm2_vecf_orbit_params.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
double
boxm2_vecf_orbit_params
::height_to_width_ratio()
{
  double width = eye_radius_*(x_max_-x_min_)*scale_x();
  vnl_vector_fixed<double, 5> mm = this->m(0.0);
  vnl_vector_fixed<double, 5> c0 = eyelid_coefs_t0();
  vnl_vector_fixed<double, 5> c1 = eyelid_coefs_t1();
  double dp0 = dot_product(mm, c0 ), dp1 = dot_product(mm, c1 );
  double height = dp0-dp1;
  height_to_width_ratio_ = (height*std::cos(dphi_rad_)*scale_y())/width;
  return height_to_width_ratio_;
}

std::ostream&  operator<<(std::ostream& s, boxm2_vecf_orbit_params const& prc){
  if(!s){
    std::cout << "Bad parameter file stream\n";
    return s;
  }
  // bad practice --- but easy
  auto & pr = const_cast<boxm2_vecf_orbit_params &>(prc);
  s << "eye_radius: " << pr.eye_radius_ << '\n';
  s << "iris_radius: " << pr.iris_radius_ << '\n';
  s << "medial_socket_radius: " << pr.medial_socket_radius_coef_ << '\n';
  s << "lateral_socket_radius: " << pr.lateral_socket_radius_coef_ << '\n';
  s << "trans_x: " << pr.x_trans() <<  '\n';
  s << "trans_y: " << pr.y_trans() <<  '\n';
  s << "trans_z: " << pr.z_trans() <<  '\n';
  s << "mm_per_pix: " << pr.mm_per_pix_ << '\n';
  s << "image_height: "  << pr.image_height_ << '\n';
  s << "x_scale: " << pr.scale_x() << '\n';
  s << "y_scale: " << pr.scale_y() << '\n';
  s << "crease_y_scale: " << pr.eyelid_crease_scale_y() << '\n';
  s << "phi_rotation_angle(deg): " << pr.dphi_rad_*(180.0/3.14159) << '\n';
  s << "inferior_lid_thickness: " << pr.inferior_lid_thickness() << '\n';
  s << "height_to_width_ratio: " << pr.height_to_width_ratio() << '\n';
  s << "mid_inf_margin_z: " << pr.mid_inferior_margin_z_<< '\n';
  s << "mid_sup_margin_z: " << pr.mid_superior_margin_z_<< '\n';
  s << "mid_crease_z: " << pr.mid_eyelid_crease_z_<< '\n';
  s << "inferior_margin_xy_error: " << pr.inferior_margin_xy_error_<< '\n';
  s << "inferior_margin_xyz_error: " << pr.inferior_margin_xyz_error_<< '\n';
  s << "superior_margin_xy_error: " << pr.superior_margin_xy_error_<< '\n';
  s << "superior_margin_xyz_error: " << pr.superior_margin_xyz_error_<< '\n';
  s << "superior_crease_xy_error: " << pr.superior_crease_xy_error_<< '\n';
  s << "superior_crease_xyz_error: " << pr.superior_crease_xyz_error_<< '\n';
  s << "look_dir: " << pr.eye_pointing_dir_.x() <<" "<<pr.eye_pointing_dir_.y()<<" "<<pr.eye_pointing_dir_.z()<< '\n';
  s << "eyelid_dt: " << pr.eyelid_dt_ << '\n';
  return s;
}

std::istream&  operator >>(std::istream& s, boxm2_vecf_orbit_params& pr){
  if(!s){
    std::cout << "Bad parameter file stream\n";
    return s;
  }
  std::string st;
  double val;
  s >> st >> pr.eye_radius_;
  s >> st >> pr.iris_radius_;
  s >> st >> pr.medial_socket_radius_coef_;
  s >> st >> pr.lateral_socket_radius_coef_;
  s >> st >> pr.trans_x_;
  s >> st >> pr.trans_y_;
  s >> st >> pr.trans_z_;
  s >> st >> pr.mm_per_pix_;
  s >> st >> pr.image_height_;
  s >> st >> val;
  pr.scale_x_coef_ = val/pr.eye_radius_;
  s >> st >> val;
  pr.scale_y_coef_ = val/pr.eye_radius_;
  s >> st >> val;
  pr.eyelid_crease_scale_y_coef_ = val/pr.eye_radius_;
  s >> st >> val;
  pr.dphi_rad_ = val*3.14159/180.0;
  s >> st >> pr.inferior_lid_radius_offset_;
  s >> st >> val;//do nothing
  s >> st >> pr.mid_inferior_margin_z_;
  s >> st >> pr.mid_superior_margin_z_;
  s >> st >> pr.mid_eyelid_crease_z_;
  s >> st >> pr.inferior_margin_xy_error_;
  s >> st >> pr.inferior_margin_xyz_error_;
  s >> st >> pr.superior_margin_xy_error_;
  s >> st >> pr.superior_margin_xyz_error_;
  s >> st >> pr.superior_crease_xy_error_;
  s >> st >> pr.superior_crease_xyz_error_;
  pr.init_sphere();
  return s;
}
