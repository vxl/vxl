#ifndef vnl_io_real_npolynomial_h
#define vnl_io_real_npolynomial_h
#ifdef __GNUC__
#pragma interface
#endif

#include <vsl/vsl_binary_io.h>
#include <vnl/vnl_real_npolynomial.h>

// This is vxl/vnl/io/vnl_io_real_npolynomial.h

//:
// \file 
// \author dac
// \date 21-Mar-2001

//: Binary save vnl_real_npolynomial to stream.
void vsl_b_write(vsl_b_ostream &os, const vnl_real_npolynomial & v);

//: Binary load vnl_real_polynomial from stream.
void vsl_b_read(vsl_b_istream &is, vnl_real_npolynomial & v);

//: Print human readable summary of object to a stream
void vsl_print_summary(vcl_ostream& os,const vnl_real_npolynomial & b);

#endif // vnl_io_real_npolynomial_h



