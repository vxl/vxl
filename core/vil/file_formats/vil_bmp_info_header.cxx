// This is mul/vil2/file_formats/vil2_bmp_info_header.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm@robots.ox.ac.uk

#include "vil2_bmp_info_header.h"

#include <vcl_iostream.h>

#include <vil2/vil2_stream.h>
#include <vil2/vil2_stream_32bit.h>

vil2_bmp_info_header::vil2_bmp_info_header()
{
  compression = 0;
  bitmap_size = 0;
  horiz_res = 0;
  verti_res = 0;
  colormapsize = 0;
  colorcount = 0;
}

void vil2_bmp_info_header::read(vil2_stream *s)
{
  compression = vil2_stream_32bit_read_little_endian(s);
  bitmap_size = vil2_stream_32bit_read_little_endian(s);
  horiz_res   = vil2_stream_32bit_read_little_endian(s);
  verti_res   = vil2_stream_32bit_read_little_endian(s);
  colormapsize= vil2_stream_32bit_read_little_endian(s);
  colorcount  = vil2_stream_32bit_read_little_endian(s);
}

void vil2_bmp_info_header::write(vil2_stream *s) const
{
  vil2_stream_32bit_write_little_endian(s, compression);
  vil2_stream_32bit_write_little_endian(s, bitmap_size);
  vil2_stream_32bit_write_little_endian(s, horiz_res);
  vil2_stream_32bit_write_little_endian(s, verti_res);
  vil2_stream_32bit_write_little_endian(s, colormapsize);
  vil2_stream_32bit_write_little_endian(s, colorcount);
}

void vil2_bmp_info_header::print(vcl_ostream &s) const
{
  s << "vil2_bmp_info_header:\n"
    << "  compression  : " << compression << vcl_endl
    << "  bitmap_size  : " << bitmap_size << vcl_endl
    << "  horiz_res    : " << horiz_res << vcl_endl
    << "  verti_res    : " << verti_res << vcl_endl
    << "  colormapsize : " << colormapsize << vcl_endl
    << "  colorcount   : " << colorcount << vcl_endl << vcl_endl;
}
