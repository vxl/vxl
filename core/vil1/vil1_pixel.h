#ifndef vil_pixel_h_
#define vil_pixel_h_
#ifdef __GNUC__
#pragma interface
#endif
// This is vxl/vil/vil_pixel.h

//:
// \file
// \brief Convenient descriptions of format combinations
// \author awf@robots.ox.ac.uk


//: Common combinations of component format, bits, planes and components.
enum vil_pixel_format_t {
  VIL_PIXEL_FORMAT_UNKNOWN,
  VIL_BYTE,       /*!< 1 x W x H x 1 of UINT x 8*/
  VIL_RGB_BYTE,   /*!< 1 x W x H x 3 of UINT x 8*/
  VIL_RGBA_BYTE,  /*!< 1 x W x H x 4 of UINT x 8*/
  VIL_RGB_UINT16, /*!< 1 x W x H x 3 of UINT x 16*/
  VIL_RGB_FLOAT,  /*!< 1 x W x H x 3 of IEEE_FLOAT x 32*/
  VIL_RGB_DOUBLE, /*!< 1 x W x H x 3 of IEEE_DOUBLE x 64*/
  VIL_UINT16,     /*!< 1 x W x H x 1 of UINT x 16*/
  VIL_UINT32,     /*!< 1 x W x H x 1 of UINT x 32*/
  VIL_FLOAT,      /*!< 1 x W x H x 1 of IEEE_FLOAT x 32*/
  VIL_DOUBLE,     /*!< 1 x W x H x 1 of IEEE_DOUBLE x 64*/
  VIL_COMPLEX     /*!< 1 x W x H x 1 of COMPLEX x 64*/
};

class vil_image;

//: Returns one of the `vil_pixel_format's.
// A standard RGB RGB RGB image has pixel_type() == VIL_RGB_BYTE
vil_pixel_format_t vil_pixel_format(vil_image const &);

#endif // vil_pixel_h_
