#include <fstream>
#include <iostream>
#include <algorithm>
#include <limits>
#include "boxm2_vecf_labeled_point.h"
#include "boxm2_vecf_fit_orbit.h"
#include "boxm2_vecf_eyelid.h"
#include "boxm2_vecf_eyelid_crease.h"
#include "boxm2_vecf_plot_orbit.h"
#include <vgl/vgl_vector_3d.h>
#include <vgl/algo/vgl_fit_sphere_3d.h>
#include <bvrml/bvrml_write.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
# define USE_SPHERE 0
# define SET_CREASE_T 0
void boxm2_vecf_fit_orbit::fill_smid_map(){
  smid_map_["left_eye_medial_canthus"]=LEFT_EYE_MEDIAL_CANTHUS;
  smid_map_["left_eye_lateral_canthus"]=LEFT_EYE_LATERAL_CANTHUS;
  smid_map_["left_eye_inferior_margin"]=LEFT_EYE_INFERIOR_MARGIN;
  smid_map_["left_eye_superior_margin"]=LEFT_EYE_SUPERIOR_MARGIN;
  smid_map_["left_eye_superior_crease"]=LEFT_EYE_SUPERIOR_CREASE;
  smid_map_["left_eye_sclera"]=LEFT_SCLERA;
  smid_map_["left_eye_inferior_lid_surface"]=LEFT_INFERIOR_LID_SURFACE;
  smid_map_["left_Nz"]=LEFT_Nz;
  smid_map_["left_iris_radius"]=LEFT_IRIS_RADIUS;

  smid_map_["right_eye_medial_canthus"]=RIGHT_EYE_MEDIAL_CANTHUS;
  smid_map_["right_eye_lateral_canthus"]=RIGHT_EYE_LATERAL_CANTHUS;
  smid_map_["right_eye_inferior_margin"]=RIGHT_EYE_INFERIOR_MARGIN;
  smid_map_["right_eye_superior_margin"]=RIGHT_EYE_SUPERIOR_MARGIN;
  smid_map_["right_eye_superior_crease"]=RIGHT_EYE_SUPERIOR_CREASE;
  smid_map_["right_eye_sclera"]=RIGHT_SCLERA;
  smid_map_["right_eye_inferior_lid_surface"]=RIGHT_INFERIOR_LID_SURFACE;
  smid_map_["right_Nz"]=RIGHT_Nz;
  smid_map_["right_iris_radius"]=RIGHT_IRIS_RADIUS;
}
bool boxm2_vecf_fit_orbit::add_labeled_point(boxm2_vecf_labeled_point lp){
  auto iit = smid_map_.find(lp.label_);
  if(iit == smid_map_.end() ){
    std::cout << "Measurement label " << lp.label_ << " doesn't exist\n";
    return false;
  }
  lpts_[iit->second] = lp;
  return true;
}
//
// there are two anchor file formats at present:
// x, y, z, label
// ...
// i.e. 3-d point and label for all entries
// and one where the iris diameter and Nz values are scalar, i.e.,
// v, Nz
// v, Diris
//
// moreover in the point label format the corresponding labels are
// v, v, v, left_iris_radius (right_iris_radius)
// v, v, v, left_Nz (right_Nz)
//
// these lablels are the used in internal maps, not Diris or Nz
// so the reader maps to the internal labels. The implementation below
// can ingest either format
//
bool boxm2_vecf_fit_orbit::read_anchor_file(std::string const& path){
  only_2d_data_ = false;
  bool is_right = false;
  std::size_t found=path.find("right");
  if (found!=std::string::npos)
    is_right = true;

  // parse generic point with label file format
  std::map<std::string, std::vector<vgl_point_3d<double> > > anchors;
  bool good = boxm2_vecf_labeled_point::read_points(path, anchors);
  if(!good){
    std::cout << "Parse of file " << path << " failed\n";
    return false;
  }
  // now that the file is parsed the labeled points can be added to the
  // internal database, lpts_ (labeled points)
  for(auto & anchor : anchors){
    std::string lab = anchor.first;
    std::vector<vgl_point_3d<double> >& pts = anchor.second;
    double x = 0.0, y= 0.0, z = 0.0;
    double np = 0.0;
    for(auto pit = pts.begin();
        pit != pts.end(); ++pit, np+=1.0){
      x += pit->x(); y += pit->y(); z += pit->z();
    }
    if(np == 0.0){
    std::cout << "No points for label  " << lab << "\n";
    return false;
    }
    x /= np;      y /= np;  z /= np;
    // special cases
    if(lab == "Diris"){
      lab = "left_iris_radius";
      if(is_right)
        lab = "right_iris_radius";
      x*=0.5;y*=0.5;z*=0.5;
    }
    if(lab == "Nz"){
      lab = "left_Nz";
      if(is_right)
        lab = "right_Nz";
    }
    boxm2_vecf_labeled_point lp(x, y, z, lab);
    if(!this->add_labeled_point(lp))
      return false;
  }
  return true;
}
//
// code for reading a file generated from dlib image landmarks that
// are located using tree regression
//

// predicate for sorting on the x coordinate of a point
static bool pt_less(const vgl_point_2d<double>& pa, const vgl_point_2d<double>& pb)
{
  return pa.x() < pb.x();
}
// add a part of file type designation, dlabel, to the internal database with designation, olabel
// e.g. dlabel == left_lateral_canthus maps to olabel == left_eye_lateral_canthus
bool boxm2_vecf_fit_orbit::add_dlib_anchor_part(std::map<std::string, std::vector<vgl_point_2d<double> > > const& parts,
                                                std::string const& dlabel, std::string const& olabel)
{
  std::map<std::string, std::vector<vgl_point_2d<double> > >::const_iterator lit;
  lit = parts.find(dlabel);
  if(lit == parts.end())
    return false;
  // does the part bound the iris?
  std::size_t found=dlabel.find("circ");
  bool circ_pt = (found != std::string::npos);

  std::vector<vgl_point_2d<double> > pts = lit->second;
  vgl_point_2d<double> p;
  if((pts.size()==1)&&!circ_pt)
    p = pts[0];
  else if((pts.size() == 2)&& circ_pt){

    const vgl_point_2d<double>& p0 = pts[0];
    const vgl_point_2d<double>& p1 = pts[1];
        //compute iris radius
    double d = 0.5*std::sqrt((p0.x()-p1.x())*(p0.x()-p1.x()) + (p0.y()-p1.y())*(p0.y()-p1.y()));
    boxm2_vecf_labeled_point lap(d, d, d, olabel);
    if(!this->add_labeled_point(lap))
      return false;
    return true;
  }else if((pts.size() == 3)&&!circ_pt){
    // get middle point in x coordinate as the anchor point
    std::sort(pts.begin(), pts.end(), pt_less);
    p = pts[1];
  }else{
   std::cout << "wrong number of parts " <<  pts.size()<< "for " << dlabel << '\n';
    return false;
  }
  // initialize the z coordinate to 0
  boxm2_vecf_labeled_point lp(p.x(), p.y(), 0.0, olabel);
  if(!this->add_labeled_point(lp))
    return false;
  return true;
 }
//
// in the 3-d format there are separate files for the picked points on the margins and the crease
// this function enters dlib file points into the map for this contour data even though some points
// are in both sets. For example, the inferior margin point is also in the inferior margin orbit dataset.
// In the 3-d case, the two points are often slightly different, since they are picked separately,
// but in the dlib case they are identical.
//
bool boxm2_vecf_fit_orbit::add_dlib_orbit_data(std::map<std::string, std::vector<vgl_point_2d<double> > > const& parts,
                                               std::string const& dlabel, std::string const& olabel)
{
  auto sit = smid_map_.find(olabel);
  if(sit == smid_map_.end() ){
    std::cout << "data label " << olabel << " doesn't exist\n";
    return false;
  }
  std::map<std::string, std::vector<vgl_point_2d<double> > >::const_iterator lit;
  lit = parts.find(dlabel);
  if(lit == parts.end())
    return false;
  const std::vector<vgl_point_2d<double> >& pts = lit->second;
  for(auto pt : pts)
    orbit_data_[sit->second].push_back(vgl_point_3d<double>(pt.x(), pt.y(), 0.0));
  return true;
}

