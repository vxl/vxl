/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif

#include "vil_32bit.h"
#include <vil/vil_stream.h>
#include <vxl_config.h>

unsigned vil_32bit_read_big_endian(vil_stream *s)
{
  vxl_uint_8 bytes[4];
  s->read(bytes, sizeof bytes);
  return (vxl_uint_32(bytes[0])<<24) + (vxl_uint_32(bytes[1])<<16) + (vxl_uint_32(bytes[2])<<8) + (vxl_uint_32(bytes[3]));
}

unsigned vil_32bit_read_little_endian(vil_stream *s)
{
  vxl_uint_8 bytes[4];
  s->read(bytes, sizeof bytes);
  return (vxl_uint_32(bytes[3])<<24) + (vxl_uint_32(bytes[2])<<16) + (vxl_uint_32(bytes[1])<<8) + (vxl_uint_32(bytes[0]));
}

void vil_32bit_write_big_endian(vil_stream *s, unsigned w)
{
  vxl_uint_8 bytes[4];
  bytes[0] = w >> 24;
  bytes[1] = w >> 16;
  bytes[2] = w >> 8;
  bytes[3] = w >> 0;
  s->write(bytes, sizeof bytes);
}

void vil_32bit_write_little_endian(vil_stream *s, unsigned w)
{
  vxl_uint_8 bytes[4];
  bytes[0] = w >> 0;
  bytes[1] = w >> 8;
  bytes[2] = w >> 16;
  bytes[3] = w >> 24;
  s->write(bytes, sizeof bytes);
}
