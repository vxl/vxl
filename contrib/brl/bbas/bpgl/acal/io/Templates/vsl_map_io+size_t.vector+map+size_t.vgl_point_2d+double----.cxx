#include <functional>
#include <map>
#include <vector>

#include <vgl/vgl_point_2d.h>
#include <vgl/io/vgl_io_point_2d.h>
#include <vsl/vsl_vector_io.h>

#include <vsl/vsl_map_io.hxx>

#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif


// typedef to avoid comma in macro
typedef std::vector<std::map<size_t, vgl_point_2d<double> > > vmip;

// Used to serialize focus tracks
VSL_MAP_IO_INSTANTIATE(size_t, vmip, std::less<size_t>);
