// This is oxl/osl/internals/droid.cxx
#include "droid.h"

#include <vcl_cmath.h>   // pow()
#include <vcl_cstring.h> // memset()

#include <osl/osl_roi_window.h>

//--------------------------------------------------------------------------------
//
// Computes gradient at each pixel.
// Uses the convolution mask [ 2  1 0 -1 -2].
//
void droid::compute_gradx_grady (osl_roi_window                 *window_str,
                                 vil1_memory_image_of<vxl_byte>  *image_ptr,
                                 vil1_memory_image_of<int> *image_gradx_ptr,
                                 vil1_memory_image_of<int> *image_grady_ptr)

{
  int row_start = window_str->row_start_index;
  int col_start = window_str->col_start_index;
  int row_end   = window_str->row_end_index;
  int col_end   = window_str->col_end_index;

  for (int row = row_start; row < row_end; row++) {
    for (int col = col_start; col < col_end; col++) {
#if 1
      (*image_gradx_ptr)[row][col]
        = 2*((*image_ptr)[row][col+2]-(*image_ptr)[row][col-2])
        +((*image_ptr)[row][col+1]-(*image_ptr)[row][col-1]);

      (*image_grady_ptr) [row][col]
        = 2*((*image_ptr)[row+2][col]-(*image_ptr)[row-2][col])
        +((*image_ptr)[row+1][col]-(*image_ptr)[row-1][col]);
#else
      (*image_gradx_ptr)[row][col] = (*image_ptr)[row][col+1] - (*image_ptr)[row][col-1];
      (*image_grady_ptr)[row][col] = (*image_ptr)[row+1][col] - (*image_ptr)[row-1][col];
#endif
    }
  }
}


//-----------------------------------------------------------------------------
//
// Computes the (singular) second moment matrices of the gradient at each pixel.
// Uses a local sum to estimate the gradient.
// This means that the operator used to compute the gradient in the x-direction
// is :
//     2     1     0    -1    -2
//     2     1     0    -1    -2
//     2     1     0    -1    -2
//     2     1     0    -1    -2
//     2     1     0    -1    -2
//
void droid::compute_fxx_fxy_fyy (osl_roi_window              *window_str,
                                 vil1_memory_image_of<int>   *image_gradx_ptr,
                                 vil1_memory_image_of<int>   *image_grady_ptr,
                                 vil1_memory_image_of<float> *image_fxx_ptr,
                                 vil1_memory_image_of<float> *image_fxy_ptr,
                                 vil1_memory_image_of<float> *image_fyy_ptr)
{
  float gradx,grady;

  int row_start = window_str->row_start_index;
  int col_start = window_str->col_start_index;
  int row_end   = window_str->row_end_index;
  int col_end   = window_str->col_end_index;

  for (int row = row_start; row < row_end; row++) {
    for (int col = col_start; col < col_end; col++) {

      gradx = (float) ((*image_gradx_ptr) [row-2][col  ] +
                       (*image_gradx_ptr) [row-1][col  ] +
                       (*image_gradx_ptr) [row  ][col  ] +
                       (*image_gradx_ptr) [row+1][col  ] +
                       (*image_gradx_ptr) [row+2][col  ]);

      grady = (float) ((*image_grady_ptr) [row  ][col-2] +
                       (*image_grady_ptr) [row  ][col-1] +
                       (*image_grady_ptr) [row  ][col  ] +
                       (*image_grady_ptr) [row  ][col+1] +
                       (*image_grady_ptr) [row  ][col+2]);

      (*image_fxx_ptr) [row][col] = gradx * gradx;
      (*image_fxy_ptr) [row][col] = gradx * grady;
      (*image_fyy_ptr) [row][col] = grady * grady;
    }
  }
}


