#ifndef vil3d_binary_dilate_h_
#define vil3d_binary_dilate_h_
//:
//  \file
//  \brief Perform binary dilation on 3D images
//  \author Tim Cootes

#include <vil3d/algo/vil3d_structuring_element.h>
#include <vil3d/vil3d_image_view.h>


//: Return true if any im[offset[k]] is non-zero
inline bool vil3d_binary_dilate(const bool* im, const std::ptrdiff_t* offset, unsigned n)
{
  for (unsigned i=0;i<n;++i) if (im[offset[i]]) return true;
  return false;
}

//: Return true if any image pixel under element centred at (i0,j0,k0) is non-zero
//  Checks boundary overlap
inline bool vil3d_binary_dilate(const vil3d_image_view<bool>& image, unsigned plane,
                                const vil3d_structuring_element& element,
                                int i0, int j0, int k0)
{
  unsigned n = element.p_i().size();
  for (unsigned int a=0;a<n;++a)
  {
    // Note that -ives become huge positives and are thus ignored by the if
    unsigned int i = i0+element.p_i()[a];
    unsigned int j = j0+element.p_j()[a];
    unsigned int k = k0+element.p_k()[a];
    if (i<image.ni() && j<image.nj() && k<image.nk() && image(i,j,k,plane))
      return true;
  }
  return false;
}

//: Dilates src_image to produce dest_image (assumed single plane)
// \relatesalso vil3d_image_view
// \relatesalso vil3d_structuring_element
void vil3d_binary_dilate(const vil3d_image_view<bool>& src_image,
                        vil3d_image_view<bool>& dest_image,
                        const vil3d_structuring_element& element);


#endif // vil3d_binary_dilate_h_
