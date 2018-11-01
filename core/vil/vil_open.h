// This is core/vil/vil_open.h
#ifndef vil_open_h_
#define vil_open_h_
//:
// \file
// \brief make a vil_stream from a filename, an URL, etc.
//
// \author fsm

#include <vil/vil_stream.h>
#include <vxl_config.h>

//: make a vil_stream from a filename, an URL, etc.
// \relatesalso vil_stream
vil_stream *vil_open(char const* what, char const* how = "r");


#if defined(_WIN32) && VXL_USE_WIN_WCHAR_T
//: make a vil_stream from a filename, an URL, etc.
// \relatesalso vil_stream
vil_stream *vil_open(wchar_t const* what, char const* how = "r");
#endif //defined(_WIN32) && VXL_USE_WIN_WCHAR_T

#endif // vil_open_h_
