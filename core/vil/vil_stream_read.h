#ifndef vil_stream_read_h_
#define vil_stream_read_h_
//:
// \file
// \brief read numbers from vil_stream
//
// \verbatim
//  Modifications
//   23 Oct.2003 - Peter Vanroose - Added support for 64-bit int pixels
// \endverbatim

#include <vxl_config.h>
class vil_stream;

vxl_uint_16 vil_stream_read_big_endian_uint_16(vil_stream *);
vxl_uint_16 vil_stream_read_little_endian_uint_16(vil_stream *);

vxl_uint_32 vil_stream_read_big_endian_uint_32(vil_stream *);
vxl_uint_32 vil_stream_read_little_endian_uint_32(vil_stream *);

vxl_int_32 vil_stream_read_big_endian_int_32(vil_stream *s);
vxl_int_32 vil_stream_read_little_endian_int_32(vil_stream *s);

#if VXL_HAS_INT_64
vxl_uint_64 vil_stream_read_big_endian_uint_64(vil_stream *);
vxl_uint_64 vil_stream_read_little_endian_uint_64(vil_stream *);
#endif

//: Reads in a 4-byte big-endian float.
// \relatesalso vil_stream
float vil_stream_read_big_endian_float(vil_stream* is);

//: Reads in n 16 bit unsigned ints.
// Caller is responsible for allocating enough space.
// \relatesalso vil_stream
void vil_stream_read_big_endian_int_16(vil_stream* is,
                                       vxl_uint_16* data, unsigned n);

#endif // vil_stream_read_h_
