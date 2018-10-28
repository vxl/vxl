// This is mul/vil3d/algo/vil3d_corners.hxx
#ifndef vil3d_corners_hxx_
#define vil3d_corners_hxx_
//:
// \file
// \brief  Class containing functions to estimate corner-ness in various ways.
// \author Tim Cootes

#include "vil3d_corners.h"

#include <vil3d/vil3d_math.h>
#include <vil3d/vil3d_plane.h>
#include <vil3d/algo/vil3d_grad_3x3x3.h>
#include <vil3d/algo/vil3d_smooth_121.h>
#include <cassert>
#include <vcl_compiler.h>

//: Compute gradients, products and smoothed versions
template <class srcT, class destT>
void vil3d_corners<srcT,destT>::compute_smooth_gradient_products(const vil3d_image_view<srcT>& src_im)
{
  // Compute the gradients
  vil3d_grad_3x3x3(src_im,grad_i,grad_j,grad_k);

  unsigned ni=src_im.ni();
  unsigned nj=src_im.nj();
  unsigned nk=src_im.nk();

  // Set up grad_product_ and smooth_grad_product_ to be 6 plane
  // images, with the plane being the fastest varying (planestep=1)
  vil_pixel_format fmt = vil_pixel_format_of(destT());
  vil_memory_chunk_sptr chunk1 = new vil_memory_chunk(ni*nj*nk*6*sizeof(destT),
                                    vil_pixel_format_component_format(fmt));
  grad_product_ = vil3d_image_view<destT>(chunk1,
                                          reinterpret_cast<destT *>(chunk1->data()),
                                          ni, nj, nk, 6,
                                          6, 6*ni, 6*ni*nj, 1);
  vil_memory_chunk_sptr chunk2 = new vil_memory_chunk(ni*nj*nk*6*sizeof(destT),
                                    vil_pixel_format_component_format(fmt));
  smooth_grad_product_ = vil3d_image_view<destT>(chunk2,
                                          reinterpret_cast<destT *>(chunk2->data()),
                                          ni, nj, nk, 6,
                                          6, 6*ni, 6*ni*nj, 1);

  // Fill planes of grad_product_ with products of gradients
  vil3d_image_view<destT> gigi = vil3d_plane(grad_product_,0);
  vil3d_math_image_product(grad_i,grad_i,gigi);
  vil3d_image_view<destT> gigj = vil3d_plane(grad_product_,1);
  vil3d_math_image_product(grad_i,grad_j,gigj);
  vil3d_image_view<destT> gigk = vil3d_plane(grad_product_,2);
  vil3d_math_image_product(grad_i,grad_k,gigk);
  vil3d_image_view<destT> gjgj = vil3d_plane(grad_product_,3);
  vil3d_math_image_product(grad_j,grad_j,gjgj);
  vil3d_image_view<destT> gjgk = vil3d_plane(grad_product_,4);
  vil3d_math_image_product(grad_j,grad_k,gjgk);
  vil3d_image_view<destT> gkgk = vil3d_plane(grad_product_,5);
  vil3d_math_image_product(grad_k,grad_k,gkgk);

  // Smooth the products
  vil3d_smooth_121(grad_product_,smooth_grad_product_);
}

//: Computes cornerness measure for every pixel
//  At each pixel, computes det(N)/trace(N), where
//  N is a matrix generated by smoothing over gradient
//  product matrices M = (gi, gj, gk)' * (gi, gj, gk),
//  where gi = i-gradient at a pixel etc.
//
//  This is the Op3 corner operator described by Rohr.
template <class srcT, class destT>
void vil3d_corners<srcT,destT>::cornerness1(
                   const vil3d_image_view<srcT>& src_im,
                   vil3d_image_view<destT>& cornerness)
{
  assert(src_im.nplanes()==1);
  compute_smooth_gradient_products(src_im);

  unsigned ni=src_im.ni();
  unsigned nj=src_im.nj();
  unsigned nk=src_im.nk();

  // Compute the cornerness value
  cornerness.set_size(ni,nj,nk);

  std::ptrdiff_t c_istep=cornerness.istep();
  std::ptrdiff_t c_jstep=cornerness.jstep();
  std::ptrdiff_t c_kstep=cornerness.kstep();
  std::ptrdiff_t s_istep=smooth_grad_product_.istep();
  std::ptrdiff_t s_jstep=smooth_grad_product_.jstep();
  std::ptrdiff_t s_kstep=smooth_grad_product_.kstep();

  destT* c_data = cornerness.origin_ptr();
  destT* s_data = smooth_grad_product_.origin_ptr();
  for (unsigned k=0;k<nk;++k,c_data+=c_kstep,s_data+=s_kstep)
  {
    destT* c_row = c_data;
    destT* s_row = s_data;
    for (unsigned j=0;j<nj;++j,c_row+=c_jstep,s_row+=s_jstep)
    {
      destT* c_p=c_row;
      destT* s_p=s_row;
      for (unsigned i=0;i<ni;++i,c_p+=c_istep,s_p+=s_istep)
      {
        // detN = det(N) where
        // N = (s_p[0] s_p[1] s_p[2])
        //     (s_p[1] s_p[3] s_p[4])
        //     (s_p[2] s_p[4] s_p[5])
        destT detN =   s_p[0]*(s_p[3]*s_p[5]-s_p[4]*s_p[4])
                     - s_p[1]*(s_p[1]*s_p[5]-s_p[2]*s_p[4])
                     + s_p[2]*(s_p[1]*s_p[3]-s_p[2]*s_p[3]);
        destT trN  = s_p[0]+s_p[3]+s_p[5];
        if (trN<1e-4) *c_p=0;
        else          *c_p=detN/trN;
      }
    }
  }
}

#define VIL3D_CORNERS_INSTANTIATE(srcT, destT) \
template class vil3d_corners<srcT, destT >

#endif // vil3d_corners_hxx_
