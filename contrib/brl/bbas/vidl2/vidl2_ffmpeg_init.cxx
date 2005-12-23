// This is contrib/brl/bbas/vidl2/vidl2_ffmpeg_init.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Matt Leotta
// \date   21 Dec 2005
//
//-----------------------------------------------------------------------------

#include "vidl2_ffmpeg_init.h"
#include <ffmpeg/avformat.h>

//--------------------------------------------------------------------------------

void vidl2_ffmpeg_init()
{
  static bool initialized = false;
  if( ! initialized ) {
    av_register_all();
    av_log_set_level(AV_LOG_ERROR);
    initialized = true;
  }
}
