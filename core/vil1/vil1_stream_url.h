#ifndef vil_stream_url_h_
#define vil_stream_url_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
*/

//:
// \file

#include <vil/vil_stream.h>

//: open an URL

struct vil_stream_url : vil_stream
{
  vil_stream_url(char const *);
  ~vil_stream_url();

  bool ok();
  int write(void const *buf, int n);
  int read(void *buf, int n);
  int tell();
  void seek(int position);

private:
  vil_stream *underlying;
  vil_stream_url(vil_stream_url const &) { }
  void operator=(vil_stream_url const &) { }
};

#endif
