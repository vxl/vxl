#include <vcl_fstream.h>
#include "boxm2_vecf_fit_orbit.h"
#include "boxm2_vecf_eyelid.h"
#include "boxm2_vecf_eyelid_crease.h"
#include <vgl/vgl_vector_3d.h>
#include <vgl/algo/vgl_fit_sphere_3d.h>
# define USE_SPHERE 1
void boxm2_vecf_fit_orbit::fill_smid_map(){
  smid_map_["left_eye_inner_cusp"]=LEFT_EYE_INNER_CUSP;
  smid_map_["left_eye_outer_cusp"]=LEFT_EYE_OUTER_CUSP;
  smid_map_["left_eye_mid_lower_lid"]=LEFT_EYE_MID_LOWER_LID;
  smid_map_["left_eye_mid_upper_lid"]=LEFT_EYE_MID_UP_LID;
  smid_map_["left_eye_mid_crease"]=LEFT_EYE_MID_CREASE;
  smid_map_["left_eye_sclera"]=LEFT_SCLERA;
  smid_map_["left_Nz"]=LEFT_Nz;
  smid_map_["left_iris_radius"]=LEFT_IRIS_RADIUS;

  smid_map_["right_eye_inner_cusp"]=RIGHT_EYE_INNER_CUSP;
  smid_map_["right_eye_outer_cusp"]=RIGHT_EYE_OUTER_CUSP;
  smid_map_["right_eye_mid_lower_lid"]=RIGHT_EYE_MID_LOWER_LID;
  smid_map_["right_eye_mid_upper_lid"]=RIGHT_EYE_MID_UP_LID;
  smid_map_["right_eye_mid_crease"]=RIGHT_EYE_MID_CREASE;
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
  lit = lpts_.find(LEFT_EYE_INNER_CUSP);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& le_ic  = lit->second.p3d_;
  lit = lpts_.find(LEFT_EYE_OUTER_CUSP);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& le_oc  = lit->second.p3d_;
  double dx = le_oc.x()-le_ic.x();
  rad = dx/(left_params_.x_max_-left_params_.x_min_);
  return true;
}

bool boxm2_vecf_fit_orbit::left_trans_x_from_outer_cusp(double& trx){
        vcl_map<mids, labeled_point>::iterator lit;
  lit = lpts_.find(LEFT_EYE_OUTER_CUSP);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& le_oc  = lit->second.p3d_;
  double xm = left_params_.eye_radius_*left_params_.x_max_;
  trx = (le_oc.x()-xm);
  return true;
}

bool boxm2_vecf_fit_orbit::left_trans_y_from_outer_cusp( double& tr_y){
  vcl_map<mids, labeled_point>::iterator lit;
  lit = lpts_.find(LEFT_EYE_OUTER_CUSP);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& le_oc  = lit->second.p3d_;
  tr_y = le_oc.y()-left_params_.y_off_;
  return true;
}

