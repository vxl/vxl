#include "bsgm_pair_selector.h"
#include <vnl/vnl_math.h>
#include <bjson/bjson.h>
#include <algorithm>
#include <fstream>
#include <bpgl/acal/acal_metadata.h>

std::string bsgm_pair_selector::remove_crop_postfix(std::string const& iname)
{
  size_t s = iname.find("_crop");
  if (s == std::string::npos)
    return iname;
  std::string temp = iname;
  temp.replace(s, 5, std::string(""));
  return temp;
}

bool bsgm_pair_selector::load_uncorr_affine_cams(std::string affine_cam_path)
{
  return acal_f_utils::read_affine_cameras(affine_cam_path, acams_);
}

bool bsgm_pair_selector::set_inames()
{
  size_t ni = meta_.img_meta_.size(), ng = meta_.geo_corr_meta_.size();
  if (ni == 0 || ng == 0) {
    std::cout << "null image or geo_corr metadata (n_imeta, n_gmeta) " << ni << ' ' << ng << std::endl;
    return false;
  }
  for (size_t i = 0; i<ng; ++i) {
    // remove images with too large a projection error or a duplicate, indicated by a proj error of -1
    const geo_corr_metadata& gm = meta_.geo_corr_meta_[i];
    if(gm.rms_proj_err_ > params_.max_proj_err_)
      continue;
    if(gm.rms_proj_err_ == -1.0)
      continue;
    //geo_corr metadata has the actual image name including the _crop postfix
    //the base image name is required
    inames_[gm.image_id_] = remove_crop_postfix(gm.image_name_);
  }
  std::cout << inames_.size() << " images with proj error at or below " << params_.max_proj_err_ << std::endl;
  for (std::map<size_t, std::string>::iterator iit = inames_.begin();
      iit != inames_.end(); ++iit) {
    size_t i = iit->first;
    std::string iname = iit->second;
    dtime datetime;
    if (!acal_f_utils::datetime_from_iname(iname, datetime)) {
      std::cout << "can't resolve datetime from " << iname << std::endl;
      return false;
    }
    dtimes_[i]=datetime;
  }
  return true;
}

vgl_vector_3d<double>  bsgm_pair_selector::sun_direction(std::string const& iname){
  vgl_vector_3d<double> ret(0.0, 0.0, 0.0);
  double deg_to_rad = vnl_math::pi/180.0;
  bool found = false;
  vgl_vector_2d<double> sph_dir(0.0, 0.0);
  size_t n = meta_.img_meta_.size();
  for (size_t i = 0; i<n&&!found; ++i) {
    if (iname == (meta_.img_meta_[i].image_name_)) {
      found = true;
      sph_dir =  meta_.img_meta_[i].sph_sun_dir_;
    }
  }
  if (!found) {
    std::cout << "Warning - for sun direction can't find image name " << iname << " in metadata" << std::endl;
    return ret;
  }
  double s = sin(sph_dir.y()*deg_to_rad);
  double c = cos(sph_dir.y()*deg_to_rad);
  double x = -c*sin(sph_dir.x()*deg_to_rad);
  double y = -c*cos(sph_dir.x()*deg_to_rad);
  double z = -s;
  ret.set(x, y, z);
  return ret;
}

double bsgm_pair_selector::gsd(std::string const& iname)
{
  double ret = 0.0;
  size_t n = meta_.img_meta_.size();
  bool found = false;
  for (size_t i = 0; i<n&&!found; ++i)
    if (iname == (meta_.img_meta_[i].image_name_))
    {
      found = true;
      ret =  meta_.img_meta_[i].gsd_;
    }
  if (!found) {
    std::cout << "Warning - for gsd can't find image name " << iname << " in metadata" << std::endl;
  }
  return ret;
}

