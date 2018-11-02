// Instantiation of IO for std::map<unsigned char, std::vector<vgl_point_2d<double> > >
#include <iostream>
#include <vector>
#include <vsl/vsl_vector_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_2d.h>
#include <vsl/vsl_map_io.hxx>
typedef std::vector<vgl_point_2d<double> > vec_point_2d_double;
typedef std::less<unsigned char> comp;
VSL_MAP_IO_INSTANTIATE(unsigned char, vec_point_2d_double, comp);
