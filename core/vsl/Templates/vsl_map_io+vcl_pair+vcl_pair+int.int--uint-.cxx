// Instantiation of IO for vcl_map<vcl_pair<int,int>,unsigned int>
#include <vsl/vsl_map_io.txx>
#include <vsl/vsl_pair_io.txx>
typedef vcl_pair<int,int> pii;
typedef vcl_less<vcl_pair<int,int> > comp;
VSL_MAP_IO_INSTANTIATE(pii, unsigned, comp);
