#include <vbl/vbl_triple.h>
#include <vcl_map.txx>

typedef vbl_triple<unsigned int, unsigned int, unsigned int> triple_uuu;
VCL_MAP_INSTANTIATE(triple_uuu, double, vcl_less<triple_uuu>);
