#include <vcl_fstream.h>
#include "boxm2_vecf_fit_orbit.h"
#include "boxm2_vecf_eyelid.h"
#include "boxm2_vecf_eyelid_crease.h"
#include <vgl/vgl_vector_3d.h>
#include <vgl/algo/vgl_fit_sphere_3d.h>
#include <bvrml/bvrml_write.h>
#include <vcl_limits.h>
# define USE_SPHERE 0
# define SET_CREASE_T 0
void boxm2_vecf_fit_orbit::fill_smid_map(){
  smid_map_["left_eye_medial_canthus"]=LEFT_EYE_MEDIAL_CANTHUS;
  smid_map_["left_eye_lateral_canthus"]=LEFT_EYE_LATERAL_CANTHUS;
  smid_map_["left_eye_inferior_margin"]=LEFT_EYE_INFERIOR_MARGIN;
  smid_map_["left_eye_superior_margin"]=LEFT_EYE_SUPERIOR_MARGIN;
  smid_map_["left_eye_superior_crease"]=LEFT_EYE_SUPERIOR_CREASE;
  smid_map_["left_eye_sclera"]=LEFT_SCLERA;
  smid_map_["left_Nz"]=LEFT_Nz;
  smid_map_["left_iris_radius"]=LEFT_IRIS_RADIUS;

  smid_map_["right_eye_medial_canthus"]=RIGHT_EYE_MEDIAL_CANTHUS;
  smid_map_["right_eye_lateral_canthus"]=RIGHT_EYE_LATERAL_CANTHUS;
  smid_map_["right_eye_inferior_margin"]=RIGHT_EYE_INFERIOR_MARGIN;
  smid_map_["right_eye_superior_margin"]=RIGHT_EYE_SUPERIOR_MARGIN;
  smid_map_["right_eye_superior_crease"]=RIGHT_EYE_SUPERIOR_CREASE;
  smid_map_["right_eye_sclera"]=RIGHT_SCLERA;
  smid_map_["right_Nz"]=RIGHT_Nz;
  smid_map_["right_iris_radius"]=RIGHT_IRIS_RADIUS;
}
bool boxm2_vecf_fit_orbit::add_labeled_point(labeled_point lp){ 
  vcl_map<vcl_string, mids>::iterator iit = smid_map_.find(lp.label_);
  if(iit == smid_map_.end() ){
    vcl_cout << "Measurement label " << lp.label_ << " doesn't exist\n";
    return false;
  }
  lpts_[iit->second] = lp;
  return true;  
}
bool boxm2_vecf_fit_orbit::read_anchor_file(vcl_string const& path){
  vcl_ifstream istr(path.c_str());
  if(!istr.is_open()){
    vcl_cout << "Anchor file " << path << " not found\n";
    return false;
  }
  vcl_map<vcl_string, vcl_vector<vgl_point_3d<double> > > anchors;
  while(true){
    if(istr.eof())
      break;
    double x, y, z;
    unsigned char c;
    istr >> x >> c;
    if(c!=',')
      return false;
    istr >> y >> c;
    if(c!=',')
      return false;
    istr >> z >> c;
    if(c!=',')
      return false;
    vcl_string lab;
    istr >> lab;
        if(lab=="") continue;
    vgl_point_3d<double> p(x,y,z);
    anchors[lab].push_back(p);
  }
  for(vcl_map<vcl_string, vcl_vector<vgl_point_3d<double> > >::iterator ait = anchors.begin();
      ait != anchors.end(); ++ait){
    const vcl_string& lab = ait->first;
    vcl_vector<vgl_point_3d<double> >& pts = ait->second;
    double x = 0.0, y= 0.0, z = 0.0;
    double np = 0.0;
    for(vcl_vector<vgl_point_3d<double> >::iterator pit = pts.begin();
        pit != pts.end(); ++pit, np+=1.0){
      x += pit->x(); y += pit->y(); z += pit->z();
    }
    if(np == 0.0){
    vcl_cout << "No points for label  " << lab << "\n";
    return false;
    }
    x /= np;      y /= np;  z /= np;
    labeled_point lp(x, y, z, lab);
    if(!this->add_labeled_point(lp))
      return false;
  }
  return true;
}
bool boxm2_vecf_fit_orbit::left_eye_radius( double& rad){
  // find left eye inner cusp - corresponds to xmin
  vcl_map<mids, labeled_point>::iterator lit;
  lit = lpts_.find(LEFT_EYE_MEDIAL_CANTHUS);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& le_mc  = lit->second.p3d_;
  lit = lpts_.find(LEFT_EYE_LATERAL_CANTHUS);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& le_lc  = lit->second.p3d_;
  double dx = le_lc.x()-le_mc.x();
  rad = vcl_fabs(dx/(left_params_.x_max_-left_params_.x_min_));
  return true;
}
bool boxm2_vecf_fit_orbit::set_left_iris_radius(){
  vcl_map<mids, labeled_point>::iterator lit;
  lit = lpts_.find(LEFT_IRIS_RADIUS);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& l_iris_r  = lit->second.p3d_;
  left_params_.iris_radius_ = l_iris_r.x();
  return true;
}
bool boxm2_vecf_fit_orbit::left_trans_x_from_lateral_canthus(double& trx){
        vcl_map<mids, labeled_point>::iterator lit;
  lit = lpts_.find(LEFT_EYE_LATERAL_CANTHUS);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& le_oc  = lit->second.p3d_;
  double xm = left_params_.eye_radius_*left_params_.x_max_;
  trx = (le_oc.x()-xm);
  return true;
}

bool boxm2_vecf_fit_orbit::left_trans_y_from_lateral_canthus( double& tr_y){
  vcl_map<mids, labeled_point>::iterator lit;
  lit = lpts_.find(LEFT_EYE_LATERAL_CANTHUS);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& le_oc  = lit->second.p3d_;
  //tr_y = le_oc.y()-left_params_.y_off_;
  tr_y = le_oc.y();
  return true;
}

