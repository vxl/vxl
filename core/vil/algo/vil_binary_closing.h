#ifndef vil2_binary_closing_h_
#define vil2_binary_closing_h_
//:
//  \file
//  \brief Perform binary closing on images
//  \author Tim Cootes

#include <vil2/algo/vil2_binary_dilate.h>
#include <vil2/algo/vil2_binary_erode.h>

//: Applies morphological closing operation with given structuring element
//  Applies dilation followed by erosion
// \relates vil2_image_view
// \relates vil2_structuring_element
inline void vil2_binary_closing(const vil2_image_view<bool>& src_image,
                                vil2_image_view<bool>& dest_image,
                                const vil2_structuring_element& element)
{
  vil2_image_view<bool> dilated_im;
  vil2_binary_dilate(src_image,dilated_im,element);
  vil2_binary_erode(dilated_im,dest_image,element);
}

#endif // vil2_binary_closing_h_
