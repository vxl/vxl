//:
// \file
// \author Tim Cootes

#include "vil2_io_memory_chunk.h"

#define write_case_macro(T)\
vsl_b_write(os,unsigned(chunk.size()/sizeof(T ))); \
vsl_b_write_block(os,(const T*) chunk.const_data(), \
                     chunk.size()/sizeof(T ))


//: Binary save vil2_memory_chunk to stream.
void vsl_b_write(vsl_b_ostream &os, const vil2_memory_chunk& chunk)
{
  const short io_version_no = 1;
  vsl_b_write(os, io_version_no);
  vsl_b_write(os, int(chunk.pixel_format()));

  switch (vil2_pixel_format_component_format(chunk.pixel_format()))
  {
    case VIL2_PIXEL_FORMAT_UINT_32:
      write_case_macro(vxl_uint_32);
      break;
    case VIL2_PIXEL_FORMAT_INT_32:
      write_case_macro(vxl_int_32);
      break;
    case VIL2_PIXEL_FORMAT_UINT_16:
      write_case_macro(vxl_uint_16);
      break;
    case VIL2_PIXEL_FORMAT_INT_16:
      write_case_macro(vxl_int_16);
      break;
    case VIL2_PIXEL_FORMAT_BYTE:
      write_case_macro(vxl_byte);
      break;
    case VIL2_PIXEL_FORMAT_SBYTE:
      write_case_macro(vxl_sbyte);
      break;
    case VIL2_PIXEL_FORMAT_FLOAT:
      write_case_macro(float);
      break;
    case VIL2_PIXEL_FORMAT_DOUBLE:
      write_case_macro(double);
      break;
    case VIL2_PIXEL_FORMAT_BOOL:
      write_case_macro(bool);
      break;
    default:
      vcl_cerr << "I/O ERROR: vsl_b_write(vsl_b_istream&, vil2_memory_chunk&) \n";
      vcl_cerr << "           Unknown component type  "<< "\n";
      return;
  }
}

#undef write_case_macro

#define read_case_macro(T)\
chunk.resize(n*sizeof(T ),pixel_format); \
vsl_b_read_block(is,(T *)chunk.data(),n)

//: Binary load vil2_memory_chunk from stream.
void vsl_b_read(vsl_b_istream &is, vil2_memory_chunk& chunk)
{
  if (!is) return;

  short w;
  vsl_b_read(is, w);
  int format;
  vil2_pixel_format pixel_format;
  unsigned n;
  switch(w)
  {
  case 1:
    vsl_b_read(is, format); pixel_format=vil2_pixel_format(format);
    vsl_b_read(is, n);
    switch (pixel_format)
    {
      case VIL2_PIXEL_FORMAT_UINT_32:
        read_case_macro(vxl_uint_32);
        break;
      case VIL2_PIXEL_FORMAT_INT_32:
        read_case_macro(vxl_int_32);
        break;
      case VIL2_PIXEL_FORMAT_UINT_16:
        read_case_macro(vxl_uint_16);
        break;
      case VIL2_PIXEL_FORMAT_INT_16:
        read_case_macro(vxl_int_16);
        break;
      case VIL2_PIXEL_FORMAT_BYTE:
        read_case_macro(vxl_byte);
        break;
      case VIL2_PIXEL_FORMAT_SBYTE:
        read_case_macro(vxl_sbyte);
        break;
      case VIL2_PIXEL_FORMAT_FLOAT:
        read_case_macro(float);
        break;
      case VIL2_PIXEL_FORMAT_DOUBLE:
        read_case_macro(double);
        break;
      case VIL2_PIXEL_FORMAT_BOOL:
        read_case_macro(bool);
        break;
      default:
        vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vil2_memory_chunk&) \n";
        vcl_cerr << "           Unknown pixel format "<< format << "\n";
        is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
        return;
    }
    break;

  default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vil2_memory_chunk&) \n";
    vcl_cerr << "           Unknown version number "<< w << "\n";
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

#undef read_case_macro

//: Binary save vil2_memory_chunk to stream  by pointer
void vsl_b_write(vsl_b_ostream &os, const vil2_memory_chunk* chunk_ptr)
{
  bool not_null_ptr = (chunk_ptr!=0);
  vsl_b_write(os,not_null_ptr);
  if (not_null_ptr)
    vsl_b_write(os,*chunk_ptr);
}

//: Binary load vil2_memory_chunk from stream  onto the heap
void vsl_b_read(vsl_b_istream &is, vil2_memory_chunk*& p)
{
  delete p;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr)
  {
    p = new vil2_memory_chunk();
    vsl_b_read(is, *p);
  }
  else
    p = 0;
}

//: Print human readable summary of a vil2_memory_chunk object to a stream
void vsl_print_summary(vcl_ostream& os,const vil2_memory_chunk& chunk)
{
  os<<"vil2_memory_chunk containing "<<chunk.size()<<" bytes of "<<chunk.pixel_format();
}
