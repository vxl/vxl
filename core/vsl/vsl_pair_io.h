// This is vxl/vsl/vsl_pair_io.h
#ifndef vsl_pair_io_h_
#define vsl_pair_io_h_
//:
// \file 
// \brief   binary IO functions for vcl_pair<S, T>
// \author  Ian Scott

#include <vcl_iosfwd.h>
#include <vcl_utility.h>

class vsl_b_ostream;
class vsl_b_istream;

//: Write pair to binary stream
template <class S, class T>
void vsl_b_write(vsl_b_ostream& s, const vcl_pair<S, T>& v);

//: Read pair from binary stream
template <class S, class T>
void vsl_b_read(vsl_b_istream& s, vcl_pair<S, T>& v);

//: Print human readable summary of object to a stream
template <class S, class T>
void vsl_print_summary(vcl_ostream & os,const vcl_pair<S, T> &v);

#endif // vsl_pair_io_h_
