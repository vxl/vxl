#include <vcl/vcl_map.txx>
#include <vcl/vcl_multimap.txx>

VCL_MULTIMAP_INSTANTIATE(double, int, vcl_less<double>);
VCL_MAP_INSTANTIATE(double, int, vcl_less<double>);
