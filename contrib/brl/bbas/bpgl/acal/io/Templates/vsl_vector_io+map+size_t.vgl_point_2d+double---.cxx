#include <map>

#include <vgl/vgl_point_2d.h>
#include <vgl/io/vgl_io_point_2d.h>
#include <vsl/vsl_map_io.h>

#include <vsl/vsl_vector_io.hxx>

#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif


// typedef to avoid comma in macro
typedef std::map<size_t, vgl_point_2d<double> > mip;

// Used to serialize focus tracks
VSL_VECTOR_IO_INSTANTIATE(mip);
