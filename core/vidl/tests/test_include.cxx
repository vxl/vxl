#include <vidl/vidl_config.h> // needed for VIDL_HAS_FFMPEG etc.
#include <vidl/vidl_v4l2_pixel_format.h>
#include <vidl/vidl_color.h>
#include <vidl/vidl_convert.h>
#include <vidl/vidl_exception.h>
#include <vidl/vidl_frame.h>
#include <vidl/vidl_frame_sptr.h>
#include <vidl/vidl_pixel_format.h>
#include <vidl/vidl_pixel_iterator.h>
#include <vidl/vidl_istream.h>
#include <vidl/vidl_istream_sptr.h>
#include <vidl/vidl_istream_image_resource.h>
#include <vidl/vidl_image_list_istream.h>
#include <vidl/vidl_ostream.h>
#include <vidl/vidl_ostream_sptr.h>
#include <vidl/vidl_image_list_ostream.h>
#include <vidl/vidl_iidc1394_params.h>
#if VIDL_HAS_VIDEODEV2
#include <vidl/vidl_v4l2_device.h>
#include <vidl/vidl_v4l2_device_sptr.h>
#include <vidl/vidl_v4l2_devices.h>
#include <vidl/vidl_v4l2_istream.h>
#include <vidl/vidl_v4l2_control.h>
#endif
#if VIDL_HAS_FFMPEG
#include <vidl/vidl_ffmpeg_init.h>
#include <vidl/vidl_ffmpeg_istream.h>
#include <vidl/vidl_ffmpeg_ostream.h>
#include <vidl/vidl_ffmpeg_ostream_params.h>
#include <vidl/vidl_ffmpeg_convert.h>
#endif
#if VIDL_HAS_DC1394
#include <vidl/vidl_dc1394_istream.h>
#endif
#if VIDL_HAS_DSHOW
#include <vidl/vidl_dshow.h>
#include <vidl/vidl_dshow_file_istream.h>
#include <vidl/vidl_dshow_live_istream.h>
#include <vidl/vidl_dshow_istream_params.h>
#if VIDL_HAS_DSHOW_ESF
#include <vidl/vidl_dshow_istream_params_esf.h>
#endif // HAS_DSHOW_ESF
#endif // HAS_DSHOW

int main() { return 0; }
