// This is core/vil1/vil1_stream_core.cxx
// \author fsm

#include "vil1_stream_core.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

vil1_stream_core::~vil1_stream_core()
{
  for (auto & i : block_)
    delete [] i;
  block_.clear();
}

//--------------------------------------------------------------------------------

vil1_streampos vil1_stream_core::read (void *buf, vil1_streampos n)
{
  assert(n>=0);

  vil1_streampos rv = m_transfer((char*)buf, curpos_, n, true );
  curpos_ += rv;
  return rv;
}

vil1_streampos vil1_stream_core::write(void const *buf, vil1_streampos n)
{
  assert(n>=0);
  vil1_streampos rv = m_transfer((char*)(const_cast<void*>(buf)), curpos_, n, false); // const violation!
  curpos_ += rv;
  return rv;
}

//--------------------------------------------------------------------------------

vil1_streampos vil1_stream_core::m_transfer(char *buf, vil1_streampos pos, vil1_streampos n, bool read)
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
    vil1_streampos tpos = pos;
    vil1_streampos tn   = n;
    while (tn>0) {
      vil1_streampos bl = tpos/(long)blocksize_;     // which block
      vil1_streampos s = tpos - (long)blocksize_*bl; // start index in block_
      vil1_streampos z = ((tn+s > (long)blocksize_) ? (long)blocksize_-s : tn); // number of bytes to write
      char *tmp = block_[bl];
      if (read)
        for (vil1_streampos k=0; k<z; ++k)
          tbuf[k] = tmp[s+k]; // prefer memcpy ?
      else
      {
        assert (s+z <= (long)blocksize_);
        for (vil1_streampos k=0; k<z; ++k)
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
