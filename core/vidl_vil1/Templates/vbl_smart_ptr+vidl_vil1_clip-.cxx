// This avoids automatic instantiation of other smart pointers:
#define vidl_vil1_codec_sptr_h
class vidl_vil1_codec_sptr {};
#define vidl_vil1_frame_sptr_h
class vidl_vil1_frame_sptr {};

#include <vidl_vil1/vidl_vil1_clip.h>
#include <vbl/vbl_smart_ptr.txx>

VBL_SMART_PTR_INSTANTIATE(vidl_vil1_clip);
