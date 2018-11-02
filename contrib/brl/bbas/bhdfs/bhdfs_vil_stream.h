// This is brl/bbas/bhdfs/bhdfs_vil_stream.h
#ifndef bhdfs_vil_stream_h_
#define bhdfs_vil_stream_h_
//:
// \file
// \brief A vil_stream implementation using std::fstream
// \author    awf@robots.ox.ac.uk
// \date 16 Feb 00

#include <vil/vil_stream.h>
#include "bhdfs_fstream.h"
#include <vxl_config.h>

//: A vil_stream implementation using bhdfs_fstream
class bhdfs_vil_stream : public vil_stream
{
 public:
  //: supported flags are "r" (read only), "w" (for write and truncate), (append is not supported by libdhfs yet)
  bhdfs_vil_stream(char const* filename, char const* mode);

#if defined(_WIN32) && VXL_USE_WIN_WCHAR_T
  //: supported flags are "r" (read only), "w" (for write and truncate), (append is not supported by libdhfs yet)
  bhdfs_vil_stream(wchar_t const* filename, char const* mode);
#endif

  // implement virtual vil_stream interface:
  bool ok() const { return f_->ok(); }
  vil_streampos write(void const* buf, vil_streampos n);
  vil_streampos read(void* buf, vil_streampos n);
  vil_streampos tell() const;
  void seek(vil_streampos position);

  vil_streampos file_size() const;

 protected:
  ~bhdfs_vil_stream();

 private:
  bhdfs_fstream_sptr f_;
};

#endif // bhdfs_vil_stream_h_
