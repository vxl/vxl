#ifndef vil2_binary_opening_h_
#define vil2_binary_opening_h_
//: \file
//  \brief Perform binary opening on images
//  \author Tim Cootes

#include <vil2/algo/vil2_binary_dilate.h>
#include <vil2/algo/vil2_binary_erode.h>

//: Applies morphological opening operation with given structuring element
//  Applies erosion followed by dilation
// \relates vil2_image_view
// \relates vil2_structuring_element
inline void vil2_binary_opening(const vil2_image_view<bool>& src_image,
                        vil2_image_view<bool>& dest_image,
                        const vil2_structuring_element& element)
{
  vil2_image_view<bool> eroded_im;
	vil2_binary_erode(src_image,eroded_im,element);
	vil2_binary_dilate(eroded_im,dest_image,element);
}

#endif
