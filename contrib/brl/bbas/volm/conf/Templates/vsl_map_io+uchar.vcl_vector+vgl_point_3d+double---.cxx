// Instantiation of IO for vcl_map<unsigned char, vcl_vector<vgl_point_3d<double> > >
#include <vsl/vsl_vector_io.h>
#include <vcl_vector.h>
#include <vgl/vgl_point_3d.h>
#include <vsl/vsl_map_io.hxx>
typedef vcl_vector<vgl_point_3d<double> > vec_point_3d_double;
typedef vcl_less<unsigned char> comp;
VSL_MAP_IO_INSTANTIATE(unsigned char, vec_point_3d_double, comp);
