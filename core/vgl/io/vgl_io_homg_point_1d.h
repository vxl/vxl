// This is core/vgl/io/vgl_io_homg_point_1d.h
#ifndef vgl_io_homg_point_1d_h_
#define vgl_io_homg_point_1d_h_
//:
// \file
// \author Peter Vanroose
// \date 24 Oct 2002

#include <vgl/vgl_homg_point_1d.h>
#include <vsl/vsl_binary_io.h>

//: Binary save vgl_homg_point_1d to stream.
template <class T>
void vsl_b_write(vsl_b_ostream& os, vgl_homg_point_1d<T> const& p);

//: Binary load vgl_homg_point_1d from stream.
template <class T>
void vsl_b_read(vsl_b_istream& is, vgl_homg_point_1d<T> & p);

//: Print human readable summary of object to a stream
template <class T>
void vsl_print_summary(std::ostream& os,vgl_homg_point_1d<T> const& p);

#endif // vgl_io_homg_point_1d_h_
