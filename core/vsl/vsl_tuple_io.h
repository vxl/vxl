// This is core/vsl/vsl_tuple_io.h
#ifndef vsl_tuple_io_h_
#define vsl_tuple_io_h_
//:
// \file
// \brief   binary IO functions for std::tuple<T1, T2, ...>
// \author  Noah Johnson

#include <iosfwd>
#include <tuple>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class vsl_b_ostream;
class vsl_b_istream;

//: Write tuple to binary stream
template <typename... Types>
void
vsl_b_write(vsl_b_ostream & os, const std::tuple<Types...> & v);

//: Read tuple from binary stream
template <typename... Types>
void
vsl_b_read(vsl_b_istream & is, std::tuple<Types...> & v);

//: Print human readable summary of tuple to a stream
template <typename... Types>
void
vsl_print_summary(std::ostream & os, const std::tuple<Types...> & v);

#endif // vsl_tuple_io_h_
