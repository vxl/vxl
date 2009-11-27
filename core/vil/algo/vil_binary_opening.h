#ifndef vil_binary_opening_h_
#define vil_binary_opening_h_
//:
// \file
// \brief Perform binary opening on images
// \author Tim Cootes

#include <vil/algo/vil_binary_dilate.h>
#include <vil/algo/vil_binary_erode.h>

//: Applies morphological opening operation with given structuring element
//  Applies erosion followed by dilation
// \relatesalso vil_image_view
// \relatesalso vil_structuring_element
inline void vil_binary_opening(const vil_image_view<bool>& src_image,
                               vil_image_view<bool>& dest_image,
                               const vil_structuring_element& element)
{
  vil_image_view<bool> eroded_im;
  vil_binary_erode(src_image,eroded_im,element);
  vil_binary_dilate(eroded_im,dest_image,element);
}

#endif // vil_binary_opening_h_
