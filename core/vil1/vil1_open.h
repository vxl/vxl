// This is core/vil1/vil1_open.h
#ifndef vil1_open_h_
#define vil1_open_h_
//:
// \file
// \brief make a vil1_stream from a filename, an URL, etc.
//
// \author fsm
//
// \verbatim
//  Modifications
//\endverbatim

#include "vil1_stream.h"

vil1_stream *
vil1_open(const char * what, const char * how = "r");

#endif // vil1_open_h_
