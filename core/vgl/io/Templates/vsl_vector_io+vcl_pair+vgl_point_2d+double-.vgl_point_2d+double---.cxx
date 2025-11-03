#include <vgl/io/vgl_io_point_2d.h>
#include "vsl/vsl_pair_io.h"
#include "vsl/vsl_vector_io.hxx"
using pair_pp = std::pair<vgl_point_2d<double>, vgl_point_2d<double>>;
VSL_VECTOR_IO_INSTANTIATE(pair_pp);
