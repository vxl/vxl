// This is vxl/vsl/vsl_vector_io.h
#ifndef vsl_vector_io_h_
#define vsl_vector_io_h_
//:
// \file 
// \brief binary IO functions for vcl_vector<T>
// \author Tim Cootes

#include <vcl_iosfwd.h>
#include <vcl_vector.h>
#include <vsl/vsl_fwd.h>

//: Write vector to binary stream
template <class T>
void vsl_b_write(vsl_b_ostream& s, const vcl_vector<T>& v);

//: Read vector from binary stream
template <class T>
void vsl_b_read(vsl_b_istream& s, vcl_vector<T>& v);

//: Print human readable summary of object to a stream
template <class T>
void vsl_print_summary(vcl_ostream & os,const vcl_vector<T> &v);

#endif // vsl_vector_io_h_
