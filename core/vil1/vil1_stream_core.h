// This is core/vil1/vil1_stream_core.h
#ifndef vil1_stream_core_h_
#define vil1_stream_core_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief An in-core vil1_stream implementation
// \author  fsm

#include <vcl_vector.h>
#include <vil1/vil1_stream.h>

//: An in-core vil1_stream implementation.
// This is an infinite stream - reads past the last point
// written will succeed but will return garbage data.
class vil1_stream_core : public vil1_stream
{
  vil1_streampos curpos_;       // current file pointer.
  unsigned blocksize_;
  vcl_vector<char*> block_;
  vil1_streampos tailpos_; // size of file so far

 public:
  vil1_stream_core(unsigned block_size = 16384)
    : curpos_(0), blocksize_(block_size), tailpos_(0) {}

  //: get current file size
  unsigned size() const { return tailpos_; }

  //: Read or write n bytes at position pos.
  // This does not change the current position.
  // When read=false, buf is actually a "char const *".
  vil1_streampos m_transfer(char *buf, vil1_streampos pos, vil1_streampos n, bool read);

  // implement virtual vil1_stream interface:
  bool ok() const { return true; }
  vil1_streampos read (void       *buf, vil1_streampos n);
  vil1_streampos write(void const *buf, vil1_streampos n);
  vil1_streampos tell()     const    { return curpos_; }
  void seek(vil1_streampos position) { curpos_ = position; }

 protected:
  ~vil1_stream_core();
};

#endif // vil1_stream_core_h_
