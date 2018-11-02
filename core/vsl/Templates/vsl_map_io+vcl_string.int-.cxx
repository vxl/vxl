// Instantiation of IO for std::map<int>
#include <string>
#include <vsl/vsl_map_io.hxx>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
VSL_MAP_IO_INSTANTIATE(std::string, int, std::less<std::string>);
