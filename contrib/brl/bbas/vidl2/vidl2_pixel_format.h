// This is brl/bbas/vidl2/vidl2_pixel_format.h
#ifndef vidl2_pixel_format_h_
#define vidl2_pixel_format_h_
//:
// \file
// \brief Supported pixel formats for video frames
//
// \author Matt Leotta
// \date  13 Jan 2006
//
// This file defines the set of known video frame pixel formats.
// The enum vidl2_pixel_format enumerates the types while the
// template specializations of vidl2_pixel_traits define their
// basic traits.  These specializations are defined using the
// macro vidl2_pt_mac.  The pixel traits are:
//  - <b>name</b> a string representation for the format
//  - <b>bits per pixel</b> the number of bits used to represent a pixel
//  - <b>number of channels</b> the number of color channels encoded
//  - <b>packed</b> flag indicating the data is packed into macro pixels
//  - <b>planar</b> flag indicating the data for each pixel is spread over
//                    multiple planes
//
// vidl2_pixel_format differs from vil_pixel_format in that the
// vidl2 formats are representations typically used by video
// hardware and in video codecs to encode a frame.  The vil pixel
// formats are more useful for image processing and are related
// to the C++ data types that may be arranged in a regular array
// to make an image.  A vidl2_pixel_format may have components
// in multiple planes of different sizes or may have data from
// multiple image pixels encoded as a single macro pixel.
//
// This file also contains several functions to check the traits
// of a pixel format at runtime.  These functions use template
// metaprogramming to generate conditionals that probe the formats
// defined in this file.  So you don't need to modify these functions
// when you add a new pixel format into this header file.

#include <vcl_string.h>
#include <vcl_iosfwd.h>

//: Describes the format of pixel encoding
// in a video frame buffer
enum vidl2_pixel_format
{
  VIDL2_PIXEL_FORMAT_UNKNOWN = -1,

  VIDL2_PIXEL_FORMAT_RGB_24,
  VIDL2_PIXEL_FORMAT_BGR_24,
  VIDL2_PIXEL_FORMAT_RGBA_32,
  VIDL2_PIXEL_FORMAT_RGB_565,
  VIDL2_PIXEL_FORMAT_RGB_555,

  VIDL2_PIXEL_FORMAT_YUV_444,
  VIDL2_PIXEL_FORMAT_YUV_444P,
  VIDL2_PIXEL_FORMAT_YUV_422,
  VIDL2_PIXEL_FORMAT_YUV_422P,
  VIDL2_PIXEL_FORMAT_YUV_420P,
  VIDL2_PIXEL_FORMAT_YUV_411,
  VIDL2_PIXEL_FORMAT_YUV_411P,
  VIDL2_PIXEL_FORMAT_YUV_410P,
  VIDL2_PIXEL_FORMAT_UYVY_422,
  VIDL2_PIXEL_FORMAT_UYVY_411,

  VIDL2_PIXEL_FORMAT_MONO_1,
  VIDL2_PIXEL_FORMAT_MONO_8,
  VIDL2_PIXEL_FORMAT_MONO_16,

  // Add values here

  VIDL2_PIXEL_FORMAT_ENUM_END
};


//: Traits of the pixel formats
// - name a string name for the format
// - bits_per_pixel the effective number of bits per pixel
// - num_channels the number of color channels
// - planar are the pixel components arranged on multiple planes
// - packed are the pixels packed into macro pixels
struct vidl2_pixel_traits
{
  vcl_string name;
  unsigned bits_per_pixel;
  unsigned num_channels;
  bool packed;
  bool planar;
};


//: Define traits for a given vidl2_pixel_format
// All pixel traits should be defined using the macro below
template <vidl2_pixel_format pix_type>
struct vidl2_pixel_traits_of;
#define vidl2_pt_mac(F,N,B,C,K,L)\
VCL_DEFINE_SPECIALIZATION \
struct vidl2_pixel_traits_of<VIDL2_PIXEL_FORMAT_##F> \
{\
  static inline vcl_string name() { return N; }\
  enum { bits_per_pixel = B };\
  enum { num_channels = C };\
  enum { packed = K };\
  enum { planar = L };\
}

