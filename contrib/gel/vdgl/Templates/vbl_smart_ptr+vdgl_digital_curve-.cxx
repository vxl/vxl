// This avoids automatic instantiation of other smart pointers:
#define vsol_box_2d_sptr_h
class vsol_box_2d_sptr {};
#define vdgl_edgel_chain_sptr_h
class vdgl_edgel_chain_sptr {};
#define vdgl_interpolator_sptr_h
class vdgl_interpolator_sptr {};

#include <vdgl/vdgl_digital_curve.h>
#include <vbl/vbl_smart_ptr.txx>

VBL_SMART_PTR_INSTANTIATE(vdgl_digital_curve);
