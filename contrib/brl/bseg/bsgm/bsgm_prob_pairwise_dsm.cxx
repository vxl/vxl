// This is brl/bseg/bsgm/bsgm_prob_pairwise_dsm.cxx

#include "bsgm_prob_pairwise_dsm.h"
#include "bsgm_error_checking.h"
#include "bsgm_multiscale_disparity_estimator.h"
#include <limits>
#include <vil/vil_convert.h>
#include <vil/vil_save.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_distance.h>
#include <bvrml/bvrml_write.h> // for custom color map
#include <bvgl/bvgl_k_nearest_neighbors_3d.h>
#include <bpgl/algo/bpgl_3d_from_disparity.h>
#include <bpgl/algo/bpgl_heightmap_from_disparity.h>
#include <bpgl/algo/bpgl_gridding.h>

void bsgm_prob_pairwise_dsm::compute_byte()
{
  //remove small negative values due to interpolation during rectification
  vil_image_view<float> fview0 = rip_.rectified_fview0(), fview1 = rip_.rectified_fview1();
  ni_ = fview0.ni(); nj_ = fview0.nj();
  for (size_t j = 0; j<nj_; ++j) {
    for (size_t i = 0; i<ni_; ++i) {
      float f0 = fview0(i,j), f1 = fview1(i,j);
      if (f0<0.0f) fview0(i,j) = 0.0f;
      if (f1<0.0f) fview1(i,j) = 0.0f;
    }
  }
  vil_convert_stretch_range(fview0, rect_bview0_);
  vil_convert_stretch_range(fview1, rect_bview1_);
#if 0
  std::string rect0_path = "d:/Data/tile_test/crop/tile_0/pairwise_dsms/rect0.tif";
  std::string rect1_path = "d:/Data/tile_test/crop/tile_0/pairwise_dsms/rect1.tif";
  vil_save(rect_bview0_, rect0_path.c_str());
  vil_save(rect_bview1_, rect1_path.c_str());
#endif
}

vil_image_view<vxl_byte> bsgm_prob_pairwise_dsm::invalid_map() const
{
  vil_image_view<vxl_byte> ret;
  vil_convert_cast(invalid_map_, ret);
  return ret;
}

vil_image_view<vxl_byte> bsgm_prob_pairwise_dsm::rev_invalid_map() const
{
  vil_image_view<vxl_byte> ret;
  vil_convert_cast(invalid_map_reverse_, ret);
  return ret;
}

bool bsgm_prob_pairwise_dsm::compute_disparity()
{
  vxl_byte border_val = 0;

  bsgm_compute_invalid_map(rect_bview0_, rect_bview1_, invalid_map_, min_disparity_, num_disparities(), border_val);

  bsgm_multiscale_disparity_estimator mde(params_.de_params_, ni_, nj_, num_disparities(), num_active_disparities());

  float invalid_disp = NAN;//required for Dan's implementation of triangulation
  bool good = mde.compute(rect_bview0_, rect_bview1_, invalid_map_, min_disparity_,
                          invalid_disp, params_.multi_scale_mode_, disp_r_);
  return good;
}

bool bsgm_prob_pairwise_dsm::compute_rev_disparity()
{
  vxl_byte border_val = 0;

  bsgm_compute_invalid_map(rect_bview1_, rect_bview0_, invalid_map_reverse_, min_disparity_, num_disparities(), border_val);

  bsgm_multiscale_disparity_estimator mde(params_.de_params_, ni_, nj_, num_disparities(), num_active_disparities());

  float invalid_disp = NAN;//required for Dan's implementation of triangulation
  bool good = mde.compute(rect_bview1_, rect_bview0_, invalid_map_reverse_, min_disparity_,
                          invalid_disp, params_.multi_scale_mode_, disp_r_reverse_);
  return good;
}

void bsgm_prob_pairwise_dsm::compute_dsm_and_ptset(vgl_box_3d<double> const& scene_box)
{
  vgl_point_3d<double> pmin = scene_box.min_point(), pmax = scene_box.max_point();
  vgl_point_3d<float> pminf(pmin.x(), pmin.y(), pmin.z()), pmaxf(pmax.x(), pmax.y(), pmax.z());
  vgl_box_3d<float> fbox;
  fbox.add(pminf), fbox.add(pmaxf);
  float samp_dist = params_.point_sample_dist_;
  //
  //                              img0               img1     v
  // in sgm disparity estimation img_targ(x,y) <-> img_ref( x + disp_target(x,y), y )
  // the heightmap code requires  img1(u, v) <--> img0(u - disparity, v)
  // thus the order is reversed                          ^
  //
  height_map_ =  bpgl_heightmap_from_disparity(rip_.rect_acam1(), rip_.rect_acam0(), disp_r_, scene_box, samp_dist);
  vgl_point_2d<float> ul(pminf.x(), pmaxf.y());
  std::vector<vgl_point_3d<float> > pts3d;
  // bpgl_gridding::pointset_from_grid(height_map_, ul, samp_dist, pts3d);
  bpgl_pointset_from_disparity(rip_.rect_acam1(), rip_.rect_acam0(), disp_r_, fbox, pts3d);
  ptset_ = vgl_pointset_3d<float>(pts3d);
}

