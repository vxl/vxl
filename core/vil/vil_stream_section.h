// This is contrib/mul/vil2/vil2_stream_section.h
#ifndef vil2_stream_section_h_
#define vil2_stream_section_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief make a section of a vil2_stream behave like a vil2_stream
// \author  fsm@robots.ox.ac.uk

#include <vil2/vil2_stream.h>

//: make a section of a vil2_stream behave like a vil2_stream.
//
// It is possible to have multiple vil2_stream_sections using the same
// underlying stream simultaneously. This is accomplished by keeping
// a note of the current position and seeking a lot.
//
// Note however that this is *not* threadsafe.
struct vil2_stream_section : public vil2_stream
{
  //:
  // skip to position 'begin' in underlying stream and translate seeks,
  // reads and writes relative to that position into seeks, reads and
  // writes in the underlying stream.
  vil2_stream_section(vil2_stream *underlying, int begin);

  //:
  // as above, but will not allow seeks, reads or writes past 'end'.
  vil2_stream_section(vil2_stream *underlying, int begin, int end);

  // implement virtual vil2_stream interface:
  bool ok() const { return underlying_->ok(); }
  vil2_streampos write(void const* buf, vil2_streampos n);
  vil2_streampos read(void* buf, vil2_streampos n);
  vil2_streampos  tell() { return current_; } // regardless of what the underlying stream is doing.
  void seek(vil2_streampos position);

 protected:
  ~vil2_stream_section();

 private:
  vil2_stream *underlying_;
  vil2_streampos begin_;
  vil2_streampos end_;
  vil2_streampos current_;
};

#endif // vil2_stream_section_h_
