// This is core/vil/algo/vil_gauss_reduce.cxx
#include <cmath>
#include "vil_gauss_reduce.h"
//:
// \file
// \brief Functions to smooth and sub-sample an image in one direction
// \author Tim Cootes

#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vxl_config.h> // for vxl_byte
#include <vnl/vnl_erf.h>
#include <vnl/vnl_math.h>

//: Smooth and subsample single plane src_im in x to produce dest_im
//  Applies 1-5-8-5-1 filter in x, then samples
//  every other pixel.  Fills [0,(ni+1)/2-1][0,nj-1] elements of dest

template <>
void vil_gauss_reduce_1plane(const vxl_byte* src_im,
                             unsigned src_ni, unsigned src_nj,
                             std::ptrdiff_t s_x_step, std::ptrdiff_t s_y_step,
                             vxl_byte* dest_im,
                             std::ptrdiff_t d_x_step, std::ptrdiff_t d_y_step)
{
  vxl_byte* d_row = dest_im;
  const vxl_byte* s_row = src_im;
  std::ptrdiff_t sxs2 = s_x_step*2;
  unsigned ni2 = (src_ni-3)/2;
  for (unsigned y=0;y<src_nj;++y)
  {
    // Set first element of row
    *d_row = static_cast<vxl_byte>(
              vnl_math::rnd( 0.071f * s_row[sxs2]
                           + 0.357f * s_row[s_x_step]
                           + 0.572f * s_row[0]));

    vxl_byte * d = d_row + d_x_step;
    const vxl_byte* s = s_row + sxs2;
    for (unsigned x=0;x<ni2;++x)
    {
      *d = static_cast<vxl_byte>(
            vnl_math::rnd( 0.05*s[-sxs2]    + 0.05*s[sxs2]
                         + 0.25*s[-s_x_step]+ 0.25*s[s_x_step]
                         + 0.4*s[0]));

      d += d_x_step;
      s += sxs2;
    }
    // Set last elements of row
    *d = static_cast<vxl_byte>(
          vnl_math::rnd( 0.071f * s[-sxs2]
                       + 0.357f * s[-s_x_step]
                       + 0.572f * s[0]));

    d_row += d_y_step;
    s_row += s_y_step;
  }
}

//: Smooth and subsample single plane src_im in x to produce dest_im
//  Applies 1-5-8-5-1 filter in x, then samples
//  every other pixel.  Fills [0,(ni+1)/2-1][0,nj-1] elements of dest
template <>
void vil_gauss_reduce_1plane(const float* src_im,
                             unsigned src_ni, unsigned src_nj,
                             std::ptrdiff_t s_x_step, std::ptrdiff_t s_y_step,
                             float* dest_im, std::ptrdiff_t d_x_step, std::ptrdiff_t d_y_step)
{
  float* d_row = dest_im;
  const float* s_row = src_im;
  std::ptrdiff_t sxs2 = s_x_step*2;
  unsigned ni2 = (src_ni-3)/2;
  for (unsigned y=0;y<src_nj;++y)
  {
    // Set first element of row
    *d_row =  0.071f * s_row[sxs2]
            + 0.357f * s_row[s_x_step]
            + 0.572f * s_row[0];
    float * d = d_row + d_x_step;
    const float* s = s_row + sxs2;
    for (unsigned x=0;x<ni2;++x)
    {
      *d =  0.05f*(s[-sxs2] + s[sxs2])
          + 0.25f*(s[-s_x_step]+ s[s_x_step])
          + 0.40f*s[0];

      d += d_x_step;
      s += sxs2;
    }
    // Set last elements of row
    *d =  0.071f * s[-sxs2]
        + 0.357f * s[-s_x_step]
        + 0.572f * s[0];

    d_row += d_y_step;
    s_row += s_y_step;
  }
}


