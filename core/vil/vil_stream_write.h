// This is core/vil/vil_stream_write.h
#ifndef vil_stream_write_h_
#define vil_stream_write_h_
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
// \endverbatim

#include <vxl_config.h>
class vil_stream;

//:
// \relatesalso vil_stream
void vil_stream_write_big_endian_uint_16(vil_stream *, vxl_uint_16);
//:
// \relatesalso vil_stream
void vil_stream_write_little_endian_uint_16(vil_stream *, vxl_uint_16);

//:
// \relatesalso vil_stream
void vil_stream_write_big_endian_uint_32(vil_stream *, vxl_uint_32);
//:
// \relatesalso vil_stream
void vil_stream_write_little_endian_uint_32(vil_stream *, vxl_uint_32);

//:
// \relatesalso vil_stream
void vil_stream_write_big_endian_int_32(vil_stream *, vxl_int_32);
//:
// \relatesalso vil_stream
void vil_stream_write_little_endian_int_32(vil_stream *, vxl_int_32);

#if VXL_HAS_INT_64
//:
// \relatesalso vil_stream
void vil_stream_write_big_endian_uint_64(vil_stream *, vxl_uint_64);
//:
// \relatesalso vil_stream
void vil_stream_write_little_endian_uint_64(vil_stream *, vxl_uint_64);
#endif

#endif // vil_stream_write_h_
