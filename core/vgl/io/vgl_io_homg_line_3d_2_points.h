// This is core/vgl/io/vgl_io_homg_line_3d_2_points.h
#ifndef vgl_io_homg_line_3d_2_points_h
#define vgl_io_homg_line_3d_2_points_h
//:
// \file
// \author John KANG (Manchester)
// \date 21-Mar-2001

#include <vgl/vgl_homg_line_3d_2_points.h>
#include <vsl/vsl_binary_io.h>

//: Binary save vgl_homg_line_3d_2_points to stream.
template <class T>
void vsl_b_write(vsl_b_ostream &os, const vgl_homg_line_3d_2_points<T> & p);

//: Binary load vgl_homg_line_3d_2_points from stream.
template <class T>
void vsl_b_read(vsl_b_istream &is, vgl_homg_line_3d_2_points<T> & p);

//: Print human readable summary of object to a stream
template <class T>
void vsl_print_summary(std::ostream& os,const vgl_homg_line_3d_2_points<T> & p);

#endif // vgl_io_homg_line_3d_2_points_h
