#include <vtol/vtol_vertex.h>
#if 1 // needed by some "eager" compilers, because vtol_vertex.h refers to
      // vtol_edge_sptr and the compiler tries to instantiate vbl_smart_ptr<vtol_edge>
# include <vtol/vtol_edge.h>
#endif
#include <vbl/vbl_smart_ptr.txx>

VBL_SMART_PTR_INSTANTIATE(vtol_vertex);
