// This is core/vil/file_formats/vil_bmp_info_header.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm

#include "vil_bmp_info_header.h"

#include <vcl_iostream.h>

#include <vil/vil_stream.h>
#include <vil/vil_stream_read.h>
#include <vil/vil_stream_write.h>

vil_bmp_info_header::vil_bmp_info_header()
{
  compression = 0;
  bitmap_size = 0;
  horiz_res = 0;
  verti_res = 0;
  colormapsize = 0;
  colorcount = 0;
}

void vil_bmp_info_header::read(vil_stream *s)
{
  compression = vil_stream_read_little_endian_uint_32(s);
  bitmap_size = vil_stream_read_little_endian_uint_32(s);
  horiz_res   = vil_stream_read_little_endian_uint_32(s);
  verti_res   = vil_stream_read_little_endian_uint_32(s);
  colormapsize= vil_stream_read_little_endian_uint_32(s);
  colorcount  = vil_stream_read_little_endian_uint_32(s);
}

void vil_bmp_info_header::write(vil_stream *s) const
{
  vil_stream_write_little_endian_uint_32(s, compression);
  vil_stream_write_little_endian_uint_32(s, bitmap_size);
  vil_stream_write_little_endian_uint_32(s, horiz_res);
  vil_stream_write_little_endian_uint_32(s, verti_res);
  vil_stream_write_little_endian_uint_32(s, colormapsize);
  vil_stream_write_little_endian_uint_32(s, colorcount);
}

void vil_bmp_info_header::print(vcl_ostream &s) const
{
  s << "vil_bmp_info_header:\n"
    << "  compression  : " << compression << vcl_endl
    << "  bitmap_size  : " << bitmap_size << vcl_endl
    << "  horiz_res    : " << horiz_res << vcl_endl
    << "  verti_res    : " << verti_res << vcl_endl
    << "  colormapsize : " << colormapsize << vcl_endl
    << "  colorcount   : " << colorcount << vcl_endl << vcl_endl;
}
