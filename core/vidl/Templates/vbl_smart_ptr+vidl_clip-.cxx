// This avoids automatic instantiation of other smart pointers:
#define vidl_codec_sptr_h
class vidl_codec_sptr {};
#define vidl_frame_sptr_h
class vidl_frame_sptr {};

#include <vidl/vidl_clip.h>
#include <vbl/vbl_smart_ptr.txx>

VBL_SMART_PTR_INSTANTIATE(vidl_clip);
