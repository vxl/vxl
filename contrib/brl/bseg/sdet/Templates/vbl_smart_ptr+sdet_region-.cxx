// This avoids automatic instantiation of other smart pointers:
#define vsol_box_2d_sptr_h
class vsol_box_2d_sptr {};
#define vsol_region_2d_sptr_h
class vsol_region_2d_sptr {};
#define vsol_polygon_2d_sptr_h
class vsol_polygon_2d_sptr {};
#define vdgl_digital_region_sptr_h
class vdgl_digital_region_sptr {};

#include <sdet/sdet_region.h>
#include <vbl/vbl_smart_ptr.txx>

VBL_SMART_PTR_INSTANTIATE(sdet_region);
