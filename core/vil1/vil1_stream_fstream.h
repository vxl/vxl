//-*- c++ -*-------------------------------------------------------------------
#ifndef vil_stream_fstream_h_
#define vil_stream_fstream_h_
#ifdef __GNUC__
#pragma interface
#endif
// Author: awf@robots.ox.ac.uk
// Created: 16 Feb 00

#include <fstream.h>
#include <vil/vil_stream.h>

//: A vil_stream implementation using fstream
class vil_stream_fstream : public vil_stream {
public:
  vil_stream_fstream(char const* filename, char const* mode);
  ~vil_stream_fstream();
  
  int write(void const* buf, int n);
  int read(void* buf, int n);
  int tell();
  void seek(int position);
  
private:
  fstream f_;
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vil_stream_fstream.
