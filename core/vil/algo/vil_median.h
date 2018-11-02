#ifndef vil_median_h_
#define vil_median_h_
//:
// \file
// \brief Perform median filtering on images
// \author Tim Cootes

#include <algorithm>
#include <vil/algo/vil_structuring_element.h>
#include <vil/vil_image_view.h>
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Return r-th sorted value of im[offset[k]]
//  Values im[offset[k]] placed into values[k] then sorted. \a values
//  should be a random access iterator into a container of T such
//  that the range [values,values+n) is valid.
template <class T, class Iter>
inline T vil_sorted_value(const T* im, const std::ptrdiff_t* offset, Iter values,
                          unsigned n, unsigned r)
{
  Iter v = values;
  for (unsigned i=0;i<n;++i,++v) *v=im[offset[i]];
  std::nth_element(values, values+r, values+n);
  return values[r];
}

//: Return (n*r)-th sorted value of pixels under element centred at (i0,j0)
// \param r in [0,1].
// \param values used to store values sampled from image before sorting
// Checks boundary overlap
// \relatesalso vil_image_view
// \relatesalso vil_structuring_element
template <class T>
inline T vil_sorted_value(const vil_image_view<T>& image, unsigned plane,
                          const vil_structuring_element& element, int i0, int j0,
                          std::vector<T>& values, double r)
{
  values.clear();
  std::size_t n = element.p_i().size();
  for (std::size_t k=0;k<n;++k)
  {
    unsigned int i = i0+element.p_i()[k];
    unsigned int j = j0+element.p_j()[k];
    if (i<image.ni() && j<image.nj())
      values.push_back(image(i,j,plane));
  }
  std::nth_element(values.begin(),values.begin()+std::size_t(r*(values.size()-1)),
    values.end());
  return values[std::size_t(r*(values.size()-1))];
}

//: Computes median value of pixels under structuring element.
// dest_image(i0,j0) is the median value of the pixels under the
// structuring element when it is centred on src_image(i0,j0)
// \relatesalso vil_image_view
// \relatesalso vil_structuring_element
template <class T>
void vil_median(const vil_image_view<T>& src_image,
                vil_image_view<T>& dest_image,
                const vil_structuring_element& element);

#endif // vil_median_h_
