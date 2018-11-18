// This is brl/bbas/bhdfs/bhdfs_fstream.cxx

#include "bhdfs_fstream.h"

int modeflags(char const* mode)
{
  if (*mode == 0)
    return 0;

  if (*mode == 'r')
    return O_RDONLY;

  if (*mode == 'w')
    return O_WRONLY | O_TRUNC;

#if 0 // append mode is not supported by libhdfs, the GNU C flag is as follows if supported
  if (*mode == 'a')
    return O_WRONLY | O_APPEND;
#endif
  std::cerr << std::endl << __FILE__ ": DODGY MODE " << mode << std::endl;
  return 0;
}

bhdfs_fstream::bhdfs_fstream(std::string fn, char const* flags) : fname_(fn)
{
  if (!bhdfs_manager::exists()) {
    std::cerr << "bhdfs_manager instance does not exist! check hdfs connection!\n";
    throw 0;
  }

  f_ = hdfsOpenFile(bhdfs_manager::instance()->fs_, fn.c_str(), modeflags(flags), 0, 0, 0);

  if (!f_)  {
      std::cerr << "Failed to open " << fn << " for writing!\n";
      throw 0;
  }
}

bhdfs_fstream::~bhdfs_fstream()
{
  if (ok() && hdfsCloseFile(bhdfs_manager::instance()->fs_, f_) < 0)
    std::cerr << "warning: in bhdfs_fstream::close() - cannot close " << fname_ << std::endl;
}

bool bhdfs_fstream::close()
{
  int val = hdfsCloseFile(bhdfs_manager::instance()->fs_, f_);
  if (val < 0)
    std::cerr << "warning: in bhdfs_fstream::close() - cannot close " << fname_ << std::endl;
  f_ = 0; // now ok() returns false
  return val != -1;
}

bhdfs_streampos bhdfs_fstream::write(void const* buf, bhdfs_streampos n)
{
  tSize num_written_bytes = hdfsWrite(bhdfs_manager::instance()->fs_, f_, buf, n);
  if (hdfsFlush(bhdfs_manager::instance()->fs_, f_))  {
     std::cerr << "Failed to 'flush' " << fname_ << '\n';
     throw 0;
   }
  return (bhdfs_streampos)num_written_bytes;
}

bhdfs_streampos bhdfs_fstream::read(void* buf, bhdfs_streampos n)
{
  bhdfs_streampos read_n = (bhdfs_streampos)hdfsRead(bhdfs_manager::instance()->fs_, f_, buf, tSize(n));
  return read_n;
}

bhdfs_streampos bhdfs_fstream::tell() const
{
  return (bhdfs_streampos)hdfsTell(bhdfs_manager::instance()->fs_, f_);
}

void bhdfs_fstream::seek(bhdfs_streampos pos)
{
  int val = hdfsSeek(bhdfs_manager::instance()->fs_, f_, tOffset(pos));
  if (val < 0)
    std::cerr << "warning: in bhdfs_fstream::seek() - cannot seek " << fname_ << " to pos: " << pos << std::endl;
}

// caution: libhdfs does not always return actual size, so may only return 0
bhdfs_streampos bhdfs_fstream::file_size() const
{
  hdfsFileInfo* info = hdfsGetPathInfo(bhdfs_manager::instance()->fs_, fname_.c_str());
  return info[0].mSize;
  hdfsFreeFileInfo(info, 1);
}
