// This is core/vil2/vil2_stream_32bit.h
#ifndef vil2_stream_32bit_h_
#define vil2_stream_32bit_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief read/write integers to/from vil2_stream
//
// Functions to read and write integers to and from a vil2_stream.
// The endianness refers to the format in the stream, not the
// native format of the compiler or execution environment.
//
// \author    fsm

class vil2_stream;

unsigned vil2_stream_32bit_read_big_endian(vil2_stream *);
unsigned vil2_stream_32bit_read_little_endian(vil2_stream *);

void vil2_stream_32bit_write_big_endian(vil2_stream *, unsigned);
void vil2_stream_32bit_write_little_endian(vil2_stream *, unsigned);

#endif // vil2_stream_32bit_h_
