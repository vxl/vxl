#include <fstream>
#include <algorithm>
#include <limits>

#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_bilin_interp.h>
#include <vul/vul_file.h>

#include "acal_f_utils.h"
#include "acal_planar_feature_matcher.h"


bool
acal_planar_feature_matcher::load_solved_images(
    std::map<size_t, std::string>& solved_image_paths)
{
  // Make sure there are some images to load
  if (solved_image_paths.size() == 0) {
    std::cout << "No solved images to load" << std::endl;
    return false;
  }

  // For each path
  for (std::map<size_t, std::string>::iterator iit = solved_image_paths.begin();
       iit != solved_image_paths.end(); ++iit)
  {
    size_t idx = iit->first;
    std::string ipath = iit->second;

    // Load the image view
    vil_image_view<vxl_byte> view = vil_load(ipath.c_str());
    if (view.ni() == 0) {
      std::cout << "Couldn't load image from " << ipath << std::endl;
      return false;
    }

    // Store the loaded image view in this instance
    this->solved_images_[idx] = view;
  }
  return true;
}


bool
acal_planar_feature_matcher::load_solved_affine_cams(
    std::map<size_t, std::string>& solved_aff_cam_paths)
{
  // Make sure there are some cameras to load
  if (solved_aff_cam_paths.size() == 0) {
    std::cout << "No solved cameras to load" << std::endl;
    return false;
  }

  // For each path
  for (std::map<size_t, std::string>::iterator iit = solved_aff_cam_paths.begin();
       iit != solved_aff_cam_paths.end(); ++iit)
  {
    size_t idx = iit->first;
    std::string affine_cam_path = iit->second;

    // Load the affine camera
    vpgl_affine_camera<double> acam;
    std::ifstream istr(affine_cam_path.c_str());
    if (!istr) {
      std::cout << "Can't load affine camera from " << affine_cam_path << std::endl;
      return false;
    }
    istr >> acam;
    acam.set_viewing_distance(1000.0);

    // Store the loaded camera in this instance
    this->solved_cams_[idx] = acam;
  }
  return true;
}


void
acal_planar_feature_matcher::backproject_patch(
    vil_image_view<vxl_byte> const& view,
    vpgl_affine_camera<double> const& cam,
    vgl_point_3d<double> const& p3d,
    vil_image_view<vxl_byte>& patch,
    double u_off, double v_off)
{
  double r = this->params_.patch_radius_;
  double s = this->params_.pt_spacing_;
  size_t ni = static_cast<size_t>(2.0*(ceil(r/s))) + 1;
  patch.set_size(ni, ni);
  patch.fill(vxl_byte(0));
  size_t u = 0, v = 0;
  int niv = view.ni(), njv = view.nj();
  for (double dy = -r; dy <= r; dy += s, v++) {
    for (double dx = -r, u = 0; dx <= r; dx += s, u++) {
      vgl_point_3d<double> p(p3d.x() + dx, p3d.y() + dy, p3d.z());
      vgl_point_2d<double> p2d = cam.project(p);
      double x = p2d.x(), y = p2d.y();
      int du = static_cast<int>(x+u_off), dv = static_cast<int>(y+v_off);
      if (du < 0 || du >= niv || dv < 0 || dv >= njv)
        continue;
      double dval = vil_bilin_interp_safe_extend(view, x+u_off, y+v_off);
      unsigned val = static_cast<unsigned>(dval);
      patch(u, v) = static_cast<vxl_byte>(val);
    }
  }
}


bool
acal_planar_feature_matcher::generate_image_patches()
{
  size_t n = this->track_3d_pts_.size();
  if (n == 0) {
    std::cout << "no focus 3-d points" << std::endl;
    return false;
  }
  this->planar_patches_.resize(n);
  double r = this->params_.patch_radius_;
  double s = this->params_.pt_spacing_;
  for (size_t i = 0; i<n; ++i)
  {
    std::map<size_t, vil_image_view<vxl_byte> > patches;
    const vgl_point_3d<double>& p3d = this->track_3d_pts_[i];
    size_t nc = this->track_cams_.size();
    for (size_t c = 0; c<nc; ++c)
    {
      size_t cidx = this->track_cams_[c];
      if (this->solved_images_.count(cidx) == 0 || this->solved_cams_.count(cidx) == 0) {
        std::cout << "camera index " << cidx << " not in solved images or cameras" << std::endl;
        return false;
      }
      const vil_image_view<vxl_byte>& img = this->solved_images_[cidx];
      const vpgl_affine_camera<double>& cam = this->solved_cams_[cidx];
      vil_image_view<vxl_byte> patch;
      this->backproject_patch(img, cam, p3d, patch);
      patches[cidx] = patch;
    }
    this->planar_patches_[i] = patches;
  }
  return true;
}


