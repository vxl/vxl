// This is core/vidl/vidl_pixel_format.h
#ifndef vidl_pixel_format_h_
#define vidl_pixel_format_h_
//:
// \file
// \brief Supported pixel formats for video frames
//
// \author Matt Leotta
// \date  13 Jan 2006
//
// This file defines the set of known video frame pixel formats.
// The enum vidl_pixel_format enumerates the types while the
// template specializations of vidl_pixel_traits define their
// basic traits.  These specializations are defined using the
// macro vidl_pt_mac.  The pixel traits are:
//  - <b>name</b> a string representation for the format
//  - <b>type</b> the VXL C++ type used to return a pixel component
//  - <b>bits per pixel</b> the number of bits used to represent a pixel
//  - <b>number of channels</b> the number of color channels encoded
//                              (now determined by color)
//  - <b>color</b> the color encoding of the pixels (i.e. RGB, YUV)
//  - <b>arrangement</b> the way pixels are arranged in memory.  This
//       could be in single file, packed into macropixels, or in planes.
//  - <b>chroma shift X</b> the chroma subsampling factor is 2 ^ shift
//       in the horizontal direction.
//  - <b>chroma shift Y</b> the chroma subsampling factor is 2 ^ shift
//       in the vertical direction.
//
// vidl_pixel_format differs from vil_pixel_format in that the
// vidl formats are representations typically used by video
// hardware and in video codecs to encode a frame.  The vil pixel
// formats are more useful for image processing and are related
// to the C++ data types that may be arranged in a regular array
// to make an image.  A vidl_pixel_format may have components
// in multiple planes of different sizes or may have data from
// multiple image pixels encoded as a single macro pixel.
//
// This file also contains several functions to check the traits
// of a pixel format at runtime.  These functions use template
// metaprogramming to generate conditionals that probe the formats
// defined in this file.  So you don't need to modify these functions
// when you add a new pixel format into this header file.

#include <string>
#include <cstddef>
#include <iosfwd>
#include <typeinfo>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vxl_config.h>
#include <vidl/vidl_export.h>

//: Describes the format of pixel encoding in a video frame buffer
enum vidl_pixel_format
{
  VIDL_PIXEL_FORMAT_UNKNOWN = -1,

  VIDL_PIXEL_FORMAT_RGB_24,
  VIDL_PIXEL_FORMAT_RGB_24P,
  VIDL_PIXEL_FORMAT_BGR_24,
  VIDL_PIXEL_FORMAT_RGBA_32,
  VIDL_PIXEL_FORMAT_RGBA_32P,
  VIDL_PIXEL_FORMAT_RGB_565,
  VIDL_PIXEL_FORMAT_RGB_555,

  VIDL_PIXEL_FORMAT_YUV_444P,
  VIDL_PIXEL_FORMAT_YUV_422P,
  VIDL_PIXEL_FORMAT_YUV_420P,
  VIDL_PIXEL_FORMAT_YVU_420P,
  VIDL_PIXEL_FORMAT_YUV_411P,
  VIDL_PIXEL_FORMAT_YUV_410P,
  VIDL_PIXEL_FORMAT_UYV_444,
  VIDL_PIXEL_FORMAT_YUYV_422,
  VIDL_PIXEL_FORMAT_UYVY_422,
  VIDL_PIXEL_FORMAT_UYVY_411,

  VIDL_PIXEL_FORMAT_MONO_1,
  VIDL_PIXEL_FORMAT_MONO_8,
  VIDL_PIXEL_FORMAT_MONO_16,
  VIDL_PIXEL_FORMAT_MONO_F32,
  VIDL_PIXEL_FORMAT_RGB_F32,
  VIDL_PIXEL_FORMAT_RGB_F32P,

  // Add values here
  VIDL_PIXEL_FORMAT_ENUM_END
};


//: Describes the color encoding of a pixel format
enum vidl_pixel_color
{
  VIDL_PIXEL_COLOR_UNKNOWN = -1,

