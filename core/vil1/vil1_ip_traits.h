// This is core/vil1/vil1_ip_traits.h
#ifndef vil1_ip_traits_h_
#define vil1_ip_traits_h_
#include <vcl_config_compiler.h>
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
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

#if VCL_CAN_DO_PARTIAL_SPECIALIZATION
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
#else
// It's like the good old days... #define declare_list(T)
#define VIL1_IP_TRAITS_DECLARE(T, RT, PT) \
VCL_DEFINE_SPECIALIZATION struct vil1_ip_traits<T > {\
 typedef PT pixel_type;\
 typedef RT row_type;\
 };

#define VIL1_IP_TRAITS_DECLARE_ALL(T)\
VIL1_IP_TRAITS_DECLARE(T       *      *, T      *, T)\
VIL1_IP_TRAITS_DECLARE(T       * const*, T      *, T)\
VIL1_IP_TRAITS_DECLARE(T       * const* const, T      * const, T)\
VIL1_IP_TRAITS_DECLARE(T  const* const*, T const*, T)

VIL1_IP_TRAITS_DECLARE_ALL(unsigned char)
VIL1_IP_TRAITS_DECLARE_ALL(short)
VIL1_IP_TRAITS_DECLARE_ALL(int)
VIL1_IP_TRAITS_DECLARE_ALL(float)

#endif // VCL_CAN_DO_PARTIAL_SPECIALIZATION

#endif // vil1_ip_traits_h_
