// This is mul/mil3d/mil3d_gauss_reduce_3d.cxx
#include "mil3d_gauss_reduce_3d.h"
//:
//  \file
//  \brief Functions to smooth and sub-sample image in one direction
//  \author Tim Cootes

#include <mil/mil_gauss_reduce_2d.h>


//: Smooth and subsample single plane src_im in x to produce dest_im
//  Applies 1-5-8-5-1 filter in x, then samples every other pixel.
//  Fills [0,(nx+1)/2-1][0,ny-1][0,nz-1] elements of dest.
void mil3d_gauss_reduce_3d(vxl_byte* dest_im,
                           int d_x_step, int d_y_step, int d_z_step,
                           const vxl_byte* src_im,
                           int src_nx, int src_ny, int src_nz,
                           int s_x_step, int s_y_step, int s_z_step)
{
  for (int z=0;z<src_nz;++z)
  {
    mil_gauss_reduce_2d(dest_im,d_x_step, d_y_step,
                        src_im, src_nx,src_ny, s_x_step,s_y_step);
    dest_im += d_z_step;
    src_im  += s_z_step;
  }
}

//: Smooth and subsample single plane src_im in x to produce dest_im
//  Applies 1-5-8-5-1 filter in x, then samples every other pixel.
//  Fills [0,(nx+1)/2-1][0,ny-1][0,nz-1] elements of dest.
void mil3d_gauss_reduce_3d(float* dest_im,
                           int d_x_step, int d_y_step, int d_z_step,
                           const float* src_im,
                           int src_nx, int src_ny, int src_nz,
                           int s_x_step, int s_y_step, int s_z_step)
{
  for (int z=0;z<src_nz;++z)
  {
    mil_gauss_reduce_2d(dest_im,d_x_step, d_y_step,
                        src_im, src_nx,src_ny, s_x_step,s_y_step);
    dest_im += d_z_step;
    src_im  += s_z_step;
  }
}


//: Smooth and subsample single plane src_im in x to produce dest_im
//  Applies 1-5-8-5-1 filter in x, then samples every other pixel.
//  Fills [0,(nx+1)/2-1][0,ny-1][0,nz-1] elements of dest.
void mil3d_gauss_reduce_3d(int* dest_im,
                           int d_x_step, int d_y_step, int d_z_step,
                           const int* src_im,
                           int src_nx, int src_ny, int src_nz,
                           int s_x_step, int s_y_step, int s_z_step)
{
  for (int z=0;z<src_nz;++z)
  {
    mil_gauss_reduce_2d(dest_im,d_x_step, d_y_step,
                        src_im, src_nx,src_ny, s_x_step,s_y_step);
    dest_im += d_z_step;
    src_im  += s_z_step;
  }
}

