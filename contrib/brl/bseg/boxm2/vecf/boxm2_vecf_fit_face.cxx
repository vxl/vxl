#include <fstream>
#include <algorithm>
#include <limits>
#include <iomanip>
#include "boxm2_vecf_labeled_point.h"
#include "boxm2_vecf_fit_face.h"
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_ray_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_intersection.h>
#include <vgl/vgl_pointset_3d.h>
#include <vgl/algo/vgl_h_matrix_3d_compute_affine.h>
#include <bvrml/bvrml_write.h>
#include <vnl/vnl_matrix_fixed.h>
void boxm2_vecf_fit_face::fill_smid_map(){
  smid_map_["left_lateral_canthus"]=LEFT_LATERAL_CANTHUS;
  smid_map_["right_lateral_canthus"]=RIGHT_LATERAL_CANTHUS;
  smid_map_["left_medial_canthus"]=LEFT_MEDIAL_CANTHUS;
  smid_map_["right_medial_canthus"]=RIGHT_MEDIAL_CANTHUS;
  smid_map_["mid_upper_jaw"]=MID_UPPER_JAW;
  smid_map_["mid_forehead"]=MID_FOREHEAD;
  smid_map_["mid_forehead_normal"]=MID_FOREHEAD_NORMAL;
  smid_map_["canthus_midpoint"]=CANTHUS_MIDPOINT;
  smid_map_["forehead_intersection"]=FOREHEAD_INTERSECTION;
  smid_map_["nose"]=NOSE;
  smid_map_["chin"]=CHIN;
}
bool boxm2_vecf_fit_face::add_labeled_point(boxm2_vecf_labeled_point lp){
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
bool boxm2_vecf_fit_face::read_anchor_file(std::string const& path){

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
    boxm2_vecf_labeled_point lp(x, y, z, lab);
    if(!this->add_labeled_point(lp))
      return false;
  }
  return true;
}
bool boxm2_vecf_fit_face::compute_auxillary_points(){
  // convert point format to vector format (labeled point)
  std::map<mids, boxm2_vecf_labeled_point>::iterator lit;
  lit = lpts_.find(MID_FOREHEAD_NORMAL);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& fn = lit->second.p3d_;
  forehead_normal_.set(fn.x(), fn.y(), fn.z());

  lit = lpts_.find(MID_FOREHEAD);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& fmid = lit->second.p3d_;

  // midpoint of line joining the canthi
   lit = lpts_.find(LEFT_LATERAL_CANTHUS);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& llc = lit->second.p3d_;
  lit = lpts_.find(RIGHT_LATERAL_CANTHUS);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& rlc = lit->second.p3d_;
  vgl_vector_3d<double> r_to_l = llc-rlc;
  vgl_point_3d<double> midp = rlc+(0.5*r_to_l);
  cmv_.set(midp.x(), midp.y(), midp.z());
  boxm2_vecf_labeled_point lpmid(midp, "canthus_midpoint");
  this->add_labeled_point(lpmid);

  // affine-invariant point on forehead plane
  // construct a ray from the canthi midpoint parallel to the forehead
  // normal and then find the intersection with the forehead plane
  vgl_ray_3d<double> ray(midp, forehead_normal_);
  vgl_plane_3d<double> pl(forehead_normal_, fmid);
  vgl_point_3d<double> fore_int;
  bool success = vgl_intersection(ray, pl, fore_int);
  if(!success){
    std::cout << "Intersection of ray with forehead plane - failed\n";
    return false;
  }
  boxm2_vecf_labeled_point lpint(fore_int, "forehead_intersection");
  this->add_labeled_point(lpint);
  return true;
}
bool boxm2_vecf_fit_face::set_trans(){

  // get target points previously read from anchor file
  std::map<mids, boxm2_vecf_labeled_point>::iterator lit;

  lit = lpts_.find(LEFT_LATERAL_CANTHUS);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& llc_tgt = lit->second.p3d_;

  lit = lpts_.find(RIGHT_LATERAL_CANTHUS);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& rlc_tgt = lit->second.p3d_;

  lit = lpts_.find(LEFT_MEDIAL_CANTHUS);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& lmc_tgt = lit->second.p3d_;

  lit = lpts_.find(RIGHT_MEDIAL_CANTHUS);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& rmc_tgt = lit->second.p3d_;

  lit = lpts_.find(MID_UPPER_JAW);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& mjaw_tgt = lit->second.p3d_;

  lit = lpts_.find(FOREHEAD_INTERSECTION);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& fint_tgt = lit->second.p3d_;

  lit = lpts_.find(NOSE);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& nose_tgt = lit->second.p3d_;

  lit = lpts_.find(CHIN);
  if(lit == lpts_.end())
    return false;
  const vgl_point_3d<double>& chin_tgt = lit->second.p3d_;

  //get source points
  vgl_point_3d<double> llc_src = params_.left_lateral_canthus_;
  vgl_point_3d<double> rlc_src = params_.right_lateral_canthus_;
  vgl_point_3d<double> lmc_src = params_.left_medial_canthus_;
  vgl_point_3d<double> rmc_src = params_.right_medial_canthus_;
  vgl_point_3d<double> mjaw_src = params_.mid_upper_jaw_;
  vgl_point_3d<double> fint_src = params_.forehead_intersection_;
  vgl_point_3d<double> nose_src = params_.nose_;
  vgl_point_3d<double> chin_src = params_.chin_;

  std::vector<vgl_homg_point_3d<double> > source_pts, target_pts;
  source_pts.emplace_back(llc_src); source_pts.emplace_back(rlc_src);
  source_pts.emplace_back(lmc_src); source_pts.emplace_back(rmc_src);
  source_pts.emplace_back(mjaw_src); source_pts.emplace_back(fint_src);
  source_pts.emplace_back(nose_src); source_pts.emplace_back(chin_src);

  target_pts.emplace_back(llc_tgt);  target_pts.emplace_back(rlc_tgt);
  target_pts.emplace_back(lmc_tgt);  target_pts.emplace_back(rmc_tgt);
  target_pts.emplace_back(mjaw_tgt); target_pts.emplace_back(fint_tgt);
  target_pts.emplace_back(nose_tgt); target_pts.emplace_back(chin_tgt);

  vgl_h_matrix_3d_compute_affine hca;
  bool success = hca.compute(source_pts, target_pts, params_.trans_);
  if(!success) return false;
  //for debug purposes
  std::cout << params_.trans_ << '\n'<< std::flush;
  vnl_matrix_fixed<double, 3, 3> R, S;
  params_.trans_.polar_decomposition(S, R);
  std::cout << "Rotation part\n " << R << '\n'<< std::flush;
  std::cout << "Symmetric part\n " << S << '\n'<< std::flush;

  auto n = static_cast<unsigned>(source_pts.size());
  for(unsigned i = 0; i<n; ++i){
    vgl_homg_point_3d<double> hts = params_.trans_(source_pts[i]);
    vgl_point_3d<double> ts(hts), t(target_pts[i]);
    std::cout << std::setprecision(3) << ts << ' ' << t << ' ' << (t-ts).length() << '\n'<< std::flush;
  }

  return true;
}

