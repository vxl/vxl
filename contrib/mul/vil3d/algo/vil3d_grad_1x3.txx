// This is mul/vil3d/algo/vil3d_grad_1x3.txx
#ifndef vil3d_grad_1x3_txx_
#define vil3d_grad_1x3_txx_
//:
// \file
// \brief Apply grad gradient filter to an image
// \author Tim Cootes

#include "vil3d_grad_1x3.h"
#include <vil3d/vil3d_plane.h>

//: Compute gradients of an image using (-0.5 0 0.5) grad filters
//  Computes both i,j and k gradients of an ni x nj x nk plane of data
//  1 pixel border around grad images is set to zero
template<class srcT, class destT>
void vil3d_grad_1x3(const vil3d_image_view<srcT>& src,
                    vil3d_image_view<destT>& grad_ijk)
{
  unsigned np = src.nplanes();
  unsigned ni = src.ni();
  unsigned nj = src.nj();
  unsigned nk = src.nk();
  grad_ijk.set_size(ni,nj,nk,3*np);
  for (unsigned p=0;p<np;++p)
  {
    vil3d_image_view<destT> grad_i_plane = vil3d_plane(grad_ijk,3*p);
    vil3d_image_view<destT> grad_j_plane = vil3d_plane(grad_ijk,3*p+1);
    vil3d_image_view<destT> grad_k_plane = vil3d_plane(grad_ijk,3*p+2);
    vil3d_grad_1x3_1plane(vil3d_plane(src,p),
                          grad_i_plane,grad_j_plane,grad_k_plane);
  }
}

//: Apply grad 1x3 gradient filter to 2D image
template<class srcT, class destT>
void vil3d_grad_1x3(const vil3d_image_view<srcT>& src,
                    vil3d_image_view<destT>& grad_i,
                    vil3d_image_view<destT>& grad_j,
                    vil3d_image_view<destT>& grad_k)
{
  unsigned np = src.nplanes();
  unsigned ni = src.ni();
  unsigned nj = src.nj();
  unsigned nk = src.nk();
  grad_i.set_size(ni,nj,nk,np);
  grad_j.set_size(ni,nj,nk,np);
  grad_k.set_size(ni,nj,nk,np);
  for (unsigned p=0;p<np;++p)
  {
    vil3d_image_view<destT> grad_i_plane = vil3d_plane(grad_i,p);
    vil3d_image_view<destT> grad_j_plane = vil3d_plane(grad_j,p);
    vil3d_image_view<destT> grad_k_plane = vil3d_plane(grad_k,p);
    vil3d_grad_1x3_1plane(vil3d_plane(src,p),
                          grad_i_plane,grad_j_plane,grad_k_plane);
  }
}

//: Compute square gradient magnitude of 3D image
//  Use (-0.5,0,+0.5) filters in i,j,k
template<class srcT, class destT>
void vil3d_grad_1x3_mag_sq(const vil3d_image_view<srcT>& src,
                           vil3d_image_view<destT>& grad_mag2)
{
  unsigned np = src.nplanes();
  unsigned ni = src.ni();
  unsigned nj = src.nj();
  unsigned nk = src.nk();
  grad_mag2.set_size(ni,nj,nk,np);
  for (unsigned p=0;p<np;++p)
  {
    vil3d_image_view<destT> grad_plane = vil3d_plane(grad_mag2,p);
    vil3d_grad_1x3_mag_sq_1plane(vil3d_plane(src,p),grad_plane);
  }
}


#undef vil3d_GRAD_1X3_INSTANTIATE
#define vil3d_GRAD_1X3_INSTANTIATE(srcT, destT) \
template void vil3d_grad_1x3(const vil3d_image_view< srcT >& src, \
                                   vil3d_image_view<destT >& grad_ijk); \
template void vil3d_grad_1x3(const vil3d_image_view< srcT >& src, \
                                   vil3d_image_view<destT >& grad_i, \
                                   vil3d_image_view<destT >& grad_j, \
                                   vil3d_image_view<destT >& grad_k); \
template void vil3d_grad_1x3_mag_sq(const vil3d_image_view<srcT >& src, \
                                   vil3d_image_view<destT >& grad_mag2)

#endif // vil3d_grad_1x3_txx_
