#include <vidl2/vidl2_color.h>
#include <vidl2/vidl2_convert.h>
#include <vidl2/vidl2_frame.h>
#include <vidl2/vidl2_frame_sptr.h>
#include <vidl2/vidl2_pixel_format.h>
#include <vidl2/vidl2_pixel_iterator.h>
#include <vidl2/vidl2_istream.h>
#include <vidl2/vidl2_image_list_istream.h>
#include <vidl2/vidl2_ostream.h>
#include <vidl2/vidl2_image_list_ostream.h>
#include <vidl2/vidl2_iidc1394_params.h>
#ifdef HAS_FFMPEG
#include <vidl2/vidl2_ffmpeg_init.h>
#include <vidl2/vidl2_ffmpeg_istream.h>
#include <vidl2/vidl2_ffmpeg_ostream.h>
#include <vidl2/vidl2_ffmpeg_ostream_params.h>
#include <vidl2/vidl2_ffmpeg_convert.h>
#endif
#ifdef HAS_DC1394
#include <vidl2/vidl2_dc1394_istream.h>
#endif

int main() { return 0; }
