#include <vdgl/vdgl_intensity_face.h>
#if 1 // needed by some "eager" compilers, because vdgl_intensity_face.h refers to
      // vtol_one_chain_sptr and the compiler tries to instantiate vbl_smart_ptr<vtol_one_chain>
# include <vtol/vtol_one_chain.h>
#endif
#include <vbl/vbl_smart_ptr.txx>
VBL_SMART_PTR_INSTANTIATE(vdgl_intensity_face);
