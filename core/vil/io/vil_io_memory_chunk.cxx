// This is core/vil/io/vil_io_memory_chunk.cxx
#include "vil_io_memory_chunk.h"
//:
// \file
// \author Tim Cootes
// \verbatim
//  Modifications
//   Feb.2003 - Ian Scott - Upgraded IO to use vsl_block_binary io
//   23 Oct.2003 - Peter Vanroose - Added support for 64-bit int pixels
// \endvarbatim

#include <vsl/vsl_block_binary.h>
#include <vsl/vsl_complex_io.h>

#define write_case_macro(T)\
vsl_b_write(os,unsigned(chunk.size()/sizeof(T ))); \
vsl_block_binary_write(os,(const T*) chunk.const_data(),chunk.size()/sizeof(T))


//: Binary save vil_memory_chunk to stream.
void vsl_b_write(vsl_b_ostream &os, const vil_memory_chunk& chunk)
{
  const short io_version_no = 2;
  vsl_b_write(os, io_version_no);
  vsl_b_write(os, int(chunk.pixel_format()));

  switch (vil_pixel_format_component_format(chunk.pixel_format()))
  {
#if VXL_HAS_INT_64
    case VIL_PIXEL_FORMAT_UINT_64:
      write_case_macro(vxl_uint_64);
      break;
    case VIL_PIXEL_FORMAT_INT_64:
      write_case_macro(vxl_int_64);
      break;
#endif
    case VIL_PIXEL_FORMAT_UINT_32:
      write_case_macro(vxl_uint_32);
      break;
    case VIL_PIXEL_FORMAT_INT_32:
      write_case_macro(vxl_int_32);
      break;
    case VIL_PIXEL_FORMAT_UINT_16:
      write_case_macro(vxl_uint_16);
      break;
    case VIL_PIXEL_FORMAT_INT_16:
      write_case_macro(vxl_int_16);
      break;
    case VIL_PIXEL_FORMAT_BYTE:
      write_case_macro(vxl_byte);
      break;
    case VIL_PIXEL_FORMAT_SBYTE:
      write_case_macro(vxl_sbyte);
      break;
    case VIL_PIXEL_FORMAT_FLOAT:
      write_case_macro(float);
      break;
    case VIL_PIXEL_FORMAT_DOUBLE:
      write_case_macro(double);
      break;
    case VIL_PIXEL_FORMAT_BOOL:
      write_case_macro(bool);
      break;
    case VIL_PIXEL_FORMAT_COMPLEX_FLOAT:
      write_case_macro(vcl_complex<float>);
      break;
    case VIL_PIXEL_FORMAT_COMPLEX_DOUBLE:
      write_case_macro(vcl_complex<double>);
      break;
    default:
      vcl_cerr << "I/O ERROR: vsl_b_write(vsl_b_istream&, vil_memory_chunk&)\n"
               << "           Unknown component type\n";
      return;
  }
}

#undef write_case_macro


// This file never uses the fast versions of vsl_b_read_block, so just locally
//implement the old slow version.
#define read_case_macro_v1(T)\
chunk.set_size(n*sizeof(T ),pixel_format); \
  for (unsigned i=0; i<n; ++i)\
    vsl_b_read(is, static_cast<T *>(chunk.data())[i]);

#define read_case_macro_v2(T)\
chunk.set_size(n*sizeof(T ),pixel_format); \
vsl_block_binary_read(is,static_cast<T *>(chunk.data()),n)

