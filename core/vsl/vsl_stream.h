// This is core/vsl/vsl_stream.h
#ifndef vsl_stream_h_
#define vsl_stream_h_
//:
// \file
// \brief Allows you to use the stream output operator instead of vsl_print_summary.
// \author Ian Scott
// So instead of
// \code
//   os << "Blah: ";
//   vsl_print_summary(os, blah);
// \endcode
// you can use
// \code
//   os << "Blah: " << vsl_stream_summary(blah);
// \endcode

#include <ostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//: Convert a vsl_print_summary function call to a streamable object.
template <class T>
struct vsl_stream_summary_t
{
  const T& x;
  vsl_stream_summary_t(const T& xx): x(xx) {}
};

template <class S>
inline vsl_stream_summary_t<S> vsl_stream_summary(const S& xx)
{
  return vsl_stream_summary_t<S>(xx);
}


//: Insert conversion object into stream
template <class T>
inline std::ostream& operator <<(std::ostream &os, const vsl_stream_summary_t<T>& sss)
{
  vsl_print_summary(os, sss.x);
  return os;
}


#endif // vsl_stream_h_
