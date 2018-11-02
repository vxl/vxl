#include <iostream>
#include <cmath>
#include <sstream>
#include "brip_filter_bank.h"
//
#include <brip/brip_vil_float_ops.h>
#include <brip/brip_vil_ops.h>
#include <vil/vil_resample_bicub.h>
#include <vil/vil_save.h>
#include <vil/vil_load.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vul/vul_file.h>

// cutoff ratio determines Gaussian values that are assumed negligible
static int gauss_radius(float sigma, float cutoff_ratio)
{
  double sigma_sq_inv = 1/(sigma*sigma);
  return static_cast<int>(std::sqrt((-2.0*std::log(cutoff_ratio))/sigma_sq_inv)+0.5);
}

brip_filter_bank::brip_filter_bank(unsigned n_levels,
                                   double scale_range, float lambda0,
                                   float lambda1, float theta_interval,
                                   float cutoff_ratio)
: ni_(0), nj_(0),n_levels_(n_levels), lambda0_(lambda0), lambda1_(lambda1),
  theta_interval_(theta_interval), cutoff_ratio_(cutoff_ratio)
{
  assert(n_levels_);
  if (n_levels == 1) {// only one level in the pyramid
    scale_ratio_ = 1.0;//actually meaningless
  }
  else {
    double ninv = 1.0/(n_levels-1);
    scale_ratio_ = std::pow(scale_range, ninv);
  }
}

void brip_filter_bank::set_params(unsigned n_levels, double scale_range, float lambda0, float lambda1, float theta_interval, float cutoff_ratio)
{
  n_levels_ = n_levels;
  lambda0_ = lambda0; lambda1_ = lambda1;
  theta_interval_ = theta_interval; cutoff_ratio_ = cutoff_ratio;
  assert(n_levels_);
  if (n_levels == 1) {// only one level in the pyramid
    scale_ratio_ = 1.0;//actually meaningless
  }
  else {
    double ninv = 1.0/(n_levels-1);
    scale_ratio_ = std::pow(scale_range, ninv);
  }
}

brip_filter_bank::brip_filter_bank(unsigned n_levels,
                                   double scale_range, float lambda0,
                                   float lambda1, float theta_interval,
                                   float cutoff_ratio,
                                   vil_image_view<float> const& image):
  n_levels_(n_levels), lambda0_(lambda0), lambda1_(lambda1),
  theta_interval_(theta_interval), cutoff_ratio_(cutoff_ratio)
{
  assert(n_levels_);
  ni_ = image.ni();   nj_ = image.nj();
  scale_pyramid_ = vil_pyramid_image_view<float>(image);
  if (n_levels == 1) { // only one level in the pyramid
    scale_ratio_ = 1.0;//actually meaningless
  }
  else {
    double ninv = 1.0/(n_levels-1);
    scale_ratio_ = std::pow(scale_range, ninv);
    this->construct_scale_pyramid();
  }
  this->compute_filter_responses();
}

void brip_filter_bank::set_image(vil_image_view<float> const& image)
{
  ni_ = image.ni();   nj_ = image.nj();
  scale_pyramid_ = vil_pyramid_image_view<float>(image);
  if (scale_ratio_ != 1.0) this->construct_scale_pyramid();
  this->compute_filter_responses();
}

void brip_filter_bank::construct_scale_pyramid()
{
  //smooth with Gaussian sigma given by the scale ratio and downsample
  auto radius =
    static_cast<unsigned>(gauss_radius(static_cast<float>(scale_ratio_), cutoff_ratio_));
  unsigned int n_samples = 2*radius +1;
  std::cout << " \t in brip_filter_bank level: 0 (original) size of img: " << ni_ << " " << nj_ << '\n';
  for (unsigned level = 1; level<n_levels_; ++level) {
    vil_image_view<float> smooth;
    double scale_m1;
    //image from previous level
    vil_image_view<float> level_m1_view = scale_pyramid_.get_view(level-1,
                                                                  scale_m1);
    //smooth image
    brip_gauss_filter(level_m1_view, smooth, scale_ratio_, n_samples,
                      vil_convolve_no_extend);
    // downsample image
    double dni = level_m1_view.ni(), dnj = level_m1_view.nj();
    // image dimensions for next level
    dni /= scale_ratio_;     dnj /= scale_ratio_;
    double scale = scale_m1 / scale_ratio_;
    auto sni = static_cast<unsigned>(dni+0.5);
    auto snj = static_cast<unsigned>(dnj+0.5);
    std::cout << " \t in brip_filter_bank level: " << level << " size of img: " << sni << " " << snj << '\n';
    auto* lview = new vil_image_view<float>();
    vil_image_view_base_sptr lview_ptr = lview;
    vil_resample_bicub(smooth, *lview, sni, snj);
    scale_pyramid_.add_view(lview_ptr, scale);
  }
}

