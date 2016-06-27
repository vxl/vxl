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
# include "vidl_ffmpeg_ostream_v1.hxx"
#elif LIBAVCODEC_BUILD < ((52<<16)+(10<<8)+0)  // before ver 52.10.0
# include "vidl_ffmpeg_ostream_v2.hxx"
#elif LIBAVCODEC_BUILD < ((53<<16)+(0<<8)+0)  // before ver 53.0.0
# include "vidl_ffmpeg_ostream_v3.hxx"
#elif LIBAVCODEC_BUILD < ((53<<16)+(24<<8)+0)  // before ver 53.24.0
# include "vidl_ffmpeg_ostream_v4.hxx"
#elif LIBAVCODEC_BUILD < ((54<<16)+(23<<8)+100)  // before ver 54.23.100
# include "vidl_ffmpeg_ostream_v0.9.hxx"
#elif LIBAVCODEC_BUILD < ((55<<16)+(28<<8)+1)  // before ver 55.28.1
# include "vidl_ffmpeg_ostream_v0.11.hxx"
#elif LIBAVCODEC_BUILD < ((56<<16)+(0<<8)+100)  // before ver 56.0.100
# pragma message("FFMPEG is NOT supported for LIBAVCODEC version between 55.28.1 and 56.0.100")
# include "vidl_ffmpeg_ostream_stub.hxx"
#else
# include "vidl_ffmpeg_ostream_v56.hxx"
#endif

#else // VIDL_HAS_FFMPEG

# include "vidl_ffmpeg_ostream_stub.hxx"

#endif // VIDL_HAS_FFMPEG
