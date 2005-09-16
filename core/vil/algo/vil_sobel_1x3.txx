// This is core/vil/algo/vil_sobel_1x3.txx
#ifndef vil_sobel_1x3_txx_
#define vil_sobel_1x3_txx_
//:
// \file
// \brief Apply sobel gradient filter to an image
// \author Tim Cootes

#include "vil_sobel_1x3.h"

//: Apply Sobel 1x3 gradient filter to image.
//  dest has twice as many planes as src, with dest plane (2i) being the i-gradient
//  of source plane i and dest plane (2i+1) being the j-gradient.
template<class srcT, class destT>
void vil_sobel_1x3(const vil_image_view<srcT>& src,
                   vil_image_view<destT>& grad_ij)
{
  unsigned np = src.nplanes();
  unsigned ni = src.ni();
  unsigned nj = src.nj();
  grad_ij.set_size(ni,nj,2*np);
  for (unsigned p=0;p<np;++p)
  {
    vil_sobel_1x3_1plane(src.top_left_ptr()+p*src.planestep(),
                         src.istep(),src.jstep(),
                         grad_ij.top_left_ptr()+2*p*grad_ij.planestep(),
                         grad_ij.istep(),grad_ij.jstep(),
                         grad_ij.top_left_ptr()+(2*p+1)*grad_ij.planestep(),
                         grad_ij.istep(),grad_ij.jstep(), ni,nj);
  }
}

//: Apply Sobel 1x3 gradient filter to 2D image
template<class srcT, class destT>
void vil_sobel_1x3(const vil_image_view<srcT>& src,
                   vil_image_view<destT>& grad_i,
                   vil_image_view<destT>& grad_j)
{
  unsigned np = src.nplanes();
  unsigned ni = src.ni();
  unsigned nj = src.nj();
  grad_i.set_size(ni,nj,np);
  grad_j.set_size(ni,nj,np);
  for (unsigned p=0;p<np;++p)
  {
    vil_sobel_1x3_1plane(src.top_left_ptr()+p*src.planestep(),
                         src.istep(),src.jstep(),
                         grad_i.top_left_ptr()+p*grad_i.planestep(),
                         grad_i.istep(),grad_i.jstep(),
                         grad_j.top_left_ptr()+p*grad_j.planestep(),
                         grad_j.istep(),grad_j.jstep(), ni,nj);
  }
}

template<class srcT, class destT>    
void vil_sobel_1x3_1plane(const srcT* src,
                          vcl_ptrdiff_t s_istep, vcl_ptrdiff_t s_jstep,
                          destT* gi, vcl_ptrdiff_t gi_istep, vcl_ptrdiff_t gi_jstep,
                          destT* gj, vcl_ptrdiff_t gj_istep, vcl_ptrdiff_t gj_jstep,
                          unsigned ni, unsigned nj)
{
  const destT point5=static_cast<destT>(0.5);
  const destT zero=static_cast<destT>(0.0);

  const srcT* s_data = src;
  destT* gi_data = gi;
  destT* gj_data = gj;

  if (ni==0 || nj==0) return;
  if (ni==1)
  {
      // Zero the elements in the column
    for (unsigned j=0;j<nj;++j)
    {
      *gi_data = zero;
      *gj_data = zero;
      gi_data += gi_jstep;
      gj_data += gj_jstep;
    }
    return;
  }
  if (nj==1)
  {
      // Zero the elements in the column
    for (unsigned i=0;i<ni;++i)
    {
      *gi_data = zero;
      *gj_data = zero;
      gi_data += gi_istep;
      gj_data += gj_istep;
    }
    return;
  }

  // Compute relative grid positions
  //     o2
  //  o4    o5
  //     o7
  const vcl_ptrdiff_t o2 = s_jstep;
  const vcl_ptrdiff_t o4 = -s_istep;
  const vcl_ptrdiff_t o5 = s_istep;
  const vcl_ptrdiff_t o7 = -s_jstep;

  const unsigned ni1 = ni-1;
  const unsigned nj1 = nj-1;

  s_data += s_istep + s_jstep;
  gi_data += gi_jstep;
  gj_data += gj_jstep;
  for (unsigned j=1;j<nj1;++j)
  {
    const srcT* s = s_data;
    destT* pgi = gi_data;
    destT* pgj = gj_data;

    // Zero the first elements in the rows
    *pgi = zero; pgi+=gi_istep;
    *pgj = zero; pgj+=gj_istep;


    for (unsigned i=1;i<ni1;++i)
    {
      // Compute gradient in i
      // Note: Multiply each element individually
      //      to ensure conversion to double before addition
      *pgi = point5*static_cast<destT>(s[o5]) - point5*static_cast<destT>(s[o4]);
      // Compute gradient in j
      *pgj = point5*static_cast<destT>(s[o2]) - point5*static_cast<destT>(s[o7]);

      s+=s_istep;
      pgi += gi_istep;
      pgj += gj_istep;
    }

    // Zero the last elements in the rows
    *pgi = zero;
    *pgj = zero;

    // Move to next row
    s_data  += s_jstep;
    gi_data += gi_jstep;
    gj_data += gj_jstep;
  }

  // Zero the first and last rows
  for (unsigned i=0;i<ni;++i)
  {
    *gi=zero; gi+=gi_istep;
    *gj=zero; gj+=gj_istep;
    *gi_data = zero; gi_data+=gi_istep;
    *gj_data = zero; gj_data+=gj_istep;
  }
}



#undef VIL_SOBEL_1X3_INSTANTIATE
#define VIL_SOBEL_1X3_INSTANTIATE(srcT, destT) \
template void vil_sobel_1x3(const vil_image_view< srcT >& src, \
                            vil_image_view<destT >& grad_ij); \
template void vil_sobel_1x3(const vil_image_view< srcT >& src, \
                            vil_image_view<destT >& grad_i, \
                            vil_image_view<destT >& grad_j)

#endif // vil_sobel_1x3_txx_
