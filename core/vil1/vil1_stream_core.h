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
  vil_streampos curpos_;       // current file pointer.
  unsigned blocksize_;
  vcl_vector<char*> block_;
  vil_streampos tailpos_; // size of file so far

public:
  vil_stream_core(unsigned block_size = 16384)
    : curpos_(0), blocksize_(block_size), tailpos_(0) {}

  //: get current file size
  unsigned size() const { return tailpos_; }

  // this does not change the current position
  vil_streampos m_transfer(char *buf, vil_streampos pos, vil_streampos n, bool read);

  // implement virtual vil_stream interface:
  bool ok() const { return true; }
  vil_streampos read (void       *buf, vil_streampos n);
  vil_streampos write(void const *buf, vil_streampos n);
  vil_streampos tell()              { return curpos_; }
  void seek(vil_streampos position) { curpos_ = position; }

protected:
  ~vil_stream_core();
};

#endif // vil_stream_core_h_
