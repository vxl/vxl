// This is vxl/vil/vil_stream_core.cxx

// @author fsm@robots.ox.ac.uk

#ifdef __GNUC__
#pragma implementation
#endif
#include "vil_stream_core.h"

#include <vcl_cassert.h>

vil_stream_core::~vil_stream_core()
{
  for (unsigned i=0; i<block_.size(); ++i)
    delete [] block_[i];
  block_.clear();
}

//--------------------------------------------------------------------------------

int vil_stream_core::read (void       *buf, int n)
{
  assert(n>=0);

  int rv = m_transfer((char*)buf, curpos_, n, true );
  curpos_ += rv;
  return rv;
}

int vil_stream_core::write(void const *buf, int n)
{
  assert(n>=0);
  int rv = m_transfer((char*)buf, curpos_, n, false); // const violation!
  curpos_ += rv;
  return rv;
}

//--------------------------------------------------------------------------------

int vil_stream_core::m_transfer(char *buf, int pos, int n, bool read)
{
  assert(n>=0);
  assert(pos>=0);

  if (read)
  {
    if (n+pos > int(tailpos_))
    {
      if (pos > int(tailpos_))
        n = 0;
      else
        n = tailpos_ - pos;
    }
    if (n==0) return 0;
  }
  else
    // chunk up to the required size :
    while (int(blocksize_)*int(block_.size()) < pos+n)
      block_.push_back(new char [blocksize_]);

  // transfer data
  {
    char     *tbuf = buf;
    unsigned  tpos = pos;
    unsigned  tn   = n;
    while (tn>0) {
      unsigned bl = tpos/blocksize_;     // which block
      unsigned s = tpos - blocksize_*bl; // start index in block_
      unsigned z = ((tn > blocksize_-s) ? blocksize_-s : tn); // number of bytes to write
      char *tmp = block_[bl];
      if (read)
        for (unsigned k=0; k<z; ++k)
          tbuf[k] = tmp[s+k]; // prefer memcpy ?
      else
      {
        assert (s+z <= blocksize_);
        for (unsigned k=0; k<z; ++k)
          tmp[s+k] = tbuf[k]; // prefer memcpy ?
      }
      tbuf += z;
      tn   -= z;
      tpos += z;
    }
  }

  // update tailpos_
  if (int(tailpos_) < pos+n)
    tailpos_ = pos+n;

  // always succeed.
  return n;
}
