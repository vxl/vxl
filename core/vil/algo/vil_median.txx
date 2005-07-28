#ifndef vil_median_txx_
#define vil_median_txx_
//:
// \file
// \brief Perform median filtering on images
// \author Tim Cootes

#include "vil_median.h"
#include <vcl_cassert.h>

//: Computes median value of pixels under structuring element.
// dest_image(i0,j0) is the median value of the pixels under the
// structuring element when it is centred on src_image(i0,j0)
template <class T>
void vil_median(const vil_image_view<T>& src_image,
                vil_image_view<T>& dest_image,
                const vil_structuring_element& element)
{
  assert(src_image.nplanes()==1);
  unsigned ni = src_image.ni();
  unsigned nj = src_image.nj();
  dest_image.set_size(ni,nj,1);

  vcl_ptrdiff_t s_istep = src_image.istep(),  s_jstep = src_image.jstep();
  vcl_ptrdiff_t d_istep = dest_image.istep(), d_jstep = dest_image.jstep();

  const T* src_row0 = src_image.top_left_ptr();
  T* dest_row0 = dest_image.top_left_ptr();

  vcl_vector<vcl_ptrdiff_t> offset;
  vil_compute_offsets(offset,element,s_istep,s_jstep);

  vcl_vector<T> value_wkspce;

  // Define box in which all element will be valid
  int ilo = -element.min_i();
  int ihi = ni-1-element.max_i();
  int jlo = -element.min_j();
  int jhi = nj-1-element.max_j();

  // Deal with left edge
  for (int i=0;i<ilo;++i)
    for (unsigned int j=0;j<nj;++j)
      dest_image(i,j,0)=vil_sorted_value(src_image,0,element,i,j,value_wkspce,0.5);
  // Deal with right edge
  for (unsigned int i=ihi+1;i<ni;++i)
    for (unsigned int j=0;j<nj;++j)
      dest_image(i,j,0)=vil_sorted_value(src_image,0,element,i,j,value_wkspce,0.5);
  // Deal with bottom edge
  for (int i=ilo;i<=ihi;++i)
    for (int j=0;j<jlo;++j)
      dest_image(i,j,0)=vil_sorted_value(src_image,0,element,i,j,value_wkspce,0.5);
  // Deal with top edge
  for (int i=ilo;i<=ihi;++i)
    for (unsigned int j=jhi+1;j<nj;++j)
      dest_image(i,j,0)=vil_sorted_value(src_image,0,element,i,j,value_wkspce,0.5);

  // No bounds checks in the interior, so we must make sure there is enough space in
  // the workspace.
  value_wkspce.resize( offset.size() );
  int rank = int(0.5*(offset.size()-1));
  for (int j=jlo;j<=jhi;++j)
  {
    const T* src_p = src_row0 + j*s_jstep + ilo*s_istep;
    T* dest_p = dest_row0 + j*d_jstep + ilo * d_istep;

    for (int i=ilo;i<=ihi;++i,src_p+=s_istep,dest_p+=d_istep)
      *dest_p=vil_sorted_value(src_p,&offset[0],value_wkspce.begin(),offset.size(),rank);
  }
}

#undef VIL_MEDIAN_INSTANTIATE
#define VIL_MEDIAN_INSTANTIATE(T) \
template void vil_median(const vil_image_view< T >& src_image, \
                         vil_image_view< T >& dest_image, \
                         const vil_structuring_element& element)

#endif // vil_median_txx_
