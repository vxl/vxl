#ifndef vil_binary_erode_h_
#define vil_binary_erode_h_
//:
// \file
// \brief Perform binary erosion on images
// \author Tim Cootes

#include <vil/algo/vil_structuring_element.h>
#include <vil/vil_image_view.h>


//: Return false if any im[offset[k]] is zero
inline bool vil_binary_erode(const bool* im, const vcl_ptrdiff_t* offset, unsigned n)
{
  for (unsigned i=0;i<n;++i) if (!im[offset[i]]) return false;
  return true;
}

//: Return false if any image pixel under element centred at (i0,j0) is zero
//  Checks boundary overlap
inline bool vil_binary_erode(const vil_image_view<bool>& image, unsigned plane,
                             const vil_structuring_element& element, int i0, int j0)
{
  unsigned n = element.p_i().size();
  for (unsigned int k=0;k<n;++k)
  {
    unsigned int i = i0+element.p_i()[k];
    unsigned int j = j0+element.p_j()[k];
    if (i<image.ni() && j<image.nj() && !image(i,j,plane))
      return false;
  }
  return true;
}

//: Erodes src_image to produce dest_image (assumed single plane)
// \relates vil_image_view
// \relates vil_structuring_element
void vil_binary_erode(const vil_image_view<bool>& src_image,
                      vil_image_view<bool>& dest_image,
                      const vil_structuring_element& element);

#endif // vil_binary_erode_h_
