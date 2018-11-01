// This is core/vil1/vil1_ip_traits.h
#ifndef vil1_ip_traits_h_
#define vil1_ip_traits_h_
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
//:
// \file
// \brief Describe image types
// \author   awf@robots.ox.ac.uk
// \date 02 Mar 00

struct vil1_ip_traits_null_type {};

//: Describe image types (like char**)
template <class T>
struct vil1_ip_traits
{
  //: The return type of operator[][]
  typedef vil1_ip_traits pixel_type;

  //: The return type of operator[]
  typedef vil1_ip_traits row_type;
};

//: Traits for c-like arrays
template <class T>
struct vil1_ip_traits<T * *>
{
  typedef T pixel_type;
  typedef T* row_type;
};

template <class T>
struct vil1_ip_traits<T const* const*>
{
  typedef T pixel_type;
  typedef T const* row_type;
};

template <class T>
struct vil1_ip_traits<T * const*>
{
  typedef T pixel_type;
  typedef T const* row_type;
};

#endif // vil1_ip_traits_h_
