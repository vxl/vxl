#include <fstream>

#include "vgl/vgl_box_3d.h"
#include "vil/vil_load.h"
#include "vil/vil_convert.h"
#include "vil/vil_bilin_interp.h"
#include "vil/vil_bicub_interp.h"
#include "vnl/vnl_random.h"
#include "vnl/vnl_det.h"
#include "vnl/vnl_math.h"
#include "vnl/vnl_inverse.h"

#include <vpgl/algo/vpgl_camera_convert.h>
#include <vpgl/algo/vpgl_equi_rectification.h>
#include "bpgl_rectify_image_pair.h"
#include <vpgl/vpgl_fundamental_matrix.h>
#include <vpgl/vpgl_affine_fundamental_matrix.h>


template <class CAMT>
void bpgl_rectify_image_pair<CAMT>::
set_valid_types()
{
  valid_cam_types_.push_back("vpgl_affine_camera");
  valid_cam_types_.push_back("vpgl_perspective_camera");
}


template <class CAMT>
bool bpgl_rectify_image_pair<CAMT>::
valid_type(std::string const& type)
{
  for (size_t i = 0; i < valid_cam_types_.size(); ++i)
    if (type == valid_cam_types_[i])
      return true;
  return false;
}


template <class CAMT>
void bpgl_rectify_image_pair<CAMT>::
set_images(vil_image_view_base_sptr const& view0,
           vil_image_view_base_sptr const& view1)
{
  // convert images to float
  vil_image_view_base_sptr fview0 = vil_convert_cast(float(), view0);
  if (!fview0)
    throw std::runtime_error("can't convert view 0 to float");

  vil_image_view_base_sptr fview1 = vil_convert_cast(float(), view1);
  if (!fview1)
    throw std::runtime_error("can't convert view 1 to float");

  // save to instance
  fview0_ = *fview0;
  fview1_ = *fview1;
}


template <class CAMT>
void bpgl_rectify_image_pair<CAMT>::
set_images(vil_image_resource_sptr const& resc0,
           vil_image_resource_sptr const& resc1)
{
  // get image views
  auto view0 = resc0->get_view();
  if (!view0)
    throw std::runtime_error("null view from resource 0");

  auto view1 = resc1->get_view();
  if (!view1)
    throw std::runtime_error("null view from resource 1");

  // save to instance
  this->set_images(view0, view1);
}


template <class CAMT>
bool bpgl_rectify_image_pair<CAMT>::
load_camera(std::string const& cam_path, CAMT& cam)
{
  std::string ctype = cam.type_name();
  if (!this->valid_type(ctype)) {
    std::cerr << "can't process camera of type " << ctype << std::endl;
    return false;
  }
  std::ifstream istr(cam_path);
  if (!istr) {
    std::cout << "Can't open camera path " << cam_path << std::endl;
    return false;
  }
  istr >> cam;
  return true;
}


