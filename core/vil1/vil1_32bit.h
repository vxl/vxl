// This is vxl/vil/vil_32bit.h
#ifndef vil_32bit_h_
#define vil_32bit_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief read/write integers to/from vil_stream
//
// Functions to read and write integers to and from a vil_stream.
// The endianness refers to the format in the stream, not the
// native format of the compiler or execution environment.
//
// \author    fsm

class vil_stream;

unsigned vil_32bit_read_big_endian(vil_stream *);
unsigned vil_32bit_read_little_endian(vil_stream *);

void vil_32bit_write_big_endian(vil_stream *, unsigned);
void vil_32bit_write_little_endian(vil_stream *, unsigned);

#endif // vil_32bit_h_
