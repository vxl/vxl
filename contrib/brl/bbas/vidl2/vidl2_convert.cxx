// This is brl/bbas/vidl2/vidl2_convert.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Matt Leotta
// \date   20 Jan 2006
//
//-----------------------------------------------------------------------------

#include "vidl2_convert.h"
#include "vidl2_frame.h"
#include "vidl2_pixel_format.h"
#include "vidl2_pixel_iterator.txx"
#include "vidl2_color.h"
#include <vil/vil_convert.h>
#include <vil/vil_memory_chunk.h>
#include <vcl_cstring.h>
#include <vcl_cassert.h>
#include <vcl_memory.h>

//--------------------------------------------------------------------------------


namespace {

//: Define the function pointer for pixel format conversion functions
typedef bool (*converter_func)(const vidl2_frame& in_frame, vidl2_frame& out_frame);


//: Default pixel format conversion - it fails
bool default_conversion(const vidl2_frame& in_frame, vidl2_frame& out_frame)
{
  vcl_cerr << "No routine to convert " << in_frame.pixel_format()
           << " to " << out_frame.pixel_format() << vcl_endl;
  return false;
}


//: Use memcpy when the formats are the same
bool copy_conversion(const vidl2_frame& in_frame, vidl2_frame& out_frame)
{
  assert(in_frame.pixel_format() == out_frame.pixel_format());
  assert(in_frame.size() == out_frame.size());
  vcl_memcpy(out_frame.data(), in_frame.data(), in_frame.size());
  return true;
}


//: Convert to an intermediate RGB_24 frame
// This is inefficient, but will provide the functionality until
// an optimized version is written
// defined later because it uses conversion_table
bool intermediate_rgb24_conversion(const vidl2_frame& in_frame, vidl2_frame& out_frame);


// Default pixel format conversion - it fails
template <vidl2_pixel_format in_Fmt, vidl2_pixel_format out_Fmt>
struct convert
{
  enum { defined = false };
  static inline bool apply(const vidl2_frame& /*in_frame*/,
                           vidl2_frame& /*out_frame*/)
  {
    return false;
  }
};

//=============================================================================
// Start of generic pixel conversions


//: The generic pixel conversion function
bool convert_generic(const vidl2_frame& in_frame,
                     vidl2_frame& out_frame)
{
  // create pixel iterators for each frame
  vcl_auto_ptr<vidl2_pixel_iterator> in_pitr(vidl2_make_pixel_iterator(in_frame));
  if(!in_pitr.get())
    return false;
  vcl_auto_ptr<vidl2_pixel_iterator> out_pitr(vidl2_make_pixel_iterator(out_frame));
  if(!out_pitr.get())
    return false;

  vidl2_pixel_iterator& in_itr = *in_pitr;
  vidl2_pixel_iterator& out_itr = *out_pitr;

  vidl2_pixel_traits in_t = vidl2_pixel_format_traits(in_frame.pixel_format());
  vidl2_pixel_traits out_t = vidl2_pixel_format_traits(out_frame.pixel_format());

  // find the color conversion function
  vidl2_color_conv_fptr color_conv =
      vidl2_color_converter_func( in_t.color,  in_t.bits_per_pixel,
                                  out_t.color, out_t.bits_per_pixel);
  if(!color_conv)
    return false;

  const unsigned int num_pix = in_frame.ni() * in_frame.nj();
  vxl_byte in_pixel[4], out_pixel[4]; // assume pixels are no more than 4 bytes
  for (unsigned int c=0; c<num_pix; ++c, ++in_itr, ++out_itr){
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
VCL_DEFINE_SPECIALIZATION
struct convert<VIDL2_PIXEL_FORMAT_RGB_24, VIDL2_PIXEL_FORMAT_UYVY_422>
{
  enum { defined = true };
  static inline bool apply(const vidl2_frame& in_frame,
                           vidl2_frame& out_frame)
  {
    assert(in_frame.pixel_format()==VIDL2_PIXEL_FORMAT_RGB_24);
    assert(out_frame.pixel_format()==VIDL2_PIXEL_FORMAT_UYVY_422);
    const vxl_byte* rgb = reinterpret_cast<const vxl_byte*>(in_frame.data());
    vxl_byte* uyvy = reinterpret_cast<vxl_byte*>(out_frame.data());
    unsigned int num_half_pix = (in_frame.ni() * in_frame.nj() + 1)/2;
    for (unsigned int c=0; c<num_half_pix; ++c){
      const vxl_byte& r1 = *(rgb++);
      const vxl_byte& g1 = *(rgb++);
      const vxl_byte& b1 = *(rgb++);
      const vxl_byte& r2 = *(rgb++);
      const vxl_byte& g2 = *(rgb++);
      const vxl_byte& b2 = *(rgb++);
      vxl_byte y1,u1,v1,y2,u2,v2;
      vidl2_color_convert_rgb2yuv(r1,g1,b1,y1,u1,v1);
      vidl2_color_convert_rgb2yuv(r2,g2,b2,y2,u2,v2);
      *(uyvy++) = (u1+u2)/2;
      *(uyvy++) = y1;
      *(uyvy++) = (v1+v2)/2;
      *(uyvy++) = y2;
    }
    return true;
  }
};


// UYVY_422 to RGB_24
VCL_DEFINE_SPECIALIZATION
struct convert<VIDL2_PIXEL_FORMAT_UYVY_422, VIDL2_PIXEL_FORMAT_RGB_24>
{
  enum { defined = true };
  static inline bool apply(const vidl2_frame& in_frame,
                           vidl2_frame& out_frame)
  {
    assert(in_frame.pixel_format()==VIDL2_PIXEL_FORMAT_UYVY_422);
    assert(out_frame.pixel_format()==VIDL2_PIXEL_FORMAT_RGB_24);
    const vxl_byte* uyvy = reinterpret_cast<const vxl_byte*>(in_frame.data());
    vxl_byte* rgb = reinterpret_cast<vxl_byte*>(out_frame.data());
    unsigned int num_half_pix = (in_frame.ni() * in_frame.nj() + 1)/2;
    for (unsigned int c=0; c<num_half_pix; ++c){
      const vxl_byte& u1 = *(uyvy++);
      const vxl_byte& y1 = *(uyvy++);
      const vxl_byte& v1 = *(uyvy++);
      const vxl_byte& y2 = *(uyvy++);
      vxl_byte r,g,b;
      vidl2_color_convert_yuv2rgb(y1,u1,v1,r,g,b);
      *(rgb++) = r;
      *(rgb++) = g;
      *(rgb++) = b;
      vidl2_color_convert_yuv2rgb(y2,u1,v1,r,g,b);
      *(rgb++) = r;
      *(rgb++) = g;
      *(rgb++) = b;
    }
    return true;
  }
};


// UYVY_422 to MONO_8
VCL_DEFINE_SPECIALIZATION
struct convert<VIDL2_PIXEL_FORMAT_UYVY_422, VIDL2_PIXEL_FORMAT_MONO_8>
{
  enum { defined = true };
  static inline bool apply(const vidl2_frame& in_frame,
                           vidl2_frame& out_frame)
  {
    assert(in_frame.pixel_format()==VIDL2_PIXEL_FORMAT_UYVY_422);
    assert(out_frame.pixel_format()==VIDL2_PIXEL_FORMAT_MONO_8);
    const vxl_byte* uyvy = reinterpret_cast<const vxl_byte*>(in_frame.data());
    vxl_byte* mono = reinterpret_cast<vxl_byte*>(out_frame.data());
    unsigned int num_half_pix = (in_frame.ni() * in_frame.nj() + 1)/2;
    for (unsigned int c=0; c<num_half_pix; ++c){
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



// End of pixel conversion specializations
//=============================================================================


//: Generates an entry into the table of pixel format conversions functions
// This is called for every pair for pixel formats to build the table
template <vidl2_pixel_format in_Fmt, vidl2_pixel_format out_Fmt>
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
    else if (vidl2_pixel_iterator_valid<in_Fmt>::value &&
            vidl2_pixel_iterator_valid<out_Fmt>::value)
      table_entry = &convert_generic;
    else
      table_entry = &default_conversion;
  }
};

//: Recursive template metaprogram to generate conditionals for converting each pair of pixel types
template <int Fmt_Code>
struct table_init
{
  static inline void populate(converter_func table[VIDL2_PIXEL_FORMAT_ENUM_END][VIDL2_PIXEL_FORMAT_ENUM_END])
  {
    const vidl2_pixel_format in_fmt = vidl2_pixel_format(Fmt_Code/VIDL2_PIXEL_FORMAT_ENUM_END);
    const vidl2_pixel_format out_fmt = vidl2_pixel_format(Fmt_Code%VIDL2_PIXEL_FORMAT_ENUM_END);
    table_entry_init<in_fmt,out_fmt>::set_entry(table[in_fmt][out_fmt]);
    table_init<Fmt_Code-1>::populate(table);
  }
};


//: The base case
VCL_DEFINE_SPECIALIZATION
struct table_init<0>
{
  static inline void populate(converter_func table[VIDL2_PIXEL_FORMAT_ENUM_END][VIDL2_PIXEL_FORMAT_ENUM_END])
  {
    const vidl2_pixel_format in_fmt = vidl2_pixel_format(0);
    const vidl2_pixel_format out_fmt = vidl2_pixel_format(0);
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
    table_init<VIDL2_PIXEL_FORMAT_ENUM_END*VIDL2_PIXEL_FORMAT_ENUM_END-1>::populate(table);
  }

  //: Apply the conversion
  bool operator()(const vidl2_frame& in_frame, vidl2_frame& out_frame) const
  {
    return (*table[in_frame.pixel_format()][out_frame.pixel_format()])(in_frame, out_frame);
  }
 private:
  //: Table of conversion functions
  converter_func table[VIDL2_PIXEL_FORMAT_ENUM_END][VIDL2_PIXEL_FORMAT_ENUM_END];
};

//: Instantiate a global conversion function table
converter conversion_table;

//: Convert to an intermediate RGB_24 frame
// Defined here because it uses conversion_table
bool intermediate_rgb24_conversion(const vidl2_frame& in_frame, vidl2_frame& out_frame)
{
  unsigned int ni = in_frame.ni(), nj = in_frame.nj();
  vil_memory_chunk_sptr memory = new vil_memory_chunk(ni*nj*3, VIL_PIXEL_FORMAT_BYTE);
  vidl2_memory_chunk_frame temp_frame(ni,nj,VIDL2_PIXEL_FORMAT_RGB_24,memory);
  return conversion_table(in_frame, temp_frame) &&
      conversion_table(temp_frame, out_frame);
}

} // end anonymous namespace

//--------------------------------------------------------------------------------

//: Convert the pixel format of a frame
//
// The \p in_frame->data() is converted from \p in_frame->pixel_format()
// to \p out_frame->pixel_format() and stored in \p out_frame->data()
// \returns false if the output frame data is not the correct size.
bool vidl2_convert_frame(const vidl2_frame& in_frame,
                               vidl2_frame& out_frame)
{
  vidl2_pixel_format in_fmt = in_frame.pixel_format();
  vidl2_pixel_format out_fmt = out_frame.pixel_format();

  if (in_fmt == VIDL2_PIXEL_FORMAT_UNKNOWN ||
     out_fmt == VIDL2_PIXEL_FORMAT_UNKNOWN)
    return false;

  unsigned ni = in_frame.ni();
  unsigned nj = in_frame.nj();
  unsigned out_size = vidl2_pixel_format_buffer_size(ni,nj,out_fmt);

  if (out_frame.size() != out_size ||
     out_frame.ni() != ni ||
     out_frame.nj() != nj ||
     !out_frame.data() )
    return false;

  // call the appropriate function in the conversion table
  return conversion_table(in_frame, out_frame);
}


//: Convert the pixel format of a frame
//
// The convert \p in_frame to a \p format by allocating
// a new frame buffer
vidl2_frame_sptr vidl2_convert_frame(const vidl2_frame_sptr& in_frame,
                                     vidl2_pixel_format format)
{
  if (format == VIDL2_PIXEL_FORMAT_UNKNOWN)
    return NULL;

  unsigned ni = in_frame->ni();
  unsigned nj = in_frame->nj();
  unsigned size = vidl2_pixel_format_buffer_size(ni,nj,format);
  vil_memory_chunk_sptr memory = new vil_memory_chunk(size, VIL_PIXEL_FORMAT_BYTE);
  vidl2_frame_sptr out_frame = new vidl2_memory_chunk_frame(ni, nj, format, memory);

  if (vidl2_convert_frame(*in_frame, *out_frame))
    return out_frame;

  return NULL;
}


//: Convert the image view to a frame
// Will wrap the memory if possible, if not the image is converted to
// the closest vidl2_pixel_format
vidl2_frame_sptr vidl2_convert_to_frame(const vil_image_view_base_sptr& image)
{
  if(!image)
    return NULL;

  // try to wrap the image memory in a frame
  vidl2_frame_sptr frame = new vidl2_memory_chunk_frame(*image);
  if(frame->pixel_format() != VIDL2_PIXEL_FORMAT_UNKNOWN)
    return frame;

  // if the image could not be wrapped convert it
  unsigned ni = image->ni(), nj = image->nj(), np = image->nplanes();

  // special case for 16 bit images
  if(image->pixel_format() == VIL_PIXEL_FORMAT_UINT_16)
  {
    if(np != 1)
      return NULL;
    vil_image_view<vxl_uint_16> img(ni,nj);
    img.deep_copy(vil_image_view<vxl_uint_16>(*image));
    return new vidl2_memory_chunk_frame(ni, nj, VIDL2_PIXEL_FORMAT_MONO_16,
                                        img.memory_chunk());
  }

  vidl2_pixel_format format = VIDL2_PIXEL_FORMAT_UNKNOWN;
  if(np == 1)
    format = VIDL2_PIXEL_FORMAT_MONO_8;
  else if(np == 3)
    format = VIDL2_PIXEL_FORMAT_RGB_24P;
  else if(np == 4)
    format = VIDL2_PIXEL_FORMAT_RGBA_32P;
  else
    return NULL;

  vil_image_view<vxl_byte> img;
  if(image->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
    img.deep_copy(vil_image_view<vxl_byte>(*image));
  else
  {
    vil_image_view_base_sptr bimage = vil_convert_cast(vxl_byte(),image);
    if(!bimage)
      return NULL;
    img = *bimage;
  }
  return new vidl2_memory_chunk_frame(ni, nj, format,
                                      img.memory_chunk());

}


//: convert the frame into an image view
// possibly converts the pixel data type
// always create a deep copy of the data
bool vidl2_convert_to_view(const vidl2_frame& frame,
                           vil_image_view_base& image,
                           vidl2_pixel_color require_color)
{
  if (frame.pixel_format() == VIDL2_PIXEL_FORMAT_UNKNOWN ||
      frame.data() == NULL)
    return false;

  vidl2_pixel_color in_color = vidl2_pixel_format_color(frame.pixel_format());
  if(require_color == VIDL2_PIXEL_COLOR_UNKNOWN)
    require_color = in_color;

  unsigned ni = frame.ni(), nj = frame.nj();
  unsigned np = vidl2_pixel_color_num_channels(require_color);

  // resize the image if necessary
  image.set_size(ni,nj,np);

  // special case for MONO_16
  if(frame.pixel_format() == VIDL2_PIXEL_FORMAT_MONO_16){
    vil_image_view<vxl_uint_16> wrapper(static_cast<const vxl_uint_16*>(frame.data()),
                                        ni,nj,1,1,ni,ni*nj);
    if(image.pixel_format() == VIL_PIXEL_FORMAT_UINT_16){
      vil_image_view<vxl_uint_16>& img = static_cast<vil_image_view<vxl_uint_16>&>(image);
      img.deep_copy(vil_image_view<vxl_uint_16>(wrapper));
      return true;
    }

    switch ( vil_pixel_format_component_format(image.pixel_format()) ){
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



  vidl2_pixel_format default_format = VIDL2_PIXEL_FORMAT_UNKNOWN;
  if(image.pixel_format() == VIL_PIXEL_FORMAT_BYTE)
  {
    vil_image_view<vxl_byte>& img = static_cast<vil_image_view<vxl_byte>&>(image);
    bool interleaved = (img.planestep() == 1);

    switch(require_color){
      case VIDL2_PIXEL_COLOR_MONO:
        default_format = VIDL2_PIXEL_FORMAT_MONO_8; break;
      case VIDL2_PIXEL_COLOR_RGB:
        default_format = interleaved?VIDL2_PIXEL_FORMAT_RGB_24
                                    :VIDL2_PIXEL_FORMAT_RGB_24P; break;
      case VIDL2_PIXEL_COLOR_RGBA:
        default_format = interleaved?VIDL2_PIXEL_FORMAT_RGBA_32
                                    :VIDL2_PIXEL_FORMAT_RGBA_32P; break;
      case VIDL2_PIXEL_COLOR_YUV:
        default_format = interleaved?VIDL2_PIXEL_FORMAT_UYV_444
                                    :VIDL2_PIXEL_FORMAT_YUV_444P; break;
      default:
        break;
    }
  }


  vidl2_frame_sptr out_frame = new vidl2_memory_chunk_frame(image,default_format);
  // if the image can be wrapped as a frame
  if(out_frame->pixel_format() != VIDL2_PIXEL_FORMAT_UNKNOWN){
    vidl2_convert_frame(frame, *out_frame);
    return true;
  }

  // use an intermediate format
  vidl2_pixel_format out_fmt;
  switch(in_color){
    case VIDL2_PIXEL_COLOR_MONO:
      out_fmt = VIDL2_PIXEL_FORMAT_MONO_8; break;
    case VIDL2_PIXEL_COLOR_RGB:
      out_fmt = VIDL2_PIXEL_FORMAT_RGB_24P; break;
    case VIDL2_PIXEL_COLOR_RGBA:
      out_fmt = VIDL2_PIXEL_FORMAT_RGBA_32P; break;
    case VIDL2_PIXEL_COLOR_YUV:
      out_fmt = VIDL2_PIXEL_FORMAT_YUV_444P; break;
    default:
      return false;
  }

  vil_image_view<vxl_byte> temp(ni,nj,np);
  out_frame = new vidl2_memory_chunk_frame(ni,nj,out_fmt,temp.memory_chunk());
  vidl2_convert_frame(frame, *out_frame);

  switch ( vil_pixel_format_component_format(image.pixel_format()) ){
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
vidl2_convert_wrap_in_view(const vidl2_frame& frame)
{
  vidl2_pixel_format format = frame.pixel_format();
  vidl2_pixel_traits pt =  vidl2_pixel_format_traits(format);
  if( pt.chroma_shift_x != 0 || pt.chroma_shift_y != 0 ||
      pt.bits_per_pixel % pt.num_channels != 0)
    return NULL;

  unsigned ni = frame.ni(), nj = frame.nj();
  unsigned np = pt.num_channels;
  vcl_ptrdiff_t i_step, j_step, p_step;
  switch(pt.arrangement){
    case VIDL2_PIXEL_ARRANGE_SINGLE:
      i_step = np;
      j_step = np*ni;
      p_step = 1;
      break;
    case VIDL2_PIXEL_ARRANGE_PLANAR:
      i_step = 1;
      j_step = ni;
      p_step = ni*nj;
      break;
    default:
      // Cannot wrap other pixel arrangements
      return NULL;
  }
  vcl_ptrdiff_t top_left_offset = 0;

  if(format == VIDL2_PIXEL_FORMAT_BGR_24){
    top_left_offset = 3;
    p_step = -1;
  }

  // Create a view of a memory chunk frame
  if( const vidl2_memory_chunk_frame* cf =
      dynamic_cast<const vidl2_memory_chunk_frame*>(&frame) )
  {
    vil_memory_chunk_sptr chunk = cf->memory_chunk();
    if(format == VIDL2_PIXEL_FORMAT_MONO_16){
      const vxl_uint_16* top_left = static_cast<const vxl_uint_16*>(cf->data()) + top_left_offset;
      return new vil_image_view<vxl_uint_16>(chunk,top_left, ni,nj,np, i_step,j_step,p_step);
    }
    else if(format == VIDL2_PIXEL_FORMAT_MONO_1){
      const bool* top_left = static_cast<const bool*>(cf->data()) + top_left_offset;
      return new vil_image_view<bool>(chunk,top_left, ni,nj,np, i_step,j_step,p_step);
    }
    const vxl_byte* top_left = static_cast<const vxl_byte*>(cf->data()) + top_left_offset;
    return new vil_image_view<vxl_byte>(chunk,top_left, ni,nj,np, i_step,j_step,p_step);
  }

  // Create a view of a frame (without ownership of the data)
  if(format == VIDL2_PIXEL_FORMAT_MONO_16){
    const vxl_uint_16* top_left = static_cast<const vxl_uint_16*>(frame.data()) + top_left_offset;
    return new vil_image_view<vxl_uint_16>(top_left, ni,nj,np, i_step,j_step,p_step);
  }
  else if(format == VIDL2_PIXEL_FORMAT_MONO_1){
    const bool* top_left = static_cast<const bool*>(frame.data()) + top_left_offset;
    return new vil_image_view<bool>(top_left, ni,nj,np, i_step,j_step,p_step);
  }
  const vxl_byte* top_left = static_cast<const vxl_byte*>(frame.data()) + top_left_offset;
  return new vil_image_view<vxl_byte>(top_left, ni,nj,np, i_step,j_step,p_step);
}
