// This is core/vgl/io/vgl_io_h_matrix_2d.h
#ifndef vgl_io_h_matrix_2d_h_
#define vgl_io_h_matrix_2d_h_
//:
// \file
// \brief contains functions vsl_b_write, vsl_b_read and vsl_print_summary
// \date 16 Aug 2010
// \author Gamze Tunali
//
// \verbatim
//  Modifications
//   16 Aug 2010 - Gamze Tunali - creation
// \endverbatim

#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vsl/vsl_binary_io.h>

//: Binary save vgl_h_matrix_2d to stream.
template <class T>
void vsl_b_write(vsl_b_ostream &os, const vgl_h_matrix_2d<T>& t);

//: Binary load vgl_h_matrix_2d from stream.
template <class T>
void vsl_b_read(vsl_b_istream &is, vgl_h_matrix_2d<T>& v);

#endif // #ifndef vgl_io_h_matrix_2d_h_
