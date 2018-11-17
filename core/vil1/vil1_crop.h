// This is core/vil1/vil1_crop.h
#ifndef vil1_crop_h_
#define vil1_crop_h_
//:
// \file
// \author awf@robots.ox.ac.uk
// \date 16 Feb 00

#include <vil1/vil1_fwd.h>

//: Crop to a region of SRC.
vil1_image vil1_crop(const vil1_image& SRC, int x0, int y0, int w, int h);

#endif // vil1_crop_h_
