// This is core/vgl/io/vgl_io_homg_plane_3d.h
#ifndef vgl_io_homg_plane_3d_h
#define vgl_io_homg_plane_3d_h
//:
// \file
// \author Christine Beeston
// \date 21-Mar-2001

#include <vgl/vgl_homg_plane_3d.h>
#include <vsl/vsl_binary_io.h>

//: Binary save vgl_homg_plane_3d to stream.
template <class T>
void vsl_b_write(vsl_b_ostream &os, const vgl_homg_plane_3d<T> & v);

//: Binary load vgl_homg_plane_3d from stream.
template <class T>
void vsl_b_read(vsl_b_istream &is, vgl_homg_plane_3d<T> & v);

//: Print human readable summary of object to a stream
template <class T>
void vsl_print_summary(std::ostream& os,const vgl_homg_plane_3d<T> & b);

#endif // vgl_io_homg_plane_3d_h
