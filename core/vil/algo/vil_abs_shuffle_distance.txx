#ifndef vil_abs_shuffle_distance_txx_
#define vil_abs_shuffle_distance_txx_
//:
// \file
// \brief Compute shuffle distance between two images
// \author Tim Cootes

#include "vil_abs_shuffle_distance.h"
#include <vcl_cassert.h>

//: Computes shuffle distance between image1 and image2
// For each pixel in image1 it finds the pixel in image2 with
// the closest value in an offset area defined by the element.
// Returns mean over all pixels of this minimum value.
// \relates vil_image_view
// \relates vil_structuring_element
template <class T1, class T2>
double vil_abs_shuffle_distance(const vil_image_view<T1>& image1,
                                const vil_image_view<T2>& image2,
                                const vil_structuring_element& element,
                                bool include_borders)
{
  unsigned ni = image1.ni();
  unsigned nj = image1.nj();
  assert(image1.nplanes()==1);
  assert(image2.nplanes()==1);
  assert(image2.ni()==ni);
  assert(image2.nj()==nj);

  vcl_ptrdiff_t istep1 = image1.istep(),  jstep1 = image1.jstep(),
                istep2 = image2.istep(), jstep2 = image2.jstep();

  const T1* image1_row0 = image1.top_left_ptr();
  const T2* image2_row0 = image2.top_left_ptr();

  vcl_vector<vcl_ptrdiff_t> offset;
  vil_compute_offsets(offset,element,istep2,jstep2);

  // Define box in which all element will be valid
  int ilo = -element.min_i();
  int ihi = ni-1-element.max_i();
  int jlo = -element.min_j();
  int jhi = nj-1-element.max_j();

  double sum=0.0;

  if (include_borders)
  {
    // Deal with left edge
    for (int i=0;i<ilo;++i)
      for (unsigned int j=0;j<nj;++j)
        sum+=vil_abs_shuffle_distance(image1(i,j),image2,0,element,i,j);
    // Deal with right edge
    for (unsigned int i=ihi+1;i<ni;++i)
      for (unsigned int j=0;j<nj;++j)
        sum+=vil_abs_shuffle_distance(image1(i,j),image2,0,element,i,j);

    // Deal with bottom edge
    for (int i=ilo;i<=ihi;++i)
      for (int j=0;j<jlo;++j)
        sum+=vil_abs_shuffle_distance(image1(i,j),image2,0,element,i,j);
    // Deal with top edge
    for (int i=ilo;i<=ihi;++i)
      for (unsigned int j=jhi+1;j<nj;++j)
        sum+=vil_abs_shuffle_distance(image1(i,j),image2,0,element,i,j);
  }

  for (int j=jlo;j<=jhi;++j)
  {
    const T1* p1 = image1_row0 + j*jstep1 + ilo*istep1;
    const T2* p2 = image2_row0 + j*jstep2 + ilo*istep2;

    for (int i=ilo;i<=ihi;++i,p1+=istep1,p2+=istep2)
      sum += vil_abs_shuffle_distance(*p1,p2,&offset[0],offset.size());
  }
  
  int np = ni*nj;
  if (!include_borders) np = (1+ihi-ilo)*(1+jhi-jlo);
  
  return sum/np;
}

#undef VIL_ABS_SHUFFLE_DISTANCE_INSTANTIATE
#define VIL_ABS_SHUFFLE_DISTANCE_INSTANTIATE( T1, T2 ) \
template double vil_abs_shuffle_distance(const vil_image_view< T1 >& image1, \
                                  const vil_image_view< T2 >& image2, \
                                  const vil_structuring_element& element, \
                                  bool include_borders)

#endif // vil_abs_shuffle_distance_txx_
