#ifndef vul_temp_filename_h_
#define vul_temp_filename_h_
#ifdef __GNUC__
#pragma interface
#endif

// \file
// \author Amitha Perera <perera@cs.rpi.edu>
// \brief  Generates a temporary filename.
// This is to avoid (GNU) linker warnings (and errors!) about calls to
// tmpnam being unsafe.

#include <vcl_string.h>

//: Generates a temporary filename.
// There is a possible race condition, in that some other process may
// open a the temporary file between the time tmpnam creates (and
// tests) it, and the time the calling program opens it.
vcl_string vul_temp_filename( );

#endif
