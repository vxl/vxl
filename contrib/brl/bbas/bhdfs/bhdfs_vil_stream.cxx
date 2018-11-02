// This is brl/bbas/bhdfs/bhdfs_vil_stream.cxx
#include "bhdfs_vil_stream.h"
//:
// \file

//:
// supported flags are "r" (read only), "w" (for write and truncate), (append is not supported by libdhfs yet)
bhdfs_vil_stream::bhdfs_vil_stream(char const* fn, char const* mode)
{
  f_ = new bhdfs_fstream(fn, mode);
}

#if defined(_WIN32) && VXL_USE_WIN_WCHAR_T
//:
// supported flags are "r" (read only), "w" (for write and truncate), (append is not supported by libdhfs yet)
bhdfs_vil_stream::bhdfs_vil_stream(wchar_t const* fn, char const* mode)
{
  f_ = new bhdfs_fstream(fn, mode);
}
#endif //defined(_WIN32) && VXL_USE_WIN_WCHAR_T

bhdfs_vil_stream::~bhdfs_vil_stream()
{
  std::cout << "In bhdfs_vil_stream::~bhdfs_vil_stream()!\n";
  f_->close();
  f_ = 0;
}

vil_streampos bhdfs_vil_stream::write(void const* buf, vil_streampos n)
{
  return f_->write(buf, n);
}


vil_streampos bhdfs_vil_stream::read(void* buf, vil_streampos n)
{
  return f_->read(buf, n);
}

vil_streampos bhdfs_vil_stream::tell() const
{
  return f_->tell();
}

void bhdfs_vil_stream::seek(vil_streampos position)
{
  f_->seek(position);
}

vil_streampos bhdfs_vil_stream::file_size() const
{
  f_->file_size();
}
