#ifndef vil_abs_shuffle_distance_h_
#define vil_abs_shuffle_distance_h_
//:
// \file
// \brief Compute shuffle distance between two images
// \author Tim Cootes

#include <vil/algo/vil_structuring_element.h>
#include <vil/vil_image_view.h>

//: Return minimum value of |im[offset[k]]-v0| k=0..n-1
template <class T1, class T2>
inline double vil_abs_shuffle_distance(T1 v0, const T2* im,
                                       const vcl_ptrdiff_t* offset, unsigned n)
{
  double min_v = im[offset[0]]<v0?(v0-im[offset[0]]):(im[offset[0]]-v0);
  for (unsigned i=1;i<n;++i)
  {
    T2 v1 = im[offset[i]];
    double abs_diff = (v0<v1?(v1-v0):(v0-v1));
    if (abs_diff<min_v) min_v=abs_diff;
  }
  return min_v;
}

//: Return min difference of pixels under structuring element centred at (i0,j0)
//  I.e., returns minimum of |v0-image(i,j,plane)| over (i,j) in element.
//  Checks boundary overlap.
template <class T1, class T2>
inline double vil_abs_shuffle_distance(T1 v0, const vil_image_view<T2>& image,
                             unsigned plane,
                             const vil_structuring_element& element,
                             int i0, int j0)
{
  double min_v=9e99;
  unsigned n = element.p_i().size();
  for (unsigned int k=0;k<n;++k)
  {
    unsigned int i = i0+element.p_i()[k];
    unsigned int j = j0+element.p_j()[k];
    if (i<image.ni() && j<image.nj())
    {
      T2 v1 = image(i,j,plane);
      double abs_diff = (v0<v1?(v1-v0):(v0-v1));
      if (abs_diff < min_v) { min_v = abs_diff; }
    }
  }
  return min_v;
}

//: Computes shuffle distance between image1 and image2
// For each pixel in image1 it finds the pixel in image2 with
// the closest value in an offset area defined by the element.
// Returns mean over all pixels of this minimum value.
// Images must be of same size.
// If include_borders is false then only include pixels
// for which the structuring element is entirely within the image.
// \relates vil_image_view
// \relates vil_structuring_element
template <class T1, class T2>
double vil_abs_shuffle_distance(const vil_image_view<T1>& image1,
                                const vil_image_view<T2>& image2,
                                const vil_structuring_element& element,
                                bool include_borders=true);

#endif // vil_abs_shuffle_distance_h_
