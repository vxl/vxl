/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation "vil_bmp_file_header"
#endif
#include "vil_bmp_file_header.h"
#include <vcl/vcl_iostream.h>
#include <vil/vil_stream.h>
#include <vil/vil_16bit.h>
#include <vil/vil_32bit.h>

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

void vil_bmp_file_header::print(ostream &s) const
{
  s << "vil_bmp_file_header:" << endl
    << "  magic   : " << vcl_hex 
    << "0x" << unsigned(magic[0]) << ' '
    << "0x" << unsigned(magic[1]) << endl
    << "  filesize: 0x" << file_size << endl
    << "  reserved: 0x" << reserved1 << endl
    << "  reserved: 0x" << reserved2 << endl
    << "  offset  : 0x" << bitmap_offset << endl
    << vcl_dec << endl;
}

void vil_bmp_file_header::read(vil_stream *s)
{
  s->read(&magic, sizeof(magic));
  file_size = vil_32bit_read_little_endian(s);
  reserved1 = vil_16bit_read_little_endian(s);
  reserved2 = vil_16bit_read_little_endian(s);
  bitmap_offset = vil_32bit_read_little_endian(s);
}

void vil_bmp_file_header::write(vil_stream *s) const
{
  s->write(&magic, sizeof(magic));
  vil_32bit_write_little_endian(s, file_size);
  vil_16bit_write_little_endian(s, reserved1);
  vil_16bit_write_little_endian(s, reserved2);
  vil_32bit_write_little_endian(s, bitmap_offset);
}

bool vil_bmp_file_header::signature_valid() const
{
  return
    magic[0] == BMP_SIGNATURE_BYTE_0 &&
    magic[1] == BMP_SIGNATURE_BYTE_1;
}