//: Binary load vil_memory_chunk from stream.
void vsl_b_read(vsl_b_istream &is, vil_memory_chunk& chunk)
{
  if (!is) return;

  short w;
  vsl_b_read(is, w);
  int format;
  vil_pixel_format pixel_format;
  unsigned n;
  switch(w)
  {
  case 1:
    vsl_b_read(is, format); pixel_format=vil_pixel_format(format);
    vsl_b_read(is, n);
    switch (pixel_format)
    {
#if VXL_HAS_INT_64
      case VIL_PIXEL_FORMAT_UINT_64:
        read_case_macro_v1(vxl_uint_64);
        break;
      case VIL_PIXEL_FORMAT_INT_64:
        read_case_macro_v1(vxl_int_64);
        break;
#endif
      case VIL_PIXEL_FORMAT_UINT_32:
        read_case_macro_v1(vxl_uint_32);
        break;
      case VIL_PIXEL_FORMAT_INT_32:
        read_case_macro_v1(vxl_int_32);
        break;
      case VIL_PIXEL_FORMAT_UINT_16:
        read_case_macro_v1(vxl_uint_16);
        break;
      case VIL_PIXEL_FORMAT_INT_16:
        read_case_macro_v1(vxl_int_16);
        break;
      case VIL_PIXEL_FORMAT_BYTE:
        read_case_macro_v1(vxl_byte);
        break;
      case VIL_PIXEL_FORMAT_SBYTE:
        read_case_macro_v1(vxl_sbyte);
        break;
      case VIL_PIXEL_FORMAT_FLOAT:
        read_case_macro_v1(float);
        break;
      case VIL_PIXEL_FORMAT_DOUBLE:
        read_case_macro_v1(double);
        break;
      case VIL_PIXEL_FORMAT_BOOL:
        read_case_macro_v1(bool);
        break;
      // No version 1 complex images were ever written.
      default:
        vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vil_memory_chunk&)\n"
                 << "           Unknown pixel format "<< format << '\n';
        is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
        return;
    }
    break;

  case 2:
    vsl_b_read(is, format); pixel_format=vil_pixel_format(format);
    vsl_b_read(is, n);
    switch (pixel_format)
    {
#if VXL_HAS_INT_64
      case VIL_PIXEL_FORMAT_UINT_64:
        read_case_macro_v2(vxl_uint_64);
        break;
      case VIL_PIXEL_FORMAT_INT_64:
        read_case_macro_v2(vxl_int_64);
        break;
#endif
      case VIL_PIXEL_FORMAT_UINT_32:
        read_case_macro_v2(vxl_uint_32);
        break;
      case VIL_PIXEL_FORMAT_INT_32:
        read_case_macro_v2(vxl_int_32);
        break;
      case VIL_PIXEL_FORMAT_UINT_16:
        read_case_macro_v2(vxl_uint_16);
        break;
      case VIL_PIXEL_FORMAT_INT_16:
        read_case_macro_v2(vxl_int_16);
        break;
      case VIL_PIXEL_FORMAT_BYTE:
        read_case_macro_v2(vxl_byte);
        break;
      case VIL_PIXEL_FORMAT_SBYTE:
        read_case_macro_v2(vxl_sbyte);
        break;
      case VIL_PIXEL_FORMAT_FLOAT:
        read_case_macro_v2(float);
        break;
      case VIL_PIXEL_FORMAT_DOUBLE:
        read_case_macro_v2(double);
        break;
      case VIL_PIXEL_FORMAT_BOOL:
        read_case_macro_v2(bool);
        break;
      case VIL_PIXEL_FORMAT_COMPLEX_FLOAT:
        read_case_macro_v2(vcl_complex<float>);
        break;
      case VIL_PIXEL_FORMAT_COMPLEX_DOUBLE:
        read_case_macro_v2(vcl_complex<double>);
        break;
      default:
        vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vil_memory_chunk&)\n"
                 << "           Unknown pixel format "<< format << '\n';
        is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
        return;
    }
    break;

  default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vil_memory_chunk&)\n"
             << "           Unknown version number "<< w << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

#undef read_case_macro

//: Binary save vil_memory_chunk to stream  by pointer
void vsl_b_write(vsl_b_ostream &os, const vil_memory_chunk* chunk_ptr)
{
  bool not_null_ptr = (chunk_ptr!=0);
  vsl_b_write(os,not_null_ptr);
  if (not_null_ptr)
    vsl_b_write(os,*chunk_ptr);
}

//: Binary load vil_memory_chunk from stream  onto the heap
void vsl_b_read(vsl_b_istream &is, vil_memory_chunk*& p)
{
  delete p;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr)
  {
    p = new vil_memory_chunk();
    vsl_b_read(is, *p);
  }
  else
    p = 0;
}

//: Print human readable summary of a vil_memory_chunk object to a stream
void vsl_print_summary(vcl_ostream& os,const vil_memory_chunk& chunk)
{
  os<<"vil_memory_chunk containing "<<chunk.size()<<" bytes of "<<chunk.pixel_format();
}
