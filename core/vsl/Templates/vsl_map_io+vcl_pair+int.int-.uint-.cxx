#include <utility>
#include <vsl/vsl_map_io.hxx>
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
typedef std::pair<int,int> pii;
typedef std::less<std::pair<int,int> > comp;
VSL_MAP_IO_INSTANTIATE(pii, unsigned, comp);
