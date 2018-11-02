// This is core/vsl/vsl_vector_io.h
#ifndef vsl_vector_io_h_
#define vsl_vector_io_h_
//:
// \file
// \brief binary IO functions for std::vector<T>
// \author Tim Cootes

#include <iosfwd>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_fwd.h>

//: Write vector to binary stream
template <class T>
void vsl_b_write(vsl_b_ostream& s, const std::vector<T>& v);

//: Read vector from binary stream
template <class T>
void vsl_b_read(vsl_b_istream& s, std::vector<T>& v);

//: Print human readable summary of object to a stream
template <class T>
void vsl_print_summary(std::ostream & os,const std::vector<T> &v);


//: Write vector<bool> to binary stream
template <>
void vsl_b_write(vsl_b_ostream& s, const std::vector<bool>& v);

//: Read vector<bool> from binary stream
template <>
void vsl_b_read(vsl_b_istream& s, std::vector<bool>& v);

//: Print human readable summary of object to a stream
template <>
void vsl_print_summary(std::ostream & os,const std::vector<bool> &v);

#endif // vsl_vector_io_h_