template <class CAMT>
void bpgl_rectify_image_pair<CAMT>::
compute_warp_dimensions_offsets(int ni0, int nj0, int ni1, int nj1)
{
  double dni0 = static_cast<double>(ni0 - 1), dnj0 = static_cast<double>(nj0 - 1);
  double dni1 = static_cast<double>(ni1 - 1), dnj1 = static_cast<double>(nj1 - 1);

  std::vector<vnl_vector_fixed<double, 3> > corners_0(4), corners_1(4);
  std::vector<vnl_vector_fixed<double, 3> > Hcorners_0(4), Hcorners_1(4);

  corners_0[0] = vnl_vector_fixed<double, 3>(0, 0, 1);
  corners_0[1] = vnl_vector_fixed<double, 3>(dni0, 0, 1);
  corners_0[2] = vnl_vector_fixed<double, 3>(dni0, dnj0, 1);
  corners_0[3] = vnl_vector_fixed<double, 3>(0, dnj0, 1);

  corners_1[0] = vnl_vector_fixed<double, 3>(0, 0, 1);
  corners_1[1] = vnl_vector_fixed<double, 3>(dni1, 0, 1);
  corners_1[2] = vnl_vector_fixed<double, 3>(dni1, dnj1, 1);
  corners_1[3] = vnl_vector_fixed<double, 3>(0, dnj1, 1);

  for (size_t c = 0; c < 4; ++c) {
    Hcorners_0[c] =  H0_ * corners_0[c];
    Hcorners_0[c] /= Hcorners_0[c][2];
    Hcorners_1[c] =  H1_ * corners_1[c];
    Hcorners_1[c] /= Hcorners_1[c][2];
  }

  double mini0 = std::numeric_limits<double>::max(), maxi0 = -mini0;
  double minj0 = std::numeric_limits<double>::max(), maxj0 = -minj0;
  double mini1 = std::numeric_limits<double>::max(), maxi1 = -mini1;
  double minj1 = std::numeric_limits<double>::max(), maxj1 = -minj1;

  for (size_t c = 0; c<4; ++c) {
    if (Hcorners_0[c][0] < mini0)
      mini0 = Hcorners_0[c][0];
    if (Hcorners_1[c][0] < mini1)
      mini1 = Hcorners_1[c][0];
    if (Hcorners_0[c][0] > maxi0)
      maxi0 = Hcorners_0[c][0];
    if (Hcorners_1[c][0] > maxi1)
      maxi1 = Hcorners_1[c][0];
    if (Hcorners_0[c][1] < minj0)
      minj0 = Hcorners_0[c][1];
    if (Hcorners_1[c][1] < minj1)
      minj1 = Hcorners_1[c][1];
    if (Hcorners_0[c][1] > maxj0)
      maxj0 = Hcorners_0[c][1];
    if (Hcorners_1[c][1] > maxj1)
      maxj1 = Hcorners_1[c][1];
  }

  double w0 = (maxi0 - mini0), h0 = (maxj0 - minj0);
  double w1 = (maxi1 - mini1), h1 = (maxj1 - minj1);
  double w = w0, h = h0;
  du_off_ = mini0; dv_off_ = minj0;
  if (w1 < w) {
    w = w1;
    du_off_ = mini1;
  }
  if (h1 < h) {
    h = h1;
    dv_off_ = minj1;
  }

  //double scaled_w = w*params_.upsample_scale_, scaled_h = h*params_.upsample_scale_;
  // replaced by scale_to_input_size()
  out_ni_ = static_cast<size_t>(w + 0.5) + 1;
  out_nj_ = static_cast<size_t>(h + 0.5) + 1;
}


template <class CAMT>
double bpgl_rectify_image_pair<CAMT>::
scale_to_input_size(int ni0, int nj0,
                    int ni1, int nj1)
{
  this->compute_warp_dimensions_offsets(ni0, nj0, ni1, nj1);
  double w0 = std::max(ni0, ni1);
  double h0 = std::max(nj0, nj1);
  double sw = w0 / out_ni_;
  double sh = h0 / out_nj_;
  // 1.2 is geometric mean of 1 and sqrt(2)(45deg rotation)
  // as a compromise scale for a rotated image
  double s = 1.2 * params_.upsample_scale_ * std::max(sw, sh);
  out_ni_ *= s;
  out_nj_ *= s;
  return s;
}


