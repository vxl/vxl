#include <vcl/vcl_map.txx>
#include <vcl/vcl_multimap.txx>

VCL_MULTIMAP_INSTANTIATE(unsigned int, unsigned int, vcl_less<unsigned int>);
VCL_MAP_INSTANTIATE(unsigned int, unsigned int, vcl_less<unsigned int>);
