#include <vtol/vtol_two_chain.h>
#if 1 // needed by some "eager" compilers, because vtol_two_chain.h refers to
      // vtol_face_sptr and the compiler tries to instantiate vbl_smart_ptr<vtol_face>
# include <vtol/vtol_face.h>
# include <vtol/vtol_face_2d.h>
# include <vtol/vtol_block.h>
#endif
#include <vbl/vbl_smart_ptr.txx>

VBL_SMART_PTR_INSTANTIATE(vtol_two_chain);
