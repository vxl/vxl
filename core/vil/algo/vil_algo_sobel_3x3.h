#ifndef vil2_algo_sobel_3x3_h_
#define vil2_algo_sobel_3x3_h_
//: \file
//  \brief Apply 3x3 sobel operator to image data
//  \author Tim Cootes

#include <vil2/vil2_image_view.h>

//: Compute gradients of single plane of 2D data using 3x3 Sobel filters
//  Computes both x and y gradients of an nx x ny plane of data
//  1 pixel border around grad images is set to zero
void vil2_algo_sobel_3x3_1plane(float* gx, int gx_xstep, int gx_ystep,
                         float* gy, int gy_xstep, int gy_ystep,
                         const unsigned char* src,
                         int s_xstep, int s_ystep, unsigned nx, unsigned ny);

//: Compute gradients of single plane of 2D data using 3x3 Sobel filters
//  Computes both x and y gradients of an nx x ny plane of data.
//  1 pixel border around grad images is set to zero
void vil2_algo_sobel_3x3_1plane(float* gx, int gx_xstep, int gx_ystep,
                         float* gy, int gy_xstep, int gy_ystep,
                         const float* src,
                         int s_xstep, int s_ystep, unsigned nx, unsigned ny);

//: Compute gradients of an image using 3x3 Sobel filters
//  Computes both x and y gradients of an nx x ny plane of data
//  1 pixel border around grad images is set to zero
template<class srcT, class destT>
void vil2_algo_sobel_3x3(vil2_image_view<destT>& grad_x,
                         vil2_image_view<destT>& grad_y,
                         vil2_image_view<srcT>& src);

//: Compute gradients of an image using 3x3 Sobel filters
//  Computes both x and y gradients of an nx x ny plane of data
//  grad_xy has twice as many planes as src, with dest plane (2i) being the x-gradient
//  of source plane i and dest plane (2i+1) being the y-gradient.
//  1 pixel border around grad images is set to zero
template<class srcT, class destT>
void vil2_algo_sobel_3x3(vil2_image_view<destT>& grad_xy,
                         vil2_image_view<srcT>& src);
#endif
