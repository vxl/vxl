// This is contrib/mul/vil2/vil2_open.h
#ifndef vil2_open_h_
#define vil2_open_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief make a vil2_stream from a filename, an URL, etc.
//
// \author fsm
//
// \verbatim
//  Modifications
//\endverbatim

#include <vil2/vil2_stream.h>

vil2_stream *vil2_open(char const* what, char const* how = "r");

#endif // vil2_open_h_
