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

#include <vil/vil_image_view_base.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_blocked_image_resource_sptr.h>
#include <bsta/bsta_histogram.h>
#include <bgui/bgui_graph_tableau.h>
#include <vgui/vgui_range_map_params_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>
class bgui_image_utils
{
 public:
  //:constructors
  bgui_image_utils();

  bgui_image_utils(vil_image_resource_sptr const& image,
                   vsol_polygon_2d_sptr const& poly = nullptr);

  ~bgui_image_utils(){}

  //: Set the image on which properties are computed
  void set_image(vil_image_resource_sptr const& image);
  void set_poly(vsol_polygon_2d_sptr const& poly);

  void set_bin_limit( unsigned bin_limit){ bin_limit_ = bin_limit;}
  void set_n_skip_upper_bins(unsigned n_skip){n_skip_upper_bins_=n_skip;}
  void set_n_skip_lower_bins(unsigned n_skip){n_skip_lower_bins_=n_skip;}
  void set_percent_limit(double limit){percent_limit_ = limit;}
  void set_min_blocks(unsigned min_blocks){min_blocks_ = min_blocks;}
  void set_scan_fraction(double scan_fraction){scan_fraction_ = scan_fraction;}

  bool range(double& min_value, double& max_value, unsigned plane = 0);
  bsta_histogram<double> histogram(unsigned plane = 0)
  { if (!hist_valid_)this->construct_histogram(); return hist_[plane]; }
  bgui_graph_tableau_sptr hist_graph();

  //: Default range map based on default max min values
  bool default_range_map(vgui_range_map_params_sptr& rmp,
                         double gamma = 1.0, bool invert = false,
                         bool gl_map = false, bool cache=true);

  //: Range map computed from the image histogram
  bool range_map_from_hist(float gamma, bool invert,
                           bool gl_map, bool cache,
                           vgui_range_map_params_sptr& rmp);

 private:
  //private utilities

  bool init_histogram_from_data();

  bool set_data_from_view(vil_image_view_base_sptr const& view,
                          double fraction = 1.0);

  bool set_data_by_random_blocks(const unsigned total_num_blocks,
                                 vil_blocked_image_resource_sptr const& bir,
                                 double fraction = 1.0);
  bool set_data_inside_polygon(vil_blocked_image_resource_sptr const& bir);

  bool construct_histogram();
  double compute_lower_bound(unsigned plane = 0);
  double compute_upper_bound(unsigned plane = 0);
  //members

  bool hist_valid_;
  double percent_limit_;
  unsigned bin_limit_;
  unsigned n_skip_upper_bins_;
  unsigned n_skip_lower_bins_;
  unsigned min_blocks_;
  double scan_fraction_;

  vil_image_resource_sptr image_;
  vsol_polygon_2d_sptr poly_;
  std::vector<bsta_histogram<double> > hist_;
  std::vector<std::vector<double> > data_;
};


#endif // bgui_image_utils_h_
