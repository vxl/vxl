// This is core/vil/vil_stream_url.h
#ifndef vil_stream_url_h_
#define vil_stream_url_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief open an URL
// \author  fsm

#include <vil/vil_stream.h>

//: open an URL

class vil_stream_url : public vil_stream
{
  vil_stream *u_; // underlying stream

  // Make private to prevent use.
  vil_stream_url(vil_stream_url const &): vil_stream() { }
  void operator=(vil_stream_url const &) { }
 public:
  vil_stream_url(char const *);
  ~vil_stream_url();

  // implement virtual vil_stream interface:
  bool ok() const { return u_ && u_->ok(); }
  vil_streampos write(void const *buf, vil_streampos n) { return u_ ? u_->write(buf, n) : 0L; }
  // strictly speaking, writes should fail, but that isn't useful in any way.
  vil_streampos read(void *buf, vil_streampos n) { return u_ ? u_->read(buf, n) : 0L; }
  vil_streampos tell() { return u_ ? u_->tell() : (vil_streampos)(-1L); }
  void seek(vil_streampos position) { if (u_) u_->seek(position); }

  vil_streampos file_size() { return u_ ? u_->file_size() : 0L; }
};

#endif // vil_stream_url_h_