template <class CAMT>
void bpgl_rectify_image_pair<CAMT>::
compute_rectification(vgl_box_3d<double>const& scene_box,
                      int ni0, int nj0,
                      int ni1, int nj1)
{

  // if explicit sizes not given, then look to see if we can get that information from loaded images
  if (ni0 == -1) {
    if (fview0_.size() > 0) {
      ni0 = fview0_.ni();
      nj0 = fview0_.nj();
    } else {
      throw std::runtime_error("compute_rectification called without explicit "
                               "image 0 size given, and image 0 is not set");
    }

    if (fview1_.size() > 0) {
      ni1 = fview1_.ni();
      nj1 = fview1_.nj();
    } else {
      throw std::runtime_error("compute_rectification called without explicit "
                               "image 1 size given, and image 1 is not set");
    }
  }

  // image dimensions
  double dni0 = static_cast<double>(ni0), dnj0 = static_cast<double>(nj0);
  double dni1 = static_cast<double>(ni1), dnj1 = static_cast<double>(nj1);

  // scene bounds
  double min_x = scene_box.min_x();
  double min_y = scene_box.min_y();
  double width = scene_box.width(), height = scene_box.height();

  // min disparity Z plane elevation
  double z0 = 0.5 * (scene_box.min_z() + scene_box.max_z());
  if (vnl_math::isfinite(params_.min_disparity_z_))
    z0 = params_.min_disparity_z_;

  vnl_random rng;
  std::vector<vnl_vector_fixed<double, 3> > img_pts0, img_pts1;
  double inside_pts = 0.0;
  for (unsigned i = 0; i < params_.n_points_; i++) {
    double x = rng.drand64() * width + min_x;  // sample in local coords
    double y = rng.drand64() * height + min_y;
    double u0, v0, u1, v1;

    cam0_.project(x, y, z0, u0, v0);
    bool proj_0_good = (u0 >= 0 && u0 < ni0 && v0 >= 0 && v0 < nj0);

    cam1_.project(x, y, z0, u1, v1);
    bool proj_1_good = (u1 >= 0 && u1 < ni1 && v1 >= 0 && v1 < nj1);

    // store points regardless of if they project into each image
    img_pts0.emplace_back(u0, v0, 1);
    img_pts1.emplace_back(u1, v1, 1);

    // but accumulate the points that intersect each image
    if (proj_0_good && proj_1_good)
      inside_pts += 1.0;
  }

  // if too few points have both images in common - problem
  double overlap_fraction = double(inside_pts) / double(params_.n_points_);
  if (overlap_fraction < params_.min_overlap_fraction_) {
    std::ostringstream buffer;
    buffer << "Fatal - an insufficient fraction of zero disparity plane points project into both images "
           << overlap_fraction << " < " << params_.min_overlap_fraction_ << std::endl;
    throw std::runtime_error(buffer.str());
  }

  // sanity check
  bool epi_constraint = true;
  vnl_matrix_fixed<double, 3, 3> Fm;
  if (cam0_.type_name() == "vpgl_perspective_camera") {
    vnl_matrix_fixed<double, 3, 4> m0 = cam0_.get_matrix(), m1 = cam1_.get_matrix();
    vpgl_proj_camera<double> pc0(m0), pc1(m1);
    vpgl_fundamental_matrix<double> Fp(pc0, pc1);
    Fm = Fp.get_matrix();
  } else if (cam0_.type_name() == "vpgl_affine_camera") {
    vnl_matrix_fixed<double, 3, 4> m0 = cam0_.get_matrix(), m1 = cam1_.get_matrix();
    vpgl_affine_camera<double> A0(m0), A1(m1);
    vpgl_affine_fundamental_matrix<double> Fa(A0, A1);
    Fm = Fa.get_matrix();
  } else {
    std::ostringstream buffer;
    buffer << "Can't rectify image camera of type " << cam0_.type_name() << std::endl;
    throw std::runtime_error(buffer.str());
  }
  for (size_t k = 0; k < img_pts0.size(); ++k) {
    vnl_vector_fixed<double, 3> pr = img_pts0[0], line_l, pl = img_pts1[0];
    line_l = Fm * pr;
    // normalize line
    line_l /= sqrt(line_l[0] * line_l[0] + line_l[1] * line_l[1]);
    double dp = dot_product(line_l, pl);
    epi_constraint = epi_constraint && fabs(dp) < 1.0e-6;
  }
  if (!epi_constraint) {
    throw std::runtime_error("epipolar constraint doesn't hold for F matrix and sampled points");
  }

  if (cam0_.type_name() == "vpgl_affine_camera") {
    vnl_matrix_fixed<double, 3, 4> m0 = cam0_.get_matrix(), m1 = cam1_.get_matrix();
    vpgl_affine_camera<double> A0(m0), A1(m1);
    if (!vpgl_equi_rectification::rectify_pair(A0, A1, img_pts0, img_pts1, H0_, H1_)) {
      throw std::runtime_error("vpgl equi rectification failed");
    }
  } else if (cam0_.type_name() == "vpgl_perspective_camera") {
    if (!vpgl_equi_rectification::rectify_pair(cam0_, cam1_, img_pts0, img_pts1, H0_, H1_)) {
      throw std::runtime_error("vpgl equi rectification failed");
    }
  }
  double singular_tol = 1.0e-6;
  if ((fabs(vnl_det(H0_)) < singular_tol) ||
      (fabs(vnl_det(H1_)) < singular_tol)) {
    throw std::runtime_error("vpgl rectification produced singular homography(s)");
  }

  // second sanity check
  bool equal_y = true;
  for (size_t k = 0; k < img_pts0.size()&&equal_y; ++k) {
    vnl_vector_fixed<double, 3> p0 = img_pts0[k], hp0, p1 = img_pts1[k], hp1;
    hp0 = H0_ * p0; hp1 = H1_ * p1;
    double y0 = hp0[1] / hp0[2], y1 = hp1[1] / hp1[2];
    double dy = fabs(y1 - y0);
    if (dy > 0.001)
      equal_y = false;
  }
  if (!equal_y) {
    throw std::runtime_error("homographies do not map to equal row positions");
  }

  // third sanity check
  double x_dif_sum = 0.0;
  for (size_t k = 0; k < img_pts0.size(); ++k) {
    vnl_vector_fixed<double, 3> p0 = img_pts0[k], hp0, p1 = img_pts1[k], hp1;
    hp0 = H0_ * p0; hp1 = H1_ * p1;
    double x0 = hp0[0] / hp0[2], x1 = hp1[0] / hp1[2];
    double dx = x1 - x0;
    x_dif_sum += dx;
  }
  x_dif_sum /= img_pts0.size();
  bool x_shift_min = fabs(x_dif_sum) < 0.1;
  if (!x_shift_min) {
    throw std::runtime_error("homographies do not minimize column shift");
  }
  double sf = this->scale_to_input_size(ni0, nj0, ni1, nj1);
  vnl_matrix_fixed<double, 3, 3> tr,sc;
  tr.set_identity();
  tr[0][2] = -du_off_; tr[1][2] = -dv_off_;
  H0_ = tr * H0_;
  H1_ = tr * H1_;
  sc.set_identity();
  sc[0][0] = sf; sc[1][1] = sc[0][0];
  H0_ = sc * H0_;
  H1_ = sc * H1_;
}