//: Smooth and subsample single plane src_im in x to produce dest_im
//  Applies 1-5-8-5-1 filter in x, then samples
//  every other pixel.  Fills [0,(ni+1)/2-1][0,nj-1] elements of dest
template <>
void vil_gauss_reduce_1plane(const double* src_im,
                             unsigned src_ni, unsigned src_nj,
                             std::ptrdiff_t s_x_step, std::ptrdiff_t s_y_step,
                             double* dest_im, std::ptrdiff_t d_x_step, std::ptrdiff_t d_y_step)
{
  double* d_row = dest_im;
  const double* s_row = src_im;
  std::ptrdiff_t sxs2 = s_x_step*2;
  unsigned ni2 = (src_ni-3)/2;
  for (unsigned y=0;y<src_nj;++y)
  {
    // Set first element of row
    *d_row =  0.071 * s_row[sxs2]
            + 0.357 * s_row[s_x_step]
            + 0.572 * s_row[0];
    double * d = d_row + d_x_step;
    const double* s = s_row + sxs2;
    for (unsigned x=0;x<ni2;++x)
    {
      *d =  0.05f*(s[-sxs2] + s[sxs2])
          + 0.25f*(s[-s_x_step]+ s[s_x_step])
          + 0.40f*s[0];

      d += d_x_step;
      s += sxs2;
    }
    // Set last elements of row
    *d =  0.071f * s[-sxs2]
        + 0.357f * s[-s_x_step]
        + 0.572f * s[0];

    d_row += d_y_step;
    s_row += s_y_step;
  }
}

//: Smooth and subsample single plane src_im in x to produce dest_im
//  Applies 1-5-8-5-1 filter in x, then samples
//  every other pixel.  Fills [0,(ni+1)/2-1][0,nj-1] elements of dest
template <>
void vil_gauss_reduce_1plane(const int* src_im,
                             unsigned src_ni, unsigned src_nj,
                             std::ptrdiff_t s_x_step, std::ptrdiff_t s_y_step,
                             int* dest_im,
                             std::ptrdiff_t d_x_step, std::ptrdiff_t d_y_step)
{
  int* d_row = dest_im;
  const int* s_row = src_im;
  std::ptrdiff_t sxs2 = s_x_step*2;
  unsigned ni2 = (src_ni-3)/2;
  for (unsigned y=0;y<src_nj;++y)
  {
    // Set first element of row
    *d_row = vnl_math::rnd( 0.071f * s_row[sxs2]
                          + 0.357f * s_row[s_x_step]
                          + 0.572f * s_row[0]);

    int * d = d_row + d_x_step;
    const int* s = s_row + sxs2;
    for (unsigned x=0;x<ni2;++x)
    {
      *d = vnl_math::rnd( 0.05*s[-sxs2] + 0.25*s[-s_x_step] +
                          0.05*s[ sxs2] + 0.25*s[ s_x_step] +
                          0.4 *s[0]);

      d += d_x_step;
      s += sxs2;
    }
    // Set last elements of row
    *d = vnl_math::rnd( 0.071f * s[-sxs2]
                      + 0.357f * s[-s_x_step]
                      + 0.572f * s[0]);

    d_row += d_y_step;
    s_row += s_y_step;
  }
}

//: Smooth and subsample single plane src_im in x to produce dest_im
//  Applies 1-5-8-5-1 filter in x, then samples
//  every other pixel.  Fills [0,(ni+1)/2-1][0,nj-1] elements of dest
template <>
void vil_gauss_reduce_1plane(const vxl_int_16* src_im,
                             unsigned src_ni, unsigned src_nj,
                             std::ptrdiff_t s_x_step, std::ptrdiff_t s_y_step,
                             vxl_int_16* dest_im,
                             std::ptrdiff_t d_x_step, std::ptrdiff_t d_y_step)
{
  vxl_int_16* d_row = dest_im;
  const vxl_int_16* s_row = src_im;
  std::ptrdiff_t sxs2 = s_x_step*2;
  unsigned ni2 = (src_ni-3)/2;
  for (unsigned y=0;y<src_nj;++y)
  {
    // Set first element of row
    *d_row = static_cast<vxl_int_16>(
              vnl_math::rnd( 0.071f * s_row[sxs2]
                           + 0.357f * s_row[s_x_step]
                           + 0.572f * s_row[0]));

    vxl_int_16 * d = d_row + d_x_step;
    const vxl_int_16* s = s_row + sxs2;
    for (unsigned x=0;x<ni2;++x)
    {
      // The 0.5 offset in the following ensures rounding
      *d = vxl_int_16(0.5 + 0.05*s[-sxs2] + 0.25*s[-s_x_step]
                    + 0.05*s[ sxs2] + 0.25*s[ s_x_step]
                    + 0.4 *s[0]);

      d += d_x_step;
      s += sxs2;
    }
    // Set last elements of row
    *d = static_cast<vxl_int_16>(
          vnl_math::rnd( 0.071f * s[-sxs2]
                       + 0.357f * s[-s_x_step]
                       + 0.572f * s[0]));

    d_row += d_y_step;
    s_row += s_y_step;
  }
}