bool boxm2_vecf_fit_face::load_composite_face_params(std::string const& params_path){
  std::ifstream istr(params_path.c_str());
  if(!istr){
    std::cout << "Can't open parameter path " << params_path << '\n';
    return false;
  }
  istr >> params_;
  return true;
}

bool boxm2_vecf_fit_face::transform_face(std::string const& source_face_path, std::string const& target_face_path) const{
  vgl_pointset_3d<double> src_ptset, trg_ptset;
  std::ifstream sistr(source_face_path.c_str());
    if(!sistr)
      return false;
    sistr >> src_ptset;

    sistr.close();

  trg_ptset = params_.trans_(src_ptset);

  std::ofstream tostr(target_face_path.c_str());
    if(!tostr)
      return false;
    tostr << trg_ptset;
    tostr.close();
  return true;
}

bool boxm2_vecf_fit_face::inverse_transform_face(std::string const& source_face_path, std::string const& target_face_path) const{
  vgl_pointset_3d<double> src_ptset, trg_ptset;
  std::ifstream sistr(source_face_path.c_str());
    if(!sistr)
      return false;
    sistr >> src_ptset;

    sistr.close();

  trg_ptset = params_.trans_.preimage(src_ptset);

  std::ofstream tostr(target_face_path.c_str());
    if(!tostr)
      return false;
    tostr << trg_ptset;

    tostr.close();
  return true;
}
