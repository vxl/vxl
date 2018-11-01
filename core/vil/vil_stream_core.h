// This is core/vil/vil_stream_core.h
#ifndef vil_stream_core_h_
#define vil_stream_core_h_
//:
// \file
// \brief An in-core vil_stream implementation
// \author  fsm

#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_stream.h>

//: An in-core vil_stream implementation.
// This is an infinite stream - reads past the last point
// written will succeed but will return garbage data.
class vil_stream_core : public vil_stream
{
  vil_streampos curpos_;       // current file pointer.
  unsigned blocksize_;
  std::vector<char*> block_;
  vil_streampos tailpos_; // size of file so far

 public:
  vil_stream_core(unsigned block_size = 16384);

  //: get current file size
  vil_streampos size() const { return tailpos_; }

  //: Read or write n bytes at position pos.
  // This does not change the current position.
  // When read=false, buf is actually a "char const *".
  vil_streampos m_transfer(char *buf, vil_streampos pos, vil_streampos n, bool read);

  // implement virtual vil_stream interface:
  bool ok() const override { return true; }
  vil_streampos read (void       *buf, vil_streampos n) override;
  vil_streampos write(void const *buf, vil_streampos n) override;
  vil_streampos tell()     const override    { return curpos_; }
  void seek(vil_streampos position) override { curpos_ = position; }

  vil_streampos file_size() const override { return tailpos_; }

 protected:
  ~vil_stream_core() override;
};

#endif // vil_stream_core_h_