bool boxm2_vecf_fit_orbit::read_dlib_part_file(std::string const& path, bool image_data){
  from_image_data_ = image_data;
  std::ifstream istr(path.c_str());
  if(!istr.is_open()){
    std::cout << "dlib part file at" << path << " not found\n";
    return false;
  }
  only_2d_data_ = true;
  std::string lab;
   unsigned char c;
  if(from_image_data_)
    istr >> image_height_ >> c >> lab;
  if(lab != "image_height")
    return false;
  std::map<std::string, std::vector<vgl_point_2d<double> > > parts;
  bool is_right = false;
  double x, y;
  //this loop test may look strange but it is much more reliable than
  //testing istr.eof()
  while(istr >> x >> c){
    if(c!=',')
      return false;
    istr >> y >> c;
    if(c!=',')
      return false;
    istr >> lab;
    if(lab=="") continue;
    std::size_t found=lab.find("right");
    if (found!=std::string::npos)
      is_right = true;
    //
    // the image y coordinate is inverted as in a matrix index
    // so to convert to Cartesian head coordinates, image height is needed
    //
    vgl_point_2d<double> p(x,y);
    parts[lab].push_back(p);
  }
  // add anchor points and at the same time mapping from dlib labels to legacy labels
  if(is_right){
    if(!add_dlib_anchor_part(parts, "right_lateral_canthus", "right_eye_lateral_canthus"))
      return false;
    if(!add_dlib_anchor_part(parts, "right_medial_canthus", "right_eye_medial_canthus"))
      return false;
    if(!add_dlib_anchor_part(parts, "right_inferior_margin", "right_eye_inferior_margin"))
      return false;
    if(!add_dlib_anchor_part(parts, "right_superior_margin", "right_eye_superior_margin"))
      return false;
    if(!add_dlib_anchor_part(parts, "right_superior_crease", "right_eye_superior_crease"))
      return false;
    if(!add_dlib_anchor_part(parts, "right_iris_circ", "right_iris_radius"))
      return false;
  }else{
    if(!add_dlib_anchor_part(parts, "left_lateral_canthus", "left_eye_lateral_canthus"))
      return false;
    if(!add_dlib_anchor_part(parts, "left_medial_canthus", "left_eye_medial_canthus"))
      return false;
    if(!add_dlib_anchor_part(parts, "left_inferior_margin", "left_eye_inferior_margin"))
      return false;
    if(!add_dlib_anchor_part(parts, "left_superior_margin", "left_eye_superior_margin"))
      return false;
    if(!add_dlib_anchor_part(parts, "left_superior_crease", "left_eye_superior_crease"))
      return false;
    if(!add_dlib_anchor_part(parts, "left_iris_circ", "left_iris_radius"))
      return false;
  }
  //add orbit data
  if(is_right){
    if(!add_dlib_orbit_data(parts, "right_inferior_margin", "right_eye_inferior_margin"))
      return false;
    if(!add_dlib_orbit_data(parts, "right_superior_margin", "right_eye_superior_margin"))
      return false;
    if(!add_dlib_orbit_data(parts, "right_superior_crease", "right_eye_superior_crease"))
      return false;
  }else{
    if(!add_dlib_orbit_data(parts, "left_inferior_margin", "left_eye_inferior_margin"))
      return false;
    if(!add_dlib_orbit_data(parts, "left_superior_margin", "left_eye_superior_margin"))
      return false;
    if(!add_dlib_orbit_data(parts, "left_superior_crease", "left_eye_superior_crease"))
      return false;
  }
  return true;
}
//
// assumes iris_radius has been set from the dlib parts file
// it is observed that iris radius is fairly constant
// across individuals and can be used to provide an
// approximate mapping to mm coordinates, which eases
// interpretation of the model parameters.
//
void boxm2_vecf_fit_orbit::normalize_eye_data(float fixed_amount){
  double mm_per_pix;
  if(fixed_amount == 0.0f){
    double ril = left_params_.iris_radius_;
    double rir = right_params_.iris_radius_;
    double ria = (ril + rir)/2.0;
    double ari = left_params_.avg_iris_radius_;
    left_params_.iris_radius_ = ari;
    right_params_.iris_radius_ = ari;
    //conversion factor in millimeters per pixel
    mm_per_pix = ari/ria;
  }else
    mm_per_pix = fixed_amount;

  left_params_.mm_per_pix_ = mm_per_pix;
  right_params_.mm_per_pix_ = mm_per_pix;
  // for transforming to image coordinates
  left_params_.image_height_ = image_height_;
  right_params_.image_height_ = image_height_;
  // convert the database coordinates to mm
  for(auto & lpt : lpts_){
    vgl_point_3d<double>& p = lpt.second.p3d_;
    p.set(p.x()*mm_per_pix,(image_height_- p.y())*mm_per_pix, p.z()*mm_per_pix);
  }

  for(auto & dit : orbit_data_){
    std::vector<vgl_point_3d<double> >& pts = dit.second;
    for(auto & p : pts){
      p.set(p.x()*mm_per_pix, (image_height_-p.y())*mm_per_pix, p.z()*mm_per_pix);
    }
  }
}
bool boxm2_vecf_fit_orbit::left_eye_radius( double& rad){
  // find left eye medial canthus - corresponds to xmin
  std::map<mids, boxm2_vecf_labeled_point>::iterator lit;
  lit = lpts_.find(LEFT_EYE_MEDIAL_CANTHUS);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& le_mc  = lit->second.p3d_;
  lit = lpts_.find(LEFT_EYE_LATERAL_CANTHUS);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& le_lc  = lit->second.p3d_;
  double dx = le_lc.x()-le_mc.x();
  rad = std::fabs(dx/(left_params_.x_max_-left_params_.x_min_));
  return true;
}
//
// the socket radii are where the eyelid and crease curves start to flatten to planes
// on each side of the eye sphere, i.e., the planar surfaces of the orbit socket
//
bool boxm2_vecf_fit_orbit::left_eye_socket_radii_coefs( double& lateral_radius_coef, double& medial_radius_coef){
  //double r = left_params_.lid_sph_.radius();
  double r = left_params_.eyelid_radius();//JLM
  double rsq = r*r;
  std::map<mids, boxm2_vecf_labeled_point>::iterator lit;
  lit = lpts_.find(LEFT_EYE_LATERAL_CANTHUS);
  if(lit == lpts_.end())
    return false;
  vgl_point_3d<double>& le_lc  = lit->second.p3d_;
  lit = lpts_.find(LEFT_EYE_MEDIAL_CANTHUS);
  if(lit == lpts_.end())
    return false;
  vgl_point_3d<double>& le_mc  = lit->second.p3d_;

  // if only 2-d data is available then use nominal radius values
  // to set the z position of the socket planes
  if(only_2d_data_){
    // report nominal values
    lateral_radius_coef = left_params_.lateral_socket_radius_coef_;
    medial_radius_coef = left_params_.medial_socket_radius_coef_;

    double xlat = lateral_radius_coef;
    double arg = rsq*(1.0-xlat*xlat);
    if(arg<0.0){
      std::cout << "+++ Left Lat Fail ++++\n";
      return false;
    }
    // set the lateral canthus z coordinate
    // from the lid sphere surface
    double zlat = std::sqrt(arg);
    le_lc.set(le_lc.x(), le_lc.y(), zlat);

    double xmed = medial_radius_coef;
    arg = rsq*(1.0-xmed*xmed);
    if(arg<0.0){
      std::cout << "+++ Left Med Fail ++++\n";
      return false;
    }
    // set the medial canthus z coordinate
    // from the lid sphere surface
    double zmed = std::sqrt(arg);
    le_mc.set(le_mc.x(), le_mc.y(), zmed);
    return true;
  }
  // otherwise use the measured z coordinates of the
  // lateral and medial canthi to set the socket radii
  double zmed =  le_mc.z() - left_params_.z_trans();
  double xmed_sq = rsq - zmed*zmed;
  if(xmed_sq <0.0){
    std::cout << "+++ Left Med Fail ++++\n";
    medial_radius_coef = left_params_.medial_socket_radius_coef_;
  }else{
    double xmed = std::sqrt(xmed_sq);
    medial_radius_coef = xmed/r;
  }

  double zlat =  le_lc.z() - left_params_.z_trans();
  double xlat_sq = rsq - zlat*zlat;
  if(xlat_sq <0.0){
    lateral_radius_coef = left_params_.lateral_socket_radius_coef_;
    std::cout << "+++ Left Lat Fail ++++\n";
  }else{
    double xlat = std::sqrt(xlat_sq);
    lateral_radius_coef = xlat/r;
  }
  return true;
}

