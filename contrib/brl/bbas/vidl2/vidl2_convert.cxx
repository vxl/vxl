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


// Default pixel format conversion - it fails
template <vidl2_pixel_format in_Fmt, vidl2_pixel_format out_Fmt>
struct convert
{
  enum { defined = false };
  static inline bool apply(const vidl2_frame& in_frame,
                           vidl2_frame& out_frame)
  {
    return false;
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
    // FIXME this is not actually implemented yet
    // just an example of how to do the specialization
    return false;
  }
};


// End of pixel conversion specializations
//=============================================================================

//: Generates an entry into the table of pixel format conversions functions
template <vidl2_pixel_format in_Fmt, vidl2_pixel_format out_Fmt>
struct table_entry_init
{
  static inline void set_entry(converter_func& table_entry)
  {
    // This should be done at compile time with partial specialization
    if(in_Fmt == out_Fmt)
      table_entry = &copy_conversion;
    else if(convert<in_Fmt,out_Fmt>::defined)
      table_entry = &convert<in_Fmt,out_Fmt>::apply;
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
