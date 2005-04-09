#include <vgl/io/vgl_io_point_2d.h>
#include <vsl/vsl_pair_io.h>
#include <vsl/vsl_vector_io.txx>
typedef vcl_pair<vgl_point_2d<double>,vgl_point_2d<double> > pair_pp;
VSL_VECTOR_IO_INSTANTIATE(pair_pp);
