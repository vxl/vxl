// This is vxl/vil/vil_stream_url.h
#ifndef vil_stream_url_h_
#define vil_stream_url_h_
#ifdef __GNUC__
#pragma interface
#endif

//:
// \file
// \brief open an URL
// \author  fsm@robots.ox.ac.uk

#include <vil/vil_stream.h>

//: open an URL

class vil_stream_url : public vil_stream
{
  vil_stream *u_; // underlying stream
  vil_stream_url(vil_stream_url const &) { }
  void operator=(vil_stream_url const &) { }
public:
  vil_stream_url(char const *);
  ~vil_stream_url();

  // implement virtual vil_stream interface:
  bool ok() const { return u_ && u_->ok(); }
  int write(void const *buf, int n) { return u_ ? u_->write(buf, n) : 0; }
  // strictly speaking, writes should fail, but that isn't useful in any way.
  int read(void *buf, int n) { return u_ ? u_->read(buf, n) : 0; }
  int tell() { return u_ ? u_->tell() : -1; }
  void seek(int position) { if (u_) u_->seek(position); }
};

#endif
