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
#include <vcl_cstring.h>

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
template <vidl2_pixel_format in_Fmt, vidl2_pixel_format out_Fmt>
struct convert_generic
{
  static inline bool apply(const vidl2_frame& in_frame,
                           vidl2_frame& out_frame)
  {
    assert(in_frame.pixel_format()==in_Fmt);
    assert(out_frame.pixel_format()==out_Fmt);
    typedef typename vidl2_pixel_traits_of<in_Fmt>::type in_type;
    typedef typename vidl2_pixel_traits_of<out_Fmt>::type out_type;
    typedef vidl2_color_converter<
        vidl2_pixel_color(vidl2_pixel_traits_of<in_Fmt>::color_idx),
        vidl2_pixel_traits_of<in_Fmt>::num_channels,
        vidl2_pixel_color(vidl2_pixel_traits_of<out_Fmt>::color_idx),
        vidl2_pixel_traits_of<out_Fmt>::num_channels > color_converter;
    vidl2_pixel_iterator<in_Fmt> in_itr(in_frame);
    vidl2_pixel_iterator<out_Fmt> out_itr(out_frame);

    const unsigned int num_pix = in_frame.ni() * in_frame.nj();
    in_type in_pixel[vidl2_pixel_traits_of<in_Fmt>::num_channels];
    out_type out_pixel[vidl2_pixel_traits_of<out_Fmt>::num_channels];
    for(unsigned int c=0; c<num_pix; ++c, ++in_itr, ++out_itr){
      in_itr.get(in_pixel);
      color_converter::convert(in_pixel, out_pixel);
      out_itr.set(out_pixel);
    }
    return true;
  }
};



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
    for(unsigned int c=0; c<num_half_pix; ++c){
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
    for(unsigned int c=0; c<num_half_pix; ++c){
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
    if(in_Fmt == out_Fmt)
      table_entry = &copy_conversion;
    else if(convert<in_Fmt,out_Fmt>::defined)
      table_entry = &convert<in_Fmt,out_Fmt>::apply;
    else if(convert<in_Fmt,VIDL2_PIXEL_FORMAT_RGB_24>::defined &&
            convert<VIDL2_PIXEL_FORMAT_RGB_24,out_Fmt>::defined)
      table_entry = &intermediate_rgb24_conversion;
    else if(vidl2_pixel_iterator_valid<in_Fmt>::value &&
            vidl2_pixel_iterator_valid<out_Fmt>::value)
      table_entry = &convert_generic<in_Fmt,out_Fmt>::apply;
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
// defined here because it uses conversion_table
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
// If the output frame data is not the correct size new memory
// will be allocated
bool vidl2_convert_frame(const vidl2_frame& in_frame,
                               vidl2_frame& out_frame)
{
  vidl2_pixel_format in_fmt = in_frame.pixel_format();
  vidl2_pixel_format out_fmt = out_frame.pixel_format();

  if(in_fmt == VIDL2_PIXEL_FORMAT_UNKNOWN ||
     out_fmt == VIDL2_PIXEL_FORMAT_UNKNOWN)
    return false;

  unsigned ni = in_frame.ni();
  unsigned nj = in_frame.nj();
  unsigned out_size = vidl2_pixel_format_buffer_size(ni,nj,out_fmt);

  if(out_frame.size() != out_size ||
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
  if(format == VIDL2_PIXEL_FORMAT_UNKNOWN)
    return NULL;

  unsigned ni = in_frame->ni();
  unsigned nj = in_frame->nj();
  unsigned size = vidl2_pixel_format_buffer_size(ni,nj,format);
  vil_memory_chunk_sptr memory = new vil_memory_chunk(size, VIL_PIXEL_FORMAT_BYTE);
  vidl2_frame_sptr out_frame = new vidl2_memory_chunk_frame(ni, nj, format, memory);

  if(vidl2_convert_frame(*in_frame, *out_frame))
    return out_frame;

  return NULL;
}
