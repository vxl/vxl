#include <functional>

#include <vpgl/vpgl_affine_camera.h>
#include <vpgl/io/vpgl_io_affine_camera.h>

#include <vsl/vsl_map_io.hxx>

#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif


// Used to serialize acams
VSL_MAP_IO_INSTANTIATE(size_t, vpgl_affine_camera<double>, std::less<size_t>);
