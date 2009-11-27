// This is core/vil/vil_rotate.h
#ifndef vil_rotate_h_
#define vil_rotate_h_
//:
// \file
// \brief rotate an image, using the resampling functions
// \author dac
//
// A front end to the resampling functions that allows
// an image to be rotated by any angle theta

#include <vil/vil_image_view.h>

//: Rotate image by angle theta
//  \relatesalso vil_image_view
template <class sType, class dType>
void vil_rotate_image(const vil_image_view<sType>& src_image,
                      vil_image_view<dType>& dest_image,
                      double theta_deg);

#endif // vil_rotate_h_