void bsgm_prob_pairwise_dsm::prob_heightmap(vgl_box_3d<double> const& scene_box)
{
  size_t n = prob_ptset_.size();
  if (n == 0)
    return;
  std::vector<vgl_point_2d<double> > triangulated_xy;
  std::vector<float> height_values, prob_values;
  for (size_t i = 0; i<n; ++i) {
    const vgl_point_3d<float>& p = prob_ptset_.p(i);
    triangulated_xy.emplace_back(p.x(), p.y());
    height_values.push_back(p.z());
    prob_values.push_back(prob_ptset_.sc(i));
  }
  vgl_point_2d<double> upper_left(scene_box.min_x(), scene_box.max_y());
  size_t ni = height_map_.ni(), nj = height_map_.nj();
  double gsd = params_.point_sample_dist_;
  size_t n_nbr = params_.num_nearest_nbrs_;
  double max_d = 2.0*gsd;

  bpgl_gridding::linear_interp<double, float> interp_fun(max_d, NAN);
  prob_height_map_z_ = bpgl_gridding::grid_data_2d(triangulated_xy, height_values,
                                                   upper_left, ni, nj, gsd,
                                                   interp_fun, n_nbr);
  prob_height_map_prob_ = bpgl_gridding::grid_data_2d(triangulated_xy, prob_values,
                                                      upper_left, ni, nj, gsd,
                                                      interp_fun, n_nbr);
}

bool bsgm_prob_pairwise_dsm::z_vs_disparity_scale(double& scale) const
{
  scale = 0.0;
  size_t ni = disp_r_.ni(), nj = disp_r_.nj();
  size_t i0 = ni/2, j0 = nj/2;
  std::vector<double> vd, vz;
  for (size_t j = j0; j<(j0 + nj/4); j++) {
    for (size_t i = i0; i<(i0 + ni/4); i++) {
      float d = disp_r_(i,j);
      if(vnl_math::isfinite(d)){
        float z = tri_image_3d_(i,j,2);
        if(vnl_math::isfinite(z)){
        vd.push_back(d);
        vz.push_back(z);
        }
      }
    }
  }
  double Sz = 0, Sd = 0, Sdz = 0, Sd2 = 0;
  size_t n = vd.size();
  if (n == 0) {
    std::cout << "no valid disparity-z pairs were found" << std::endl;
    return false;
  }
  for (size_t i = 0; i<n; ++i) {
    double d = vd[i], z = vz[i];
    Sz += z;
    Sd += d;
    Sdz += d*z;
    Sd2 += d*d;
  }
  Sz/=n; Sd/=n; Sdz/=n; Sd2/=n;
  double det = Sd2-(Sd*Sd);
  if (fabs(det)<1.0e-6) {
    std::cout << "singular det in computing z(d) slope" << std::endl;
    return false;
  }
  scale = (Sdz-Sd*Sz)/det;
  return true;
}

bool bsgm_prob_pairwise_dsm::compute_pointset_prob()
{
  // overall pointset probability is related to how rapidly z changes with disparity
  float p_mul = 1.0f;
  if (params_.use_z_vs_d_prob_) {
    if (!z_vs_disparity_scale(z_vs_disp_scale_))
      return false;
    float fscale = static_cast<float>(fabs(z_vs_disp_scale_)) - params_.min_z_vs_d_scale_;
    if (fscale < 0.0f) fscale = 0.0f;
    fscale *= fscale;
    float var = params_.z_vs_d_std_dev_;
    var *= var;
    p_mul = exp(-(0.5f*fscale)/var);
  }
  float sdsq = params_.std_dev_;
  prob_distr_ = bsta_histogram<float>(0.0f, 1.0f, 100);
  //float norm = sqrt(2.0)/(sdsq*sqrt(3.14159));
  sdsq *= sdsq;
  size_t n = ptset_.size(), n_nbrs = params_.num_nearest_nbrs_;
  if (n == 0)
    return false;
  prob_ptset_.clear();
  bvgl_k_nearest_neighbors_3d<float> knn(ptset_reverse_);
  float davg = 0.0f, dsq = 0.0f, dz_avg = 0.0f, dz_sq = 0.0f;
  float nstat = 0.0f;
  for (size_t i = 0; i<n; ++i) {
    const vgl_point_3d<float>& p = ptset_.p(i);
    vgl_point_3d<float> cp;
    if (!knn.closest_point(p, cp)) {
      std::cout << "KNN index failed to find neighbors - fatal" << std::endl;
      return false;
    }
    float d = vgl_distance(p, cp);
    float prob = p_mul * exp(-d*d/sdsq);
    prob_distr_.upcount(prob, 1.0);
    prob_ptset_.add_point_with_scalar(p, prob);
  }
  return true;
}

