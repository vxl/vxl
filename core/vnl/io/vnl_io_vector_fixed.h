#ifndef vnl_io_vector_fixed_h
#define vnl_io_vector_fixed_h
#ifdef __GNUC__
#pragma interface
#endif

// This is vxl/vnl/io/vnl_io_vector_fixed.h

//:
// \file 
// \author Louise Butcher
// \date 21-Mar-2001

#include <vsl/vsl_binary_io.h>
#include <vnl/vnl_vector_fixed.h>


//: Binary save vnl_vector_fixed to stream.
template <class T, int n>
void vsl_b_write(vsl_b_ostream & os, const vnl_vector_fixed<T,n> & v);

//: Binary load vnl_vector_fixed from stream.
template <class T, int n>
void vsl_b_read(vsl_b_istream & is, vnl_vector_fixed<T,n> & v);

//: Print human readable summary of object to a stream
template <class T, int n>
void vsl_print_summary(vcl_ostream & os,const vnl_vector_fixed<T,n> & b);


#endif // vnl_io_vector_fixed_h
