#ifndef vil_stream_core_h_
#define vil_stream_core_h_
#ifdef __GNUC__
#pragma interface
#endif
// This is vxl/vil/vil_stream_core.h


//:
// \file
// \brief An in-core vil_stream implementation
// \author  fsm@robots.ox.ac.uk

#include <vcl_vector.h>
#include <vil/vil_stream.h>

//:
// An in-core vil_stream implementation.
// This is an infinite stream - reads past the last point
// written will succeed but will return garbage data.
struct vil_stream_core : vil_stream
{
  vil_stream_core(unsigned block_size = 16384);

  //: get current file size
  unsigned size() const;

  // this does not change the current position
  int m_transfer(char *buf, int pos, int n, bool read);

  //: implement virtual interface
  bool ok();
  int  read (void       *buf, int n);
  int  write(void const *buf, int n);
  int  tell();
  void seek(int position);

protected:
  ~vil_stream_core();

private:
  int curpos;       // current file pointer.

  unsigned blocksize;
  vcl_vector<char*> block;
  unsigned tailpos; // size of file so far
};

#endif // vil_stream_core_h_
