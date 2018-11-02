// This is core/vidl/vidl_pixel_format.cxx
#include <iostream>
#include "vidl_pixel_format.h"
//:
// \file
// \author Matt Leotta
//
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#define vidl_ppi_mac(FMT)\
const std::ptrdiff_t vidl_pixel_pack_of<VIDL_PIXEL_FORMAT_##FMT>::offset[macro_pix_size][num_channels]

// Define the packing order for each packed vidl_pixel_format
// The main purpose of this struct is to define a static
// array of pointer offsets to describe the packing.
//
// The array vidl_pixel_pack_of<format>::offset is a 2D array
// of pointer offsets from the start of the macro pixel.  The
// size of the array is macro-pixel-size by number-of-channels.
// The value offset[i][j] gives the offset to the jth channel
// of the ith pixel in the current macro-pixel.  For example,
// offset[1][0] gives the 'Y' channel (if YUV) or 'R' channel
// (if RGB) of the second pixel in the macro pixel

vidl_ppi_mac(YUYV_422) = {{0,1,3},{2,1,3}};
vidl_ppi_mac(UYVY_422) = {{1,0,2},{3,0,2}};
vidl_ppi_mac(UYVY_411) = {{1,0,3},{2,0,3},{4,0,3},{5,0,3}};

#undef vidl_ppi_mac


//=============================================================================

//: Recursive template metaprogram to generate conditionals for checking the traits of each defined pixel type
template <vidl_pixel_format pix_type>
struct check_types
{
  static inline
  void traits(vidl_pixel_format f, vidl_pixel_traits& t)
  {
    if (f == pix_type){
      t.name           = vidl_pixel_traits_of<pix_type>::name();
      t.type           = &typeid(typename vidl_pixel_traits_of<pix_type>::type);
      t.bits_per_pixel = vidl_pixel_traits_of<pix_type>::bits_per_pixel;
      t.num_channels   = vidl_pixel_traits_of<pix_type>::num_channels;
      t.color          = vidl_pixel_traits_of<pix_type>::color();
      t.arrangement    = vidl_pixel_traits_of<pix_type>::arrangement();
      t.chroma_shift_x = vidl_pixel_traits_of<pix_type>::chroma_shift_x;
      t.chroma_shift_y = vidl_pixel_traits_of<pix_type>::chroma_shift_y;
    }
    else
      check_types<vidl_pixel_format(pix_type-1)>::traits(f,t);
  }

  static inline vidl_pixel_format from_string(const std::string& s)
  {
    if (s == vidl_pixel_traits_of<pix_type>::name())
      return pix_type;
    return check_types<vidl_pixel_format(pix_type-1)>::from_string(s);
  }
};


//: The base case: unknown pixel type
template <>
struct check_types<VIDL_PIXEL_FORMAT_UNKNOWN>
{
  static inline
  void traits(vidl_pixel_format /*f*/, vidl_pixel_traits& t)
  {
    t.name           = vidl_pixel_traits_of<VIDL_PIXEL_FORMAT_UNKNOWN>::name();
    t.type           = &typeid(vidl_pixel_traits_of<VIDL_PIXEL_FORMAT_UNKNOWN>::type);
    t.bits_per_pixel = vidl_pixel_traits_of<VIDL_PIXEL_FORMAT_UNKNOWN>::bits_per_pixel;
    t.num_channels   = vidl_pixel_traits_of<VIDL_PIXEL_FORMAT_UNKNOWN>::num_channels;
    t.color          = vidl_pixel_traits_of<VIDL_PIXEL_FORMAT_UNKNOWN>::color();
    t.arrangement    = vidl_pixel_traits_of<VIDL_PIXEL_FORMAT_UNKNOWN>::arrangement();
    t.chroma_shift_x = vidl_pixel_traits_of<VIDL_PIXEL_FORMAT_UNKNOWN>::chroma_shift_x;
    t.chroma_shift_y = vidl_pixel_traits_of<VIDL_PIXEL_FORMAT_UNKNOWN>::chroma_shift_y;
  }

  static inline vidl_pixel_format from_string(const std::string& /*s*/)
  {
    return VIDL_PIXEL_FORMAT_UNKNOWN;
  }
};


//=============================================================================

//: Return the number of channels needed in a color mode
unsigned
vidl_pixel_color_num_channels(vidl_pixel_color c)
{
  switch (c) {
    case VIDL_PIXEL_COLOR_MONO:
      return vidl_color_traits_of<VIDL_PIXEL_COLOR_MONO>::num_channels;
    case VIDL_PIXEL_COLOR_RGB:
      return vidl_color_traits_of<VIDL_PIXEL_COLOR_RGB>::num_channels;
    case VIDL_PIXEL_COLOR_YUV:
      return vidl_color_traits_of<VIDL_PIXEL_COLOR_YUV>::num_channels;
    case VIDL_PIXEL_COLOR_RGBA:
      return vidl_color_traits_of<VIDL_PIXEL_COLOR_RGBA>::num_channels;
    default:
      break;
  }
  return vidl_color_traits_of<VIDL_PIXEL_COLOR_UNKNOWN>::num_channels;
}


//: Return the set of traits for pixel format f
vidl_pixel_traits
vidl_pixel_format_traits(vidl_pixel_format f)
{
  // Template metaprogramming automatically generates the conditions
  // to check each enum value from VIDL_PIXEL_FORMAT_UNKNOWN
  // to VIDL_PIXEL_FORMAT_ENUM_END-1
  vidl_pixel_traits t;
  check_types<vidl_pixel_format(VIDL_PIXEL_FORMAT_ENUM_END-1)>::traits(f,t);
  return t;
}


//: Output a pretty string representing the pixel format.
std::ostream & operator << (std::ostream &os, vidl_pixel_format f)
{
  // Template metaprogramming automatically generates the conditions
  // to check each enum value from VIDL_PIXEL_FORMAT_UNKNOWN
  // to VIDL_PIXEL_FORMAT_ENUM_END-1
  os << vidl_pixel_format_traits(f).name;
  return os;
}


//: Convert a string into a pixel format.
// This uses the same encoding as operator<<.
vidl_pixel_format vidl_pixel_format_from_string(const std::string& s)
{
  // Template metaprogramming automatically generates the conditions
  // to check each enum value from VIDL_PIXEL_FORMAT_UNKNOWN
  // to VIDL_PIXEL_FORMAT_ENUM_END-1
  return check_types<vidl_pixel_format(VIDL_PIXEL_FORMAT_ENUM_END-1)>::from_string(s);
}


//: Compute the size (in bytes) of a \a ni x \a nj image buffer of pixel format \a f
unsigned
vidl_pixel_format_buffer_size(unsigned ni, unsigned nj, vidl_pixel_format f)
{
  // FIXME This may be incorrect for some formats that require blocks
  // of macro pixels when the size of the image is not evenly divisible
  // by the size of the block.
  unsigned long bits = ((unsigned long)ni)*nj*vidl_pixel_format_bpp(f);
  return (bits + 7)/8;
}
