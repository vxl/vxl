// Instantiation of IO for std::map<int>
#include <string>
#include <vsl/vsl_map_io.hxx>
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
VSL_MAP_IO_INSTANTIATE(int,std::string, std::less<int>);
