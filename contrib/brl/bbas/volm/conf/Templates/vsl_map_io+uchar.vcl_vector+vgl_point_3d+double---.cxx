// Instantiation of IO for std::map<unsigned char, std::vector<vgl_point_3d<double> > >
#include <iostream>
#include <vector>
#include <vsl/vsl_vector_io.h>
#include <vcl_compiler.h>
#include <vgl/vgl_point_3d.h>
#include <vsl/vsl_map_io.hxx>
typedef std::vector<vgl_point_3d<double> > vec_point_3d_double;
typedef std::less<unsigned char> comp;
VSL_MAP_IO_INSTANTIATE(unsigned char, vec_point_3d_double, comp);
