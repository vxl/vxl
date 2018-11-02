#ifndef vil3d_abs_shuffle_distance_hxx_
#define vil3d_abs_shuffle_distance_hxx_
//:
// \file
// \brief Compute shuffle distance between two images
// \author Vlad Petrovic and Tim Cootes

#include "vil3d_abs_shuffle_distance.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Computes shuffle distance between image1 and image2
// For each pixel in image1 it finds the pixel in image2 with
// the closest value in an offset area defined by the element.
// Returns mean over all pixels of this minimum value.
// \relatesalso vil3d_image_view
// \relatesalso vil3d_structuring_element
template <class T1, class T2>
double vil3d_abs_shuffle_distance(const vil3d_image_view<T1>& image1,
                                  const vil3d_image_view<T2>& image2,
                                  const vil3d_structuring_element& element,
                                  bool include_borders)
{
  // Get image dimensions
  unsigned ni = image1.ni();
  unsigned nj = image1.nj();
  unsigned nk = image1.nk();
  // Assert images are single plain and same size
  assert(image1.nplanes()==1);
  assert(image2.nplanes()==1);
  assert(image2.ni()==ni);
  assert(image2.nj()==nj);
  assert(image2.nk()==nk);

  std::ptrdiff_t istep1 = image1.istep(), jstep1 = image1.jstep(), kstep1 = image1.kstep(),
                istep2 = image2.istep(), jstep2 = image2.jstep(), kstep2 = image2.kstep();

  std::vector<std::ptrdiff_t> offset;
  vil3d_compute_offsets(offset, element, istep1, jstep1, kstep1);

  // Define box in which all element will be valid
  int ilo = -element.min_i();
  int ihi = ni-1-element.max_i();
  int jlo = -element.min_j();
  int jhi = nj-1-element.max_j();
  int klo = -element.min_k();
  int khi = nk-1-element.max_k();

  double sum=0.0;

  if (include_borders)
  {
    // Deal with left edge
    for (unsigned int i=0; int(i)<ilo; ++i)
      for (unsigned int j=0; j<nj; ++j)
        for (unsigned int k=0; k<nk; ++k)
          sum+=vil3d_abs_shuffle_distance(image1(i,j,k),image2,0,element,i,j,k);
    // Deal with right edge
    for (unsigned int i=ihi+1; i<ni; ++i)
      for (unsigned int j=0;j<nj;++j)
        for (unsigned int k=0; k<nk; ++k)
          sum+=vil3d_abs_shuffle_distance(image1(i,j,k),image2,0,element,i,j,k);

    // Deal with bottom edge
    for (unsigned int i=ilo; int(i)<=ihi; ++i)
      for (unsigned int j=0; int(j)<jlo; ++j)
        for (unsigned int k=klo; int(k)<khi; ++k)
          sum+=vil3d_abs_shuffle_distance(image1(i,j,k),image2,0,element,i,j,k);
    // Deal with top edge
    for (unsigned int i=ilo; int(i)<=ihi; ++i)
      for (unsigned int j=jhi+1; j<nj; ++j)
        for (unsigned int k=klo; int(k)<khi; ++k)
          sum+=vil3d_abs_shuffle_distance(image1(i,j,k),image2,0,element,i,j,k);

    // Deal with front edge
    for (unsigned int i=ilo; int(i)<=ihi; ++i)
      for (unsigned int j=jlo; int(j)<jhi; ++j)
        for (unsigned int k=0; int(k)<klo; ++k)
          sum+=vil3d_abs_shuffle_distance(image1(i,j,k),image2,0,element,i,j,k);
    // Deal with back edge
    for (unsigned int i=ilo; int(i)<=ihi; ++i)
      for (unsigned int j=jlo; int(j)<jhi; ++j)
        for (unsigned int k=khi+1; k<nk; ++k)
          sum+=vil3d_abs_shuffle_distance(image1(i,j,k),image2,0,element,i,j,k);
  }

  const T1* image1_start = image1.origin_ptr();
  const T2* image2_start = image2.origin_ptr();

  for (unsigned int k=klo; int(k)<=khi; ++k)
    for (unsigned int j=jlo; int(j)<=jhi; ++j)
    {
      const T1* p1 = image1_start + k*kstep1 + j*jstep1 + ilo*istep1;
      const T2* p2 = image2_start + k*kstep2 + j*jstep2 + ilo*istep2;

      for (int i=ilo; i<=ihi; ++i,p1+=istep1,p2+=istep2)
        sum += vil3d_abs_shuffle_distance(*p1,p2,&offset[0],offset.size());
    }

  // Work out the number of evaluated pixels
  int np = ni*nj*nk;
  if (!include_borders) np = (1+ihi-ilo)*(1+jhi-jlo)*(1+khi-klo);

  return sum/np;
}


