// This is brl/bseg/strk/strk_info_tracker_params.h
#ifndef strk_info_tracker_params_h_
#define strk_info_tracker_params_h_
//:
// \file
// \brief parameter mixin for strk_info_tracker
//
// \author
//    Joseph L. Mundy - August. 20, 2003
//    Brown University
//
//-----------------------------------------------------------------------------
#include <gevd/gevd_param_mixin.h>
#include <vcl_iosfwd.h>

class strk_info_tracker_params : public gevd_param_mixin
{
 public:
  strk_info_tracker_params(const int n_samples = 10,
                           const float search_radius = 5.0,
                           const float angle_range = 0.0,
                           const float scale_range = 0.0,
                           const float sigma = 1.0,
                           const bool gradient_info = true,
                           const bool color_info = false,
                           const float min_gradient = 0,
                           const float parzen_sigma = 0,
                           const unsigned int intensity_hist_bins = 16,
                           const unsigned int gradient_dir_hist_bins = 8,
                           const unsigned int color_hist_bins = 8,
                           const bool use_background = false,
                           const bool renyi_joint_entropy = false,
                           const bool verbose = false,
                           const bool debug = false);

  strk_info_tracker_params(const strk_info_tracker_params& old_params);
 ~strk_info_tracker_params() {}

  bool SanityCheck();
  friend
    vcl_ostream& operator<<(vcl_ostream& os,const strk_info_tracker_params& tp);
 protected:
  void InitParams(int n_samples,
                  float search_radius,
                  float angle_range,
                  float scale_range,
                  float sigma,
                  bool gradient_info,
                  bool color_info,
                  float min_gradient,
                  float parzen_sigma,
                  unsigned int intensity_hist_bins,
                  unsigned int gradient_dir_hist_bins,
                  unsigned int color_hist_bins,
                  bool use_background,
                  bool renyi_joint_entropy,
                  bool verbose,
                  bool debug);
 public:
  //
  // Parameter blocks and parameters
  //
  int n_samples_;           //!< number of samples generated per iteration
  float search_radius_;     //!< window size for generating samples
  float angle_range_;       //!< angle range for generating samples
  float scale_range_;       //!< scale range for generating samples
  float sigma_;             //!< smoothing kernel radius for estimating gradient
  bool gradient_info_;      //!< Combine gradient and intensity info
  bool color_info_;         //!< Combine color and intensity info
  float min_gradient_;      //!< minimum gradient magnitude to be considered
  float parzen_sigma_;      //!< smoothing for the histogram density
  unsigned int intensity_hist_bins_; //!< number of intensity histogram bins
  unsigned int gradient_dir_hist_bins_;//!< number of grad dir histogram bins
  unsigned int color_hist_bins_;//!< number of color histogram bins
  bool use_background_;     //!< employ a background model
  bool renyi_joint_entropy_;//!< use renyi entropy for joint distributions
  bool verbose_;            //!< informative messages to cout
  bool debug_;              //!< informative debug messages to cout
};

#endif // strk_info_tracker_params_h_