bool
acal_planar_feature_matcher::save_planar_patches(std::string const& patch_dir)
{
  // Make sure the patch directory exists
  if (!vul_file::exists(patch_dir)) {
    std::cout << patch_dir << " does not exist" << std::endl;
    return false;
  }

  // Make sure we have some planar patches to save
  size_t n = this->planar_patches_.size();
  if (n == 0) {
    std::cout << "No patches to save" << std::endl;
    return false;
  }

  // Make a folder inside the patch directory for every image
  std::map<size_t, std::string> image_dirs;
  std::map<size_t, vil_image_view<vxl_byte> >& patches = this->planar_patches_[0];
  for (std::map<size_t, vil_image_view<vxl_byte> >::iterator pit = patches.begin();
        pit != patches.end(); ++pit)
  {
    size_t pidx = pit->first;
    if (this->solved_images_.count(pidx) == 0) {
      std::cout << "image id " << pidx << " doesn't exist in solved images" << std::endl;
      return false;
    }

    std::string idir = patch_dir + std::to_string(pidx) + "/";
    if (!vul_file::exists(idir)) {
      if (!vul_file::make_directory(idir)) {
        std::cout << "Can't make directory " << idir << std::endl;
        return false;
      }
    }
    image_dirs[pidx] = idir;
  }

  // Write each planar patch to the correct image folder
  for (size_t i = 0; i<n; ++i)
  {
    std::map<size_t, vil_image_view<vxl_byte> >& temp = this->planar_patches_[i];
    for (std::map<size_t, vil_image_view<vxl_byte> >::iterator pit = temp.begin();
        pit != temp.end(); ++pit) {
      size_t cidx = pit->first;
      std::stringstream ss;
      ss <<  "patch_0" << i << "_0" << cidx << ".tif";
      std::string ipath = ss.str();
      ipath = image_dirs[cidx] + ipath;
      vil_save(temp[cidx], ipath.c_str());
    }
  }
  return true;
}


//: apply a grid search for a local maximum in patch appearance score
bool
acal_planar_feature_matcher::local_maximum(
    vgl_vector_2d<double> grid_center, double grid_increment, double radius,
    vil_image_view<vxl_byte> const& ref_patch, vgl_point_3d<double> const& p3d,
    vil_image_view<vxl_byte> const& img, vpgl_affine_camera<double> const& cam,
    vgl_vector_2d<double>& max_location, double& max_score)
{
  max_score = -std::numeric_limits<double>::max();
  double max_du = 0.0, max_dv = 0.0;
  double u0 = grid_center.x(), v0 = grid_center.y();
  for (double dv = -radius; dv <= radius; dv+=grid_increment) {
    for (double du = -radius; du <= radius; du+=grid_increment) {
      vil_image_view<vxl_byte> mpatch;
      this->backproject_patch(img, cam, p3d, mpatch, u0+du, v0+dv);
      double s = 0.0;
      if (!acal_match_utils::gradient_direction_score(mpatch, ref_patch, s))
        continue;
      if (s > max_score) {
        max_score = s;
        max_du = du;
        max_dv = dv;
      }
    }
  }

  // local maximum at search boundary - fail
  if (max_dv == -radius || max_dv == radius || max_du == -radius || max_du == radius)
    return false;
  max_location.set(u0+ max_du, v0+ max_dv);
  return true;
}


bool
acal_planar_feature_matcher::set_match_translations(
    size_t cam_id,
    vil_image_view<vxl_byte> const& img,
    vpgl_affine_camera<double> const& cam)
{
  size_t n = this->planar_patches_.size();
  if (n == 0) {
    std::cout << "No patches to match" << std::endl;
    return false;
  }
  this->cam_translations_.clear();
  int ni = img.ni(), nj = img.nj();
  double coarse_r = static_cast<double>(this->params_.coarse_search_radius_);
  double fine_r = static_cast<double>(this->params_.fine_search_radius_);
  double coarse_inc = static_cast<double>(this->params_.coarse_search_increment_);
  double fine_inc = static_cast<double>(this->params_.fine_search_increment_);
  for (size_t i = 0; i<n; ++i)
  {
    std::map<size_t, vil_image_view<vxl_byte> >& temp = this->planar_patches_[i];
    const vgl_point_3d<double>& p3d = this->track_3d_pts_[i];
    vgl_point_2d<double> p2d = cam.project(p3d);
    int u0 = static_cast<int>(p2d.x()), v0 = static_cast<int>(p2d.y());
    if (u0<0 || u0>=ni || v0<0 || v0>=nj) {
      std::cout << "for cam " << cam_id << " track point " << p3d << " projects outside of image" << std::endl;
      continue;
    }

    // carry out a two stage search with a coarse and fine grid
    // the search for local maximum score fails if the translation is
    // on the search boundary
    std::map<size_t, std::pair<vgl_vector_2d<double>, double> > max_trans;
    for (std::map<size_t, vil_image_view<vxl_byte> >::iterator pit = temp.begin();
        pit != temp.end(); ++pit)
    {
      size_t cidx = pit->first;
      const vil_image_view<vxl_byte>& patch = pit->second;
      vgl_vector_2d<double> coarse_max_p_trans, coarse_center(0.0, 0.0);
      double coarse_max_p_score;
      if (!local_maximum(coarse_center, coarse_inc, coarse_r, patch, p3d, img, cam, coarse_max_p_trans, coarse_max_p_score))
        continue;
      // use the coarse solution as the center of the fine search grid
      vgl_vector_2d<double> max_p_trans, center(0.0, 0.0);
      double max_p_score;
      if (!local_maximum(coarse_max_p_trans, fine_inc, fine_r, patch, p3d, img, cam, max_p_trans, max_p_score))
        continue;
      planar_match_score ps;
      double prob = ps.p(max_p_score);
      if (prob < ps.min_prob_)
        continue;
      std::pair<vgl_vector_2d<double>, double> trans(max_p_trans, max_p_score);
      max_trans[cidx] = trans;
    }

  	if (max_trans.size() > 0)
  		this->cam_translations_.push_back(max_trans);
  	else
  		std::cout << "no local max for track id " << i << std::endl;
  }

  if (this->cam_translations_.size() == 0) {
    std::cout << "failed to find any translations for unsolved cam " << cam_id << std::endl;
    return false;
  }
  this->test_cam_ray_dir_ = cam.ray_dir();
  return true;
}


