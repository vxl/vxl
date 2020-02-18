// This is brl/bseg/bsgm/bsgm_prob_pairwise_dsm.cxx

#include "bsgm_prob_pairwise_dsm.h"
#include "bsgm_error_checking.h"
#include "bsgm_multiscale_disparity_estimator.h"
#include <limits>
#include <stdexcept>
#include "vil/vil_convert.h"
#include "vil/vil_save.h"
#include "vnl/vnl_math.h"
#include "vgl/vgl_box_3d.h"
#include "vgl/vgl_point_3d.h"
#include "vgl/vgl_distance.h"
#include <bvrml/bvrml_write.h> // for custom color map
#include <bvgl/bvgl_k_nearest_neighbors_3d.h>
#include <bpgl/algo/bpgl_3d_from_disparity.h>
#include <bpgl/algo/bpgl_heightmap_from_disparity.h>
#include <bpgl/algo/rectify_params.h>

// ----------
// Rectification
// ----------
void bsgm_prob_pairwise_dsm::rectify()
{
  // set parameters
  rectify_params rp;
  rp.min_disparity_z_ = mid_z_;
  rp.upsample_scale_ = params_.upsample_scale_factor_;
  // rectified images
  vil_image_view<float> fview0, fview1;
  // carry out rectification
  if(affine_){
    rip_affine_.set_params(rp);
    if(!rip_affine_.process(scene_box_))
      throw std::runtime_error("affine rectification failed");
    fview0 = rip_affine_.rectified_fview0(), fview1 = rip_affine_.rectified_fview1();
  }else{
    rip_perspective_.set_params(rp);
    if(!rip_perspective_.process(scene_box_))
      throw std::runtime_error("perspective rectification failed");
    fview0 = rip_perspective_.rectified_fview0(), fview1 = rip_perspective_.rectified_fview1();
  }
  // remove small negative values due to interpolation during rectification
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
}


// ----------
// Disparity
// ----------

// compute disparity map from input images
void bsgm_prob_pairwise_dsm::compute_disparity(
    const vil_image_view<vxl_byte>& img,
    const vil_image_view<vxl_byte>& img_reference,
    bool forward,
    vil_image_view<bool>& invalid,
    vil_image_view<float>& disparity)
{
  vxl_byte border_val = 0;
  float invalid_disp = NAN; //required for triangulation implementation
  bool good = true;
  bsgm_compute_invalid_map(img, img_reference, invalid,
                           min_disparity_, num_disparities(), border_val);
  if(params_.coarse_dsm_disparity_estimate_){
    bsgm_multiscale_disparity_estimator mde(params_.de_params_, ni_, nj_,
                                            num_disparities(), num_active_disparities());
                                            params_.downscale_exponent_);

    good = mde.compute(img, img_reference, invalid,
                       min_disparity_, invalid_disp, params_.multi_scale_mode_,
                       disparity);
    if (!good)
      throw std::runtime_error("Multiscale disparity estimator failed");
  }else{//use input min_disparity
    size_t ni = img.ni(), nj = img.nj();

    vil_image_view<int> min_disparity(ni, nj);
    if(forward)// img = img0, ref = img1
      min_disparity.fill(min_disparity_);
    else // reverse img = img1, ref = img0
      min_disparity.fill(-(num_disparities()-fabs(min_disparity_)));

    bsgm_disparity_estimator bsgm(params_.de_params_, ni, nj, num_disparities());
    good = bsgm.compute(img, img_reference, invalid, min_disparity, invalid_disp, disparity);
    if (!good)
      throw std::runtime_error("disparity estimator failed");
  }
}

// compute forward disparity
void bsgm_prob_pairwise_dsm::compute_disparity_fwd()
{
  bool forward = true;
  compute_disparity(rect_bview0_, rect_bview1_, forward, invalid_map_fwd_, disparity_fwd_);
}

// compute reverse disparity
void bsgm_prob_pairwise_dsm::compute_disparity_rev()
{
  bool forward = false;
  compute_disparity(rect_bview1_, rect_bview0_, forward, invalid_map_rev_, disparity_rev_);
}


// ----------
// Heightmaps
// ----------

