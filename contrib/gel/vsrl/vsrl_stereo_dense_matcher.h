#ifndef vsrl_stereo_dense_matcher_h
#define vsrl_stereo_dense_matcher_h

//:
//  \file

#include <vsrl/vsrl_raster_dp_setup.h>
#include <vil/vil_image.h>
#include <vsrl/vsrl_dense_matcher.h>
#include <vsrl/vsrl_image_correlation.h>

//:
// This class will take a pair of stereo images
// where each point on one raster matches a point
// on the corresponding raster.
// Once two images are inserted, dynamic programming on
// each raster is performed. The user can then find the
// point corespondences between each pixel in the image.

class vsrl_stereo_dense_matcher : public vsrl_dense_matcher
{
  vsrl_image_correlation image_correlation_;
  vsrl_raster_dp_setup **raster_array_;
  int num_raster_;
  int correlation_range_; // the correlation range of the data

 public:

  // constructor
  vsrl_stereo_dense_matcher(const vil_image &im1, const vil_image &im2);

  // destructor
  ~vsrl_stereo_dense_matcher();

  // There may be some initial calculalations that we want to
  // perform without computing all of the dense calculations
  void initial_calculations();

  // perform the dence matching
  void execute();

  // get the assigned coordinate
  virtual  int get_assignment(int x, int y);

  // get the disparity of pixel x,y
  virtual int get_disparity(int x, int y);

  // write an image of the disparity
  void write_disparity_image(char *filename);

  // print out the correlation costs for point x,y
  void print_correlation_cost(int x, int y);

 private:

  void evaluate_raster(int i); // performs the dynamic program on the raster
};

#endif
