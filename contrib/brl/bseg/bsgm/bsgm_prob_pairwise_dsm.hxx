// This is brl/bseg/bsgm/bsgm_prob_pairwise_dsm.cxx

#include "bsgm_prob_pairwise_dsm.h"
#include "bsgm_error_checking.h"
#include "bsgm_multiscale_disparity_estimator.h"
#include <algorithm>
#include <cmath>
#include <type_traits>
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

#define debug_print false


// ----------
// Rectification
// ----------
template <class CAM_T, class PIX_T>
void bsgm_prob_pairwise_dsm<CAM_T, PIX_T>::rectify_windows()
{
  // target window -> rectified target + reference windows
  if (!target_window_.is_empty()) {
    rip_.rectify_window_pair(target_window_, min_disparity_, max_disparity_);
    rect_target_window_ = rip_.rect_window0();
    rect_reference_window_ = rip_.rect_window1();
  }
}


template <class CAM_T, class PIX_T>
void bsgm_prob_pairwise_dsm<CAM_T, PIX_T>::rectify()
{
  // set parameters
  rectify_params rp;
  rp.min_disparity_z_ = mid_z_;
  rp.upsample_scale_ = params_.upsample_scale_factor_;
  rp.min_overlap_fraction_ = params_.min_overlap_fraction_;
  rp.window_padding_ = params_.window_padding_;
  // census requires a margin for its kernel, and xgrad requires a margin of 1
  rp.kernel_margin_ = std::max<size_t>(params_.de_params_.census_rad, 1);
  rip_.set_params(rp);

  // if rectification matrices weren't precomputed, compute them now
  if (std::isnan(H0_(0, 0)) || std::isnan(H1_(0, 0)) || rect_ni_ == 0 || rect_nj_ == 0) {
    if (debug_print)
      std::cout << "computing rectification again in bsgm_prob_pairwise_dsm::rectify" << std::endl;
    rip_.compute_rectification(scene_box_);
    H0_ = rip_.H0();
    H1_ = rip_.H1();
    std::pair<size_t, size_t> dims_pair = rip_.rectified_dims();
    rect_ni_ = dims_pair.first;
    rect_nj_ = dims_pair.second;
  }
  else {
    // if they were precomputed, then set them
    if (debug_print)
      std::cout << "setting homographies in bsgm_prob_pairwise_dsm::rectify" << std::endl;
    rip_.set_homographies(H0_, H1_, rect_ni_, rect_nj_);
  }

  // rectify cameras for later
  rect_cam0_ = rip_.rectify_camera(cam0_, H0_);
  rect_cam1_ = rip_.rectify_camera(cam1_, H1_);

  // target window -> rectified target + reference windows
  this->rectify_windows();

  // warp images
  rip_.warp_image_pair();
  vil_image_view<float> rect_fview0 = rip_.rectified_fview0();
  vil_image_view<float> rect_fview1 = rip_.rectified_fview1();

  // get range of values in rect_fview0
  int start_i_target, start_j_target, end_i_target, end_j_target;
  if (rect_target_window_.is_empty()) {
    start_i_target = 0;
    start_j_target = 0;
    end_i_target = rect_ni_;
    end_j_target = rect_nj_;
  }
  else {
    start_i_target = rect_target_window_.min_x() - rp.kernel_margin_;
    start_j_target = rect_target_window_.min_y() - rp.kernel_margin_;
    end_i_target = rect_target_window_.max_x() + rp.kernel_margin_;
    end_j_target = rect_target_window_.max_y() + rp.kernel_margin_;

    // clip to image bounds
    start_i_target = std::max<int>(0, start_i_target);
    start_j_target = std::max<int>(0, start_j_target);
    end_i_target = std::min<int>(rect_ni_, end_i_target);
    end_j_target = std::min<int>(rect_nj_, end_j_target);
  }
  float max_0 = 0.0f;
  float f0;
  for (size_t j = start_j_target; j < end_j_target; ++j) {
    for (size_t i = start_i_target; i < end_i_target; ++i) {
      f0 = rect_fview0(i, j);
      // remove small excursions below 0 due to interpolation
      if (f0 < 0.0f)
        rect_fview0(i, j) = 0.0f;
      if (f0 > max_0)
        max_0 = f0;
    }
  }
  if (max_0 == 0.0f) {
    throw std::runtime_error("empty warped target image");
  }

  // get range of values in rect_fview1
  int start_i_reference, start_j_reference, end_i_reference, end_j_reference;
  if (rect_reference_window_.is_empty()) {
    start_i_reference = 0;
    start_j_reference = 0;
    end_i_reference = rect_ni_;
    end_j_reference = rect_nj_;
  }
  else {
    start_i_reference = rect_reference_window_.min_x() - rp.kernel_margin_;
    start_j_reference = rect_reference_window_.min_y() - rp.kernel_margin_;
    end_i_reference = rect_reference_window_.max_x() + rp.kernel_margin_;
    end_j_reference = rect_reference_window_.max_y() + rp.kernel_margin_;

    // clip to image bounds
    start_i_reference = std::max<int>(0, start_i_reference);
    start_j_reference = std::max<int>(0, start_j_reference);
    end_i_reference = std::min<int>(rect_ni_, end_i_reference);
    end_j_reference = std::min<int>(rect_nj_, end_j_reference);
  }
  float max_1 = 0.0f;
  float f1;
  for (size_t j = start_j_reference; j < end_j_reference; ++j) {
    for (size_t i = start_i_reference; i < end_i_reference; ++i) {
      f1 = rect_fview1(i, j);
      // remove small excursions below 0 due to interpolation
      if (f1 < 0.0f)
        rect_fview1(i, j) = 0.0f;
      if (f1 > max_1)
        max_1 = f1;
    }
  }
  if (max_1 == 0.0f) {
    throw std::runtime_error("empty warped reference image");
  }

  // PIX_T == unsigned short
  bool pix_type_short = std::is_same<PIX_T, unsigned short>::value;

  // sanity check
  if(params_.effective_bits_per_pixel_ <=8 && pix_type_short)
    throw std::runtime_error("pixel type and intensity dynamic range inconsistent");

  // stretch range according to bits per pixel
  float max_v = 255.0f;
  if (pix_type_short)  // use range of (0, 2^effective_bpp-1)
    max_v = std::pow(2.0f, params_.effective_bits_per_pixel_) - 1.0f;

  if (debug_print) {
    std::cout << "max_0 = " << max_0 << std::endl;
    std::cout << "max_1 = " << max_1 << std::endl;
    std::cout << "max_v = " << max_v << std::endl;
  }

  // stretch warped image 0
  float scale_0 = max_v / max_0;
  rect_bview0_.set_size(rect_ni_, rect_nj_);
  for (size_t j = start_j_target; j < end_j_target; ++j) {
    for (size_t i = start_i_target; i < end_i_target; ++i) {
      float f0 = rect_fview0(i, j);
      rect_bview0_(i, j) = PIX_T(scale_0 * f0);
    }
  }

  // stretch warped image 1
  float scale_1 = max_v / max_1;
  rect_bview1_.set_size(rect_ni_, rect_nj_);
  for (size_t j = start_j_reference; j < end_j_reference; ++j) {
    for (size_t i = start_i_reference; i < end_i_reference; ++i) {
      float f1 = rect_fview1(i, j);
      rect_bview1_(i, j) = PIX_T(scale_1 * f1);
    }
  }
}


