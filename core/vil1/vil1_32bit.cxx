// This is vxl/vil/vil_32bit.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm@robots.ox.ac.uk


#include "vil_32bit.h"
#include <vil/vil_stream.h>
#include <vxl_config.h>

typedef vxl_uint_8  word8;
typedef vxl_uint_32 word32;

unsigned vil_32bit_read_big_endian(vil_stream *s)
{
  word8 bytes[4];
  s->read(bytes, sizeof bytes);
  return (word32(bytes[0])<<24) + (word32(bytes[1])<<16) + (word32(bytes[2])<<8) + (word32(bytes[3]));
}

unsigned vil_32bit_read_little_endian(vil_stream *s)
{
  word8 bytes[4];
  s->read(bytes, sizeof bytes);
  return (word32(bytes[3])<<24) + (word32(bytes[2])<<16) + (word32(bytes[1])<<8) + (word32(bytes[0]));
}

void vil_32bit_write_big_endian(vil_stream *s, unsigned w)
{
  word8 bytes[4];
  bytes[0] = w >> 24;
  bytes[1] = w >> 16;
  bytes[2] = w >> 8;
  bytes[3] = w >> 0;
  s->write(bytes, sizeof bytes);
}

void vil_32bit_write_little_endian(vil_stream *s, unsigned w)
{
  word8 bytes[4];
  bytes[0] = w >> 0;
  bytes[1] = w >> 8;
  bytes[2] = w >> 16;
  bytes[3] = w >> 24;
  s->write(bytes, sizeof bytes);
}
