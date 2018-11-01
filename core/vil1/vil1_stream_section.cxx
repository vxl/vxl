// This is core/vil1/vil1_stream_section.cxx
//:
// \file
// \author fsm

#include <iostream>
#include "vil1_stream_section.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// underlying_: pointer to underlying stream.
// begin_     : start of section in the underlying stream.
// end_       : end of section in the underlying stream. -1 if there is no (explicit) end.
// current_   : current position (in the underlying stream) of the adapted stream.

vil1_stream_section::vil1_stream_section(vil1_stream *underlying, int begin)
  : underlying_(underlying)
  , begin_(begin)
  , end_((vil1_streampos)(-1L))
  , current_(begin)
{
  assert(underlying != nullptr);
  assert(begin >= 0);
  underlying_->ref();
}

vil1_stream_section::vil1_stream_section(vil1_stream *underlying, int begin, int end)
  : underlying_(underlying)
  , begin_(begin)
  , end_(end)
  , current_(begin)
{
  assert(underlying != nullptr);
  assert(begin >= 0);
  assert(begin <= end);
  underlying->ref();
}

vil1_stream_section::~vil1_stream_section()
{
  // unreffing the underlying stream might cause deletion of *this, so
  // zero out the pointer first.
  vil1_stream *u = underlying_;
  underlying_ = nullptr;
  u->unref();
}

vil1_streampos vil1_stream_section::write(void const* buf, vil1_streampos n)
{
  assert(n >= 0); // wouldn't you want to be told?

  // huh? this should never happen, even if someone else is
  // manipulating the underlying stream too.
  assert(begin_<=current_);
  if (end_ != -1L)
    assert(current_<=end_);

  // shrink given buffer so it fits into our section.
  if (end_ != -1L  &&  current_ + n > end_)
    n = end_ - current_;

  // seek to where we have been telling the clients we are.
  underlying_->seek(current_);

  // this could be a bug in the caller's code or merely a
  // failure to seek on underlying stream.
  assert(underlying_->tell() == current_);

  vil1_streampos nb = underlying_->write(buf, n);
  if (nb != -1L)
    current_ += nb;
  return nb;
}

vil1_streampos vil1_stream_section::read(void* buf, vil1_streampos n)
{
  assert(n >= 0); // wouldn't you want to be told?

  // huh? this should never happen, even if someone else is
  // manipulating the underlying stream too.
  assert(begin_<=current_);
  if (end_ != -1L)
    assert(current_<=end_);

  // shrink given buffer so it fits into our section.
  if (end_ != -1L  &&  current_ + n > end_)
    n = end_ - current_;

  // seek to where we have been telling the clients we are.
  underlying_->seek(current_);

  // this could be a bug in the caller's code or merely a
  // failure to seek on underlying stream.
  assert(underlying_->tell() == current_);

  vil1_streampos nb = underlying_->read(buf, n);
  if (nb != -1L)
    current_ += nb;
  return nb;
}

void vil1_stream_section::seek(vil1_streampos position)
{
  assert(position >= 0); // I would want to be told about this.

  if (end_ != -1L  &&  begin_ + position > end_) {
    std::cerr << __FILE__ << ": attempt to seek past given section (failed).\n";
    return;
  }
  else
    current_ = begin_ + position;
}
