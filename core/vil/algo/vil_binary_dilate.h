#ifndef vil2_binary_dilate_h_
#define vil2_binary_dilate_h_
//: \file
//  \brief Perform binary dilation on images
//  \author Tim Cootes

#include <vil2/algo/vil2_structuring_element.h>
#include <vil2/vil2_image_view.h>


//: Return true if any im[offset[k]] is non-zero
inline bool vil2_binary_dilate(const vxl_byte* im, const int* offset, unsigned n)
{
  for (unsigned i=0;i<n;++i) if (im[offset[i]]>0) return true;
  return false;
}

//: Return true if any image pixel under element centred at (i0,j0) is non-zero
//  Checks boundary overlap
inline bool vil2_binary_dilate(const vil2_image_view<vxl_byte>& image, unsigned plane,
                        const vil2_structuring_element& element, int i0, int j0)
{
  unsigned n = element.p_i().size();
  for (int k=0;k<n;++k)
  {
    int i = i0+element.p_i()[k];
    int j = j0+element.p_j()[k];
	if (i>=0 && i<image.ni() && j>=0 && j<image.nj() && image(i,j,plane)>0) return true;
  }
  return false;
}

//: Dilates src_image to produce dest_image (assumed single plane)
void vil2_binary_dilate(const vil2_image_view<vxl_byte>& src_image,
                        vil2_image_view<vxl_byte>& dest_image,
                        const vil2_structuring_element& element);


#endif
