// This is core/vil/file_formats/vil_bmp_core_header.cxx
//:
// \file
// \author fsm

#include <iostream>
#include "vil_bmp_core_header.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vil/vil_stream.h>
#include <vil/vil_stream_read.h>
#include <vil/vil_stream_write.h>

vil_bmp_core_header::vil_bmp_core_header()
{
  header_size = disk_size;
  width = 0;
  height = 0;
  planes = 1;
  bitsperpixel = 8;
}

void vil_bmp_core_header::read(vil_stream *s)
{
  header_size  = vil_stream_read_little_endian_uint_32(s);
  width        = vil_stream_read_little_endian_int_32(s);
  height       = vil_stream_read_little_endian_int_32(s);
  planes       = vil_stream_read_little_endian_uint_16(s);
  bitsperpixel = vil_stream_read_little_endian_uint_16(s);
  // allowed values for bitsperpixel are 1 4 8 16 24 32; currently we only support 8 and 24
}

void vil_bmp_core_header::write(vil_stream *s) const
{
  vil_stream_write_little_endian_uint_32(s, header_size);
  vil_stream_write_little_endian_int_32(s, width);
  vil_stream_write_little_endian_int_32(s, height);
  vil_stream_write_little_endian_uint_16(s, planes);
  vil_stream_write_little_endian_uint_16(s, bitsperpixel);
}

void vil_bmp_core_header::print(std::ostream &s) const
{
  s << "vil_bmp_core_header:\n"
    << "  header_size  : " << header_size  << std::endl
    << "  width        : " << width        << std::endl
    << "  height       : " << height       << std::endl
    << "  planes       : " << planes       << std::endl
    << "  bitsperpixel : " << bitsperpixel << std::endl << std::endl;
}
