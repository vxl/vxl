// This avoids automatic instantiation of other smart pointers:
#define vsol_box_2d_sptr_h
class vsol_box_2d_sptr {};

#include <vpro/vpro_video_process.h>
#include <vbl/vbl_smart_ptr.txx>

VBL_SMART_PTR_INSTANTIATE(vpro_video_process);
