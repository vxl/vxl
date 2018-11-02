#include <iostream>
#include <iomanip>
#include <bvrml/bvrml_write.h>
#include "boxm2_vecf_orbit_param_stats.h"
#include "boxm2_vecf_plot_orbit.h"
#include <vnl/algo/vnl_svd.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
void boxm2_vecf_orbit_param_stats::average_params(){
  for(auto & pit : param_map_){
    const std::string& pid = pit.first;
    boxm2_vecf_orbit_params avg;
    boxm2_vecf_orbit_params& left_params = (pit.second).first;
    boxm2_vecf_orbit_params& right_params = (pit.second).second;
    avg.eye_radius_= 0.5*(left_params.eye_radius_ + right_params.eye_radius_);
    devs_[pid]["eye_radius"]=std::fabs(left_params.eye_radius_-avg.eye_radius_);
    avg.iris_radius_= 0.5*(left_params.iris_radius_ + right_params.iris_radius_);
    devs_[pid]["iris_radius"]=std::fabs(left_params.iris_radius_-avg.iris_radius_);
    avg.medial_socket_radius_coef_ = 0.5*(left_params.medial_socket_radius_coef_ + right_params.medial_socket_radius_coef_);
    devs_[pid]["medial_socket_radius_coef"]=std::fabs(left_params.medial_socket_radius_coef_-avg.medial_socket_radius_coef_);
    avg.lateral_socket_radius_coef_ = 0.5*(left_params.lateral_socket_radius_coef_ + right_params.lateral_socket_radius_coef_);
    devs_[pid]["lateral_socket_radius_coef"]=std::fabs(left_params.lateral_socket_radius_coef_-avg.lateral_socket_radius_coef_);
    avg.inferior_lid_radius_offset_ = 0.5*(left_params.inferior_lid_radius_offset_ + right_params.inferior_lid_radius_offset_);
    devs_[pid]["inferior_lid_radius_offset"]=std::fabs(left_params.inferior_lid_radius_offset_-avg.inferior_lid_radius_offset_);
    avg.eyelid_crease_scale_y_coef_ = 0.5*(left_params.eyelid_crease_scale_y_coef_ + right_params.eyelid_crease_scale_y_coef_);
    devs_[pid]["eyelid_crease_scale_y_coef"]=std::fabs(left_params.eyelid_crease_scale_y_coef_-avg.eyelid_crease_scale_y_coef_);
    double h_to_w = left_params.height_to_width_ratio();
    h_to_w += right_params.height_to_width_ratio();
    avg.height_to_width_ratio_ = 0.5*h_to_w;
    devs_[pid]["height_to_width_ratio"]=std::fabs(left_params.height_to_width_ratio_-avg.height_to_width_ratio_);
    avg.scale_x_coef_ =  0.5*(left_params.scale_x_coef_ + right_params.scale_x_coef_);
    devs_[pid]["scale_x_coef"]=std::fabs(left_params.scale_x_coef_-avg.scale_x_coef_);
    avg.scale_y_coef_ =  0.5*(left_params.scale_y_coef_ + right_params.scale_y_coef_);
    devs_[pid]["scale_y_coef"]=std::fabs(left_params.scale_y_coef_-avg.scale_y_coef_);
    avg.mid_eyelid_crease_z_ = 0.5*(left_params.mid_eyelid_crease_z_ + right_params.mid_eyelid_crease_z_);
    devs_[pid]["mid_eyelid_crease_z"]=std::fabs(left_params.mid_eyelid_crease_z_-avg.mid_eyelid_crease_z_);
    param_avg_[pit.first] = avg;
  }
}
void boxm2_vecf_orbit_param_stats::generate_stats(){
  //average left and right vals
  this->average_params();
}
void boxm2_vecf_orbit_param_stats::print_stats() {
  std::cout << "patient_id  eye_radius iris_radius lid_radius_off height/width cr_scale_y crease_z_rat\n";
  for(auto & pit : param_avg_){
    const boxm2_vecf_orbit_params& pr = pit.second;
    const std::string& pid = pit.first;
    std::cout << pid << ' ' << pr.eye_radius_ << ' '<< devs_[pid]["eye_radius"]<< ' '
             << pr.iris_radius_ << ' '<< devs_[pid]["iris_radius"] << ' '
             << pr.inferior_lid_radius_offset_ << ' ' << devs_[pid]["inferior_lid_radius_offset"];
    double rat = pr.height_to_width_ratio_;
    std::cout << ' '<< rat << ' '<< devs_[pid]["height_to_width_ratio"]
             << ' '<< pr.eyelid_crease_scale_y() << ' ' << devs_[pid]["eyelid_crease_scale_y_coef"]*pr.eye_radius_
             << ' ' << pr.mid_eyelid_crease_z_/pr.lid_sph_.radius() << ' ' << devs_[pid]["mid_eyelid_crease_z"]/pr.lid_sph_.radius()<< '\n';
  }
}
void boxm2_vecf_orbit_param_stats::print_xy_fitting_error(){
  std::cout << "patient_id  left_inf_xy left_sup_xy left_sup_crease right_inf_xy right_sup_xy right_sup_crease_xy \n";
  for(auto & pit : param_map_){
    const std::string& pid = pit.first;
    boxm2_vecf_orbit_params& lp = (pit.second).first;
    boxm2_vecf_orbit_params& rp = (pit.second).second;

    std::cout << pid << ' ' << lp.inferior_margin_xy_error_ << ' '
             << lp.superior_margin_xy_error_ << ' '
                   << lp.superior_crease_xy_error_ << ' '
             << rp.inferior_margin_xy_error_ << ' '
                   << rp.superior_margin_xy_error_ << ' '
                   << rp.superior_crease_xy_error_ << '\n';
 }
}

