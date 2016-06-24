// This is core/vidl/vidl_ffmpeg_istream.cxx
#include "vidl_ffmpeg_istream.h"

#include <vidl/vidl_config.h>

#if VIDL_HAS_FFMPEG

#include "vidl_ffmpeg_ostream.h"

// The ffmpeg API keeps changing, so we use different implementations
// depending on which version of ffmpeg we have.

extern "C" {
// some versions of FFMPEG require this definition before including
// the headers for C++ compatibility
#define __STDC_CONSTANT_MACROS
#if FFMPEG_IN_SEVERAL_DIRECTORIES
#include <libavformat/avformat.h>
#else
#include <ffmpeg/avformat.h>
#endif
}

#if LIBAVFORMAT_BUILD < ((52<<16)+(2<<8)+0)  // before ver 52.2.0
# include "vidl_ffmpeg_istream_v1.hxx"
#elif LIBAVFORMAT_BUILD < ((53<<16)+(0<<8)+0)  // before ver 53.0.0
# include "vidl_ffmpeg_istream_v2.hxx"
#elif LIBAVFORMAT_BUILD < ((53<<16)+(24<<8)+0)  // before ver 53.24.0
# include "vidl_ffmpeg_istream_v3.hxx"
#elif LIBAVFORMAT_BUILD < ((55<<16))  // before ver 55.0.0
# include "vidl_ffmpeg_istream_v0.9.hxx"
#elif LIBAVFORMAT_BUILD < ((56<<16)+(6<<8)+1) // before ver 56.6.1
# pragma message("FFMPEG is NOT supported for LIBAVFORMTAT version between 55.0.0 and 56.6.1")
# include "vidl_ffmpeg_istream_stub.hxx"
#else
# include "vidl_ffmpeg_istream_v56.hxx"
#endif

#else // VIDL_HAS_FFMPEG

# include "vidl_ffmpeg_istream_stub.hxx"

#endif // VIDL_HAS_FFMPEG
