#ifdef HAS_AVI
#include <vidl1/vidl1_avicodec.h>
# if defined(_MSC_VER) || defined(__BORLANDC__) || defined(__MINGW32__)
#include <vidl1/vidl1_win_avicodec.h>
# else
#include <vidl1/vidl1_avifile_avicodec.h>
# endif
#endif
#include <vidl1/vidl1_clip.h>
#include <vidl1/vidl1_clip_sptr.h>
#include <vidl1/vidl1_codec.h>
#include <vidl1/vidl1_codec_sptr.h>
#ifdef HAS_FFMPEG
#include <vidl1/vidl1_ffmpeg_codec.h>
#endif
#include <vidl1/vidl1_file_sequence.h>
#include <vidl1/vidl1_frame.h>
#include <vidl1/vidl1_frame_sptr.h>
#include <vidl1/vidl1_frame_resource.h>
#include <vidl1/vidl1_image_list_codec.h>
#include <vidl1/vidl1_image_list_codec_sptr.h>
#include <vidl1/vidl1_io.h>
#include <vidl1/vidl1_movie.h>
#include <vidl1/vidl1_movie_sptr.h>
#ifdef HAS_MPEG2
#include <vidl1/vidl1_mpegcodec.h>
#include <vidl1/vidl1_mpegcodec_helper.h>
#include <vidl1/vidl1_mpegcodec_sptr.h>
#endif
#include <vidl1/vidl1_vob_frame_index.h>
#include <vidl1/vidl1_yuv_2_rgb.h>

int main() { return 0; }
