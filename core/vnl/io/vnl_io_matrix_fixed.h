#ifndef vnl_io_matrix_fixed_h
#define vnl_io_matrix_fixed_h
#ifdef __GNUC__
#pragma interface
#endif

// This is vxl/vnl/io/vnl_io_matrix_fixed.h

//:
// \file 
// \author Louise Butcher
// \date 21-Mar-2001

#include <vsl/vsl_binary_io.h>
#include <vnl/vnl_matrix_fixed.h>

//: Binary save vnl_matrix_fixed to stream.
template <class T, int m, int n>
void vsl_b_write(vsl_b_ostream & os, const vnl_matrix_fixed<T,m,n> & v);

//: Binary load vnl_matrix_fixed from stream.
template <class T, int m,int n>
void vsl_b_read(vsl_b_istream & is, vnl_matrix_fixed<T,m,n> & v);

//: Print human readable summary of object to a stream
template <class T, int m, int n>
void vsl_print_summary(vcl_ostream & os,const vnl_matrix_fixed<T,m,n> & b);


#endif // vnl_io_matrix_fixed_h
