// This is core/vil/algo/vil_sobel_3x3.h
#ifndef vil_sobel_3x3_h_
#define vil_sobel_3x3_h_
//:
// \file
// \brief Apply 3x3 sobel operator to image data
// \author Tim Cootes

#include <vil/vil_image_view.h>

//: Compute gradients of an image using 3x3 Sobel filters
//  Computes both i and j gradients of an ni x nj plane of data
//  1 pixel border around grad images is set to zero
// \relates vil_image_view
template<class srcT, class destT>
void vil_sobel_3x3(const vil_image_view<srcT>& src,
                   vil_image_view<destT>& grad_i,
                   vil_image_view<destT>& grad_j);

//: Compute gradients of an image using 3x3 Sobel filters
//  Computes both i and j gradients of an nx x ny plane of data
//  grad_ij has twice as many planes as src, with dest plane (2i) being the i-gradient
//  of source plane i and dest plane (2i+1) being the j-gradient.
//  1 pixel border around grad images is set to zero
// \relates vil_image_view
template<class srcT, class destT>
void vil_sobel_3x3(const vil_image_view<srcT>& src,
                   vil_image_view<destT>& grad_ij);

//: Compute gradients of single plane of 2D data using 3x3 Sobel filters
//  Computes both i and j gradients of an ni x nj plane of data
//  1 pixel border around grad images is set to zero
void vil_sobel_3x3_1plane(const unsigned char* src,
                          vcl_ptrdiff_t s_istep, vcl_ptrdiff_t s_jstep,
                          float* gi, vcl_ptrdiff_t gi_istep, vcl_ptrdiff_t gi_jstep,
                          float* gj, vcl_ptrdiff_t gj_istep, vcl_ptrdiff_t gj_jstep,
                          unsigned ni, unsigned nj);

//: Compute gradients of single plane of 2D data using 3x3 Sobel filters
//  Computes both i and j gradients of an ni x nj plane of data
//  1 pixel border around grad images is set to zero
void vil_sobel_3x3_1plane(const unsigned char* src,
                          vcl_ptrdiff_t s_istep, vcl_ptrdiff_t s_jstep,
                          double* gi, vcl_ptrdiff_t gi_istep, vcl_ptrdiff_t gi_jstep,
                          double* gj, vcl_ptrdiff_t gj_istep, vcl_ptrdiff_t gj_jstep,
                          unsigned ni, unsigned nj);

//: Compute gradients of single plane of 2D data using 3x3 Sobel filters
//  Computes both i and j gradients of an ni x nj plane of data.
//  1 pixel border around grad images is set to zero
void vil_sobel_3x3_1plane(const float* src,
                          vcl_ptrdiff_t s_istep, vcl_ptrdiff_t s_jstep,
                          float* gi, vcl_ptrdiff_t gi_istep, vcl_ptrdiff_t gi_jstep,
                          float* gj, vcl_ptrdiff_t gj_istep, vcl_ptrdiff_t gj_jstep,
                          unsigned ni, unsigned nj);

//: Compute gradients of single plane of 2D data using 3x3 Sobel filters
//  Computes both i and j gradients of an ni x nj plane of data.
//  1 pixel border around grad images is set to zero
void vil_sobel_3x3_1plane(const double* src,
                          vcl_ptrdiff_t s_istep, vcl_ptrdiff_t s_jstep,
                          double* gi, vcl_ptrdiff_t gi_istep, vcl_ptrdiff_t gi_jstep,
                          double* gj, vcl_ptrdiff_t gj_istep, vcl_ptrdiff_t gj_jstep,
                          unsigned ni, unsigned nj);

#endif // vil_sobel_3x3_h_
