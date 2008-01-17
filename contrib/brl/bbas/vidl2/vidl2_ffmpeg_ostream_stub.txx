// This is brl/bbas/vidl2/vidl2_ffmpeg_ostream_stub.txx

//:
// \file
// \brief A stub implementation when ffmpeg is not available.
//
// \author Amitha Perera
// \date 26 Dec 2007

#include "vidl2_ffmpeg_ostream.h"

#include <vcl_iostream.h>

struct vidl2_ffmpeg_ostream::pimpl
{
};


vidl2_ffmpeg_ostream
::vidl2_ffmpeg_ostream()
  : os_( 0 )
{
}

vidl2_ffmpeg_ostream
::vidl2_ffmpeg_ostream(const vcl_string&,
                       const vidl2_ffmpeg_ostream_params&)
  : os_( 0 )
{
  vcl_cerr << "vidl2_ffmpeg_ostream: warning: ffmpeg support is not compiled in\n";
}


vidl2_ffmpeg_ostream
::~vidl2_ffmpeg_ostream()
{
}


bool
vidl2_ffmpeg_ostream
::open()
{
  vcl_cerr << "vidl2_ffmpeg_ostream: warning: ffmpeg support is not compiled in\n";
  return false;
}


void
vidl2_ffmpeg_ostream
::close()
{
}


bool
vidl2_ffmpeg_ostream
::is_open() const
{
  return false;
}


bool
vidl2_ffmpeg_ostream
::write_frame(const vidl2_frame_sptr& )
{
  return false;
}
