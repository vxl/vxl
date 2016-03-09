// Instantiation of IO for std::map<int>
#include <vsl/vsl_map_io.hxx>
#include <vcl_compiler.h>
#include <string>
VSL_MAP_IO_INSTANTIATE(int,std::string, std::less<int>);
