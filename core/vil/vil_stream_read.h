#ifndef vil_stream_read_h_
#define vil_stream_read_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief read numbers from vil_stream

#include <vxl_config.h>

class vil_stream;

vxl_uint_16 vil_stream_read_big_endian_uint_16(vil_stream *);
vxl_uint_16 vil_stream_read_little_endian_uint_16(vil_stream *);

vxl_uint_32 vil_stream_read_big_endian_uint_32(vil_stream *);
vxl_uint_32 vil_stream_read_little_endian_uint_32(vil_stream *);

float vil_stream_read_big_endian_float(vil_stream* is);


//: Reads in n 16 bit unsigned ints.
// Caller is responsible for allocating enough space.
void vil_stream_read_big_endian_int_16(vil_stream* is,
                                        vxl_uint_16* data, unsigned n);

#endif // vil_stream_read_h_
