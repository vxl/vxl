#ifndef mil_gauss_reduce_2d_h_
#define mil_gauss_reduce_2d_h_

//: \file
//  \brief Functions to smooth and sub-sample image in one direction
//  \author Tim Cootes

#include <vil/vil_byte.h>

//: Smooth and subsample single plane src_im in x to produce dest_im
//  Applies 1-5-8-5-1 filter in x, then samples
//  every other pixel.  Fills [0,(nx+1)/2-1][0,ny-1] elements of dest
//  Assumes dest_im has suffient data allocated.
//
//  This is essentially a utility function, used by mil_gauss_pyramid_builder
//
//  By applying twice we can obtain a full gaussian smoothed and
//  sub-sampled 2D image (see mil_gauss_pyramid_builder)
void mil_gauss_reduce_2d(vil_byte* dest_im,
                     int d_x_step, int d_y_step,
					 const vil_byte* src_im,
					 int src_nx, int src_ny,
					 int s_x_step, int s_y_step);



#endif // mil_gauss_reduce_2d_h_


