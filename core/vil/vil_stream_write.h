// This is core/vil/vil_stream_write.h
#ifndef vil_stream_write_h_
#define vil_stream_write_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief write integers to vil_stream
//
// Functions to write integers to aa vil_stream.
// The endianness refers to the format in the stream, not the
// native format of the compiler or execution environment.
//
// \author    fsm
//
// \verbatim
//  Modifications
//   23 Oct.2003 - Peter Vanroose - Added support for 64-bit int pixels
// \endvarbatim

#include <vxl_config.h>
class vil_stream;

void vil_stream_write_big_endian_uint_16(vil_stream *, vxl_uint_16);
void vil_stream_write_little_endian_uint_16(vil_stream *, vxl_uint_16);

void vil_stream_write_big_endian_uint_32(vil_stream *, vxl_uint_32);
void vil_stream_write_little_endian_uint_32(vil_stream *, vxl_uint_32);

#if VXL_HAS_INT_64
void vil_stream_write_big_endian_uint_64(vil_stream *, vxl_uint_64);
void vil_stream_write_little_endian_uint_64(vil_stream *, vxl_uint_64);
#endif

#endif // vil_stream_write_h_