void bsgm_pair_selector::cache_pair_info()
{
  bsgm_pair_less pl(params_.best_angle_, params_.low_angle_bnd_, params_.high_angle_bnd_,params_.max_sun_ang_, params_.max_gsd_ratio_, sun_angle_diffs_, ang_diffs_, gsd_ratios_);
  size_t n = ordered_pairs_.size();
  for (size_t k = 0; k<n; ++k) {
    size_t i = ordered_pairs_[k].first, j = ordered_pairs_[k].second;
    std::string iname_i = inames_[i], iname_j = inames_[j];
    bsgm_pair_info pinf;
    pinf.i_ =i; pinf.j_ = j;
    pinf.iname_i_ = iname_i; pinf.iname_j_ = iname_j;
    pinf.view_angle_diff_ = ang_diffs_[i][j];
    pinf.sun_angle_diff_ = sun_angle_diffs_[i][j];
    pinf.gsd_ratio_ = gsd_ratios_[i][j];
    pinf.cost_ = pl.cost(pinf.view_angle_diff_, pinf.sun_angle_diff_, pinf.gsd_ratio_);
    ordered_pair_info_.push_back(pinf);
  }
}

bool bsgm_pair_selector::prioritize_pairs()
{
  f_params params;
  double dgen_tol = 1.0/(params.ray_uncertainty_tol_);
  // get time differences
  std::map<size_t, std::string>::iterator before_end = inames_.end();
  std::advance(before_end, -1);
  size_t ii = 0;
  for (std::map<size_t, std::string>::iterator iit = inames_.begin();
      iit != before_end; ++iit, ++ii)
  {
    size_t idx = iit->first;
    const dtime& dti = dtimes_[idx];
    std::map<size_t, std::string>::iterator jit = inames_.begin();
    std::advance(jit, ii + 1);
    for (; jit != inames_.end(); ++jit) {
      size_t jdx = jit->first;
      const dtime& dtj = dtimes_[jdx];
      int dt = abs(dtime::time_diff(dti, dtj));
      time_diffs_[idx][jdx] = dt;
    }
  }

  // get view ray differences (angle in degrees)
  ii = 0;
  for (std::map<size_t, std::string>::iterator iit = inames_.begin();
      iit != before_end; ++iit, ++ii) {
    size_t idx = iit->first;
    vgl_vector_3d<double> dir_i = acams_[idx].ray_dir();
    if (dir_i.z()>0.0) dir_i = -dir_i;
    std::string iname = inames_[idx];
    std::string nc_iname   = remove_crop_postfix(iname);
    vgl_vector_3d<double> sun_ang_i = sun_direction(nc_iname);
    double gsd_i = gsd(nc_iname);
    std::map<size_t, std::string>::iterator jit = inames_.begin();
    std::advance(jit, ii+1);
    for (;jit != inames_.end(); ++jit) {
      size_t jdx = jit->first;
      std::string jname = inames_[jdx];
      std::string nc_jname  = remove_crop_postfix(jname);
      vgl_vector_3d<double> sun_ang_j = sun_direction(nc_jname);
      double sun_ang_dif = fabs(angle(sun_ang_i, sun_ang_j));
      sun_angle_diffs_[idx][jdx]=sun_ang_dif * 180.0 / vnl_math::pi;
      vgl_vector_3d<double> dir_j = acams_[jdx].ray_dir();
      if (dir_j.z()>0.0) dir_j = -dir_j;
      double gsd_j = gsd(nc_jname);
      double ang = fabs(angle(dir_i, dir_j))*180.0/vnl_math::pi;
      ang_diffs_[idx][jdx] = ang;

      double el = acos(fabs(dir_i.z()));
      double az = atan2(dir_i.y(), dir_i.x());
      if (fabs(dir_j.z()) < fabs(dir_i.z())) {
        el = acos(fabs(dir_j.z()));
        az = atan2(dir_j.y(), dir_j.x());
      }
      el *= 180.0/vnl_math::pi;
      az *= 180.0/vnl_math::pi;
      view_az_el_[idx][jdx] = std::pair<double,double>(az, el);
      double gsd_ratio = gsd_i/gsd_j;
      if (gsd_ratio<1.0)
        gsd_ratio = 1.0/gsd_ratio;
      gsd_ratios_[idx][jdx] = gsd_ratio;
      double dp = dot_product(dir_i, dir_j);
      double dgen = fabs(1.0 - (dp*dp));
      no_ray_degen_[idx][jdx] = dgen > 0.1*dgen_tol;
    }
  }
  ii = 0;
  for (std::map<size_t, std::string>::iterator iit = inames_.begin();
      iit != before_end; ++iit, ++ii) {
    size_t idx = iit->first;
    std::map<size_t, std::string>::iterator jit = inames_.begin();
    std::advance(jit, ii + 1);
    for (; jit != inames_.end(); ++jit) {
      size_t jdx = jit->first;
      if (no_ray_degen_[idx][jdx])
        ordered_pairs_.emplace_back(idx, jdx);
    }
  }
  bsgm_pair_less pl(params_.best_angle_, params_.low_angle_bnd_, params_.high_angle_bnd_,params_.max_sun_ang_, params_.max_gsd_ratio_, sun_angle_diffs_, ang_diffs_, gsd_ratios_);
  std::sort(ordered_pairs_.begin(),ordered_pairs_.end(), pl);
  cache_pair_info();
  return true;
}