// ----------
// Disparity
// ----------

// compute disparity map from input images
template <class CAM_T, class PIX_T>
void bsgm_prob_pairwise_dsm<CAM_T, PIX_T>::compute_disparity(
    const vil_image_view<PIX_T>& img,
    const vil_image_view<PIX_T>& img_reference,
    bool forward,
    vil_image_view<bool>& invalid,
    vil_image_view<float>& disparity,
    vgl_box_2d<int>& img_window,
    vgl_box_2d<int>& img_reference_window)
{
  vxl_byte border_val = 0;
  float invalid_disp = NAN; //required for triangulation implementation
  bool good = true;
  float dynamic_range_factor = bits_per_pix_factors_[params_.effective_bits_per_pixel_];
  bsgm_compute_invalid_map<PIX_T>(img, img_reference, invalid, min_disparity_,
                                  num_disparities(), border_val, img_window);
  if (params_.coarse_dsm_disparity_estimate_) {
    bsgm_multiscale_disparity_estimator mde(params_.de_params_, rect_ni_, rect_nj_,
                                            num_disparities(), num_active_disparities());

    good = mde.compute(img, img_reference, invalid,
                       min_disparity_, invalid_disp, params_.multi_scale_mode_,
                       disparity, dynamic_range_factor);
    if (!good)
      throw std::runtime_error("Multiscale disparity estimator failed");
  } else {  // use input min_disparity

    // SGM cost volume dimensions (full image size, or window)
    size_t cost_volume_width, cost_volume_height;
    if (img_window.is_empty()) {
      cost_volume_width = img.ni();
      cost_volume_height = img.nj();
    }
    else {
      cost_volume_width = img_window.width();
      cost_volume_height = img_window.height();
    }

    // default min disparity per pixel
    vil_image_view<int> min_disparity(cost_volume_width, cost_volume_height);
    if (forward)  // img = img0, ref = img1
      min_disparity.fill(min_disparity_);
    else  // reverse img = img1, ref = img0
      min_disparity.fill(-(num_disparities() + min_disparity_));

    bsgm_disparity_estimator bsgm(params_.de_params_, cost_volume_width,
                                  cost_volume_height, num_disparities());
    good = bsgm.compute(img, img_reference, invalid, min_disparity,
                        invalid_disp, disparity, dynamic_range_factor,
                        false, img_window, img_reference_window);
    if (!good)
      throw std::runtime_error("disparity estimator failed");
  }
}

