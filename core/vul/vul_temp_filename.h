// This is vxl/vul/vul_temp_filename.h
#ifndef vul_temp_filename_h_
#define vul_temp_filename_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief  Generates a temporary filename.
// \author Amitha Perera <perera@cs.rpi.edu>
//
// This is to avoid (GNU) linker warnings (and errors!) about calls to
// tmpnam being unsafe.

#include <vcl_string.h>

//: Generates a temporary filename.
// There is a possible race condition, in that some other process may
// open a the temporary file between the time tmpnam creates (and
// tests) it, and the time the calling program opens it.
vcl_string vul_temp_filename( );

#endif // vul_temp_filename_h_