std::vector<std::pair<std::string, std::string> > bsgm_pair_selector::pair_inames()
{
  std::vector<std::pair<std::string, std::string> > ret;
  for(std::vector<std::pair<size_t, size_t> >::const_iterator pit = ordered_pairs_.begin();
      pit != ordered_pairs_.end(); ++pit) {
    std::pair<std::string, std::string> pr(inames_[pit->first], inames_[pit->second]);
    ret.push_back(pr);
  }
  return ret;
}

bool bsgm_pair_selector::read_image_metadata()
{
  std::string meta_path = meta_dir_ + image_meta_fname_;
  Json::Value root;
  Json::Reader reader;
  std::ifstream istr(meta_path.c_str());
  if (!istr) {
    std::cout << "Can't open " << meta_path << " to read metadata" << std::endl;
    return false;
  }
  bool good = reader.parse(istr, root);
  if (!good) {
    std::cout << " Json parse failed for file " << meta_path << std::endl;
    return false;
  }
  meta_.deserialize_image_meta(root);
  return true;
}

bool bsgm_pair_selector::read_geo_corr_metadata(bool seed_cameras_only)
{
  std::string meta_path = meta_dir_ + geo_corr_meta_fname_;
  Json::Value root, non_seed_root;
  Json::Reader reader;
  std::ifstream istr(meta_path.c_str());
  if (!istr) {
    std::cout << "Can't open " << meta_path << " to read metadata" << std::endl;
    return false;
  }
  bool good = reader.parse(istr, root);
  if (!good) {
    std::cout << " Json parse failed for file " << meta_path << std::endl;
    return false;
  }
  meta_.deserialize_geo_corr_meta(root);
  if (!seed_cameras_only) {
    std::string non_seed_meta_path = meta_dir_ + non_seed_geo_corr_meta_fname_;
    std::ifstream non_seed_istr(non_seed_meta_path.c_str());
    if (!non_seed_istr) {
      std::cout << "Can't open " << non_seed_meta_path << " to read metadata" << std::endl;
      return false;
    }
    good = reader.parse(non_seed_istr, non_seed_root);
    if (!good) {
      std::cout << " Json parse failed for file " << non_seed_meta_path << std::endl;
      return false;
    }
    non_seed_meta_.deserialize_geo_corr_meta(non_seed_root);
    meta_.geo_corr_meta_.insert(meta_.geo_corr_meta_.end(), non_seed_meta_.geo_corr_meta_.begin(), non_seed_meta_.geo_corr_meta_.end());
  }
  return true;
}

