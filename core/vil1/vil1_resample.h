// This is core/vil1/vil1_resample.h
#ifndef vil1_resample_h_
#define vil1_resample_h_
//:
// \file
// \author fsm
// \brief Returns an image adaptor which will resample the original image.
//
// Note hat resampling will happen only at the point when get_section()
// is called on the returned image, and only on that (sub)section of the image.

#include <vil1/vil1_image.h>

//:
vil1_image vil1_resample(vil1_image const &, int new_width, int new_height);

#endif // vil1_resample_h_
