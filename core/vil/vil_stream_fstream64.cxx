// This is core/vil/vil_stream_fstream64.cxx
#ifdef VIL_USE_FSTREAM64 // only compile this file when needed
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include "vil_stream_fstream64.h"
#include <vcl_cassert.h>
#include <vcl_iostream.h> //for vcl_cerr

#if defined(WIN32)
#include <io.h>
#endif
#include <fcntl.h>
#include <vcl_cstdlib.h>
#include <vcl_cstdio.h>
#include <sys/stat.h>

#ifndef WIN32
//RaR: My attempt at cross-platform compatibility
//     This stuff really isn't that important because
//     this file is only added to the configuration
//     for WIN32 builds
#define _O_RDWR O_RDWR
#define _O_RDONLY O_RDONLY
#define _O_WRONLY O_WRONLY
#define _O_BINARY 0
#define _O_CREAT O_CREAT
#define _S_IREAD S_IREAD
#define _S_IWRITE S_IWRITE
#define _open ::open
#define _close ::close
#define _telli64(fd) ::lseek(fd, 0, SEEK_CUR)
#define _lseeki64 ::lseek
#include <unistd.h>
#define _commit ::fsync
#define _read ::read
#define _write ::write
#endif

#define xerr if (true) ; else (vcl_cerr << "vcl_fstream#" << fd_ << ": ")

static int modeflags(char const* mode)
{
  bool read = false,
       write = false;

  for ( unsigned int i = 0; mode[i] != 0; ++i ) {
    if ( mode[i] == 'r' )
      read = true;
    else if ( mode[i] == 'w' )
      write = true;
  }

  if ( read && write ) return _O_RDWR;
  else if ( read ) return _O_RDONLY;
  else if ( write ) return _O_WRONLY;

  vcl_cerr << '\n' << __FILE__ ": DODGY MODE " << mode << '\n';
  return 0;
}


vil_stream_fstream64::vil_stream_fstream64(char const* fn, char const* mode) :
  mode_( modeflags(mode) )
{
  if ( mode_ == O_RDONLY ) {
    fd_ = _open( fn, mode_ | _O_BINARY  );
  } else {
    fd_ = _open( fn, mode_ | _O_CREAT | _O_BINARY, _S_IREAD | _S_IWRITE );
  }
  if ( fd_ == -1 ){
    vcl_cerr << "vil_stream_fstream::Could not open [" << fn << "]\n";
  }
}

vil_stream_fstream64::~vil_stream_fstream64()
{
  //xerr << "vil_stream_fstream64 # " << fd_ << " being deleted\n";

  if ( ok() ){
    _close( fd_ );
  }
}

vil_streampos vil_stream_fstream64::write(void const* buf, vil_streampos n)
{
  assert(ok());

  if ( !( ( mode_ == _O_WRONLY ) ||
          ( mode_ == _O_RDWR ) ) )
  {
    vcl_cerr << "vil_stream_fstream64: write failed, stream not open for write\n";
    return 0;
  }

  //cast should be ok unless trying to write >2GB (not likely)
  int ret_val = _write( fd_, buf, (unsigned int)n );
  if ( ret_val == -1 ){
    vcl_cerr << ("vil_stream_fstream64: ERROR: write failed!\n");
    return 0;
  } else {
    //apparently calling _commit is relatively slow
    //for file types like pnm whose put_view() functions
    //write() one byte at a time, it takes minutes to write a
    //single (relatively) small file.  That's why we comment
    //out this commit() (ie. flush) logic.  All tests pass,
    //so this should be ok.
    //if ( _commit( fd_ ) == -1 ){
    //  return 0;
    //} else {
      return ret_val;
    //}
  }
}


vil_streampos vil_stream_fstream64::read(void* buf, vil_streampos n)
{
  assert(ok());

  if ( !( ( mode_ == _O_RDONLY ) ||
          ( mode_ == _O_RDWR ) ) )
  {
    xerr << "vil_stream_fstream64: read failed, stream not open for read\n";
    return 0;
  }
  //cast should be ok unless trying to read >2GB
  int ret_val = _read( fd_, buf, (unsigned int)n );
  if ( ret_val == -1 )
    xerr << "read failed!\n";
  else if ( ret_val < n )
    xerr << "only read " << ret_val << vcl_endl;

  return ret_val;
}

vil_streampos vil_stream_fstream64::tell() const
{
  assert(ok());
  return _telli64( fd_ );
}

void vil_stream_fstream64::seek(vil_streampos position)
{
  assert(ok());
  long long ret_val = _lseeki64( fd_, position, SEEK_SET );
  if ( ret_val == -1L ){
    xerr << "error during seek.";
  }
}

vil_streampos vil_stream_fstream64::file_size() const
{
  vil_streampos curr = tell();
  _lseeki64( fd_, 0, SEEK_END );
  vil_streampos end = tell();
  _lseeki64( fd_, curr, SEEK_SET );
  return end;
}

#endif // VIL_USE_FSTREAM64
