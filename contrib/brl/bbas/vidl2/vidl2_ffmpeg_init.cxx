// This is brl/bbas/vidl2/vidl2_ffmpeg_init.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Matt Leotta
// \date   21 Dec 2005
//
//-----------------------------------------------------------------------------
// some versons of FFMPEG require this definition before including
// the headers for C++ compatibility
#define __STDC_CONSTANT_MACROS

#include "vidl2_ffmpeg_init.h"
#include <vidl2/vidl2_config.h>
extern "C" {
#if FFMPEG_IN_SEVERAL_DIRECTORIES
#include <libavformat/avformat.h>
#else
#include <ffmpeg/avformat.h>
#endif
}

//--------------------------------------------------------------------------------

void vidl2_ffmpeg_init()
{
  static bool initialized = false;
  if ( ! initialized ) {
    av_register_all();
    av_log_set_level(AV_LOG_ERROR);
    initialized = true;
  }
}
