// This is vxl/vil/vil_resample.h
#ifndef vil_resample_h_
#define vil_resample_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author fsm
// \brief Returns an image adaptor which will resample the original image.
// Note hat resampling will happen only at the point when get_section()
// is called on the returned image, and only on that (sub)section of the image.

#include <vil/vil_image.h>

//:
vil_image vil_resample(vil_image const &, int new_width, int new_height);

#endif // vil_resample_h_
