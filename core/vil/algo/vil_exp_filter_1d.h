#ifndef vil_exp_filter_1d_h_
#define vil_exp_filter_1d_h_
//:
// \file
// \brief Apply exponential filter
// \author Tim Cootes

#include <vil/vil_image_view.h>

//: Apply exponential filter to 1D data
//  Apply filter to n values src[i*sstep] to produce output dest[i*dstep]
//  Symmetric exponential filter of the form exp(c*|x|) applied. c=log(k)
//  Uses fast recursive implementation.
template <class srcT, class destT, class accumT>
inline void vil_exp_filter_1d(const srcT* src, std::ptrdiff_t sstep,
                              destT* dest, std::ptrdiff_t dstep,
                              int n, accumT k)
{
  const srcT* s = src;
  const srcT* src_end = src + n*sstep;
  double f = (1-k)/(1+k);

  // Forward pass
  accumT rt=0;
  while (s!=src_end)
  {
    rt += *s;
    *dest = destT(f * rt);
    rt *= k;
    s+=sstep; dest+=dstep;
  }

  // Backward pass
  s-=sstep; dest-=dstep;
  src_end = src-sstep;
  rt=0;
  while (s!=src_end)
  {
    // Central value already included once, so only add it after updating dest.
    *dest += destT(f * rt);
    rt += *s;
    rt *= k;
    s-=sstep; dest-=dstep;
  }
}

//: Apply exponential filter along i to src_im to produce dest_im
//  Symmetric exponential filter of the form exp(c*|i|) applied. c=log(k)
//  Uses fast recursive implementation.
// \relatesalso vil_image_view
template <class srcT, class destT, class accumT>
inline void vil_exp_filter_i(const vil_image_view<srcT>& src_im,
                             vil_image_view<destT>& dest_im,
                             accumT k)
{
  unsigned ni = src_im.ni();
  unsigned nj = src_im.nj();
  dest_im.set_size(ni,nj,src_im.nplanes());
  std::ptrdiff_t s_istep = src_im.istep(), s_jstep = src_im.jstep();
  std::ptrdiff_t d_istep = dest_im.istep(),d_jstep = dest_im.jstep();

  for (unsigned p=0;p<src_im.nplanes();++p)
  {
    const srcT*  src_row  = src_im.top_left_ptr()+p*src_im.planestep();
    destT* dest_row = dest_im.top_left_ptr()+p*dest_im.planestep();
    // Filter each row
    for (unsigned j=0;j<nj;++j,src_row+=s_jstep,dest_row+=d_jstep)
      vil_exp_filter_1d(src_row,s_istep, dest_row,d_istep,   ni, k);
  }
}

//: Apply exponential filter along j to src_im to produce dest_im
//  Symmetric exponential filter of the form exp(c*|j|) applied. c=log(k)
//  Uses fast recursive implementation.
// \relatesalso vil_image_view
template <class srcT, class destT, class accumT>
inline void vil_exp_filter_j(const vil_image_view<srcT>& src_im,
                             vil_image_view<destT>& dest_im,
                             accumT k)
{
  unsigned ni = src_im.ni();
  unsigned nj = src_im.nj();
  dest_im.set_size(ni,nj,src_im.nplanes());
  std::ptrdiff_t s_istep = src_im.istep(), s_jstep = src_im.jstep();
  std::ptrdiff_t d_istep = dest_im.istep(),d_jstep = dest_im.jstep();

  for (unsigned p=0;p<src_im.nplanes();++p)
  {
    const srcT*  src_col  = src_im.top_left_ptr()+p*src_im.planestep();
    destT* dest_col = dest_im.top_left_ptr()+p*dest_im.planestep();
    // Filter each col
    for (unsigned i=0;i<ni;++i,src_col+=s_istep,dest_col+=d_istep)
      vil_exp_filter_1d(src_col,s_jstep, dest_col,d_jstep,   nj, k);
  }
}

#endif // vil_exp_filter_1d_h_
