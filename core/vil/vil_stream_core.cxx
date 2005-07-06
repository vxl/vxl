// This is core/vil/vil_stream_core.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm

#include "vil_stream_core.h"
#include <vcl_cassert.h>
#include <vcl_limits.h> 

vil_stream_core::vil_stream_core(unsigned block_size)
    : curpos_(0), blocksize_(block_size), tailpos_(0) 
{ }

//--------------------------------------------------------------------------------

vil_stream_core::~vil_stream_core()
{
  for (unsigned i=0; i<block_.size(); ++i)
    delete [] block_[i];
  block_.clear();
}

//--------------------------------------------------------------------------------

vil_streampos vil_stream_core::read (void *buf, vil_streampos n)
{
  assert(n>=0);

  vil_streampos rv = m_transfer((char*)buf, curpos_, n, true );
  curpos_ += rv;
  return rv;
}

//--------------------------------------------------------------------------------

vil_streampos vil_stream_core::write(void const *buf, vil_streampos n)
{
  assert(n>=0);
  vil_streampos rv = m_transfer(static_cast<char*>(const_cast<void *>(buf)), curpos_, n, false);
  curpos_ += rv;
  return rv;
}

//--------------------------------------------------------------------------------

vil_streampos vil_stream_core::m_transfer(char *buf, vil_streampos pos, vil_streampos n, bool read)
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
    vil_streampos tpos = pos;
    vil_streampos tn   = n;
    while (tn>0) {
      vil_streampos bl = tpos/(long)blocksize_;     // which block
      vil_streampos s = tpos - (long)blocksize_*bl; // start index in block_
      vil_streampos z = ((tn+s > (long)blocksize_) ? (long)blocksize_-s : tn); // number of bytes to write
      //it would take a very large in-memory stream for this assert to fail (>2GB).
      //That should not happen.  If it does, then we have to think of plan b.
      assert( bl <= vcl_numeric_limits< size_t >::max() );
      char *tmp = block_[(size_t)bl];
      if (read)
        for (vil_streampos k=0; k<z; ++k)
          tbuf[k] = tmp[s+k]; // prefer memcpy ?
      else
      {
        assert (s+z <= (long)blocksize_);
        for (vil_streampos k=0; k<z; ++k)
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
