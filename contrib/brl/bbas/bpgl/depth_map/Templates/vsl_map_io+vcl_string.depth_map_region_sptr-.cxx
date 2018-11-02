#include <iostream>
#include <string>
#include <depth_map/depth_map_region_sptr.h>
#include <vsl/vsl_map_io.hxx>
#include <vbl/io/vbl_io_smart_ptr.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
VSL_MAP_IO_INSTANTIATE(std::string, depth_map_region_sptr, std::less<std::string>);
