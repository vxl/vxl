/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "vil_stream_section.h"
#include <vcl/vcl_compiler.h>
#include <vcl/vcl_cassert.h>
#include <vcl/vcl_iostream.h>

vil_stream_section::vil_stream_section(vil_stream *underlying, int pos, int n VCL_DEFAULT_VALUE(-1))
  : base(underlying)
  , start(pos)
  , length(n)
  , curpos(0)
{
  assert(base);
  base->seek(pos);
  if (base->tell() != pos) {
    cerr << "underlying stream cannot seek to given position" << endl;
    assert(false);
  }
}

vil_stream_section::~vil_stream_section() {
}

//--------------------------------------------------------------------------------

int vil_stream_section::transfer(void *buf, int n, bool read) {
  if (n<0) // error
    return -1;

  // do not allow transfer past the end :
  if (length != -1 && curpos+n > length)
    n = length-curpos;
  
  //
  base->seek(start + curpos);
  int rv = read ? base->read(buf, n) : base->write(buf, n);
  
  if (rv<0) // failure
    return -1;

  // success
  curpos += rv;
  return rv;
}

int vil_stream_section::read (void       *buf, int n) {
  return transfer(buf, n, true);
}

int vil_stream_section::write(void const *buf, int n) {
  return transfer((void*)buf, n, false);
}

int vil_stream_section::tell() {
  return curpos;
}

void vil_stream_section::seek(int position) {
  assert(length == -1 || (0<=position && position<=length));
  curpos = position;
}
