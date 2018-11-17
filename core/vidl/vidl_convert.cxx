// This is core/vidl/vidl_convert.cxx
//:
// \file
// \author Matt Leotta
// \date   20 Jan 2006
//
// \verbatim
//  Modifications
//   10 Jul.2008 - Antonio Garrido - Added conversions for RGB_24(P),MONO8 and YUYV_422
// \endverbatim
//
//-----------------------------------------------------------------------------

#include <cstring>
#include <iostream>
#include <memory>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "vidl_convert.h"
#include "vidl_frame.h"
#include "vidl_pixel_format.h"
#include "vidl_pixel_iterator.hxx"
#include "vidl_color.h"
#include <vidl/vidl_config.h>
#if VIDL_HAS_FFMPEG
// make use of the convert function using ffmpeg
#include "vidl_ffmpeg_convert.h"
#endif

#include <vil/vil_convert.h>
#include <vil/vil_new.h>
#include <vil/vil_memory_chunk.h>
#include <cassert>

//--------------------------------------------------------------------------------


namespace {

//: Define the function pointer for pixel format conversion functions
typedef bool (*converter_func)(vidl_frame const& in_frame, vidl_frame& out_frame);


//: Default pixel format conversion - it fails
bool default_conversion(vidl_frame const& in_frame, vidl_frame& out_frame)
{
  std::cerr << "No routine to convert " << in_frame.pixel_format()
           << " to " << out_frame.pixel_format() << std::endl;
  return false;
}


//: Use memcpy when the formats are the same
bool copy_conversion(vidl_frame const& in_frame, vidl_frame& out_frame)
{
  assert(in_frame.pixel_format() == out_frame.pixel_format());
  assert(in_frame.size() == out_frame.size());
  std::memcpy(out_frame.data(), in_frame.data(), in_frame.size());
  return true;
}


// Default pixel format conversion - it fails
template <vidl_pixel_format in_Fmt, vidl_pixel_format out_Fmt>
struct convert
{
  enum { defined = false };
  static inline bool apply(vidl_frame const& /*in_frame*/,
                           vidl_frame& /*out_frame*/)
  {
    return false;
  }
};

//=============================================================================
// Start of generic pixel conversions


//: The generic pixel conversion function
bool convert_generic(vidl_frame const& in_frame,
                     vidl_frame& out_frame)
{
  // create pixel iterators for each frame
  std::unique_ptr<vidl_pixel_iterator> in_pitr(vidl_make_pixel_iterator(in_frame));
  if (!in_pitr.get())
    return false;
  std::unique_ptr<vidl_pixel_iterator> out_pitr(vidl_make_pixel_iterator(out_frame));
  if (!out_pitr.get())
    return false;

  vidl_pixel_iterator& in_itr = *in_pitr;
  vidl_pixel_iterator& out_itr = *out_pitr;

  vidl_pixel_traits in_t = vidl_pixel_format_traits(in_frame.pixel_format());
  vidl_pixel_traits out_t = vidl_pixel_format_traits(out_frame.pixel_format());

  // find the color conversion function
  vidl_color_conv_fptr color_conv =
      vidl_color_converter_func( in_t.color,  *in_t.type,
                                 out_t.color, *out_t.type);
  if (!color_conv)
    return false;

  const unsigned int num_pix = in_frame.ni() * in_frame.nj();
  // assume pixels are no more than 32 bytes (that's 4 doubles)
  vxl_byte in_pixel[32], out_pixel[32];
  for (unsigned int c=0; c<num_pix; ++c, ++in_itr, ++out_itr) {
    in_itr.get_data(in_pixel);
    color_conv(in_pixel, out_pixel);
    out_itr.set_data(out_pixel);
  }
  return true;
}


//=============================================================================
// Start of pixel conversion specializations
// Write optimized conversion specializations below

// RGB_24 to UYVY_422
template <>
struct convert<VIDL_PIXEL_FORMAT_RGB_24, VIDL_PIXEL_FORMAT_UYVY_422>
{
  enum { defined = true };
  static bool apply(vidl_frame const& in_frame,
                    vidl_frame& out_frame)
  {
    assert(in_frame.pixel_format()==VIDL_PIXEL_FORMAT_RGB_24);
    assert(out_frame.pixel_format()==VIDL_PIXEL_FORMAT_UYVY_422);
    const auto* rgb = reinterpret_cast<const vxl_byte*>(in_frame.data());
    auto* uyvy = reinterpret_cast<vxl_byte*>(out_frame.data());
    unsigned int num_half_pix = (in_frame.ni() * in_frame.nj() + 1)/2;
    for (unsigned int c=0; c<num_half_pix; ++c) {
      const vxl_byte& r1 = *(rgb++);
      const vxl_byte& g1 = *(rgb++);
      const vxl_byte& b1 = *(rgb++);
      const vxl_byte& r2 = *(rgb++);
      const vxl_byte& g2 = *(rgb++);
      const vxl_byte& b2 = *(rgb++);
      vxl_byte y1,u1,v1,y2,u2,v2;
      vidl_color_convert_rgb2yuv(r1,g1,b1,y1,u1,v1);
      vidl_color_convert_rgb2yuv(r2,g2,b2,y2,u2,v2);
      *(uyvy++) = (u1+u2)/2u;
      *(uyvy++) = y1;
      *(uyvy++) = (v1+v2)/2u;
      *(uyvy++) = y2;
    }
    return true;
  }
};


// UYVY_422 to RGB_24
template <>
struct convert<VIDL_PIXEL_FORMAT_UYVY_422, VIDL_PIXEL_FORMAT_RGB_24>
{
  enum { defined = true };
  static bool apply(vidl_frame const& in_frame,
                    vidl_frame& out_frame)
  {
    assert(in_frame.pixel_format()==VIDL_PIXEL_FORMAT_UYVY_422);
    assert(out_frame.pixel_format()==VIDL_PIXEL_FORMAT_RGB_24);
    const auto* uyvy = reinterpret_cast<const vxl_byte*>(in_frame.data());
    auto* rgb = reinterpret_cast<vxl_byte*>(out_frame.data());
    unsigned int num_half_pix = (in_frame.ni() * in_frame.nj() + 1)/2;
    for (unsigned int c=0; c<num_half_pix; ++c) {
      const vxl_byte& u1 = *(uyvy++);
      const vxl_byte& y1 = *(uyvy++);
      const vxl_byte& v1 = *(uyvy++);
      const vxl_byte& y2 = *(uyvy++);
      vxl_byte r,g,b;
      vidl_color_convert_yuv2rgb(y1,u1,v1,r,g,b);
      *(rgb++) = r;
      *(rgb++) = g;
      *(rgb++) = b;
      vidl_color_convert_yuv2rgb(y2,u1,v1,r,g,b);
      *(rgb++) = r;
      *(rgb++) = g;
      *(rgb++) = b;
    }
    return true;
  }
};


// UYVY_422 to MONO_8
template <>
struct convert<VIDL_PIXEL_FORMAT_UYVY_422, VIDL_PIXEL_FORMAT_MONO_8>
{
  enum { defined = true };
  static bool apply(vidl_frame const& in_frame,
                    vidl_frame& out_frame)
  {
    assert(in_frame.pixel_format()==VIDL_PIXEL_FORMAT_UYVY_422);
    assert(out_frame.pixel_format()==VIDL_PIXEL_FORMAT_MONO_8);
    const auto* uyvy = reinterpret_cast<const vxl_byte*>(in_frame.data());
    auto* mono = reinterpret_cast<vxl_byte*>(out_frame.data());
    unsigned int num_half_pix = (in_frame.ni() * in_frame.nj() + 1)/2;
    for (unsigned int c=0; c<num_half_pix; ++c) {
      ++uyvy;
      const vxl_byte& y1 = *(uyvy++);
      ++uyvy;
      const vxl_byte& y2 = *(uyvy++);
      *(mono++) = y1;
      *(mono++) = y2;
    }
    return true;
  }
};


// RGB_24 to YUYV_422
template <>
struct convert<VIDL_PIXEL_FORMAT_RGB_24, VIDL_PIXEL_FORMAT_YUYV_422>
{
  enum { defined = true };
  static bool apply(vidl_frame const& in_frame,
                    vidl_frame& out_frame)
  {
    assert(in_frame.pixel_format()==VIDL_PIXEL_FORMAT_RGB_24);
    assert(out_frame.pixel_format()==VIDL_PIXEL_FORMAT_YUYV_422);
    const auto* rgb = reinterpret_cast<const vxl_byte*>(in_frame.data());
    auto* yuyv = reinterpret_cast<vxl_byte*>(out_frame.data());
    unsigned int num_half_pix = (in_frame.ni() * in_frame.nj() + 1)/2;
    for (unsigned int c=0; c<num_half_pix; ++c) {
      const vxl_byte& r1 = *(rgb++);
      const vxl_byte& g1 = *(rgb++);
      const vxl_byte& b1 = *(rgb++);
      const vxl_byte& r2 = *(rgb++);
      const vxl_byte& g2 = *(rgb++);
      const vxl_byte& b2 = *(rgb++);
      vxl_byte y1,u1,v1,y2,u2,v2;
      vidl_color_convert_rgb2yuv(r1,g1,b1,y1,u1,v1);
      vidl_color_convert_rgb2yuv(r2,g2,b2,y2,u2,v2);
      *(yuyv++) = y1;
      *(yuyv++) = (u1+u2)/2u;
      *(yuyv++) = y2;
      *(yuyv++) = (v1+v2)/2u;
    }
    return true;
  }
};


// YUYV_422 to RGB_24
template <>
struct convert<VIDL_PIXEL_FORMAT_YUYV_422, VIDL_PIXEL_FORMAT_RGB_24>
{
  enum { defined = true };
  static bool apply(vidl_frame const& in_frame,
                    vidl_frame& out_frame)
  {
    assert(in_frame.pixel_format()==VIDL_PIXEL_FORMAT_YUYV_422);
    assert(out_frame.pixel_format()==VIDL_PIXEL_FORMAT_RGB_24);
    const auto* yuyv = reinterpret_cast<const vxl_byte*>(in_frame.data());
    auto* rgb = reinterpret_cast<vxl_byte*>(out_frame.data());
    unsigned int num_half_pix = (in_frame.ni() * in_frame.nj() + 1)/2;
    for (unsigned int c=0; c<num_half_pix; ++c) {
      const vxl_byte& y1 = *(yuyv++);
      const vxl_byte& u1 = *(yuyv++);
      const vxl_byte& y2 = *(yuyv++);
      const vxl_byte& v1 = *(yuyv++);
      vxl_byte r,g,b;
      vidl_color_convert_yuv2rgb(y1,u1,v1,r,g,b);
      *(rgb++) = r;
      *(rgb++) = g;
      *(rgb++) = b;
      vidl_color_convert_yuv2rgb(y2,u1,v1,r,g,b);
      *(rgb++) = r;
      *(rgb++) = g;
      *(rgb++) = b;
    }
    return true;
  }
};


// RGB_24P to YUYV_422
template <>
struct convert<VIDL_PIXEL_FORMAT_RGB_24P, VIDL_PIXEL_FORMAT_YUYV_422>
{
  enum { defined = true };
  static bool apply(vidl_frame const& in_frame,
                    vidl_frame& out_frame)
  {
    assert(in_frame.pixel_format()==VIDL_PIXEL_FORMAT_RGB_24P);
    assert(out_frame.pixel_format()==VIDL_PIXEL_FORMAT_YUYV_422);
    const auto* red = reinterpret_cast<const vxl_byte*>(in_frame.data());
    const vxl_byte* green= red+in_frame.ni() * in_frame.nj();
    const vxl_byte* blue= green+in_frame.ni() * in_frame.nj();
    auto* yuyv = reinterpret_cast<vxl_byte*>(out_frame.data());
    unsigned int num_half_pix = (in_frame.ni() * in_frame.nj() + 1)/2;
    for (unsigned int c=0; c<num_half_pix; ++c) {
      const vxl_byte& r1 = *(red++);
      const vxl_byte& g1 = *(green++);
      const vxl_byte& b1 = *(blue++);
      const vxl_byte& r2 = *(red++);
      const vxl_byte& g2 = *(green++);
      const vxl_byte& b2 = *(blue++);
      vxl_byte y1,u1,v1,y2,u2,v2;
      vidl_color_convert_rgb2yuv(r1,g1,b1,y1,u1,v1);
      vidl_color_convert_rgb2yuv(r2,g2,b2,y2,u2,v2);
      *(yuyv++) = y1;
      *(yuyv++) = (u1+u2)/2u;
      *(yuyv++) = y2;
      *(yuyv++) = (v1+v2)/2u;
    }
    return true;
  }
};

// YUYV_422 to RGB_24P
template <>
struct convert<VIDL_PIXEL_FORMAT_YUYV_422, VIDL_PIXEL_FORMAT_RGB_24P>
{
  enum { defined = true };
  static bool apply(vidl_frame const& in_frame,
                    vidl_frame& out_frame)
  {
    assert(in_frame.pixel_format()==VIDL_PIXEL_FORMAT_YUYV_422);
    assert(out_frame.pixel_format()==VIDL_PIXEL_FORMAT_RGB_24P);
    const auto* yuyv = reinterpret_cast<const vxl_byte*>(in_frame.data());
    auto* red = reinterpret_cast<vxl_byte*>(out_frame.data());
    vxl_byte* green = red+out_frame.ni()*out_frame.nj();
    vxl_byte* blue = green+out_frame.ni()*out_frame.nj();
    unsigned int num_half_pix = (in_frame.ni() * in_frame.nj() + 1)/2;
    for (unsigned int c=0; c<num_half_pix; ++c) {
      const vxl_byte& y1 = *(yuyv++);
      const vxl_byte& u1 = *(yuyv++);
      const vxl_byte& y2 = *(yuyv++);
      const vxl_byte& v1 = *(yuyv++);
      vxl_byte r,g,b;
      vidl_color_convert_yuv2rgb(y1,u1,v1,r,g,b);
      *(red++) = r;
      *(green++) = g;
      *(blue++) = b;
      vidl_color_convert_yuv2rgb(y2,u1,v1,r,g,b);
      *(red++) = r;
      *(green++) = g;
      *(blue++) = b;
    }
    return true;
  }
};

// YUYV_422 to MONO_8
template <>
struct convert<VIDL_PIXEL_FORMAT_YUYV_422, VIDL_PIXEL_FORMAT_MONO_8>
{
  enum { defined = true };
  static bool apply(vidl_frame const& in_frame,
                    vidl_frame& out_frame)
  {
    assert(in_frame.pixel_format()==VIDL_PIXEL_FORMAT_YUYV_422);
    assert(out_frame.pixel_format()==VIDL_PIXEL_FORMAT_MONO_8);
    const auto* yuyv = reinterpret_cast<const vxl_byte*>(in_frame.data());
    auto* mono = reinterpret_cast<vxl_byte*>(out_frame.data());
    unsigned int num_half_pix = (in_frame.ni() * in_frame.nj() + 1)/2;
    for (unsigned int c=0; c<num_half_pix; ++c) {
      const vxl_byte& y1 = *(yuyv++);
      ++yuyv;
      const vxl_byte& y2 = *(yuyv++);
      ++yuyv;
      *(mono++) = y1;
      *(mono++) = y2;
    }
    return true;
  }
};


// End of pixel conversion specializations
//=============================================================================


//: Generates an entry into the table of pixel format conversions functions
// This is called for every pair for pixel formats to build the table
template <vidl_pixel_format in_Fmt, vidl_pixel_format out_Fmt>
struct table_entry_init
{
  static inline void set_entry(converter_func& table_entry)
  {
    // This should be done at compile time with partial specialization
    // This run time code generates many functions that are never actually used
    if (in_Fmt == out_Fmt)
      table_entry = &copy_conversion;
    else if (convert<in_Fmt,out_Fmt>::defined)
      table_entry = &convert<in_Fmt,out_Fmt>::apply;
    else if (vidl_pixel_iterator_valid<in_Fmt>::value &&
             vidl_pixel_iterator_valid<out_Fmt>::value)
      table_entry = &convert_generic;
    else
      table_entry = &default_conversion;
  }
};

//: Recursive template metaprogram to generate conditionals for converting each pair of pixel types
template <int Fmt_Code>
struct table_init
{
  static inline void populate(converter_func table[VIDL_PIXEL_FORMAT_ENUM_END][VIDL_PIXEL_FORMAT_ENUM_END])
  {
    const auto in_fmt = vidl_pixel_format(Fmt_Code/VIDL_PIXEL_FORMAT_ENUM_END);
    const auto out_fmt = vidl_pixel_format(Fmt_Code%VIDL_PIXEL_FORMAT_ENUM_END);
    table_entry_init<in_fmt,out_fmt>::set_entry(table[in_fmt][out_fmt]);
    table_init<Fmt_Code-1>::populate(table);
  }
};


//: The base case
template <>
struct table_init<0>
{
  static inline void populate(converter_func table[VIDL_PIXEL_FORMAT_ENUM_END][VIDL_PIXEL_FORMAT_ENUM_END])
  {
    const auto in_fmt = vidl_pixel_format(0);
    const auto out_fmt = vidl_pixel_format(0);
    table_entry_init<in_fmt,out_fmt>::set_entry(table[in_fmt][out_fmt]);
  }
};


//: A table of all conversion functions
class converter
{
 public:
  //: Constructor - generate the table
  converter()
  {
    // generate the table of function pointers
    table_init<VIDL_PIXEL_FORMAT_ENUM_END*VIDL_PIXEL_FORMAT_ENUM_END-1>::populate(table);
  }

