// This is core/vil/vil_stream_section.h
#ifndef vil_stream_section_h_
#define vil_stream_section_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief make a section of a vil_stream behave like a vil_stream
// \author  fsm

#include <vil/vil_stream.h>

//: make a section of a vil_stream behave like a vil_stream.
//
// It is possible to have multiple vil_stream_sections using the same
// underlying stream simultaneously. This is accomplished by keeping
// a note of the current position and seeking a lot.
//
// Note however that this is *not* threadsafe.
struct vil_stream_section : public vil_stream
{
  //:
  // skip to position 'begin' in underlying stream and translate seeks,
  // reads and writes relative to that position into seeks, reads and
  // writes in the underlying stream.
  vil_stream_section(vil_stream *underlying, int begin);

  //:
  // as above, but will not allow seeks, reads or writes past 'end'.
  vil_stream_section(vil_stream *underlying, int begin, int end);

  // implement virtual vil_stream interface:
  bool ok() const { return underlying_->ok(); }
  vil_streampos write(void const* buf, vil_streampos n);
  vil_streampos read(void* buf, vil_streampos n);
  vil_streampos  tell() { return current_; } // regardless of what the underlying stream is doing.
  void seek(vil_streampos position);

  vil_streampos file_size();

 protected:
  ~vil_stream_section();

 private:
  vil_stream *underlying_;
  vil_streampos begin_;
  vil_streampos end_;
  vil_streampos current_;
};

#endif // vil_stream_section_h_
