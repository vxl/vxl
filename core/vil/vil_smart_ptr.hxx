// This is core/vil/vil_smart_ptr.hxx
#ifndef vil_smart_ptr_hxx_
#define vil_smart_ptr_hxx_

#include <iostream>
#include "vil_smart_ptr.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// Template definitions for ref() and unref().
// The client can specialize them between including this file and
// calling the instantiation macros.
template <class T>
void vil_smart_ptr<T>::ref(T *p)
{
  if (p)
    p->ref();
}

template <class T>
void vil_smart_ptr<T>::unref(T *p)
{
  if (p)
    p->unref();
}

template <class T>
struct vil_smart_ptr_T_as_string { static char const *str() { return "T"; } };

template <class T>
std::ostream& operator<< (std::ostream& os, vil_smart_ptr<T> const& r)
{
  return os << "vil_smart_ptr<"
            << vil_smart_ptr_T_as_string<T>::str()
            << ">(" << static_cast<const void*>( r.as_pointer()) << ')';
}

//------------------------------------------------------------------------------

#undef  VIL_SMART_PTR_INSTANTIATE
#define VIL_SMART_PTR_INSTANTIATE(T,...) \
template class __VA_ARGS__ vil_smart_ptr<T >; \
template <> struct __VA_ARGS__ vil_smart_ptr_T_as_string<T > \
{ static char const *str() { return #T; } }; \
template __VA_ARGS__ std::ostream& operator<< (std::ostream&, vil_smart_ptr<T > const&)

#endif // vil_smart_ptr_hxx_