  //: Apply the conversion
  bool operator()(vidl_frame const& in_frame, vidl_frame& out_frame) const
  {
    return (*table[in_frame.pixel_format()][out_frame.pixel_format()])(in_frame, out_frame);
  }
 private:
  //: Table of conversion functions
  converter_func table[VIDL_PIXEL_FORMAT_ENUM_END][VIDL_PIXEL_FORMAT_ENUM_END];
};

//: Instantiate a global conversion function table
converter conversion_table;

#if 0
//: Convert to an intermediate RGB_24 frame
// This is inefficient, but will provide the functionality until
// an optimized version is written
// defined later because it uses conversion_table
bool intermediate_rgb24_conversion(vidl_frame const& in_frame, vidl_frame& out_frame);


//: Convert to an intermediate RGB_24 frame
// Defined here because it uses conversion_table
bool intermediate_rgb24_conversion(vidl_frame const& in_frame, vidl_frame& out_frame)
{
  unsigned int ni = in_frame.ni(), nj = in_frame.nj();
  vil_memory_chunk_sptr memory = new vil_memory_chunk(ni*nj*3, VIL_PIXEL_FORMAT_BYTE);
  vidl_memory_chunk_frame temp_frame(ni,nj,VIDL_PIXEL_FORMAT_RGB_24,memory);
  return conversion_table(in_frame, temp_frame) &&
      conversion_table(temp_frame, out_frame);
}
#endif

} // end anonymous namespace

