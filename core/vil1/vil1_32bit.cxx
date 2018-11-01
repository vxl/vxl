// This is core/vil1/vil1_32bit.cxx
//:
// \file
// \author fsm


#include "vil1_32bit.h"
#include <vil1/vil1_stream.h>
#include <vxl_config.h>

typedef vxl_uint_8  word8;
typedef vxl_uint_32 word32;

unsigned vil1_32bit_read_big_endian(vil1_stream *s)
{
  word8 bytes[4];
  s->read(bytes, sizeof bytes);
  return (word32(bytes[0])<<24) + (word32(bytes[1])<<16) + (word32(bytes[2])<<8) + (word32(bytes[3]));
}

unsigned vil1_32bit_read_little_endian(vil1_stream *s)
{
  word8 bytes[4];
  s->read(bytes, sizeof bytes);
  return (word32(bytes[3])<<24) + (word32(bytes[2])<<16) + (word32(bytes[1])<<8) + (word32(bytes[0]));
}

void vil1_32bit_write_big_endian(vil1_stream *s, unsigned w)
{
  word8 bytes[4];
  bytes[0] = w >> 24;
  bytes[1] = w >> 16;
  bytes[2] = w >> 8;
  bytes[3] = w >> 0;
  s->write(bytes, sizeof bytes);
}

void vil1_32bit_write_little_endian(vil1_stream *s, unsigned w)
{
  word8 bytes[4];
  bytes[0] = w >> 0;
  bytes[1] = w >> 8;
  bytes[2] = w >> 16;
  bytes[3] = w >> 24;
  s->write(bytes, sizeof bytes);
}
