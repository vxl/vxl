// This is mul/mil/mil_gauss_reduce_2d.cxx
#include "mil_gauss_reduce_2d.h"
//:
//  \file
//  \brief Functions to smooth and sub-sample image in one direction
//  \author Tim Cootes

//: Smooth and subsample single plane src_im in x to produce dest_im
//  Applies 1-5-8-5-1 filter in x, then samples
//  every other pixel.  Fills [0,(nx+1)/2-1][0,ny-1] elements of dest
void mil_gauss_reduce_2d(vxl_byte* dest_im,
                         int d_x_step, int d_y_step,
                         const vxl_byte* src_im,
                         int src_nx, int src_ny,
                         int s_x_step, int s_y_step)
{
    vxl_byte* d_row = dest_im;
    const vxl_byte* s_row = src_im;
    int sxs2 = s_x_step*2;
    int nx2 = (src_nx-3)/2;
    for (int y=0;y<src_ny;++y)
    {
        // Set first element of row
        *d_row = *s_row;
        vxl_byte * d = d_row + d_x_step;
        const vxl_byte* s = s_row + sxs2;
        for (int x=0;x<nx2;++x)
        {
            // The 0.5 offset in the following ensures rounding
            *d = vxl_byte(0.5+ 0.05*s[-sxs2]     + 0.05*s[sxs2]
                             + 0.25*s[-s_x_step] + 0.25*s[s_x_step]
                             + 0.4 *s[0]);

            d += d_x_step;
            s += sxs2;
        }
        // Set last elements of row
        *d = *s;

        d_row += d_y_step;
        s_row += s_y_step;
    }
}

//: Smooth and subsample single plane src_im in x to produce dest_im
//  Applies 1-5-8-5-1 filter in x, then samples
//  every other pixel.  Fills [0,(nx+1)/2-1][0,ny-1] elements of dest
void mil_gauss_reduce_2d(float* dest_im,
                         int d_x_step, int d_y_step,
                         const float* src_im,
                         int src_nx, int src_ny,
                         int s_x_step, int s_y_step)
{
    float* d_row = dest_im;
    const float* s_row = src_im;
    int sxs2 = s_x_step*2;
    int nx2 = (src_nx-3)/2;
    for (int y=0;y<src_ny;++y)
    {
        // Set first element of row
        *d_row = *s_row;
        float * d = d_row + d_x_step;
        const float* s = s_row + sxs2;
        for (int x=0;x<nx2;++x)
        {
            *d = 0.05f*(s[-sxs2] + s[sxs2])
                +0.25f*(s[-s_x_step]+ s[s_x_step])
                +0.40f*s[0];

            d += d_x_step;
            s += sxs2;
        }
        // Set last elements of row
        *d = *s;

        d_row += d_y_step;
        s_row += s_y_step;
    }
}


//: Smooth and subsample single plane src_im in x to produce dest_im
//  Applies 1-5-8-5-1 filter in x, then samples
//  every other pixel.  Fills [0,(nx+1)/2-1][0,ny-1] elements of dest
void mil_gauss_reduce_2d(int* dest_im,
                         int d_x_step, int d_y_step,
                         const int* src_im,
                         int src_nx, int src_ny,
                         int s_x_step, int s_y_step)
{
    int* d_row = dest_im;
    const int* s_row = src_im;
    int sxs2 = s_x_step*2;
    int nx2 = (src_nx-3)/2;
    for (int y=0;y<src_ny;++y)
    {
        // Set first element of row
        *d_row = *s_row;
        int * d = d_row + d_x_step;
        const int* s = s_row + sxs2;
        for (int x=0;x<nx2;++x)
        {
            // The 0.5 offset in the following ensures rounding
            *d = int(0.5 + 0.05*s[-sxs2]     + 0.05*s[sxs2]
                         + 0.25*s[-s_x_step] + 0.25*s[s_x_step]
                         + 0.4 *s[0]);

            d += d_x_step;
            s += sxs2;
        }
        // Set last elements of row
        *d = *s;

        d_row += d_y_step;
        s_row += s_y_step;
    }
}


