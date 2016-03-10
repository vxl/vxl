#ifndef vil3d_binary_erode_h_
#define vil3d_binary_erode_h_
//:
// \file
// \brief Perform binary erosion on 3D images
// \author Tim Cootes

#include <vil3d/algo/vil3d_structuring_element.h>
#include <vil3d/vil3d_image_view.h>


//: Return false if any im[offset[k]] is zero
inline bool vil3d_binary_erode(const bool* im, const std::ptrdiff_t* offset, unsigned n)
{
  for (unsigned i=0;i<n;++i) if (!im[offset[i]]) return false;
  return true;
}

//: Erodes src_image to produce dest_image (assumed single plane)
// \relatesalso vil3d_image_view
// \relatesalso vil3d_structuring_element
void vil3d_binary_erode(const vil3d_image_view<bool>& src_image,
                       vil3d_image_view<bool>& dest_image,
                       const vil3d_structuring_element& element);

#endif // vil3d_binary_erode_h_
