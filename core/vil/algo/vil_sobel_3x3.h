#ifndef vil2_sobel_3x3_h_
#define vil2_sobel_3x3_h_
//: \file
//  \brief Apply 3x3 sobel operator to image data
//  \author Tim Cootes

#include <vil2/vil2_image_view.h>

//: Compute gradients of an image using 3x3 Sobel filters
//  Computes both i and j gradients of an ni x nj plane of data
//  1 pixel border around grad images is set to zero
// \relates vil2_image_view
template<class srcT, class destT>
void vil2_sobel_3x3(const vil2_image_view<srcT>& src,
                         vil2_image_view<destT>& grad_i,
                         vil2_image_view<destT>& grad_j);

//: Compute gradients of an image using 3x3 Sobel filters
//  Computes both i and j gradients of an nx x ny plane of data
//  grad_ij has twice as many planes as src, with dest plane (2i) being the i-gradient
//  of source plane i and dest plane (2i+1) being the j-gradient.
//  1 pixel border around grad images is set to zero
// \relates vil2_image_view
template<class srcT, class destT>
void vil2_sobel_3x3(const vil2_image_view<srcT>& src,
                         vil2_image_view<destT>& grad_ij);

//: Compute gradients of single plane of 2D data using 3x3 Sobel filters
//  Computes both i and j gradients of an ni x nj plane of data
//  1 pixel border around grad images is set to zero
void vil2_sobel_3x3_1plane(const unsigned char* src,
                         int s_istep, int s_jstep,
                         float* gi, int gi_istep, int gi_jstep,
                         float* gj, int gj_istep, int gj_jstep,
                         unsigned ni, unsigned nj);

//: Compute gradients of single plane of 2D data using 3x3 Sobel filters
//  Computes both i and j gradients of an ni x nj plane of data.
//  1 pixel border around grad images is set to zero
void vil2_sobel_3x3_1plane(const float* src,
                         int s_istep, int s_jstep,
                         float* gi, int gi_istep, int gi_jstep,
                         float* gj, int gj_istep, int gj_jstep,
                         unsigned ni, unsigned nj);

#endif
