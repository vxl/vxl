#ifndef vil2_binary_erode_h_
#define vil2_binary_erode_h_
//:
// \file
// \brief Perform binary erosion on images
// \author Tim Cootes

#include <vil2/algo/vil2_structuring_element.h>
#include <vil2/vil2_image_view.h>


//: Return false if any im[offset[k]] is zero
inline bool vil2_binary_erode(const bool* im, const vcl_ptrdiff_t* offset, unsigned n)
{
  for (unsigned i=0;i<n;++i) if (!im[offset[i]]) return false;
  return true;
}

//: Erodes src_image to produce dest_image (assumed single plane)
// \relates vil2_image_view
// \relates vil2_structuring_element
void vil2_binary_erode(const vil2_image_view<bool>& src_image,
                       vil2_image_view<bool>& dest_image,
                       const vil2_structuring_element& element);

#endif // vil2_binary_erode_h_
