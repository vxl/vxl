// This is core/vgl/io/vgl_io_homg_line_2d.h
#ifndef vgl_io_homg_line_2d_h_
#define vgl_io_homg_line_2d_h_
//:
// \file
// \brief contains functions vsl_b_write, vsl_b_read and vsl_print_summary
// \date 21 Mar 2001
// \author Franck Bettinger
//
// \verbatim
//  Modifications
//   2001/03/21 Franck Bettinger  Creation
// \endverbatim

#include <vgl/vgl_homg_line_2d.h>
#include <vsl/vsl_binary_io.h>

//: Binary save vgl_homg_line_2d to stream.
template <class T>
void vsl_b_write(vsl_b_ostream &os, const vgl_homg_line_2d<T>& v);

//: Binary load vgl_homg_line_2d from stream.
template <class T>
void vsl_b_read(vsl_b_istream &is, vgl_homg_line_2d<T>& v);

//: Print human readable summary of a vgl_homg_line_2d object to a stream
template <class T>
void vsl_print_summary(std::ostream& os,const vgl_homg_line_2d<T>& v);

#endif // #ifndef vgl_io_homg_line_2d_h_
