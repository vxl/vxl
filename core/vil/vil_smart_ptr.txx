// This is mul/vil2/vil2_smart_ptr.txx
#ifndef vil2_smart_ptr_txx_
#define vil2_smart_ptr_txx_

#include "vil2_smart_ptr.h"
#include <vcl_iostream.h>

// Template definitions for ref() and unref().
// The client can specialize them between including this file and
// calling the instantiation macros.
template <class T>
void vil2_smart_ptr<T>::ref(T *p)
{
  if (p)
    p->ref();
}

template <class T>
void vil2_smart_ptr<T>::unref(T *p)
{
  if (p)
    p->unref();
}

template <class T>
struct vil2_smart_ptr_T_as_string { static char const *str() { return "T"; } };

template <class T>
vcl_ostream& operator<< (vcl_ostream& os, vil2_smart_ptr<T> const& r)
{
  return os << "vil2_smart_ptr<"
            << vil2_smart_ptr_T_as_string<T>::str()
            << ">(" << static_cast<const void*>( r.as_pointer()) << ")";
}

//--------------------------------------------------------------------------------

#undef  VIL2_SMART_PTR_INSTANTIATE
#define VIL2_SMART_PTR_INSTANTIATE(T) \
template class vil2_smart_ptr<T >; \
VCL_DEFINE_SPECIALIZATION struct vil2_smart_ptr_T_as_string<T > \
{ static char const *str() { return #T; } }; \
template vcl_ostream& operator<< (vcl_ostream&, vil2_smart_ptr<T > const&); \
VCL_INSTANTIATE_INLINE(bool operator== (vil2_smart_ptr<T > const&, vil2_smart_ptr<T > const&))

#endif // vil2_smart_ptr_txx_