//: Smooth and subsample single plane src_im in x, result is 2/3rd size
//  Applies alternate 1-3-1, 1-1 filter in x, then samples
//  every other pixel.  Fills [0,(2*ni+1)/3-1][0,nj-1] elements of dest
//
//  Note, 131 filter only an approximation
template <>
void vil_gauss_reduce_2_3_1plane(const float* src_im,
                                 unsigned src_ni, unsigned src_nj,
                                 std::ptrdiff_t s_x_step, std::ptrdiff_t s_y_step,
                                 float* dest_im, std::ptrdiff_t d_x_step, std::ptrdiff_t d_y_step)
{
  float* d_row = dest_im;
  const float* s_row = src_im;
  std::ptrdiff_t sxs2 = s_x_step*2,sxs3 = s_x_step*3;
  unsigned d_ni = (2*src_ni+1)/3;
  unsigned d_ni2 = d_ni/2;
  for (unsigned y=0;y<src_nj;++y)
  {
    // Set first elements of row
    d_row[0]        = 0.75f*s_row[0] + 0.25f*s_row[s_x_step];
    d_row[d_x_step] = 0.5f*s_row[s_x_step] + 0.5f*s_row[sxs2];
    float * d = d_row + 2*d_x_step;
    const float* s = s_row + sxs3;
    for (unsigned x=1;x<d_ni2;++x)
    {
      *d = 0.2f*(s[-s_x_step] + s[s_x_step])+0.6f*s[0];
      d += d_x_step;
      *d = 0.5f*(s[s_x_step] + s[sxs2]);
      d += d_x_step;
      s += sxs3;
    }
    // Set last elements of row
    if (src_ni%3==1) *d=0.75f*s[-s_x_step] + 0.25f*s[0];
    else
    if (src_ni%3==2) *d=0.2f*(s[-s_x_step] + s[s_x_step])+0.6f*s[0];

    d_row += d_y_step;
    s_row += s_y_step;
  }
}

//: Smooth and subsample single plane src_im in x, result is 2/3rd size
//  Applies alternate 1-3-1, 1-1 filter in x, then samples
//  every other pixel.  Fills [0,(2*ni+1)/3-1][0,nj-1] elements of dest
//
//  Note, 131 filter only an approximation
template <>
void vil_gauss_reduce_2_3_1plane(const double* src_im,
                                 unsigned src_ni, unsigned src_nj,
                                 std::ptrdiff_t s_x_step, std::ptrdiff_t s_y_step,
                                 double* dest_im, std::ptrdiff_t d_x_step, std::ptrdiff_t d_y_step)
{
  double* d_row = dest_im;
  const double* s_row = src_im;
  std::ptrdiff_t sxs2 = s_x_step*2,sxs3 = s_x_step*3;
  unsigned d_ni = (2*src_ni+1)/3;
  unsigned d_ni2 = d_ni/2;
  for (unsigned y=0;y<src_nj;++y)
  {
    // Set first elements of row
    d_row[0]        = 0.75*s_row[0] + 0.25*s_row[s_x_step];
    d_row[d_x_step] = 0.5*s_row[s_x_step] + 0.5*s_row[sxs2];
    double * d = d_row + 2*d_x_step;
    const double* s = s_row + sxs3;
    for (unsigned x=1;x<d_ni2;++x)
    {
      *d = 0.2*(s[-s_x_step] + s[s_x_step])+0.6*s[0];
      d += d_x_step;
      *d = 0.5*(s[s_x_step] + s[sxs2]);
      d += d_x_step;
      s += sxs3;
    }
    // Set last elements of row
    if (src_ni%3==1) *d=0.75*s[-s_x_step] + 0.25*s[0];
    else
    if (src_ni%3==2) *d=0.2*(s[-s_x_step] + s[s_x_step])+0.6*s[0];

    d_row += d_y_step;
    s_row += s_y_step;
  }
}