// compute height (tri_3d, ptset, heightmap)
template <class CAM_T>
void bsgm_prob_pairwise_dsm::compute_height(const CAM_T& cam, const CAM_T& cam_reference,
                                            const vil_image_view<float>& disparity,
                                            vil_image_view<float>& tri_3d, vgl_pointset_3d<float>& ptset,
                                            vil_image_view<float>& heightmap)
{
  // triangulated image
  tri_3d = bpgl_3d_from_disparity(cam, cam_reference, disparity);

  // convert triangulated image to pointset & heightmap
  auto bh = this->get_bpgl_heightmap();
  bh.pointset_from_tri(tri_3d, ptset);
  bh.heightmap_from_pointset(ptset, heightmap);
}

// compute forward height
void bsgm_prob_pairwise_dsm::compute_height_fwd()
{
  if(affine_)
    this->compute_height(rip_affine_.rect_cam0(), rip_affine_.rect_cam1(), disparity_fwd_,
                         tri_3d_fwd_, ptset_fwd_, heightmap_fwd_);
  else
    this->compute_height(rip_perspective_.rect_cam0(), rip_perspective_.rect_cam1(), disparity_fwd_,
                         tri_3d_fwd_, ptset_fwd_, heightmap_fwd_);
}

// compute reverse height
void bsgm_prob_pairwise_dsm::compute_height_rev()
{
  if(affine_)
    this->compute_height(rip_affine_.rect_cam1(), rip_affine_.rect_cam0(), disparity_rev_,
                         tri_3d_rev_, ptset_rev_, heightmap_rev_);
  else
    this->compute_height(rip_perspective_.rect_cam1(), rip_perspective_.rect_cam0(), disparity_rev_,
                         tri_3d_rev_, ptset_rev_, heightmap_rev_);
}


// ----------
// Probabilistic heightmaps
// ----------

// compute probablistic height (ptset, heightmap, probability)
bool bsgm_prob_pairwise_dsm::compute_prob()
{
  // check number of points
  if (ptset_fwd_.size() == 0) {
    std::runtime_error("ptset_fwd_ is empty");
  }
  if (ptset_rev_.size() == 0) {
    std::runtime_error("ptset_rev_ is empty");
  }

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

  size_t n = ptset_fwd_.size();
  prob_ptset_.clear();

  bvgl_k_nearest_neighbors_3d<float> knn(ptset_rev_);
  for (size_t i = 0; i<n; ++i) {
    const vgl_point_3d<float>& p = ptset_fwd_.p(i);
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

  // check size
  n = prob_ptset_.size();
  if (n == 0) {
    std::runtime_error("prob_ptset_ is empty");
  }

  // convert pointset to images
  auto bh = this->get_bpgl_heightmap();
  bh.heightmap_from_pointset(prob_ptset_, prob_heightmap_z_, prob_heightmap_prob_);
  return true;
}


bool bsgm_prob_pairwise_dsm::z_vs_disparity_scale(double& scale) const
{
  scale = 0.0;
  size_t ni = disparity_fwd_.ni(), nj = disparity_fwd_.nj();
  size_t i0 = ni/2, j0 = nj/2;
  std::vector<double> vd, vz;
  for (size_t j = j0; j<(j0 + nj/4); j++) {
    for (size_t i = i0; i<(i0 + ni/4); i++) {
      float d = disparity_fwd_(i,j);
      if(vnl_math::isfinite(d)){
        float z = tri_3d_fwd_(i,j,2);
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


// ----------
// HELPER FUNCTIONS
// ----------

// return boolean image to byte image
vil_image_view<vxl_byte> bsgm_prob_pairwise_dsm::bool_to_byte(
    const vil_image_view<bool>& img) const
{
  vil_image_view<vxl_byte> ret;
  vil_convert_cast(img, ret);
  return ret;
}

// return bpgl_heightmap class
bpgl_heightmap<float>
bsgm_prob_pairwise_dsm::get_bpgl_heightmap() const
{
  // scene box as float
  auto scene_box_as_float = vgl_box_3d<float>(
      float(this->scene_box_.min_x()),
      float(this->scene_box_.min_y()),
      float(this->scene_box_.min_z()),
      float(this->scene_box_.max_x()),
      float(this->scene_box_.max_y()),
      float(this->scene_box_.max_z()));

  // return bpgl_heightmap instance
  bpgl_heightmap<float> bh;
  bh.ground_sample_distance(this->params_.ground_sample_dist_);
  bh.heightmap_bounds(scene_box_as_float);
  bh.min_neighbors(this->params_.min_neighbors_);
  bh.max_neighbors(this->params_.max_neighbors_);
  bh.neighbor_dist_factor(this->params_.neighbor_dist_factor_);
  return bh;
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
