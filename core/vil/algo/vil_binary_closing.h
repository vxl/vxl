#ifndef vil_binary_closing_h_
#define vil_binary_closing_h_
//:
// \file
// \brief Perform binary closing on images
// \author Tim Cootes

#include <vil/algo/vil_binary_dilate.h>
#include <vil/algo/vil_binary_erode.h>

//: Applies morphological closing operation with given structuring element
//  Applies dilation followed by erosion
// \relatesalso vil_image_view
// \relatesalso vil_structuring_element
inline void vil_binary_closing(const vil_image_view<bool>& src_image,
                               vil_image_view<bool>& dest_image,
                               const vil_structuring_element& element)
{
  vil_image_view<bool> dilated_im;
  vil_binary_dilate(src_image,dilated_im,element);
  vil_binary_erode(dilated_im,dest_image,element);
}

#endif // vil_binary_closing_h_
