// This is core/vil1/vil1_stream_fstream.h
#ifndef vil1_stream_fstream_h_
#define vil1_stream_fstream_h_
//:
// \file
// \brief A vil1_stream implementation using std::fstream
// \author    awf@robots.ox.ac.uk
// \date 16 Feb 00

#include <fstream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil1/vil1_stream.h>

//: A vil1_stream implementation using std::fstream
class vil1_stream_fstream : public vil1_stream
{
 public:
  vil1_stream_fstream(char const* filename, char const* mode);

  // implement virtual vil1_stream interface:
  bool ok() const override { return f_.good(); }
  vil1_streampos write(void const* buf, vil1_streampos n) override;
  vil1_streampos read(void* buf, vil1_streampos n) override;
  vil1_streampos tell() const override;
  void seek(vil1_streampos position) override;

 protected:
  ~vil1_stream_fstream() override;

 private:
  std::ios::openmode flags_;
  mutable std::fstream f_;
  int id_;
};

#endif // vil1_stream_fstream_h_
