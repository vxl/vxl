// This is core/vil1/vil1_pixel.h
#ifndef vil1_pixel_h_
#define vil1_pixel_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Convenient descriptions of format combinations
// \author awf@robots.ox.ac.uk

class vil1_image;

//: Common combinations of component format, bits, planes and components.
enum vil1_pixel_format_t
{
  VIL1_PIXEL_FORMAT_UNKNOWN,
  VIL1_BYTE,       /*!< 1 x W x H x 1 of UINT x 8*/
  VIL1_RGB_BYTE,   /*!< 1 x W x H x 3 of UINT x 8*/
  VIL1_RGBA_BYTE,  /*!< 1 x W x H x 4 of UINT x 8*/
  VIL1_RGB_UINT16, /*!< 1 x W x H x 3 of UINT x 16*/
  VIL1_RGB_FLOAT,  /*!< 1 x W x H x 3 of IEEE_FLOAT x 32*/
  VIL1_RGB_DOUBLE, /*!< 1 x W x H x 3 of IEEE_DOUBLE x 64*/
  VIL1_UINT16,     /*!< 1 x W x H x 1 of UINT x 16*/
  VIL1_UINT32,     /*!< 1 x W x H x 1 of UINT x 32*/
  VIL1_FLOAT,      /*!< 1 x W x H x 1 of IEEE_FLOAT x 32*/
  VIL1_DOUBLE,     /*!< 1 x W x H x 1 of IEEE_DOUBLE x 64*/
  VIL1_COMPLEX     /*!< 1 x W x H x 1 of COMPLEX x 64*/
};

inline
const char* vil1_print(vil1_pixel_format_t f)
{
  switch (f)
  {
    case VIL1_PIXEL_FORMAT_UNKNOWN: return "VIL1_PIXEL_FORMAT_UNKNOWN";
    case VIL1_BYTE: return "VIL1_BYTE";
    case VIL1_RGB_BYTE: return "VIL1_RGB_BYTE";
    case VIL1_RGBA_BYTE: return "VIL1_RGBA_BYTE";
    case VIL1_RGB_UINT16: return "VIL1_RGB_UINT16";
    case VIL1_RGB_FLOAT: return "VIL1_RGB_FLOAT";
    case VIL1_RGB_DOUBLE: return "VIL1_RGB_DOUBLE";
    case VIL1_UINT16: return "VIL1_UINT16";
    case VIL1_UINT32: return "VIL1_UINT32";
    case VIL1_FLOAT: return "VIL1_FLOAT";
    case VIL1_DOUBLE: return "VIL1_DOUBLE";
    case VIL1_COMPLEX: return "VIL1_COMPLEX";
    default: return "VIL1_PIXEL_FORMAT_INVALID";
  }
}

//: Returns one of the `vil1_pixel_format's.
// A standard RGB RGB RGB image has pixel_type() == VIL1_RGB_BYTE
vil1_pixel_format_t vil1_pixel_format(vil1_image const &);

#endif // vil1_pixel_h_
