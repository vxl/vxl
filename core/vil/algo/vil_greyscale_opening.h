#ifndef vil2_greyscale_opening_h_
#define vil2_greyscale_opening_h_
//:
//  \file
//  \brief Perform greyscale opening on images
//  \author Tim Cootes

#include <vil2/algo/vil2_greyscale_dilate.h>
#include <vil2/algo/vil2_greyscale_erode.h>

//: Applies morphological opening operation with given structuring element
//  Applies erosion followed by dilation
// \relates vil2_image_view
// \relates vil2_structuring_element
template <class T>
inline void vil2_greyscale_opening(const vil2_image_view<T>& src_image,
                                   vil2_image_view<T>& dest_image,
                                   const vil2_structuring_element& element)
{
  vil2_image_view<T> eroded_im;
  vil2_greyscale_erode(src_image,eroded_im,element);
  vil2_greyscale_dilate(eroded_im,dest_image,element);
}

#endif // vil2_greyscale_opening_h_