void boxm2_vecf_orbit_param_stats::print_xyz_fitting_error(){
  std::cout << "patient_id  left_inf_xyz left_sup_xyz left_sup_crease right_inf_xyz right_sup_xyz right_sup_crease_xyz \n";
  for(auto & pit : param_map_){
    const std::string& pid = pit.first;
    boxm2_vecf_orbit_params& lp = (pit.second).first;
    boxm2_vecf_orbit_params& rp = (pit.second).second;

    std::cout << pid << ' ' << lp.inferior_margin_xyz_error_ << ' '
             << lp.superior_margin_xyz_error_ << ' '
                   << lp.superior_crease_xyz_error_ << ' '
             << rp.inferior_margin_xyz_error_ << ' '
                   << rp.superior_margin_xyz_error_ << ' '
                   << rp.superior_crease_xyz_error_ << '\n';
 }
}
bool boxm2_vecf_orbit_param_stats::merge_margins_and_crease(){
  for(auto & pit : param_map_){
    const std::string& pid = pit.first;
    boxm2_vecf_orbit_params lp = (pit.second).first;
    boxm2_vecf_orbit_params rp = (pit.second).second;

    // plot left eye
    double xm_min_left = lp.x_min()-10.0;
    double xm_max_left = lp.x_max()+10.0;
    std::vector<vgl_point_3d<double> > left_inf_pts, left_sup_pts, left_crease_pts;
    boxm2_vecf_plot_orbit::plot_inferior_margin(lp, false, xm_min_left, xm_max_left, left_inf_pts);
    boxm2_vecf_plot_orbit::plot_superior_margin(lp, false, xm_min_left, xm_max_left, left_sup_pts);
    boxm2_vecf_plot_orbit::plot_crease(lp, false, xm_min_left, xm_max_left, left_crease_pts);
    int left_imin = -1, left_imax = -1;
    bool success = boxm2_vecf_plot_orbit::plot_limits(left_inf_pts, left_sup_pts, left_imin, left_imax);
    if(!success){
      std::cout << "Find left plot limits failed for "<< pid << "\n";
      continue;
    }
    // get left medial canthus (as origin)
    vgl_point_3d<double> plc_inf = left_inf_pts[left_imin];
    vgl_point_3d<double> plc_sup = left_sup_pts[left_imin];
#if 1// medial canthus origin (leads to excessive scatter)
    vgl_vector_3d<double> vlc(0.5*(plc_inf.x() + plc_sup.x()),
                              0.5*(plc_inf.y() + plc_sup.y()),
                              0.5*(plc_inf.z() + plc_sup.z()));
#endif
    // use sclera-derived z origin and palpebral x-y origin instead
    //vgl_vector_3d<double> vlc(0.0, 0.0, lp.eyelid_radius());
    // plot right eye
    double xm_min_right = rp.x_min()-10.0;
    double xm_max_right = rp.x_max()+10.0;
    std::vector<vgl_point_3d<double> > right_inf_pts, right_sup_pts, right_crease_pts;
    boxm2_vecf_plot_orbit::plot_inferior_margin(rp, false, xm_min_right, xm_max_right, right_inf_pts);
    boxm2_vecf_plot_orbit::plot_superior_margin(rp, false, xm_min_right, xm_max_right, right_sup_pts);
    boxm2_vecf_plot_orbit::plot_crease(rp, false, xm_min_right, xm_max_right, right_crease_pts);
    int right_imin = -1, right_imax = -1;
    success = boxm2_vecf_plot_orbit::plot_limits(right_inf_pts, right_sup_pts, right_imin, right_imax);
    if(!success){
      std::cout << "Find right plot limits failed\n";
      return false;
    }
    // get right medial canthus (as origin)
    vgl_point_3d<double> prc_inf = right_inf_pts[right_imin];
    vgl_point_3d<double> prc_sup = right_sup_pts[right_imin];
#if 1// medial canthus origin (leads to excessive scatter)
    vgl_vector_3d<double> vrc(0.5*(prc_inf.x() + prc_sup.x()),
                              0.5*(prc_inf.y() + prc_sup.y()),
                              0.5*(prc_inf.z() + prc_sup.z()));
#endif
    // use sclera-derived z origin and palpebral x-y origin instead
    //vgl_vector_3d<double> vrc(0.0, 0.0, rp.eyelid_radius());
    // merge margins and crease
    std::vector<vgl_point_3d<double> > inf_pts, sup_pts, crease_pts;
    for(int i = left_imin; i<=left_imax; ++i){
      inf_pts.push_back(left_inf_pts[i]-vlc);
      sup_pts.push_back(left_sup_pts[i]-vlc);
      crease_pts.push_back(left_crease_pts[i]-vlc);
    }
    for(int i = right_imin; i<=right_imax; ++i){
      inf_pts.push_back(right_inf_pts[i]-vrc);
      sup_pts.push_back(right_sup_pts[i]-vrc);
      crease_pts.push_back(right_crease_pts[i]-vrc);
    }
    merged_inf_margin_[pid]=inf_pts;
    merged_sup_margin_[pid]=sup_pts;
    merged_crease_[pid]=crease_pts;

  }
  return true;
}
bool boxm2_vecf_orbit_param_stats::plot_merged_margins(std::ofstream& os, unsigned sample_skip){
  if(!os){
    std::cout << "bad ostream in boxm2_vecf_orbit_param_stats\n";
    return false;
  }
  bvrml_write::write_vrml_header(os);
  float r = 0.5f; // error range 1.0 mm
  // write inferior margins
  for(auto & pit : merged_inf_margin_){
     std::vector<vgl_point_3d<double> > inf_pts = pit.second;
     auto n = static_cast<unsigned>(inf_pts.size());
     for(unsigned i = 0; i<n; i+=sample_skip){
       vgl_point_3d<double> pd = inf_pts[i];
       vgl_point_3d<float> pf(static_cast<float>(pd.x()),
                              static_cast<float>(pd.y()),
                              static_cast<float>(pd.z()));
       vgl_sphere_3d<float> sp(pf, r);
       bvrml_write::write_vrml_sphere(os, sp, 1.0f, 1.0f, 0.0f);
     }
  }
  // write superior margins
  for(auto & pit : merged_sup_margin_){
    std::vector<vgl_point_3d<double> > sup_pts = pit.second;
    auto n = static_cast<unsigned>(sup_pts.size());
    for(unsigned i = 0; i<n; i+=sample_skip){
      vgl_point_3d<double> pd = sup_pts[i];
      vgl_point_3d<float> pf(static_cast<float>(pd.x()),
                             static_cast<float>(pd.y()),
                             static_cast<float>(pd.z()));
      vgl_sphere_3d<float> sp(pf, r);
      bvrml_write::write_vrml_sphere(os, sp, 1.0f, 0.0f, 0.0f);
    }
  }
  // write creases
  for(auto & pit : merged_crease_){
    std::vector<vgl_point_3d<double> > crease_pts = pit.second;
    auto n = static_cast<unsigned>(crease_pts.size());
    for(unsigned i = 0; i<n; i+=sample_skip){
      vgl_point_3d<double> pd = crease_pts[i];
      vgl_point_3d<float> pf(static_cast<float>(pd.x()),
                             static_cast<float>(pd.y()),
                             static_cast<float>(pd.z()));
      vgl_sphere_3d<float> sp(pf, r);
      bvrml_write::write_vrml_sphere(os, sp, 0.0f, 1.0f, 1.0f);
    }
  }
  os.close();
  return true;
}
void boxm2_vecf_orbit_param_stats::compute_feature_vectors(){
  unsigned dim = 12;
  for(auto & mit : merged_inf_margin_){
    std::string pid = mit.first;
    std::vector<vgl_point_3d<double> > inf_pts = mit.second;
    std::vector<vgl_point_3d<double> > sup_pts = merged_sup_margin_[pid];
    std::vector<vgl_point_3d<double> > crease_pts = merged_crease_[pid];
    vgl_point_3d<double> pl = inf_pts[inf_pts.size()-1];
    vgl_point_3d<double> pmini, pmaxs, pmaxc;
    unsigned n = 0;
#if 1
    n = static_cast<unsigned>(inf_pts.size());
    double yi = 1000.0;
    for(unsigned i = 0; i<n; ++i){
      vgl_point_3d<double> pi = inf_pts[i];
      if(pi.y()<yi){
        yi = pi.y();
        pmini = pi;
      }
    }
#endif
    n = static_cast<unsigned>(sup_pts.size());
    double ys = -1000.0;
    for(unsigned i = 0; i<n; ++i){
      vgl_point_3d<double> ps = sup_pts[i];
      if(ps.y()>ys){
        ys = ps.y();
        pmaxs = ps;
      }
    }
    n = static_cast<unsigned>(crease_pts.size());
    double yc = -1000.0;
    for(unsigned i = 0; i<n; ++i){
      vgl_point_3d<double> pc = crease_pts[i];
      if(pc.y()>yc){
        yc = pc.y();
        pmaxc = pc;
      }
    }
    vnl_matrix<double> fv(dim,1,0.0);
#if 0
    double alpha = (ys-yi)/pl.x();
    double zeta = 0.5*(pmaxc.z()-pl.z())/8.7;
    double kappa = (yc-ys)/ys;
    double eta = (pmaxs.z()-pmaxc.z())/pmaxs.z();
    fv[0][0]= alpha;     fv[1][0]= zeta;
    fv[2][0]= kappa;     fv[3][0]= eta;
#endif
#if 1
    fv[0][0]=pl.x(); fv[1][0]=pl.y(); fv[2][0]= pl.z();
    fv[3][0]=pmaxs.x(); fv[4][0]=pmaxs.y(); fv[5][0]= pmaxs.z();
    fv[6][0]=pmaxc.x(); fv[7][0]=pmaxc.y(); fv[8][0]= pmaxc.z();
    fv[9][0]=pmini.x(); fv[10][0]=pmini.y(); fv[11][0]= pmini.z();
#endif
    feature_vectors_[pid]=fv;
  }
}

