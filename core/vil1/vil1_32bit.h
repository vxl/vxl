#ifndef vil_32bit_h_
#define vil_32bit_h_
#ifdef __GNUC__
#pragma interface "vil_32bit"
#endif

// .NAME vil_32bit
// .SECTION Description
// Functions to read and write integers to and from a vil_stream.
// The endianness refers to the format in the stream, not the
// native format of the compiler or execution environment.
//
// .SECTION Author
//     fsm

class vil_stream;
#include <vxl_misc_config.h>

vxl_uint32 vil_32bit_read_big_endian(vil_stream *);
vxl_uint32 vil_32bit_read_little_endian(vil_stream *);

void vil_32bit_write_big_endian(vil_stream *, vxl_uint32);
void vil_32bit_write_little_endian(vil_stream *, vxl_uint32);

#endif
