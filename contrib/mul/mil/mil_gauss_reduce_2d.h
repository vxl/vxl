#ifndef mil_gauss_reduce_2d_h_
#define mil_gauss_reduce_2d_h_
//:
//  \file
//  \brief Functions to smooth and sub-sample image in one direction
//  \author Tim Cootes
//  These are not templated because
//  a) Each type tends to need a slightly different implementation
//  b) Let's not have too many templates.

#include <vxl_config.h>

//: Smooth and subsample single plane src_im in x to produce dest_im
//  Applies 1-5-8-5-1 filter in x, then samples
//  every other pixel.  Fills [0,(nx+1)/2-1][0,ny-1] elements of dest
//  Assumes dest_im has sufficient data allocated.
//
//  This is essentially a utility function, used by mil_gauss_pyramid_builder
//
//  By applying twice we can obtain a full Gaussian smoothed and
//  sub-sampled 2D image (see mil_gauss_pyramid_builder)
void mil_gauss_reduce_2d(vxl_byte* dest_im,
                         int d_x_step, int d_y_step,
                         const vxl_byte* src_im,
                         int src_nx, int src_ny,
                         int s_x_step, int s_y_step);

//: Smooth and subsample single plane src_im in x to produce dest_im
//  Applies 1-5-8-5-1 filter in x, then samples
//  every other pixel.  Fills [0,(nx+1)/2-1][0,ny-1] elements of dest
//  Assumes dest_im has sufficient data allocated.
//
//  This is essentially a utility function, used by mil_gauss_pyramid_builder
//
//  By applying twice we can obtain a full Gaussian smoothed and
//  sub-sampled 2D image (see mil_gauss_pyramid_builder)
void mil_gauss_reduce_2d(float* dest_im,
                         int d_x_step, int d_y_step,
                         const float* src_im,
                         int src_nx, int src_ny,
                         int s_x_step, int s_y_step);

//: Smooth and subsample single plane src_im in x to produce dest_im
//  Applies 1-5-8-5-1 filter in x, then samples
//  every other pixel.  Fills [0,(nx+1)/2-1][0,ny-1] elements of dest
//  Assumes dest_im has sufficient data allocated.
//
//  This is essentially a utility function, used by mil_gauss_pyramid_builder
//
//  By applying twice we can obtain a full Gaussian smoothed and
//  sub-sampled 2D image (see mil_gauss_pyramid_builder)
void mil_gauss_reduce_2d(int* dest_im,
                         int d_x_step, int d_y_step,
                         const int* src_im,
                         int src_nx, int src_ny,
                         int s_x_step, int s_y_step);

//: Smooth and subsample single plane src_im in x to produce dest_im using 121 filter in x and y
//  Smoothes with a 3x3 filter and subsamples
void mil_gauss_reduce_121_2d(vxl_byte* dest_im,
                             int d_x_step, int d_y_step,
                             const vxl_byte* src_im,
                             int src_nx, int src_ny,
                             int s_x_step, int s_y_step);

//: Smooth and subsample single plane src_im in x to produce dest_im using 121 filter in x and y
//  Smoothes with a 3x3 filter and subsamples
void mil_gauss_reduce_121_2d(float* dest_im,
                             int d_x_step, int d_y_step,
                             const float* src_im,
                             int src_nx, int src_ny,
                             int s_x_step, int s_y_step);

//: Smooth and subsample single plane src_im in x to produce dest_im using 121 filter in x and y
//  Smoothes with a 3x3 filter and subsamples
void mil_gauss_reduce_121_2d(int* dest_im,
                             int d_x_step, int d_y_step,
                             const int* src_im,
                             int src_nx, int src_ny,
                             int s_x_step, int s_y_step);

#endif // mil_gauss_reduce_2d_h_