  VIDL_PIXEL_COLOR_MONO,
  VIDL_PIXEL_COLOR_RGB,
  VIDL_PIXEL_COLOR_RGBA,
  VIDL_PIXEL_COLOR_YUV,

  // Add values here

  VIDL_PIXEL_COLOR_ENUM_END
};


//: Describes the arrangement of pixels in a pixel format
enum vidl_pixel_arrangement
{
  VIDL_PIXEL_ARRANGE_UNKNOWN = -1,

  VIDL_PIXEL_ARRANGE_SINGLE,
  VIDL_PIXEL_ARRANGE_PACKED,
  VIDL_PIXEL_ARRANGE_PLANAR,
  VIDL_PIXEL_ARRANGE_PALETTE,

  // Add values here

  VIDL_PIXEL_ARRANGE_ENUM_END
};


//: Traits of the pixel formats
// - name a string name for the format
// - bits_per_pixel the effective number of bits per pixel
// - color the color mode used
// - planar are the pixel components arranged on multiple planes
// - packed are the pixels packed into macro pixels
struct vidl_pixel_traits
{
  std::string name;
  const std::type_info* type;
  unsigned bits_per_pixel;
  unsigned num_channels;
  vidl_pixel_color color;
  vidl_pixel_arrangement arrangement;
  unsigned chroma_shift_x;
  unsigned chroma_shift_y;
};

// ***** Start: temporary hack to avoid conflict *****
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
// ***** End: temporary hack to avoid conflict *****

//: Define limits on the minimum and maximum pixel values
// Also define \a chroma_zero as the U and V value to represent
// gray (no color) in YUV encoding
template <class T>
struct vidl_pixel_limits;

template <>
struct vidl_pixel_limits<vxl_byte>
{
  static inline vxl_byte min() {return 0x00;}
  static inline vxl_byte max() {return 0xFF;}
  static inline vxl_byte chroma_zero() {return 0x80;}
};

template <>
struct vidl_pixel_limits<bool>
{
  static inline bool min() {return false;}
  static inline bool max() {return true;}
  // chroma zero is not well defined, YUV can't be boolean
  static inline bool chroma_zero() {return false;}
};

template <>
struct vidl_pixel_limits<vxl_uint_16>
{
  static inline vxl_uint_16 min() {return 0x0000;}
  static inline vxl_uint_16 max() {return 0xFFFF;}
  static inline vxl_uint_16 chroma_zero() {return 0x8000;}
};

template <>
struct vidl_pixel_limits<float>
{
  static inline float min() {return 0.0f;}
  static inline float max() {return 1.0f;}
  static inline float chroma_zero() {return 0.0f;}
};

template <>
struct vidl_pixel_limits<double>
{
  static inline double min() {return 0.0;}
  static inline double max() {return 1.0;}
  static inline double chroma_zero() {return 0.0f;}
};


//: Define the color traits for each vidl_pixel_color
// For now this is just the number of channels
template <vidl_pixel_color color_type>
    struct vidl_color_traits_of;
#define vidl_ct_mac(COL,NC)\
template <> \
struct vidl_color_traits_of<VIDL_PIXEL_COLOR_##COL> \
{\
  enum { num_channels = (NC) }; \
}

vidl_ct_mac( UNKNOWN,  0 );
vidl_ct_mac( MONO,     1 );
vidl_ct_mac( RGB,      3 );
vidl_ct_mac( RGBA,     4 );
vidl_ct_mac( YUV,      3 );

#undef vidl_ct_mac