//-----------------------------------------------------------------------------
//
// Computes the cornerness for each pixel.
// Returns the maximum overall cornerness.
//
float droid::compute_cornerness (osl_roi_window              *window_str,
                                 vil1_memory_image_of<float> *image_fxx_ptr,
                                 vil1_memory_image_of<float> *image_fxy_ptr,
                                 vil1_memory_image_of<float> *image_fyy_ptr,
                                 float scale,
                                 vil1_memory_image_of<float> *pixel_cornerness)
{
  float corner_max = 0;

  for (int row = window_str->row_start_index; row < window_str->row_end_index; row++) {
    for (int col = window_str->col_start_index; col < window_str->col_end_index; col++) {

      // pixel cornerness = (fxx*fyy-fxy^2) - scale*(fxx+fyy)^2
#if 0
      float determinant
        = (*image_fxx_ptr) [row][col] * (*image_fyy_ptr) [row][col]
        - (*image_fxy_ptr) [row][col] * (*image_fxy_ptr) [row][col];
      float trace
        = (*image_fxx_ptr) [row][col] + (*image_fyy_ptr) [row][col];
#else
      float fxx = (*image_fxx_ptr) [row][col];
      float fxy = (*image_fxy_ptr) [row][col];
      float fyy = (*image_fyy_ptr) [row][col];
      float determinant = fxx*fyy - fxy*fxy;
      float trace = fxx + fyy;
#endif
      (*pixel_cornerness) [row][col] = determinant - scale*trace*trace;

      // update max
      if ((*pixel_cornerness) [row][col] > corner_max)
        corner_max = (*pixel_cornerness) [row][col];
    }
  }

  return corner_max;
}


//-----------------------------------------------------------------------------
//
// marks those pixels whose cornerness exceed corner_min and
// which are local cornerness maxima in 5x5 windows.
// they are marked by setting the corresponding pixel in
// image_corner_max_ptr to true.
//
int droid::find_corner_maxima (float corner_min,
                               osl_roi_window              *window_str,
                               vil1_memory_image_of<float> *pixel_cornerness,
                               vil1_memory_image_of<bool>  *image_corner_max_ptr)
{
  // allow for the spread when setting the loop values - don't explicitly
  // update window to be smaller because this routine might be called multiple
  // times when trying to get required corner count.
  int row, col;

  // zero out the first and last two rows and columns of *image_corner_max_ptr :

  //
  row = window_str->row_start_index;
  for (col = window_str->col_start_index; col < window_str->col_end_index; col++)
    (*image_corner_max_ptr) [row][col] = false;
  row++;
  for (col = window_str->col_start_index; col < window_str->col_end_index; col++)
    (*image_corner_max_ptr) [row][col] = false;

  //
  row = window_str->row_end_index-1;
  for (col = window_str->col_start_index; col < window_str->col_end_index; col++)
    (*image_corner_max_ptr) [row][col] = false;
  row--;
  for (col = window_str->col_start_index; col < window_str->col_end_index; col++)
    (*image_corner_max_ptr) [row][col] = false;

  //
  col = window_str->col_start_index;
  for (row = window_str->row_start_index; row < window_str->row_end_index; row++)
    (*image_corner_max_ptr) [row][col] = false;
  col++;
  for (row = window_str->row_start_index; row < window_str->row_end_index; row++)
    (*image_corner_max_ptr) [row][col] = false;

  //
  col = window_str->col_end_index-1;
  for (row = window_str->row_start_index; row < window_str->row_end_index; row++)
    (*image_corner_max_ptr) [row][col] = false;
  col--;
  for (row = window_str->row_start_index; row < window_str->row_end_index; row++)
    (*image_corner_max_ptr) [row][col] = false;

  // we look for pixels which have cornerness above the threshold
  // and which are local maxima in a 5x5 pixel neighbourhood :
  //   .....
  //   .....
  //   ..x..
  //   .....
  //   .....
  int maxima_count = 0;
  for (row = window_str->row_start_index+2; row < window_str->row_end_index-2; row++) {
    for (col = window_str->col_start_index+2; col < window_str->col_end_index-2; col++) {
      // get cornerness at the central pixel :
      float pc=(*pixel_cornerness)[row][col];

      // if above threshold and a local maximum, then it counts :
      if ((pc > corner_min)

          && (pc >= (*pixel_cornerness) [row-2][col-2])
          && (pc >= (*pixel_cornerness) [row-2][col-1])
          && (pc >= (*pixel_cornerness) [row-2][col  ])
          && (pc >= (*pixel_cornerness) [row-2][col+1])
          && (pc >= (*pixel_cornerness) [row-2][col+2])

          && (pc >= (*pixel_cornerness) [row-1][col-2])
          && (pc >= (*pixel_cornerness) [row-1][col-1])
          && (pc >= (*pixel_cornerness) [row-1][col  ])
          && (pc >= (*pixel_cornerness) [row-1][col+1])
          && (pc >= (*pixel_cornerness) [row-1][col+2])

          && (pc >= (*pixel_cornerness) [row  ][col-2])
          && (pc >= (*pixel_cornerness) [row  ][col-1])
          /* --------------------------------------- */
          && (pc >  (*pixel_cornerness) [row  ][col+1])
          && (pc >  (*pixel_cornerness) [row  ][col+2])

          && (pc >  (*pixel_cornerness) [row+1][col-2])
          && (pc >  (*pixel_cornerness) [row+1][col-1])
          && (pc >  (*pixel_cornerness) [row+1][col  ])
          && (pc >  (*pixel_cornerness) [row+1][col+1])
          && (pc >  (*pixel_cornerness) [row+1][col+2])

          && (pc >  (*pixel_cornerness) [row+2][col-2])
          && (pc >  (*pixel_cornerness) [row+2][col-1])
          && (pc >  (*pixel_cornerness) [row+2][col  ])
          && (pc >  (*pixel_cornerness) [row+2][col+1])
          && (pc >  (*pixel_cornerness) [row+2][col+2]))
        {
          (*image_corner_max_ptr) [row][col] = true;
          maxima_count++;
        }

      else
        (*image_corner_max_ptr) [row][col] = false;
    }
  }
  return maxima_count;
}


