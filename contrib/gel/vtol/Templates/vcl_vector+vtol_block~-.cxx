#include <vcl_vector.h>
#include <vcl_vector.txx>
#include <vcl_algorithm.txx>
#include <vcl_algorithm.h>
#include <vtol/vtol_edge.h>
#include <vtol/vtol_block.h>
//#include <vtol/some_stubs.h>

typedef class vtol_block *elt_t;

VCL_VECTOR_INSTANTIATE(elt_t);
VCL_FIND_INSTANTIATE(vcl_vector<elt_t>::iterator, elt_t);