//: Smooth and subsample single plane src_im in x, result is 2/3rd size
//  Applies alternate 1-3-1, 1-1 filter in x, then samples
//  every other pixel.  Fills [0,(2*ni+1)/3-1][0,nj-1] elements of dest
//
//  Note, 131 filter only an approximation
template <>
void vil_gauss_reduce_2_3_1plane(const vxl_byte* src_im,
                                 unsigned src_ni, unsigned src_nj,
                                 std::ptrdiff_t s_x_step, std::ptrdiff_t s_y_step,
                                 vxl_byte* dest_im, std::ptrdiff_t d_x_step, std::ptrdiff_t d_y_step)
{
  vxl_byte* d_row = dest_im;
  const vxl_byte* s_row = src_im;
  std::ptrdiff_t sxs2 = s_x_step*2,sxs3 = s_x_step*3;
  unsigned d_ni = (2*src_ni+1)/3;
  unsigned d_ni2 = d_ni/2;
  for (unsigned y=0;y<src_nj;++y)
  {
    // Set first elements of row
    // The 0.5 offset in the following ensures rounding
    d_row[0]        = vxl_byte(0.5f + 0.75f*s_row[0] + 0.25f*s_row[s_x_step]);
    d_row[d_x_step] = vxl_byte(0.5f + 0.5f*s_row[s_x_step] + 0.5f*s_row[sxs2]);
    vxl_byte * d = d_row + 2*d_x_step;
    const vxl_byte* s = s_row + sxs3;
    for (unsigned x=1;x<d_ni2;++x)
    {
      *d = vxl_byte(0.5f + 0.2f*(s[-s_x_step] + s[s_x_step]) + 0.6f*s[0]);
      d += d_x_step;
      *d = vxl_byte(0.5f + 0.5f*(s[s_x_step]  + s[sxs2]));
      d += d_x_step;
      s += sxs3;
    }
    // Set last elements of row
    if (src_ni%3==1)
      *d = vxl_byte(0.5f + 0.75f*s[-s_x_step] + 0.25f*s[0]);
    else if (src_ni%3==2)
      *d = vxl_byte(0.5f + 0.2f*(s[-s_x_step] + s[s_x_step]) + 0.6f*s[0]);

    d_row += d_y_step;
    s_row += s_y_step;
  }
}


//: Smooth and subsample single plane src_im in x, result is 2/3rd size
//  Applies alternate 1-3-1, 1-1 filter in x, then samples
//  every other pixel.  Fills [0,(2*ni+1)/3-1][0,nj-1] elements of dest
//
//  Note, 131 filter only an approximation
template <>
void vil_gauss_reduce_2_3_1plane(const int* src_im,
                                 unsigned src_ni, unsigned src_nj,
                                 std::ptrdiff_t s_x_step, std::ptrdiff_t s_y_step,
                                 int* dest_im, std::ptrdiff_t d_x_step, std::ptrdiff_t d_y_step)
{
  int* d_row = dest_im;
  const int* s_row = src_im;
  std::ptrdiff_t sxs2 = s_x_step*2,sxs3 = s_x_step*3;
  unsigned d_ni = (2*src_ni+1)/3;
  unsigned d_ni2 = d_ni/2;
  for (unsigned y=0;y<src_nj;++y)
  {
    // Set first elements of row
    // The 0.5 offset in the following ensures rounding
    d_row[0]        = int(0.5f + 0.75f*s_row[0] + 0.25f*s_row[s_x_step]);
    d_row[d_x_step] = int(0.5f + 0.5f*s_row[s_x_step] + 0.5f*s_row[sxs2]);
    int * d = d_row + 2*d_x_step;
    const int* s = s_row + sxs3;
    for (unsigned x=1;x<d_ni2;++x)
    {
      *d = int(0.5f + 0.2f*(s[-s_x_step] + s[s_x_step]) + 0.6f*s[0]);
      d += d_x_step;
      *d = int(0.5f + 0.5f*(s[s_x_step]  + s[sxs2]));
      d += d_x_step;
      s += sxs3;
    }
    // Set last elements of row
    if (src_ni%3==1)
      *d = int(0.5f + 0.75f*s[-s_x_step] + 0.25f*s[0]);
    else if (src_ni%3==2)
      *d = int(0.5f + 0.2f*(s[-s_x_step] + s[s_x_step]) + 0.6f*s[0]);

    d_row += d_y_step;
    s_row += s_y_step;
  }
}

