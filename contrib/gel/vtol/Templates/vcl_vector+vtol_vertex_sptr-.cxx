#include <vtol/vtol_vertex_sptr.h>
#include <vcl_vector.txx>

VCL_VECTOR_INSTANTIATE(vtol_vertex_sptr);

#include <vcl_algorithm.txx>
VCL_FIND_INSTANTIATE(vcl_vector<vtol_vertex_sptr>::const_iterator,
                     vtol_vertex_sptr);
VCL_FIND_INSTANTIATE(vcl_vector<vtol_vertex_sptr>::iterator,
                     vtol_vertex_sptr);
