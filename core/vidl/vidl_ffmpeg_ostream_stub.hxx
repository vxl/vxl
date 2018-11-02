// This is core/vidl/vidl_ffmpeg_ostream_stub.hxx
#ifndef vidl_ffmpeg_ostream_stub_hxx_
#define vidl_ffmpeg_ostream_stub_hxx_
#include <iostream>
#include <stdexcept>
#include <string>
#include "vidl_ffmpeg_ostream.h"

//:
// \file
// \brief A stub implementation when ffmpeg is not available.
//
// \author Amitha Perera
// \date 26 Dec 2007

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

struct vidl_ffmpeg_ostream::pimpl
{
};


vidl_ffmpeg_ostream
::vidl_ffmpeg_ostream()
  : os_( nullptr )
{
  std::cerr << "vidl_ffmpeg_ostream: warning: ffmpeg support is not compiled in\n";
}

vidl_ffmpeg_ostream
::vidl_ffmpeg_ostream(const std::string&,
                      const vidl_ffmpeg_ostream_params&)
  : os_( nullptr )
{
  std::cerr << "vidl_ffmpeg_ostream: warning: ffmpeg support is not compiled in\n";
}


vidl_ffmpeg_ostream
::~vidl_ffmpeg_ostream()
{
}


bool
vidl_ffmpeg_ostream
::open()
{
  throw std::runtime_error( "vidl_ffmpeg_ostream: ffmpeg support is not compiled in" );
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

#endif // vidl_ffmpeg_ostream_stub_hxx_
