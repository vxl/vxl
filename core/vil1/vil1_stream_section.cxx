/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation "vil_stream_section"
#endif
#include "vil_stream_section.h"
#include <vcl/vcl_cassert.h>
#include <vcl/vcl_iostream.h>

// underlying: pointer to underlying stream.
// begin     : start of section in the underlying stream.
// end       : end of section in the underlying stream. -1 if there is no (explicit) end.
// current   : current position (in the underlying stream) of the adapted stream.

vil_stream_section::vil_stream_section(vil_stream *underlying_, int begin_)
  : underlying(underlying_)
  , begin(begin_)
  , end(-1)
  , current(begin_)
{
  assert(underlying);
  assert(begin >= 0);
  underlying->ref();
}

vil_stream_section::vil_stream_section(vil_stream *underlying_, int begin_, int end_)
  : underlying(underlying_)
  , begin(begin_)
  , end(end_)
  , current(begin_)
{
  assert(underlying);
  assert(begin >= 0);
  assert(begin <= end);
  underlying->ref();
}

vil_stream_section::~vil_stream_section()
{
  // unreffing the underlying stream might cause deletion of *this, so
  // zero out the pointer first.
  vil_stream *u = underlying;
  underlying = 0;
  u->unref();
}

int vil_stream_section::write(void const* buf, int n)
{
  assert(n >= 0); // wouldn't you want to be told?

  // huh? this should never happen, even if someone else is
  // manipulating the underlying stream too.
  assert(begin<=current);
  if (end != -1)
    assert(current<=end);
  
  // shrink given buffer so it fits into our section.
  if (end != -1  &&  current + n > end)
    n = end - current;
  
  // seek to where we have been telling the clients we are.
  underlying->seek(current);
  
  // this could be a bug in the caller's code or merely a
  // failure to seek on underlying stream.
  assert(underlying->tell() == current);

  int nb = underlying->write(buf, n);
  if (nb >= 0)
    current += nb;
  return nb;
}

int vil_stream_section::read(void* buf, int n)
{
  assert(n >= 0); // wouldn't you want to be told?

  // huh? this should never happen, even if someone else is
  // manipulating the underlying stream too.
  assert(begin<=current);
  if (end != -1)
    assert(current<=end);
  
  // shrink given buffer so it fits into our section.
  if (end != -1  &&  current + n > end)
    n = end - current;
  
  // seek to where we have been telling the clients we are.
  underlying->seek(current);

  // this could be a bug in the caller's code or merely a
  // failure to seek on underlying stream.
  assert(underlying->tell() == current);

  int nb = underlying->read(buf, n);
  if (nb >= 0)
    current += nb;
  return nb;
}

int vil_stream_section::tell()
{
  return current; // regardless of what the underlying stream is doing.
}

void vil_stream_section::seek(int position)
{
  assert(position >= 0); // I would want to be told about this.
 
  if (end != -1  &&  begin + position > end) {
    cerr << __FILE__ << ": attempt to seek past given section (failed)." << endl;
    return;
  }
  else
    current = begin + position;
}
