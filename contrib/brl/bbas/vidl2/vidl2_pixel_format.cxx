// This is brl/bbas/vidl2/vidl2_pixel_format.cxx
#include "vidl2_pixel_format.h"
//:
// \file
// \author Matt Leotta
//
#include <vcl_iostream.h>

//: Recursive template metaprogram to generate conditionals for checking the traits of each defined pixel type
template <vidl2_pixel_format pix_type>
struct check_types
{
  static inline
  void traits(vidl2_pixel_format f, vidl2_pixel_traits& t)
  {
    if (f == pix_type){
      t.name           = vidl2_pixel_traits_of<pix_type>::name();
      t.bits_per_pixel = vidl2_pixel_traits_of<pix_type>::bits_per_pixel();
      t.num_channels   = vidl2_pixel_traits_of<pix_type>::num_channels();
      t.packed         = vidl2_pixel_traits_of<pix_type>::packed();
      t.planar         = vidl2_pixel_traits_of<pix_type>::planar();
    }
    else
      check_types<vidl2_pixel_format(pix_type-1)>::traits(f,t);
  }

  static inline vidl2_pixel_format from_string(const vcl_string& s)
  {
    if (s == vidl2_pixel_traits_of<pix_type>::name())
      return pix_type;
    return check_types<vidl2_pixel_format(pix_type-1)>::from_string(s);
  }

};


//: The base case: unknown pixel type
VCL_DEFINE_SPECIALIZATION
struct check_types<VIDL2_PIXEL_FORMAT_UNKNOWN>
{
  static inline
  void traits(vidl2_pixel_format f, vidl2_pixel_traits& t)
  {
    t.name           = vidl2_pixel_traits_of<VIDL2_PIXEL_FORMAT_UNKNOWN>::name();
    t.bits_per_pixel = vidl2_pixel_traits_of<VIDL2_PIXEL_FORMAT_UNKNOWN>::bits_per_pixel();
    t.num_channels   = vidl2_pixel_traits_of<VIDL2_PIXEL_FORMAT_UNKNOWN>::num_channels();;
    t.packed         = vidl2_pixel_traits_of<VIDL2_PIXEL_FORMAT_UNKNOWN>::packed();
    t.planar         = vidl2_pixel_traits_of<VIDL2_PIXEL_FORMAT_UNKNOWN>::planar();;
  }

  static inline vidl2_pixel_format from_string(const vcl_string& s)
  {
    return VIDL2_PIXEL_FORMAT_UNKNOWN;
  }

};


//=============================================================================


//: Return the set of traits for pixel format f
vidl2_pixel_traits
vidl2_pixel_format_traits(vidl2_pixel_format f)
{
  // Template metaprogramming automatically generates the conditions
  // to check each enum value from VIDL2_PIXEL_FORMAT_UNKNOWN
  // to VIDL2_PIXEL_FORMAT_ENUM_END-1
  vidl2_pixel_traits t;
  check_types<vidl2_pixel_format(VIDL2_PIXEL_FORMAT_ENUM_END-1)>::traits(f,t);
  return t;
}


//: Output a pretty string representing the pixel format.
vcl_ostream & operator << (vcl_ostream &os, vidl2_pixel_format f)
{
  // Template metaprogramming automatically generates the conditions
  // to check each enum value from VIDL2_PIXEL_FORMAT_UNKNOWN
  // to VIDL2_PIXEL_FORMAT_ENUM_END-1
  os << vidl2_pixel_format_traits(f).name;
  return os;
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


//: Compute the size (in bytes) of a \a ni x \a nj image buffer of pixel format \a f
unsigned
vidl2_pixel_format_buffer_size(unsigned ni, unsigned nj, vidl2_pixel_format f)
{
  // FIXME This may be incorrect for some formats that require blocks
  // of macro pixels when the size of the image is not evenly divisible
  // by the size of the block.
  unsigned long bits = ((unsigned long)ni)*nj*vidl2_pixel_format_bpp(f);
  return (bits + 7)/8;
}