// the main probability routine
bool bsgm_prob_pairwise_dsm::compute_dsm_and_ptset_prob(vgl_box_3d<double> const& scene_box)
{
  vgl_point_3d<double> pmin = scene_box.min_point(), pmax = scene_box.max_point();
  vgl_point_3d<float> pminf(pmin.x(), pmin.y(), pmin.z()), pmaxf(pmax.x(), pmax.y(), pmax.z());
  vgl_box_3d<float> fbox;
  fbox.add(pminf), fbox.add(pmaxf);
  float samp_dist = params_.point_sample_dist_;
  // std::cout << "mid z " << mid_z_ << std::endl;
  //
  //                              img0               img1     v
  // in sgm disparity estimation img_targ(x,y) <-> img_ref( x + disp_target(x,y), y )
  // the heightmap code also requires  img1(u, v) <--> img0(u + disparity, v)
  //
  ///
  // forward triangulated 3-d data
  std::vector<vgl_point_3d<float> > pts3d, pts3d_reverse;

  tri_image_3d_ = bpgl_3d_from_disparity(rip_.rect_acam0(), rip_.rect_acam1(), disp_r_);
  height_map_ =  bpgl_heightmap_from_tri_image(rip_.rect_acam0(), rip_.rect_acam1(), tri_image_3d_, scene_box, samp_dist);

  tri_image_3d_reverse_ = bpgl_3d_from_disparity(rip_.rect_acam1(), rip_.rect_acam0(), disp_r_reverse_);
  height_map_reverse_ =  bpgl_heightmap_from_tri_image(rip_.rect_acam1(), rip_.rect_acam0(), tri_image_3d_reverse_,
                                                       scene_box, samp_dist);

  bpgl_pointset_from_tri_image(rip_.rect_acam0(), rip_.rect_acam1(), tri_image_3d_, fbox, pts3d);
  bpgl_pointset_from_tri_image(rip_.rect_acam1(), rip_.rect_acam0(), tri_image_3d_reverse_, fbox, pts3d_reverse);
  ptset_ = vgl_pointset_3d<float>(pts3d);
  ptset_reverse_ = vgl_pointset_3d<float>(pts3d_reverse);
  return this->compute_pointset_prob();
}

static void map_prob_to_color(float prob, float& r, float& g, float& b)
{
  if (prob<0.0f) prob=0.0f;
  else if (prob>1.0f) prob=1.0f;
  float ncolors = static_cast<float>(bvrml_custom_color::heatmap_custom_size);
  size_t color_index = static_cast<size_t>(prob*(ncolors-1));
  if (color_index>=bvrml_custom_color::heatmap_custom_size)
    color_index = bvrml_custom_color::heatmap_custom_size - 1;
  r = static_cast<float>(bvrml_custom_color::heatmap_custom[color_index][0]);
  g = static_cast<float>(bvrml_custom_color::heatmap_custom[color_index][1]);
  b = static_cast<float>(bvrml_custom_color::heatmap_custom[color_index][2]);
}

bool bsgm_prob_pairwise_dsm::save_prob_ptset_color(std::string const& path) const
{
  size_t n = prob_ptset_.size();
  float max = prob_distr_.value_with_area_above(0.05f);
  std::ofstream ostr(path.c_str());
  if (!ostr) {
    std::cout << "Can't open " << path << " to write color ptset" << std::endl;
    return false;
  }
  for (size_t i = 0; i<n; ++i) {
    float prob = prob_ptset_.sc(i);
    float norm_prob = prob/max;
    float r, g, b;
    map_prob_to_color(norm_prob, r, g, b);
    const vgl_point_3d<float>& p = prob_ptset_.p(i);
    ostr << p.x() << ' ' << p.y() << ' ' << p.z() << ' ' << r << ' ' << g << ' ' << b << std::endl;
  }
  ostr.close();
  return true;
}
