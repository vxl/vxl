// This is core/vil1/vil1_memory_image_window.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Andrew W. Fitzgibbon, Oxford RRG
// \date   19 Aug 96
//-----------------------------------------------------------------------------

#include "vil1_memory_image_window.h"
#include <vcl_cmath.h>

vil1_memory_image_window::vil1_memory_image_window(
           const vil1_memory_image_of<vil1_byte>& image,
           int centre_x, int centre_y, int mask_size):
  image1_(image)
{
  init(centre_x, centre_y, mask_size);
}

void vil1_memory_image_window::init(int centre_x, int centre_y, int mask_size)
{
  mask_size_ = mask_size;
  centre1_x_ = centre_x;
  centre1_y_ = centre_y;
  mask1_col_index_ = centre_x - mask_size_ / 2;
  mask1_row_index_ = centre_y - mask_size_ / 2;
}

float vil1_memory_image_window::mean_intensity()
{
  int tot = 0;
  for (int row_index = 0; row_index < mask_size_; row_index++)
    for (int col_index = 0; col_index < mask_size_; col_index++)
      tot += image1_(mask1_col_index_ +col_index, mask1_row_index_ +row_index);
  return (float)tot / (float)(mask_size_ * mask_size_);
}

inline int labs(int x) { return (x > 0) ? x : -x; }

//:Return early if difference becomes greater than early_exit_level.
// This is a useful check to have anyway as the default arg of MAXINT avoids
// accumulator overflow which can easily happen on certain medical and range
// images.
int vil1_memory_image_window::sum_abs_diff(const vil1_memory_image_of<vil1_byte>& image2,
                                           int centre2_x, int centre2_y,
                                           int early_exit_level)
{
  int mask2_col_index = centre2_x - mask_size_ / 2;
  int mask2_row_index = centre2_y - mask_size_ / 2;

  // make sure that we don't ask for pixels outside the image - PVr, 1 dec. 1997
  int row_start = 0;
  if (row_start < -mask1_row_index_) row_start = -mask1_row_index_;
  if (row_start < -mask2_row_index) row_start = -mask2_row_index;
  int row_end = mask_size_;
  if (row_end >= int(image1_.width())-mask1_row_index_) row_end = image1_.width()-mask1_row_index_-1;
  if (row_end >= int(image2.width())-mask2_row_index) row_end = image2.width()-mask2_row_index-1;
  int col_start = 0;
  if (col_start < -mask1_col_index_) col_start = -mask1_col_index_;
  if (col_start < -mask2_col_index) col_start = -mask2_col_index;
  int col_end = mask_size_;
  if (col_end >= int(image1_.width())-mask1_col_index_) col_end = image1_.width()-mask1_col_index_-1;
  if (col_end >= int(image2.width())-mask2_col_index) col_end = image2.width()-mask2_col_index-1;

  int difference_total = 0;
  for (int row_index = row_start; row_index < row_end; row_index++)
    for (int col_index = col_start; col_index < col_end; col_index++)
    {
      int p1 = image1_(mask1_col_index_ + col_index, mask1_row_index_ + row_index);
      int p2 =  image2( mask2_col_index + col_index,  mask2_row_index + row_index);

      difference_total += p1>p2 ? p1-p2 : p2-p1; // avoid vnl dependency - PVr

      // Check to see if we can return early -- this is also useful as it implicitly
      // avoids accumulator overflow.
      if (difference_total > early_exit_level)
        return difference_total;
    }
  return difference_total;
}

