// This is core/vidl/vidl_ffmpeg_ostream_stub.txx
#ifndef vidl_ffmpeg_ostream_stub_txx_
#define vidl_ffmpeg_ostream_stub_txx_
#include "vidl_ffmpeg_ostream.h"
//:
// \file
// \brief A stub implementation when ffmpeg is not available.
//
// \author Amitha Perera
// \date 26 Dec 2007

#include <vcl_iostream.h>

struct vidl_ffmpeg_ostream::pimpl
{
};


vidl_ffmpeg_ostream
::vidl_ffmpeg_ostream()
  : os_( 0 )
{
}

vidl_ffmpeg_ostream
::vidl_ffmpeg_ostream(const vcl_string&,
                      const vidl_ffmpeg_ostream_params&)
  : os_( 0 )
{
  vcl_cerr << "vidl_ffmpeg_ostream: warning: ffmpeg support is not compiled in\n";
}


vidl_ffmpeg_ostream
::~vidl_ffmpeg_ostream()
{
}


bool
vidl_ffmpeg_ostream
::open()
{
  vcl_cerr << "vidl_ffmpeg_ostream: warning: ffmpeg support is not compiled in\n";
  return false;
}


void
vidl_ffmpeg_ostream
::close()
{
}


bool
vidl_ffmpeg_ostream
::is_open() const
{
  return false;
}


bool
vidl_ffmpeg_ostream
::write_frame(const vidl_frame_sptr& )
{
  return false;
}

#endif // vidl_ffmpeg_ostream_stub_txx_
