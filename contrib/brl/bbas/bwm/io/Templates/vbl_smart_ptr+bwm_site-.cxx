#include <bwm/io/bwm_site.h>
#include <vbl/vbl_smart_ptr.hxx>

// dummy declarations (to avoid having to include too many header files)
class vsol_box_2d {public:static void ref(){} static void unref(){}};
class vsol_box_3d {public:static void ref(){} static void unref(){}};

VBL_SMART_PTR_INSTANTIATE(bwm_site);
