// This is core/vil1/file_formats/vil1_bmp_info_header.cxx
//:
// \file
// \author fsm

#include <iostream>
#include "vil1_bmp_info_header.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vil1/vil1_stream.h>
#include <vil1/vil1_32bit.h>

vil1_bmp_info_header::vil1_bmp_info_header()
{
  compression = 0;
  bitmap_size = 0;
  horiz_res = 0;
  verti_res = 0;
  colormapsize = 0;
  colorcount = 0;
}

void vil1_bmp_info_header::read(vil1_stream *s)
{
  compression = vil1_32bit_read_little_endian(s);
  bitmap_size = vil1_32bit_read_little_endian(s);
  horiz_res   = vil1_32bit_read_little_endian(s);
  verti_res   = vil1_32bit_read_little_endian(s);
  colormapsize= vil1_32bit_read_little_endian(s);
  colorcount  = vil1_32bit_read_little_endian(s);
}

void vil1_bmp_info_header::write(vil1_stream *s) const
{
  vil1_32bit_write_little_endian(s, compression);
  vil1_32bit_write_little_endian(s, bitmap_size);
  vil1_32bit_write_little_endian(s, horiz_res);
  vil1_32bit_write_little_endian(s, verti_res);
  vil1_32bit_write_little_endian(s, colormapsize);
  vil1_32bit_write_little_endian(s, colorcount);
}

void vil1_bmp_info_header::print(std::ostream &s) const
{
  s << "vil1_bmp_info_header:\n"
    << "  compression  : " << compression << std::endl
    << "  bitmap_size  : " << bitmap_size << std::endl
    << "  horiz_res    : " << horiz_res << std::endl
    << "  verti_res    : " << verti_res << std::endl
    << "  colormapsize : " << colormapsize << std::endl
    << "  colorcount   : " << colorcount << std::endl << std::endl;
}
