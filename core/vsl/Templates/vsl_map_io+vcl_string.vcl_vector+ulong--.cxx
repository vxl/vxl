// Instantiation of IO for vcl_map<vcl_string,vcl_vector<unsigned long> >
#include <vsl/vsl_vector_io.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vsl/vsl_map_io.txx>
typedef vcl_vector<unsigned long> vec_ulong;
typedef vcl_less<vcl_string> comp;
VSL_MAP_IO_INSTANTIATE(vcl_string, vec_ulong, comp);