template <class CAMT>
CAMT bpgl_rectify_image_pair<CAMT>::
rectify_camera(CAMT cam,
               vnl_matrix_fixed<double, 3, 3> H)
{
  vnl_matrix_fixed<double, 3, 4> M = cam.get_matrix();
  M = H * M;
  CAMT rect_cam;
  rect_cam.set_matrix(M);
  return rect_cam;
}


template <class CAMT>
vgl_box_2d<int> bpgl_rectify_image_pair<CAMT>::
rectify_window(vgl_box_2d<int> const& window,
               vnl_matrix_fixed<double, 3, 3> H,
               size_t ni, size_t nj,
               int padding)
{
  if (window.is_empty()) {
    return vgl_box_2d<int>();
  }

  // corners of window (homogeneous coordinates)
  vnl_vector_fixed<double, 3> ll_corner(window.min_x(), window.min_y(), 1);
  vnl_vector_fixed<double, 3> ur_corner(window.max_x(), window.max_y(), 1);

  // apply homography to corners
  vnl_vector_fixed<double, 3> ll_corner_rect = H * ll_corner;
  vnl_vector_fixed<double, 3> ur_corner_rect = H * ur_corner;

  // homogeneous -> cartesian coordinates
  double ll_corner_rect_x = ll_corner_rect[0] / ll_corner_rect[2];
  double ll_corner_rect_y = ll_corner_rect[1] / ll_corner_rect[2];
  double ur_corner_rect_x = ur_corner_rect[0] / ur_corner_rect[2];
  double ur_corner_rect_y = ur_corner_rect[1] / ur_corner_rect[2];

  // axis-aligned bounding box around rectified corners
  double min_x_rect_d = std::min(ll_corner_rect_x, ur_corner_rect_x);
  double min_y_rect_d = std::min(ll_corner_rect_y, ur_corner_rect_y);
  double max_x_rect_d = std::max(ll_corner_rect_x, ur_corner_rect_x);
  double max_y_rect_d = std::max(ll_corner_rect_y, ur_corner_rect_y);

  // snap to pixel
  int min_x_rect = static_cast<int>(std::floor(min_x_rect_d));
  int min_y_rect = static_cast<int>(std::floor(min_y_rect_d));
  int max_x_rect = static_cast<int>(std::ceil(max_x_rect_d));
  int max_y_rect = static_cast<int>(std::ceil(max_y_rect_d));

  // expand by padding
  min_x_rect -= padding;
  min_y_rect -= padding;
  max_x_rect += padding;
  max_y_rect += padding;

  // clip to image bounds
  min_x_rect = std::max<int>(min_x_rect, 0);
  min_y_rect = std::max<int>(min_y_rect, 0);
  max_x_rect = std::min<int>(max_x_rect, ni);
  max_y_rect = std::min<int>(max_y_rect, nj);

  // degenerate case
  if (min_x_rect >= max_x_rect || min_y_rect >= max_y_rect) {
    /* throw std::runtime_error("rectified window is unusable"); */
    return vgl_box_2d<int>();
  }

  return vgl_box_2d<int>(min_x_rect, max_x_rect, min_y_rect, max_y_rect);
}


