// This is oxl/osl/osl_convolve.cxx
#include "osl_convolve.h"
//:
//  \file

#include <osl/osl_roi_window.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//-----------------------------------------------------------------------------

// Depending on the mask size, this routine will call the
// appropriate float_mask? routine.
void osl_convolve (osl_roi_window const *window,
                   osl_1d_half_kernel<double> const *mask,
                   vil1_memory_image_of<float> *image,
                   vil1_memory_image_of<float> *scratch)
{
  switch (mask->count) {
  case 2:
    osl_convolve2 (window, mask, image, scratch);
    break;
  case 3:
    osl_convolve3 (window, mask, image, scratch);
    break;
  case 4:
    osl_convolve4 (window, mask, image, scratch);
    break;
  case 5:
    osl_convolve5 (window, mask, image, scratch);
    break;
  case 6:
    osl_convolve6 (window, mask, image, scratch);
    break;
  default:
    osl_convolven (window, mask, image, scratch);
    break;
  }
}

//-----------------------------------------------------------------------------

//: A convolution routine for masks of size 2.
void osl_convolve2 (osl_roi_window const *window,
                    osl_1d_half_kernel<double> const *mask,
                    vil1_memory_image_of<float> *image,
                    vil1_memory_image_of<float> *scratch)
{
  assert(mask->count == 2);
  int row_min = window->row_start_index;
  int col_min = window->col_start_index+1;
  int row_max = window->row_end_index;
  int col_max = window->col_end_index-1;

  double mask0 = mask->array [0];
  double mask1 = mask->array [1];

  // horizontal convolution.

  for (int row = row_min; row < row_max; row++) {
    for (int col = col_min; col < col_max; col++) {
      (*scratch) [row][col] = float(
          (*image) [row][col] * mask0
        +((*image) [row][col-1]+(*image) [row][col+1]) * mask1);
    }
  }

  // vertical convolution on top of the horizontal convolution.

  row_min += 1;
  row_max -= 1;

  for (int row = row_min; row < row_max; row++) {
    for (int col = col_min; col < col_max; col++) {
      (*image) [row][col] = float(
          (*scratch) [row][col] * mask0
        +((*scratch) [row-1][col]+(*scratch) [row+1][col]) * mask1);
    }
  }
}


//-----------------------------------------------------------------------------

//: A convolution routine for masks of size 3.
void osl_convolve3 (osl_roi_window const *window,
                    osl_1d_half_kernel<double> const *mask,
                    vil1_memory_image_of<float> *image,
                    vil1_memory_image_of<float> *scratch)
{
  assert(mask->count == 3);
  int row_min = window->row_start_index;
  int col_min = window->col_start_index+2;
  int row_max = window->row_end_index;
  int col_max = window->col_end_index-2;

  double mask0 = mask->array [0];
  double mask1 = mask->array [1];
  double mask2 = mask->array [2];

  // horizontal convolution.

  for (int row = row_min; row < row_max; row++) {
    for (int col = col_min; col < col_max; col++) {
      (*scratch) [row][col] = float(
           (*image) [row][col] * mask0
        + ((*image) [row][col-1]+(*image) [row][col+1]) * mask1
        + ((*image) [row][col-2]+(*image) [row][col+2]) * mask2);
    }
  }

  // vertical convolution on top of the horizontal convolution.

  row_min += 2;
  row_max -= 2;

  for (int row = row_min; row < row_max; row++) {
    for (int col = col_min; col < col_max; col++) {
      (*image) [row][col] = float(
           (*scratch) [row][col] * mask0
        + ((*scratch) [row-1][col]+(*scratch) [row+1][col]) * mask1
        + ((*scratch) [row-2][col]+(*scratch) [row+2][col]) * mask2);
    }
  }
}


//-----------------------------------------------------------------------------

//: -- A convolution routine for masks of size 4.
void osl_convolve4 (osl_roi_window const *window,
                    osl_1d_half_kernel<double> const *mask,
                    vil1_memory_image_of<float> *image,
                    vil1_memory_image_of<float> *scratch)
{
  assert(mask->count == 4);
  int row_min = window->row_start_index;
  int col_min = window->col_start_index+3;
  int row_max = window->row_end_index;
  int col_max = window->col_end_index-3;

  double mask0 = mask->array [0];
  double mask1 = mask->array [1];
  double mask2 = mask->array [2];
  double mask3 = mask->array [3];

  // horizontal convolution.

  for (int row = row_min; row < row_max; row++) {
    for (int col = col_min; col < col_max; col++) {
      (*scratch) [row][col] = float(
          (*image) [row][col] * mask0
        +((*image) [row][col-1]+(*image) [row][col+1]) * mask1
        +((*image) [row][col-2]+(*image) [row][col+2]) * mask2
        +((*image) [row][col-3]+(*image) [row][col+3]) * mask3);
    }
  }

  // vertical convolution on top of the horizontal convolution.

  row_min += 3;
  row_max -= 3;

  for (int row = row_min; row < row_max; row++) {
    for (int col = col_min; col < col_max; col++) {
      (*image) [row][col] = float(
          (*scratch) [row][col] * mask0
        +((*scratch) [row-1][col]+(*scratch) [row+1][col]) * mask1
        +((*scratch) [row-2][col]+(*scratch) [row+2][col]) * mask2
        +((*scratch) [row-3][col]+(*scratch) [row+3][col]) * mask3);
    }
  }
}


//-----------------------------------------------------------------------------

