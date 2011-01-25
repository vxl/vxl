#ifndef USE_V123
#include <vidl/vidl_ffmpeg_istream_stub.txx>
#include <vidl/vidl_ffmpeg_ostream_stub.txx>
#else
#include <vidl/vidl_ffmpeg_istream_v1.txx>
#include <vidl/vidl_ffmpeg_istream_v2.txx>
#include <vidl/vidl_ffmpeg_ostream_v1.txx>
#include <vidl/vidl_ffmpeg_ostream_v2.txx>
#include <vidl/vidl_ffmpeg_ostream_v3.txx>
#endif
#include <vidl/vidl_pixel_iterator.txx>

int main() { return 0; }