bool boxm2_vecf_fit_orbit::left_eye_x_scale(double& left_x_scale){
  vcl_map<mids, labeled_point>::iterator lit;
  lit = lpts_.find(LEFT_EYE_LATERAL_CANTHUS);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& le_oc  = lit->second.p3d_;
  double x = le_oc.x()-left_params_.trans_x_;
  double xm = left_params_.eye_radius_*left_params_.x_max_;
  double temp  = x/xm;
  
  lit = lpts_.find(LEFT_EYE_MEDIAL_CANTHUS);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& le_ic  = lit->second.p3d_;
  x = le_ic.x()-left_params_.trans_x_;
  xm = left_params_.eye_radius_*left_params_.x_min_;
  double temp1 = x/xm;
  //geometric mean of scales
  left_x_scale = vcl_sqrt(temp*temp1);
  return true;
}

bool boxm2_vecf_fit_orbit::left_eye_y_scale(double& left_y_scale){
  vcl_map<mids, labeled_point>::iterator lit;
  lit = lpts_.find(LEFT_EYE_INFERIOR_MARGIN);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& mid_lower  = lit->second.p3d_;
  double temp = left_params_.trans_y_ ;
  double y = mid_lower.y()-temp;
  double xm = mid_lower.x()-left_params_.x_trans();
  boxm2_vecf_eyelid elid(left_params_);
  double yy = elid.Y(xm, 1.05);
  left_y_scale = y/yy;
  return true;
}
bool boxm2_vecf_fit_orbit::left_eye_inferior_margin_t(double& left_inf_t){
  vcl_map<mids, labeled_point>::iterator lit;
  lit = lpts_.find(LEFT_EYE_INFERIOR_MARGIN);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& mid_lower  = lit->second.p3d_;
  double temp = left_params_.trans_y_ ;
  double y = mid_lower.y()-temp;
  double xm = mid_lower.x()-left_params_.x_trans();
  boxm2_vecf_eyelid elid(left_params_);
  left_inf_t = elid.t(xm, y);
  return true;
}
bool boxm2_vecf_fit_orbit::left_eye_superior_margin_t(double& superior_margin_t){
  vcl_map<mids, labeled_point>::iterator lit;
  lit = lpts_.find(LEFT_EYE_SUPERIOR_MARGIN);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& mid_upper  = lit->second.p3d_;
   double temp = left_params_.trans_y_;
  double y = mid_upper.y()-temp;
  double xm = mid_upper.x()-left_params_.x_trans();
  boxm2_vecf_eyelid elid(left_params_);
  superior_margin_t = elid.t(xm, y);
  return true;
}

bool boxm2_vecf_fit_orbit::left_eyelid_crease_scale_y(double& crease_scale_y){
  vcl_map<mids, labeled_point>::iterator lit;
  lit = lpts_.find(LEFT_EYE_SUPERIOR_CREASE);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& mid_superior_crease  = lit->second.p3d_;
  double temp = left_params_.trans_y_;
  double y = mid_superior_crease.y()-temp;
  double xm = mid_superior_crease.x() - left_params_.x_trans();
  boxm2_vecf_eyelid_crease crease(left_params_);
  double yy = crease.Y(xm, left_params_.eyelid_crease_ct_);
  crease_scale_y = y/yy;
  return true;
}
bool boxm2_vecf_fit_orbit::left_mid_eyelid_crease_z(double& crease_z){
  vcl_map<mids, labeled_point>::iterator lit;
  lit = lpts_.find(LEFT_EYE_SUPERIOR_CREASE);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& mid_superior_crease  = lit->second.p3d_;
  crease_z = mid_superior_crease.z()-left_params_.z_trans();
  return true;
}

bool boxm2_vecf_fit_orbit::left_eye_superior_margin_crease_t(double& left_sup_crease_t){
  vcl_map<mids, labeled_point>::iterator lit;
  lit = lpts_.find(LEFT_EYE_SUPERIOR_CREASE);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& mid_superior_crease  = lit->second.p3d_;
  double temp = left_params_.trans_y_;
  double y = mid_superior_crease.y()-temp;
  double xm = mid_superior_crease.x() - left_params_.x_trans();
  boxm2_vecf_eyelid_crease crease(left_params_);
  left_sup_crease_t = crease.t(xm, y);
  return true;
}

bool boxm2_vecf_fit_orbit::left_medial_lateral_angle(double& ang_rad){
// find left eye inner cusp - corresponds to xmin
  vcl_map<mids, labeled_point>::iterator lit;
  lit = lpts_.find(LEFT_EYE_MEDIAL_CANTHUS);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& le_mc  = lit->second.p3d_;
  lit = lpts_.find(LEFT_EYE_LATERAL_CANTHUS);
  if(lit == lpts_.end()) 
    return false;
  const vgl_point_3d<double>& le_lc  = lit->second.p3d_;
  double dy = le_mc.y()-le_lc.y();
  double dx = le_mc.x()-le_lc.x();
  ang_rad = vcl_atan(dy/dx);
  return true;
}
void  boxm2_vecf_fit_orbit::set_left_medial_lateral_angle(double& ang_rad){
  left_params_.dphi_rad_=   ang_rad - nominal_medial_lateral_angle_rad_;
} 

