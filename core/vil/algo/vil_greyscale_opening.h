#ifndef vil_greyscale_opening_h_
#define vil_greyscale_opening_h_
//:
// \file
// \brief Perform greyscale opening on images
// \author Tim Cootes

#include <vil/algo/vil_greyscale_dilate.h>
#include <vil/algo/vil_greyscale_erode.h>

//: Applies morphological opening operation with given structuring element
//  Applies erosion followed by dilation
// \relatesalso vil_image_view
// \relatesalso vil_structuring_element
template <class T>
inline void vil_greyscale_opening(const vil_image_view<T>& src_image,
                                  vil_image_view<T>& dest_image,
                                  const vil_structuring_element& element)
{
  vil_image_view<T> eroded_im;
  vil_greyscale_erode(src_image,eroded_im,element);
  vil_greyscale_dilate(eroded_im,dest_image,element);
}

#endif // vil_greyscale_opening_h_
