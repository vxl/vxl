#include <vsl/vsl_map_io.txx>
#include <vcl_utility.h>
typedef vcl_pair<int,int> pii;
typedef vcl_less<vcl_pair<int,int> > comp;
VSL_MAP_IO_INSTANTIATE(pii, unsigned, comp);