//-----------------------------------------------------------------------------
//
// Change value of "corner_min" to get a more acceptable no of corners.
//
float droid::compute_corner_min (float /*corner_min*/,
                                 float corner_max,
                                 int corner_count_max,
                                 osl_roi_window              *window_str,
                                 vil1_memory_image_of<float> *pixel_cornerness,
                                 vil1_memory_image_of<bool>  *image_corner_max_ptr)
{
#define DR_BIN_COUNT 200

  float
    corner_4throot,
    corner_max_4throot;

  int
    bin_array [DR_BIN_COUNT],
    bin_index,
    corner_count;

  vcl_memset (bin_array, 0, sizeof (bin_array));
  corner_max_4throot = (float)vcl_pow ((double) corner_max, 0.25);

  for (int row = window_str->row_start_index; row < window_str->row_end_index; row++) {
    for (int col = window_str->col_start_index; col < window_str->col_end_index; col++) {
      if ((*image_corner_max_ptr) [row][col]) {
        corner_4throot = (float)vcl_pow ((double) (*pixel_cornerness) [row][col], 0.25);
        bin_index = (int) ((DR_BIN_COUNT-1) * corner_4throot / corner_max_4throot);
        bin_array [bin_index]++;
      }
    }
  }

  corner_count = 0;
  bin_index = DR_BIN_COUNT-1;
  while ((corner_count < corner_count_max) && (bin_index > 0)) {
    corner_count += bin_array [bin_index];
    bin_index--;
  }

  // increment by 2 : one to return to the threshold index, and a further one to
  // drop corner count below the threshold.

  bin_index += 2;

  return (float)vcl_pow ((double) bin_index / (double) (DR_BIN_COUNT-1) * corner_max_4throot, 4.0);

#undef DR_BIN_COUNT
}


//-----------------------------------------------------------------------------

