// This is core/vil1/file_formats/vil1_bmp_core_header.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm

#include "vil1_bmp_core_header.h"

#include <vcl_iostream.h>

#include <vil1/vil1_stream.h>
#include <vil1/vil1_16bit.h>
#include <vil1/vil1_32bit.h>

vil1_bmp_core_header::vil1_bmp_core_header()
{
  header_size = disk_size;
  width = 0;
  height = 0;
  planes = 1;
  bitsperpixel = 8;
}

void vil1_bmp_core_header::read(vil1_stream *s)
{
  header_size  = vil1_32bit_read_little_endian(s);
  width        = vil1_32bit_read_little_endian(s);
  height       = vil1_32bit_read_little_endian(s);
  planes       = vil1_16bit_read_little_endian(s);
  bitsperpixel = vil1_16bit_read_little_endian(s);
  // allowed values for bitsperpixel are 1 4 8 16 24 32; currently we only support 8 and 24
}

void vil1_bmp_core_header::write(vil1_stream *s) const
{
  vil1_32bit_write_little_endian(s, header_size);
  vil1_32bit_write_little_endian(s, width);
  vil1_32bit_write_little_endian(s, height);
  vil1_16bit_write_little_endian(s, planes);
  vil1_16bit_write_little_endian(s, bitsperpixel);
}

void vil1_bmp_core_header::print(vcl_ostream &s) const
{
  s << "vil1_bmp_core_header:\n"
    << "  header_size  : " << header_size  << vcl_endl
    << "  width        : " << width        << vcl_endl
    << "  height       : " << height       << vcl_endl
    << "  planes       : " << planes       << vcl_endl
    << "  bitsperpixel : " << bitsperpixel << vcl_endl << vcl_endl;
}
