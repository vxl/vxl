#ifndef vil3d_abs_shuffle_distance_h_
#define vil3d_abs_shuffle_distance_h_
//:
// \file
// \brief Compute shuffle distance between two 3D images
// \author Vlad Petrovic & Tim Cootes

#include <vil3d/algo/vil3d_structuring_element.h>
#include <vil3d/vil3d_image_view.h>

//: Return minimum value of |im[offset[k]]-v0| k=0..n-1
template <class T1, class T2>
inline double vil3d_abs_shuffle_distance(T1 v0, const T2* im,
                                       const std::ptrdiff_t* offset, unsigned n)
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

//: Return min difference of pixels under structuring element centred at (i0,j0,k0)
//  I.e., returns minimum of |v0-image(i,j,k,plane)| over (i,j,k) in element.
//  Checks boundary overlap.
template <class T1, class T2>
inline double vil3d_abs_shuffle_distance(T1 v0, const vil3d_image_view<T2>& image,
                             unsigned plane,
                             const vil3d_structuring_element& element,
                             int i0, int j0, int k0)
{
  double min_v=9e99;
  unsigned n = element.p_i().size();
  for (unsigned int m=0;m<n;++m)
  {
    unsigned int i = i0+element.p_i()[m];
    unsigned int j = j0+element.p_j()[m];
    unsigned int k = k0+element.p_k()[m];
    if (i<image.ni() && j<image.nj() && k<image.nk())
    {
      T2 v1 = image(i,j,k,plane);
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
// \relatesalso vil_image_view
// \relatesalso vil_structuring_element
template <class T1, class T2>
double vil3d_abs_shuffle_distance(const vil3d_image_view<T1>& image1,
                                const vil3d_image_view<T2>& image2,
                                const vil3d_structuring_element& element,
                                bool include_borders=true);

//: Computes shuffle distance between image1 and image2
// For each pixel in image1 it finds the pixel in image2 with
// the closest value in an offset area defined by the element.
// If include_borders is false then only include pixels
// for which the structuring element is entirely within the image.
// \relatesalso vil_image_view
// \relatesalso vil_structuring_element
template <class T1, class T2>
void vil3d_abs_shuffle_distance(const vil3d_image_view<T1>& image1,
                                const vil3d_image_view<T2>& image2,
                                const vil3d_structuring_element& element,
                                vil3d_image_view<T1>& image3);

#endif // vil3d_abs_shuffle_distance_h_