//--------------------------------------------------------------------------------

//: Convert the pixel format of a frame
//
// The \p in_frame->data() is converted from \p in_frame->pixel_format()
// to \p out_frame->pixel_format() and stored in \p out_frame->data()
// \returns false if the output frame data is not the correct size.
bool vidl_convert_frame(vidl_frame const& in_frame,
                        vidl_frame& out_frame)
{
  vidl_pixel_format in_fmt = in_frame.pixel_format();
  vidl_pixel_format out_fmt = out_frame.pixel_format();

  if (in_fmt  == VIDL_PIXEL_FORMAT_UNKNOWN ||
      out_fmt == VIDL_PIXEL_FORMAT_UNKNOWN)
    return false;

  unsigned ni = in_frame.ni();
  unsigned nj = in_frame.nj();
  unsigned out_size = vidl_pixel_format_buffer_size(ni,nj,out_fmt);

  if (out_frame.size() != out_size ||
      out_frame.ni() != ni ||
      out_frame.nj() != nj ||
      !out_frame.data() )
    return false;

  // call the appropriate function in the conversion table
  bool const ret = conversion_table(in_frame, out_frame);

#if VIDL_HAS_FFMPEG
  // Fall back to the function that utilizes ffmpeg's conversion
  if (!ret)
    return vidl_ffmpeg_convert(in_frame, out_frame);
#endif // VIDL_HAS_FFMPEG
  return ret;
}

