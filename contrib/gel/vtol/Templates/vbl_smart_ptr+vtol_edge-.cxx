#include <vtol/vtol_edge.h>
#ifdef VCL_KAI
# include <vsol/vsol_point.h>
# include <vsol/vsol_curve.h>
#endif
#if 1 // needed by some "eager" compilers, because vtol_edge.h refers to
      // vtol_edge_2d_sptr and the compiler tries to instantiate vbl_smart_ptr<vtol_edge_2d>
# include <vtol/vtol_edge_2d.h>
#endif
#include <vbl/vbl_smart_ptr.txx>

VBL_SMART_PTR_INSTANTIATE(vtol_edge);
