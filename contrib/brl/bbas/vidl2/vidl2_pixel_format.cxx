// This is brl/bbas/vidl2/vidl2_pixel_format.cxx
#include "vidl2_pixel_format.h"
//:
// \file
// \author Matt Leotta
//
#include <vcl_iostream.h>

#define vidl2_ppi_mac(FMT)\
const vcl_ptrdiff_t vidl2_pixel_pack_of<VIDL2_PIXEL_FORMAT_##FMT>::offset[macro_pix_size][num_channels]

// Define the packing order for each packed vidl2_pixel_format
// The main purpose of this struct is to define a static
// array of pointer offsets to describe the packing.
//
// The array vidl2_pixel_pack_of<format>::offset is a 2D array
// of pointer offsets from the start of the macro pixel.  The
// size of the array is macro-pixel-size by number-of-channels.
// The value offset[i][j] gives the offset to the jth channel
// of the ith pixel in the current macro-pixel.  For example,
// offset[1][0] gives the 'Y' channel (if YUV) or 'R' channel
// (if RGB) of the second pixel in the macro pixel

vidl2_ppi_mac(YUYV_422) = {{0,1,3},{2,1,3}};
vidl2_ppi_mac(UYVY_422) = {{1,0,2},{3,0,2}};
vidl2_ppi_mac(UYVY_411) = {{1,0,3},{2,0,3},{4,0,3},{5,0,3}};

#undef vidl2_ppi_mac


//=============================================================================

//: Recursive template metaprogram to generate conditionals for checking the traits of each defined pixel type
template <vidl2_pixel_format pix_type>
struct check_types
{
  static inline
  void traits(vidl2_pixel_format f, vidl2_pixel_traits& t)
  {
    if (f == pix_type){
      t.name           = vidl2_pixel_traits_of<pix_type>::name();
      t.bits_per_pixel = vidl2_pixel_traits_of<pix_type>::bits_per_pixel;
      t.num_channels   = vidl2_pixel_traits_of<pix_type>::num_channels;
      t.color          = vidl2_pixel_traits_of<pix_type>::color();
      t.arrangement    = vidl2_pixel_traits_of<pix_type>::arrangement();
      t.chroma_shift_x = vidl2_pixel_traits_of<pix_type>::chroma_shift_x;
      t.chroma_shift_y = vidl2_pixel_traits_of<pix_type>::chroma_shift_y;
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
    t.bits_per_pixel = vidl2_pixel_traits_of<VIDL2_PIXEL_FORMAT_UNKNOWN>::bits_per_pixel;
    t.num_channels   = vidl2_pixel_traits_of<VIDL2_PIXEL_FORMAT_UNKNOWN>::num_channels;
    t.color          = vidl2_pixel_traits_of<VIDL2_PIXEL_FORMAT_UNKNOWN>::color();
    t.arrangement    = vidl2_pixel_traits_of<VIDL2_PIXEL_FORMAT_UNKNOWN>::arrangement();
    t.chroma_shift_x = vidl2_pixel_traits_of<VIDL2_PIXEL_FORMAT_UNKNOWN>::chroma_shift_x;
    t.chroma_shift_y = vidl2_pixel_traits_of<VIDL2_PIXEL_FORMAT_UNKNOWN>::chroma_shift_y;
  }

  static inline vidl2_pixel_format from_string(const vcl_string& s)
  {
    return VIDL2_PIXEL_FORMAT_UNKNOWN;
  }

};


//=============================================================================

//: Return the number of channels needed in a color mode
unsigned
    vidl2_pixel_color_num_channels(vidl2_pixel_color c)
{
  switch(c){
    case VIDL2_PIXEL_COLOR_MONO:
      return vidl2_color_traits_of<VIDL2_PIXEL_COLOR_MONO>::num_channels;
    case VIDL2_PIXEL_COLOR_RGB:
      return vidl2_color_traits_of<VIDL2_PIXEL_COLOR_RGB>::num_channels;
    case VIDL2_PIXEL_COLOR_YUV:
      return vidl2_color_traits_of<VIDL2_PIXEL_COLOR_YUV>::num_channels;
    case VIDL2_PIXEL_COLOR_RGBA:
      return vidl2_color_traits_of<VIDL2_PIXEL_COLOR_RGBA>::num_channels;
    default:
      break;
  }
  return vidl2_color_traits_of<VIDL2_PIXEL_COLOR_UNKNOWN>::num_channels;
}


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
