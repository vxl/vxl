// This is vxl/vil/file_formats/vil_bmp_info_header.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm

#include "vil_bmp_info_header.h"

#include <vcl_iostream.h>

#include <vil/vil_stream.h>
#include <vil/vil_32bit.h>

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
  compression = vil_32bit_read_little_endian(s);
  bitmap_size = vil_32bit_read_little_endian(s);
  horiz_res   = vil_32bit_read_little_endian(s);
  verti_res   = vil_32bit_read_little_endian(s);
  colormapsize= vil_32bit_read_little_endian(s);
  colorcount  = vil_32bit_read_little_endian(s);
}

void vil_bmp_info_header::write(vil_stream *s) const
{
  vil_32bit_write_little_endian(s, compression);
  vil_32bit_write_little_endian(s, bitmap_size);
  vil_32bit_write_little_endian(s, horiz_res);
  vil_32bit_write_little_endian(s, verti_res);
  vil_32bit_write_little_endian(s, colormapsize);
  vil_32bit_write_little_endian(s, colorcount);
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
