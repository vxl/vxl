// This is mul/mil3d/mil3d_gauss_reduce_3d.h
#ifndef mil3d_gauss_reduce_3d_h_
#define mil3d_gauss_reduce_3d_h_
//:
//  \file
//  \brief Functions to smooth and sub-sample 3D images in one direction
//  \author Tim Cootes
//  These are not templated because
//  a) Each type tends to need a slightly different implementation
//  b) Let's not have too many templates.

#include <vxl_config.h>

//: Smooth and subsample single plane src_im in x to produce dest_im
//  Applies 1-5-8-5-1 filter in x, then samples
//  every other pixel.  Fills [0,(nx+1)/2-1][0,ny-1][0,nz-1] elements of dest
//  Assumes dest_im has sufficient data allocated.
//
//  This is essentially a utility function, used by mil3d_gauss_pyramid_builder_3d
//
//  By applying three times we can obtain a full Gaussian smoothed and
//  sub-sampled 3D image (see mil3d_gauss_pyramid_builder_3d)
void mil3d_gauss_reduce_3d(vxl_byte* dest_im,
                           int d_x_step, int d_y_step, int d_z_step,
                           const vxl_byte* src_im,
                           int src_nx, int src_ny, int src_nz,
                           int s_x_step, int s_y_step, int s_z_step);

//: Smooth and subsample single plane src_im in x to produce dest_im
//  Applies 1-5-8-5-1 filter in x, then samples
//  every other pixel.  Fills [0,(nx+1)/2-1][0,ny-1][0,nz-1] elements of dest
//  Assumes dest_im has sufficient data allocated.
//
//  This is essentially a utility function, used by mil3d_gauss_pyramid_builder_3d
//
//  By applying three times we can obtain a full Gaussian smoothed and
//  sub-sampled 3D image (see mil3d_gauss_pyramid_builder_3d)
void mil3d_gauss_reduce_3d(float* dest_im,
                           int d_x_step, int d_y_step, int d_z_step,
                           const float* src_im,
                           int src_nx, int src_ny, int src_nz,
                           int s_x_step, int s_y_step, int s_z_step);


//: Smooth and subsample single plane src_im in x to produce dest_im
//  Applies 1-5-8-5-1 filter in x, then samples
//  every other pixel.  Fills [0,(nx+1)/2-1][0,ny-1][0,nz-1] elements of dest
//  Assumes dest_im has sufficient data allocated.
//
//  This is essentially a utility function, used by mil3d_gauss_pyramid_builder_3d
//
//  By applying three times we can obtain a full Gaussian smoothed and
//  sub-sampled 3D image (see mil3d_gauss_pyramid_builder_3d)
void mil3d_gauss_reduce_3d(int* dest_im,
                           int d_x_step, int d_y_step, int d_z_step,
                           const int* src_im,
                           int src_nx, int src_ny, int src_nz,
                           int s_x_step, int s_y_step, int s_z_step);


#endif // mil3d_gauss_reduce_3d_h_
