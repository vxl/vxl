// This is core/vidl/vidl_ffmpeg_istream_stub.hxx
#ifndef vidl_ffmpeg_istream_stub_hxx_
#define vidl_ffmpeg_istream_stub_hxx_
#include <string>
#include <stdexcept>
#include "vidl_ffmpeg_istream.h"

//:
// \file
// \brief A null implementation for the ffmpeg video reader
//
// \author Amitha Perera
// \date 14 Jan 2008

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

struct vidl_ffmpeg_istream::pimpl
{
};


vidl_ffmpeg_istream
::vidl_ffmpeg_istream()
{
  std::cerr << "vidl_ffmpeg_istream: warning: ffmpeg support is not compiled in\n";
}

vidl_ffmpeg_istream
::vidl_ffmpeg_istream(const std::string& /*filename*/)
{
  std::cerr << "vidl_ffmpeg_istream: warning: ffmpeg support is not compiled in\n";
}


vidl_ffmpeg_istream
::~vidl_ffmpeg_istream()
{
}

bool
vidl_ffmpeg_istream
::open(const std::string& /*filename*/)
{
  throw std::runtime_error( "vidl_ffmpeg_istream: ffmpeg support is not compiled in" );
}


void
vidl_ffmpeg_istream
::close()
{
}


bool
vidl_ffmpeg_istream
::is_open() const
{
  return false;
}

bool
vidl_ffmpeg_istream
::is_valid() const
{
  return false;
}


bool
vidl_ffmpeg_istream
::is_seekable() const
{
  return false;
}

int
vidl_ffmpeg_istream
::num_frames() const
{
  return -1;
}

unsigned int
vidl_ffmpeg_istream
::frame_number() const
{
  return 0;
}


//: Return the width of each frame
unsigned int
vidl_ffmpeg_istream
::width() const
{
  return 0;
}


//: Return the height of each frame
unsigned int
vidl_ffmpeg_istream
::height() const
{
  return 0;
}


//: Return the pixel format
vidl_pixel_format
vidl_ffmpeg_istream
::format() const
{
  return VIDL_PIXEL_FORMAT_UNKNOWN;
}


//: Return the frame rate (0.0 if unspecified)
double
vidl_ffmpeg_istream
::frame_rate() const
{
  return 0.0;
}


//: Return the duration in seconds (0.0 if unknown)
double
vidl_ffmpeg_istream
::duration() const
{
  return 0.0;
}


bool
vidl_ffmpeg_istream
::advance()
{
  return false;
}


vidl_frame_sptr
vidl_ffmpeg_istream
::read_frame()
{
  return vidl_frame_sptr();
}


vidl_frame_sptr
vidl_ffmpeg_istream
::current_frame()
{
  return vidl_frame_sptr();
}


bool
vidl_ffmpeg_istream
::seek_frame(unsigned int /* frame_nr */)
{
  return false;
}


std::deque<vxl_byte>
vidl_ffmpeg_istream
::current_metadata()
{
  return std::deque<vxl_byte>();
}


bool
vidl_ffmpeg_istream
::has_metadata() const
{
  return false;
}


double
vidl_ffmpeg_istream::
current_pts() const
{
  return 0.0;
}


std::vector<vxl_byte>
vidl_ffmpeg_istream::
current_packet_data() const
{
  return std::vector<vxl_byte>();
}

#endif // vidl_ffmpeg_istream_stub_hxx_
