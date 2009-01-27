// This is brl/bbas/vidl2/vidl2_ffmpeg_ostream.cxx

#include <vidl2/vidl2_config.h>

#if VIDL2_HAS_FFMPEG

#include "vidl2_ffmpeg_ostream.h"

// The ffmpeg API keeps changing, so we use different implementations
// depending on which version of ffmpeg we have.

extern "C" {
// some versions of FFMPEG require this definition before including 
// the headers for C++ compatibility
#define __STDC_CONSTANT_MACROS
#if FFMPEG_IN_SEVERAL_DIRECTORIES
#include <libavcodec/avcodec.h>
#else
#include <ffmpeg/avcodec.h>
#endif
}

#if LIBAVCODEC_BUILD < ((51<<16)+(49<<8)+0)  // ver 51.49.0
# include "vidl2_ffmpeg_ostream_v1.txx"
#elif LIBAVCODEC_BUILD < ((52<<16)+(11<<8)+0)  // before ver 52.11.0
# include "vidl2_ffmpeg_ostream_v2.txx"
#else
# include "vidl2_ffmpeg_ostream_v3.txx"
#endif

#else // VIDL2_HAS_FFMPEG

# include "vidl2_ffmpeg_ostream_stub.txx"

#endif // VIDL2_HAS_FFMPEG
