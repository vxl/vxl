// This is core/vil1/vil1_stream_section.h
#ifndef vil1_stream_section_h_
#define vil1_stream_section_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief make a section of a vil1_stream behave like a vil1_stream
// \author  fsm

#include <vil1/vil1_stream.h>

//: make a section of a vil1_stream behave like a vil1_stream.
//
// It is possible to have multiple vil1_stream_sections using the same
// underlying stream simultaneously. This is accomplished by keeping
// a note of the current position and seeking a lot.
//
// Note however that this is *not* threadsafe.
struct vil1_stream_section : public vil1_stream
{
  //:
  // skip to position 'begin' in underlying stream and translate seeks,
  // reads and writes relative to that position into seeks, reads and
  // writes in the underlying stream.
  vil1_stream_section(vil1_stream *underlying, int begin);

  //:
  // as above, but will not allow seeks, reads or writes past 'end'.
  vil1_stream_section(vil1_stream *underlying, int begin, int end);

  // implement virtual vil1_stream interface:
  bool ok() const { return underlying_->ok(); }
  vil1_streampos write(void const* buf, vil1_streampos n);
  vil1_streampos read(void* buf, vil1_streampos n);
  vil1_streampos tell() const { return current_; } // regardless of what the underlying stream is doing.
  void seek(vil1_streampos position);

 protected:
  ~vil1_stream_section();

 private:
  vil1_stream *underlying_;
  vil1_streampos begin_;
  vil1_streampos end_;
  vil1_streampos current_;
};

#endif // vil1_stream_section_h_
