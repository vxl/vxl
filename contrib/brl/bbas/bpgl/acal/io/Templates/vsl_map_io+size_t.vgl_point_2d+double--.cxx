#include <functional>

#include <vgl/vgl_point_2d.h>
#include <vgl/io/vgl_io_point_2d.h>

#include <vsl/vsl_map_io.hxx>

#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif


// Used to serialize focus tracks
VSL_MAP_IO_INSTANTIATE(size_t, vgl_point_2d<double>, std::less<size_t>);