bool boxm2_vecf_fit_orbit::left_eye_x_scale(double& left_x_scale){
  vcl_map<mids, labeled_point>::iterator lit;
  lit = lpts_.find(LEFT_EYE_OUTER_CUSP);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& le_oc  = lit->second.p3d_;
  double x = le_oc.x()-left_params_.trans_x_;
  double xm = left_params_.eye_radius_*left_params_.x_max_;
  double temp  = x/xm;
  
  lit = lpts_.find(LEFT_EYE_INNER_CUSP);
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
  lit = lpts_.find(LEFT_EYE_MID_LOWER_LID);
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
bool boxm2_vecf_fit_orbit::left_eye_upper_lid_t(double& upper_lid_t){
  vcl_map<mids, labeled_point>::iterator lit;
  lit = lpts_.find(LEFT_EYE_MID_UP_LID);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& mid_upper  = lit->second.p3d_;
   double temp = left_params_.trans_y_;
  double y = mid_upper.y()-temp;
  double xm = mid_upper.x()-left_params_.x_trans();
  boxm2_vecf_eyelid elid(left_params_);
  upper_lid_t = elid.t(xm, y);
  return true;
}

bool boxm2_vecf_fit_orbit::left_eyelid_crease_scale_y(double& crease_scale_y){
  vcl_map<mids, labeled_point>::iterator lit;
  lit = lpts_.find(LEFT_EYE_MID_CREASE);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& mid_crease  = lit->second.p3d_;
  double temp = left_params_.trans_y_;
  double y = mid_crease.y()-temp;
  double xm = mid_crease.x() - left_params_.x_trans();
  boxm2_vecf_eyelid_crease crease(left_params_);
  double yy = crease.Y(xm, 0.0);
  crease_scale_y = y/yy;
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
    right_sphere_ = sph;
  else
    left_sphere_ = sph;
  return true;
}

bool boxm2_vecf_fit_orbit::right_eye_radius( double& rad){
  // find right eye inner cusp - corresponds to xmin
  vcl_map<mids, labeled_point>::iterator lit;
  lit = lpts_.find(RIGHT_EYE_INNER_CUSP);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& re_ic  = lit->second.p3d_;
  lit = lpts_.find(RIGHT_EYE_OUTER_CUSP);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& re_oc  = lit->second.p3d_;
  double dx =(re_ic.x()-re_oc.x());
 rad = dx/(right_params_.x_max_-right_params_.x_min_);

  return true;
}

bool boxm2_vecf_fit_orbit::right_trans_y_from_outer_cusp( double& tr_y){
  vcl_map<mids, labeled_point>::iterator lit;
  lit = lpts_.find(RIGHT_EYE_OUTER_CUSP);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& re_oc  = lit->second.p3d_;
  tr_y = re_oc.y()-right_params_.y_off_;
  return true;
}

bool boxm2_vecf_fit_orbit::right_trans_x_from_outer_cusp(double& trx){
        vcl_map<mids, labeled_point>::iterator lit;
  lit = lpts_.find(RIGHT_EYE_OUTER_CUSP);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& re_oc  = lit->second.p3d_;
  double xm = -right_params_.eye_radius_*right_params_.x_max_;
  trx =(re_oc.x()-xm);
  return true;
}

bool boxm2_vecf_fit_orbit::right_eye_x_scale(double& right_x_scale){
  vcl_map<mids, labeled_point>::iterator lit;
  lit = lpts_.find(RIGHT_EYE_OUTER_CUSP);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& re_oc  = lit->second.p3d_;
  double xoc = re_oc.x()-right_params_.trans_x_;
  double xmx = -(right_params_.eye_radius_*right_params_.x_max_);

  lit = lpts_.find(RIGHT_EYE_INNER_CUSP);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& re_ic  = lit->second.p3d_;
  double xic = re_ic.x()-left_params_.trans_x_;
  double xmi = -right_params_.eye_radius_*right_params_.x_min_;
  right_x_scale = vcl_fabs((xmx-xmi)/(xoc-xic));
  return true;
}


bool boxm2_vecf_fit_orbit::right_eye_y_scale(double& right_y_scale){
  vcl_map<mids, labeled_point>::iterator lit;
  lit = lpts_.find(RIGHT_EYE_MID_LOWER_LID);
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
bool boxm2_vecf_fit_orbit::right_eye_upper_lid_t(double& upper_lid_t){
  vcl_map<mids, labeled_point>::iterator lit;
  lit = lpts_.find(RIGHT_EYE_MID_UP_LID);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& mid_upper  = lit->second.p3d_;
  double temp = right_params_.trans_y_;
  double y = mid_upper.y()-temp;
  double xm = mid_upper.x()- right_params_.x_trans();
  boxm2_vecf_eyelid elid(right_params_);
  upper_lid_t = elid.t(-xm, y);
  return true;
}

bool boxm2_vecf_fit_orbit::right_eyelid_crease_scale_y(double& crease_scale_y){
  vcl_map<mids, labeled_point>::iterator lit;
  lit = lpts_.find(RIGHT_EYE_MID_CREASE);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& mid_crease  = lit->second.p3d_;
  double temp = right_params_.trans_y_;
  double y = mid_crease.y()-temp;
  double xm = mid_crease.x() - right_params_.x_trans();
  boxm2_vecf_eyelid_crease crease(right_params_);
  double yy = crease.Y(xm, 0.0);
  crease_scale_y = y/yy;
  return true;
}

bool boxm2_vecf_fit_orbit::fit_left(){
  bool good = this->fit_sclera("left_eye_sclera");
  if(!good)
    return false;
  double trx = 0.0, tr_y = 0.0, trz = 0.0;
// the fitted sphere radius appears to be more accurate than from the cusp distances
// also the x origin seems more accurate when estimated from the sphere center than cusps
#if !USE_SPHERE 
  double eye_rad; 
  bool good = this->left_eye_radius(eye_rad);
  if(!good)
    return false;
  this->set_left_eye_radius(eye_rad);
  good = this->left_trans_x(trx);
  if(!good)
    return false;
#else
  this->set_left_eye_radius(left_sphere_.radius());

  trx = left_sphere_.centre().x(); trz = left_sphere_.centre().z();
  this->set_left_trans_z(trz);
#endif

  this->set_left_trans_x(trx);

  // setting trans y from the sphere center seems not as accurate
  // as from the outer cusp.
  good = this->left_trans_y_from_outer_cusp(tr_y);
  if(!good)
    return false;
  this->set_left_trans_y(tr_y);

  double left_x_scale;
  good = this->left_eye_x_scale(left_x_scale);
  if(!good)
    return false;
  this->set_left_eye_x_scale(left_x_scale);

  double left_y_scale; 
  good = this->left_eye_y_scale(left_y_scale);
  if(!good)
    return false;
  this->set_left_eye_y_scale(left_y_scale);
  double left_upper_lid_t;
  good = this->left_eye_upper_lid_t(left_upper_lid_t);
  if(!good)
    return false;
  this->set_left_eye_upper_lid_t(left_upper_lid_t);
  double crease_scale_y;
  good = this->left_eyelid_crease_scale_y(crease_scale_y);
  if(!good)
    return false;
  this->set_left_eyelid_crease_scale_y(crease_scale_y);
  return true;
}

bool boxm2_vecf_fit_orbit::fit_right(){
  bool good = this->fit_sclera("right_eye_sclera");
  if(!good)
    return false;
  double trx = 0.0, tr_y = 0.0, trz = 0.0;
  // the fitted sphere radius appears to be more accurate than estimated from the cusp distances
  // also the x origin seems more accurate when estimated from the sphere center than from cusps
#if !USE_SPHERE
  double eye_rad; 
  bool good = this->right_eye_radius(eye_rad);
  if(!good)
    return false;
  this->set_right_eye_radius(eye_rad);
  good = this->right_trans_x(trx);
  if(!good)
    return false;
#else
  this->set_right_eye_radius(right_sphere_.radius());

  trx = right_sphere_.centre().x(); trz = right_sphere_.centre().z();
  this->set_right_trans_z(trz);
#endif

  this->set_right_trans_x(trx);

  // setting trans y from the sphere center seems not as accurate
  // as from the outer cusp.
  good = this->right_trans_y_from_outer_cusp(tr_y);
  if(!good)
    return false;
  this->set_right_trans_y(tr_y);

  double right_x_scale;
  good = this->right_eye_x_scale(right_x_scale);
  if(!good)
    return false;
  this->set_right_eye_x_scale(right_x_scale);

  double right_y_scale; 
  good = this->right_eye_y_scale(right_y_scale);
  if(!good)
    return false;
  this->set_right_eye_y_scale(right_y_scale);

  double right_upper_lid_t;
  good = this->right_eye_upper_lid_t(right_upper_lid_t);
  if(!good)
    return false;
  this->set_right_eye_upper_lid_t(right_upper_lid_t);
  double crease_scale_y;
  good = this->right_eyelid_crease_scale_y(crease_scale_y);
  if(!good)
    return false;
  this->set_right_eyelid_crease_scale_y(crease_scale_y);
  return true;
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
void boxm2_vecf_fit_orbit::plot_lower_lid(vcl_ostream& ostr, bool is_right){
  boxm2_vecf_eyelid lelid(left_params_);
  boxm2_vecf_eyelid relid(right_params_);
  for(double xm = -20.0; xm<=20.0; xm+=0.5){
    double yy=0.0, zz = 0.0;
    if(is_right){
      yy = relid.Y(-xm, 1.05);
      zz = relid.Z(-xm, 1.05);
    }else{
       yy = lelid.Y(xm, 1.05);
       zz = relid.Z(xm, 1.05);
    }
    ostr << xm << ' ' << yy << ' ' << zz << '\n';
  }
}
void boxm2_vecf_fit_orbit::plot_upper_lid(vcl_ostream& ostr, bool is_right){
  boxm2_vecf_eyelid lelid(left_params_);
  boxm2_vecf_eyelid relid(right_params_);
  double lid_t = 0.0;
  if(is_right)
    lid_t = 1.0+right_params_.eyelid_dt_;
  else
    lid_t = 1.0+left_params_.eyelid_dt_;

  for(double xm = -20.0; xm<=20.0; xm+=0.5){
    double yy=0.0, zz = 0.0;
    if(is_right){
      yy = relid.Y(-xm, lid_t);
      zz = relid.Z(-xm, lid_t);
    }else{
       yy = lelid.Y(xm, lid_t);
       zz = lelid.Z(xm, lid_t);
    }
    ostr << xm << ' ' << yy << ' ' << zz << '\n';
  }
}
void boxm2_vecf_fit_orbit::plot_crease(vcl_ostream& ostr, bool is_right){
  boxm2_vecf_eyelid_crease lcrease(left_params_);
  boxm2_vecf_eyelid_crease rcrease(right_params_);
  for(double xm = -20.0; xm<=20.0; xm+=0.5){
    double yy=0.0, zz = 0.0;
    if(is_right){
      yy = rcrease.Y(-xm, 0.0);
      zz = rcrease.Z(-xm, 0.0);
    }else{
       yy = lcrease.Y(xm, 0.0);
       zz = lcrease.Y(xm, 0.0);
    }
    ostr << xm << ' ' << yy << ' ' << zz << '\n';
  }
}
bool boxm2_vecf_fit_orbit::plot_orbit_data(vcl_string const& data_desc){
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
  vcl_cout << data_desc << " data values \n";
  for(unsigned i = 0; i<pts.size(); ++i){
    vgl_point_3d<double>& p = pts[i];
    double xm = p.x()-params.x_trans();
    double y = p.y()-params.trans_y_;
    double z = p.z()-params.trans_z_;
    vcl_cout << xm << ' ' << y << ' ' << z << '\n';
  }
  vcl_cout << data_desc << " model values\n";
  if(iit->second == LEFT_EYE_MID_LOWER_LID || iit->second == RIGHT_EYE_MID_LOWER_LID )
    this->plot_lower_lid(vcl_cout, is_right);
  else if(iit->second == LEFT_EYE_MID_UP_LID || iit->second == RIGHT_EYE_MID_UP_LID )
    this->plot_upper_lid(vcl_cout, is_right);
  else if(iit->second == LEFT_EYE_MID_CREASE || iit->second == RIGHT_EYE_MID_CREASE )
    this->plot_crease(vcl_cout, is_right);
  return true;
}
