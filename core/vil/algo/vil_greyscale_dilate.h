#ifndef vil_greyscale_dilate_h_
#define vil_greyscale_dilate_h_
//:
// \file
// \brief Perform greyscale dilation on images
// \author Tim Cootes

#include <vil/algo/vil_structuring_element.h>
#include <vil/vil_image_view.h>

//: Return maximum value of im[offset[k]]
template <class T>
inline T vil_greyscale_dilate(const T* im, const vcl_ptrdiff_t* offset, unsigned n)
{
  T max_v = im[offset[0]];
  for (unsigned i=1;i<n;++i)
    if (im[offset[i]]>max_v) max_v=im[offset[i]];
  return max_v;
}

//: Return max of pixels under structuring element centred at (i0,j0)
//  Checks boundary overlap.  Returns 0 if structuring element is empty.
template <class T>
inline T vil_greyscale_dilate(const vil_image_view<T>& image, unsigned plane,
                              const vil_structuring_element& element,
                              int i0, int j0)
{
  T max_v = T();
  bool first=true;
  unsigned n = element.p_i().size();
  for (unsigned int k=0;k<n;++k)
  {
    unsigned int i = i0+element.p_i()[k];
    unsigned int j = j0+element.p_j()[k];
    if (i<image.ni() && j<image.nj())
    {
      if  (first || image(i,j,plane) > max_v) {
        max_v=image(i,j,plane); first=false; }
    }
  }
  return max_v;
}

//: Dilates src_image to produce dest_image (assumed single plane).
// dest_image(i0,j0) is the maximum value of the pixels under the
// structuring element when it is centred on src_image(i0,j0)
// \relates vil_image_view
// \relates vil_structuring_element
template <class T>
void vil_greyscale_dilate(const vil_image_view<T>& src_image,
                          vil_image_view<T>& dest_image,
                          const vil_structuring_element& element);

#endif // vil_greyscale_dilate_h_
