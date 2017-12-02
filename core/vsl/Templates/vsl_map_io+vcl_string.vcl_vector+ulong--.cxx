// Instantiation of IO for std::map<std::string,std::vector<unsigned long> >
#include <vector>
#include <string>
#include <vsl/vsl_vector_io.h>
#include <vcl_compiler.h>
#include <vsl/vsl_map_io.hxx>
typedef std::vector<unsigned long> vec_ulong;
typedef std::less<std::string> comp;
VSL_MAP_IO_INSTANTIATE(std::string, vec_ulong, comp);
