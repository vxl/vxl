// This is core/vil1/vil1_stream_fstream.cxx

#include <iostream>
#include "vil1_stream_fstream.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

static std::ios::openmode modeflags(char const* mode)
{
  if (*mode == 0)
    return std::ios::openmode(0);

  if (*mode == 'r') {
    if (mode[1] == '+')
      return std::ios::in | std::ios::out | modeflags(mode+2);
    else
      return std::ios::in | modeflags(mode+1);
  }

  if (*mode == 'w') {
    if (mode[1] == '+')
      return std::ios::in | std::ios::out | std::ios::trunc | modeflags(mode+2);
    else
      return std::ios::out | modeflags(mode+1);
  }

  std::cerr << std::endl << __FILE__ ": DODGY MODE " << mode << std::endl;
  return std::ios::openmode(0);
}

#define xerr if (true) ; else (std::cerr << "std::fstream#" << id_ << ": ")

static int id = 0;

vil1_stream_fstream::vil1_stream_fstream(char const* fn, char const* mode):
  flags_(modeflags(mode)),
  f_(fn, flags_ | std::ios::binary) // need ios::binary on windows.
{
  id_ = ++id;
  xerr << "vil1_stream_fstream(\"" << fn << "\", \""<<mode<<"\") = " << id_ << '\n';
#if 0
  if (!f_)
    std::cerr << "vil1_stream_fstream::Could not open [" << fn << "]\n";
#endif // 0
}

#if 0
vil1_stream_fstream::vil1_stream_fstream(std::fstream& f):
  f_(f.rdbuf()->fd())
{
}
#endif // 0

vil1_stream_fstream::~vil1_stream_fstream()
{
  xerr << "vil1_stream_fstream# " << id_ << " being deleted\n";
}

vil1_streampos vil1_stream_fstream::write(void const* buf, vil1_streampos n)
{
  assert(id > 0);
  if (!(flags_ & std::ios::out)) {
    std::cerr << "vil1_stream_fstream: write failed, not a std::ostream\n";
    return 0;
  }

  vil1_streampos a = tell();
  xerr << "write " << n << std::endl;
  f_.write((char const*)buf, n);
  if (!f_.good())
    std::cerr << ("vil1_stream_fstream: ERROR: write failed!\n");
  vil1_streampos b = tell();
  f_.flush();
  return b-a;
}


vil1_streampos vil1_stream_fstream::read(void* buf, vil1_streampos n)
{
  assert(id > 0);

  if (!(flags_ & std::ios::in))
    return 0;

  vil1_streampos a = tell();
  xerr << "read " << n << std::endl;
  f_.read((char *)buf, n);

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

  vil1_streampos b = tell();

  vil1_streampos numread = b-a;
  if (b < a) { xerr << "urgh!\n"; return numread; }
  if (numread != n) { xerr << "only read " << numread << std::endl; }
  return numread;
}

vil1_streampos vil1_stream_fstream::tell() const
{
  assert(id > 0);
  if (flags_ & std::ios::in) {
    xerr << "tellg\n";
    return static_cast<vil1_streampos>(f_.tellg());
  }
  if (flags_ & std::ios::out) {
    xerr << "tellp\n";
    return static_cast<vil1_streampos>(f_.tellp());
  }

  assert(false); // did you get here? use at least one of std::ios::in, std::ios::out.
  return static_cast<vil1_streampos>(-1L);
}

void vil1_stream_fstream::seek(vil1_streampos position)
{
  assert(id > 0);
  bool fi = (flags_ & std::ios::in)  != 0;
  bool fo = (flags_ & std::ios::out) != 0;

  if (fi && fo) {
    xerr << "seekg and seekp to " << position << std::endl;
    if (position != vil1_streampos(f_.tellg())) {
      f_.seekg(position);
      f_.seekp(position);
      assert(f_.good());
    }
  }
  else if (fi) {
    xerr << "seek to " << position << std::endl;
    if (position != vil1_streampos(f_.tellg())) {
      f_.seekg(position);
      assert(f_.good());
    }
  }
  else if (fo) {
    xerr << "seekp to " << position << std::endl;
    int at = static_cast<vil1_streampos>(f_.tellp());
    if (position != at) {
      xerr << "seekp to " << position << ", at " << (long)f_.tellp() << std::endl;
      f_.seekp(position);
      assert(f_.good());
    }
  }
  else
    assert(false); // did you get here? use at least one of std::ios::in, std::ios::out.
}