// provide the possibility of NAN as an invalid pixel value. Useful for subsequent
// processing to avoid the invalid warp regions in the rectified image.
template <class CAMT>
void bpgl_rectify_image_pair<CAMT>::
warp_image(vil_image_view<float> fview,
           vnl_matrix_fixed<double, 3, 3> const& H,
           vil_image_view<float>& fwarp,
           size_t out_ni, size_t out_nj,
           vgl_box_2d<int> const& rectified_window,
           size_t margin)  // margin needed for kernels
{
  size_t img_start_i, img_start_j, img_end_i, img_end_j;
  if (rectified_window.is_empty()) {
    img_start_i = 0;
    img_start_j = 0;
    img_end_i = out_ni;
    img_end_j = out_nj;
  }
  else {
    // pad the window by a margin (so we have image data outside the window for kernels)
    img_start_i = rectified_window.min_x() - margin;
    img_start_j = rectified_window.min_y() - margin;
    img_end_i = rectified_window.max_x() + margin;
    img_end_j = rectified_window.max_y() + margin;

    // clip to image bounds
    img_start_i = std::max<size_t>(0, img_start_i);
    img_start_j = std::max<size_t>(0, img_start_j);
    img_end_i = std::min<size_t>(out_ni, img_end_i);
    img_end_j = std::min<size_t>(out_nj, img_end_j);
  }

  size_t ni = fview.ni(), nj = fview.nj();
  double dni = static_cast<double>(ni);
  double dnj = static_cast<double>(nj);
  fwarp.set_size(out_ni, out_nj);
  fwarp.fill(params_.invalid_pixel_val_);
  vnl_matrix_fixed<double, 3, 3> Hinv = vnl_inverse(H);
  vnl_vector_fixed<double, 3> opix(0.0, 0.0, 1), hopix;
  hopix = Hinv * opix;
  vnl_vector_fixed<double, 3> mpix(out_ni, out_nj, 1), hmpix;
  hmpix = Hinv * mpix;

  for (size_t j = img_start_j; j < img_end_j; ++j) {
    for (size_t i = img_start_i; i < img_end_i; ++i) {
      double du = static_cast<double>(i);
      double dv = static_cast<double>(j);
      vnl_vector_fixed<double, 3> pix(du, dv, 1), hpix;
      hpix = Hinv * pix;
      double du_h = hpix[0] / hpix[2], dv_h = hpix[1] / hpix[2];
      if (du_h < 0.0 || du_h >= dni || dv_h < 0.0 || dv_h >= dnj)
        continue;
      //float fval = vil_bilin_interp_safe_extend(fview, du_h, dv_h);
      float fval = vil_bicub_interp_safe_extend(fview, du_h, dv_h);
      fwarp(i, j) = fval;
    }
  }
}


// requires calling compute_rectification first
template <class CAMT>
void bpgl_rectify_image_pair<CAMT>::rectify_window_pair(
    vgl_box_2d<int> const& target_window,
    int min_disparity,
    int max_disparity)
{

  if (target_window.is_empty())
    return;

  // rectify the target window
  rect_window0_ = this->rectify_window(target_window, H0_, out_ni_,
                                       out_nj_, params_.window_padding_);

  // estimate the reference window by expanding the target window using the
  // min and max disparities.
  int min_x = rect_window0_.min_x() + min_disparity;
  int max_x = rect_window0_.max_x() + max_disparity;
  int min_y = rect_window0_.min_y();
  int max_y = rect_window0_.max_y();

  // clip to image bounds
  min_x = std::max<int>(0, min_x);
  max_x = std::min<int>(out_ni_, max_x);

  rect_window1_ = vgl_box_2d<int>(min_x, max_x, min_y, max_y);
}


template <class CAMT>
void bpgl_rectify_image_pair<CAMT>::warp_image_pair()
{
  // warp first image into rect_fview0_
  this->warp_image(fview0_, H0_, rect_fview0_, out_ni_, out_nj_, rect_window0_, params_.kernel_margin_);

  // warp second image into rect_fview1_
  this->warp_image(fview1_, H1_, rect_fview1_, out_ni_, out_nj_, rect_window1_, params_.kernel_margin_);
}


// Code for easy instantiation.
#undef BPGL_RECTIFY_IMAGE_PAIR_INSTANTIATE
#define BPGL_RECTIFY_IMAGE_PAIR_INSTANTIATE(CAMT) \
template class bpgl_rectify_image_pair<CAMT>