// used when inferior lid 3-d points are available to estimate the thickness of the
// inferior eyelid pouch
bool boxm2_vecf_fit_orbit::left_eye_inferior_lid_thickness(std::string const& data_desc, double& dr){
  if(only_2d_data_){
    dr = left_params_.inferior_lid_radius_offset_;
    return true;
  }
  auto iit = smid_map_.find(data_desc);
  if(iit == smid_map_.end() ){
    std::cout << "data label " << data_desc << " doesn't exist\n";
    return false;
  }
  if(!has_inferior_surface_pts_){
    dr = left_params_.eyelid_radius_offset_;
    return true;
  }
  const std::vector<vgl_point_3d<double> >& pts = orbit_data_[iit->second];
  if(!pts.size()){
    std::cout << "No data of type " << data_desc <<" in left_eye_inferior_lid_thickness" <<'\n';
    return false;
  }
  double x0 = left_params_.x_trans(), y0 = left_params_.y_trans()+left_params_.y_off_, z0 =left_params_.z_trans(), r = left_params_.eye_radius_;
  std::vector<double> dr_vals;
  for(const auto & pt : pts){
    double xi = pt.x(), yi = pt.y(), zi = pt.z();
    double rsqi =  (xi-x0)*(xi-x0) + (yi-y0)*(yi-y0) + (zi-z0)*(zi-z0);
    double ri = std::sqrt(rsqi);
    ri -= r;
    if(ri<0.0)
      ri = 0.0;
    dr_vals.push_back(ri);
  }
  if(dr_vals.size() == 0)
    return false;
  std::sort(dr_vals.begin(), dr_vals.end());
  // smallest 25% of dr values
  unsigned n_small = static_cast<unsigned>(dr_vals.size())/4;
  if(n_small==0)
    n_small = 1;
  double dr_mean = 0;
  for(unsigned i = 0; i<n_small; ++i)
    dr_mean += dr_vals[i];
  dr = dr_mean/static_cast<double>(n_small);
  return true;
}

bool boxm2_vecf_fit_orbit::set_left_iris_radius(){
  std::map<mids, boxm2_vecf_labeled_point>::iterator lit;
  lit = lpts_.find(LEFT_IRIS_RADIUS);
  if(lit == lpts_.end()){
    std::cout<<"left iris radius not found"<<std::endl;
    return false;
  }
  const vgl_point_3d<double>& l_iris_r  = lit->second.p3d_;
  left_params_.iris_radius_ = l_iris_r.x();
  return true;
}
bool boxm2_vecf_fit_orbit::left_trans_x_from_lateral_canthus(double& trx){
        std::map<mids, boxm2_vecf_labeled_point>::iterator lit;
  lit = lpts_.find(LEFT_EYE_LATERAL_CANTHUS);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& le_oc  = lit->second.p3d_;
  double xm = left_params_.eye_radius_*left_params_.x_max_;
  trx = (le_oc.x()-xm);
  return true;
}

bool boxm2_vecf_fit_orbit::left_trans_y_from_lateral_canthus( double& tr_y){
  std::map<mids, boxm2_vecf_labeled_point>::iterator lit;
  lit = lpts_.find(LEFT_EYE_LATERAL_CANTHUS);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& le_oc  = lit->second.p3d_;
  //tr_y = le_oc.y()-left_params_.y_off_;
  tr_y = le_oc.y();
  return true;
}
//
// uses picked sclera 3-d points to estimate the center of the eye sphere, given
// the estimated radius
//
bool boxm2_vecf_fit_orbit::left_trans_z_from_sclera(std::string const& data_desc, double& tr_z){
  if(only_2d_data_){
    tr_z = 0.0;
    return true;
  }

  auto iit = smid_map_.find(data_desc);
  if(iit == smid_map_.end() ){
    std::cout << "data label " << data_desc << " doesn't exist\n";
    return false;
  }
  const std::vector<vgl_point_3d<double> >& pts = orbit_data_[iit->second];
  if(!pts.size()){
    std::cout << "No data of type " << data_desc <<" in left trans_z_from_sclera" <<'\n';
    return false;
  }
  double x0 = left_params_.x_trans(), y0 = left_params_.y_trans(), r = left_params_.eye_radius_;
  double sum_z0 = 0.0, nz = 0.0, rsq = r*r;
  for(auto pit = pts.begin();
      pit != pts.end(); ++pit, nz+=1.0){
    double xi = pit->x(), yi = pit->y(), zi = pit->z();
    double sq = rsq - (xi-x0)*(xi-x0) - (yi-y0)*(yi-y0);
    if(sq<0.0)
      continue;
    sum_z0 += zi-std::sqrt(sq);
  }
  if(nz<=0.0)
    return false;
  tr_z = sum_z0/nz;
  return true;
}
bool boxm2_vecf_fit_orbit::left_eye_x_scale(double& left_x_scale){
  std::map<mids, boxm2_vecf_labeled_point>::iterator lit;
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
  left_x_scale = std::sqrt(temp*temp1);
  return true;
}

