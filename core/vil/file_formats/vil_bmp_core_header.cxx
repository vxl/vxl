// This is mul/vil2/file_formats/vil2_bmp_core_header.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm

#include "vil2_bmp_core_header.h"

#include <vcl_iostream.h>

#include <vil2/vil2_stream.h>
#include <vil2/vil2_stream_read.h>
#include <vil2/vil2_stream_write.h>

vil2_bmp_core_header::vil2_bmp_core_header()
{
  header_size = disk_size;
  width = 0;
  height = 0;
  planes = 1;
  bitsperpixel = 8;
}

void vil2_bmp_core_header::read(vil2_stream *s)
{
  header_size  = vil2_stream_read_little_endian_uint_32(s);
  width        = vil2_stream_read_little_endian_uint_32(s);
  height       = vil2_stream_read_little_endian_uint_32(s);
  planes       = vil2_stream_read_little_endian_uint_16(s);
  bitsperpixel = vil2_stream_read_little_endian_uint_16(s);
  // allowed values for bitsperpixel are 1 4 8 16 24 32; currently we only support 8 and 24
}

void vil2_bmp_core_header::write(vil2_stream *s) const
{
  vil2_stream_write_little_endian_uint_32(s, header_size);
  vil2_stream_write_little_endian_uint_32(s, width);
  vil2_stream_write_little_endian_uint_32(s, height);
  vil2_stream_write_little_endian_uint_16(s, planes);
  vil2_stream_write_little_endian_uint_16(s, bitsperpixel);
}

void vil2_bmp_core_header::print(vcl_ostream &s) const
{
  s << "vil2_bmp_core_header:\n"
    << "  header_size  : " << header_size  << vcl_endl
    << "  width        : " << width        << vcl_endl
    << "  height       : " << height       << vcl_endl
    << "  planes       : " << planes       << vcl_endl
    << "  bitsperpixel : " << bitsperpixel << vcl_endl << vcl_endl;
}
