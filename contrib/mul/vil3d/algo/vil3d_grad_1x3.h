#ifndef vil3d_grad_1x3_h_
#define vil3d_grad_1x3_h_
//:
//  \file
//  \brief Apply 1x3 gradient operator (-0.5 0 0.5) to image data
//  \author Tim Cootes

#include <vil3d/vil3d_image_view.h>
#include <vxl_config.h>

//: Compute gradients of an image using (-0.5 0 0.5) Sobel filters
//  Computes both i,j and k gradients of an ni x nj x nk plane of data
//  1 pixel border around grad images is set to zero
// \relates vil3d_image_view
template<class srcT, class destT>
void vil3d_grad_1x3(const vil3d_image_view<srcT>& src,
                    vil3d_image_view<destT>& grad_i,
                    vil3d_image_view<destT>& grad_j,
                    vil3d_image_view<destT>& grad_k);

//: Compute gradients of an image using (-0.5 0 0.5) filters
//  Computes both i,j and k gradients of an image.
//  grad_ijk has three times as many planes as src, with dest plane (3i) being the i-gradient
//  of source plane i and dest plane (3i+1) being the j-gradient etc
//  1 pixel border around grad images is set to zero
// \relates vil3d_image_view
template<class srcT, class destT>
void vil3d_grad_1x3(const vil3d_image_view<srcT>& src,
                    vil3d_image_view<destT>& grad_ijk);

//: Compute gradients of single plane of 2D data using (-0.5 0 0.5) filters
//  Computes both i,j and k gradients of an ni x nj x nk plane of data
//  1 pixel border around grad images is set to zero
void vil3d_grad_1x3_1plane(const vil3d_image_view<vxl_byte>& src,
                    vil3d_image_view<float>& grad_i,
                    vil3d_image_view<float>& grad_j,
                    vil3d_image_view<float>& grad_k);

//: Compute gradients of single plane of 2D data using (-0.5 0 0.5) filters
//  Computes both i,j and k gradients of an ni x nj x nk plane of data
//  1 pixel border around grad images is set to zero
void vil3d_grad_1x3_1plane(const vil3d_image_view<float>& src,
                    vil3d_image_view<float>& grad_i,
                    vil3d_image_view<float>& grad_j,
                    vil3d_image_view<float>& grad_k);

#endif // vil3d_grad_1x3_h_
