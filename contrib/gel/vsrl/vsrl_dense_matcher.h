#ifndef vsrl_dense_matcher_h
#define vsrl_dense_matcher_h

#include <vil/vil_image.h>

// this is a base class for various dence matchers

class vsrl_dense_matcher
{
 protected:

  vil_image image1_;
  int correlation_range_;

 public:

  // constructor

  vsrl_dense_matcher(const vil_image &image1);

  // destructor

  virtual ~vsrl_dense_matcher();

  // set the correlation range

  virtual void set_correlation_range(int range);

  // get the assigned coordinate

  virtual int get_assignment(int x, int y) = 0;

  // get the disparity of pixel x,y

  virtual int get_disparity(int x, int y) = 0;

  // get the width and height of image1

  int get_width() { return image1_.width(); }
  int get_height() { return image1_.height(); }
};

#endif
