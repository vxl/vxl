// This is mul/vil3d/algo/vil3d_grad_1x3.txx
#ifndef vil3d_grad_1x3_txx_
#define vil3d_grad_1x3_txx_
//:
// \file
// \brief Apply grad gradient filter to an image
// \author Tim Cootes

#include "vil3d_grad_1x3.h"
#include <vil3d/vil3d_plane.h>
#include <vil3d/vil3d_slice.h>






//: Compute 1 gradient of single plane of 3D data using 1x3 grad filters
template <class srcT, class gradT, class accumT>
void vil3d_grad_1x3_1dir(const srcT *src,
                         gradT * grad, vcl_ptrdiff_t delta_step,
                         vcl_ptrdiff_t isrc, vcl_ptrdiff_t jsrc, vcl_ptrdiff_t ksrc,
                         vcl_ptrdiff_t igrad, vcl_ptrdiff_t jgrad, vcl_ptrdiff_t kgrad,
                         unsigned ni, unsigned nj, unsigned nk, accumT /*dummy*/ )
{
  for (unsigned k=0;k<nk;++k)    
    for (unsigned j=0;j<nj;++j)
    {      
      const srcT* s = src + ksrc*k + jsrc*j;
      gradT* g = grad + kgrad*k + jgrad*j;

      for (unsigned i=0; i<ni; ++i)
      {
        // Compute gradient
        // Note: Multiply each element individually
        //      to ensure conversion to float before subtraction
        *g = static_cast<gradT>(
             (static_cast<accumT>(s[delta_step]) - static_cast<accumT>(s[-delta_step]))
              / static_cast<accumT>(2));

        s += isrc;
        g += igrad;
      }
    }
}




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
    vil3d_grad_1x3_1dir(&src(1,0,0,p), &grad_i_plane(1,0,0), src.istep(),
                        src.istep(), src.jstep(), src.kstep(),
                        grad_i_plane.istep(), grad_i_plane.jstep(), grad_i_plane.kstep(),
                        ni-2, nj, nk, srcT());
    vil3d_grad_1x3_1dir(&src(0,1,0,p), &grad_j_plane(0,1,0), src.jstep(),
                        src.istep(), src.jstep(), src.kstep(),
                        grad_j_plane.istep(), grad_j_plane.jstep(), grad_j_plane.kstep(),
                        ni, nj-2, nk, srcT());
    vil3d_grad_1x3_1dir(&src(0,0,1,p), &grad_k_plane(0,0,1), src.kstep(),
                        src.istep(), src.jstep(), src.kstep(),
                        grad_k_plane.istep(), grad_k_plane.jstep(), grad_k_plane.kstep(),
                        ni, nj, nk-2, srcT());
    
    vil3d_slice_jk(grad_i_plane,0).fill(0.0f);
    vil3d_slice_jk(grad_i_plane,ni-1).fill(0.0f);
    vil3d_slice_ik(grad_j_plane,0).fill(0.0f);
    vil3d_slice_ik(grad_j_plane,nj-1).fill(0.0f);
    vil3d_slice_ij(grad_k_plane,0).fill(0.0f);
    vil3d_slice_ij(grad_k_plane,nk-1).fill(0.0f);
  }
}

//: Apply grad 1x3 gradient filter to 3D image
template<class srcT, class destT>
void vil3d_grad_1x3(const vil3d_image_view<srcT>& src,
                    vil3d_image_view<destT>& grad_i,
                    vil3d_image_view<destT>& grad_j,
                    vil3d_image_view<destT>& grad_k)
{
  unsigned ni = src.ni();
  unsigned nj = src.nj();
  unsigned nk = src.nk();
  unsigned np = src.nplanes();
  grad_i.set_size(ni,nj,nk,np);
  grad_j.set_size(ni,nj,nk,np);
  grad_k.set_size(ni,nj,nk,np);
  for (unsigned p=0;p<np;++p)
  {
    vil3d_grad_1x3_1dir(&src(1,0,0,p), &grad_i(1,0,0,p), src.istep(),
                        src.istep(), src.jstep(), src.kstep(),
                        grad_i.istep(), grad_i.jstep(), grad_i.kstep(),
                        ni-2, nj, nk, srcT());
    vil3d_grad_1x3_1dir(&src(0,1,0,p), &grad_j(0,1,0,p), src.jstep(),
                        src.istep(), src.jstep(), src.kstep(),
                        grad_j.istep(), grad_j.jstep(), grad_j.kstep(),
                        ni, nj-2, nk, srcT());
    vil3d_grad_1x3_1dir(&src(0,0,1,p), &grad_k(0,0,1,p), src.kstep(),
                        src.istep(), src.jstep(), src.kstep(),
                        grad_k.istep(), grad_k.jstep(), grad_k.kstep(),
                        ni, nj, nk-2, srcT());
  }
  vil3d_slice_jk(grad_i,0).fill(0.0f);
  vil3d_slice_jk(grad_i,ni-1).fill(0.0f);
  vil3d_slice_ik(grad_j,0).fill(0.0f);
  vil3d_slice_ik(grad_j,nj-1).fill(0.0f);
  vil3d_slice_ij(grad_k,0).fill(0.0f);
  vil3d_slice_ij(grad_k,nk-1).fill(0.0f);

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
