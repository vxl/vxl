/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "vil_bmp_core_header.h"

#include <vcl/vcl_iostream.h>

#include <vil/vil_stream.h>
#include <vil/vil_16bit.h>
#include <vil/vil_32bit.h>

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
  header_size  = vil_32bit_read_little_endian(s);
  width        = vil_32bit_read_little_endian(s);
  height       = vil_32bit_read_little_endian(s);
  planes       = vil_16bit_read_little_endian(s);
  bitsperpixel = vil_16bit_read_little_endian(s);
}

void vil_bmp_core_header::write(vil_stream *s) const
{
  vil_32bit_write_little_endian(s, header_size);
  vil_32bit_write_little_endian(s, width);
  vil_32bit_write_little_endian(s, height);
  vil_16bit_write_little_endian(s, planes);
  vil_16bit_write_little_endian(s, bitsperpixel);
}

void vil_bmp_core_header::print(ostream &s) const
{
  s << "vil_bmp_core_header:" << endl
    << "  header_size  : " << header_size  << endl
    << "  width        : " << width        << endl
    << "  height       : " << height       << endl
    << "  planes       : " << planes       << endl
    << "  bitsperpixel : " << bitsperpixel << endl
    << endl;
}
