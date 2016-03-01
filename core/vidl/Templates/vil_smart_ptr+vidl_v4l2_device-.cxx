#include <vil/vil_smart_ptr.hxx>
#include <vidl/vidl_config.h>
#if VIDL_HAS_VIDEODEV2
#include <vidl/vidl_v4l2_device.h>
#else
struct vidl_v4l2_device { void ref() {} void unref() {} }; // dummy class
#endif
VIL_SMART_PTR_INSTANTIATE(vidl_v4l2_device);
