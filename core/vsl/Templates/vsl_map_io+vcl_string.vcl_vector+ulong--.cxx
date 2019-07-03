// Instantiation of IO for std::map<std::string,std::vector<unsigned long> >
#include <vector>
#include <string>
#include <vsl/vsl_vector_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_map_io.hxx>
using vec_ulong = std::vector<unsigned long>;
using comp = std::less<std::string>;
VSL_MAP_IO_INSTANTIATE(std::string, vec_ulong, comp);
