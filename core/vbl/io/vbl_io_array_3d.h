// This is core/vbl/io/vbl_io_array_3d.h
#ifndef vbl_io_array_3d_h
#define vbl_io_array_3d_h
//:
// \file
// \author K.Y.McGaul
// \date   22-Mar-2001

#include <iosfwd>
#include <vsl/vsl_fwd.h>
#include <vbl/vbl_array_3d.h>
#include <vcl_compiler.h>

//: Binary save vbl_array_3d to stream.
template <class T>
void vsl_b_write(vsl_b_ostream & os, const vbl_array_3d<T> & v);

//: Binary load vbl_array_3d from stream.
template <class T>
void vsl_b_read(vsl_b_istream & is, vbl_array_3d<T> & v);

//: Print human readable summary of object to a stream
template <class T>
void vsl_print_summary(std::ostream & os,const vbl_array_3d<T> & b);

#endif // vbl_io_array_3d_h
