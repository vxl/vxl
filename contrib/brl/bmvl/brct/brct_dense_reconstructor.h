//--*-c++-*----
#ifndef brct_dense_reconstructor_h_
#define brct_dense_reconstructor_h_

//:
// \file
// \brief A class to carry out dense stereo reconstruction
//
// \author J.L. Mundy
// \verbatim
//  Initial version April 14, 2004
// \endverbatim
//
//////////////////////////////////////////////////////////////////////

#include <vcl_vector.h>
#include <vcl_string.h>
#include <vil1/vil1_image.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsrl/vsrl_image_correlation.h>
#include <vsrl/vsrl_raster_dp_setup.h>
#include <vsrl/vsrl_dense_matcher.h>

class brct_dense_reconstructor : public vsrl_dense_matcher
{
  vsrl_image_correlation image_correlation_;
  vsrl_raster_dp_setup **raster_array_;
  int num_raster_;
  int correlation_range_; // the correlation range of the data

 public:

  // constructor
  brct_dense_reconstructor(vil1_image const& im1, vil1_image const& img2);

  // destructor
  ~brct_dense_reconstructor();
  //: set search range
  void set_search_range(const int range);

  //: set correlation window radius
  void set_correlation_window_radius(const int radius);

  //: set inner null cost
  void set_inner_cost(const double inner_cost);

  //: set outer null cost
  void set_outer_cost(const double outer_cost);

  //: set continuity cost
  void set_continuity_cost(const double continuity_cost);

  //: set up dense matching data
  void initial_calculations();

  //: performs the dynamic program on the specified raster
  void evaluate_raster(const int i);

  //: do all rasters
  void execute();

  //: print parameters
  void print_params();

  //: get vsol points corresponding to a line from image 0
  vcl_vector<vsol_point_2d_sptr> points0(const int i, const int del = 5);

  //: get vsol points corresponding to a line from image 1
  vcl_vector<vsol_point_2d_sptr> points1(const int i, const int del = 5);

  //: get the assigned coordinate
  virtual  int get_assignment(int x, int y);

  //: get the disparity of pixel x,y
  virtual int get_disparity(int x, int y);

  //: get the correlation values within the search range
  void get_correlation(const int x, const int y, vcl_vector<int>& xpos, vcl_vector<double>& corr);

  //: write an image of the disparity
  void write_disparity_image(char *filename);

  //: print out the correlation costs for point x,y
  void print_correlation_cost(const int x, const int y);
};

#endif // brct_dense_reconstructor_h_
