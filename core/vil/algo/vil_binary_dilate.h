#ifndef vil_binary_dilate_h_
#define vil_binary_dilate_h_
//:
// \file
// \brief Perform binary dilation on images
// \author Tim Cootes

#include <vil/algo/vil_structuring_element.h>
#include <vil/vil_image_view.h>
#include <vil/vil_border.h>

//: Return true if any im[offset[k]] is non-zero
inline bool vil_binary_dilate(const bool* im, const std::ptrdiff_t* offset, unsigned n)
{
  for (unsigned i=0;i<n;++i) if (im[offset[i]]) return true;
  return false;
}

//: Return true if any image pixel under element centred at (i0,j0) is non-zero
//  Checks boundary overlap
// \relatesalso vil_structuring_element
template <class imAccessorT>
inline bool vil_binary_dilate(const imAccessorT& image, unsigned plane,
                              const vil_structuring_element& element, int i0, int j0)
{
  unsigned n = element.p_i().size();
  for (unsigned int k=0;k<n;++k)
  {
    unsigned int i = i0+element.p_i()[k];
    unsigned int j = j0+element.p_j()[k];
    if (image(i,j,plane))
      return true;
  }
  return false;
}

//: Dilates src_image to produce dest_image (assumed single plane)
// \relatesalso vil_image_view
// \relatesalso vil_structuring_element
void vil_binary_dilate(const vil_image_view<bool>& src_image,
                       vil_image_view<bool>& dest_image,
                       const vil_structuring_element& element);

//: Dilates src_image to produce dest_image (assumed single plane)
// \relatesalso vil_image_view
// \relatesalso vil_structuring_element
// \relatesalso vil_border
void vil_binary_dilate(const vil_image_view<bool>& src_image,
                       vil_image_view<bool>& dest_image,
                       const vil_structuring_element& element,
                       const vil_border<vil_image_view<bool> >& border);

#endif // vil_binary_dilate_h_
