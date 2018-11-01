// This is core/vul/vul_temp_filename.h
#ifndef vul_temp_filename_h_
#define vul_temp_filename_h_
//:
// \file
// \brief  Generates a temporary filename.
// \author Amitha Perera <perera@cs.rpi.edu>
//
// This is to avoid (GNU) linker warnings (and errors!) about calls to
// tmpnam being unsafe.

#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Generates a temporary filename.
// There is a possible race condition, in that some other process may
// open a the temporary file between the time tmpnam creates (and
// tests) it, and the time the calling program opens it.
std::string vul_temp_filename( );

#endif // vul_temp_filename_h_