//: Convert the pixel format of a frame
//
// The convert \p in_frame to a \p format by allocating
// a new frame buffer
vidl_frame_sptr vidl_convert_frame(const vidl_frame_sptr& in_frame,
                                   vidl_pixel_format format)
{
  if (format == VIDL_PIXEL_FORMAT_UNKNOWN)
    return nullptr;

  unsigned ni = in_frame->ni();
  unsigned nj = in_frame->nj();
  unsigned size = vidl_pixel_format_buffer_size(ni,nj,format);
  vil_memory_chunk_sptr memory = new vil_memory_chunk(size, VIL_PIXEL_FORMAT_BYTE);
  vidl_frame_sptr out_frame = new vidl_memory_chunk_frame(ni, nj, format, memory);

  if (vidl_convert_frame(*in_frame, *out_frame))
    return out_frame;

  return nullptr;
}


//: Convert the image view to a frame
// Will wrap the memory if possible, if not the image is converted to
// the closest vidl_pixel_format
vidl_frame_sptr vidl_convert_to_frame(const vil_image_view_base_sptr& image)
{
  if (!image)
    return nullptr;
  return vidl_convert_to_frame(*image);
}


//: Convert the image view to a frame
// Will wrap the memory if possible, if not the image is converted to
// the closest vidl_pixel_format
vidl_frame_sptr vidl_convert_to_frame(const vil_image_view_base& image)
{
  // try to wrap the image memory in a frame
  vidl_frame_sptr frame = new vidl_memory_chunk_frame(image);
  if (frame->pixel_format() != VIDL_PIXEL_FORMAT_UNKNOWN)
    return frame;

  // if the image could not be wrapped convert it
  unsigned ni = image.ni(), nj = image.nj(), np = image.nplanes();

  // use the pixel component format to account for
  // images of type vil_rgb<T>, vil_rgba<T>, etc.
  vil_pixel_format cmp_format =
      vil_pixel_format_component_format(image.pixel_format());
  unsigned int num_cmp = vil_pixel_format_num_components(image.pixel_format());
  unsigned int num_channels = np * num_cmp;

  // special case for 16 bit images
  if (cmp_format == VIL_PIXEL_FORMAT_UINT_16)
  {
    if (num_channels == 1)
    {
      vil_image_view<vxl_uint_16> img(ni,nj);
      img.deep_copy(vil_image_view<vxl_uint_16>(image));
      return new vidl_memory_chunk_frame(ni, nj, VIDL_PIXEL_FORMAT_MONO_16,
                                         img.memory_chunk());
    }
  }
  // special case for 32 bit float images
  else if (cmp_format == VIL_PIXEL_FORMAT_FLOAT)
  {
    if (num_channels == 1 || num_channels == 3)
    {
      vidl_pixel_format format = VIDL_PIXEL_FORMAT_UNKNOWN;
      if (num_channels == 1)
        format = VIDL_PIXEL_FORMAT_MONO_F32;
      else
        format = VIDL_PIXEL_FORMAT_RGB_F32P;

      vil_image_view<vxl_ieee_32> img(ni,nj,num_channels);
      img.deep_copy(vil_image_view<vxl_ieee_32>(image));
      return new vidl_memory_chunk_frame(ni, nj, format,
                                         img.memory_chunk());
    }
  }

  vidl_pixel_format format = VIDL_PIXEL_FORMAT_UNKNOWN;
  if (num_channels == 1)
    format = VIDL_PIXEL_FORMAT_MONO_8;
  else if (num_channels == 3)
    format = VIDL_PIXEL_FORMAT_RGB_24P;
  else if (num_channels == 4)
    format = VIDL_PIXEL_FORMAT_RGBA_32P;
  else
    return nullptr;

  vil_image_view<vxl_byte> img;
  if (image.pixel_format() == VIL_PIXEL_FORMAT_BYTE)
    img.deep_copy(vil_image_view<vxl_byte>(image));
  else
  {
    vil_image_resource_sptr resrc = vil_new_image_resource_of_view(image);
    vil_image_view_base_sptr bimage = vil_convert_cast(vxl_byte(),resrc->get_view());
    if (!bimage)
      return nullptr;
    img = *bimage;
  }
  return new vidl_memory_chunk_frame(ni, nj, format,
                                     img.memory_chunk());
}


