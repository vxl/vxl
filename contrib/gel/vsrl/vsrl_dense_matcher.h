#ifndef vsrl_dense_matcher_h
#define vsrl_dense_matcher_h
//:
// \file
// \brief this is a base class for various dense matchers
//
// \verbatim
//  Modifications
//   10 Sep. 2004 Peter Vanroose  Inlined all 1-line methods in class decl
// \endverbatim

#include <vil1/vil1_image.h>

class vsrl_dense_matcher
{
 protected:

  vil1_image image1_;
  int correlation_range_;

 public:

  // constructor

  vsrl_dense_matcher(vil1_image const& image1) : image1_(image1) {}

  // destructor

  virtual ~vsrl_dense_matcher() {}

  // set the correlation range

  void set_correlation_range(int range) { correlation_range_=range; }

  // get the correlation range

  int get_correlation_range() { return correlation_range_; }

  // get the assigned coordinate

  virtual int get_assignment(int x, int y) = 0;

  // get the disparity of pixel x,y

  virtual int get_disparity(int x, int y) = 0;

  // get the width and height of image1

  int get_width() { return image1_.width(); }
  int get_height() { return image1_.height(); }
};

#endif
