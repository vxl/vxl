#ifndef vil_stream_FILE_ptr_h_
#define vil_stream_FILE_ptr_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
*/
// Created: 17 Feb 2000
// Purpose: turn a vil_stream * into a FILE *

#include <vcl/vcl_string.h> // C++ specific includes first
#include <vcl/vcl_cstdio.h>
#include <vil/vil_stream.h>

//: this adapts a vil_stream * into a FILE * by creating a tmpfile().
class vil_stream_FILE_ptr {
public:
  vil_stream_FILE_ptr(vil_stream *); // will delete the vil_stream.
  ~vil_stream_FILE_ptr(); 

  operator FILE *() const { return fp; }
  
private:
  vcl_string filename;
  FILE *fp;
};

#endif