//            format    name        bpp  nc  packed  planar
//            ------    ---------   ---  --  ------  ------
vidl2_pt_mac( UNKNOWN,  "unknown",  0,   0,  false,  false );

vidl2_pt_mac( RGB_24,   "RGB 24",   24,  3,  false,  false );
vidl2_pt_mac( BGR_24,   "BGR 24",   24,  3,  false,  false );
vidl2_pt_mac( RGBA_32,  "RGBA 32",  32,  4,  false,  false );
vidl2_pt_mac( RGB_565,  "RGB 565",  16,  3,  false,  false );
vidl2_pt_mac( RGB_555,  "RGB 555",  16,  3,  false,  false );

vidl2_pt_mac( YUV_444,  "YUV 444",  24,  3,  false,  false );
vidl2_pt_mac( YUV_444P, "YUV 444P", 24,  3,  false,  true  );
vidl2_pt_mac( YUV_422,  "YUV 422",  16,  3,  true,   false );
vidl2_pt_mac( YUV_422P, "YUV 422P", 16,  3,  false,  true  );
vidl2_pt_mac( YUV_420P, "YUV 420P", 12,  3,  false,  true  );
vidl2_pt_mac( YUV_411,  "YUV 411",  12,  3,  true,   false );
vidl2_pt_mac( YUV_411P, "YUV 411P", 12,  3,  false,  true  );
vidl2_pt_mac( YUV_410P, "YUV 410P", 10,  3,  false,  true  );
vidl2_pt_mac( UYVY_422, "UYVY 422", 16,  3,  true,   false );
vidl2_pt_mac( UYVY_411, "UYVY 411", 12,  3,  true,   false );

vidl2_pt_mac( MONO_1,   "Mono 1",   1,   1,  false,  false );
vidl2_pt_mac( MONO_8,   "Mono 8",   8,   1,  false,  false );
vidl2_pt_mac( MONO_16,  "Mono 16",  16,  1,  false,  false );

#undef vidl2_pt_mac


//=============================================================================
// The following functions provide runtime lookup of pixel traits
// These use template metaprogramming to generate the conditionals to
// check the traits of each vidl2_pixel_format.  You do not need to
// modify the function definitions when adding new types.


//: Return the set of traits for pixel format f
vidl2_pixel_traits
vidl2_pixel_format_traits(vidl2_pixel_format f);


//: Return the effective number of bits per image pixel in pixel format f
inline unsigned
vidl2_pixel_format_bpp(vidl2_pixel_format f)
{
  return vidl2_pixel_format_traits(f).bits_per_pixel;
}


//: Return the number of color channels encoded in pixel format f
inline unsigned
vidl2_pixel_format_num_channels(vidl2_pixel_format f)
{
  return vidl2_pixel_format_traits(f).num_channels;
}


//: Return true if pixels in format f are packed into macropixels
inline bool
vidl2_pixel_format_packed(vidl2_pixel_format f)
{
  return vidl2_pixel_format_traits(f).packed;
}


//: Return true if pixel format f has multiple planes
inline bool
vidl2_pixel_format_planar(vidl2_pixel_format f)
{
  return vidl2_pixel_format_traits(f).planar;
}


//: Output a pretty string representing the pixel format.
vcl_ostream &
operator << (vcl_ostream &os, vidl2_pixel_format f);


//: Convert a string into a pixel format.
inline vcl_string
vidl2_pixel_format_to_string(vidl2_pixel_format f)
{
  return vidl2_pixel_format_traits(f).name;
}


//: Convert a string into a pixel format.
vidl2_pixel_format
vidl2_pixel_format_from_string(const vcl_string& s);


//: Compute the size (in bytes) of a \a ni x \a nj image buffer of pixel format \a f
unsigned
vidl2_pixel_format_buffer_size(unsigned ni, unsigned nj, vidl2_pixel_format f);

#endif // vidl2_pixel_format_h_
