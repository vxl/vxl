// This is contrib/brl/bbas/vidl2/vidl2_pixel_format.cxx
#include "vidl2_pixel_format.h"
//:
// \file
// \author Matt Leotta
//


//: Recursive template metaprogram to generate conditionals
// for checking the traits of each defined pixel type 
template <vidl2_pixel_format pix_type>
struct check_types{
  static inline unsigned bpp(vidl2_pixel_format f)
  {
    if(f == pix_type)
      return vidl2_pixel_traits<pix_type>::bits_per_pixel();
    return check_types<vidl2_pixel_format(pix_type-1)>::bpp(f);
  }

  static inline bool packed(vidl2_pixel_format f)
  {
    if(f == pix_type)
      return vidl2_pixel_traits<pix_type>::packed();
    return check_types<vidl2_pixel_format(pix_type-1)>::packed(f);
  }

  static inline bool planar(vidl2_pixel_format f)
  {
    if(f == pix_type)
      return vidl2_pixel_traits<pix_type>::planar();
    return check_types<vidl2_pixel_format(pix_type-1)>::planar(f);
  }

  static inline vidl2_pixel_format from_string(const vcl_string& s)
  {
    if(s == vidl2_pixel_traits<pix_type>::name())
      return pix_type;
    return check_types<vidl2_pixel_format(pix_type-1)>::from_string(s);
  }

  static inline vcl_string to_string(vidl2_pixel_format f)
  {
    if(f == pix_type)
      return vidl2_pixel_traits<pix_type>::name();
    return check_types<vidl2_pixel_format(pix_type-1)>::to_string(f);
  }
};


//: The base case: unknown pixel type
VCL_DEFINE_SPECIALIZATION
struct check_types<VIDL2_PIXEL_FORMAT_UNKNOWN>{
  static inline unsigned bpp(vidl2_pixel_format f)
  {
    return vidl2_pixel_traits<VIDL2_PIXEL_FORMAT_UNKNOWN>::bits_per_pixel();
  }

  static inline bool packed(vidl2_pixel_format f)
  {
    return vidl2_pixel_traits<VIDL2_PIXEL_FORMAT_UNKNOWN>::packed();
  }

  static inline bool planar(vidl2_pixel_format f)
  {
    return vidl2_pixel_traits<VIDL2_PIXEL_FORMAT_UNKNOWN>::planar();
  }

  static inline vidl2_pixel_format from_string(const vcl_string& s)
  {
    return VIDL2_PIXEL_FORMAT_UNKNOWN;
  }

  static inline vcl_string to_string(vidl2_pixel_format f)
  {
    return vidl2_pixel_traits<VIDL2_PIXEL_FORMAT_UNKNOWN>::name();
  }
};


//=============================================================================


//: Return the effective number of bits per image pixel in pixel format f
unsigned
vidl2_pixel_format_bpp(enum vidl2_pixel_format f)
{
  // Template metaprogramming automatically generates the conditions
  // to check each enum value from VIDL2_PIXEL_FORMAT_UNKNOWN
  // to VIDL2_PIXEL_FORMAT_ENUM_END-1
  return check_types<vidl2_pixel_format(VIDL2_PIXEL_FORMAT_ENUM_END-1)>::bpp(f);
}


//: Return true if pixels in format f are packed into macropixels 
bool
vidl2_pixel_format_packed(vidl2_pixel_format f)
{
  // Template metaprogramming automatically generates the conditions
  // to check each enum value from VIDL2_PIXEL_FORMAT_UNKNOWN
  // to VIDL2_PIXEL_FORMAT_ENUM_END-1
  return check_types<vidl2_pixel_format(VIDL2_PIXEL_FORMAT_ENUM_END-1)>::packed(f);
}


//: Return true if pixel format f has multiple planes
bool
vidl2_pixel_format_planar(vidl2_pixel_format f)
{
  // Template metaprogramming automatically generates the conditions
  // to check each enum value from VIDL2_PIXEL_FORMAT_UNKNOWN
  // to VIDL2_PIXEL_FORMAT_ENUM_END-1
  return check_types<vidl2_pixel_format(VIDL2_PIXEL_FORMAT_ENUM_END-1)>::planar(f);
}


//: Output a pretty string representing the pixel format.
vcl_ostream & operator << (vcl_ostream &os, vidl2_pixel_format f)
{
  // Template metaprogramming automatically generates the conditions
  // to check each enum value from VIDL2_PIXEL_FORMAT_UNKNOWN
  // to VIDL2_PIXEL_FORMAT_ENUM_END-1
  os << check_types<vidl2_pixel_format(VIDL2_PIXEL_FORMAT_ENUM_END-1)>::to_string(f);
  return os;
}


//: Convert a string into a pixel format. 
vcl_string
vidl2_pixel_format_to_string(vidl2_pixel_format f)
{
  // Template metaprogramming automatically generates the conditions
  // to check each enum value from VIDL2_PIXEL_FORMAT_UNKNOWN
  // to VIDL2_PIXEL_FORMAT_ENUM_END-1
  return check_types<vidl2_pixel_format(VIDL2_PIXEL_FORMAT_ENUM_END-1)>::to_string(f);
}


//: Convert a string into a pixel format.
// This uses the same encoding as operator<<.
vidl2_pixel_format vidl2_pixel_format_from_string(const vcl_string& s)
{
  // Template metaprogramming automatically generates the conditions
  // to check each enum value from VIDL2_PIXEL_FORMAT_UNKNOWN
  // to VIDL2_PIXEL_FORMAT_ENUM_END-1
  return check_types<vidl2_pixel_format(VIDL2_PIXEL_FORMAT_ENUM_END-1)>::from_string(s);
}

