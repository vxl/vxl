#include <vtol/vtol_edge_2d.h>
#include <vcl_vector.txx>

VCL_VECTOR_INSTANTIATE(vtol_edge_2d_sptr);

#include <vcl_algorithm.txx>
VCL_FIND_INSTANTIATE(vcl_vector<vtol_edge_2d_sptr>::const_iterator,
                     vtol_edge_2d_sptr);
#if 0 // Is this still needed? Gives errors with gcc 3.2 on Linux
VCL_FIND_INSTANTIATE(vcl_vector<vtol_edge_2d_sptr>::const_iterator,
                     const vtol_edge_2d *);
#endif