void
acal_planar_feature_matcher::compute_scores_stats_median_trans()
{
  std::vector<double> u_trans, v_trans, scores;
  double min_score = std::numeric_limits<double>::max(), max_score = 0.0;
  double score_sum = 0.0, score_sq = 0, median_score = 0.0, ns = 0.0;
  for (std::vector<std::map<size_t, std::pair<vgl_vector_2d<double>, double> > > ::iterator sit = this->cam_translations_.begin();
      sit != this->cam_translations_.end(); ++sit)
  {
    std::map<size_t, std::pair<vgl_vector_2d<double>, double> >& temp = *sit;
    for (std::map<size_t, std::pair<vgl_vector_2d<double>, double> >::iterator trit = temp.begin();
        trit != temp.end(); ++trit) {
      u_trans.push_back(trit->second.first.x());
      v_trans.push_back(trit->second.first.y());
      double s = trit->second.second;
      if(s < min_score)
        min_score = s;
      if(s > max_score)
        max_score = s;
      scores.push_back(s);
      score_sum += s;
      score_sq += s*s;
      ns +=1;
    }
  }

  double mean_score = 0.0, score_std_dev = 0.0;
  if(ns >=1) {
    mean_score = score_sum / ns;
    score_std_dev = sqrt(score_sq/ns - mean_score*mean_score);
  }

  std::sort(u_trans.begin(), u_trans.end());
  std::sort(v_trans.begin(), v_trans.end());
  std::sort(scores.begin(), scores.end());
  size_t n = u_trans.size();
  if (n == 1) {
    this->median_translation_.set(u_trans[0], v_trans[0]);
    this->match_score_stats_.median_score_ = scores[0];
    return;
  }

  size_t mid = n/2;
  this->median_translation_.set(u_trans[mid], v_trans[mid]);
  this->match_score_stats_.median_score_ = scores[mid];

  this->match_score_stats_.min_score_ = min_score;
  this->match_score_stats_.max_score_ = max_score;
  this->match_score_stats_.mean_score_ = mean_score;
  this->match_score_stats_.score_std_dev_ = score_std_dev;
}


bool
acal_planar_feature_matcher::process(
    size_t cam_id,
    vil_image_view<vxl_byte> const& img,
    vpgl_affine_camera<double> const& cam)
{
  if (!this->set_match_translations(cam_id, img, cam))
    return false;
  this->compute_scores_stats_median_trans();
  return true;
}


void
acal_planar_feature_matcher::print_matches()
{
  for (std::vector<std::map<size_t, std::pair<vgl_vector_2d<double>, double> > >::iterator trit = this->cam_translations_.begin();
      trit != this->cam_translations_.end(); ++trit)
  {
    std::map<size_t, std::pair<vgl_vector_2d<double>, double> >& temp = (*trit);
    for (std::map<size_t, std::pair<vgl_vector_2d<double>, double> >::iterator cit = temp.begin();
        cit != temp.end(); ++cit)
    {
      std::pair<vgl_vector_2d<double>, double>& pr = cit->second;
      vgl_vector_3d<double> ref_dir = solved_cams_[cit->first].ray_dir();
      double ang = angle(ref_dir, test_cam_ray_dir_)*180.0/3.14159;
      std::cout << cit->first << ' ' << ang << ' ' << pr.first.x() << ' ' << pr.first.y() << ' ' << pr.second << std::endl;
    }
  }
}


bool
acal_planar_feature_matcher::save_ground_truth_test_patch(
    size_t pt_id,
    std::string const& patch_dir,
    vil_image_view<vxl_byte> const& img,
    vpgl_affine_camera<double> const& cam,
    double du, double dv)
{
  vil_image_view<vxl_byte> mpatch;
  this->backproject_patch(img, cam, track_3d_pts_[pt_id], mpatch, du, dv);
  return vil_save(mpatch, (patch_dir + "test_match_gt.tif").c_str());
}
