// This is core/vgl/io/vgl_io_infinite_line_3d.h
#ifndef vgl_io_infinite_line_3d_h_
#define vgl_io_infinite_line_3d_h_
//:
// \file
// \brief contains functions vsl_b_write, vsl_b_read and vsl_print_summary
// \date 16 Mar 2001
// \author Franck Bettinger
//
// \verbatim
//  Modifications
//   2001/03/16 Franck Bettinger    Creation
// \endverbatim

#include <vgl/vgl_infinite_line_3d.h>
#include <vsl/vsl_binary_io.h>

//: Binary save vgl_infinite_line_3d to stream.
template <class T>
void vsl_b_write(vsl_b_ostream &os, const vgl_infinite_line_3d<T>& v);

//: Binary load vgl_infinite_line_3d from stream.
template <class T>
void vsl_b_read(vsl_b_istream &is, vgl_infinite_line_3d<T>& v);

//: Print human readable summary of a vgl_infinite_line_3d object to a stream
template <class T>
void vsl_print_summary(std::ostream& os,const vgl_infinite_line_3d<T>& v);

#endif // #ifndef vgl_io_infinite_line_3d_h_
