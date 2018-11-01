// This is core/vil1/vil1_16bit.h
#ifndef vil1_16bit_h_
#define vil1_16bit_h_
//:
// \file
// \brief read/write integers to/from vil1_stream
//
// Functions to read and write integers to and from a vil1_stream.
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

class vil1_stream;

unsigned vil1_16bit_read_big_endian(vil1_stream *);
unsigned vil1_16bit_read_little_endian(vil1_stream *);

void vil1_16bit_write_big_endian(vil1_stream *, unsigned);
void vil1_16bit_write_little_endian(vil1_stream *, unsigned);

#endif // vil1_16bit_h_
