// This is contrib/mul/vil2/vil2_stream_url.h
#ifndef vil2_stream_url_h_
#define vil2_stream_url_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief open an URL
// \author  fsm@robots.ox.ac.uk

#include <vil2/vil2_stream.h>

//: open an URL

class vil2_stream_url : public vil2_stream
{
  vil2_stream *u_; // underlying stream
  vil2_stream_url(vil2_stream_url const &) { }
  void operator=(vil2_stream_url const &) { }
 public:
  vil2_stream_url(char const *);
  ~vil2_stream_url();

  // implement virtual vil2_stream interface:
  bool ok() const { return u_ && u_->ok(); }
  vil2_streampos write(void const *buf, vil2_streampos n) { return u_ ? u_->write(buf, n) : 0L; }
  // strictly speaking, writes should fail, but that isn't useful in any way.
  vil2_streampos read(void *buf, vil2_streampos n) { return u_ ? u_->read(buf, n) : 0L; }
  vil2_streampos tell() { return u_ ? u_->tell() : (vil2_streampos)(-1L); }
  void seek(vil2_streampos position) { if (u_) u_->seek(position); }
};

#endif // vil2_stream_url_h_
