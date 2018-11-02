// This is brl/bbas/bil/bil_image_bounds_from_histogram.h
#ifndef bil_image_bounds_from_histogram_h_
#define bil_image_bounds_from_histogram_h_
//:
// \file
// \brief Compute image bounds from histograms (handling large images with random blocks)
// \author J.L. Mundy
// \date July 25, 2016

#include <vector>
#include <vil/vil_image_resource.h>
#include <vil/vil_blocked_image_resource_sptr.h>
#include <bsta/bsta_histogram.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_polygon.h>
class bil_image_bounds_from_histogram{
 public:
 bil_image_bounds_from_histogram():imgr_(nullptr), bin_limit_(1000), n_skip_upper_bins_(0),
    n_skip_lower_bins_(1), min_blocks_(50), scan_fraction_(0.005), np_(1){}

 bil_image_bounds_from_histogram(vil_image_resource_sptr const& imgr):
  imgr_(imgr),bin_limit_(1000), n_skip_upper_bins_(0),
    n_skip_lower_bins_(1), min_blocks_(50), scan_fraction_(0.005){this->init();}

 bil_image_bounds_from_histogram(vil_image_resource_sptr const& imgr,
                                 unsigned bin_limit, unsigned n_skip_lower,
                                 unsigned n_skip_upper, unsigned min_blocks, double scan_fraction):
  imgr_(imgr), bin_limit_(bin_limit), n_skip_lower_bins_(n_skip_lower),
    n_skip_upper_bins_(n_skip_upper),min_blocks_(min_blocks),scan_fraction_(scan_fraction)
  {this->init();}

  void set_image(vil_image_resource_sptr const& imgr){imgr_ = imgr; this->init();}
  void set_polygon(vgl_polygon<double> const& poly){poly_ = poly;}
  void set_bin_skip(unsigned n_skip_lower_bins, unsigned n_skip_upper_bins){
    n_skip_lower_bins_ = n_skip_lower_bins;
    n_skip_upper_bins_ = n_skip_upper_bins;}
  void set_bin_limit(unsigned bin_limit){bin_limit_ = bin_limit;}
  void set_min_blocks(unsigned min_blocks){min_blocks_ = min_blocks;}
  void set_scan_fraction(double scan_fraction){scan_fraction_ = scan_fraction;}
  bool construct_histogram();
  double lower_bound(unsigned color_plane, double percent_limit) const;
  double upper_bound(unsigned color_plane, double percent_limit) const;
  bsta_histogram<double> histogram(unsigned color_plane) const{
    if(color_plane<np_) return hists_[color_plane];
    return bsta_histogram<double>(0.0, 0.0, 1);
  }
 private:
  void init();
  bool init_histogram_from_data();
  bool set_data_inside_polygon(vil_blocked_image_resource_sptr const& bir);
  bool set_data_from_view(vil_image_view_base_sptr const& view,
                          double fraction = 1.0);
  bool set_data_by_random_blocks(const unsigned total_num_blocks,
                                 vil_blocked_image_resource_sptr const& bir,
                                 double fraction);
  unsigned np_;
  double percent_limit_;
  unsigned bin_limit_;
  unsigned n_skip_upper_bins_;
  unsigned n_skip_lower_bins_;
  unsigned min_blocks_;
  double scan_fraction_;
  //number of histograms determined by the number of color planes
  std::vector<bsta_histogram<double> > hists_;
  vil_image_resource_sptr imgr_;
  vgl_polygon<double> poly_;
  std::vector<std::vector<double> > data_;
};

#endif // bil_image_bounds_from_histogram_h_
