#include <depth_map/depth_map_region_sptr.h>
#include <vsl/vsl_map_io.txx>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vcl_string.h>
VSL_MAP_IO_INSTANTIATE(vcl_string, depth_map_region_sptr, vcl_less<vcl_string>);
