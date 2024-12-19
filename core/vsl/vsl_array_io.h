// This is core/vsl/vsl_array_io.h
#ifndef vsl_array_io_h_
#define vsl_array_io_h_
//:
// \file
// \brief   binary IO functions for std::array<T, N>
// \author  Noah Johnson

#include <array>
#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class vsl_b_ostream;
class vsl_b_istream;

//: Write array to binary stream
template <class T, size_t N>
void
vsl_b_write(vsl_b_ostream & os, const std::array<T, N> & v);

//: Read array from binary stream
template <class T, size_t N>
void
vsl_b_read(vsl_b_istream & is, std::array<T, N> & v);

//: Print human readable summary of array to a stream
template <class T, size_t N>
void
vsl_print_summary(std::ostream & os, const std::array<T, N> & v);

#endif // vsl_array_io_h_