//: Define traits for a given vidl_pixel_format
// All pixel traits should be defined using the macro below
// The anonymous enums allow the values available to the
// compiler to for use in generic programming.  For values
// that are already enums, a function is provided so the
// user does not need to worry about enum type clashes.
template <vidl_pixel_format pix_type>
struct vidl_pixel_traits_of;
#define vidl_pt_mac(FMT,NAME,T,BPP,CLR,ARNG,XCS,YCS)\
template <> \
struct vidl_pixel_traits_of<VIDL_PIXEL_FORMAT_##FMT> \
{\
  static inline std::string name() { return NAME; }\
  typedef T type;\
  enum { bits_per_pixel = (BPP) };\
  enum { num_channels = vidl_color_traits_of<VIDL_PIXEL_COLOR_##CLR>::num_channels };\
  static inline vidl_pixel_color color() { return VIDL_PIXEL_COLOR_##CLR; }\
  enum { color_idx = VIDL_PIXEL_COLOR_##CLR };\
  static inline vidl_pixel_arrangement arrangement() { return VIDL_PIXEL_ARRANGE_##ARNG; }\
  enum { arrangement_idx = VIDL_PIXEL_ARRANGE_##ARNG };\
  enum { chroma_shift_x = (XCS) };\
  enum { chroma_shift_y = (YCS) };\
}

//            format    name             type         bpp  color    arrange  xcs  ycs
//            ------    ---------        ----         ---  -------  -------  ---  ---
vidl_pt_mac( UNKNOWN,  "unknown",       void,        0,   UNKNOWN, UNKNOWN, 0,   0  );

vidl_pt_mac( RGB_24,   "RGB 24",        vxl_byte,    24,  RGB,     SINGLE,  0,   0  );
vidl_pt_mac( RGB_24P,  "RGB 24P",       vxl_byte,    24,  RGB,     PLANAR,  0,   0  );
vidl_pt_mac( BGR_24,   "BGR 24",        vxl_byte,    24,  RGB,     SINGLE,  0,   0  );
vidl_pt_mac( RGBA_32,  "RGBA 32",       vxl_byte,    32,  RGBA,    SINGLE,  0,   0  );
vidl_pt_mac( RGBA_32P, "RGBA 32P",      vxl_byte,    32,  RGBA,    PLANAR,  0,   0  );
vidl_pt_mac( RGB_565,  "RGB 565",       vxl_byte,    16,  RGB,     SINGLE,  0,   0  );
vidl_pt_mac( RGB_555,  "RGB 555",       vxl_byte,    16,  RGB,     SINGLE,  0,   0  );

vidl_pt_mac( YUV_444P, "YUV 444P",      vxl_byte,    24,  YUV,     PLANAR,  0,   0  );
vidl_pt_mac( YUV_422P, "YUV 422P",      vxl_byte,    16,  YUV,     PLANAR,  1,   0  );
vidl_pt_mac( YUV_420P, "YUV 420P",      vxl_byte,    12,  YUV,     PLANAR,  1,   1  );
vidl_pt_mac( YVU_420P, "YVU 420P",      vxl_byte,    12,  YUV,     PLANAR,  1,   1  );
vidl_pt_mac( YUV_411P, "YUV 411P",      vxl_byte,    12,  YUV,     PLANAR,  2,   0  );
vidl_pt_mac( YUV_410P, "YUV 410P",      vxl_byte,    10,  YUV,     PLANAR,  2,   1  );
vidl_pt_mac( UYV_444,  "UYV 444",       vxl_byte,    24,  YUV,     SINGLE,  0,   0  );
vidl_pt_mac( YUYV_422, "YUYV 422",      vxl_byte,    16,  YUV,     PACKED,  1,   0  );
vidl_pt_mac( UYVY_422, "UYVY 422",      vxl_byte,    16,  YUV,     PACKED,  1,   0  );
vidl_pt_mac( UYVY_411, "UYVY 411",      vxl_byte,    12,  YUV,     PACKED,  2,   0  );

vidl_pt_mac( MONO_1,   "Mono 1",        bool,        1,   MONO,    SINGLE,  0,   0  );
vidl_pt_mac( MONO_8,   "Mono 8",        vxl_byte,    8,   MONO,    SINGLE,  0,   0  );
vidl_pt_mac( MONO_16,  "Mono 16",       vxl_uint_16, 16,  MONO,    SINGLE,  0,   0  );
vidl_pt_mac( MONO_F32, "Mono float 32", vxl_ieee_32, 32,  MONO,    SINGLE,  0,   0  );
vidl_pt_mac( RGB_F32,  "RGB float 32",  vxl_ieee_32, 96,  RGB,     SINGLE,  0,   0  );
vidl_pt_mac( RGB_F32P, "RGB float 32P", vxl_ieee_32, 96,  RGB,     PLANAR,  0,   0  );

#undef vidl_pt_mac


//: Define the packing order for each packed vidl_pixel_format
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
//
// \note the offset arrays are defined in vidl_pixel_format.cxx
template <vidl_pixel_format pix_type>
struct vidl_pixel_pack_of;
#define vidl_pp_mac(FMT)\
template <> \
struct vidl_pixel_pack_of<VIDL_PIXEL_FORMAT_##FMT> \
{\
  enum { macro_pix_size = 1<<vidl_pixel_traits_of<VIDL_PIXEL_FORMAT_##FMT>::chroma_shift_x }; \
  enum { num_channels = vidl_pixel_traits_of<VIDL_PIXEL_FORMAT_##FMT>::num_channels }; \
  static VIDL_EXPORT const std::ptrdiff_t offset[macro_pix_size][num_channels]; \
}

vidl_pp_mac( YUYV_422 );
vidl_pp_mac( UYVY_422 );
vidl_pp_mac( UYVY_411 );

#undef vidl_pp_mac


//=============================================================================
// The following functions provide runtime lookup of pixel traits
// These use template metaprogramming to generate the conditionals to
// check the traits of each vidl_pixel_format.  You do not need to
// modify the function definitions when adding new types.


//: Return the number of channels needed in a color mode
unsigned
vidl_pixel_color_num_channels(vidl_pixel_color c);


//: Return the set of traits for pixel format f
vidl_pixel_traits
vidl_pixel_format_traits(vidl_pixel_format f);


//: Return the typeid of the pixel format datatype
inline const std::type_info&
vidl_pixel_format_typeid(vidl_pixel_format f)
{
  return *vidl_pixel_format_traits(f).type;
}

//: Return the effective number of bits per image pixel in pixel format f
inline unsigned
vidl_pixel_format_bpp(vidl_pixel_format f)
{
  return vidl_pixel_format_traits(f).bits_per_pixel;
}


//: Return the number of color channels encoded in pixel format f
inline unsigned
vidl_pixel_format_num_channels(vidl_pixel_format f)
{
  return vidl_pixel_format_traits(f).num_channels;
}


//: Return the color encoding for the pixel format
inline vidl_pixel_color
vidl_pixel_format_color(vidl_pixel_format f)
{
  return vidl_pixel_format_traits(f).color;
}


//: Return the pixel arrangement for a given format
inline vidl_pixel_arrangement
vidl_pixel_format_arrangement(vidl_pixel_format f)
{
  return vidl_pixel_format_traits(f).arrangement;
}


//: Return the chroma shift in the horizontal direction
inline unsigned
vidl_pixel_format_chroma_shift_x(vidl_pixel_format f)
{
  return vidl_pixel_format_traits(f).chroma_shift_x;
}


//: Return the chroma shift in the vertical direction
inline unsigned
vidl_pixel_format_chroma_shift_y(vidl_pixel_format f)
{
  return vidl_pixel_format_traits(f).chroma_shift_y;
}


//: Output a pretty string representing the pixel format.
std::ostream &
operator << (std::ostream &os, vidl_pixel_format f);


//: Convert a string into a pixel format.
inline std::string
vidl_pixel_format_to_string(vidl_pixel_format f)
{
  return vidl_pixel_format_traits(f).name;
}


//: Convert a string into a pixel format.
vidl_pixel_format
vidl_pixel_format_from_string(const std::string& s);


//: Compute the size (in bytes) of a \a ni x \a nj image buffer of pixel format \a f
unsigned
vidl_pixel_format_buffer_size(unsigned ni, unsigned nj, vidl_pixel_format f);

#endif // vidl_pixel_format_h_
