#include <vcl_string.h>
#include <vcl_vector.txx>
#include <vcl_utility.txx>
#include <gmvl/gmvl_node_sptr.h>

typedef vcl_pair<vcl_string,vcl_vector<gmvl_node_sptr> > pair_si;

VCL_VECTOR_INSTANTIATE( pair_si);

VCL_INSTANTIATE_INLINE( pair_si make_pair(vcl_string const &, vcl_vector<gmvl_node_sptr> const &));
