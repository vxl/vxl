// This is vxl/vil/vil_memory_image_window.h
#ifndef vil_memory_image_window_h_
#define vil_memory_image_window_h_
#ifdef __GNUC__
#pragma interface
#endif
//:
// \file
// \brief Operations on a small region of an image

#include "vil_memory_image_of.h"
#include <vil/vil_byte.h>

const int vil_memory_image_window_maxint = 0x07ffffff;

//: Operations on a small region of an image
//    vil_memory_image_window centres a mask around a pixel in an vil_memory_image_of,
//    and implements a few comparison operators: SSD, NSSD, NCC.
class vil_memory_image_window {
public:
  // Constructors/Destructors--------------------------------------------------

  vil_memory_image_window(const vil_memory_image_of<vil_byte>& image, int centre_x, int centre_y, int mask_size);

  // Computations--------------------------------------------------------------

#if 0
  //: DEPRECATED. This function has *never* computed the SSD. It returns the sum of abs diff.
  // The old behaviour is duplicated by sum_abs_diff() .
  // David Capel May 2002.
  int sum_squared_differences(const vil_memory_image_of<vil_byte>& image2,
                              int centre2_x, int centre2_y,
                              int early_exit_level = vil_memory_image_window_maxint);

  //: DEPRECATED. This function does not compute norm cross correlation. It returns a normalized sum of abs diff.
  // The old behaviour is duplicated by normalised_sum_abs_diff().
  // David Capel May 2002.
  int ncc(const vil_memory_image_of<vil_byte>& image2,
          int centre2_x, int centre2_y,
          double normalise_ratio,
          int early_exit_level = vil_memory_image_window_maxint);
#endif

  int sum_abs_diff(const vil_memory_image_of<vil_byte>& image2,
                   int centre2_x, int centre2_y,
                   int early_exit_level = vil_memory_image_window_maxint);

  int sum_sqr_diff(const vil_memory_image_of<vil_byte>& image2,
                   int centre2_x, int centre2_y,
                   int early_exit_level = vil_memory_image_window_maxint);

  int normalised_sum_abs_diff(const vil_memory_image_of<vil_byte>& image2,
                              int centre2_x, int centre2_y,
                              double normalise_ratio,
                              int early_exit_level = vil_memory_image_window_maxint);

  double normalised_cross_correlation(const vil_memory_image_of<vil_byte>& image2,
                                      int centre2_x, int centre2_y);

  float mean_intensity();

protected:
  // Data Members--------------------------------------------------------------
  const vil_memory_image_of<vil_byte>& image1_;
  int mask_size_;
  int mask1_col_index_;
  int mask1_row_index_;
  int centre1_x_;
  int centre1_y_;

  void init(int centre_x, int centre_y, int mask_size);

private:
  // Helpers-------------------------------------------------------------------
  vil_memory_image_window(const vil_memory_image_window& that);
  vil_memory_image_window& operator=(const vil_memory_image_window& that);
};

#endif // vil_memory_image_window_h_