void bsgm_pair_selector::print_ordered_pairs()
{
  std::cout << ">>>>>>>>>>>>>>>Ordered pairs<<<<<<<<<<<<<<<<<<<" << std::endl;
  std::cout << "i   j   view     sun     gsd      cost" << std::endl;
  bsgm_pair_less pl(params_.best_angle_, params_.low_angle_bnd_, params_.high_angle_bnd_,params_.max_sun_ang_, params_.max_gsd_ratio_, sun_angle_diffs_, ang_diffs_, gsd_ratios_);
  for (std::vector<std::pair<size_t, size_t> >::const_iterator pit = ordered_pairs_.begin();
       pit != ordered_pairs_.end(); ++pit) {
    size_t i = pit->first, j = pit->second;
    double view_ang_dif = ang_diffs_[i][j];
    double sun_angle_dif = sun_angle_diffs_[i][j];
    double gsd_rat = gsd_ratios_[i][j];
    double c = pl.cost(view_ang_dif, sun_angle_dif, gsd_rat);
    std::cout << i << ' ' << j << ' ' << view_ang_dif << ' '<< sun_angle_dif << ' ' << gsd_rat << ' ' << c << std::endl;
  }
}

void bsgm_pair_selector::print_pair_el_az()
{
  std::cout << "i   j   el     az " << std::endl;
  for (std::vector<std::pair<size_t, size_t> >::const_iterator pit = ordered_pairs_.begin();
       pit != ordered_pairs_.end(); ++pit) {
    size_t i = pit->first, j = pit->second;
    double az = view_az_el_[i][j].first, el = view_az_el_[i][j].second;
    std::cout << i << ' ' << j << ' ' << el << ' '<< az << std::endl;
  }
}

bool bsgm_pair_selector::save_ordered_pairs()
{
  acal_metadata meta;
  std::string path = tile_dir_ + subdir_ + pair_output_fname_;
  std::ofstream ostr(path.c_str());
  if (!ostr) {
    std::cout << "Can't open " << path<< " to write image pairs" << std::endl;
    return false;
  }
  size_t n = ordered_pairs_.size();
  for (size_t i = 0; i<n; ++i) {
    const std::pair<size_t, size_t>& pr = ordered_pairs_[i];
    pair_selection_metadata psm;
    psm.i_ = pr.first;
    psm.j_ = pr.second;
    std::string iname_no_corr = remove_crop_postfix(inames_[pr.first]);
    std::string jname_no_corr = remove_crop_postfix(inames_[pr.second]);
    psm.iname_i_ = iname_no_corr;
    psm.iname_j_ = jname_no_corr;
    psm.cost_ = ordered_pair_info_[i].cost_;
    meta.pair_meta_.push_back(psm);
  }
  Json::Value pair_root;
  meta.serialize_pair_selection_meta(pair_root);
  if (pair_root.isNull()) {
    std::cout << "serialize pair metadata failed" << std::endl;
    return false;
  }
  Json::StyledStreamWriter writer;
  writer.write(ostr, pair_root);
  return true;
}

bool bsgm_pair_selector::save_all_ordered_pairs()
{
  acal_metadata meta;
  std::string path = meta_dir_ + pair_output_fname_;
  std::ofstream ostr(path.c_str());
  if (!ostr) {
    std::cout << "Can't open " << path<< " to write image pairs" << std::endl;
    return false;
  }
  size_t n = ordered_pairs_.size();
  for (size_t i = 0; i<n; ++i) {
    const std::pair<size_t, size_t>& pr = ordered_pairs_[i];
    pair_selection_metadata psm;
    psm.i_ = pr.first;
    psm.j_ = pr.second;
    std::string iname_no_corr = remove_crop_postfix(inames_[pr.first]);
    std::string jname_no_corr = remove_crop_postfix(inames_[pr.second]);
    psm.iname_i_ = iname_no_corr;
    psm.iname_j_ = jname_no_corr;
    psm.cost_ = ordered_pair_info_[i].cost_;
    meta.pair_meta_.push_back(psm);
  }
  Json::Value pair_root;
  meta.serialize_pair_selection_meta(pair_root);
  if (pair_root.isNull()) {
    std::cout << "serialize pair metadata failed" << std::endl;
    return false;
  }
  Json::StyledStreamWriter writer;
  writer.write(ostr, pair_root);
  return true;
}
