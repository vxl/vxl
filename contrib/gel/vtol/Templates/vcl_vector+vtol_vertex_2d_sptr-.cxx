#include <vtol/vtol_vertex_2d_sptr.h>
#include <vcl_vector.txx>

VCL_VECTOR_INSTANTIATE(vtol_vertex_2d_sptr);

#include <vcl_algorithm.txx>
VCL_FIND_INSTANTIATE(vcl_vector<vtol_vertex_2d_sptr>::const_iterator,
                     vtol_vertex_2d_sptr);
VCL_FIND_INSTANTIATE(vcl_vector<vtol_vertex_2d_sptr>::iterator,
                     vtol_vertex_2d_sptr);
