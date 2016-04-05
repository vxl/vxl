// This is core/vgl/io/vgl_io_box_2d.h
#ifndef vgl_io_box_2d_h
#define vgl_io_box_2d_h
//:
// \file
// \author dac
// \date 20-Mar-2001

#include <vgl/vgl_box_2d.h>
#include <vsl/vsl_binary_io.h>

//: Binary save vgl_box_2d to stream.
template <class T>
void vsl_b_write(vsl_b_ostream &os, const vgl_box_2d<T> & v);

//: Binary load vgl_box_2d from stream.
template <class T>
void vsl_b_read(vsl_b_istream &is, vgl_box_2d<T> & v);

//: Print human readable summary of object to a stream
template <class T>
void vsl_print_summary(std::ostream& os,const vgl_box_2d<T> & b);

#endif // vgl_io_box_2d_h
