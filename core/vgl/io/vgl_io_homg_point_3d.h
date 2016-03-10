// This is core/vgl/io/vgl_io_homg_point_3d.h
#ifndef vgl_io_homg_point_3d_h
#define vgl_io_homg_point_3d_h
//:
// \file
// \author P D Allen
// \date 20-Mar-2001

#include <vgl/vgl_homg_point_3d.h>
#include <vsl/vsl_binary_io.h>

//: Binary save vgl_homg_point_3d to stream.
template <class T>
void vsl_b_write(vsl_b_ostream &os, const vgl_homg_point_3d<T> & p);

//: Binary load vgl_homg_point_3d from stream.
template <class T>
void vsl_b_read(vsl_b_istream &is, vgl_homg_point_3d<T> & p);

//: Print human readable summary of object to a stream
template <class T>
void vsl_print_summary(std::ostream& os,const vgl_homg_point_3d<T> & p);

#endif // vgl_io_homg_point_3d_h
