#ifndef vil_32bit_h_
#define vil_32bit_h_
#ifdef __GNUC__
#pragma interface
#endif
// This is vxl/vil/vil_32bit.h

//:
// \file
// \brief vil_32bit - read/write integers to/from vil_stream
// \author    fsm
// Functions to read and write integers to and from a vil_stream.
// The endianness refers to the format in the stream, not the
// native format of the compiler or execution environment.

class vil_stream;

unsigned vil_32bit_read_big_endian(vil_stream *);
unsigned vil_32bit_read_little_endian(vil_stream *);

void vil_32bit_write_big_endian(vil_stream *, unsigned);
void vil_32bit_write_little_endian(vil_stream *, unsigned);

#endif // vil_32bit_h_
