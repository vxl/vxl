// This is core/vil1/vil1_memory_image_window.h
#ifndef vil1_memory_image_window_h_
#define vil1_memory_image_window_h_
//:
// \file
// \brief Operations on a small region of an image

#include "vil1_memory_image_of.h"
#include <vxl_config.h>

const int vil1_memory_image_window_maxint = 0x07ffffff;

//: Operations on a small region of an image.
//  vil1_memory_image_window centres a mask around a pixel in a vil1_memory_image_of,
//  and implements a few comparison operators: SSD, NSSD, NCC.
class vil1_memory_image_window
{
 public:
  // Constructors/Destructors--------------------------------------------------

  vil1_memory_image_window(const vil1_memory_image_of<vxl_byte>& image, int centre_x, int centre_y, int mask_size);

  // Computations--------------------------------------------------------------

  int sum_abs_diff(const vil1_memory_image_of<vxl_byte>& image2,
                   int centre2_x, int centre2_y,
                   int early_exit_level = vil1_memory_image_window_maxint);

  int sum_sqr_diff(const vil1_memory_image_of<vxl_byte>& image2,
                   int centre2_x, int centre2_y,
                   int early_exit_level = vil1_memory_image_window_maxint);

  int normalised_sum_abs_diff(const vil1_memory_image_of<vxl_byte>& image2,
                              int centre2_x, int centre2_y,
                              double normalise_ratio,
                              int early_exit_level = vil1_memory_image_window_maxint);

  double normalised_cross_correlation(const vil1_memory_image_of<vxl_byte>& image2,
                                      int centre2_x, int centre2_y);

  float mean_intensity();

 protected:
  // Data Members--------------------------------------------------------------
  const vil1_memory_image_of<vxl_byte>& image1_;
  int mask_size_;
  int mask1_col_index_;
  int mask1_row_index_;
  int centre1_x_;
  int centre1_y_;

  void init(int centre_x, int centre_y, int mask_size);

 private:
  // Helpers-------------------------------------------------------------------
  vil1_memory_image_window(const vil1_memory_image_window& that) = delete;
  vil1_memory_image_window& operator=(const vil1_memory_image_window& that) = delete;
};

#endif // vil1_memory_image_window_h_
