#include <vtol/vtol_topology_object_sptr.h>
#include <vcl_vector.txx>

VCL_VECTOR_INSTANTIATE(vtol_topology_object_sptr);

#include <vcl_algorithm.txx>
VCL_FIND_INSTANTIATE(vcl_vector<vtol_topology_object_sptr>::const_iterator,
                     vtol_topology_object_sptr);
VCL_FIND_INSTANTIATE(vcl_vector<vtol_topology_object_sptr>::iterator,
                     vtol_topology_object_sptr);
#if 0 // Is this still needed? Gives errors with gcc 3.2 on Linux
VCL_FIND_INSTANTIATE(vcl_vector<vtol_topology_object_sptr>::const_iterator,
                     const vtol_topology_object*);
#endif