//: A convolution routine for masks of size 5.
void osl_convolve5 (osl_roi_window const *window,
                    osl_1d_half_kernel<double> const *mask,
                    vil1_memory_image_of<float> *image,
                    vil1_memory_image_of<float> *scratch)
{
  assert(mask->count == 5);
  int row_min = window->row_start_index;
  int col_min = window->col_start_index+4;
  int row_max = window->row_end_index;
  int col_max = window->col_end_index-4;

  double mask0 = mask->array [0];
  double mask1 = mask->array [1];
  double mask2 = mask->array [2];
  double mask3 = mask->array [3];
  double mask4 = mask->array [4];

  // horizontal convolution.

  for (int row = row_min; row < row_max; row++) {
    for (int col = col_min; col < col_max; col++) {
      (*scratch) [row][col] = float(
           (*image) [row][col] * mask0
        + ((*image) [row][col-1]+(*image) [row][col+1]) * mask1
        + ((*image) [row][col-2]+(*image) [row][col+2]) * mask2
        + ((*image) [row][col-3]+(*image) [row][col+3]) * mask3
        + ((*image) [row][col-4]+(*image) [row][col+4]) * mask4);
    }
  }

  // vertical convolution on top of the horizontal convolution.

  row_min += 4;
  row_max -= 4;

  for (int row = row_min; row < row_max; row++) {
    for (int col = col_min; col < col_max; col++) {
      (*image) [row][col] = float(
           (*scratch) [row][col] * mask0
        + ((*scratch) [row-1][col]+(*scratch) [row+1][col]) * mask1
        + ((*scratch) [row-2][col]+(*scratch) [row+2][col]) * mask2
        + ((*scratch) [row-3][col]+(*scratch) [row+3][col]) * mask3
        + ((*scratch) [row-4][col]+(*scratch) [row+4][col]) * mask4);
    }
  }
}


//-----------------------------------------------------------------------------

//: A convolution routine for masks of size 6.
void osl_convolve6 (osl_roi_window const *window,
                    osl_1d_half_kernel<double> const *mask,
                    vil1_memory_image_of<float> *image,
                    vil1_memory_image_of<float> *scratch)
{
  assert(mask->count == 6);
  int row_min = window->row_start_index;
  int col_min = window->col_start_index+5;
  int row_max = window->row_end_index;
  int col_max = window->col_end_index-5;

  double mask0 = mask->array [0];
  double mask1 = mask->array [1];
  double mask2 = mask->array [2];
  double mask3 = mask->array [3];
  double mask4 = mask->array [4];
  double mask5 = mask->array [5];

  // horizontal convolution.

  for (int row = row_min; row < row_max; row++) {
    for (int col = col_min; col < col_max; col++) {
      (*scratch) [row][col] = float(
           (*image) [row][col] * mask0
        + ((*image) [row][col-1]+(*image) [row][col+1]) * mask1
        + ((*image) [row][col-2]+(*image) [row][col+2]) * mask2
        + ((*image) [row][col-3]+(*image) [row][col+3]) * mask3
        + ((*image) [row][col-4]+(*image) [row][col+4]) * mask4
        + ((*image) [row][col-5]+(*image) [row][col+5]) * mask5);
    }
  }

  // vertical convolution on top of the horizontal convolution.

  row_min += 5;
  row_max -= 5;

  for (int row = row_min; row < row_max; row++) {
    for (int col = col_min; col < col_max; col++) {
      (*image) [row][col] = float(
           (*scratch) [row][col] * mask0
        + ((*scratch) [row-1][col]+(*scratch) [row+1][col]) * mask1
        + ((*scratch) [row-2][col]+(*scratch) [row+2][col]) * mask2
        + ((*scratch) [row-3][col]+(*scratch) [row+3][col]) * mask3
        + ((*scratch) [row-4][col]+(*scratch) [row+4][col]) * mask4
        + ((*scratch) [row-5][col]+(*scratch) [row+5][col]) * mask5);
    }
  }
}

//--------------------------------------------------------------------------------

//: A convolution routine for masks of any size.
void osl_convolven (osl_roi_window const *window,
                    osl_1d_half_kernel<double> const *mask,
                    vil1_memory_image_of<float> *image,
                    vil1_memory_image_of<float> *scratch)
{
  int row_min = window->row_start_index;
  int col_min = window->col_start_index+mask->count-1;
  int row_max = window->row_end_index;
  int col_max = window->col_end_index-(mask->count-1);

  /* horizontal convolution. */

  for (int row = row_min; row < row_max; row++) {
    for (int col = col_min; col < col_max; col++) {
      double v = (*image) [row][col] * mask->array [0];

      for (unsigned int mask_index = 1; mask_index < mask->count; mask_index++)
        v += ((*image) [row][col-mask_index]
             +(*image) [row][col+mask_index]) * mask->array [mask_index];
      (*scratch) [row][col] = float(v);
    }
  }

  /* vertical convolution on top of the horizontal convolution. */

  row_min += mask->count-1;
  row_max -= mask->count-1;

  for (int row = row_min; row < row_max; row++) {
    for (int col = col_min; col < col_max; col++) {
      double v = (*scratch) [row][col] * mask->array [0];

      for (unsigned int mask_index = 1; mask_index < mask->count; mask_index++)
        v += ((*scratch) [row-mask_index][col]+
              (*scratch) [row+mask_index][col]) * mask->array [mask_index];
      (*image) [row][col] = float(v);
    }
  }
}

//--------------------------------------------------------------------------------
