#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \brief read numbers from vil_stream
//
// Functions to read integers and floats from a vil_stream.
// The endianness refers to the format in the stream, not the
// native format of the compiler or execution environment.
//
// \author  fsm
//
// \verbatim
// Modifications
// Peter Vanroose, July 2000: corrected serious bug: VXL_LITTLE_ENDIAN not needed
//                    (implementation was wrong for VXL_BIG_ENDIAN machines)
// Ian Scott, May 2003: rearrange explicit io, to allow for easier expansion.
// \endverbatim

#include "vil_stream_read.h"
#include <vcl_cassert.h>

#include <vil/vil_stream.h>
#include <vxl_config.h>

vxl_uint_16 vil_stream_read_big_endian_uint_16(vil_stream *s)
{
  vxl_uint_8 bytes[2];
  s->read(bytes, sizeof bytes);
  return vxl_uint_16(bytes[1]) + (vxl_uint_16(bytes[0])<<8);
}

vxl_uint_16 vil_stream_read_little_endian_uint_16(vil_stream *s)
{
  vxl_uint_8 bytes[2];
  s->read(bytes, sizeof bytes);
  return vxl_uint_16(bytes[0]) + (vxl_uint_16(bytes[1])<<8);
}


vxl_uint_32 vil_stream_read_big_endian_uint_32(vil_stream *s)
{
  vxl_byte bytes[4];
  s->read(bytes, sizeof bytes);
  return (vxl_uint_32(bytes[0])<<24) + (vxl_uint_32(bytes[1])<<16) + (vxl_uint_32(bytes[2])<<8) + (vxl_uint_32(bytes[3]));
}

vxl_uint_32 vil_stream_read_little_endian_uint_32(vil_stream *s)
{
  vxl_byte bytes[4];
  s->read(bytes, sizeof bytes);
  return (vxl_uint_32(bytes[3])<<24) + (vxl_uint_32(bytes[2])<<16) + (vxl_uint_32(bytes[1])<<8) + (vxl_uint_32(bytes[0]));
}


// The following function should be moved to relevant places in vil soon
// This static function is only needed if it will be used below
#if VXL_LITTLE_ENDIAN
static void swap16(char *a, unsigned n)
{
  char c;
  for (unsigned i = 0; i < n * 2; i += 2)
  {
    c = a[i]; a[i] = a[i+1]; a[i+1] = c;
  }
}
#endif

// The following function should be moved to relevant places in vil soon
// This static function is only needed if it will be used below
#if VXL_LITTLE_ENDIAN
static void swap32(char *a, unsigned n)
{
  char c;
  for (unsigned i = 0; i < n * 4; i += 4)
  {
    c = a[i];
    a[i] = a[i+3];
    a[i+3] = c;
    c = a[i+1];
    a[i+1] = a[i+2];
    a[i+2] = c;
  }
}
#endif

// The following function should be moved to relevant places in vil soon
float vil_stream_read_big_endian_float(vil_stream* is)
{
  float f;
  is->read((char*)&f,4);
#if VXL_LITTLE_ENDIAN
  swap32((char*)&f,1);
#endif
  return f;
}

// The following function should be moved to relevant places in vil soon
// Reads in n shorts, assumed to be two bytes, into data[i]
void vil_stream_read_big_endian_int_16(vil_stream* is,
                                       vxl_uint_16* data, unsigned n)
{
  assert(sizeof(short)==2);
  is->read((char*)data,n*2);
#if VXL_LITTLE_ENDIAN
  swap16((char*)data,n);
#endif
}
