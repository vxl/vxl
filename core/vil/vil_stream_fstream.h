// This is core/vil/vil_stream_fstream.h
#ifndef vil_stream_fstream_h_
#define vil_stream_fstream_h_

#include <fstream>
#ifdef VIL_USE_FSTREAM64
#include <vil/vil_stream_fstream64.h>
#endif //VIL_USE_FSTREAM64

//:
// \file
// \brief A vil_stream implementation using std::fstream
// \author    awf@robots.ox.ac.uk
// \date 16 Feb 00

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_stream.h>
#include <vxl_config.h>

//: A vil_stream implementation using std::fstream
class vil_stream_fstream : public vil_stream
{
 public:
  vil_stream_fstream(char const* filename, char const* mode);

#if defined(_WIN32) && VXL_USE_WIN_WCHAR_T
  vil_stream_fstream(wchar_t const* filename, char const* mode);
#endif

  // implement virtual vil_stream interface:
  bool ok() const override { return f_.good(); }
  vil_streampos write(void const* buf, vil_streampos n) override;
  vil_streampos read(void* buf, vil_streampos n) override;
  vil_streampos tell() const override;
  void seek(vil_streampos position) override;

  vil_streampos file_size() const override;

 protected:
  ~vil_stream_fstream() override;

  // There are a (very few) occasions when access to the underlying stream is useful
  std::fstream & underlying_stream() {return f_;}
 private:
  std::ios::openmode flags_;
  mutable std::fstream f_;
  int id_;
  mutable vil_streampos end_;
};

#endif // vil_stream_fstream_h_