//:Return early if difference becomes greater than early_exit_level.
// This is a useful check to have anyway as the default arg of MAXINT avoids
// accumulator overflow which can easily happen on certain medical and range
// images.
int vil1_memory_image_window::sum_sqr_diff(const vil1_memory_image_of<vil1_byte>& image2,
                                           int centre2_x, int centre2_y,
                                           int early_exit_level)
{
  int mask2_col_index = centre2_x - mask_size_ / 2;
  int mask2_row_index = centre2_y - mask_size_ / 2;

  // make sure that we don't ask for pixels outside the image - PVr, 1 dec. 1997
  int row_start = 0;
  if (row_start < -mask1_row_index_) row_start = -mask1_row_index_;
  if (row_start < -mask2_row_index) row_start = -mask2_row_index;
  int row_end = mask_size_;
  if (row_end >= int(image1_.width())-mask1_row_index_) row_end = image1_.width()-mask1_row_index_-1;
  if (row_end >= int(image2.width())-mask2_row_index) row_end = image2.width()-mask2_row_index-1;
  int col_start = 0;
  if (col_start < -mask1_col_index_) col_start = -mask1_col_index_;
  if (col_start < -mask2_col_index) col_start = -mask2_col_index;
  int col_end = mask_size_;
  if (col_end >= int(image1_.width())-mask1_col_index_) col_end = image1_.width()-mask1_col_index_-1;
  if (col_end >= int(image2.width())-mask2_col_index) col_end = image2.width()-mask2_col_index-1;

  int difference_total = 0;
  for (int row_index = row_start; row_index < row_end; row_index++)
    for (int col_index = col_start; col_index < col_end; col_index++)
    {
      int p1 = image1_(mask1_col_index_ + col_index, mask1_row_index_ + row_index);
      int p2 =  image2( mask2_col_index + col_index,  mask2_row_index + row_index);

      difference_total += (p1-p2)*(p1-p2);

      // Check to see if we can return early -- this is also useful as it implicitly
      // avoids accumulator overflow.
      if (difference_total > early_exit_level)
        return difference_total;
    }
  return difference_total;
}


int vil1_memory_image_window::normalised_sum_abs_diff(const vil1_memory_image_of<vil1_byte>& image2,
                                                      int centre2_x, int centre2_y,
                                                      double normalise_ratio,
                                                      int early_exit_level)
{
  int mask2_col_index = centre2_x - mask_size_ / 2;
  int mask2_row_index = centre2_y - mask_size_ / 2;

  int difference_total = 0;
  for (int row_index = 0; row_index < mask_size_; row_index++)
    for (int col_index = 0; col_index < mask_size_; col_index++)
    {
      int p1 = image1_(mask1_col_index_ + col_index, mask1_row_index_ + row_index);
      int p2 =  image2( mask2_col_index + col_index,  mask2_row_index + row_index);
      difference_total += labs (p1 - (int) (normalise_ratio * (float) p2));

      if (difference_total > early_exit_level)
        return difference_total;
    }

  return difference_total;
}


double vil1_memory_image_window::normalised_cross_correlation(const vil1_memory_image_of<vil1_byte>& image2,
                                                              int centre2_x, int centre2_y)
{
  // set mask size
  int n = (int)mask_size_/2;
  int m = (int)mask_size_/2;


  //////////////////////////////////////////////
  // setup the integer locations of the
  // points
  //
  int u1 = centre1_x_;
  int v1 = centre1_y_;
  int u2 = centre2_x;
  int v2 = centre2_y;

  // indices
  int i,j;

  double average_I1_uv;
  double average_I2_uv;

  //////////////////////////////////////////////
  // calculate the average intensities
  //

  average_I1_uv = 0;
  average_I2_uv = 0;

  for (i = -n; i < n+1; i++) {
    for (j = -m; j < m+1; j++) {
      average_I1_uv += image1_(u1+i,v1+j);
      average_I2_uv +=  image2(u2+i,v2+j);
    }
  }

  average_I1_uv /= ((2*n + 1)*(2*m + 1));
  average_I2_uv /= ((2*n + 1)*(2*m + 1));

  //////////////////////////////////////////////
  // calculate the std. deviations
  //

  double result_I1 = 0;
  double result_I2 = 0;

  for (i = -n; i < n+1; i++) {
    for (j = -m; j < m+1; j++) {
      double I1_uv = image1_(u1+i,v1+j);
      result_I1 += (I1_uv - average_I1_uv ) * (I1_uv - average_I1_uv);

      double I2_uv = image2(u2+i,v2+j);
      result_I2 += (I2_uv - average_I2_uv ) * (I2_uv - average_I2_uv);
    }
  }

  double std_dev_I1_uv = vcl_sqrt(result_I1);
  double std_dev_I2_uv = vcl_sqrt(result_I2);

  ///////////////////////////////////////
  // calculate the correlation score
  // again using result as a temporary
  //  variable

  double result = 0;

  for (i = -n; i < n+1; i++)
  {
    for (j = -m; j < m+1; j++)
    {
      double I1_uv;
      double I2_uv;

      I1_uv = image1_(u1+i,v1+j);
      I2_uv =  image2(u2+i,v2+j);

      result += (I1_uv - average_I1_uv)*(I2_uv - average_I2_uv);
    }
  }

  result /= vcl_sqrt(std_dev_I1_uv*std_dev_I1_uv*std_dev_I2_uv*std_dev_I2_uv);

  return result;
}
