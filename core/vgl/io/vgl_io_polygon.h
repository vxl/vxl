// This is core/vgl/io/vgl_io_polygon.h
#ifndef vgl_io_polygon_h
#define vgl_io_polygon_h
//:
// \file
// \author Nick Costen
// \date 20-Mar-2001

#include <vgl/vgl_polygon.h>
#include <vgl/io/vgl_io_point_2d.h>
#include <vsl/vsl_binary_io.h>

//: Binary save vgl_polygon to stream.
template <class T>
void vsl_b_write(vsl_b_ostream &os, const vgl_polygon<T> & v);

//: Binary load vgl_box_3d from stream.
template <class T>
void vsl_b_read(vsl_b_istream &is, vgl_polygon<T> & v);

//: Print human readable summary of object to a stream
template <class T>
void vsl_print_summary(vcl_ostream& os,const vgl_polygon<T> & b);

#endif // vgl_io_polygon_h
