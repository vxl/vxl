#ifndef vil_16bit_h_
#define vil_16bit_h_
#ifdef __GNUC__
#pragma interface
#endif

// .NAME vil_16bit
// .INCLUDE vil/vil_16bit.h
// .FILE vil_16bit.cxx
// .SECTION Description
// Functions to read and write integers to and from a vil_stream.
// The endianness refers to the format in the stream, not the
// native format of the compiler or execution environment.
//
// .SECTION Author
//    fsm
// .SECTION Modifications
// Peter Vanroose, July 2000: corrected serious bug: VXL_LITTLE_ENDIAN not needed (implementation was wrong for VXL_BIG_ENDIAN machines)

class vil_stream;

unsigned vil_16bit_read_big_endian(vil_stream *);
unsigned vil_16bit_read_little_endian(vil_stream *);

void vil_16bit_write_big_endian(vil_stream *, unsigned);
void vil_16bit_write_little_endian(vil_stream *, unsigned);

#endif