//: Smooth and subsample single plane src_im in x to produce dest_im using 121 filter in x and y
//  Smoothes with a 3x3 filter and subsamples
void mil_gauss_reduce_121_2d(vxl_byte* dest_im,
                             int d_x_step, int d_y_step,
                             const vxl_byte* src_im,
                             int src_nx, int src_ny,
                             int s_x_step, int s_y_step)
{
  int sxs2 = s_x_step*2;
  int sys2 = s_y_step*2;
  vxl_byte* d_row = dest_im+d_y_step;
  const vxl_byte* s_row1 = src_im + s_y_step;
  const vxl_byte* s_row2 = s_row1 + s_y_step;
  const vxl_byte* s_row3 = s_row2 + s_y_step;
  int nx2 = (src_nx-2)/2;
  int ny2 = (src_ny-2)/2;
  for (int y=0;y<ny2;++y)
  {
      // Set first element of row
      *d_row = *s_row2;
      vxl_byte * d = d_row + d_x_step;
      const vxl_byte* s1 = s_row1 + sxs2;
      const vxl_byte* s2 = s_row2 + sxs2;
      const vxl_byte* s3 = s_row3 + sxs2;
      for (int x=0;x<nx2;++x)
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
      if (src_nx%2==1)
        *d = *s2;

      d_row += d_y_step;
      s_row1 += sys2;
      s_row2 += sys2;
      s_row3 += sys2;
  }

  // Need to set first and last rows as well

  // Dest image should be (src_nx+1)/2 x (src_ny+1)/2
  const vxl_byte* s0 = src_im;
  int nx=(src_nx+1)/2;
  for (int i=0;i<nx;++i)
  {
    dest_im[i]= *s0;
    s0+=sxs2;
  }

  if (src_ny%2==1)
  {
    int yhi = (src_ny-1)/2;
    vxl_byte* dest_last_row = dest_im + yhi*d_y_step;
    const vxl_byte* s_last = src_im + yhi*sys2;
    for (int i=0;i<nx;++i)
    {
      dest_last_row[i]= *s_last;
      s_last+=sxs2;
    }
  }
}

//: Smooth and subsample single plane src_im in x to produce dest_im using 121 filter in x and y
//  Smoothes with a 3x3 filter and subsamples
void mil_gauss_reduce_121_2d(float* dest_im,
                             int d_x_step, int d_y_step,
                             const float* src_im,
                             int src_nx, int src_ny,
                             int s_x_step, int s_y_step)
{
  int sxs2 = s_x_step*2;
  int sys2 = s_y_step*2;
  float* d_row = dest_im+d_y_step;
  const float* s_row1 = src_im + s_y_step;
  const float* s_row2 = s_row1 + s_y_step;
  const float* s_row3 = s_row2 + s_y_step;
  int nx2 = (src_nx-2)/2;
  int ny2 = (src_ny-2)/2;
  for (int y=0;y<ny2;++y)
  {
      // Set first element of row
      *d_row = *s_row2;
      float * d = d_row + d_x_step;
      const float* s1 = s_row1 + sxs2;
      const float* s2 = s_row2 + sxs2;
      const float* s3 = s_row3 + sxs2;
      for (int x=0;x<nx2;++x)
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
      if (src_nx%2==1)
        *d = *s2;

      d_row += d_y_step;
      s_row1 += sys2;
      s_row2 += sys2;
      s_row3 += sys2;
  }

  // Need to set first and last rows as well

  // Dest image should be (src_nx+1)/2 x (src_ny+1)/2
  const float* s0 = src_im;
  int nx=(src_nx+1)/2;
  for (int i=0;i<nx;++i)
  {
    dest_im[i]= *s0;
    s0+=sxs2;
  }

  if (src_ny%2==1)
  {
    int yhi = (src_ny-1)/2;
    float* dest_last_row = dest_im + yhi*d_y_step;
    const float* s_last = src_im + yhi*sys2;
    for (int i=0;i<nx;++i)
    {
      dest_last_row[i]= *s_last;
      s_last+=sxs2;
    }
  }
}


//: Smooth and subsample single plane src_im in x to produce dest_im using 121 filter in x and y
//  Smoothes with a 3x3 filter and subsamples
void mil_gauss_reduce_121_2d(int* dest_im,
                             int d_x_step, int d_y_step,
                             const int* src_im,
                             int src_nx, int src_ny,
                             int s_x_step, int s_y_step)
{
  int sxs2 = s_x_step*2;
  int sys2 = s_y_step*2;
  int* d_row = dest_im+d_y_step;
  const int* s_row1 = src_im + s_y_step;
  const int* s_row2 = s_row1 + s_y_step;
  const int* s_row3 = s_row2 + s_y_step;
  int nx2 = (src_nx-2)/2;
  int ny2 = (src_ny-2)/2;
  for (int y=0;y<ny2;++y)
  {
      // Set first element of row
      *d_row = *s_row2;
      int * d = d_row + d_x_step;
      const int* s1 = s_row1 + sxs2;
      const int* s2 = s_row2 + sxs2;
      const int* s3 = s_row3 + sxs2;
      for (int x=0;x<nx2;++x)
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
      if (src_nx%2==1)
        *d = *s2;

      d_row += d_y_step;
      s_row1 += sys2;
      s_row2 += sys2;
      s_row3 += sys2;
  }

  // Need to set first and last rows as well

  // Dest image should be (src_nx+1)/2 x (src_ny+1)/2
  const int* s0 = src_im;
  int nx=(src_nx+1)/2;
  for (int i=0;i<nx;++i)
  {
    dest_im[i]= *s0;
    s0+=sxs2;
  }

  if (src_ny%2==1)
  {
    int yhi = (src_ny-1)/2;
    int* dest_last_row = dest_im + yhi*d_y_step;
    const int* s_last = src_im + yhi*sys2;
    for (int i=0;i<nx;++i)
    {
      dest_last_row[i]= *s_last;
      s_last+=sxs2;
    }
  }
}
