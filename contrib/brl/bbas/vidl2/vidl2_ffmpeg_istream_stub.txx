// This is brl/bbas/vidl2/vidl2_ffmpeg_istream_stub.txx
#ifndef vidl2_ffmpeg_istream_stub_txx_
#define vidl2_ffmpeg_istream_stub_txx_
#include "vidl2_ffmpeg_istream.h"
//:
// \file
// \brief A null implementation for the ffmpeg video reader
//
// \author Amitha Perera
// \date 14 Jan 2008

#include <vcl_string.h>

struct vidl2_ffmpeg_istream::pimpl
{
};


vidl2_ffmpeg_istream
::vidl2_ffmpeg_istream()
{
}

vidl2_ffmpeg_istream
::vidl2_ffmpeg_istream(const vcl_string& filename)
{
}


vidl2_ffmpeg_istream
::~vidl2_ffmpeg_istream()
{
}

bool
vidl2_ffmpeg_istream
::open(const vcl_string& filename)
{
  return false;
}


void
vidl2_ffmpeg_istream
::close()
{
}


bool
vidl2_ffmpeg_istream
::is_open() const
{
  return false;
}

bool
vidl2_ffmpeg_istream
::is_valid() const
{
  return false;
}


bool
vidl2_ffmpeg_istream
::is_seekable() const
{
  return false;
}

unsigned int
vidl2_ffmpeg_istream
::frame_number() const
{
  return 0;
}

bool
vidl2_ffmpeg_istream
::advance()
{
  return false;
}


vidl2_frame_sptr
vidl2_ffmpeg_istream
::read_frame()
{
  return vidl2_frame_sptr();
}

vidl2_frame_sptr
vidl2_ffmpeg_istream
::current_frame()
{
  return vidl2_frame_sptr();
}


bool
vidl2_ffmpeg_istream
::seek_frame(unsigned int frame_number)
{
  return false;
}

#endif // vidl2_ffmpeg_istream_stub_txx_
