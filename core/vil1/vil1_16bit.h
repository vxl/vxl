// This is vxl/vil/vil_16bit.h
#ifndef vil_16bit_h_
#define vil_16bit_h_
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
// \author  fsm
//
// \verbatim
// Modifications
// Peter Vanroose, July 2000: corrected serious bug: VXL_LITTLE_ENDIAN not needed
//                    (implementation was wrong for VXL_BIG_ENDIAN machines)
// \endverbatim

class vil_stream;

unsigned vil_16bit_read_big_endian(vil_stream *);
unsigned vil_16bit_read_little_endian(vil_stream *);

void vil_16bit_write_big_endian(vil_stream *, unsigned);
void vil_16bit_write_little_endian(vil_stream *, unsigned);

#endif // vil_16bit_h_