// compute forward disparity
template <class CAM_T, class PIX_T>
void bsgm_prob_pairwise_dsm<CAM_T, PIX_T>::compute_disparity_fwd()
{
  bool forward = true;
  compute_disparity(rect_bview0_, rect_bview1_, forward,
                    invalid_map_fwd_, disparity_fwd_,
                    rect_target_window_, rect_reference_window_);
}

// compute reverse disparity
template <class CAM_T, class PIX_T>
void bsgm_prob_pairwise_dsm<CAM_T, PIX_T>::compute_disparity_rev()
{
  bool forward = false;
  compute_disparity(rect_bview1_, rect_bview0_, forward,
                    invalid_map_rev_, disparity_rev_,
                    rect_reference_window_, rect_target_window_);
}


// ----------
// Heightmaps
// ----------

// compute height (tri_3d, ptset, heightmap)
template <class CAM_T, class PIX_T>
void bsgm_prob_pairwise_dsm<CAM_T, PIX_T>::compute_height(const CAM_T& cam, const CAM_T& cam_reference,
                                                   const vil_image_view<float>& disparity,
                                                   vil_image_view<float>& tri_3d, vgl_pointset_3d<float>& ptset,
                                                   vil_image_view<float>& heightmap)
{
  // triangulated image
  tri_3d = bpgl_3d_from_disparity(cam, cam_reference, disparity, params_.disparity_sense_);
  // convert triangulated image to pointset & heightmap
  auto bh = this->get_bpgl_heightmap();
  bh.pointset_from_tri(tri_3d, ptset);
  bh.heightmap_from_pointset(ptset, heightmap);
}

// shift camera to match window
template <class CAM_T, class PIX_T>
void bsgm_prob_pairwise_dsm<CAM_T, PIX_T>::translate_camera_into_window(
    CAM_T& cam, vgl_box_2d<int> window) {

  auto M = cam.get_matrix();
  auto M_shifted = M;  // copy to remove const-ness

  // add shift to each row. multiply by last row to cancel out homogeneous effect
  int u_shift = window.min_x();
  int v_shift = window.min_y();

  M_shifted(0, 0) -= u_shift * M_shifted(2, 0);
  M_shifted(0, 1) -= u_shift * M_shifted(2, 1);
  M_shifted(0, 2) -= u_shift * M_shifted(2, 2);
  M_shifted(0, 3) -= u_shift * M_shifted(2, 3);

  M_shifted(1, 0) -= v_shift * M_shifted(2, 0);
  M_shifted(1, 1) -= v_shift * M_shifted(2, 1);
  M_shifted(1, 2) -= v_shift * M_shifted(2, 2);
  M_shifted(1, 3) -= v_shift * M_shifted(2, 3);

  // adjust camera object's internal matrix
  cam.set_matrix(M_shifted);
}


