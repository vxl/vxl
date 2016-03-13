// This is core/vsl/vsl_pair_io.h
#ifndef vsl_pair_io_h_
#define vsl_pair_io_h_
//:
// \file
// \brief   binary IO functions for std::pair<S, T>
// \author  Ian Scott

#include <iosfwd>
#include <utility>
#include <vcl_compiler.h>

class vsl_b_ostream;
class vsl_b_istream;

//: Write pair to binary stream
template <class S, class T>
void vsl_b_write(vsl_b_ostream& s, const std::pair<S, T>& v);

//: Read pair from binary stream
template <class S, class T>
void vsl_b_read(vsl_b_istream& s, std::pair<S, T>& v);

//: Print human readable summary of object to a stream
template <class S, class T>
void vsl_print_summary(std::ostream & os,const std::pair<S, T> &v);

#endif // vsl_pair_io_h_
