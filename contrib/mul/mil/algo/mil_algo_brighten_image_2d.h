#ifndef mil_algo_brighten_image_2d_h_
#define mil_algo_brighten_image_2d_h_
//:
//  \file
//  \brief Functions to change the brightness and contrast of images.
//  \author Ian Scott

#include <mil/mil_image_2d_of.h>

//: Creates a brighter or darker version of an input image.
// dest.pixel = src.pixel * scale
template <class OUTPIX, class INPIX, class SCALE>
void mil_algo_brighten_image_2d(mil_image_2d_of<OUTPIX>& dest,
                                const mil_image_2d_of<INPIX>& src,
                                SCALE scale);

//: Creates a brighter or darker version of an input image.
// dest.pixel = src.pixel * scale + offset
template <class OUTPIX, class INPIX, class SCALE>
void mil_algo_brighten_image_2d(mil_image_2d_of<OUTPIX>& dest,
                                const mil_image_2d_of<INPIX>& src,
                                SCALE scale, SCALE offset);

//: Brighten an image.
// image.pixel = image.pixel * scale
template <class PIX, class SCALE>
void mil_algo_brighten_image_2d(mil_image_2d_of<PIX>& image,
                                SCALE scale);

//: Brighten an image.
// image.pixel = image.pixel * scale + offset
template <class PIX, class SCALE>
void mil_algo_brighten_image_2d(mil_image_2d_of<PIX>& dest,
                                SCALE scale, SCALE offset);

#endif // mil_algo_brighten_image_2d_h_
