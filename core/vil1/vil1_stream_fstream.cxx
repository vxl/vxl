#ifdef __GNUC__
#pragma implementation "vil_stream_fstream.h"
#endif

#include <assert.h>
#include <vil/vil_stream_fstream.h>

static int modeflags(char const* mode)
{
  if (*mode == 0)
    return 0;

  if (*mode == 'r') 
    return ios::in | modeflags(mode+1);
  
  if (*mode == 'w')
    return ios::out | modeflags(mode+1);

  cerr << "DODGY MODE " << mode << endl;
  return 0;
}

vil_stream_fstream::vil_stream_fstream(char const* fn, char const* mode):
  f_(fn, modeflags(mode))
{
}

//vil_stream_fstream::vil_stream_fstream(fstream& f):
//  f_(f.rdbuf()->fd())
//{
//}

vil_stream_fstream::~vil_stream_fstream()
{
}

int vil_stream_fstream::write(void const* buf, int n)
{
  streampos a = tell();
  f_.write((char const*)buf, n);
  streampos b = tell();
  return b-a;
  //return f_.good();
}


int vil_stream_fstream::read(void* buf, int n)
{
  streampos a = tell();
  f_.read((char *)buf, n);
  streampos b = tell();
  return b-a;
  //return f_.good();
}

int vil_stream_fstream::tell()
{
  return f_.tellg();
}

void vil_stream_fstream::seek(int position)
{
  if (position != f_.tellg()) {
    f_.seekg(position);
    assert(f_.good());
  }
}