bool boxm2_vecf_fit_orbit::left_eye_y_scale(double& left_y_scale){
  std::map<mids, boxm2_vecf_labeled_point>::iterator lit;
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
  std::map<mids, boxm2_vecf_labeled_point>::iterator lit;
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
  std::map<mids, boxm2_vecf_labeled_point>::iterator lit;
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
  std::map<mids, boxm2_vecf_labeled_point>::iterator lit;
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

bool boxm2_vecf_fit_orbit::left_mid_inferior_margin_z(double& marg_z){
  std::map<mids, boxm2_vecf_labeled_point>::iterator lit;
  lit = lpts_.find(  LEFT_EYE_INFERIOR_MARGIN);
  if(lit == lpts_.end())
    return false;
  vgl_point_3d<double>& mid_inf_margin  = lit->second.p3d_;
  marg_z = mid_inf_margin.z()-left_params_.z_trans();
  if(only_2d_data_){
    vgl_point_3d<double> c = left_params_.lid_sph_.centre();
    double r = left_params_.lid_sph_.radius();
    double x = mid_inf_margin.x()-c.x(),  y = mid_inf_margin.y()-c.y();
    double arg = r*r - x*x - y*y;
    if(arg<0.0)
      return false;
    marg_z = std::sqrt(arg);
    mid_inf_margin.set(mid_inf_margin.x(), mid_inf_margin.y(), marg_z);
}
  return true;
}

bool boxm2_vecf_fit_orbit::left_mid_superior_margin_z(double& marg_z){
  std::map<mids, boxm2_vecf_labeled_point>::iterator lit;
  lit = lpts_.find(  LEFT_EYE_SUPERIOR_MARGIN);
  if(lit == lpts_.end())
    return false;
 vgl_point_3d<double>& mid_sup_margin  = lit->second.p3d_;
  marg_z = mid_sup_margin.z()-left_params_.z_trans();
  if(only_2d_data_){
    vgl_point_3d<double> c = left_params_.lid_sph_.centre();
    double r = left_params_.lid_sph_.radius();
    double x = mid_sup_margin.x()-c.x(),  y = mid_sup_margin.y()-c.y();
    double arg = r*r - x*x - y*y;
    if(arg<0.0)
      return false;
    marg_z = std::sqrt(arg);
    mid_sup_margin.set(mid_sup_margin.x(), mid_sup_margin.y(), marg_z);
  }
  return true;
}

bool boxm2_vecf_fit_orbit::left_mid_eyelid_crease_z(double& crease_z){
  std::map<mids, boxm2_vecf_labeled_point>::iterator lit;
  lit = lpts_.find(LEFT_EYE_SUPERIOR_CREASE);
  if(lit == lpts_.end())
    return false;
  vgl_point_3d<double>& mid_superior_crease  = lit->second.p3d_;
  crease_z = mid_superior_crease.z()-left_params_.z_trans();
  if(only_2d_data_){
    vgl_point_3d<double> c = left_params_.lid_sph_.centre();
    double r = left_params_.lid_sph_.radius();
    double x = mid_superior_crease.x()-c.x(), y = mid_superior_crease.y()-c.y();
    double arg = r*r - x*x - y*y;
    if(arg<0.0)
      return false;
    crease_z = std::sqrt(arg);
    mid_superior_crease.set(mid_superior_crease.x(), mid_superior_crease.y(), crease_z);
  }
  return true;
}

bool boxm2_vecf_fit_orbit::left_eye_superior_margin_crease_t(double& left_sup_crease_t){
  std::map<mids, boxm2_vecf_labeled_point>::iterator lit;
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
//
// approximate value of the canthus angle
// found to be inacurate for 3-d data due to
// sampling coarseness but used for initializing
// the non-linear orbit parameter fitting algorithm
bool boxm2_vecf_fit_orbit::left_ang_rad(double& ang_rad){
// find left eye inner cusp - corresponds to xmin
  std::map<mids, boxm2_vecf_labeled_point>::iterator lit;
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
  std::cout << "le_lc  " << le_lc << "\n le_mc " << le_mc << '\n';
  ang_rad = std::atan(dy/dx);
  std::cout << "DLIB LEFT CANTHUS ANGLE " << ang_rad*180.0/3.14159 << '\n';
  return true;
}
void  boxm2_vecf_fit_orbit::set_left_ang_rad(double& ang_rad){
  left_params_.dphi_rad_=   ang_rad - nominal_canthus_angle_rad_;
}

// assign z values to dlib points derived from images
bool boxm2_vecf_fit_orbit::set_left_z_values(){
  boxm2_vecf_eyelid sup_el(left_params_, true);
  boxm2_vecf_eyelid inf_el(left_params_,false);
  boxm2_vecf_eyelid_crease cre(left_params_);
  double tinf = left_params_.lower_eyelid_tmin_, tsup = left_params_.eyelid_tmin_;
  double tcre = left_params_.eyelid_crease_ct_;
  double xp = 0.0, zp =0.0;
  std::vector<vgl_point_3d<double> > pts = orbit_data_[LEFT_EYE_INFERIOR_MARGIN];
  if(!pts.size())
    return false;
  for(auto & pt : pts){
    xp = pt.x()-left_params_.x_trans();
    zp = inf_el.Z(xp, tinf);
    pt.set(pt.x(), pt.y(), zp);
  }
  orbit_data_[LEFT_EYE_INFERIOR_MARGIN] = pts;

  pts = orbit_data_[LEFT_EYE_SUPERIOR_MARGIN];
  if(!pts.size())
    return false;
  for(auto & pt : pts){
    xp = pt.x()-left_params_.x_trans();
    zp = sup_el.Z(xp, tsup);
    pt.set(pt.x(), pt.y(), zp);
  }
  orbit_data_[LEFT_EYE_SUPERIOR_MARGIN] = pts;

  pts = orbit_data_[LEFT_EYE_SUPERIOR_CREASE];
  if(!pts.size())
    return false;
  for(auto & pt : pts){
    xp = pt.x()-left_params_.x_trans();
    zp = cre.Z(xp, tcre);
    pt.set(pt.x(), pt.y(), zp);
  }
  orbit_data_[LEFT_EYE_SUPERIOR_CREASE] = pts;
  return true;
}

// find the largest sclera data z value, used to set the z origin of the eye sphere, given the radius
bool boxm2_vecf_fit_orbit::max_sclera_z(std::string const& data_desc, double r, double& max_z){
  auto iit = smid_map_.find(data_desc);
  if(iit == smid_map_.end() ){
    std::cout << "data label " << data_desc << " doesn't exist\n";
    return false;
  }
  if(only_2d_data_){
    max_z = r;
    return true;
  }
  std::vector<vgl_point_3d<double> >& pts = orbit_data_[iit->second];
  if(!pts.size()){
    std::cout << "No data of type " << data_desc << " :max_sclera_z called\n";
    return false;
  }
  max_z = -std::numeric_limits<double>::max();
  for(auto & pt : pts)
    if(pt.z()>max_z)
      max_z = pt.z();
  return true;
}
//
// use non-linear sphere fitting to set the eye sphere parameters,
// center and radius. Found to be too variable due to inaccuracy
// of the RIH scanner on the eye surface
//
bool boxm2_vecf_fit_orbit::fit_sclera(std::string const& data_desc){
  auto iit = smid_map_.find(data_desc);
  if(iit == smid_map_.end() ){
    std::cout << "data label " << data_desc << " doesn't exist\n";
    return false;
  }
  std::vector<vgl_point_3d<double> >& pts = orbit_data_[iit->second];
  if(!pts.size()){
    std::cout << "No data of type " << data_desc << " :fit_sclera called \n";
    return false;
  }
  vgl_fit_sphere_3d<double> fit_sph(pts);
  double err = fit_sph.fit(&std::cout, true);
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
void boxm2_vecf_fit_orbit::set_canthus_angle(bool is_right){
  boxm2_vecf_orbit_params params = left_params_;
  if(is_right)
    params = right_params_;
  double xm_min = params.x_min()-10.0;
  double xm_max = params.x_max()+10.0;
  std::vector<vgl_point_3d<double> > inf_pts, sup_pts;
  boxm2_vecf_plot_orbit::plot_inferior_margin(params, is_right, xm_min, xm_max, inf_pts);
  boxm2_vecf_plot_orbit::plot_superior_margin(params, is_right, xm_min, xm_max, sup_pts);
  int imin = -1, imax = -1;
  bool success = boxm2_vecf_plot_orbit::plot_limits(inf_pts, sup_pts, imin, imax);
  if(!success){
    std::cout << "margin limits failed \n";
    return;
  }
  double lat_canthus_x= 0.0, lat_canthus_y= 0.0;
  double med_canthus_x= 0.0, med_canthus_y= 0.0;
  if(is_right){
    lat_canthus_x = 0.5*(inf_pts[imin].x()+ sup_pts[imin].x());
    lat_canthus_y = 0.5*(inf_pts[imin].y()+ sup_pts[imin].y());
    med_canthus_x = 0.5*(inf_pts[imax].x()+ sup_pts[imax].x());
    med_canthus_y = 0.5*(inf_pts[imax].y()+ sup_pts[imax].y());
  }else{
    lat_canthus_x = 0.5*(inf_pts[imax].x()+ sup_pts[imax].x());
    lat_canthus_y = 0.5*(inf_pts[imax].y()+ sup_pts[imax].y());
    med_canthus_x = 0.5*(inf_pts[imin].x()+ sup_pts[imin].x());
    med_canthus_y = 0.5*(inf_pts[imin].y()+ sup_pts[imin].y());
  }
  // temporary print out
  double canthus_ang = std::atan((med_canthus_y-lat_canthus_y)/(med_canthus_x - lat_canthus_x));
  if(is_right)
    std::cout << "RIGHT CANTHUS ANGLE " << canthus_ang*180.0/3.14159 << '\n' << std::flush;
  else
    std::cout << "LEFT CANTHUS ANGLE " << canthus_ang*180.0/3.14159 << '\n' << std::flush;
}
bool boxm2_vecf_fit_orbit::right_eye_radius( double& rad){
  // find right eye inner cusp - corresponds to xmin
  std::map<mids, boxm2_vecf_labeled_point>::iterator lit;
  lit = lpts_.find(RIGHT_EYE_MEDIAL_CANTHUS);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& re_ic  = lit->second.p3d_;
  lit = lpts_.find(RIGHT_EYE_LATERAL_CANTHUS);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& re_oc  = lit->second.p3d_;
  double dx =(re_ic.x()-re_oc.x());
 rad = std::fabs(dx/(right_params_.x_max_-right_params_.x_min_));

  return true;
}
//
// the socket radii are where the eyelid and crease curves start to flatten to planes
// on each side of the eye sphere, i.e., the planar surfaces of the orbit socket
//
bool boxm2_vecf_fit_orbit::right_eye_socket_radii_coefs( double& lateral_radius_coef, double& medial_radius_coef){
  //double r = right_params_.lid_sph_.radius();
  double r = right_params_.eyelid_radius();
  double rsq = r*r;
  std::map<mids, boxm2_vecf_labeled_point>::iterator lit;
  lit = lpts_.find(RIGHT_EYE_LATERAL_CANTHUS);
  if(lit == lpts_.end())
    return false;
  vgl_point_3d<double>& re_lc  = lit->second.p3d_;

  lit = lpts_.find(RIGHT_EYE_MEDIAL_CANTHUS);
  if(lit == lpts_.end())
    return false;
  vgl_point_3d<double>& re_mc  = lit->second.p3d_;

  // if only 2-d data is available then use nominal radius values
  // to set the z position of the socket planes
  if(only_2d_data_){
    // report nominal values
    lateral_radius_coef = right_params_.lateral_socket_radius_coef_;
    medial_radius_coef = right_params_.medial_socket_radius_coef_;
    double xlat = lateral_radius_coef;
    double arg = rsq*(1.0-xlat*xlat);
    if(arg<0.0){
      std::cout << "+++ Right Lat Fail ++++\n";
      return false;
    }
    // set the lateral canthus z coordinate
    // from the lid sphere surface
    double zlat = std::sqrt(arg);
    re_lc.set(re_lc.x(), re_lc.y(), zlat);

    double xmed = medial_radius_coef;
    arg = rsq*(1.0-xmed*xmed);
    if(arg<0.0){
      std::cout << "+++ Right Med Fail ++++\n";
      return false;
    }
    // set the medial canthus z coordinate
    // from the lid sphere surface
    double zmed = std::sqrt(arg);
    re_mc.set(re_mc.x(), re_mc.y(), zmed);
    return true;
  }
  // otherwise use the measured z coordinates of the
  // lateral and medial canthi to set the socket radii
  double zmed =  re_mc.z() - right_params_.z_trans();
  double xmed_sq = rsq - zmed*zmed;
  if(xmed_sq <0.0){
    std::cout << "+++ Right Med Fail ++++\n";
    return false;
  }else
    medial_radius_coef = std::sqrt(xmed_sq)/r;

  double zlat =  re_lc.z() - right_params_.z_trans();
  double xlat_sq = rsq - zlat*zlat;
  if(xlat_sq <0.0){
    std::cout << "+++ Right Lat Fail ++++\n";
    return false;
  }else
    lateral_radius_coef = std::sqrt(xlat_sq)/r;
  return true;
}

// used when inferior lid 3-d points are available to estimate the thickness of the
// inferior eyelid pouch
bool boxm2_vecf_fit_orbit::right_eye_inferior_lid_thickness(std::string const& data_desc, double& dr){
  if(only_2d_data_){
    dr = right_params_.inferior_lid_radius_offset_;
    return true;
  }
  auto iit = smid_map_.find(data_desc);
  if(iit == smid_map_.end() ){
    std::cout << "data label " << data_desc << " doesn't exist\n";
    return false;
  }
  const std::vector<vgl_point_3d<double> >& pts = orbit_data_[iit->second];
  if(!pts.size()){
    std::cout << "No data of type " << data_desc <<" in right eye inferior lid thickness" <<'\n';
    return false;
  }
  if(!has_inferior_surface_pts_){
    dr = right_params_.eyelid_radius_offset_;
    return true;
  }
  double x0 = right_params_.x_trans(), y0 = right_params_.y_trans()+right_params_.y_off_, z0 =right_params_.z_trans(), r = right_params_.eye_radius_;
  std::vector<double> dr_vals;
  for(const auto & pt : pts){
    double xi = pt.x(), yi = pt.y(), zi = pt.z();
    double rsqi =  (xi-x0)*(xi-x0) + (yi-y0)*(yi-y0) + (zi-z0)*(zi-z0);
    double ri = std::sqrt(rsqi);
    ri -= r;
    if(ri<0.0)
      ri = 0.0;
    dr_vals.push_back(ri);
  }
  if(dr_vals.size() == 0)
    return false;
  std::sort(dr_vals.begin(), dr_vals.end());
  // smallest 25% of dr values
  unsigned n_small = static_cast<unsigned>(dr_vals.size())/4;
  if(n_small==0)
    n_small = 1;
  double dr_mean = 0;
  for(unsigned i = 0; i<n_small; ++i)
    dr_mean += dr_vals[i];
  dr = dr_mean/static_cast<double>(n_small);
  return true;
}


bool boxm2_vecf_fit_orbit::set_right_iris_radius(){
  std::map<mids, boxm2_vecf_labeled_point>::iterator lit;
  lit = lpts_.find(RIGHT_IRIS_RADIUS);
  if(lit == lpts_.end()){
    std::cout<<"right iris radius not found"<<std::endl;
    return false;
  }
  const vgl_point_3d<double>& l_iris_r  = lit->second.p3d_;
  right_params_.iris_radius_ = l_iris_r.x();
  return true;
}

bool boxm2_vecf_fit_orbit::right_trans_y_from_lateral_canthus( double& tr_y){
  std::map<mids, boxm2_vecf_labeled_point>::iterator lit;
  lit = lpts_.find(RIGHT_EYE_LATERAL_CANTHUS);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& re_oc  = lit->second.p3d_;
  //tr_y = re_oc.y()-right_params_.y_off_;
  tr_y = re_oc.y();
  return true;
}

bool boxm2_vecf_fit_orbit::right_trans_x_from_lateral_canthus(double& trx){
        std::map<mids, boxm2_vecf_labeled_point>::iterator lit;
  lit = lpts_.find(RIGHT_EYE_LATERAL_CANTHUS);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& re_oc  = lit->second.p3d_;
  double xm = -right_params_.eye_radius_*right_params_.x_max_;
  trx =(re_oc.x()-xm);
  return true;
}

//
// uses picked sclera 3-d points to estimate the center of the eye sphere, given
// the estimated radius
//
bool boxm2_vecf_fit_orbit::right_trans_z_from_sclera(std::string const& data_desc, double& tr_z){
  if(only_2d_data_){
    tr_z = 0.0;
    return true;
  }
  auto iit = smid_map_.find(data_desc);
  if(iit == smid_map_.end() ){
    std::cout << "data label " << data_desc << " doesn't exist\n";
    return false;
  }
  const std::vector<vgl_point_3d<double> >& pts = orbit_data_[iit->second];
  if(!pts.size()){
    std::cout << "No data of type " << data_desc <<" in right_trans_z_from_sclera "<< '\n';
    return false;
  }
  double x0 = right_params_.x_trans(), y0 = right_params_.y_trans(), r = right_params_.eye_radius_;
  double sum_z0 = 0.0, nz = 0.0, rsq = r*r;
  for(auto pit = pts.begin();
      pit != pts.end(); ++pit, nz+=1.0){
    double xi = pit->x(), yi = pit->y(), zi = pit->z();
    double sq = rsq - (xi-x0)*(xi-x0) - (yi-y0)*(yi-y0);
    if(sq<0.0)
      continue;
    sum_z0 += zi-std::sqrt(sq);
  }
  if(nz<=0.0)
    return false;
  tr_z = sum_z0/nz;
  return true;
}
bool boxm2_vecf_fit_orbit::right_eye_x_scale(double& right_x_scale){
  std::map<mids, boxm2_vecf_labeled_point>::iterator lit;
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
  right_x_scale = std::fabs((xmx-xmi)/(xoc-xic));
  return true;
}


bool boxm2_vecf_fit_orbit::right_eye_y_scale(double& right_y_scale){
  std::map<mids, boxm2_vecf_labeled_point>::iterator lit;
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
  std::map<mids, boxm2_vecf_labeled_point>::iterator lit;
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
  std::map<mids, boxm2_vecf_labeled_point>::iterator lit;
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
  std::map<mids, boxm2_vecf_labeled_point>::iterator lit;
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

bool boxm2_vecf_fit_orbit::right_mid_inferior_margin_z(double& marg_z){
  std::map<mids, boxm2_vecf_labeled_point>::iterator lit;
  lit = lpts_.find(  RIGHT_EYE_INFERIOR_MARGIN);
  if(lit == lpts_.end())
    return false;
  vgl_point_3d<double>& mid_inf_margin  = lit->second.p3d_;
  marg_z = mid_inf_margin.z()-right_params_.z_trans();
  if(only_2d_data_){
    vgl_point_3d<double> c = right_params_.lid_sph_.centre();
    double r = right_params_.lid_sph_.radius();
    double x = mid_inf_margin.x()-c.x(),  y = mid_inf_margin.y()-c.y();
    double arg = r*r - x*x - y*y;
    if(arg<0.0)
      return false;
    marg_z = std::sqrt(arg);
    mid_inf_margin.set(mid_inf_margin.x(), mid_inf_margin.y(), marg_z);
  }
  return true;
}
bool boxm2_vecf_fit_orbit::right_mid_superior_margin_z(double& marg_z){
  std::map<mids, boxm2_vecf_labeled_point>::iterator lit;
  lit = lpts_.find(  RIGHT_EYE_SUPERIOR_MARGIN);
  if(lit == lpts_.end())
    return false;
  vgl_point_3d<double>& mid_sup_margin  = lit->second.p3d_;
  marg_z = mid_sup_margin.z() -right_params_.z_trans();
  if(only_2d_data_){
    vgl_point_3d<double> c = right_params_.lid_sph_.centre();
    double r = right_params_.lid_sph_.radius();
    double x = mid_sup_margin.x()-c.x(), y = mid_sup_margin.y()-c.y();
    double arg = r*r - x*x - y*y;
    if(arg<0.0)
      return false;
    marg_z = std::sqrt(arg);
    mid_sup_margin.set(mid_sup_margin.x(), mid_sup_margin.y(), marg_z);
  }
  return true;
}

bool boxm2_vecf_fit_orbit::right_mid_eyelid_crease_z(double& crease_z){
  std::map<mids, boxm2_vecf_labeled_point>::iterator lit;
  lit = lpts_.find(RIGHT_EYE_SUPERIOR_CREASE);
  if(lit == lpts_.end())
    return false;
  vgl_point_3d<double>& mid_superior_crease  = lit->second.p3d_;
  crease_z = mid_superior_crease.z()-right_params_.z_trans();
  if(only_2d_data_){
    vgl_point_3d<double> c = right_params_.lid_sph_.centre();
    double r = right_params_.lid_sph_.radius();
    double x = mid_superior_crease.x()-c.x(),  y = mid_superior_crease.y()-c.y();
    double arg = r*r - x*x - y*y;
    if(arg<0.0)
      return false;
    crease_z = std::sqrt(arg);
    mid_superior_crease.set(mid_superior_crease.x(), mid_superior_crease.y(), crease_z);
  }
  return true;
}

bool boxm2_vecf_fit_orbit::right_eye_superior_margin_crease_t(double& right_sup_crease_t){
  std::map<mids, boxm2_vecf_labeled_point>::iterator lit;
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

//
// approximate value of the canthus angle
// found to be inacurate for 3-d data due to
// sampling coarseness but used for initializing
// the non-linear orbit parameter fitting algorithm
bool boxm2_vecf_fit_orbit::right_ang_rad(double& ang_rad){
// find right eye inner cusp - corresponds to xmin
  std::map<mids, boxm2_vecf_labeled_point>::iterator lit;
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
  ang_rad = std::atan(dy/dx);
   std::cout << "re_lc  " << re_lc << "\n re_mc " << re_mc << '\n';
  std::cout << "DLIB RIGHT CANTHUS ANGLE " << ang_rad*180.0/3.14159 << '\n';
  return true;
}
void  boxm2_vecf_fit_orbit::set_right_ang_rad(double& ang_rad){

  right_params_.dphi_rad_= -(nominal_canthus_angle_rad_+ang_rad);
}

// assign z values to dlib points derived from images
bool boxm2_vecf_fit_orbit::set_right_z_values(){
  boxm2_vecf_eyelid sup_el(left_params_, true);
  boxm2_vecf_eyelid inf_el(left_params_, false);
  boxm2_vecf_eyelid_crease cre(right_params_);
  double tinf = right_params_.lower_eyelid_tmin_, tsup = right_params_.eyelid_tmin_;
  double tcre = right_params_.eyelid_crease_ct_;
  double xp = 0.0, zp =0.0;
  std::vector<vgl_point_3d<double> > pts = orbit_data_[RIGHT_EYE_INFERIOR_MARGIN];
  if(!pts.size())
    return false;
  for(auto & pt : pts){
    xp = pt.x()-right_params_.x_trans();
    zp = inf_el.Z(-xp, tinf);
    pt.set(pt.x(), pt.y(), zp);
  }
  orbit_data_[RIGHT_EYE_INFERIOR_MARGIN] = pts;

  pts = orbit_data_[RIGHT_EYE_SUPERIOR_MARGIN];
  if(!pts.size())
    return false;
  for(auto & pt : pts){
    xp = pt.x()-right_params_.x_trans();
    zp = sup_el.Z(-xp, tsup);
    pt.set(pt.x(), pt.y(), zp);
  }
  orbit_data_[RIGHT_EYE_SUPERIOR_MARGIN] = pts;

  pts = orbit_data_[RIGHT_EYE_SUPERIOR_CREASE];
  if(!pts.size())
    return false;
  for(auto & pt : pts){
        xp = pt.x()-right_params_.x_trans();
    zp = cre.Z(-xp, tcre);
    pt.set(pt.x(), pt.y(), zp);
  }
  orbit_data_[RIGHT_EYE_SUPERIOR_CREASE] = pts;
  return true;
}

// run through the data and compute model parameter values for the left orbit
bool boxm2_vecf_fit_orbit::fit_left(){
  bool good = false;

  double ang_rad = 0.0, d_trx = 0.0, trx = 0.0, d_try = 0.0, tr_y = 0.0, trz = 0.0, eye_rad = 1.0;

  good =  left_ang_rad(ang_rad);
  if(!good)
    return false;
  this->set_left_ang_rad(ang_rad);

// the fitted sphere radius appears to be more accurate than from the cusp distances
// also the x origin seems more accurate when estimated from the sphere center than cusps
#if !USE_SPHERE
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
  d_try = rlat*std::sin(left_params_.dphi_rad_);

  trx -= d_trx;

#else
  good = this->fit_sclera("left_eye_sclera");
  if(!good)
    return false;
  this->set_left_eye_radius(left_params_.sph_.radius());
  trx = left_params_.sph_.centre().x();
  trz = left_params_.sph_.centre().z();
#endif

  this->set_left_trans_x(trx);
  //this->set_left_trans_z(trz);
  // setting trans y from the sphere center seems not as accurate
  // as from the lateral canthus
  good = this->left_trans_y_from_lateral_canthus(tr_y);
  if(!good)
    return false;
  tr_y -= d_try;
  this->set_left_trans_y(tr_y);
  good = this->left_trans_z_from_sclera("left_eye_sclera", trz);
  if(!good)
    return false;
  this->set_left_trans_z(trz);
#if !USE_SPHERE
  left_params_.init_sphere();
#endif
  double lat_rad_coef = 0.0, med_rad_coef = 0.0;
  good =  left_eye_socket_radii_coefs(lat_rad_coef, med_rad_coef);
  if(!good)
    return false;
  this->set_left_eye_socket_radii_coefs(lat_rad_coef, med_rad_coef);
if(!only_2d_data_){
  double lth = 0.0;
  good = left_eye_inferior_lid_thickness("left_eye_inferior_lid_surface",lth);
  if(!good)
    return false;
  this->set_left_eye_inferior_lid_thickness(lth);
}
  double left_x_scale;
  good = this->left_eye_x_scale(left_x_scale);
  if(!good)
    return false;
  this->set_left_eye_x_scale(left_x_scale);

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
  double inf_margin_z;
  good = left_mid_inferior_margin_z(inf_margin_z);
  if(!good)
    return false;
  this->set_left_inferior_margin_z(inf_margin_z);

  double sup_margin_z;
  good = left_mid_superior_margin_z(sup_margin_z);
  if(!good)
    return false;
  this->set_left_superior_margin_z(sup_margin_z);

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

// run through the data and compute model parameter values for the right orbit
bool boxm2_vecf_fit_orbit::fit_right(){
  bool good = false;

  double ang_rad = 0.0, d_trx = 0.0, trx = 0.0, d_try = 0.0, tr_y = 0.0, trz = 0.0, eye_rad;
  good =  right_ang_rad(ang_rad);
  if(!good)
    return false;
  this->set_right_ang_rad(ang_rad);


#if !USE_SPHERE
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
  d_try = rlat*std::sin(right_params_.dphi_rad_);

  trx += d_trx;

#else
  good = this->fit_sclera("right_eye_sclera");
  if(!good)
    return false;
  this->set_right_eye_radius(right_params_.sph_.radius());
  trx = right_params_.sph_.centre().x();
  trz = right_params_.sph_.centre().z();
#endif

  this->set_right_trans_x(trx);
 // this->set_right_trans_z(trz);
  // setting trans y from the sphere center seems not as accurate
  // as from the outer cusp.
  good = this->right_trans_y_from_lateral_canthus(tr_y);
  if(!good)
    return false;
  tr_y -= d_try;
  this->set_right_trans_y(tr_y);
  good = this->right_trans_z_from_sclera("right_eye_sclera", trz);
  if(!good)
    return false;
   this->set_right_trans_z(trz);
#if !USE_SPHERE
  right_params_.init_sphere();
#endif
  double lat_rad_coef = 0.0, med_rad_coef = 0.0;
  good =  right_eye_socket_radii_coefs(lat_rad_coef, med_rad_coef);
  if(!good)
    return false;
  this->set_right_eye_socket_radii_coefs(lat_rad_coef, med_rad_coef);

  double rth = 0.0;
  good = right_eye_inferior_lid_thickness("right_eye_inferior_lid_surface",rth);
  if(!good)
    return false;
  this->set_right_eye_inferior_lid_thickness(rth);


  double right_x_scale;
  good = this->right_eye_x_scale(right_x_scale);
  if(!good)
    return false;
  this->set_right_eye_x_scale(right_x_scale);


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
  double r_inf_margin_z;
  good = right_mid_inferior_margin_z(r_inf_margin_z);
  if(!good)
    return false;
  this->set_right_inferior_margin_z(r_inf_margin_z);

  double r_sup_margin_z;
  good = right_mid_superior_margin_z(r_sup_margin_z);
  if(!good)
    return false;
  this->set_right_superior_margin_z(r_sup_margin_z);

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
bool boxm2_vecf_fit_orbit::load_orbit_data(std::string const& data_desc, std::string const& path, bool error_msg){
  auto iit = smid_map_.find(data_desc);
  if(iit == smid_map_.end() ){
    if(error_msg)
      std::cout << "data label " << data_desc << " doesn't exist\n";
    return false;
  }
  std::ifstream istr(path.c_str());
  if(!istr.is_open()){
    if(error_msg)
      std::cout << "data file for type "<< data_desc << " at " << path << " not found\n";
    return false;
  }
  // this loop termination may look strange
  // but testing the stream is more reliable
  // then the state of istr.eof()
   double x, y, z;
  unsigned char c;
  while(istr >> x >> c){
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

//
// plot the picked point data
//
bool boxm2_vecf_fit_orbit::plot_orbit_data(std::string const& data_desc, std::vector<vgl_point_3d<double> > & data){
  auto iit = smid_map_.find(data_desc);
  if(iit == smid_map_.end() ){
    std::cout << "data label " << data_desc << " doesn't exist\n";
    return false;
  }
  std::vector<vgl_point_3d<double> >& pts = orbit_data_[iit->second];
  if(!pts.size()){
    std::cout << "No data of type " << data_desc << '\n';
    return false;
  }
  bool is_right = iit->second > LEFT_IRIS_RADIUS;
  boxm2_vecf_orbit_params params = left_params_;
  if(is_right)
    params = right_params_;

  for(auto & p : pts){
    double xm = p.x()-params.x_trans();
    double y = p.y()-params.trans_y_;
    double z = p.z()-params.trans_z_;
    vgl_point_3d<double> pd(xm, y, z);
    data.push_back(pd);
  }
  return true;
}

//
// input spheres for the anchor points
//
bool boxm2_vecf_fit_orbit::display_anchors(std::ofstream& ostr, bool is_right){
  if(!ostr)
    return false;
  std::map<mids, boxm2_vecf_labeled_point>::iterator lit;
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
//
// display the input data for the orbit and optionally the model estimates for the inferior, superior margins
// and superior crease
//
bool boxm2_vecf_fit_orbit::display_orbit_vrml(std::ofstream& ostr, bool is_right, bool show_model){

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
  auto cx = static_cast<float>(c.x());
  auto cy = static_cast<float>(c.y());
  auto cz = static_cast<float>(c.z());
  auto r = static_cast<float>(sphere.radius());
  vgl_sphere_3d<float> fsphere(cx, cy, cz, r);
  bvrml_write::write_vrml_sphere(ostr, fsphere, 1.0f, 1.0f, 1.0f, 0.2f);
  // write the iris
  auto irad = static_cast<float>(params.iris_radius_);
  auto prad = static_cast<float>(params.pupil_radius_);
  bvrml_write::write_vrml_cylinder(ostr, vgl_point_3d<double>(0.0, cy, r-0.3),  vgl_vector_3d<double>(0.0, 0.0, 1.0), irad, 0.5f, 0.0f, 0.5f, 0.75f);
  bvrml_write::write_vrml_cylinder(ostr, vgl_point_3d<double>(0.0, cy, r+0.25), vgl_vector_3d<double>(0.0, 0.0, 1.0), prad, 0.1f, 0.0f, 0.0f, 0.0f);

  // write the sclera points
  std::vector<vgl_point_3d<double> > pts = orbit_data_[LEFT_SCLERA];
  if(is_right)
     pts = orbit_data_[RIGHT_SCLERA];
  for(auto & p : pts){
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
  for(auto & pt : pts){
    vgl_point_3d<double> p = pt-v;
    vgl_point_3d<float> pf(static_cast<float>(p.x()), static_cast<float>(p.y()), static_cast<float>(p.z()));
    vgl_sphere_3d<float> sp(pf, r*0.025f);
    bvrml_write::write_vrml_sphere(ostr, sp, 1.0f, 1.0f, 0.0f);
  }
  if(show_model){
  std::vector<vgl_point_3d<double> > inf_marg_pts;
  boxm2_vecf_plot_orbit::plot_inferior_margin(params, is_right, xm_min, xm_max, inf_marg_pts);
  for(auto p : inf_marg_pts){
    vgl_point_3d<float> pf(static_cast<float>(p.x()), static_cast<float>(p.y()), static_cast<float>(p.z()));
    vgl_sphere_3d<float> sp(pf, r*0.025f);
    bvrml_write::write_vrml_sphere(ostr, sp, 0.75f, 0.75f, 0.0f);
  }
  }
  pts.clear();
  // display the inferior surface
  pts = orbit_data_[LEFT_INFERIOR_LID_SURFACE];
  if(is_right)
    pts = orbit_data_[RIGHT_INFERIOR_LID_SURFACE];
  double dx = -2.0;
  if(is_right) dx = 2.0;
  vgl_vector_3d<double> vv(v.x()+dx, v.y(), v.z());
   for(auto & pt : pts){
    vgl_point_3d<double> p = pt-vv;
    vgl_point_3d<float> pf(static_cast<float>(p.x()), static_cast<float>(p.y()), static_cast<float>(p.z()));
    vgl_sphere_3d<float> sp(pf, r*0.025f);
    bvrml_write::write_vrml_sphere(ostr, sp, 1.0f, 0.75f, 0.5f);
  }
  pts.clear();
  // display the superior margin
  pts = orbit_data_[LEFT_EYE_SUPERIOR_MARGIN];
  if(is_right){
    pts = orbit_data_[RIGHT_EYE_SUPERIOR_MARGIN];
  }
  for(auto & pt : pts){
    vgl_point_3d<double> p = pt-v;
    vgl_point_3d<float> pf(static_cast<float>(p.x()), static_cast<float>(p.y()), static_cast<float>(p.z()));
    vgl_sphere_3d<float> sp(pf, r*0.025f);
    bvrml_write::write_vrml_sphere(ostr, sp, 1.0f, 0.0f, 0.0f);
  }
  if(show_model){
  std::vector<vgl_point_3d<double> > sup_marg_pts;
  boxm2_vecf_plot_orbit::plot_superior_margin(params, is_right, xm_min, xm_max, sup_marg_pts);
  for(auto p : sup_marg_pts){
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
  for(auto & pt : pts){
    vgl_point_3d<double> p = pt-v;
    vgl_point_3d<float> pf(static_cast<float>(p.x()), static_cast<float>(p.y()), static_cast<float>(p.z()));
    vgl_sphere_3d<float> sp(pf, r*0.025f);
    bvrml_write::write_vrml_sphere(ostr, sp, 0.0f, 1.0f, 1.0f);
  }
  if(show_model){
  std::vector<vgl_point_3d<double> > sup_crease_pts;
  boxm2_vecf_plot_orbit::plot_crease(params, is_right, xm_min, xm_max, sup_crease_pts);
  for(auto p : sup_crease_pts){
    vgl_point_3d<float> pf(static_cast<float>(p.x()), static_cast<float>(p.y()), static_cast<float>(p.z()));
    vgl_sphere_3d<float> sp(pf, r*0.025f);
    bvrml_write::write_vrml_sphere(ostr, sp, 0.0f, 0.75f, 0.75f);
  }
  }
  this->display_anchors(ostr, is_right);
  ostr.close();
  return true;
}

//compare the fitted orbit model for the left and right eye
bool boxm2_vecf_fit_orbit::display_left_right_orbit_model_vrml(std::ofstream& os){
  if(!os)
    return false;
  bvrml_write::write_vrml_header(os);
  boxm2_vecf_orbit_params& lp = left_params_;
  boxm2_vecf_orbit_params& rp = right_params_;

  // plot left eye
  double xm_min_left = lp.x_min()-10.0;
  double xm_max_left = lp.x_max()+10.0;
  vgl_vector_3d<double> vl(0.0, 0.0, lp.eyelid_radius());
  std::vector<vgl_point_3d<double> > left_inf_pts, left_sup_pts, left_crease_pts;
  boxm2_vecf_plot_orbit::plot_inferior_margin(lp, false, xm_min_left, xm_max_left, left_inf_pts);
  boxm2_vecf_plot_orbit::plot_superior_margin(lp, false, xm_min_left, xm_max_left, left_sup_pts);
  boxm2_vecf_plot_orbit::plot_crease(lp, false, xm_min_left, xm_max_left, left_crease_pts);
  int left_imin = -1, left_imax = -1;
  bool success = boxm2_vecf_plot_orbit::plot_limits(left_inf_pts, left_sup_pts, left_imin, left_imax);
  if(!success){
    std::cout << "Find left plot limits failed\n";
    return false;
  }
  // plot right eye
  double xm_min_right = rp.x_min()-10.0;
  double xm_max_right = rp.x_max()+10.0;
  vgl_vector_3d<double> vr(0.0, 0.0, rp.eyelid_radius());
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
  float r = 0.5f;
  vgl_point_3d<double> pd;
  vgl_point_3d<float> pf;

  // write vrml left orbit
  for(int i = left_imin; i<=left_imax; ++i){
     pd = left_inf_pts[i]-vl;
     pf.set(static_cast<float>(pd.x()),
            static_cast<float>(pd.y()),
            static_cast<float>(pd.z()));
    vgl_sphere_3d<float> spi(pf, r);
    bvrml_write::write_vrml_sphere(os, spi, 1.0f, 1.0f, 0.0f);
    pd = left_sup_pts[i]-vl;
    pf.set(static_cast<float>(pd.x()),
           static_cast<float>(pd.y()),
           static_cast<float>(pd.z()));
    vgl_sphere_3d<float> sps(pf, r);
    bvrml_write::write_vrml_sphere(os, sps, 1.0f, 0.0f, 0.0f);
    pd = left_crease_pts[i]-vl;
    pf.set(static_cast<float>(pd.x()),
           static_cast<float>(pd.y()),
           static_cast<float>(pd.z()));
    vgl_sphere_3d<float> spc(pf, r);
    bvrml_write::write_vrml_sphere(os, spc, 0.0f, 1.0f, 1.0f);
  }
  // write vrml right orbit
  for(int i = right_imin; i<=right_imax; ++i){
     pd = right_inf_pts[i]-vr;
     pf.set(static_cast<float>(pd.x()),
            static_cast<float>(pd.y()),
            static_cast<float>(pd.z()));
    vgl_sphere_3d<float> spi(pf, r);
    bvrml_write::write_vrml_sphere(os, spi, 1.0f, 1.0f, 0.5f);
    pd = right_sup_pts[i]-vr;
    pf.set(static_cast<float>(pd.x()),
           static_cast<float>(pd.y()),
           static_cast<float>(pd.z()));
    vgl_sphere_3d<float> sps(pf, r);
    bvrml_write::write_vrml_sphere(os, sps, 1.0f, 0.5f, 0.5f);
    pd = right_crease_pts[i]-vr;
    pf.set(static_cast<float>(pd.x()),
           static_cast<float>(pd.y()),
           static_cast<float>(pd.z()));
    vgl_sphere_3d<float> spc(pf, r);
    bvrml_write::write_vrml_sphere(os, spc, 0.5f, 1.0f, 1.0f);
  }
  return true;
}
//
// compute the error between the measured picked points and
// the corresponding model values
//
bool boxm2_vecf_fit_orbit::fitting_error(std::string const& data_desc){
  auto iit = smid_map_.find(data_desc);
  if(iit == smid_map_.end() ){
    std::cout << "Measurement label " << data_desc << " doesn't exist\n";
    return false;
  }
  std::vector<vgl_point_3d<double> > pts;
  vgl_vector_3d<double> vl(left_params_.x_trans(),
                            left_params_.y_trans(),
                            left_params_.z_trans());
  vgl_vector_3d<double> vr(right_params_.x_trans(),
                           right_params_.y_trans(),
                           right_params_.z_trans());
  switch(iit->second){
  case LEFT_EYE_INFERIOR_MARGIN:{
    pts = orbit_data_[LEFT_EYE_INFERIOR_MARGIN];
    auto n = static_cast<unsigned>(pts.size());
    if(n==0){
      std::cout << "no inferior margin points\n";
      return false;
    }
    boxm2_vecf_eyelid el(left_params_);
    double t = left_params_.lower_eyelid_tmin_;
    double dsum=0;
    for(unsigned i = 0; i<n; ++i)
      dsum += el.curve_distance(t, (pts[i]-vl));
    left_params_.inferior_margin_xyz_error_ = dsum/static_cast<double>(n);
    dsum=0;
    for(unsigned i = 0; i<n; ++i)
      dsum += el.curve_distance(t, (pts[i]-vl).x(), (pts[i]-vl).y());
    left_params_.inferior_margin_xy_error_ = dsum/static_cast<double>(n);
    break;
  }
  case LEFT_EYE_SUPERIOR_MARGIN:{
    pts = orbit_data_[LEFT_EYE_SUPERIOR_MARGIN];
    auto n = static_cast<unsigned>(pts.size());
    if(n==0){
      std::cout << "no superior margin points\n";
      return false;
    }
    boxm2_vecf_eyelid el(left_params_);
    double t = left_params_.eyelid_tmin_;
    double dsum=0;
    for(unsigned i = 0; i<n; ++i)
      dsum += el.curve_distance(t, (pts[i]-vl));
    left_params_.superior_margin_xyz_error_ = dsum/static_cast<double>(n);
    dsum=0;
    for(unsigned i = 0; i<n; ++i)
      dsum += el.curve_distance(t, (pts[i]-vl).x(), (pts[i]-vl).y());
    left_params_.superior_margin_xy_error_ = dsum/static_cast<double>(n);
    break;
  }
  case LEFT_EYE_SUPERIOR_CREASE:{
    pts = orbit_data_[LEFT_EYE_SUPERIOR_CREASE];
    auto n = static_cast<unsigned>(pts.size());
    if(n==0){
      std::cout << "no superior crease points\n";
      return false;
    }
    boxm2_vecf_eyelid_crease el(left_params_);
    double t = left_params_.eyelid_crease_ct_;
    double dsum=0;
    for(unsigned i = 0; i<n; ++i)
      dsum += el.curve_distance(t, (pts[i]-vl));
    left_params_.superior_crease_xyz_error_ = dsum/static_cast<double>(n);
    dsum=0;
    for(unsigned i = 0; i<n; ++i)
      dsum += el.curve_distance(t, (pts[i]-vl).x(), (pts[i]-vl).y());
    left_params_.superior_crease_xy_error_ = dsum/static_cast<double>(n);
    break;
  }
  case RIGHT_EYE_INFERIOR_MARGIN:{
    pts = orbit_data_[RIGHT_EYE_INFERIOR_MARGIN];
    auto n = static_cast<unsigned>(pts.size());
    if(n==0){
      std::cout << "no inferior margin points\n";
      return false;
    }
    boxm2_vecf_eyelid el(right_params_);
    double t = right_params_.lower_eyelid_tmin_;
    double dsum=0;
    for(unsigned i = 0; i<n; ++i){
      vgl_point_3d<double> pd = pts[i]-vr;
      dsum += el.curve_distance(t, -pd.x(), pd.y(), pd.z());
    }
    right_params_.inferior_margin_xyz_error_ = dsum/static_cast<double>(n);
    dsum=0;
    for(unsigned i = 0; i<n; ++i)
      dsum += el.curve_distance(t, -(pts[i]-vr).x(), (pts[i]-vr).y());
    right_params_.inferior_margin_xy_error_ = dsum/static_cast<double>(n);
    break;
  }
  case RIGHT_EYE_SUPERIOR_MARGIN:{
    pts = orbit_data_[RIGHT_EYE_SUPERIOR_MARGIN];
    auto n = static_cast<unsigned>(pts.size());
    if(n==0){
      std::cout << "no superior margin points\n";
      return false;
    }
    boxm2_vecf_eyelid el(right_params_);
    double t = right_params_.eyelid_tmin_;
    double dsum=0;
    for(unsigned i = 0; i<n; ++i){
      vgl_point_3d<double> pd = pts[i]-vr;
      dsum += el.curve_distance(t, -pd.x(), pd.y(), pd.z());
    }
    right_params_.superior_margin_xyz_error_ = dsum/static_cast<double>(n);
    dsum=0;
    for(unsigned i = 0; i<n; ++i){
      double d = el.curve_distance(t, -(pts[i]-vr).x(), (pts[i]-vr).y());
      dsum += d;
    }
    right_params_.superior_margin_xy_error_ = dsum/static_cast<double>(n);
    break;
  }
  case RIGHT_EYE_SUPERIOR_CREASE:{
    pts = orbit_data_[RIGHT_EYE_SUPERIOR_CREASE];
    auto n = static_cast<unsigned>(pts.size());
    if(n==0){
      std::cout << "no superior crease points\n";
      return false;
    }
    boxm2_vecf_eyelid_crease el(right_params_);
    double t = right_params_.eyelid_crease_ct_;
    double dsum=0;
    for(unsigned i = 0; i<n; ++i){
      vgl_point_3d<double> pd = pts[i]-vr;
      dsum += el.curve_distance(t, -pd.x(), pd.y(), pd.z());
    }
    right_params_.superior_crease_xyz_error_ = dsum/static_cast<double>(n);
    dsum=0;
    for(unsigned i = 0; i<n; ++i)
      dsum += el.curve_distance(t, -(pts[i]-vr).x(), (pts[i]-vr).y());
    right_params_.superior_crease_xy_error_ = dsum/static_cast<double>(n);
    break;
  }
  default:{
    std::cout << "shouldn't happen\n";
    return false;
  }
  }
  return true;
}