//: Smooth and subsample single plane src_im in x, result is 2/3rd size
//  Applies alternate 1-3-1, 1-1 filter in x, then samples
//  every other pixel.  Fills [0,(2*ni+1)/3-1][0,nj-1] elements of dest
//
//  Note, 131 filter only an approximation
template <>
void vil_gauss_reduce_2_3_1plane(const vxl_int_16* src_im,
                                 unsigned src_ni, unsigned src_nj,
                                 std::ptrdiff_t s_x_step, std::ptrdiff_t s_y_step,
                                 vxl_int_16* dest_im, std::ptrdiff_t d_x_step, std::ptrdiff_t d_y_step)
{
  vxl_int_16* d_row = dest_im;
  const vxl_int_16* s_row = src_im;
  std::ptrdiff_t sxs2 = s_x_step*2,sxs3 = s_x_step*3;
  unsigned d_ni = (2*src_ni+1)/3;
  unsigned d_ni2 = d_ni/2;
  for (unsigned y=0;y<src_nj;++y)
  {
    // Set first elements of row
    // The 0.5 offset in the following ensures rounding
    d_row[0]        = vxl_int_16(0.5f + 0.75f*s_row[0] + 0.25f*s_row[s_x_step]);
    d_row[d_x_step] = vxl_int_16(0.5f + 0.5f*s_row[s_x_step] + 0.5f*s_row[sxs2]);
    vxl_int_16 * d = d_row + 2*d_x_step;
    const vxl_int_16* s = s_row + sxs3;
    for (unsigned x=1;x<d_ni2;++x)
    {
      *d = vxl_int_16(0.5f + 0.2f*(s[-s_x_step] + s[s_x_step]) + 0.6f*s[0]);
      d += d_x_step;
      *d = vxl_int_16(0.5f + 0.5f*(s[s_x_step]  + s[sxs2]));
      d += d_x_step;
      s += sxs3;
    }
    // Set last elements of row
    if (src_ni%3==1)
      *d = vxl_int_16(0.5f + 0.75f*s[-s_x_step] + 0.25f*s[0]);
    else if (src_ni%3==2)
      *d = vxl_int_16(0.5f + 0.2f*(s[-s_x_step] + s[s_x_step]) + 0.6f*s[0]);

    d_row += d_y_step;
    s_row += s_y_step;
  }
}

//: Smooth and subsample single plane src_im in x to produce dest_im using 121 filter in x and y
//  Smooths with a 3x3 filter and subsamples
template <>
void vil_gauss_reduce_121_1plane(const vxl_byte* src_im,
                                 unsigned src_ni, unsigned src_nj,
                                 std::ptrdiff_t s_x_step, std::ptrdiff_t s_y_step,
                                 vxl_byte* dest_im, std::ptrdiff_t d_x_step, std::ptrdiff_t d_y_step)
{
  std::ptrdiff_t sxs2 = s_x_step*2;
  std::ptrdiff_t sys2 = s_y_step*2;
  vxl_byte* d_row = dest_im+d_y_step;
  const vxl_byte* s_row1 = src_im + s_y_step;
  const vxl_byte* s_row2 = s_row1 + s_y_step;
  const vxl_byte* s_row3 = s_row2 + s_y_step;
  unsigned ni2 = (src_ni-2)/2;
  unsigned nj2 = (src_nj-2)/2;
  for (unsigned y=0;y<nj2;++y)
  {
    // Set first element of row
    *d_row = *s_row2;
    vxl_byte * d = d_row + d_x_step;
    const vxl_byte* s1 = s_row1 + sxs2;
    const vxl_byte* s2 = s_row2 + sxs2;
    const vxl_byte* s3 = s_row3 + sxs2;
    for (unsigned x=0;x<ni2;++x)
    {
      // The following is a little inefficient - could group terms to reduce arithmetic
      // Add 0.5 so that truncating effectively rounds
      *d = vxl_byte( 0.0625f * s1[-s_x_step] + 0.125f * s1[0] + 0.0625f * s1[s_x_step]
                   + 0.1250f * s2[-s_x_step] + 0.250f * s2[0] + 0.1250f * s2[s_x_step]
                   + 0.0625f * s3[-s_x_step] + 0.125f * s3[0] + 0.0625f * s3[s_x_step] +0.5);

      d += d_x_step;
      s1 += sxs2;
      s2 += sxs2;
      s3 += sxs2;
    }
    // Set last elements of row
    if (src_ni&1)
      *d = *s2;

    d_row += d_y_step;
    s_row1 += sys2;
    s_row2 += sys2;
    s_row3 += sys2;
  }

  // Need to set first and last rows as well

  // Dest image should be (src_ni+1)/2 x (src_nj+1)/2
  const vxl_byte* s0 = src_im;
  unsigned ni=(src_ni+1)/2;
  for (unsigned i=0;i<ni;++i)
  {
    dest_im[i]= *s0;
    s0+=sxs2;
  }

  if (src_nj&1)
  {
    unsigned yhi = (src_nj-1)/2;
    vxl_byte* dest_last_row = dest_im + yhi*d_y_step;
    const vxl_byte* s_last = src_im + yhi*sys2;
    for (unsigned i=0;i<ni;++i)
    {
      dest_last_row[i]= *s_last;
      s_last+=sxs2;
    }
  }
}


