#ifndef vcl_new_h_
#define vcl_new_h_
/*
  fsm
*/

#include "vcl_compiler.h"


// -------------------- gcc with old library
#if defined(VCL_GCC) && !defined(GNU_LIBSTDCXX_V3)
# include <new.h>
// -------------------- iso
#else
# include <new>
#endif

// Provide vcl_destroy() and vcl_construct() :
template <class T>
inline
void vcl_destroy(T *p) { p->~T(); }

template <class U, class V>
inline
void vcl_construct(U * p, V const & value) { new (p) U(value); }

#define vcl_bad_alloc std::bad_alloc
#define vcl_set_new_handler std::set_new_handler

#endif // vcl_new_h_