//: convert the frame into an image view
// possibly converts the pixel data type
// always create a deep copy of the data
bool vidl_convert_to_view(vidl_frame const& frame,
                          vil_image_view_base& image,
                          vidl_pixel_color require_color)
{
  if (frame.pixel_format() == VIDL_PIXEL_FORMAT_UNKNOWN ||
      frame.data() == nullptr)
    return false;

  vidl_pixel_color in_color = vidl_pixel_format_color(frame.pixel_format());
  if (require_color == VIDL_PIXEL_COLOR_UNKNOWN)
    require_color = in_color;

  unsigned ni = frame.ni(), nj = frame.nj();
  unsigned np = vidl_pixel_color_num_channels(require_color);

  // resize the image if necessary
  image.set_size(ni,nj,np);

  // special case for MONO_16
  if (frame.pixel_format() == VIDL_PIXEL_FORMAT_MONO_16) {
    vil_image_view<vxl_uint_16> wrapper(static_cast<const vxl_uint_16*>(frame.data()),
                                        ni,nj,1,1,ni,ni*nj);
    if (image.pixel_format() == VIL_PIXEL_FORMAT_UINT_16) {
      auto& img = static_cast<vil_image_view<vxl_uint_16>&>(image);
      img.deep_copy(vil_image_view<vxl_uint_16>(wrapper));
      return true;
    }

    switch ( vil_pixel_format_component_format(image.pixel_format()) ) {
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define macro(F , T) \
    case F: {\
      vil_image_view<T> & dest_ref = static_cast<vil_image_view<T> &>(image); \
      vil_convert_cast( wrapper, dest_ref); break;}

#if VXL_HAS_INT_64
    macro( VIL_PIXEL_FORMAT_UINT_64, vxl_uint_64 );
    macro( VIL_PIXEL_FORMAT_INT_64, vxl_int_64 );
#endif
    macro( VIL_PIXEL_FORMAT_UINT_32, vxl_uint_32 );
    macro( VIL_PIXEL_FORMAT_INT_32, vxl_int_32 );
    macro( VIL_PIXEL_FORMAT_INT_16, vxl_int_16 );
    macro( VIL_PIXEL_FORMAT_BYTE, vxl_byte );
    macro( VIL_PIXEL_FORMAT_SBYTE, vxl_sbyte );
    macro( VIL_PIXEL_FORMAT_FLOAT, float );
    macro( VIL_PIXEL_FORMAT_DOUBLE, double );
    macro( VIL_PIXEL_FORMAT_BOOL, bool );
#undef macro
#endif // DOXYGEN_SHOULD_SKIP_THIS
      default:
        return false;
    }
    return true;
  }

  vidl_pixel_format default_format = VIDL_PIXEL_FORMAT_UNKNOWN;
  if (image.pixel_format() == VIL_PIXEL_FORMAT_BYTE)
  {
    auto& img = static_cast<vil_image_view<vxl_byte>&>(image);
    bool interleaved = (img.planestep() == 1);

    switch (require_color) {
      case VIDL_PIXEL_COLOR_MONO:
        default_format = VIDL_PIXEL_FORMAT_MONO_8; break;
      case VIDL_PIXEL_COLOR_RGB:
        default_format = interleaved?VIDL_PIXEL_FORMAT_RGB_24
                                    :VIDL_PIXEL_FORMAT_RGB_24P; break;
      case VIDL_PIXEL_COLOR_RGBA:
        default_format = interleaved?VIDL_PIXEL_FORMAT_RGBA_32
                                    :VIDL_PIXEL_FORMAT_RGBA_32P; break;
      case VIDL_PIXEL_COLOR_YUV:
        default_format = interleaved?VIDL_PIXEL_FORMAT_UYV_444
                                    :VIDL_PIXEL_FORMAT_YUV_444P; break;
      default:
        break;
    }
  }

  vidl_frame_sptr out_frame = new vidl_memory_chunk_frame(image,default_format);
  // if the image can be wrapped as a frame
  if (out_frame->pixel_format() != VIDL_PIXEL_FORMAT_UNKNOWN) {
    vidl_convert_frame(frame, *out_frame);
    return true;
  }

  // use an intermediate format
  vidl_pixel_format out_fmt;
  switch (in_color) {
    case VIDL_PIXEL_COLOR_MONO:
      out_fmt = VIDL_PIXEL_FORMAT_MONO_8; break;
    case VIDL_PIXEL_COLOR_RGB:
      out_fmt = VIDL_PIXEL_FORMAT_RGB_24P; break;
    case VIDL_PIXEL_COLOR_RGBA:
      out_fmt = VIDL_PIXEL_FORMAT_RGBA_32P; break;
    case VIDL_PIXEL_COLOR_YUV:
      out_fmt = VIDL_PIXEL_FORMAT_YUV_444P; break;
    default:
      return false;
  }

  vil_image_view<vxl_byte> temp(ni,nj,np);
  out_frame = new vidl_memory_chunk_frame(ni,nj,out_fmt,temp.memory_chunk());
  vidl_convert_frame(frame, *out_frame);

  switch ( vil_pixel_format_component_format(image.pixel_format()) ) {
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define macro(F , T) \
    case F: {\
      vil_image_view<T> & dest_ref = static_cast<vil_image_view<T> &>(image); \
      vil_convert_cast( temp, dest_ref); break;}

#if VXL_HAS_INT_64
    macro( VIL_PIXEL_FORMAT_UINT_64, vxl_uint_64 );
    macro( VIL_PIXEL_FORMAT_INT_64, vxl_int_64 );
#endif
    macro( VIL_PIXEL_FORMAT_UINT_32, vxl_uint_32 );
    macro( VIL_PIXEL_FORMAT_INT_32, vxl_int_32 );
    macro( VIL_PIXEL_FORMAT_UINT_16, vxl_uint_16 );
    macro( VIL_PIXEL_FORMAT_INT_16, vxl_int_16 );
    macro( VIL_PIXEL_FORMAT_BYTE, vxl_byte );
    macro( VIL_PIXEL_FORMAT_SBYTE, vxl_sbyte );
    macro( VIL_PIXEL_FORMAT_FLOAT, float );
    macro( VIL_PIXEL_FORMAT_DOUBLE, double );
    macro( VIL_PIXEL_FORMAT_BOOL, bool );
#undef macro
#endif // DOXYGEN_SHOULD_SKIP_THIS
    default:
      return false;
  }
  return true;
}