bool boxm2_vecf_fit_orbit::max_sclera_z(vcl_string const& data_desc, double& max_z){
  vcl_map<vcl_string, mids>::iterator iit = smid_map_.find(data_desc);
  if(iit == smid_map_.end() ){
    vcl_cout << "data label " << data_desc << " doesn't exist\n";
    return false;
  } 
  vcl_vector<vgl_point_3d<double> >& pts = orbit_data_[iit->second];
  if(!pts.size()){
    vcl_cout << "No data of type " << data_desc << '\n';
    return false;
  }
  max_z = -vcl_numeric_limits<double>::max();
  for(vcl_vector<vgl_point_3d<double> >::iterator pit = pts.begin();
      pit != pts.end(); ++pit)
    if(pit->z()>max_z)
      max_z = pit->z();
  return true;
}
bool boxm2_vecf_fit_orbit::fit_sclera(vcl_string const& data_desc){
  vcl_map<vcl_string, mids>::iterator iit = smid_map_.find(data_desc);
  if(iit == smid_map_.end() ){
    vcl_cout << "data label " << data_desc << " doesn't exist\n";
    return false;
  } 
  vcl_vector<vgl_point_3d<double> >& pts = orbit_data_[iit->second];
  if(!pts.size()){
    vcl_cout << "No data of type " << data_desc << '\n';
    return false;
  }
  vgl_fit_sphere_3d<double> fit_sph(pts);
  double err = fit_sph.fit(&vcl_cout, true);
  vgl_sphere_3d<double> sph = fit_sph.get_sphere_nonlinear_fit();
  double tol = 0.1*sph.radius();
  if(err>=tol)
    return false;
  bool is_right = iit->second > LEFT_IRIS_RADIUS;
  if(is_right)
    right_params_.sph_ = sph;
  else
    left_params_.sph_ = sph;
  return true;
}

bool boxm2_vecf_fit_orbit::right_eye_radius( double& rad){
  // find right eye inner cusp - corresponds to xmin
  vcl_map<mids, labeled_point>::iterator lit;
  lit = lpts_.find(RIGHT_EYE_MEDIAL_CANTHUS);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& re_ic  = lit->second.p3d_;
  lit = lpts_.find(RIGHT_EYE_LATERAL_CANTHUS);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& re_oc  = lit->second.p3d_;
  double dx =(re_ic.x()-re_oc.x());
 rad = vcl_fabs(dx/(right_params_.x_max_-right_params_.x_min_));

  return true;
}

bool boxm2_vecf_fit_orbit::set_right_iris_radius(){
  vcl_map<mids, labeled_point>::iterator lit;
  lit = lpts_.find(RIGHT_IRIS_RADIUS);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& l_iris_r  = lit->second.p3d_;
  right_params_.iris_radius_ = l_iris_r.x();
  return true;
}

bool boxm2_vecf_fit_orbit::right_trans_y_from_lateral_canthus( double& tr_y){
  vcl_map<mids, labeled_point>::iterator lit;
  lit = lpts_.find(RIGHT_EYE_LATERAL_CANTHUS);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& re_oc  = lit->second.p3d_;
  //tr_y = re_oc.y()-right_params_.y_off_;
  tr_y = re_oc.y();
  return true;
}

bool boxm2_vecf_fit_orbit::right_trans_x_from_lateral_canthus(double& trx){
        vcl_map<mids, labeled_point>::iterator lit;
  lit = lpts_.find(RIGHT_EYE_LATERAL_CANTHUS);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& re_oc  = lit->second.p3d_;
  double xm = -right_params_.eye_radius_*right_params_.x_max_;
  trx =(re_oc.x()-xm);
  return true;
}

bool boxm2_vecf_fit_orbit::right_eye_x_scale(double& right_x_scale){
  vcl_map<mids, labeled_point>::iterator lit;
  lit = lpts_.find(RIGHT_EYE_LATERAL_CANTHUS);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& re_oc  = lit->second.p3d_;
  double xoc = re_oc.x()-right_params_.trans_x_;
  double xmx = -(right_params_.eye_radius_*right_params_.x_max_);

  lit = lpts_.find(RIGHT_EYE_MEDIAL_CANTHUS);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& re_ic  = lit->second.p3d_;
  double xic = re_ic.x()-right_params_.trans_x_;
  double xmi = -right_params_.eye_radius_*right_params_.x_min_;
  right_x_scale = vcl_fabs((xmx-xmi)/(xoc-xic));
  return true;
}


bool boxm2_vecf_fit_orbit::right_eye_y_scale(double& right_y_scale){
  vcl_map<mids, labeled_point>::iterator lit;
  lit = lpts_.find(RIGHT_EYE_INFERIOR_MARGIN);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& mid_lower  = lit->second.p3d_;
  double temp = right_params_.trans_y_;
  double y = mid_lower.y()-temp;
  double xm = mid_lower.x() - right_params_.x_trans();
  boxm2_vecf_eyelid elid(right_params_);
  double yy = elid.Y(-xm, 1.05);
  right_y_scale = y/yy;
  return true;
}
bool boxm2_vecf_fit_orbit::right_eye_inferior_margin_t(double& right_inf_t){
  vcl_map<mids, labeled_point>::iterator lit;
  lit = lpts_.find(RIGHT_EYE_INFERIOR_MARGIN);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& mid_lower  = lit->second.p3d_;
  double temp = right_params_.trans_y_ ;
  double y = mid_lower.y()-temp;
  double xm = mid_lower.x()-right_params_.x_trans();
  boxm2_vecf_eyelid elid(right_params_);
  right_inf_t = elid.t(xm, y);
  return true;
}
bool boxm2_vecf_fit_orbit::right_eye_superior_margin_t(double& superior_margin_t){
  vcl_map<mids, labeled_point>::iterator lit;
  lit = lpts_.find(RIGHT_EYE_SUPERIOR_MARGIN);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& mid_upper  = lit->second.p3d_;
  double temp = right_params_.trans_y_;
  double y = mid_upper.y()-temp;
  double xm = mid_upper.x()- right_params_.x_trans();
  boxm2_vecf_eyelid elid(right_params_);
  superior_margin_t = elid.t(-xm, y);
  return true;
}

