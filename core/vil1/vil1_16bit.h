#ifndef vil_16bit_h_
#define vil_16bit_h_
#ifdef __GNUC__
#pragma interface "vil_16bit"
#endif
/*
  fsm@robots.ox.ac.uk
*/

// Functions to read and write integers to and from a vil_stream.
// The endianness refers to the format in the stream, not the
// native format of the compiler or execution environment.

class vil_stream;
#include <vxl_misc_config.h>

unsigned vxl_int16 vil_16bit_read_big_endian(vil_stream *);
unsigned vxl_int16 vil_16bit_read_little_endian(vil_stream *);

void vil_16bit_write_big_endian(vil_stream *, unsigned vxl_int16);
void vil_16bit_write_little_endian(vil_stream *, unsigned vxl_int16);

#endif
