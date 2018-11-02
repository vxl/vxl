// This is core/vil/vil_stream_fstream64.h
#ifndef vil_stream_fstream64_h_
#define vil_stream_fstream64_h_
#ifdef VIL_USE_FSTREAM64
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief A vil_stream implementation that can handle files bigger than 2GB
// This file is only added to the VXL build for WIN32 builds because
// MSVC compilers do not support transparent large file access via std::fstream
// (ie. vil_stream_fstream can't support large files on this platform)
//
// \author  rob@stellarscience.com
// \date 28 Jun 05

#include <vcl_compiler.h>
#include <vil/vil_stream.h>

//: A vil_stream implementation using std::fstream
class vil_stream_fstream64 : public vil_stream
{
 public:
  vil_stream_fstream64(char const* filename, char const* mode);

#if defined(_WIN32) && VXL_USE_WIN_WCHAR_T
  vil_stream_fstream64(wchar_t const* filename, char const* mode);
#endif

  // implement virtual vil_stream interface:
  bool ok() const { return fd_ != -1; }
  vil_streampos write(void const* buf, vil_streampos n);
  vil_streampos read(void* buf, vil_streampos n);
  vil_streampos tell() const;
  void seek(vil_streampos position);

  vil_streampos file_size() const;

 protected:
  ~vil_stream_fstream64();

 private:
  int fd_;
  int mode_;
};

#endif // VIL_USE_FSTREAM64
#endif // vil_stream_fstream64_h_
