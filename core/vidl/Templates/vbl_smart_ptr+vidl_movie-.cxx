// This avoids automatic instantiation of other smart pointers:
#define vidl_clip_sptr_h
class vidl_clip_sptr {};

#include <vidl/vidl_movie.h>
#include <vbl/vbl_smart_ptr.txx>

VBL_SMART_PTR_INSTANTIATE(vidl_movie);
