// This is core/vbl/vbl_smart_ptr.hxx
#ifndef vbl_smart_ptr_hxx_
#define vbl_smart_ptr_hxx_

#include <iostream>
#include "vbl_smart_ptr.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// Template definitions for ref() and unref().
// The client can specialize them between including this file and
// calling the instantiation macros.
template <class T>
void vbl_smart_ptr<T>::ref(T *p)
{
  if (p)
    p->ref();
}

template <class T>
void vbl_smart_ptr<T>::unref(T *p)
{
  if (p)
    p->unref();
}

template <class T>
struct vbl_smart_ptr_T_as_string { static char const *str() { return "T"; } };

template <class T>
std::ostream& operator<< (std::ostream& os, vbl_smart_ptr<T> const& r)
{
  return os << "vbl_smart_ptr<"
            << vbl_smart_ptr_T_as_string<T>::str()
            << ">(" << (void*) r.as_pointer() << ')';
}

//--------------------------------------------------------------------------------

#undef  VBL_SMART_PTR_INSTANTIATE
#define VBL_SMART_PTR_INSTANTIATE(T) \
template class vbl_smart_ptr<T >; \
template <> struct vbl_smart_ptr_T_as_string<T > \
{ static char const *str() { return #T; } }; \
template std::ostream& operator<< (std::ostream&, vbl_smart_ptr<T > const&)

#endif // vbl_smart_ptr_hxx_
