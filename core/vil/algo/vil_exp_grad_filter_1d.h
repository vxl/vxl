#ifndef vil2_exp_grad_filter_1d_h_
#define vil2_exp_grad_filter_1d_h_
//:
//  \file
//  \brief Apply exponential gradient filter
//  \author Tim Cootes

#include <vil2/vil2_image_view.h>

//: Apply exponential gradient filter to 1D data. Form: sign(i)*exp(c*|i|)
//  Apply filter to n values src[i*sstep] to produce output dest[i*dstep]
//  Exponential gradient filter of the form
//  sign(i)*exp(c*|i|) applied. c=log(k)
//  Uses fast recursive implementation.
template <class srcT, class destT, class accumT>
inline void vil2_exp_grad_filter_1d(const srcT* src, int sstep,
                               destT* dest, int dstep,
                               int n, double k, accumT)
{
  const srcT* s = src;
  const srcT* src_end = src + (n-1)*sstep;

  // Zero first element
  dest[0]=0; dest+=dstep;

  // Forward pass to compute -ive part of filter response
  // Initialise for first element
  accumT rt= -1*(accumT) *s; s+=sstep;
  accumT k_sum=(accumT) 1;

  while (s!=src_end)
  {
    *dest = rt/k_sum;       // Set value for -ive half of filter
	rt *= k; k_sum *= k;    // Scale sums
    rt -= *s; k_sum += 1.0; // Increment with next element
    s+=sstep; dest+=dstep;  // Move to next element
  }

  // Backward pass to compute +ive part of filter response
  dest[0]=0; dest-=dstep;
  rt= (accumT) *s; s-=sstep;
  k_sum=(accumT) 1;
  src_end = src;
  while (s!=src_end)
  {
    *dest += rt/k_sum;      // Add in value for +ive half of filter
	rt *= k; k_sum *= k;    // Scale sums
    rt += *s; k_sum += 1.0; // Increment with next element
    s-=sstep; dest-=dstep;  // Move to next element
  }
}

//: Apply exponential gradient filter to src_im (along i direction).
//  Exponential gradient filter of the form sign(i)*exp(c*|i|) applied. c=log(k)
//  Uses fast recursive implementation.
// \relates vil2_image_view
template <class srcT, class destT, class accumT>
inline void vil2_exp_grad_filter_i(const vil2_image_view<srcT>& src_im,
                                    vil2_image_view<destT>& dest_im,
                                    double k, accumT ac)
{
  unsigned ni = src_im.ni();
  unsigned nj = src_im.nj();
  dest_im.resize(ni,nj,src_im.nplanes());
  int s_istep = src_im.istep(), s_jstep = src_im.jstep();
  int d_istep = dest_im.istep(),d_jstep = dest_im.jstep();

  for (unsigned p=0;p<src_im.nplanes();++p)
  {
    const srcT*  src_row  = src_im.top_left_ptr()+p*src_im.planestep();
    destT* dest_row = dest_im.top_left_ptr()+p*dest_im.planestep();
	// Filter every row
    for (unsigned j=0;j<nj;++j,src_row+=s_jstep,dest_row+=d_jstep)
      vil2_exp_grad_filter_1d(src_row,s_istep, dest_row,d_istep,   ni, k, ac);
  }
}

//: Apply exponential gradient filter to src_im (along j direction).
//  Exponential gradient filter of the form sign(j)*exp(c*|j|) applied. c=log(k)
//  Uses fast recursive implementation.
// \relates vil2_image_view
template <class srcT, class destT, class accumT>
inline void vil2_exp_grad_filter_j(const vil2_image_view<srcT>& src_im,
                                    vil2_image_view<destT>& dest_im,
                                    double k, accumT ac)
{
  unsigned ni = src_im.ni();
  unsigned nj = src_im.nj();
  dest_im.resize(ni,nj,src_im.nplanes());
  int s_istep = src_im.istep(), s_jstep = src_im.jstep();
  int d_istep = dest_im.istep(),d_jstep = dest_im.jstep();

  for (unsigned p=0;p<src_im.nplanes();++p)
  {
    const srcT*  src_col  = src_im.top_left_ptr()+p*src_im.planestep();
    destT* dest_col = dest_im.top_left_ptr()+p*dest_im.planestep();
	// Filter every column
    for (unsigned i=0;i<ni;++i,src_col+=s_istep,dest_col+=d_istep)
      vil2_exp_grad_filter_1d(src_col,s_jstep, dest_col,d_jstep,   nj, k, ac);
  }
}

#endif // vil2_exp_grad_filter_1d_h_