//: Smooth and subsample single plane src_im in x to produce dest_im using 121 filter in x and y
//  Smooths with a 3x3 filter and subsamples
template <>
void vil_gauss_reduce_121_1plane(const float* src_im,
                                 unsigned src_ni, unsigned src_nj,
                                 std::ptrdiff_t s_x_step, std::ptrdiff_t s_y_step,
                                 float* dest_im,
                                 std::ptrdiff_t d_x_step, std::ptrdiff_t d_y_step)
{
  std::ptrdiff_t sxs2 = s_x_step*2;
  std::ptrdiff_t sys2 = s_y_step*2;
  float* d_row = dest_im+d_y_step;
  const float* s_row1 = src_im + s_y_step;
  const float* s_row2 = s_row1 + s_y_step;
  const float* s_row3 = s_row2 + s_y_step;
  unsigned ni2 = (src_ni-2)/2;
  unsigned nj2 = (src_nj-2)/2;
  for (unsigned y=0;y<nj2;++y)
  {
    // Set first element of row
    *d_row = *s_row2;
    float * d = d_row + d_x_step;
    const float* s1 = s_row1 + sxs2;
    const float* s2 = s_row2 + sxs2;
    const float* s3 = s_row3 + sxs2;
    for (unsigned x=0;x<ni2;++x)
    {
      // The following is a little inefficient - could group terms to reduce arithmetic
      *d =   0.0625f * s1[-s_x_step] + 0.125f * s1[0] + 0.0625f * s1[s_x_step]
           + 0.1250f * s2[-s_x_step] + 0.250f * s2[0] + 0.1250f * s2[s_x_step]
           + 0.0625f * s3[-s_x_step] + 0.125f * s3[0] + 0.0625f * s3[s_x_step];

      d += d_x_step;
      s1 += sxs2;
      s2 += sxs2;
      s3 += sxs2;
    }
    // Set last elements of row
    if (src_ni&1)
      *d = *s2;

    d_row += d_y_step;
    s_row1 += sys2;
    s_row2 += sys2;
    s_row3 += sys2;
  }

  // Need to set first and last rows as well

  // Dest image should be (src_ni+1)/2 x (src_nj+1)/2
  const float* s0 = src_im;
  unsigned ni=(src_ni+1)/2;
  for (unsigned i=0;i<ni;++i)
  {
    dest_im[i]= *s0;
    s0+=sxs2;
  }

  if (src_nj&1)
  {
    unsigned yhi = (src_nj-1)/2;
    float* dest_last_row = dest_im + yhi*d_y_step;
    const float* s_last = src_im + yhi*sys2;
    for (unsigned i=0;i<ni;++i)
    {
      dest_last_row[i]= *s_last;
      s_last+=sxs2;
    }
  }
}


//: Smooth and subsample single plane src_im in x to produce dest_im using 121 filter in x and y
//  Smooths with a 3x3 filter and subsamples
template <>
void vil_gauss_reduce_121_1plane(const double* src_im,
                                 unsigned src_ni, unsigned src_nj,
                                 std::ptrdiff_t s_x_step, std::ptrdiff_t s_y_step,
                                 double* dest_im,
                                 std::ptrdiff_t d_x_step, std::ptrdiff_t d_y_step)
{
  std::ptrdiff_t sxs2 = s_x_step*2;
  std::ptrdiff_t sys2 = s_y_step*2;
  double* d_row = dest_im+d_y_step;
  const double* s_row1 = src_im + s_y_step;
  const double* s_row2 = s_row1 + s_y_step;
  const double* s_row3 = s_row2 + s_y_step;
  unsigned ni2 = (src_ni-2)/2;
  unsigned nj2 = (src_nj-2)/2;
  for (unsigned y=0;y<nj2;++y)
  {
    // Set first element of row
    *d_row = *s_row2;
    double * d = d_row + d_x_step;
    const double* s1 = s_row1 + sxs2;
    const double* s2 = s_row2 + sxs2;
    const double* s3 = s_row3 + sxs2;
    for (unsigned x=0;x<ni2;++x)
    {
      // The following is a little inefficient - could group terms to reduce arithmetic
      *d =   0.0625 * s1[-s_x_step] + 0.125 * s1[0] + 0.0625 * s1[s_x_step]
           + 0.1250 * s2[-s_x_step] + 0.250 * s2[0] + 0.1250 * s2[s_x_step]
           + 0.0625 * s3[-s_x_step] + 0.125 * s3[0] + 0.0625 * s3[s_x_step];

      d += d_x_step;
      s1 += sxs2;
      s2 += sxs2;
      s3 += sxs2;
    }
    // Set last elements of row
    if (src_ni&1)
      *d = *s2;

    d_row += d_y_step;
    s_row1 += sys2;
    s_row2 += sys2;
    s_row3 += sys2;
  }

  // Need to set first and last rows as well

  // Dest image should be (src_ni+1)/2 x (src_nj+1)/2
  const double* s0 = src_im;
  unsigned ni=(src_ni+1)/2;
  for (unsigned i=0;i<ni;++i)
  {
    dest_im[i]= *s0;
    s0+=sxs2;
  }

  if (src_nj&1)
  {
    unsigned yhi = (src_nj-1)/2;
    double* dest_last_row = dest_im + yhi*d_y_step;
    const double* s_last = src_im + yhi*sys2;
    for (unsigned i=0;i<ni;++i)
    {
      dest_last_row[i]= *s_last;
      s_last+=sxs2;
    }
  }
}


