// This is core/vil/vil_stream_section.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm

#include "vil_stream_section.h"
#include <vcl_cassert.h>
#include <vcl_iostream.h>

// underlying_: pointer to underlying stream.
// begin_     : start of section in the underlying stream.
// end_       : end of section in the underlying stream. -1 if there is no (explicit) end.
// current_   : current position (in the underlying stream) of the adapted stream.

vil_stream_section::vil_stream_section(vil_stream *underlying, int begin)
  : underlying_(underlying)
  , begin_(begin)
  , end_((vil_streampos)(-1L))
  , current_(begin)
{
  assert(underlying != 0);
  assert(begin >= 0);
  underlying_->ref();
}

vil_stream_section::vil_stream_section(vil_stream *underlying, int begin, int end)
  : underlying_(underlying)
  , begin_(begin)
  , end_(end)
  , current_(begin)
{
  assert(underlying != 0);
  assert(begin >= 0);
  assert(begin <= end);
  underlying->ref();
}

vil_stream_section::~vil_stream_section()
{
  // unreffing the underlying stream might cause deletion of *this, so
  // zero out the pointer first.
  vil_stream *u = underlying_;
  underlying_ = 0;
  u->unref();
}

vil_streampos vil_stream_section::write(void const* buf, vil_streampos n)
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

  vil_streampos nb = underlying_->write(buf, n);
  if (nb != -1L)
    current_ += nb;
  return nb;
}

vil_streampos vil_stream_section::read(void* buf, vil_streampos n)
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

  vil_streampos nb = underlying_->read(buf, n);
  if (nb != -1L)
    current_ += nb;
  return nb;
}

void vil_stream_section::seek(vil_streampos position)
{
  assert(position >= 0); // I would want to be told about this.

  if (end_ != -1L  &&  begin_ + position > end_) {
    vcl_cerr << __FILE__ << ": attempt to seek past given section (failed).\n";
    return;
  }
  else
    current_ = begin_ + position;
}

vil_streampos vil_stream_section::file_size()
{
  return end_ >= begin_ ? end_ - begin_ : underlying_->file_size() - begin_;
}
