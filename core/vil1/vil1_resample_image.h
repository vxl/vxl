// This is core/vil1/vil1_resample_image.h
#ifndef vil1_resample_image_h_
#define vil1_resample_image_h_
//:
// \file
// \brief Adaptor which produces an image by resampling
//
// This class is best accessed through the external function vil1_resample().
// \verbatim
//  Modifications
//   29 Aug. 2002 - Peter Vanroose - made sure that src pixel region is not empty
// \endverbatim

class vil1_image;

//:
// Support function for vil1_resample_image_impl.
// -   T: pixel type
// -   A: accumulator type
// This class is best accessed through the external function vil1_resample().
template <class T, class A>
bool vil1_resample_image(vil1_image const &base, unsigned new_width, unsigned new_height,
                         T *buf, A *dummy,
                         int x0, int y0, int w, int h);

#endif // vil1_resample_image_h_
