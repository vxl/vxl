#include <vcl_map.txx>
#include <vcl_utility.h>
typedef vcl_pair<void*, int> pair_vi;
VCL_MAP_INSTANTIATE(unsigned long, pair_vi, vcl_less<unsigned long>);
