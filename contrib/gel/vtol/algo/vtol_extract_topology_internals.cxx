#include <vtol/algo/vtol_extract_topology_internals.h>
#include <vbl/vbl_smart_ptr.txx>

// These are instantiations of internal classes, so we don't take the
// normal Templates/* route. This file is separate because we cannot
// include vbl_smart_ptr.txx in vtol_extract_topology.cxx. See the
// comments there for the reason.
//
VBL_SMART_PTR_INSTANTIATE( vtol_extract_topology::edgel_chain );
VBL_SMART_PTR_INSTANTIATE( vtol_extract_topology::region_type );
