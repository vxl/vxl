#include "boxm2_vecf_orbit_param_stats.h"
void boxm2_vecf_orbit_param_stats::average_params(){
  for(vcl_map<vcl_string, vcl_pair<boxm2_vecf_orbit_params, boxm2_vecf_orbit_params > >::iterator pit =  param_map_.begin();
      pit!=param_map_.end(); ++pit){
    const vcl_string& pid = pit->first;
    boxm2_vecf_orbit_params avg;
    boxm2_vecf_orbit_params& left_params = (pit->second).first;
    boxm2_vecf_orbit_params& right_params = (pit->second).second;
    avg.eye_radius_= 0.5*(left_params.eye_radius_ + right_params.eye_radius_);
    devs_[pid]["eye_radius"]=vcl_fabs(left_params.eye_radius_-avg.eye_radius_);
    avg.iris_radius_= 0.5*(left_params.iris_radius_ + right_params.iris_radius_); 
    devs_[pid]["iris_radius"]=vcl_fabs(left_params.iris_radius_-avg.iris_radius_);
    avg.medial_socket_radius_coef_ = 0.5*(left_params.medial_socket_radius_coef_ + right_params.medial_socket_radius_coef_);
    devs_[pid]["medial_socket_radius_coef"]=vcl_fabs(left_params.medial_socket_radius_coef_-avg.medial_socket_radius_coef_);
    avg.lateral_socket_radius_coef_ = 0.5*(left_params.lateral_socket_radius_coef_ + right_params.lateral_socket_radius_coef_); 
    devs_[pid]["lateral_socket_radius_coef"]=vcl_fabs(left_params.lateral_socket_radius_coef_-avg.lateral_socket_radius_coef_);
    avg.inferior_lid_radius_offset_ = 0.5*(left_params.inferior_lid_radius_offset_ + right_params.inferior_lid_radius_offset_);
    devs_[pid]["inferior_lid_radius_offset"]=vcl_fabs(left_params.inferior_lid_radius_offset_-avg.inferior_lid_radius_offset_);
    avg.eyelid_crease_scale_y_coef_ = 0.5*(left_params.eyelid_crease_scale_y_coef_ + right_params.eyelid_crease_scale_y_coef_);
    devs_[pid]["eyelid_crease_scale_y_coef"]=vcl_fabs(left_params.eyelid_crease_scale_y_coef_-avg.eyelid_crease_scale_y_coef_);
    double h_to_w = left_params.height_to_width_ratio();
    h_to_w += right_params.height_to_width_ratio();
    avg.height_to_width_ratio_ = 0.5*h_to_w;
    devs_[pid]["height_to_width_ratio"]=vcl_fabs(left_params.height_to_width_ratio_-avg.height_to_width_ratio_);
    avg.scale_x_coef_ =  0.5*(left_params.scale_x_coef_ + right_params.scale_x_coef_);
    devs_[pid]["scale_x_coef"]=vcl_fabs(left_params.scale_x_coef_-avg.scale_x_coef_);
    avg.scale_y_coef_ =  0.5*(left_params.scale_y_coef_ + right_params.scale_y_coef_);
    devs_[pid]["scale_y_coef"]=vcl_fabs(left_params.scale_y_coef_-avg.scale_y_coef_);
    avg.mid_eyelid_crease_z_ = 0.5*(left_params.mid_eyelid_crease_z_ + right_params.mid_eyelid_crease_z_);
    devs_[pid]["mid_eyelid_crease_z"]=vcl_fabs(left_params.mid_eyelid_crease_z_-avg.mid_eyelid_crease_z_);
    param_avg_[pit->first] = avg;
  }
}
void boxm2_vecf_orbit_param_stats::generate_stats(){
  //average left and right vals
  this->average_params();
}
void boxm2_vecf_orbit_param_stats::print_stats() {
  vcl_cout << "patient_id  eye_radius iris_radius lid_radius_off height/width cr_scale_y crease_z_rat\n";
  for(vcl_map<vcl_string, boxm2_vecf_orbit_params>::iterator pit = param_avg_.begin();
      pit != param_avg_.end(); ++pit){
    const boxm2_vecf_orbit_params& pr = pit->second;
    const vcl_string& pid = pit->first;
    vcl_cout << pid << ' ' << pr.eye_radius_ << ' '<< devs_[pid]["eye_radius"]<< ' '
             << pr.iris_radius_ << ' '<< devs_[pid]["iris_radius"] << ' '
             << pr.inferior_lid_radius_offset_ << ' ' << devs_[pid]["inferior_lid_radius_offset"];
    double rat = pr.height_to_width_ratio_;
    vcl_cout << ' '<< rat << ' '<< devs_[pid]["height_to_width_ratio"]
             << ' '<< pr.eyelid_crease_scale_y() << ' ' << devs_[pid]["eyelid_crease_scale_y_coef"]*pr.eye_radius_
             << ' ' << pr.mid_eyelid_crease_z_/pr.lid_sph_.radius() << ' ' << devs_[pid]["mid_eyelid_crease_z"]/pr.lid_sph_.radius()<< '\n';
  }
}
void boxm2_vecf_orbit_param_stats::print_xy_fitting_error(){
  vcl_cout << "patient_id  left_inf_xy left_sup_xy left_sup_crease right_inf_xy right_sup_xy right_sup_crease_xy \n";
  for(vcl_map<vcl_string, vcl_pair<boxm2_vecf_orbit_params, boxm2_vecf_orbit_params > >::iterator pit =  param_map_.begin();
      pit!=param_map_.end(); ++pit){
    const vcl_string& pid = pit->first;
    boxm2_vecf_orbit_params& lp = (pit->second).first;
    boxm2_vecf_orbit_params& rp = (pit->second).second;

    vcl_cout << pid << ' ' << lp.inferior_margin_xy_error_ << ' '
             << lp.superior_margin_xy_error_ << ' '
                   << lp.superior_crease_xy_error_ << ' '
             << rp.inferior_margin_xy_error_ << ' '
                   << rp.superior_margin_xy_error_ << ' ' 
                   << rp.superior_crease_xy_error_ << '\n';
 }
}

void boxm2_vecf_orbit_param_stats::print_xyz_fitting_error(){
  vcl_cout << "patient_id  left_inf_xyz left_sup_xyz left_sup_crease right_inf_xyz right_sup_xyz right_sup_crease_xyz \n";
  for(vcl_map<vcl_string, vcl_pair<boxm2_vecf_orbit_params, boxm2_vecf_orbit_params > >::iterator pit =  param_map_.begin();
      pit!=param_map_.end(); ++pit){
    const vcl_string& pid = pit->first;
    boxm2_vecf_orbit_params& lp = (pit->second).first;
    boxm2_vecf_orbit_params& rp = (pit->second).second;

    vcl_cout << pid << ' ' << lp.inferior_margin_xyz_error_ << ' '
             << lp.superior_margin_xyz_error_ << ' '
                   << lp.superior_crease_xyz_error_ << ' '
             << rp.inferior_margin_xyz_error_ << ' '
                   << rp.superior_margin_xyz_error_ << ' ' 
                   << rp.superior_crease_xyz_error_ << '\n';
 }
}
