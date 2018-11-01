// This is core/vil/file_formats/vil_bmp_file_header.cxx
//:
// \file
// \author fsm

#include <iomanip>
#include <iostream>
#include "vil_bmp_file_header.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_stream.h>
#include <vil/vil_stream_write.h>
#include <vil/vil_stream_read.h>

// The signature consists of the two bytes 42, 4D in that order.
// It is not supposed to be read as a 16-bit integer.
#define BMP_SIGNATURE_BYTE_0 0x42
#define BMP_SIGNATURE_BYTE_1 0x4D

vil_bmp_file_header::vil_bmp_file_header()
{
  magic[0] = BMP_SIGNATURE_BYTE_0;
  magic[1] = BMP_SIGNATURE_BYTE_1;
  file_size = 0;
  reserved1 = 0;
  reserved2 = 0;
  bitmap_offset = 0;
}

void vil_bmp_file_header::print(std::ostream &s) const
{
  s << "vil_bmp_file_header:\n"
    << "  magic   : " << std::hex
    << "0x" << unsigned(magic[0]) << ' '
    << "0x" << unsigned(magic[1]) << std::endl
    << "  filesize: 0x" << file_size << std::endl
    << "  reserved: 0x" << reserved1 << std::endl
    << "  reserved: 0x" << reserved2 << std::endl
    << "  offset  : 0x" << bitmap_offset << std::endl
    << std::dec << std::endl;
}

void vil_bmp_file_header::read(vil_stream *s)
{
  if (s->read(&magic, sizeof(magic)) == 0) {magic[0] = magic[1] = 0;}
  file_size = vil_stream_read_little_endian_uint_32(s);
  reserved1 = vil_stream_read_little_endian_uint_16(s);
  reserved2 = vil_stream_read_little_endian_uint_16(s);
  bitmap_offset = vil_stream_read_little_endian_uint_32(s);
}

void vil_bmp_file_header::write(vil_stream *s) const
{
  s->write(&magic, sizeof(magic));
  vil_stream_write_little_endian_uint_32(s, file_size);
  vil_stream_write_little_endian_uint_16(s, reserved1);
  vil_stream_write_little_endian_uint_16(s, reserved2);
  vil_stream_write_little_endian_uint_32(s, bitmap_offset);
}

bool vil_bmp_file_header::signature_valid() const
{
  return
    magic[0] == BMP_SIGNATURE_BYTE_0 &&
    magic[1] == BMP_SIGNATURE_BYTE_1;
}
