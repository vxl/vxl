#ifndef icam_minimizer_params_h_
#define icam_minimizer_params_h_
//:
// \file
#include <string>
#include <iostream>
#include <utility>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class icam_minimizer_params
{
 public:
  //: constructor, default in case no parameters given
  icam_minimizer_params(unsigned nbins=16,
                        unsigned min_level_size=16,
                        unsigned box_reduction_k=2,
                        double axis_search_cone_multiplier=10.0,
                        double polar_range_multiplier=2.0,
                        double local_min_thresh=0.005,
                        double smooth_sigma=1.0,
                        std::string  base_path="")
                       : nbins_(nbins), min_level_size_(min_level_size),box_reduction_k_(box_reduction_k),
                         local_min_thresh_(local_min_thresh), smooth_sigma_(smooth_sigma),
                         axis_search_cone_multiplier_(axis_search_cone_multiplier),
                         polar_range_multiplier_(polar_range_multiplier),
                         base_path_(std::move(base_path)) {}

  //: copy constructor
  icam_minimizer_params(const icam_minimizer_params& p)
    : nbins_(p.nbins_), min_level_size_(p.min_level_size_),box_reduction_k_(p.box_reduction_k_),
      local_min_thresh_(p.local_min_thresh_), smooth_sigma_(p.smooth_sigma_),
      axis_search_cone_multiplier_(p.axis_search_cone_multiplier_),
      polar_range_multiplier_(p.polar_range_multiplier_),
       base_path_(p.base_path_) {}

  icam_minimizer_params& operator=(const icam_minimizer_params& p) = default;

  void print() {
    std::cout << "====== Minimizer Parameters =======\n"
             << "min pyramid dimension " << min_level_size_ << '\n'
             << "trans box reduction "   << box_reduction_k_ << '\n'
             << "axis_search_mulitplier " << axis_search_cone_multiplier_ << '\n'
             << "polar_searh_multiplier " << polar_range_multiplier_ << '\n'
             << "local_min_thresh " << local_min_thresh_ << '\n'
             << "smooth sigma " << smooth_sigma_ << '\n'
             << "histogram bins " << nbins_ << '\n'
             << "===================================\n";
  }

  //public parameters
  unsigned nbins_;
  unsigned min_level_size_;
  unsigned box_reduction_k_;
  double local_min_thresh_;
  double smooth_sigma_;
  double axis_search_cone_multiplier_;
  double polar_range_multiplier_;
  std::string base_path_;
};

#endif
