// This is contrib/mul/vil2/vil2_stream_core.cxx

// @author fsm@robots.ox.ac.uk

#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
#include "vil2_stream_core.h"

#include <vcl_cassert.h>

vil2_stream_core::~vil2_stream_core()
{
  for (unsigned i=0; i<block_.size(); ++i)
    delete [] block_[i];
  block_.clear();
}

//--------------------------------------------------------------------------------

vil2_streampos vil2_stream_core::read (void *buf, vil2_streampos n)
{
  assert(n>=0);

  vil2_streampos rv = m_transfer((char*)buf, curpos_, n, true );
  curpos_ += rv;
  return rv;
}

vil2_streampos vil2_stream_core::write(void const *buf, vil2_streampos n)
{
  assert(n>=0);
  vil2_streampos rv = m_transfer(static_cast<char*>(const_cast<void *>(buf)), curpos_, n, false);
  curpos_ += rv;
  return rv;
}

//--------------------------------------------------------------------------------

vil2_streampos vil2_stream_core::m_transfer(char *buf, vil2_streampos pos, vil2_streampos n, bool read)
{
  assert(n>=0);
  assert(pos>=0);

  if (read)
  {
    if (pos+n > tailpos_)
    {
      if (pos > tailpos_)
        n = 0;
      else
        n = tailpos_ - pos;
    }
    if (n==0L) return 0;
  }
  else
    // chunk up to the required size :
    while (blocksize_*block_.size() < (unsigned long)(pos+n))
      block_.push_back(new char [blocksize_]);

  // transfer data
  {
    char         *tbuf = buf;
    vil2_streampos tpos = pos;
    vil2_streampos tn   = n;
    while (tn>0) {
      vil2_streampos bl = tpos/(long)blocksize_;     // which block
      vil2_streampos s = tpos - (long)blocksize_*bl; // start index in block_
      vil2_streampos z = ((tn+s > (long)blocksize_) ? (long)blocksize_-s : tn); // number of bytes to write
      char *tmp = block_[bl];
      if (read)
        for (vil2_streampos k=0; k<z; ++k)
          tbuf[k] = tmp[s+k]; // prefer memcpy ?
      else
      {
        assert (s+z <= (long)blocksize_);
        for (vil2_streampos k=0; k<z; ++k)
          tmp[s+k] = tbuf[k]; // prefer memcpy ?
      }
      tbuf += z;
      tn   -= z;
      tpos += z;
    }
  }

  // update tailpos_
  if (tailpos_ < pos+n)
    tailpos_ = pos+n;

  // always succeed.
  return n;
}
