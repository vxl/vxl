// Instantiation of IO for vcl_map<vcl_string,vcl_vector<unsigned long> >
#include <vsl/vsl_map_io.txx>
#include <vsl/vsl_vector_io.txx>
#include <vcl_vector.h>
#include <vcl_string.h>
VSL_MAP_IO_INSTANTIATE(vcl_string, vcl_vector<unsigned long>, vcl_less<vcl_string>);