//: Smooth and subsample single plane src_im in x to produce dest_im using 121 filter in x and y
//  Smooths with a 3x3 filter and subsamples
template <>
void vil_gauss_reduce_121_1plane(const int* src_im,
                                 unsigned src_ni, unsigned src_nj,
                                 std::ptrdiff_t s_x_step, std::ptrdiff_t s_y_step,
                                 int* dest_im,
                                 std::ptrdiff_t d_x_step, std::ptrdiff_t d_y_step)
{
  std::ptrdiff_t sxs2 = s_x_step*2;
  std::ptrdiff_t sys2 = s_y_step*2;
  int* d_row = dest_im+d_y_step;
  const int* s_row1 = src_im + s_y_step;
  const int* s_row2 = s_row1 + s_y_step;
  const int* s_row3 = s_row2 + s_y_step;
  unsigned ni2 = (src_ni-2)/2;
  unsigned nj2 = (src_nj-2)/2;
  for (unsigned y=0;y<nj2;++y)
  {
    // Set first element of row
    *d_row = *s_row2;
    int * d = d_row + d_x_step;
    const int* s1 = s_row1 + sxs2;
    const int* s2 = s_row2 + sxs2;
    const int* s3 = s_row3 + sxs2;
    for (unsigned x=0;x<ni2;++x)
    {
      // The following is a little inefficient - could group terms to reduce arithmetic
      // Add 0.5 so that truncating effectively rounds
      *d = int( 0.0625f * s1[-s_x_step] + 0.125f * s1[0] + 0.0625f * s1[s_x_step]
              + 0.1250f * s2[-s_x_step] + 0.250f * s2[0] + 0.1250f * s2[s_x_step]
              + 0.0625f * s3[-s_x_step] + 0.125f * s3[0] + 0.0625f * s3[s_x_step] +0.5);

      d += d_x_step;
      s1 += sxs2;
      s2 += sxs2;
      s3 += sxs2;
    }
    // Set last elements of row
    if (src_ni&1)
      *d = *s2;

    d_row += d_y_step;
    s_row1 += sys2;
    s_row2 += sys2;
    s_row3 += sys2;
  }

  // Need to set first and last rows as well

  // Dest image should be (src_ni+1)/2 x (src_nj+1)/2
  const int* s0 = src_im;
  unsigned ni=(src_ni+1)/2;
  for (unsigned i=0;i<ni;++i)
  {
    dest_im[i]= *s0;
    s0+=sxs2;
  }

  if (src_nj&1)
  {
    unsigned yhi = (src_nj-1)/2;
    int* dest_last_row = dest_im + yhi*d_y_step;
    const int* s_last = src_im + yhi*sys2;
    for (unsigned i=0;i<ni;++i)
    {
      dest_last_row[i]= *s_last;
      s_last+=sxs2;
    }
  }
}

