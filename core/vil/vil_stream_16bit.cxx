// This is core/vil2/vil2_stream_16bit.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm

// ****** This is platform specific!!! Need to swap byte order for some platforms!!! ***

#include "vil2_stream_16bit.h"
#include <vil2/vil2_stream.h>
#include <vxl_config.h>

typedef vxl_uint_8  word8;
typedef vxl_uint_16 word16;

unsigned vil2_stream_16bit_read_big_endian(vil2_stream *s)
{
  word8 bytes[2];
  s->read(bytes, sizeof bytes);
  return word16(bytes[1]) + (word16(bytes[0])<<8);
}

unsigned vil2_stream_16bit_read_little_endian(vil2_stream *s)
{
  word8 bytes[2];
  s->read(bytes, sizeof bytes);
  return word16(bytes[0]) + (word16(bytes[1])<<8);
}

void vil2_stream_16bit_write_big_endian(vil2_stream *s, unsigned w)
{
  word8 bytes[2];
  bytes[0] = word8(w >> 8);
  bytes[1] = word8(w & 0xff);
  s->write(bytes, sizeof bytes);
}

void vil2_stream_16bit_write_little_endian(vil2_stream *s, unsigned w)
{
  word8 bytes[2];
  bytes[0] = word8(w & 0xff);
  bytes[1] = word8(w >> 8);
  s->write(bytes, sizeof bytes);
}
