// This is gel/vsrl/vsrl_image_correlation.h
#ifndef vsrl_image_correlation_h
#define vsrl_image_correlation_h
//:
// \file

#include <vxl_config.h>
#include <vil1/vil1_memory_image_of.h>
#include <vnl/vnl_matrix.h>

typedef vil1_memory_image_of<vxl_byte> vil1_byte_buffer;

class vil1_image;

//:
// this class will determine the correlation between two images for
// a pair of regions

class vsrl_image_correlation
{
 public:
  // constructor
  vsrl_image_correlation(const vil1_image &im1, const vil1_image &im2);

  // destructor
  ~vsrl_image_correlation();

  //: set the correlation window width
  void set_window_width(int width);

  //: set the correlation window height
  void set_window_height(int height);

  //: the number of correlations used
  void set_correlation_range(int range);
  //: the number of correlations used
  int get_correlation_range();

  //: perform a set of initial calculations
  void initial_calculations();

  //: get the correlation using the initial calculations
  double get_correlation(int x1, int y1, int delta_x);

  //: we want to know how the correlation function behaves around the point x1 y1.
  void get_correlation_stats(int x, int y, double &mean, double &std);

  //: interpolate to get subpixel accuracy for delta_x
  double get_sub_pixel_delta(int x1, int y1, int delta_x);

  //: get the correlation directly
  double get_correlation(int x1, int y1, int x2, int y2);

  // get some stats regarding the buffers

  int get_image1_width();
  int get_image1_height();

  int get_image2_width();
  int get_image2_height();

  double get_image_value1(int x, int y);
  double get_image_value2(int x, int y);

  double get_mean_1(int x, int y);
  double get_mean_2(int x, int y);
  double get_std_1(int x, int y);
  double get_std_2(int x, int y);

 private:

  int window_width_;
  int window_height_;
  int correlation_range_;

  //: the results of initial correlations
  vnl_matrix<double> **image_correlations_;

  // the local stats

  vnl_matrix<double> *std_x_;
  vnl_matrix<double> *std_y_;
  vnl_matrix<double> *mean_x_;
  vnl_matrix<double> *mean_y_;

  //: get a matrix of means and stds for a given image
  void  compute_local_stats(vnl_matrix<double> &im, vnl_matrix<double> &mean, vnl_matrix<double> &std);

  //: compute the correlation for a given x offset
  void compute_correlation(int x_offset, vnl_matrix<double> &X, vnl_matrix<double> &Y, vnl_matrix<double> &mean_x,
                           vnl_matrix<double> &mean_y, vnl_matrix<double> &std_x, vnl_matrix<double> &std_y,
                           vnl_matrix<double> &corr_matrix);

  void shift_multiply_matrix(int offset, vnl_matrix<double> &X, vnl_matrix<double> &Y,
                             vnl_matrix<double> &XY);

  //: memory images of the data

  vil1_byte_buffer buffer1_;
  vil1_byte_buffer buffer2_;

  //: compute the local stats using the slow method.
  void compute_local_stats(vil1_byte_buffer &buf, int x, int y, double &mean, double &std);

  //: make sure that x and y are in the bounds of buf
  bool check_range(vil1_byte_buffer &buf, int x, int y);
};

#endif // vsrl_image_correlation_h
