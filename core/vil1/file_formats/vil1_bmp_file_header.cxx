// This is core/vil1/file_formats/vil1_bmp_file_header.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm

#include "vil1_bmp_file_header.h"
#include <vcl_iomanip.h> // for vcl_hex, vcl_dec
#include <vcl_iostream.h>
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

void vil1_bmp_file_header::print(vcl_ostream &s) const
{
  s << "vil1_bmp_file_header:\n"
    << "  magic   : " << vcl_hex
    << "0x" << unsigned(magic[0]) << ' '
    << "0x" << unsigned(magic[1]) << vcl_endl
    << "  filesize: 0x" << file_size << vcl_endl
    << "  reserved: 0x" << reserved1 << vcl_endl
    << "  reserved: 0x" << reserved2 << vcl_endl
    << "  offset  : 0x" << bitmap_offset << vcl_endl
    << vcl_dec << vcl_endl;
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
