#ifndef vil_16bit_h_
#define vil_16bit_h_
#ifdef __GNUC__
#pragma interface
#endif
// This is vxl/vil/vil_16bit.h


//:
// \file
// \brief vil_16bit - read/write integers to/from vil_stream
// \author  fsm
//
// Functions to read and write integers to and from a vil_stream.
// The endianness refers to the format in the stream, not the
// native format of the compiler or execution environment.
//
// Modifications
// \verbatim
// Peter Vanroose, July 2000: corrected serious bug: VXL_LITTLE_ENDIAN not needed
//                    (implementation was wrong for VXL_BIG_ENDIAN machines)
// \endverbatim

class vil_stream;

unsigned vil_16bit_read_big_endian(vil_stream *);
unsigned vil_16bit_read_little_endian(vil_stream *);

void vil_16bit_write_big_endian(vil_stream *, unsigned);
void vil_16bit_write_little_endian(vil_stream *, unsigned);

#endif // vil_16bit_h_
