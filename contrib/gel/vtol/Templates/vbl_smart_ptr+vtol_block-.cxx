#include <vtol/vtol_block.h>
#if 1 // needed by some "eager" compilers, because vtol_block.h refers to
      // vtol_two_chain_sptr and the compiler tries to instantiate vbl_smart_ptr<vtol_two_chain>
# include <vtol/vtol_two_chain.h>
#endif
#include <vbl/vbl_smart_ptr.txx>

VBL_SMART_PTR_INSTANTIATE(vtol_block);
