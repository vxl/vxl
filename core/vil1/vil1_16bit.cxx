/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "vil_16bit.h"
#include <vil/vil_stream.h>
#include <vxl_config.h>

unsigned vil_16bit_read_big_endian(vil_stream *s)
{
  vxl_uint_8 bytes[2];
  s->read(bytes, sizeof bytes);
  return vxl_uint_16(bytes[1]) + (vxl_uint_16(bytes[0])<<8);
}

unsigned vil_16bit_read_little_endian(vil_stream *s)
{
  vxl_uint_8 bytes[2];
  s->read(bytes, sizeof bytes);
  return vxl_uint_16(bytes[0]) + (vxl_uint_16(bytes[1])<<8);
}

void vil_16bit_write_big_endian(vil_stream *s, unsigned w)
{
  vxl_uint_8 bytes[2];
  bytes[0] = vxl_uint_8(w >> 8);
  bytes[1] = vxl_uint_8(w & 0xff);
  s->write(bytes, sizeof bytes);
}

void vil_16bit_write_little_endian(vil_stream *s, unsigned w)
{
  vxl_uint_8 bytes[2];
  bytes[0] = vxl_uint_8(w & 0xff);
  bytes[1] = vxl_uint_8(w >> 8);
  s->write(bytes, sizeof bytes);
}
