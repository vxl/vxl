// This is core/vgl/io/vgl_io_line_3d_2_points.h
#ifndef vgl_io_line_3d_2_points_h_
#define vgl_io_line_3d_2_points_h_
//:
// \file
// \author Peter Vanroose
// \date 24 Oct 2002

#include <vgl/vgl_line_3d_2_points.h>
#include <vsl/vsl_binary_io.h>

//: Binary save vgl_line_3d_2_points to stream.
template <class T>
void vsl_b_write(vsl_b_ostream& os, vgl_line_3d_2_points<T> const& p);

//: Binary load vgl_line_3d_2_points from stream.
template <class T>
void vsl_b_read(vsl_b_istream& is, vgl_line_3d_2_points<T> & p);

//: Print human readable summary of object to a stream
template <class T>
void vsl_print_summary(std::ostream& os, vgl_line_3d_2_points<T> const& p);

#endif // vgl_io_line_3d_2_points_h_
