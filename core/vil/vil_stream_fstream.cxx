// This is core/vil/vil_stream_fstream.cxx

#include <limits>
#include <iostream>
#include <ios>
#include "vil_stream_fstream.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

static std::ios::openmode modeflags(char const* mode)
{
  if (*mode == 0)
    return std::ios::openmode(0);

  if (*mode == 'r') {
    if (mode[1] == '+' || mode[1] == 'w')
      return std::ios::in | std::ios::out | modeflags(mode+2);
    else
      return std::ios::in | modeflags(mode+1);
  }

  if (*mode == 'w') {
    if (mode[1] == '+')
      return std::ios::in | std::ios::out | std::ios::trunc | modeflags(mode+2);
    else
      return std::ios::out | std::ios::trunc | modeflags(mode+1);
  }

  std::cerr << std::endl << __FILE__ ": DODGY MODE " << mode << std::endl;
  return std::ios::openmode(0);
}

#define xerr if (true) ; else (std::cerr << "std::fstream#" << id_ << ": ")

static int id = 0;

vil_stream_fstream::vil_stream_fstream(char const* fn, char const* mode):
  flags_(modeflags(mode)),
  f_(fn, flags_ | std::ios::binary), // need ios::binary on windows.
  end_( -1 )
{
  id_ = ++id;
  xerr << "vil_stream_fstream(\"" << fn << "\", \""<<mode<<"\") = " << id_ << '\n';
#if 0
  if (!f_) {
    std::cerr << "vil_stream_fstream::Could not open [" << fn << "]\n";
  }
#endif // 0
}

#if defined(_WIN32) && VXL_USE_WIN_WCHAR_T
vil_stream_fstream::vil_stream_fstream(wchar_t const* fn, char const* mode):
  flags_(modeflags(mode)),
  f_(fn, flags_ | std::ios::binary), // need ios::binary on windows.
  end_( -1 )
{
  id_ = ++id;
}
#endif //defined(_WIN32) && VXL_USE_WIN_WCHAR_T

#if 0
vil_stream_fstream::vil_stream_fstream(std::fstream& f):
  f_(f.rdbuf()->fd()),
  end_( -1 )
{
}
#endif // 0

vil_stream_fstream::~vil_stream_fstream()
{
  xerr << "vil_stream_fstream# " << id_ << " being deleted\n";
}

vil_streampos vil_stream_fstream::write(void const* buf, vil_streampos n)
{
  assert(id > 0);
  //assures that cast (below) will be ok
  assert( n <= std::numeric_limits<std::streamoff>::max() );

  if (!(flags_ & std::ios::out)) {
    std::cerr << "vil_stream_fstream: write failed, not a std::ostream\n";
    return 0;
  }

  vil_streampos a = tell();
  xerr << "write " << n << std::endl;
  f_.write((char const*)buf, (std::streamoff)n);
  if (!f_.good())
    std::cerr << ("vil_stream_fstream: ERROR: write failed!\n");
  vil_streampos b = tell();
  f_.flush();
  return b-a;
}


vil_streampos vil_stream_fstream::read(void* buf, vil_streampos n)
{
  assert(id > 0);
  //assures that cast (below) will be ok
  assert( n <= std::numeric_limits<std::streamoff>::max() );

  if (!(flags_ & std::ios::in))
    return 0;

  vil_streampos a = tell();
  xerr << "read " << n << std::endl;
  f_.read((char *)buf, (std::streamoff)n);

  // fsm  This is for gcc 2.95 :
  // If we try to read more data than is in the file, the good()
  // function will return false even though bad() returns false
  // too. The stream is actually fine but we need to clear the
  // eof flag to use it again.
  // iscott@man It does something similar under Windows, but has the added
  // advantage, of making tell() return a sensible value (instead
  // of -1)
  if (!f_.good() && !f_.bad() && f_.eof())
    f_.clear(); // allows subsequent operations

  vil_streampos b = tell();

  vil_streampos numread = b-a;
  if (b < a) { xerr << "urgh!\n"; return numread; }
  if (numread != n) { xerr << "only read " << numread << std::endl; }
  return numread;
}

vil_streampos vil_stream_fstream::tell() const
{
  assert(id > 0);
  if (flags_ & std::ios::in) {
    xerr << "tellg\n";
    return f_.tellg();
  }

  if (flags_ & std::ios::out) {
    xerr << "tellp\n";
    return f_.tellp();
  }

  assert(false); // did you get here? use at least one of std::ios::in, std::ios::out.
  return (vil_streampos)(-1L);
}

void vil_stream_fstream::seek(vil_streampos position)
{
  assert(id > 0);
  //assures that cast (below) will be ok
  assert( position <= std::numeric_limits< std::streamoff >::max() );

  bool fi = (flags_ & std::ios::in)  != 0;
  bool fo = (flags_ & std::ios::out) != 0;

  if (fi && fo) {
    xerr << "seekg and seekp to " << position << std::endl;
    if (position != vil_streampos(f_.tellg())) {
      f_.seekg((std::streamoff)position);
      f_.seekp((std::streamoff)position);
      assert(f_.good());
    }
  }

  else if (fi) {
    xerr << "seek to " << position << std::endl;
    if (position != vil_streampos(f_.tellg())) {
      f_.seekg((std::streamoff)position);
      assert(f_.good());
    }
  }

  else if (fo) {
    xerr << "seekp to " << position << std::endl;
    std::streamoff at = f_.tellp();
    if (position != at) {
      xerr << "seekp to " << position << ", at " << (long)f_.tellp() << std::endl;
      f_.seekp((std::streamoff)position);
      assert(f_.good());
    }
  }
  else
    assert(false); // did you get here? use at least one of std::ios::in, std::ios::out.
}

vil_streampos vil_stream_fstream::file_size() const
{
  // if not already computed, do so
  if ( end_ == -1 ) {
    std::streampos curr = f_.tellg();
    f_.seekg( 0, std::ios::end );
    end_ = f_.tellg();
    f_.seekg( curr );
  }

  return end_;
}