//: Wrap the frame buffer in an image view if supported
// Returns a null pointer if not possible
vil_image_view_base_sptr
vidl_convert_wrap_in_view(vidl_frame const& frame)
{
  vidl_pixel_format format = frame.pixel_format();
  vidl_pixel_traits pt =  vidl_pixel_format_traits(format);
  if ( pt.chroma_shift_x != 0 || pt.chroma_shift_y != 0 ||
       pt.bits_per_pixel % pt.num_channels != 0)
    return nullptr;

  unsigned ni = frame.ni(), nj = frame.nj();
  unsigned np = pt.num_channels;
  std::ptrdiff_t i_step, j_step, p_step;
  switch (pt.arrangement) {
    case VIDL_PIXEL_ARRANGE_SINGLE:
      i_step = np;
      j_step = np*ni;
      p_step = 1;
      break;
    case VIDL_PIXEL_ARRANGE_PLANAR:
      i_step = 1;
      j_step = ni;
      p_step = ni*nj;
      break;
    default:
      // Cannot wrap other pixel arrangements
      return nullptr;
  }
  std::ptrdiff_t top_left_offset = 0;

  if (format == VIDL_PIXEL_FORMAT_BGR_24) {
    top_left_offset = 2;
    p_step = -1;
  }

  // Create a view of a memory chunk frame
  if ( const auto* cf =
       dynamic_cast<const vidl_memory_chunk_frame*>(&frame) )
  {
    const vil_memory_chunk_sptr& chunk = cf->memory_chunk();
    if (format == VIDL_PIXEL_FORMAT_MONO_16) {
      const vxl_uint_16* top_left = static_cast<const vxl_uint_16*>(cf->data()) + top_left_offset;
      return new vil_image_view<vxl_uint_16>(chunk,top_left, ni,nj,np, i_step,j_step,p_step);
    }
    else if (format == VIDL_PIXEL_FORMAT_MONO_1) {
      const bool* top_left = static_cast<const bool*>(cf->data()) + top_left_offset;
      return new vil_image_view<bool>(chunk,top_left, ni,nj,np, i_step,j_step,p_step);
    }
    else if (format == VIDL_PIXEL_FORMAT_MONO_F32 ||
             format == VIDL_PIXEL_FORMAT_RGB_F32 ||
             format == VIDL_PIXEL_FORMAT_RGB_F32P) {
      const vxl_ieee_32* top_left = static_cast<const vxl_ieee_32*>(cf->data()) + top_left_offset;
      return new vil_image_view<float>(chunk,top_left, ni,nj,np, i_step,j_step,p_step);
    }
    else {
      const vxl_byte* top_left = static_cast<const vxl_byte*>(cf->data()) + top_left_offset;
      return new vil_image_view<vxl_byte>(chunk,top_left, ni,nj,np, i_step,j_step,p_step);
    }
  }

  // Create a view of a frame (without ownership of the data)
  if (format == VIDL_PIXEL_FORMAT_MONO_16) {
    const vxl_uint_16* top_left = static_cast<const vxl_uint_16*>(frame.data()) + top_left_offset;
    return new vil_image_view<vxl_uint_16>(top_left, ni,nj,np, i_step,j_step,p_step);
  }
  else if (format == VIDL_PIXEL_FORMAT_MONO_1) {
    const bool* top_left = static_cast<const bool*>(frame.data()) + top_left_offset;
    return new vil_image_view<bool>(top_left, ni,nj,np, i_step,j_step,p_step);
  }
  const vxl_byte* top_left = static_cast<const vxl_byte*>(frame.data()) + top_left_offset;
  return new vil_image_view<vxl_byte>(top_left, ni,nj,np, i_step,j_step,p_step);
}
