// This is core/vbl/vbl_smart_ptr.txx
#ifndef vbl_smart_ptr_txx_
#define vbl_smart_ptr_txx_

#include "vbl_smart_ptr.h"
#include <vcl_iostream.h>

// These functions should not be defined. This allows smart pointers
// to incomplete types. Furthermore, there should never be a generic
// implementation of these functions. This allows us to specialize the
// functions and not run afoul of the rule that says that all
// specializations must be declared before the template is used. (I
// think.) Anyway, if there is no generic template implemented, there
// is no template to instantiate, and therefore the calls can only be
// resolved by calling the specialization, which means there cannot be
// any ODR violations.
//
// The functions are no longer static member functions of
// vbl_smart_ptr so that their declaration, specialization and
// instantiation are completely decoupled from vbl_smart_ptr. It's
// probably unnecessary, but I feel better if I don't specialize only
// pieces of a class; i.e. I feel better if I don't have to specialize
// vbl_smart_ptr<myclass>::ref and ::unref but don't specialize the
// others.
//
// The VBL_SMART_PTR_INSTANTIATE macro will automatically specialize
// these two functions for the class in question, so you probably
// never need to worry about it.

// The standard implementation for a complete class
#define VBL_SMART_PTR_SPECIALIZE( T )           \
VCL_DEFINE_SPECIALIZATION                       \
void vbl_smart_ptr_ref(T *p)                    \
{                                               \
  if (p)                                        \
    p->ref();                                   \
}                                               \
VCL_DEFINE_SPECIALIZATION                       \
void vbl_smart_ptr_unref(T *p)                  \
{                                               \
  if (p)                                        \
    p->unref();                                 \
}

template <class T>
struct vbl_smart_ptr_T_as_string { static char const *str() { return "T"; } };

template <class T>
vcl_ostream& operator<< (vcl_ostream& os, vbl_smart_ptr<T> const& r)
{
  return os << "vbl_smart_ptr<"
            << vbl_smart_ptr_T_as_string<T>::str()
            << ">(" << (void*) r.as_pointer() << ")";
}

//--------------------------------------------------------------------------------

#undef  VBL_SMART_PTR_INSTANTIATE
#define VBL_SMART_PTR_INSTANTIATE(T) \
VBL_SMART_PTR_SPECIALIZE(T) \
template class vbl_smart_ptr< T >; \
VCL_DEFINE_SPECIALIZATION struct vbl_smart_ptr_T_as_string<T > \
{ static char const *str() { return #T; } }; \
template vcl_ostream& operator<< (vcl_ostream&, vbl_smart_ptr<T > const&); \
VCL_INSTANTIATE_INLINE(bool operator== (vbl_smart_ptr<T > const&, vbl_smart_ptr<T > const&))

#endif // vbl_smart_ptr_txx_