//
// Compute the subpixel location of and value at cornerness maxima.
// Uses a quadratic fit.
//
bool droid::compute_subpixel_max (vil1_memory_image_of<float> *pixel_cornerness,
                                  int row, int col,
                                  double &x, double &y,
                                  bool pab_emulate)
{
  // defines to make the code shorter and hence easier to read :
#define DR_P11 (*pixel_cornerness) [row-1][col-1]
#define DR_P12 (*pixel_cornerness) [row-1][col  ]
#define DR_P13 (*pixel_cornerness) [row-1][col+1]
#define DR_P21 (*pixel_cornerness) [row  ][col-1]
#define DR_P22 (*pixel_cornerness) [row  ][col  ]
#define DR_P23 (*pixel_cornerness) [row  ][col+1]
#define DR_P31 (*pixel_cornerness) [row+1][col-1]
#define DR_P32 (*pixel_cornerness) [row+1][col  ]
#define DR_P33 (*pixel_cornerness) [row+1][col+1]

  // Create weighting coefficients for the quadratic fit. They are obtained by
  // applying the following masks to the 3x3 window centered on pixel (row,col) :
  float b,c,d,e,f;

  if (pab_emulate) { // old pab masks
    // 1/8 * [ -1  0 +1 ]
    //       [ -2  0 +2 ]
    //       [ -1  0 +1 ]
    b=0.125f*(-DR_P11 -2*DR_P21 -  DR_P31 +  DR_P13 +2*DR_P23 +  DR_P33);

    // 1/8 * [ -1 -2 -1 ]
    //       [  0  0  0 ]
    //       [ +1 +2 +1 ]
    c=0.125f*(-DR_P11 +  DR_P31 -2*DR_P12 +2*DR_P32 -  DR_P13 +  DR_P33);

    // 1/4 * [ +1 -2 +1 ]
    //       [ +2 -4 +2 ]
    //       [ +1 -2 +1 ]
    d=0.25f*(  DR_P11 +2*DR_P21 +  DR_P31 -2*DR_P12 -4*DR_P22 -2*DR_P32 +DR_P13 +2*DR_P23+DR_P33);

    // 1/4 * [ +1  0 -1 ]
    //       [  0  0  0 ]
    //       [ -1  0 +1 ]
    e=0.25f*(  DR_P11 -  DR_P31 -  DR_P13 +  DR_P33);

    // 1/4 * [ +1 +2 +1 ]
    //       [ -2 -4 -2 ]
    //       [ +1 +2 +1 ]
    f=0.25f*(  DR_P11 -2*DR_P21 +  DR_P31 +2*DR_P12 -4*DR_P22 +2*DR_P32 +DR_P13 -2*DR_P23+DR_P33);
  }
  else { // new fsm masks
    // 1/9 * [ -1 +2 -1 ]
    //       [ +2 +5 +2 ]
    //       [ -1 +2 -1 ]
    //a=....;

    // 1/6 * [ -1  0 +1 ]
    //       [ -1  0 +1 ]
    //       [ -1  0 +1 ]
    b=(-DR_P11 +  DR_P13 -  DR_P21 +  DR_P23 -  DR_P31 +  DR_P33)/6.0f;

    // 1/6 * [ -1 -1 -1 ]
    //       [  0  0  0 ]
    //       [ +1 +1 +1 ]
    c=(-DR_P11 -  DR_P12 -  DR_P13 +  DR_P31 +  DR_P32 +  DR_P33)/6.0f;

    // 1/3 * [ +1 -2 +1 ]
    //       [ +1 -2 +1 ]
    //       [ +1 -2 +1 ]
    d=(  DR_P11 -2*DR_P12 +  DR_P13 +  DR_P21 -2*DR_P22 +  DR_P23 +DR_P31 -2*DR_P32+DR_P33)/3.0f;

    // 1/4 * [ +1  0 -1 ]
    //       [  0  0  0 ]
    //       [ -1  0 +1 ]
    e=(  DR_P11 -  DR_P31 -  DR_P13 +  DR_P33)/4.0f;

    // 1/3 * [ +1 +1 +1 ]
    //       [ -2 -2 -2 ]
    //       [ +1 +1 +1 ]
    f=(  DR_P11 +  DR_P12 +  DR_P13 -2*DR_P21 -2*DR_P22 -2*DR_P23 +DR_P31 +  DR_P32+DR_P33)/3.0f;
  }

  //
  // The next bit is to find the extremum of the fitted surface by setting its
  // partial derivatives to zero. We need to solve the following linear system :
  //
  //  [ d e ] [ off_x ] + [ b ] = [ 0 ]      (dS/dx = 0)
  //  [ e f ] [ off_y ]   [ c ]   [ 0 ]      (dS/dy = 0)
  //
  // This implies that the fitted surface is
  // S(x,y) = a + b x + c y + 1/2 d x^2 + e x y + 1/2 f y^2
  // , where we don't need to know the value of a.
  //
  float det = d*f - e*e;
  if ((pab_emulate && (det != 0)) ||
      (!pab_emulate && (det > 0))) {
    float off_x = (c*e - b*f) / det;
    float off_y = (b*e - c*d) / det;

    // more than one pixel away
    if (vcl_fabs (off_x) > 1.0 || vcl_fabs (off_y) > 1.0)
      return false;
    else {
      x = col+off_x;
      y = row+off_y;
      return true;
    }
  }
  // it's a saddle surface, but the corner may be still be good :
  else if (!pab_emulate && det<0) {
    x = col;
    y = row;
    return true;
  }
  // we can\'t solve a singular system :(
  else
    return false;

#undef DR_P11
#undef DR_P12
#undef DR_P13
#undef DR_P21
#undef DR_P22
#undef DR_P23
#undef DR_P31
#undef DR_P32
#undef DR_P33
}