bool boxm2_vecf_fit_orbit::right_eyelid_crease_scale_y(double& crease_scale_y){
  vcl_map<mids, labeled_point>::iterator lit;
  lit = lpts_.find(RIGHT_EYE_SUPERIOR_CREASE);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& mid_superior_crease  = lit->second.p3d_;
  double temp = right_params_.trans_y_;
  double y = mid_superior_crease.y()-temp;
  double xm = mid_superior_crease.x() - right_params_.x_trans();
  boxm2_vecf_eyelid_crease crease(right_params_);
  double yy = crease.Y(xm, right_params_.eyelid_crease_ct_);
  crease_scale_y = y/yy;
  return true;
}

bool boxm2_vecf_fit_orbit::right_mid_eyelid_crease_z(double& crease_z){
  vcl_map<mids, labeled_point>::iterator lit;
  lit = lpts_.find(RIGHT_EYE_SUPERIOR_CREASE);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& mid_superior_crease  = lit->second.p3d_;
  crease_z = mid_superior_crease.z()-right_params_.z_trans();
  return true;
}

bool boxm2_vecf_fit_orbit::right_eye_superior_margin_crease_t(double& right_sup_crease_t){
  vcl_map<mids, labeled_point>::iterator lit;
  lit = lpts_.find(RIGHT_EYE_SUPERIOR_CREASE);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& mid_superior_crease  = lit->second.p3d_;
  double temp = right_params_.trans_y_;
  double y = mid_superior_crease.y()-temp;
  double xm = mid_superior_crease.x() - right_params_.x_trans();
  boxm2_vecf_eyelid_crease crease(right_params_);
  right_sup_crease_t = crease.t(xm, y);
  return true;
}

bool boxm2_vecf_fit_orbit::right_medial_lateral_angle(double& ang_rad){
// find right eye inner cusp - corresponds to xmin
  vcl_map<mids, labeled_point>::iterator lit;
  lit = lpts_.find(RIGHT_EYE_MEDIAL_CANTHUS);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& re_mc  = lit->second.p3d_;
  lit = lpts_.find(RIGHT_EYE_LATERAL_CANTHUS);
  if(lit == lpts_.end()) 
    return false;
  const vgl_point_3d<double>& re_lc  = lit->second.p3d_;
  double dy = re_mc.y()-re_lc.y();
  double dx = re_mc.x()-re_lc.x();
  ang_rad = vcl_atan(dy/dx);
  return true;
}
void  boxm2_vecf_fit_orbit::set_right_medial_lateral_angle(double& ang_rad){
  
  right_params_.dphi_rad_= -(nominal_medial_lateral_angle_rad_+ang_rad);
} 
bool boxm2_vecf_fit_orbit::fit_left(){
  bool good = false;
  double ang_rad = 0.0, d_trx = 0.0, trx = 0.0, d_try = 0.0, tr_y = 0.0, trz = 0.0, eye_rad = 1.0;

  good =  left_medial_lateral_angle(ang_rad);
  if(!good)
    return false;
  this->set_left_medial_lateral_angle(ang_rad);

  good= this->set_left_iris_radius();
  if(!good)
    return false;
// the fitted sphere radius appears to be more accurate than from the cusp distances
// also the x origin seems more accurate when estimated from the sphere center than cusps
#if !USE_SPHERE 
  double max_left_z = 0.0;
  good = this->max_sclera_z("left_eye_sclera", max_left_z);
   good = this->left_eye_radius(eye_rad);
  if(!good)
    return false;
  this->set_left_eye_radius(eye_rad);
  good = this->left_trans_x_from_lateral_canthus(trx);
  if(!good)
    return false;

  // a kludge for the moment===>
  double rlat = left_params_.x_max_*eye_rad;
  d_trx = left_params_.x_off_coef_*rlat;
  //<======
  d_try = rlat*vcl_sin(left_params_.dphi_rad_);

  trx -= d_trx;
  trz = max_left_z - eye_rad;
#else
  good = this->fit_sclera("left_eye_sclera");
  if(!good)
    return false;
  this->set_left_eye_radius(left_params_.sph_.radius());
  trx = left_params_.sph_.centre().x();
  trz = left_params_.sph_.centre().z();
#endif

  this->set_left_trans_x(trx);
  this->set_left_trans_z(trz);
  // setting trans y from the sphere center seems not as accurate
  // as from the lateral canthus
  good = this->left_trans_y_from_lateral_canthus(tr_y);
  if(!good)
    return false;
  tr_y -= d_try;
  this->set_left_trans_y(tr_y);

#if !USE_SPHERE 
  left_params_.init_sphere();
#endif
  double left_x_scale;
  good = this->left_eye_x_scale(left_x_scale);
  if(!good)
    return false;
  this->set_left_eye_x_scale(left_x_scale);

#if 0// don't change y scale or default lid t values
  double left_y_scale=1.0;//jlm
  //good = this->left_eye_y_scale(left_y_scale);
  if(!good)
    return false;
  this->set_left_eye_y_scale(left_y_scale);
  double left_inferior_margin_t;
  good = this->left_eye_inferior_margin_t(left_inferior_margin_t);
  if(!good)
    return false;
  this->set_left_eye_inferior_margin_t(left_inferior_margin_t);

  double left_superior_margin_t;
  good = this->left_eye_superior_margin_t(left_superior_margin_t);
  if(!good)
    return false;
  this->set_left_eye_superior_margin_t(left_superior_margin_t);
#endif

#if SET_CREASE_T
  double crease_sup_margin_t;
  good = this->left_eye_superior_margin_crease_t(crease_sup_margin_t);
  if(!good)
    return false;
  this->set_left_eye_superior_margin_crease_t(crease_sup_margin_t);
#else
  double crease_scale_y;
  good = this->left_eyelid_crease_scale_y(crease_scale_y);
  if(!good)
    return false;
  this->set_left_eyelid_crease_scale_y(crease_scale_y);
#endif
  double crease_z;
  good = this->left_mid_eyelid_crease_z(crease_z);
  if(!good)
    return false;
  this->set_left_mid_eyelid_crease_z(crease_z);
  return true;
}
void boxm2_vecf_fit_orbit::set_left_params(boxm2_vecf_orbit_params const& params){
  left_params_ = params;
  left_params_.init_sphere();
}

