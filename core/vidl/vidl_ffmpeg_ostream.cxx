// This is core/vidl/vidl_ffmpeg_ostream.cxx
#include "vidl_ffmpeg_ostream.h"

#include <vidl/vidl_config.h>

#if VIDL_HAS_FFMPEG

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
#include "vidl_ffmpeg_convert.h"

#if LIBAVCODEC_BUILD < ((51<<16)+(49<<8)+0)  // ver 51.49.0
# include "vidl_ffmpeg_ostream_v1.txx"
#elif LIBAVCODEC_BUILD < ((52<<16)+(10<<8)+0)  // before ver 52.10.0
# include "vidl_ffmpeg_ostream_v2.txx"
#elif LIBAVCODEC_BUILD < ((53<<16)+(0<<8)+0)  // before ver 53.0.0
# include "vidl_ffmpeg_ostream_v3.txx"
#else
# include "vidl_ffmpeg_ostream_v4.txx"
#endif

#else // VIDL_HAS_FFMPEG

# include "vidl_ffmpeg_ostream_stub.txx"

#endif // VIDL_HAS_FFMPEG
