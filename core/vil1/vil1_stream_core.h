// This is vxl/vil/vil_stream_core.h
#ifndef vil_stream_core_h_
#define vil_stream_core_h_
#ifdef __GNUC__
#pragma interface
#endif

//:
// \file
// \brief An in-core vil_stream implementation
// \author  fsm@robots.ox.ac.uk

#include <vcl_vector.h>
#include <vil/vil_stream.h>

//: An in-core vil_stream implementation.
// This is an infinite stream - reads past the last point
// written will succeed but will return garbage data.
class vil_stream_core : public vil_stream
{
  int curpos_;       // current file pointer.
  unsigned blocksize_;
  vcl_vector<char*> block_;
  unsigned tailpos_; // size of file so far

public:
  vil_stream_core(unsigned block_size = 16384)
    : curpos_(0), blocksize_(block_size), tailpos_(0) {}

  //: get current file size
  unsigned size() const { return tailpos_; }

  // this does not change the current position
  int m_transfer(char *buf, int pos, int n, bool read);

  // implement virtual vil_stream interface:
  bool ok() const { return true; }
  int  read (void       *buf, int n);
  int  write(void const *buf, int n);
  int  tell()             { return curpos_; }
  void seek(int position) { curpos_ = position; }

protected:
  ~vil_stream_core();
};

#endif // vil_stream_core_h_