void boxm2_vecf_orbit_param_stats::compute_covariance_matrix(){
  unsigned dim = 12; //dimension of the feature vector
  // compute mean vector
  mean_ = vnl_matrix<double>(dim, 1, 0.0);
  double nv = 0.0;
  for(auto fit = feature_vectors_.begin();
      fit != feature_vectors_.end(); ++fit, nv+=1.0)
    mean_ += fit->second;
  mean_/=nv;
  std::cout << "mean feature vector\n";
  for(unsigned i = 0; i<dim; ++i)
    std::cout << std::setprecision(2) << mean_[i][0] << ' ';
  std::cout << '\n';
  cov_ = vnl_matrix<double>(dim, dim, 0.0);
  auto m = static_cast<unsigned>(feature_vectors_.size());
  // the data matrix - rows denote the elements of the feature vector, columns each sample
  vnl_matrix<double> X(dim, m);
  unsigned k = 0;
  for(auto fit = feature_vectors_.begin();
      fit != feature_vectors_.end(); ++fit, ++k){
    vnl_matrix<double> x = (fit->second - mean_);
    for(unsigned r = 0; r<static_cast<unsigned>(x.rows()); ++r)
      X[r][k]=x[r][0];
  }
  vnl_svd<double> svd(X);
  vnl_matrix<double> W = svd.W();
  vnl_matrix<double> Wsq = W*W/m;
  std::cout << Wsq << '\n';
  vnl_matrix<double> U = svd.U();
  vnl_matrix<double> Ut = U.transpose();
  // the primary eigenvector
  std::cout << "primary eigenvector\n";
  for(unsigned i = 0; i<dim; ++i)
    std::cout << std::setprecision(2) << Ut[0][i] << ' ';
  std::cout << '\n';
  std::cout << "secondary eigenvector\n";
  for(unsigned i = 0; i<dim; ++i)
    std::cout << std::setprecision(2) << Ut[1][i] << ' ';
  std::cout << '\n';
  cov_ = U*Wsq*Ut;
  // a m x m matrix only the first dim rows are meaningful.
  // the columns are the m data samples transformed so as
  // to produce a diagonal covariance matrix
  vnl_matrix<double> Xw = Ut*X;
  k = 0;
  for(auto fit = feature_vectors_.begin();
      fit != feature_vectors_.end(); ++fit, ++k){
    std::string pid = fit->first;
    std::cout << pid << ' ' << Xw[0][k] << ' ' << Xw[1][k] << '\n';
  }
}
void boxm2_vecf_orbit_param_stats::separation_stats(){
  double avg_ratio = 0.0;
  double ns = 0.0;
  for(auto pit =  param_map_.begin();
      pit!=param_map_.end(); ++pit, ns += 1.0){
    const std::string& pid = pit->first;
    boxm2_vecf_orbit_params& left_params = (pit->second).first;
    boxm2_vecf_orbit_params& right_params = (pit->second).second;
    double avg_eye_radius= 0.5*(left_params.eye_radius_ + right_params.eye_radius_);
    vgl_vector_3d<double> org_l(left_params.x_trans(), left_params.y_trans(), left_params.z_trans());
    vgl_vector_3d<double> org_r(right_params.x_trans(), right_params.y_trans(), right_params.z_trans());
    vgl_vector_3d<double> dif = org_l - org_r;
    double sep = dif.length();
    double ratio = sep/avg_eye_radius;
    avg_ratio += ratio;
    std::cout << pid << ' ' << avg_eye_radius << ' ' << sep << ' ' << ratio << '\n';
  }
  std::cout << "Average separation ratio " << avg_ratio/ns << '\n';
}
