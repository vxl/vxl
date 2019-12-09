//:
// \file
// \author fsm
//
// \verbatim
//  Modifications
//   23 Oct.2003 - Peter Vanroose - Added support for 64-bit int pixels
// \endverbatim

#include "vil_stream_write.h"
#include "vil/vil_stream.h"
#include "vxl_config.h"

void
vil_stream_write_big_endian_uint_16(vil_stream * s, vxl_uint_16 w)
{
  vxl_uint_8 bytes[2];
  bytes[0] = vxl_uint_8(w >> 8);
  bytes[1] = vxl_uint_8(w & 0xff);
  s->write(bytes, sizeof bytes);
}

void
vil_stream_write_little_endian_uint_16(vil_stream * s, vxl_uint_16 w)
{
  vxl_uint_8 bytes[2];
  bytes[0] = vxl_uint_8(w & 0xff);
  bytes[1] = vxl_uint_8(w >> 8);
  s->write(bytes, sizeof bytes);
}

void
vil_stream_write_big_endian_uint_32(vil_stream * s, vxl_uint_32 w)
{
  vxl_byte bytes[4];
  bytes[0] = w >> 24;
  bytes[1] = w >> 16;
  bytes[2] = w >> 8;
  bytes[3] = w >> 0;
  s->write(bytes, sizeof bytes);
}

void
vil_stream_write_little_endian_uint_32(vil_stream * s, vxl_uint_32 w)
{
  vxl_byte bytes[4];
  bytes[0] = w >> 0;
  bytes[1] = w >> 8;
  bytes[2] = w >> 16;
  bytes[3] = w >> 24;
  s->write(bytes, sizeof bytes);
}

void
vil_stream_write_big_endian_int_32(vil_stream * s, vxl_int_32 w)
{
  vxl_byte bytes[4];
  bytes[0] = w >> 24;
  bytes[1] = w >> 16;
  bytes[2] = w >> 8;
  bytes[3] = w >> 0;
  s->write(bytes, sizeof bytes);
}

void
vil_stream_write_little_endian_int_32(vil_stream * s, vxl_int_32 w)
{
  vxl_byte bytes[4];
  bytes[0] = w >> 0;
  bytes[1] = w >> 8;
  bytes[2] = w >> 16;
  bytes[3] = w >> 24;
  s->write(bytes, sizeof bytes);
}

#if VXL_HAS_INT_64

void
vil_stream_write_big_endian_uint_64(vil_stream * s, vxl_uint_64 w)
{
  vxl_byte bytes[8];
  bytes[0] = static_cast<vxl_byte>(w >> 56);
  bytes[1] = static_cast<vxl_byte>(w >> 48);
  bytes[2] = static_cast<vxl_byte>(w >> 40);
  bytes[3] = static_cast<vxl_byte>(w >> 32);
  bytes[4] = static_cast<vxl_byte>(w >> 24);
  bytes[5] = static_cast<vxl_byte>(w >> 16);
  bytes[6] = static_cast<vxl_byte>(w >> 8);
  bytes[7] = static_cast<vxl_byte>(w >> 0);
  s->write(bytes, sizeof bytes);
}

void
vil_stream_write_little_endian_uint_64(vil_stream * s, vxl_uint_64 w)
{
  vxl_byte bytes[8];
  bytes[0] = static_cast<vxl_byte>(w >> 0);
  bytes[1] = static_cast<vxl_byte>(w >> 8);
  bytes[2] = static_cast<vxl_byte>(w >> 16);
  bytes[3] = static_cast<vxl_byte>(w >> 24);
  bytes[4] = static_cast<vxl_byte>(w >> 32);
  bytes[5] = static_cast<vxl_byte>(w >> 40);
  bytes[6] = static_cast<vxl_byte>(w >> 48);
  bytes[7] = static_cast<vxl_byte>(w >> 56);
  s->write(bytes, sizeof bytes);
}

#endif // VXL_HAS_INT_64