//: Smooth and subsample single plane src_im in x to produce dest_im using 121 filter in x and y
//  Smooths with a 3x3 filter and subsamples
template <>
void vil_gauss_reduce_121_1plane(const vxl_int_16* src_im,
                                 unsigned src_ni, unsigned src_nj,
                                 std::ptrdiff_t s_x_step, std::ptrdiff_t s_y_step,
                                 vxl_int_16* dest_im,
                                 std::ptrdiff_t d_x_step, std::ptrdiff_t d_y_step)
{
  std::ptrdiff_t sxs2 = s_x_step*2;
  std::ptrdiff_t sys2 = s_y_step*2;
  vxl_int_16* d_row = dest_im+d_y_step;
  const vxl_int_16* s_row1 = src_im + s_y_step;
  const vxl_int_16* s_row2 = s_row1 + s_y_step;
  const vxl_int_16* s_row3 = s_row2 + s_y_step;
  unsigned ni2 = (src_ni-2)/2;
  unsigned nj2 = (src_nj-2)/2;
  for (unsigned y=0;y<nj2;++y)
  {
    // Set first element of row
    *d_row = *s_row2;
    vxl_int_16 * d = d_row + d_x_step;
    const vxl_int_16* s1 = s_row1 + sxs2;
    const vxl_int_16* s2 = s_row2 + sxs2;
    const vxl_int_16* s3 = s_row3 + sxs2;
    for (unsigned x=0;x<ni2;++x)
    {
      // The following is a little inefficient - could group terms to reduce arithmetic
      // Add 0.5 so that truncating effectively rounds
      *d = vxl_int_16( 0.0625f * s1[-s_x_step] + 0.125f * s1[0] + 0.0625f * s1[s_x_step]
                     + 0.1250f * s2[-s_x_step] + 0.250f * s2[0] + 0.1250f * s2[s_x_step]
                     + 0.0625f * s3[-s_x_step] + 0.125f * s3[0] + 0.0625f * s3[s_x_step] +0.5);

      d += d_x_step;
      s1 += sxs2;
      s2 += sxs2;
      s3 += sxs2;
    }
    // Set last elements of row
    if (src_ni&1)
      *d = *s2;

    d_row += d_y_step;
    s_row1 += sys2;
    s_row2 += sys2;
    s_row3 += sys2;
  }

  // Need to set first and last rows as well

  // Dest image should be (src_ni+1)/2 x (src_nj+1)/2
  const vxl_int_16* s0 = src_im;
  unsigned ni=(src_ni+1)/2;
  for (unsigned i=0;i<ni;++i)
  {
    dest_im[i]= *s0;
    s0+=sxs2;
  }

  if (src_nj&1)
  {
    unsigned yhi = (src_nj-1)/2;
    vxl_int_16* dest_last_row = dest_im + yhi*d_y_step;
    const vxl_int_16* s_last = src_im + yhi*sys2;
    for (unsigned i=0;i<ni;++i)
    {
      dest_last_row[i]= *s_last;
      s_last+=sxs2;
    }
  }
}


vil_gauss_reduce_params::vil_gauss_reduce_params(double scaleStep)
{
  assert(scaleStep> 1.0  && scaleStep<=2.0);
  scale_step_ = scaleStep;
  // This arrangement gives close to a 1-5-8-5-1 filter for scalestep of 2.0;
  // and 0-0-1-0-0 for a scale step close to 1.0;
  double z = 1/std::sqrt(2.0*(scaleStep-1.0));
  filt0_ = vnl_erf(0.5 * z) - vnl_erf(-0.5 * z);
  filt1_ = vnl_erf(1.5 * z) - vnl_erf(0.5 * z);
  filt2_ = vnl_erf(2.5 * z) - vnl_erf(1.5 * z);

  double five_tap_total = 2*(filt2_ + filt1_) + filt0_;
#if 0
  double four_tap_total = filt2_ + 2*(filt1_) + filt0_;
  double three_tap_total = filt2_ + filt1_ + filt0_;
#endif

  //  Calculate 3 tap half Gaussian filter assuming constant edge extension
  filt_edge0_ = (filt0_ + filt1_ + filt2_) / five_tap_total;
  filt_edge1_ = filt1_ / five_tap_total;
  filt_edge2_ = filt2_ / five_tap_total;
#if 0
  filt_edge0_ = 1.0;
  filt_edge1_ = 0.0;
  filt_edge2_ = 0.0;
#endif
  //  Calculate 4 tap skewed Gaussian filter assuming constant edge extension
  filt_pen_edge_n1_ = (filt1_+filt2_) / five_tap_total;
  filt_pen_edge0_ = filt0_ / five_tap_total;
  filt_pen_edge1_ = filt1_ / five_tap_total;
  filt_pen_edge2_ = filt2_ / five_tap_total;

  //  Calculate 5 tap Gaussian filter
  filt0_ = filt0_ / five_tap_total;
  filt1_ = filt1_ / five_tap_total;
  filt2_ = filt2_ / five_tap_total;

  assert(filt_edge0_ > filt_edge1_);
  assert(filt_edge1_ > filt_edge2_);
}
