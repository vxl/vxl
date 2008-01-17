// This is brl/bbas/vidl2/vidl2_ffmpeg_istream.cxx

#include <vidl2/vidl2_config.h>

#if VIDL2_HAS_FFMPEG

#include "vidl2_ffmpeg_ostream.h"

// The ffmpeg API keeps changing, so we use different implementations
// depending on which version of ffmpeg we have.

#include "vidl2_ffmpeg_istream.h"

extern "C" {
#include <ffmpeg/avformat.h>
}

#if LIBAVFORMAT_BUILD < ((52<<16)+(2<<8)+0)  // before ver 52.2.0
# include "vidl2_ffmpeg_istream_v1.txx"
#else
# include "vidl2_ffmpeg_istream_v2.txx"
#endif

#else // VIDL2_HAS_FFMPEG

# include "vidl2_ffmpeg_istream_stub.txx"

#endif // VIDL2_HAS_FFMPEG
