#ifndef vil2_algo_exp_filter_1d_h_
#define vil2_algo_exp_filter_1d_h_

//: \file
//  \brief Apply exponential filter
//  \author Tim Cootes

#include <vil2/vil2_image_view.h>

//: Apply exponential filter to 1D data
//  Apply filter to n values src[i*sstep] to produce output dest[i*dstep]
//  Symmetric exponential filter of the form exp(c*|x|) applied. c=log(k)
//  Uses fast recursive implementation.
template <class srcT, class destT, class accumT>
inline void vil2_algo_exp_filter_1d(const srcT* src, int sstep,
                                    destT* dest, int dstep,
                                    int n, double k, accumT)
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

//: Apply exponential filter to src_im to produce dest_im
//  Symmetric exponential filter of the form exp(c*|x|) applied. c=log(k)
//  Uses fast recursive implementation.
// \relates vil2_image_view
template <class srcT, class destT, class accumT>
inline void vil2_algo_exp_filter_1d(const vil2_image_view<srcT>& src_im,
                                    vil2_image_view<destT>& dest_im,
                                    double k, accumT ac)
{
  unsigned ni = src_im.ni();
  unsigned nj = src_im.nj();
  int s_istep = src_im.istep(), s_jstep = src_im.jstep();
  int d_istep = dest_im.istep(),d_jstep = dest_im.jstep();

  dest_im.resize(ni,nj,src_im.nplanes());

  for (int p=0;p<src_im.nplanes();++p)
  {
    const srcT*  src_row  = src_im.top_left_ptr()+p*src_im.pstep();
    const destT* dest_row = dest_im.top_left_ptr()+p*dest_im.pstep();

  for (int j=0;j<nj;++j,src_row+=s_jstep,dest_row+=d_jstep)
      vil2_algo_exp_filter_1d(src_row,s_istep, dest_row,d_istep,   ni, ac);
  }
}

#endif