// compute forward height
template <class CAM_T, class PIX_T>
void bsgm_prob_pairwise_dsm<CAM_T, PIX_T>::compute_height_fwd(bool compute_hmap)
{
  // make copies so we can modify if we need to
  CAM_T rect_cam0_window = rect_cam0_, rect_cam1_window = rect_cam1_;

  // if we're using a window, we shift the cameras to correspond to that window
  // (so the cameras project into the windowed disparity image)
  if (!rect_target_window_.is_empty()) {
    if (debug_print) {
      std::cout << "rect_cam0 = " << rect_cam0_.get_matrix() << std::endl;
      std::cout << "rect_cam1 = " << rect_cam1_.get_matrix() << std::endl;
    }
    translate_camera_into_window(rect_cam0_window, rect_target_window_);
    translate_camera_into_window(rect_cam1_window, rect_target_window_);
    if (debug_print) {
      std::cout << "rect_cam0_ translated = " << rect_cam0_window.get_matrix() << std::endl;
      std::cout << "rect_cam1_ translated = " << rect_cam1_window.get_matrix() << std::endl;
    }
  }

  if (compute_hmap)
    this->compute_height(rect_cam0_window, rect_cam1_window, disparity_fwd_,
                         tri_3d_fwd_, ptset_fwd_, heightmap_fwd_);
  else
    tri_3d_fwd_ = bpgl_3d_from_disparity(rect_cam0_window, rect_cam1_window,
                                         disparity_fwd_, params_.disparity_sense_);
}

// compute reverse height
template <class CAM_T, class PIX_T>
void bsgm_prob_pairwise_dsm<CAM_T, PIX_T>::compute_height_rev(bool compute_hmap)
{
  // make copies so we can modify if we need to
  CAM_T rect_cam0_window = rect_cam0_, rect_cam1_window = rect_cam1_;

  // if we're using a window, we shift the cameras to correspond to that window
  // (so the cameras project into the windowed disparity image)
  if (!rect_reference_window_.is_empty()) {
    translate_camera_into_window(rect_cam0_window, rect_reference_window_);
    translate_camera_into_window(rect_cam1_window, rect_reference_window_);
  }

  if (compute_hmap)
    this->compute_height(rect_cam1_window, rect_cam0_window, disparity_rev_,
                         tri_3d_rev_, ptset_rev_, heightmap_rev_);
  else
    tri_3d_rev_ = bpgl_3d_from_disparity(rect_cam1_window, rect_cam0_window,
                                         disparity_rev_, params_.disparity_sense_);
}


// ----------
// Probabilistic heightmaps
// ----------

// compute probabilistic height (ptset, heightmap, probability)
template <class CAM_T, class PIX_T>
bool bsgm_prob_pairwise_dsm<CAM_T, PIX_T>::compute_prob(bool compute_prob_heightmap)
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
  if (compute_prob_heightmap) {
    auto bh = this->get_bpgl_heightmap();
    //bh.heightmap_from_pointset(prob_ptset_, prob_heightmap_z_, prob_heightmap_prob_);
    bh.heightmap_from_pointset(prob_ptset_, prob_heightmap_z_,
                               prob_heightmap_prob_, radial_std_dev_image_);
  }
  return true;
}


