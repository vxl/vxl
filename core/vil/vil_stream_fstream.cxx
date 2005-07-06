// This is core/vil/vil_stream_fstream.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include "vil_stream_fstream.h"
#include <vcl_cassert.h>
#include <vcl_limits.h>
#include <vcl_iostream.h>
#include <vcl_ios.h>

static vcl_ios_openmode modeflags(char const* mode)
{
  if (*mode == 0)
    return vcl_ios_openmode(0);

  if (*mode == 'r') {
    if (mode[1] == '+')
      return vcl_ios_in | vcl_ios_out | modeflags(mode+2);
    else
      return vcl_ios_in | modeflags(mode+1);
  }

  if (*mode == 'w') {
    if (mode[1] == '+')
      return vcl_ios_in | vcl_ios_out | vcl_ios_trunc | modeflags(mode+2);
    else
      return vcl_ios_out | modeflags(mode+1);
  }

  vcl_cerr << vcl_endl << __FILE__ ": DODGY MODE " << mode << vcl_endl;
  return vcl_ios_openmode(0);
}

#define xerr if (true) ; else (vcl_cerr << "vcl_fstream#" << id_ << ": ")

static int id = 0;

vil_stream_fstream::vil_stream_fstream(char const* fn, char const* mode):
  flags_(modeflags(mode)),
  f_(fn, flags_ | vcl_ios_binary), // need ios::binary on windows.
  end_( -1 )
{
  id_ = ++id;
  xerr << "vil_stream_fstream(\"" << fn << "\", \""<<mode<<"\") = " << id_ << '\n';
#if 0
  if (!f_) {
    vcl_cerr << "vil_stream_fstream::Could not open [" << fn << "]\n";
  }
#endif // 0
}

#if 0
vil_stream_fstream::vil_stream_fstream(vcl_fstream& f):
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
  //but the FreeBSD-4.­9-gcc-2.­95.­4 build seems to have a problem
  //with it, so I'm commenting it out
  //assert( position < vcl_numeric_limits< vcl_streamoff >::max() );

  if (!(flags_ & vcl_ios_out)) {
    vcl_cerr << "vil_stream_fstream: write failed, not an vcl_ostream\n";
    return 0;
  }

  vil_streampos a = tell();
  xerr << "write " << n << vcl_endl;
  f_.write((char const*)buf, (vcl_streamoff)n);
  if (!f_.good())
    vcl_cerr << ("vil_stream_fstream: ERROR: write failed!\n");
  vil_streampos b = tell();
  f_.flush();
  return b-a;
}


vil_streampos vil_stream_fstream::read(void* buf, vil_streampos n)
{
  assert(id > 0);
  //assures that cast (below) will be ok
  //but the FreeBSD-4.­9-gcc-2.­95.­4 build seems to have a problem
  //with it, so I'm commenting it out
  //assert( position < vcl_numeric_limits< vcl_streamoff >::max() );

  if (!(flags_ & vcl_ios_in))
    return 0;

  vil_streampos a = tell();
  xerr << "read " << n << vcl_endl;
  f_.read((char *)buf, (vcl_streamoff)n);

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
  if (numread != n) xerr << "only read " << numread << vcl_endl;
  return numread;
}

vil_streampos vil_stream_fstream::tell() const
{
  assert(id > 0);
  if (flags_ & vcl_ios_in) {
    xerr << "tellg\n";
    return f_.tellg();
  }

  if (flags_ & vcl_ios_out) {
    xerr << "tellp\n";
    return f_.tellp();
  }

  assert(false); // did you get here? use at least one of vcl_ios_in, vcl_ios_out.
  return (vil_streampos)(-1L);
}

void vil_stream_fstream::seek(vil_streampos position)
{
  assert(id > 0);
  //assures that cast (below) will be ok
  //but the FreeBSD-4.­9-gcc-2.­95.­4 build seems to have a problem
  //with it, so I'm commenting it out
  //assert( position < vcl_numeric_limits< vcl_streamoff >::max() );

  bool fi = (flags_ & vcl_ios_in)  != 0;
  bool fo = (flags_ & vcl_ios_out) != 0;

  if (fi && fo) {
    xerr << "seekg and seekp to " << position << vcl_endl;
    if (position != vil_streampos(f_.tellg())) {
      f_.seekg((vcl_streamoff)position);
      f_.seekp((vcl_streamoff)position);
      assert(f_.good());
    }
  }

  else if (fi) {
    xerr << "seek to " << position << vcl_endl;
    if (position != vil_streampos(f_.tellg())) {
      f_.seekg((vcl_streamoff)position);
      assert(f_.good());
    }
  }

  else if (fo) {
    xerr << "seekp to " << position << vcl_endl;
    int at = f_.tellp();
    if (position != at) {
      xerr << "seekp to " << position << ", at " << (long)f_.tellp() << vcl_endl;
      f_.seekp((vcl_streamoff)position);
      assert(f_.good());
    }
  }
  else
    assert(false); // did you get here? use at least one of vcl_ios_in, vcl_ios_out.
}

vil_streampos vil_stream_fstream::file_size() const
{
  // if not already computed, do so
  if ( end_ == -1 ) {
    vcl_streampos curr = f_.tellg();
    f_.seekg( 0, vcl_ios_end );
    end_ = f_.tellg();
    f_.seekg( curr );
  }

  return end_;
}