void brip_filter_bank::compute_filter_responses()
{
  filter_responses_.clear();
  filter_responses_.resize(n_levels_);
  std::cout << '(' << lambda0_ << ' ' << lambda1_ << ")\n";
  for (unsigned level = 0; level<n_levels_; ++level) {
    vil_image_view<float>& level_view = scale_pyramid_(level);

    vil_image_view<float> resp =
      brip_vil_float_ops::fast_extrema_rotational(level_view,
                                                  lambda0_,
                                                  lambda1_,
                                                  theta_interval_,
                                                  false,/*bright*/
                                                  false,/*mag*/
                                                  true,/*signed*/
                                                  true,/*scale invariant*/
                                                  false,/*non_max_suppress*/
                                                  cutoff_ratio_);
    //upsample the response to original size
    vil_image_view<float> upsamp_response;
    vil_resample_bicub(resp, upsamp_response, ni_, nj_);
    filter_responses_[level] = upsamp_response;
    std::cout << 1.0/scale_pyramid_.scale(level) << ' ' << std::flush;
  }
  std::cout << '\n' << std::flush;
}

unsigned brip_filter_bank::invalid_border() const
{
  // the radius due to Gauss smoothing
  double g_rad = gauss_radius(static_cast<float>(scale_ratio_), cutoff_ratio_);
  //std::cout << " \t \t filter border: gaussian radius: " << g_rad << std::endl;
  // the radius due to the anisotropic filter
  double f_rad = gauss_radius(lambda0_, cutoff_ratio_);
  //std::cout << " \t \t filter border: filter radius: " << g_rad << std::endl;
  double r = 0.42*std::sqrt(g_rad*g_rad + f_rad*f_rad);//fudge factor
  //std::cout << " \t \t filter border: r: " << r << std::endl;
  double scale_int = std::pow(scale_ratio_, double(n_levels_-1));
  //std::cout << " \t \t filter border: scale int: " << scale_int << std::endl;
  r *= scale_int;
  //std::cout << " \t \t filter border: invalid border: " << r+0.5 << std::endl;
  return static_cast<unsigned>(r+0.5);
}

bool brip_filter_bank::save_filter_responses(std::string const& dir) const
{
  /*if (!vul_file::is_directory(dir)) {
    std::cout << "filepath: " << dir << " is not a directory in brip_filter_bank::save_filter_responses(.)\n";
    return false;
  }*/
  if (!vul_file::exists(dir)) {
    std::cout << "filepath: " << dir << " is not a directory in brip_filter_bank::save_filter_responses(.)\n";
    return false;
  }
  for (unsigned level = 0; level<n_levels_; ++level) {
    std::stringstream str;
    str << "/filter_response_" << level << ".tiff" << std::ends;
    std::string path = dir + str.str();
    vil_save(filter_responses_[level], path.c_str());
  }
  return true;
}
bool brip_filter_bank::load_filter_responses(std::string const& dir, unsigned n_levels)
{
  /*if (!vul_file::is_directory(dir)) {
    std::cout << "filepath is not a directory in brip_filter_bank::save_filter_responses(.)\n";
    return false;
  }*/
  if (!vul_file::exists(dir)) {
    std::cout << "filepath: " << dir << " is not a directory in brip_filter_bank::save_filter_responses(.)\n";
    return false;
  }
  filter_responses_.clear();
  n_levels_ = n_levels;
  for (unsigned level = 0; level<n_levels; ++level) {
    std::stringstream str;
    str << "/filter_response_" << level << ".tiff" << std::ends;
    std::string path = dir + str.str();
    if (!vul_file::exists(path)) {
      std::cout << "file does not exist: " << path << " in brip_filter_bank::load_filter_responses(.)\n";
      return false;
    }
    vil_image_view<float> img = vil_load(path.c_str());
    filter_responses_.push_back(img);
    ni_ = img.ni();
    nj_ = img.nj();
  }
  return true;
}


std::ostream&  operator<<(std::ostream& s, brip_filter_bank const& r)
{
  s << "size:(" << r.ni() << ' ' << r.nj() << ") n_levels:" << r.n_levels()
    << " scale ratio:" << r.scale_ratio() << '\n'
    << "lambda0:" << r.lambda0() << " lambda1:" << r.lambda1()
    << " theta_interval:" << r.theta_interval() << '\n';
  return s;
}