// assumes disparity_fwd, xyz_fwd_ and xyz_rev_ are available
template <class CAM_T, class PIX_T>
void bsgm_prob_pairwise_dsm<CAM_T, PIX_T>::compute_xyz_prob(bool compute_prob_heightmap) {
  int ni = disparity_fwd_.ni(), nj = disparity_fwd_.nj();
  int rev_ni = tri_3d_rev_.ni(), rev_nj = tri_3d_rev_.nj();
  xyz_prob_.set_size(ni, nj, 4);
  xyz_prob_.fill(NAN);
  float sdsq = params_.std_dev_;
  sdsq *= sdsq;
  prob_distr_ = bsta_histogram<float>(0.0f, 1.0f, 100);
  prob_ptset_.clear();
  for (int j = 0; j < nj; ++j) {
    if (j >= rev_nj) continue;
    for (int i = 0; i < ni; ++i) {
      // 3-d point at (i, j)
      float xf = tri_3d_fwd_(i, j, 0), yf = tri_3d_fwd_(i, j, 1), zf = tri_3d_fwd_(i, j, 2);
      if (!vnl_math::isfinite(xf) || !vnl_math::isfinite(yf) || !vnl_math::isfinite(zf))
        continue;
      bool hit = (fabs(xf - 172.799) < 0.01) && (fabs(199.817 - yf) < 0.01);
      // disparity at (i,j)
      int disp = static_cast<int>(disparity_fwd_(i, j));
      if (!vnl_math::isfinite(disp))
        continue;
      int ir = i + disp;
      if (ir < 0 || ir >= ni || ir >= rev_ni)
        continue;
      float xr = tri_3d_rev_(ir, j, 0), yr = tri_3d_rev_(ir, j, 1), zr = tri_3d_rev_(ir, j, 2);
      if (!vnl_math::isfinite(xr) || !vnl_math::isfinite(yr) || !vnl_math::isfinite(zr))
        continue;
      float d = sqrt((xf - xr) * (xf - xr) + (yf - yr) * (yf - yr) + (zf - zr) * (zf - zr));
      float prob = exp(-d * d / sdsq);
      xyz_prob_(i, j, 0) = xf; xyz_prob_(i, j, 1) = yf; xyz_prob_(i, j, 2) = zf;
      xyz_prob_(i, j, 3) = prob;
      prob_distr_.upcount(prob, 1.0);
      prob_ptset_.add_point_with_scalar(vgl_point_3d<float>(xf, yf, zf), prob);
    }
  }
  size_t n = prob_ptset_.size();
  if (n == 0) {
    std::runtime_error("prob_ptset_ is empty");
  }

  // convert pointset to images
  if (compute_prob_heightmap) {
    auto bh = this->get_bpgl_heightmap();
    bh.heightmap_from_pointset(prob_ptset_, prob_heightmap_z_, prob_heightmap_prob_, radial_std_dev_image_);
  }
}


template <class CAM_T, class PIX_T>
void bsgm_prob_pairwise_dsm<CAM_T, PIX_T>::compute_ptset(){
  int ni = disparity_fwd_.ni(), nj = disparity_fwd_.nj();
  prob_distr_ = bsta_histogram<float>(0.0f, 1.0f, 100);
  prob_distr_.upcount(1.0, 1.0);
  prob_ptset_.clear();
  for (int j = 0; j < nj; ++j)
    for (int i = 0; i < ni; ++i) {
      // 3-d point at (i, j)
      float xf = tri_3d_fwd_(i, j, 0), yf = tri_3d_fwd_(i, j, 1), zf = tri_3d_fwd_(i, j, 2);
      if (!vnl_math::isfinite(xf) || !vnl_math::isfinite(yf) || !vnl_math::isfinite(zf))
        continue;
      prob_ptset_.add_point_with_scalar(vgl_point_3d<float>(xf, yf, zf), 1.1f);
    }
}


template <class CAM_T, class PIX_T>
bool bsgm_prob_pairwise_dsm<CAM_T, PIX_T>::z_vs_disparity_scale(double& scale) const
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
template <class CAM_T, class PIX_T>
vil_image_view<vxl_byte> bsgm_prob_pairwise_dsm<CAM_T, PIX_T>::bool_to_byte(
    const vil_image_view<bool>& img) const
{
  vil_image_view<vxl_byte> ret;
  vil_convert_cast(img, ret);
  return ret;
}

// return bpgl_heightmap class
template <class CAM_T, class PIX_T>
bpgl_heightmap<float>
bsgm_prob_pairwise_dsm<CAM_T, PIX_T>::get_bpgl_heightmap() const
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

template <class CAM_T, class PIX_T>
bool bsgm_prob_pairwise_dsm<CAM_T, PIX_T>::save_prob_ptset_color(std::string const& path) const
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

#undef BSGM_PROB_PAIRWISE_DSM_INSTANTIATE
#define BSGM_PROB_PAIRWISE_DSM_INSTANTIATE(CAMT, PIXT) \
template class bsgm_prob_pairwise_dsm<CAMT, PIXT>