bool boxm2_vecf_fit_orbit::fit_right(){
  bool good = false;
  double ang_rad = 0.0, d_trx = 0.0, trx = 0.0, d_try = 0.0, tr_y = 0.0, trz = 0.0, eye_rad;
  good =  right_medial_lateral_angle(ang_rad);
  if(!good)
    return false;
  this->set_right_medial_lateral_angle(ang_rad);

  good= this->set_right_iris_radius();
 // if(!good)
 //   return false;

#if !USE_SPHERE 
  double max_right_z = 0.0;
  good = this->max_sclera_z("right_eye_sclera", max_right_z);
   if(!good)
    return false;
   good = this->right_eye_radius(eye_rad);
  if(!good)
    return false;
  this->set_right_eye_radius(eye_rad);
  good = this->right_trans_x_from_lateral_canthus(trx);
  if(!good)
    return false;

  // a kludge for the moment===>
  double rlat = right_params_.x_max_*eye_rad;
  d_trx = right_params_.x_off_coef_*rlat;
  //<====
  d_try = rlat*vcl_sin(right_params_.dphi_rad_);
  
  trx += d_trx;
  trz = max_right_z - eye_rad;
#else
  good = this->fit_sclera("right_eye_sclera");
  if(!good)
    return false;
  this->set_right_eye_radius(right_params_.sph_.radius());
  trx = right_params_.sph_.centre().x();
  trz = right_params_.sph_.centre().z();
#endif

  this->set_right_trans_x(trx);
  this->set_right_trans_z(trz);

  // setting trans y from the sphere center seems not as accurate
  // as from the outer cusp.
  good = this->right_trans_y_from_lateral_canthus(tr_y);
  if(!good)
    return false;
  tr_y -= d_try;
  this->set_right_trans_y(tr_y);
#if !USE_SPHERE 
  right_params_.init_sphere();
#endif
  double right_x_scale;
  good = this->right_eye_x_scale(right_x_scale);
  if(!good)
    return false;
  this->set_right_eye_x_scale(right_x_scale);

#if 0 // >>don't set the scale or margin t values<<
  double right_y_scale = 1.0; //jlm
  //  good = this->right_eye_y_scale(right_y_scale); 
  if(!good)
    return false;
  this->set_right_eye_y_scale(right_y_scale);

  double right_inferior_margin_t;
  good = this->right_eye_inferior_margin_t(right_inferior_margin_t);
  if(!good)
    return false;
  this->set_right_eye_inferior_margin_t(right_inferior_margin_t);

  double right_superior_margin_t;
  good = this->right_eye_superior_margin_t(right_superior_margin_t);
  if(!good)
    return false;
  this->set_right_eye_superior_margin_t(right_superior_margin_t);
#endif

#if SET_CREASE_T
  double crease_sup_margin_t;
  good = this->right_eye_superior_margin_crease_t(crease_sup_margin_t);
  if(!good)
    return false;
  this->set_right_eye_superior_margin_crease_t(crease_sup_margin_t);
#else
  double crease_scale_y;
  good = this->right_eyelid_crease_scale_y(crease_scale_y);
  if(!good)
    return false;
  this->set_right_eyelid_crease_scale_y(crease_scale_y);
#endif
  double crease_z;
  good = this->right_mid_eyelid_crease_z(crease_z);
  if(!good)
    return false;
  this->set_right_mid_eyelid_crease_z(crease_z);
  return true;
}
void boxm2_vecf_fit_orbit::set_right_params(boxm2_vecf_orbit_params const& params){
  right_params_ = params;
  right_params_.init_sphere();
}
bool boxm2_vecf_fit_orbit::load_orbit_data(vcl_string const& data_desc, vcl_string const& path){
  vcl_map<vcl_string, mids>::iterator iit = smid_map_.find(data_desc);
  if(iit == smid_map_.end() ){
    vcl_cout << "data label " << data_desc << " doesn't exist\n";
    return false;
  } 
  vcl_ifstream istr(path.c_str());
  if(!istr.is_open()){
    vcl_cout << "data file for type "<< data_desc << " at " << path << " not found\n";
    return false;
  }
  while(true){
    if(istr.eof())
      break;
    double x, y, z;
    unsigned char c;
    istr >> x >> c;
    if(c!=',')
      return false;
    istr >> y >> c;
    if(c!=',')
      return false;
    istr >> z ;
    vgl_point_3d<double> p(x,y,z);
    orbit_data_[ iit->second ].push_back(p);
  }
return true;
}
void boxm2_vecf_fit_orbit::plot_inferior_margin(vcl_vector<vgl_point_3d<double> >& pts, bool is_right, double xm_min, double xm_max){
  double left_tinf = left_params_.lower_eyelid_tmin_;
  double right_tinf = right_params_.lower_eyelid_tmin_;
  boxm2_vecf_eyelid lelid(left_params_);
  boxm2_vecf_eyelid relid(right_params_);
  for(double xm = xm_min; xm<=xm_max; xm+=0.25){
    double yy=0.0, zz = 0.0;
    if(is_right){
        yy = relid.Y(-xm, right_tinf);
        zz = relid.Z(-xm, right_tinf);
    }else{
        yy = lelid.Y(xm, left_tinf);
        zz = lelid.Z(xm, left_tinf);
      }
    vgl_point_3d<double> p(xm, yy, zz);
    pts.push_back(p);
  }
}
void boxm2_vecf_fit_orbit::plot_superior_margin(vcl_vector<vgl_point_3d<double> >& pts, bool is_right, double xm_min, double xm_max){
  boxm2_vecf_eyelid lelid(left_params_);
  boxm2_vecf_eyelid relid(right_params_);
  double lid_t = 0.0;
  if(is_right)
    lid_t = right_params_.eyelid_tmin_;
  else
    lid_t = left_params_.eyelid_tmin_;

  for(double xm = xm_min; xm<=xm_max; xm+=0.25){
    double yy=0.0, zz = 0.0;
    if(is_right){
        yy = relid.Y(-xm, lid_t);
        zz = relid.Z(-xm, lid_t);
    }else{
       yy = lelid.Y(xm, lid_t);
       zz = lelid.Z(xm, lid_t);
    }
    vgl_point_3d<double> p(xm, yy, zz);
      pts.push_back(p);
  }
}
void boxm2_vecf_fit_orbit::plot_crease(vcl_vector<vgl_point_3d<double> >& pts, bool is_right, double xm_min, double xm_max){
  boxm2_vecf_eyelid_crease lcrease(left_params_);
  boxm2_vecf_eyelid_crease rcrease(right_params_);
  double ct = left_params_.eyelid_crease_ct_;
  if(is_right)
    ct = right_params_.eyelid_crease_ct_;
  for(double xm = xm_min; xm<=xm_max; xm+=0.25){
    double yy=0.0, zz = 0.0;
    if(is_right){
      yy = rcrease.Y(-xm, ct);
      zz = rcrease.Z(-xm, ct);
    }else{
       yy = lcrease.Y(xm, ct);
       zz = lcrease.Z(xm, ct);
    }
    vgl_point_3d<double> p(xm, yy, zz);
    pts.push_back(p);
  }
}
bool boxm2_vecf_fit_orbit::plot_orbit_data(vcl_string const& data_desc, vcl_vector<vgl_point_3d<double> > & data){
  vcl_map<vcl_string, mids>::iterator iit = smid_map_.find(data_desc);
  if(iit == smid_map_.end() ){
    vcl_cout << "data label " << data_desc << " doesn't exist\n";
    return false;
  } 
  vcl_vector<vgl_point_3d<double> >& pts = orbit_data_[iit->second];
  if(!pts.size()){
    vcl_cout << "No data of type " << data_desc << '\n';
    return false;
  }
  bool is_right = iit->second > LEFT_IRIS_RADIUS; 
  boxm2_vecf_orbit_params params = left_params_;
  if(is_right)
    params = right_params_;

  for(unsigned i = 0; i<pts.size(); ++i){
    vgl_point_3d<double>& p = pts[i];
    double xm = p.x()-params.x_trans();
    double y = p.y()-params.trans_y_;
    double z = p.z()-params.trans_z_;
    vgl_point_3d<double> pd(xm, y, z);
    data.push_back(pd);
  }
  return true;
}
bool boxm2_vecf_fit_orbit::plot_orbit_model(vcl_string const& data_desc, vcl_vector<vgl_point_3d<double> >& model, double xm_min, double xm_max){
  vcl_map<vcl_string, mids>::iterator iit = smid_map_.find(data_desc);
  if(iit == smid_map_.end() ){
    vcl_cout << "data label " << data_desc << " doesn't exist\n";
    return false;
  } 
  vcl_vector<vgl_point_3d<double> >& pts = orbit_data_[iit->second];
  if(!pts.size()){
    vcl_cout << "No data of type " << data_desc << '\n';
    return false;
  }
  bool is_right = iit->second > LEFT_IRIS_RADIUS; 
  boxm2_vecf_orbit_params params = left_params_;
  if(is_right)
    params = right_params_;
  if(iit->second == LEFT_EYE_INFERIOR_MARGIN || iit->second == RIGHT_EYE_INFERIOR_MARGIN )
    this->plot_inferior_margin(model, is_right, xm_min, xm_max);
  else if(iit->second == LEFT_EYE_SUPERIOR_MARGIN || iit->second == RIGHT_EYE_SUPERIOR_MARGIN )
    this->plot_superior_margin(model, is_right, xm_min, xm_max);
  else if(iit->second == LEFT_EYE_SUPERIOR_CREASE || iit->second == RIGHT_EYE_SUPERIOR_CREASE )
    this->plot_crease(model, is_right, xm_min, xm_max);
  return true;
}
bool boxm2_vecf_fit_orbit::display_anchors(vcl_ofstream& ostr, bool is_right){
  if(!ostr)
    return false;
  vcl_map<mids, labeled_point>::iterator lit;
  boxm2_vecf_orbit_params params = left_params_;
  if(is_right)
    params = right_params_;
  vgl_vector_3d<double> v(params.x_trans(), params.y_trans(), params.z_trans());
  float r = static_cast<float>(params.eye_radius_)*0.05f;
  if(is_right){
    lit = lpts_.find(RIGHT_EYE_MEDIAL_CANTHUS);
    if(lit == lpts_.end())
      return false;
    const vgl_point_3d<double>& re_mc  = lit->second.p3d_-v;
    vgl_point_3d<float> pmc(static_cast<float>(re_mc.x()),
                            static_cast<float>(re_mc.y()),
                            static_cast<float>(re_mc.z()));
    vgl_sphere_3d<float> spmc(pmc, r);
    // medial canthus PURPLE
    bvrml_write::write_vrml_sphere(ostr, spmc, 1.0f, 0.0f, 1.0f);

    lit = lpts_.find(RIGHT_EYE_LATERAL_CANTHUS);
    if(lit == lpts_.end())
      return false;
    const vgl_point_3d<double>& re_lc  = lit->second.p3d_-v;
    vgl_point_3d<float> plc(static_cast<float>(re_lc.x()),
                            static_cast<float>(re_lc.y()),
                            static_cast<float>(re_lc.z()));
    vgl_sphere_3d<float> splc(plc, r);
    // lateral canthus BLUE
    bvrml_write::write_vrml_sphere(ostr, splc, 0.0f, 0.0f, 1.0f);

    lit = lpts_.find(RIGHT_EYE_INFERIOR_MARGIN);
    if(lit == lpts_.end())
      return false;
    const vgl_point_3d<double>& re_im  = lit->second.p3d_-v;
    vgl_point_3d<float> pim(static_cast<float>(re_im.x()),
                            static_cast<float>(re_im.y()),
                            static_cast<float>(re_im.z()));
    vgl_sphere_3d<float> spim(pim, r);
    // inferior margin yellow
    bvrml_write::write_vrml_sphere(ostr, spim, 1.0f, 1.0f, 0.0f);

    lit = lpts_.find(RIGHT_EYE_SUPERIOR_MARGIN);
    if(lit == lpts_.end())
      return false;
    const vgl_point_3d<double>& re_sm  = lit->second.p3d_-v;
    vgl_point_3d<float> psm(static_cast<float>(re_sm.x()),
                            static_cast<float>(re_sm.y()),
                            static_cast<float>(re_sm.z()));
    vgl_sphere_3d<float> spsm(psm, r);
    // superior margin red
    bvrml_write::write_vrml_sphere(ostr, spsm, 1.0f, 0.0f, 0.0f);
    lit = lpts_.find(RIGHT_EYE_SUPERIOR_CREASE);
    if(lit == lpts_.end())
      return false;
    const vgl_point_3d<double>& re_sc  = lit->second.p3d_-v;
    vgl_point_3d<float> psc(static_cast<float>(re_sc.x()),
                            static_cast<float>(re_sc.y()),
                            static_cast<float>(re_sc.z()));
    vgl_sphere_3d<float> spsc(psc, r);
    // superior crease cyan
    bvrml_write::write_vrml_sphere(ostr, spsc, 0.0f, 1.0, 1.0f);
  }else{
    lit = lpts_.find(LEFT_EYE_MEDIAL_CANTHUS);
    if(lit == lpts_.end())
      return false;
    const vgl_point_3d<double>& le_mc  = lit->second.p3d_-v;
    vgl_point_3d<float> pmc(static_cast<float>(le_mc.x()),
                            static_cast<float>(le_mc.y()),
                            static_cast<float>(le_mc.z()));
    vgl_sphere_3d<float> spmc(pmc, r);
    bvrml_write::write_vrml_sphere(ostr, spmc, 1.0f, 0.0f, 1.0f);

    lit = lpts_.find(LEFT_EYE_LATERAL_CANTHUS);
    if(lit == lpts_.end())
      return false;
    const vgl_point_3d<double>& le_lc  = lit->second.p3d_-v;
    vgl_point_3d<float> plc(static_cast<float>(le_lc.x()),
                            static_cast<float>(le_lc.y()),
                            static_cast<float>(le_lc.z()));
    vgl_sphere_3d<float> splc(plc, r);
    bvrml_write::write_vrml_sphere(ostr, splc, 0.0f, 0.0f, 1.0f);

    lit = lpts_.find(LEFT_EYE_INFERIOR_MARGIN);
    if(lit == lpts_.end())
      return false;
    const vgl_point_3d<double>& re_im  = lit->second.p3d_-v;
    vgl_point_3d<float> pim(static_cast<float>(re_im.x()),
                            static_cast<float>(re_im.y()),
                            static_cast<float>(re_im.z()));
    vgl_sphere_3d<float> spim(pim, r);
    // inferior margin yellow
    bvrml_write::write_vrml_sphere(ostr, spim, 1.0f, 1.0f, 0.0f);

    lit = lpts_.find(LEFT_EYE_SUPERIOR_MARGIN);
    if(lit == lpts_.end())
      return false;
    const vgl_point_3d<double>& re_sm  = lit->second.p3d_-v;
    vgl_point_3d<float> psm(static_cast<float>(re_sm.x()),
                            static_cast<float>(re_sm.y()),
                            static_cast<float>(re_sm.z()));
    vgl_sphere_3d<float> spsm(psm, r);
    // superior margin red
    bvrml_write::write_vrml_sphere(ostr, spsm, 1.0f, 0.0f, 0.0f);
    lit = lpts_.find(LEFT_EYE_SUPERIOR_CREASE);
    if(lit == lpts_.end())
      return false;
    const vgl_point_3d<double>& re_sc  = lit->second.p3d_-v;
    vgl_point_3d<float> psc(static_cast<float>(re_sc.x()),
                            static_cast<float>(re_sc.y()),
                            static_cast<float>(re_sc.z()));
    vgl_sphere_3d<float> spsc(psc, r);
    // superior crease cyan
    bvrml_write::write_vrml_sphere(ostr, spsc, 0.0f, 1.0, 1.0f);
  }
  return true;
}
bool boxm2_vecf_fit_orbit::display_orbit_vrml(vcl_ofstream& ostr, bool is_right, bool show_model){
  
  if(!ostr)
    return false;
  bvrml_write::write_vrml_header(ostr);
  // plot sphere fit
  boxm2_vecf_orbit_params params = left_params_;
  if(is_right)
    params = right_params_;
  // slight expansion of range to insure complete plot
  double xm_min = params.x_min()-2.0;
  double xm_max = params.x_max()+2.0;
  vgl_sphere_3d<double>& sphere = params.sph_;
  vgl_vector_3d<double> v(params.x_trans(), params.y_trans(), params.z_trans());
  vgl_point_3d<double> c = sphere.centre()-v;
  float cx = static_cast<float>(c.x());
  float cy = static_cast<float>(c.y());
  float cz = static_cast<float>(c.z());
  float r = static_cast<float>(sphere.radius());
  vgl_sphere_3d<float> fsphere(cx, cy, cz, r);
  bvrml_write::write_vrml_sphere(ostr, fsphere, 1.0f, 1.0f, 1.0f, 0.2f);
  // write the iris
  float irad = static_cast<float>(params.iris_radius_);
  float prad = static_cast<float>(params.pupil_radius_);
  bvrml_write::write_vrml_cylinder(ostr, vgl_point_3d<double>(0.0, cy, r-0.3),  vgl_vector_3d<double>(0.0, 0.0, 1.0), irad, 0.5f, 0.0f, 0.5f, 0.75f);
  bvrml_write::write_vrml_cylinder(ostr, vgl_point_3d<double>(0.0, cy, r+0.25), vgl_vector_3d<double>(0.0, 0.0, 1.0), prad, 0.1f, 0.0f, 0.0f, 0.0f);

  // write the sclera points
  vcl_vector<vgl_point_3d<double> > pts = orbit_data_[LEFT_SCLERA];
  if(is_right)
     pts = orbit_data_[RIGHT_SCLERA];
  for(vcl_vector<vgl_point_3d<double> >::iterator pit = pts.begin();
      pit != pts.end(); ++pit){
    vgl_point_3d<double>& p = *pit;
    vgl_point_3d<float> pf(static_cast<float>(p.x()-v.x()),
                           static_cast<float>(p.y()-v.y()),
                           static_cast<float>(p.z()-v.z()));
    vgl_sphere_3d<float> sp(pf, r*0.025f);
    bvrml_write::write_vrml_sphere(ostr, sp, 0.0f, 1.0f, 0.0f);
  }
  pts.clear();
  // display the inferior margin
  pts = orbit_data_[LEFT_EYE_INFERIOR_MARGIN];
  if(is_right)
     pts = orbit_data_[RIGHT_EYE_INFERIOR_MARGIN];
  for(vcl_vector<vgl_point_3d<double> >::iterator pit = pts.begin();
      pit != pts.end(); ++pit){
    vgl_point_3d<double>& p = *pit-v;
    vgl_point_3d<float> pf(static_cast<float>(p.x()), static_cast<float>(p.y()), static_cast<float>(p.z()));
    vgl_sphere_3d<float> sp(pf, r*0.025f);
    bvrml_write::write_vrml_sphere(ostr, sp, 1.0f, 1.0f, 0.0f);
  }
  if(show_model){
  vcl_vector<vgl_point_3d<double> > inf_marg_pts;
  this->plot_inferior_margin(inf_marg_pts, is_right, xm_min, xm_max);
  for(vcl_vector<vgl_point_3d<double> >::iterator pit = inf_marg_pts.begin();
      pit != inf_marg_pts.end(); ++pit){
    vgl_point_3d<double> p = *pit;
    vgl_point_3d<float> pf(static_cast<float>(p.x()), static_cast<float>(p.y()), static_cast<float>(p.z()));
    vgl_sphere_3d<float> sp(pf, r*0.025f);
    bvrml_write::write_vrml_sphere(ostr, sp, 0.75f, 0.75f, 0.0f);
  }
  }
  pts.clear();
  // display the superior margin
  pts = orbit_data_[LEFT_EYE_SUPERIOR_MARGIN];
  if(is_right)
     pts = orbit_data_[RIGHT_EYE_SUPERIOR_MARGIN];
  for(vcl_vector<vgl_point_3d<double> >::iterator pit = pts.begin();
      pit != pts.end(); ++pit){
    vgl_point_3d<double>& p = *pit-v;
    vgl_point_3d<float> pf(static_cast<float>(p.x()), static_cast<float>(p.y()), static_cast<float>(p.z()));
    vgl_sphere_3d<float> sp(pf, r*0.025f);
    bvrml_write::write_vrml_sphere(ostr, sp, 1.0f, 0.0f, 0.0f);
  }
  if(show_model){
  vcl_vector<vgl_point_3d<double> > sup_marg_pts;
  this->plot_superior_margin(sup_marg_pts, is_right, xm_min, xm_max);
  for(vcl_vector<vgl_point_3d<double> >::iterator pit = sup_marg_pts.begin();
      pit != sup_marg_pts.end(); ++pit){
    vgl_point_3d<double> p = *pit;
    vgl_point_3d<float> pf(static_cast<float>(p.x()), static_cast<float>(p.y()), static_cast<float>(p.z()));
    vgl_sphere_3d<float> sp(pf, r*0.025f);
    bvrml_write::write_vrml_sphere(ostr, sp, 0.75f, 0.0f, 0.0f);
  }
  }
  pts.clear();
  // display the superior crease
  pts = orbit_data_[LEFT_EYE_SUPERIOR_CREASE];
  if(is_right)
     pts = orbit_data_[RIGHT_EYE_SUPERIOR_CREASE];
  for(vcl_vector<vgl_point_3d<double> >::iterator pit = pts.begin();
      pit != pts.end(); ++pit){
    vgl_point_3d<double>& p = *pit-v;
    vgl_point_3d<float> pf(static_cast<float>(p.x()), static_cast<float>(p.y()), static_cast<float>(p.z()));
    vgl_sphere_3d<float> sp(pf, r*0.025f);
    bvrml_write::write_vrml_sphere(ostr, sp, 0.0f, 1.0f, 1.0f);
  }
  if(show_model){
  vcl_vector<vgl_point_3d<double> > sup_crease_pts;
  this->plot_crease(sup_crease_pts, is_right, xm_min, xm_max);
  for(vcl_vector<vgl_point_3d<double> >::iterator pit = sup_crease_pts.begin();
      pit != sup_crease_pts.end(); ++pit){
    vgl_point_3d<double> p = *pit;
    vgl_point_3d<float> pf(static_cast<float>(p.x()), static_cast<float>(p.y()), static_cast<float>(p.z()));
    vgl_sphere_3d<float> sp(pf, r*0.025f);
    bvrml_write::write_vrml_sphere(ostr, sp, 0.0f, 0.75f, 0.75f);
  }
  }
  this->display_anchors(ostr, is_right);
  ostr.close();
  return true;
}
