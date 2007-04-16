// This is brl/bbas/bgui/bgui_image_utils.h
#ifndef bgui_image_utils_h_
#define bgui_image_utils_h_
//:
// \file
// \author  Joseph Mundy
// \brief Utilities for computing image properties used in display
// Currently only implemented for unsigned grey scale images,
//  i.e., uchar, ushort, uint.
// \verbatim
//  Modifications
//   JLM   Sept. 28, 2006  Initial version.
// \endverbatim

#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_blocked_image_resource_sptr.h>
#include <bsta/bsta_histogram.h>
#include <bgui/bgui_graph_tableau.h>

class bgui_image_utils
{
 public:
  //:constructors
  bgui_image_utils(): hist_valid_(false), percent_limit_(0.05),
    n_skip_upper_bins_(0), n_skip_lower_bins_(1),image_(0),
    hist_(bsta_histogram<double>(0.0,0.0,1)){}

  bgui_image_utils(vil_image_resource_sptr const& image):
    hist_valid_(false),percent_limit_(0.05), n_skip_upper_bins_(0),
    n_skip_lower_bins_(1), image_(image),
    hist_(bsta_histogram<double>(0.0,0.0,1)){}
  ~bgui_image_utils(){}
  //: Set the image on which properties are computed
  void set_image(vil_image_resource_sptr const& image){image_ = image;}
  void set_n_skip_upper_bins(const unsigned n_skip){n_skip_upper_bins_=n_skip;}
  void set_n_skip_lower_bins(const unsigned n_skip){n_skip_lower_bins_=n_skip;}
  void set_percent_limit(const double limit){percent_limit_ = limit;}
  void range(double& min_value, double& max_value);
  bsta_histogram<double> histogram()
  { if (!hist_valid_)this->construct_histogram(); return hist_; }
  bgui_graph_tableau_sptr hist_graph();
 private:
  //private utilities
  bool init_histogram();
  void set_hist_from_view(vil_image_view_base_sptr const& view);
  void set_hist_by_random_blocks(const unsigned total_num_blocks,
                                 vil_blocked_image_resource_sptr const& bir);
  void construct_histogram();
  double compute_lower_bound();
  double compute_upper_bound();
  //members
  bool hist_valid_;
  double percent_limit_;
  unsigned n_skip_upper_bins_;
  unsigned n_skip_lower_bins_;
  vil_image_resource_sptr image_;
  bsta_histogram<double> hist_;
};


#endif // bgui_image_utils_h_
