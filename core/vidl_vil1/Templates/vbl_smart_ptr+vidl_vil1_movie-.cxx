// This avoids automatic instantiation of other smart pointers:
#define vidl_vil1_frame_sptr_h
class vidl_vil1_frame;
typedef vidl_vil1_frame *vidl_vil1_frame_sptr;
#define vidl_vil1_clip_sptr_h
class vidl_vil1_clip_sptr {};

#include <vidl_vil1/vidl_vil1_movie.h>
#include <vbl/vbl_smart_ptr.txx>

VBL_SMART_PTR_INSTANTIATE(vidl_vil1_movie);
