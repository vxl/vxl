/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "vil_stream_core.h"

#include <vcl_cassert.h>

vil_stream_core::vil_stream_core(unsigned block_size VCL_DEFAULT_VALUE(16384))
  : curpos(0)
  , blocksize(block_size)
  , tailpos(0)
{
}

vil_stream_core::~vil_stream_core()
{
  for (unsigned i=0; i<block.size(); ++i)
    delete [] block[i];
  block.clear();
}

//--------------------------------------------------------------------------------

bool vil_stream_core::ok()
{
  return true;
}

int vil_stream_core::read (void       *buf, int n)
{
  assert(n>=0);
  int rv = m_transfer((char*)buf, curpos, n, true );
  curpos += rv;
  return rv;
}

int vil_stream_core::write(void const *buf, int n)
{
  assert(n>=0);
  int rv = m_transfer((char*)buf, curpos, n, false);
  curpos += rv;
  return rv;
}

int vil_stream_core::tell()
{
  return curpos;
}

void vil_stream_core::seek(int position)
{
  curpos = position;
}

//--------------------------------------------------------------------------------

unsigned vil_stream_core::size() const
{
  return tailpos;
}

int vil_stream_core::m_transfer(char *buf, int pos, int n, bool read)
{
  assert(n>=0);
  assert(pos>=0);
  
  // chunk up to the required size :  
  while (blocksize*block.size() < pos+n)
    block.push_back(new char [blocksize]);

  // transfer data
  {
    char     *tbuf = buf;
    unsigned  tpos = pos;
    unsigned  tn   = n;
    while (tn>0) {
      unsigned bl = tpos/blocksize;     // which block
      unsigned s = tpos - blocksize*bl; // start index in block
      unsigned z = ((tn > blocksize) ? blocksize : tn); // number of bytes to write
      char *tmp = block[bl];
      if (read)
	for (unsigned k=0; k<z; ++k)
	  tbuf[k] = tmp[s+k]; // prefer memcpy ?
      else
	for (unsigned k=0; k<z; ++k)
	  tmp[s+k] = tbuf[k]; // prefer memcpy ?
      
      tbuf += z;
      tn   -= z;
    }
  }


  // update tailpos
  if (tailpos < pos+n)
    tailpos = pos+n;

  // always succeed.
  return n;
}
