#ifndef vnl_io_vector_h
#define vnl_io_vector_h
#ifdef __GNUC__
#pragma interface
#endif

#include <vnl/vnl_vector.h>



class vsl_b_ostream;
class vsl_b_istream;
// This is vxl/vnl/io/vnl_io_vector.h

//:
// \file 
// \author Louise Bucther
// \date 20-Mar-2001


//: Binary save vnl_vector to stream.
template <class T>
void vsl_b_write(vsl_b_ostream & os, const vnl_vector<T> & v);

//: Binary load vnl_vector from stream.
template <class T>
void vsl_b_read(vsl_b_istream & is, vnl_vector<T> & v);

//: Print human readable summary of object to a stream
template <class T>
void vsl_print_summary(vcl_ostream & os,const vnl_vector<T> & b);


#endif // vnl_io_vector_h
