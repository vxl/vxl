// This is core/vil1/vil1_image_as.h
#ifndef vil1_image_as_h_
#define vil1_image_as_h_
//:
// \file
// \brief Set of vil1_image_as functions
// \author fsm

#include <vil1/vil1_image.h>

//: \brief Converts to a different pixel type.
//
// There is no scaling performed, so beware of clamping effects. Sample usage:
// \code
//   out = vil1_image_as<int>( in );
// \endcode
// or, on compilers that require the arguments to specify the templates:
// \code
//   out = vil1_image_as( in , (int*)0 );
// \endcode
// Converting a RGB buffer to greyscale will do a RGB->luminance
// conversion. That is, different weights will be applied to each of
// the R, G and B values to obtain the resulting grey value.

template<class T>
vil1_image vil1_image_as(vil1_image const & i, T* dummy /* = 0 */);

//: \brief Converts to a greyscale image with byte (8bit unsigned) pixels.
// \sa vil1_image_as
vil1_image vil1_image_as_byte(vil1_image const & i);

//: \brief Converts to a greyscale image with 16bit unsigned pixels.
// \sa vil1_image_as
vil1_image vil1_image_as_uint16(vil1_image const &image);

//: \brief Converts to a greyscale image with integer (32bit) pixels.
// \sa vil1_image_as
vil1_image vil1_image_as_int(vil1_image const & i);

//: \brief Converts to a grayscale image with float pixels.
// \sa vil1_image_as
vil1_image vil1_image_as_float(vil1_image const & i);

//: \brief Converts to a greyscale image with integer double pixels.
// \sa vil1_image_as
vil1_image vil1_image_as_double(vil1_image const & i);

//: \brief Converts to a rgb image with 8 bits per component.
// \sa vil1_image_as
vil1_image vil1_image_as_rgb_byte(vil1_image const & i);

//: \brief Converts to a rgb image with 16 bits per component.
// \sa vil1_image_as
vil1_image vil1_image_as_rgb_uint16(vil1_image const & i);

//: \brief Converts to a rgb image with float components.
// \sa vil1_image_as
vil1_image vil1_image_as_rgb_float(vil1_image const & i);

#endif // vil1_image_as_h_
