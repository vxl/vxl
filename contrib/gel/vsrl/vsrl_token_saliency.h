#ifndef vsrl_token_saliency_h
#define vsrl_token_saliency_h
//:
// \file
// this class is used to determine the saliency of a particular
// pixel. It makes this decision based on the results of a correlation
// function. We can also ask in the pixel is bounded by salient pixels
// along its raster line. In this way we can tell whether or not drastic
// measures will be required.

#include <vsrl/vsrl_image_correlation.h>

class vsrl_token_saliency
{
 public:

  // constructor
  vsrl_token_saliency(vsrl_image_correlation *image_correlation);

  // destructor
  ~vsrl_token_saliency();

  // for now we will work with thresholds on mean and std
  void set_std_threshold(double std_thresh);
  double get_std_threshold();

  void set_mean_threshold(double mean_thresh);
  double get_mean_threshold();

  // get the saliency of a particular pixel

  int get_saliency(int x, int y); // 0: not salient 1: salient

  // determine the first salient point on the left

  int get_left_salient_pixel(int x, int y);

  // determine the first salient point on the right

  int get_right_salient_pixel(int x, int y);

  // determine if the point is saliently bounded

  bool is_pixel_bounded_saliently(int x, int y);

  // get the saliency stats for pixel x, y

  void get_saliency_stats(int x, int y, double &mean, double &std);

  // print the saliency stats for pixel x, y

  void print_saliency_stats(int x, int y);

  // print the saliency stats for pixel x, y

  void print_saliency(int x, int y);

  // print the saliency stats for a given row

  void print_saliency_stats(int y);

  // print the saliency for a given row

  void print_saliency(int y);

  // save saliency image

  void create_saliency_image(char *filename);

 private:
  vsrl_image_correlation *image_correlation_; // structure used to determine image correlation

  vnl_matrix<int> *saliency_matrix_; // a matrix that keeps track of the saliencies

  // the saliency thresholds

  double std_thresh_;
  double mean_thresh_;

  // method used to compute saliencies

  void compute_saliency_matrix();
};

#endif // vsrl_token_saliency_h
