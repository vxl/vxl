// This is contrib/mul/vil2/vil2_stream_core.h
#ifndef vil2_stream_core_h_
#define vil2_stream_core_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief An in-core vil2_stream implementation
// \author  fsm

#include <vcl_vector.h>
#include <vil2/vil2_stream.h>

//: An in-core vil2_stream implementation.
// This is an infinite stream - reads past the last point
// written will succeed but will return garbage data.
class vil2_stream_core : public vil2_stream
{
  vil2_streampos curpos_;       // current file pointer.
  unsigned blocksize_;
  vcl_vector<char*> block_;
  vil2_streampos tailpos_; // size of file so far

 public:
  vil2_stream_core(unsigned block_size = 16384)
    : curpos_(0), blocksize_(block_size), tailpos_(0) {}

  //: get current file size
  unsigned size() const { return tailpos_; }

  //: Read or write n bytes at position pos.
  // This does not change the current position.
  // When read=false, buf is actually a "char const *".
  vil2_streampos m_transfer(char *buf, vil2_streampos pos, vil2_streampos n, bool read);

  // implement virtual vil2_stream interface:
  bool ok() const { return true; }
  vil2_streampos read (void       *buf, vil2_streampos n);
  vil2_streampos write(void const *buf, vil2_streampos n);
  vil2_streampos tell()              { return curpos_; }
  void seek(vil2_streampos position) { curpos_ = position; }

 protected:
  ~vil2_stream_core();
};

#endif // vil2_stream_core_h_
