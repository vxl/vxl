#include <vsl/vsl_vector_io.txx>
#include <vsl/vsl_pair_io.txx>
#include <vgl/io/vgl_io_point_2d.h>
typedef vcl_pair<vgl_point_2d<double>,vgl_point_2d<double> > pair_pp;
VSL_VECTOR_IO_INSTANTIATE(pair_pp);