//: Computes shuffle distance between image1 and image2
// For each pixel in image1 it finds the pixel in image2 with
// the closest value in an offset area defined by the element.
// \relatesalso vil3d_image_view
// \relatesalso vil3d_structuring_element
template <class T1, class T2>
void vil3d_abs_shuffle_distance(const vil3d_image_view<T1>& image1,
                                const vil3d_image_view<T2>& image2,
                                const vil3d_structuring_element& element,
                                vil3d_image_view<T1>& image3)
{
  std::cout << "image1: " << image1 << std::endl
           << "image2: " << image2 << std::endl;

  // Get image dimensions
  unsigned ni = image1.ni();
  unsigned nj = image1.nj();
  unsigned nk = image1.nk();
  // Assert images are single plain and same size
  assert(image1.nplanes()==1);
  assert(image2.nplanes()==1);
  assert(image2.ni()==ni);
  assert(image2.nj()==nj);
  assert(image2.nk()==nk);

  image3.set_size( ni, nj, nk );

  std::ptrdiff_t istep1 = image1.istep(), jstep1 = image1.jstep(), kstep1 = image1.kstep(),
                istep2 = image2.istep(), jstep2 = image2.jstep(), kstep2 = image2.kstep(),
                istep3 = image3.istep(), jstep3 = image3.jstep(), kstep3 = image3.kstep();

  std::vector<std::ptrdiff_t> offset;
  vil3d_compute_offsets(offset, element, istep1, jstep1, kstep1);

  // Define box in which all element will be valid
  int ilo = -element.min_i();
  int ihi = ni-1-element.max_i();
  int jlo = -element.min_j();
  int jhi = nj-1-element.max_j();
  int klo = -element.min_k();
  int khi = nk-1-element.max_k();

  // if (include_borders)
  {
    // Deal with left edge
    for (unsigned int i=0; int(i)<ilo; ++i)
      for (unsigned int j=0; j<nj; ++j)
        for (unsigned int k=0; k<nk; ++k)
          image3(i,j,k)=(T1)vil3d_abs_shuffle_distance(image1(i,j,k),image2,0,element,i,j,k);

    // Deal with right edge
    for (unsigned int i=ihi+1; i<ni; ++i)
      for (unsigned int j=0;j<nj;++j)
        for (unsigned int k=0; k<nk; ++k)
          image3(i,j,k)=(T1)vil3d_abs_shuffle_distance(image1(i,j,k),image2,0,element,i,j,k);

    // Deal with bottom edge
    for (unsigned int i=ilo; int(i)<=ihi; ++i)
      for (unsigned int j=0; int(j)<jlo; ++j)
        for (unsigned int k=0; k<nk; ++k)
          image3(i,j,k)=(T1)vil3d_abs_shuffle_distance(image1(i,j,k),image2,0,element,i,j,k);

    // Deal with top edge
    for (unsigned int i=ilo; int(i)<=ihi; ++i)
      for (unsigned int j=jhi+1; j<nj; ++j)
        for (unsigned int k=0; k<nk; ++k)
          image3(i,j,k)=(T1)vil3d_abs_shuffle_distance(image1(i,j,k),image2,0,element,i,j,k);

    // Deal with front edge
    for (unsigned int i=ilo; int(i)<=ihi; ++i)
      for (unsigned int j=jlo; int(j)<=jhi; ++j)
        for (unsigned int k=0; int(k)<klo; ++k)
          image3(i,j,k)=(T1)vil3d_abs_shuffle_distance(image1(i,j,k),image2,0,element,i,j,k);

    // Deal with back edge
    for (unsigned int i=ilo; int(i)<=ihi; ++i)
      for (unsigned int j=jlo; int(j)<=jhi; ++j)
        for (unsigned int k=khi+1; k<nk; ++k)
          image3(i,j,k)=(T1)vil3d_abs_shuffle_distance(image1(i,j,k),image2,0,element,i,j,k);
  }

  const T1* image1_start = image1.origin_ptr();
  const T2* image2_start = image2.origin_ptr();
  T1* image3_start = image3.origin_ptr();

  for (unsigned int k=klo; int(k)<=khi; ++k)
    for (unsigned int j=jlo; int(j)<=jhi; ++j)
    {
      const T1* p1 = image1_start + k*kstep1 + j*jstep1 + ilo*istep1;
      const T2* p2 = image2_start + k*kstep2 + j*jstep2 + ilo*istep2;
      T1* p3 = image3_start + k*kstep3 + j*jstep3 + ilo*istep3;

      for (int i=ilo; i<=ihi; ++i,p1+=istep1,p2+=istep2,p3+=istep3)
        *p3 = static_cast<T1>(vil3d_abs_shuffle_distance(*p1,p2,&offset[0],offset.size()));
    }
}


#undef VIL3D_ABS_SHUFFLE_DISTANCE_INSTANTIATE
#define VIL3D_ABS_SHUFFLE_DISTANCE_INSTANTIATE( T1, T2 ) \
template double vil3d_abs_shuffle_distance(const vil3d_image_view< T1 >& image1, \
                                           const vil3d_image_view< T2 >& image2, \
                                           const vil3d_structuring_element& element, \
                                           bool include_borders)

#undef VIL3D_ABS_SHUFFLE_DISTANCE_INSTANTIATE2
#define VIL3D_ABS_SHUFFLE_DISTANCE_INSTANTIATE2( T1, T2 ) \
template void vil3d_abs_shuffle_distance(const vil3d_image_view< T1 >& image1, \
                                         const vil3d_image_view< T2 >& image2, \
                                         const vil3d_structuring_element& element, \
                                         vil3d_image_view< T1 >& image3)

#endif // vil3d_abs_shuffle_distance_hxx_
