/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation "vil_bmp_info_header"
#endif
#include "vil_bmp_info_header.h"

#include <vcl/vcl_iostream.h>

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

void vil_bmp_info_header::print(ostream &s) const
{
  s << "vil_bmp_info_header:" << endl
    << "  compression  : " << compression << endl
    << "  bitmap_size  : " << bitmap_size << endl
    << "  horiz_res    : " << horiz_res << endl
    << "  verti_res    : " << verti_res << endl
    << "  colormapsize : " << colormapsize << endl
    << "  colorcount   : " << colorcount << endl
    << endl;
}
