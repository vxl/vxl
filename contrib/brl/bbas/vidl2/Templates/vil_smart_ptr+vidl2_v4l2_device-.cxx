#include <vil/vil_smart_ptr.txx>
#if VIDL2_HAS_VIDEODEV2
#include <vidl2/vidl2_v4l2_device.h>
#else
struct vidl2_v4l2_device { void ref() {} void unref() {} }; // dummy class
#endif
VIL_SMART_PTR_INSTANTIATE(vidl2_v4l2_device);
