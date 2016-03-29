#ifndef vil3d_exp_filter_h_
#define vil3d_exp_filter_h_
//:
// \file
// \brief Apply exponential filter to 3D images
// \author Tim Cootes

#include <vil/algo/vil_exp_filter_1d.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_switch_axes.h>

//: Apply exponential filter along i to src_im to produce dest_im
//  Symmetric exponential filter of the form exp(c*|i|) applied. c=log(kf)
//  Uses fast recursive implementation.
// \relatesalso vil_image_view
template <class srcT, class destT, class accumT>
inline void vil3d_exp_filter_i(const vil3d_image_view<srcT>& src_im,
                               vil3d_image_view<destT>& dest_im,
                               accumT kf)
{
  unsigned ni = src_im.ni();
  unsigned nj = src_im.nj();
  unsigned nk = src_im.nk();
  dest_im.set_size(ni,nj,nk,src_im.nplanes());
  std::ptrdiff_t s_istep = src_im.istep(), s_jstep = src_im.jstep();
  std::ptrdiff_t s_kstep = src_im.kstep();
  std::ptrdiff_t d_istep = dest_im.istep(),d_jstep = dest_im.jstep();
  std::ptrdiff_t d_kstep = dest_im.kstep();

  for (unsigned p=0;p<src_im.nplanes();++p)
  {
    const srcT*  src_slice  = src_im.origin_ptr()+p*src_im.planestep();
    destT* dest_slice = dest_im.origin_ptr()+p*dest_im.planestep();
    // Filter slice
    for (unsigned k=0;k<nk;++k,src_slice+=s_kstep,dest_slice+=d_kstep)
    {
      const srcT* src_row = src_slice;
      destT* dest_row = dest_slice;
      // Filter each row
      for (unsigned j=0;j<nj;++j,src_row+=s_jstep,dest_row+=d_jstep)
        vil_exp_filter_1d(src_row,s_istep, dest_row,d_istep,   ni, kf);
    }
  }
}

//: Apply exponential filter along each axis of src_im in turn
//  Symmetric exponential filter of the form exp(c*|i|) applied.
//  c=log(ki) along i, log(kj) along j, log(kk) along k.
//  Uses fast recursive implementation.
// \relatesalso vil_image_view
template <class srcT, class destT, class accumT>
inline void vil3d_exp_filter(const vil3d_image_view<srcT>& src_im,
                             vil3d_image_view<destT>& dest_im,
                             accumT ki, accumT kj, accumT kk)
{
  // Smooth along i axis
  vil3d_image_view<destT> smooth_i;
  vil3d_exp_filter_i(src_im,smooth_i,ki);

  // Smooth along j axis
  vil3d_image_view<destT> smooth_j;
  vil3d_exp_filter_i(vil3d_switch_axes_jik(smooth_i),
                     smooth_j,kj);
    // Note that smooth_j is nj * ni * nk

  // Smooth along k axis
  dest_im.set_size(src_im.ni(),src_im.nj(),src_im.nk(),src_im.nplanes());
  vil3d_image_view<destT> dest_kij = vil3d_switch_axes_kij(dest_im);

  // Use of vil3d_switch_axes_kji ensures axes are nk*ni*nj to match dest_kij
  vil3d_exp_filter_i(vil3d_switch_axes_kji(smooth_j), dest_kij,kk);
}

//: Apply exponential filter along each axis of src_im in turn
//  Symmetric exponential filter of the form exp(c*|i|) applied. c=log(kf)
//  Uses fast recursive implementation.
// \relatesalso vil_image_view
template <class srcT, class destT, class accumT>
inline void vil3d_exp_filter(const vil3d_image_view<srcT>& src_im,
                             vil3d_image_view<destT>& dest_im,
                             accumT kf)
{
  vil3d_exp_filter(src_im,dest_im,kf,kf,kf);
}

#endif // vil3d_exp_filter_h_
