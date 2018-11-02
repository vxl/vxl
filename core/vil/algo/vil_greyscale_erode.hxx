#ifndef vil_greyscale_erode_hxx_
#define vil_greyscale_erode_hxx_
//:
// \file
// \brief Perform greyscale erosion on images
// \author Tim Cootes

#include "vil_greyscale_erode.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Erodes src_image to produce dest_image (assumed single plane).
// dest_image(i0,j0) is the maximum value of the pixels under the
// structuring element when it is centred on src_image(i0,j0)
template <class T>
void vil_greyscale_erode(const vil_image_view<T>& src_image,
                         vil_image_view<T>& dest_image,
                         const vil_structuring_element& element)
{
  assert(src_image.nplanes()==1);
  unsigned ni = src_image.ni();
  unsigned nj = src_image.nj();
  dest_image.set_size(ni,nj,1);

  std::ptrdiff_t s_istep = src_image.istep(),  s_jstep = src_image.jstep(),
                d_istep = dest_image.istep(), d_jstep = dest_image.jstep();

  const T* src_row0 = src_image.top_left_ptr();
  T* dest_row0 = dest_image.top_left_ptr();

  std::vector<std::ptrdiff_t> offset;
  vil_compute_offsets(offset,element,s_istep,s_jstep);

  // Define box in which all element will be valid
  int ilo = -element.min_i();
  int ihi = ni-1-element.max_i();
  int jlo = -element.min_j();
  int jhi = nj-1-element.max_j();

  // Deal with left edge
  for (int i=0;i<ilo;++i)
    for (unsigned int j=0;j<nj;++j)
      dest_image(i,j,0)=vil_greyscale_erode(src_image,0,element,i,j);
  // Deal with right edge
  for (unsigned int i=ihi+1;i<ni;++i)
    for (unsigned int j=0;j<nj;++j)
      dest_image(i,j,0)=vil_greyscale_erode(src_image,0,element,i,j);
  // Deal with bottom edge
  for (int i=ilo;i<=ihi;++i)
    for (int j=0;j<jlo;++j)
      dest_image(i,j,0)=vil_greyscale_erode(src_image,0,element,i,j);
  // Deal with top edge
  for (int i=ilo;i<=ihi;++i)
    for (unsigned int j=jhi+1;j<nj;++j)
      dest_image(i,j,0)=vil_greyscale_erode(src_image,0,element,i,j);

  for (int j=jlo;j<=jhi;++j)
  {
    const T* src_p = src_row0 + j*s_jstep + ilo*s_istep;
    T* dest_p = dest_row0 + j*d_jstep + ilo * d_istep;

    for (int i=ilo;i<=ihi;++i,src_p+=s_istep,dest_p+=d_istep)
      *dest_p=vil_greyscale_erode(src_p,&offset[0],offset.size());
  }
}

#undef VIL_GREYSCALE_ERODE_INSTANTIATE
#define VIL_GREYSCALE_ERODE_INSTANTIATE(T) \
template void vil_greyscale_erode(const vil_image_view< T >& src_image, \
                                  vil_image_view< T >& dest_image, \
                                  const vil_structuring_element& element)

#endif // vil_greyscale_erode_hxx_
