// This is core/vil1/vil1_stream_url.h
#ifndef vil1_stream_url_h_
#define vil1_stream_url_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief open an URL
// \author  fsm

#include <vil1/vil1_stream.h>

//: open an URL

class vil1_stream_url : public vil1_stream
{
  vil1_stream *u_; // underlying stream
  // Make private to prevent use.
  vil1_stream_url(vil1_stream_url const &): vil1_stream() { }
  void operator=(vil1_stream_url const &) { }
 public:
  vil1_stream_url(char const *);
  ~vil1_stream_url();

  // implement virtual vil1_stream interface:
  bool ok() const { return u_ && u_->ok(); }
  vil1_streampos write(void const *buf, vil1_streampos n) { return u_ ? u_->write(buf, n) : 0L; }
  // strictly speaking, writes should fail, but that isn't useful in any way.
  vil1_streampos read(void *buf, vil1_streampos n) { return u_ ? u_->read(buf, n) : 0L; }
  vil1_streampos tell() { return u_ ? u_->tell() : (vil1_streampos)(-1L); }
  void seek(vil1_streampos position) { if (u_) u_->seek(position); }
};

#endif // vil1_stream_url_h_
