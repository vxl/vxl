#include <vtol/vtol_face_2d.h>
#if defined(VCL_KAI) || defined(VCL_SGI_CC)
# include <vsol/vsol_region_2d.h>
#endif
#if 1 // needed by some "eager" compilers, because vtol_face_2d.h refers to
      // vtol_one_chain_sptr and the compiler tries to instantiate vbl_smart_ptr<vtol_one_chain>
# include <vtol/vtol_one_chain.h>
#endif
#include <vbl/vbl_smart_ptr.txx>

VBL_SMART_PTR_INSTANTIATE(vtol_face_2d);
