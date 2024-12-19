// This is core/vil/vil_open.h
#ifndef vil_open_h_
#define vil_open_h_
//:
// \file
// \brief make a vil_stream from a filename, an URL, etc.
//
// \author fsm

#include "vil_stream.h"
#include <vxl_config.h>

//: make a vil_stream from a filename, an URL, etc.
// \relatesalso vil_stream
vil_stream *
vil_open(const char * what, const char * how = "r");


#if defined(_WIN32) && VXL_USE_WIN_WCHAR_T
//: make a vil_stream from a filename, an URL, etc.
// \relatesalso vil_stream
vil_stream *
vil_open(const wchar_t * what, const char * how = "r");
#endif // defined(_WIN32) && VXL_USE_WIN_WCHAR_T

#endif // vil_open_h_
