#include <vcl_vector.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vcl_map.txx>
typedef vcl_vector<vtol_edge_2d_sptr> vedge;
VCL_MAP_INSTANTIATE(int, vedge*, vcl_less<int> );
VCL_MULTIMAP_INSTANTIATE(int, vedge*, vcl_less<int> );
