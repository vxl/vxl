//-*- c++ -*-------------------------------------------------------------------
#ifndef vil_stream_fstream_h_
#define vil_stream_fstream_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vil_stream_fstream - A vil_stream implementation using fstream
// .INCLUDE vil/vil_stream_fstream.h
// .FILE vil_stream_fstream.cxx
// .SECTION Author
//    awf@robots.ox.ac.uk
// Created: 16 Feb 00

#include <vcl/vcl_fstream.h>
#include <vil/vil_stream.h>

class vil_stream_fstream : public vil_stream {
public:
  vil_stream_fstream(char const* filename, char const* mode);
  ~vil_stream_fstream();

  bool ok();
  
  int write(void const* buf, int n);
  int read(void* buf, int n);
  int tell();
  void seek(int position);
  
private:
  int flags_;
  fstream f_;
  int id_;
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vil_stream_fstream.
