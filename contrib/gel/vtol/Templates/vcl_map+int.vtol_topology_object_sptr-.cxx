#include <vtol/vtol_topology_object_sptr.h>
#include <vcl_map.txx>

VCL_MAP_INSTANTIATE(int, vtol_topology_object_sptr, vcl_less<int> );
VCL_MULTIMAP_INSTANTIATE(int, vtol_topology_object_sptr, vcl_less<int> );
