// This is core/vil1/vil1_16bit.cxx
//:
// \file
// \author fsm

#include "vil1_16bit.h"
#include "vil1/vil1_stream.h"
#include "vxl_config.h"

using word8 = vxl_uint_8;
using word16 = vxl_uint_16;

unsigned
vil1_16bit_read_big_endian(vil1_stream * s)
{
  word8 bytes[2];
  s->read(bytes, sizeof bytes);
  return word16(bytes[1]) + (word16(bytes[0]) << 8);
}

unsigned
vil1_16bit_read_little_endian(vil1_stream * s)
{
  word8 bytes[2];
  s->read(bytes, sizeof bytes);
  return word16(bytes[0]) + (word16(bytes[1]) << 8);
}

void
vil1_16bit_write_big_endian(vil1_stream * s, unsigned w)
{
  word8 bytes[2];
  bytes[0] = word8(w >> 8);
  bytes[1] = word8(w & 0xff);
  s->write(bytes, sizeof bytes);
}

void
vil1_16bit_write_little_endian(vil1_stream * s, unsigned w)
{
  word8 bytes[2];
  bytes[0] = word8(w & 0xff);
  bytes[1] = word8(w >> 8);
  s->write(bytes, sizeof bytes);
}
