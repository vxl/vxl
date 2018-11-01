// This is core/vil1/file_formats/vil1_bmp_file_header.cxx
//:
// \file
// \author fsm

#include <iomanip>
#include <iostream>
#include "vil1_bmp_file_header.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil1/vil1_stream.h>
#include <vil1/vil1_16bit.h>
#include <vil1/vil1_32bit.h>

// The signature consists of the two bytes 42, 4D in that order.
// It is not supposed to be read as a 16-bit integer.
#define BMP_SIGNATURE_BYTE_0 0x42
#define BMP_SIGNATURE_BYTE_1 0x4D

vil1_bmp_file_header::vil1_bmp_file_header()
{
  magic[0] = BMP_SIGNATURE_BYTE_0;
  magic[1] = BMP_SIGNATURE_BYTE_1;
  file_size = 0;
  reserved1 = 0;
  reserved2 = 0;
  bitmap_offset = 0;
}

void vil1_bmp_file_header::print(std::ostream &s) const
{
  s << "vil1_bmp_file_header:\n"
    << "  magic   : " << std::hex
    << "0x" << unsigned(magic[0]) << ' '
    << "0x" << unsigned(magic[1]) << std::endl
    << "  filesize: 0x" << file_size << std::endl
    << "  reserved: 0x" << reserved1 << std::endl
    << "  reserved: 0x" << reserved2 << std::endl
    << "  offset  : 0x" << bitmap_offset << std::endl
    << std::dec << std::endl;
}

void vil1_bmp_file_header::read(vil1_stream *s)
{
  if (s->read(&magic, sizeof(magic)) == 0) {magic[0] = magic[1] = 0;}
  file_size = vil1_32bit_read_little_endian(s);
  reserved1 = vil1_16bit_read_little_endian(s);
  reserved2 = vil1_16bit_read_little_endian(s);
  bitmap_offset = vil1_32bit_read_little_endian(s);
}

void vil1_bmp_file_header::write(vil1_stream *s) const
{
  s->write(&magic, sizeof(magic));
  vil1_32bit_write_little_endian(s, file_size);
  vil1_16bit_write_little_endian(s, reserved1);
  vil1_16bit_write_little_endian(s, reserved2);
  vil1_32bit_write_little_endian(s, bitmap_offset);
}

bool vil1_bmp_file_header::signature_valid() const
{
  return
    magic[0] == BMP_SIGNATURE_BYTE_0 &&
    magic[1] == BMP_SIGNATURE_BYTE_1;
}