//-----------------------------------------------------------------------------

//
// window is (2*winsize+1) by (2*winsize+1)
//
int droid::find_local_maxima(float min,int winsize,
                             int x1,int y1,
                             int x2,int y2,
                             vil1_memory_image_of<float>  *bitmap,
                             vil1_memory_image_of<bool>  *max_p)
{
  //
  for (int i=x1;i<x2;i++)
    for (int j=y1;j<y2;j++)
      (*max_p)[i][j] = (*bitmap)[i][j]>=min;

  //
  // k is the step size.
  //
  for (int k=1;k<=winsize;k++) {
    // horizontal scans :
    for (int i=x1;i<x2;i++)
      for (int j=y1;j<y2-k;j++)
        if ((*bitmap)[i][j] < (*bitmap)[i][j+k])
          (*max_p)[i][j  ]=false;
        else
          (*max_p)[i][j+k]=false;

    // vertical scan :
    for (int i=x1;i<x2-k;i++)
      for (int j=y1;j<y2;j++)
        if ((*bitmap)[i][j] < (*bitmap)[i+k][j])
          (*max_p)[i  ][j]=false;
        else
          (*max_p)[i+k][j]=false;
  }

  int count=0;
  //
  // now do the more expensive test :
  //
  for (int i=x1+winsize;i<x2-winsize;i++) {
    for (int j=y1+winsize;j<y2-winsize;j++) {
      if (!(*max_p)[i][j])
        continue;

      for (int r=1;r<=winsize;r++) {
        for (int s=1;s<=winsize;s++) {
          if ((*bitmap)[i][j] < (*bitmap)[i+r][j+s] ||
              (*bitmap)[i][j] < (*bitmap)[i-r][j+s] ||
              (*bitmap)[i][j] < (*bitmap)[i+r][j-s] ||
              (*bitmap)[i][j] < (*bitmap)[i-r][j-s]) {
            (*max_p)[i][j]=false;
            goto skip;
          }
        }
      }
      if ((*max_p)[i][j])
        count ++;
    skip: {;}
    }
  }
  return count;
}
