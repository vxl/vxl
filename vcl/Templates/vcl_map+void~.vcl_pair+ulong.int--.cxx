#include <vcl_map.txx>
#include <vcl_utility.h>
typedef vcl_pair<unsigned long, int> pair_uli;
VCL_MAP_INSTANTIATE(void*, pair_uli, vcl_less<void*>);
