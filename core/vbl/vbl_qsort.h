// This is core/vbl/vbl_qsort.h
#ifndef vbl_qsort_h_
#define vbl_qsort_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Collection of common predicates for library sort routines
// \author awf@robots.ox.ac.uk
// \date   15 Mar 00
//
// \verbatim
//  Modifications
//   971119 AWF Initial version
//   PDA (Manchester) 23/03/2001: Tidied up the documentation
//   Feb.2002 - Peter Vanroose - brief doxygen comment placed on single line
// \endverbatim


#include <vxl_config.h> // VXL_STDLIB_HAS_QSORT

#include <vcl_algorithm.h>
#if VXL_STDLIB_HAS_QSORT
# include <vcl_cstdlib.h>
#else
# include <vcl_functional.h>
# include <vcl_iostream.h>
#endif
#include <vcl_vector.h>

#include <vbl/vbl_sort.h>

#define vbl_qsort_double_ascending  vbl_sort_double_ascending
#define vbl_qsort_double_descending vbl_sort_double_descending
#define vbl_qsort_int_ascending     vbl_sort_int_ascending
#define vbl_qsort_int_descending    vbl_sort_int_descending
#define vbl_qsort_helper            vbl_sort_helper

typedef int (*vbl_qsort_compare_t)(const void* a, const void* b);

//: Sort a C array into ascending order.
//  Do this using the standard comparison operations for T,
//  namely operator> and operator==.
template <class T>
inline
void vbl_qsort_ascending(T* base, int n)
{
#if VXL_STDLIB_HAS_QSORT
  vcl_qsort(base, n, sizeof base[0], vbl_qsort_helper<T>::ascend);
#else
  vcl_sort(base, base+n, vcl_less<T>());
#endif
}

//: Sort a C array into descending order.
//  Do this using the standard comparison operations for T,
//  namely "operator>" and "operator==".
template <class T>
inline
void vbl_qsort_descending(T* base, int n)
{
#if VXL_STDLIB_HAS_QSORT
  vcl_qsort(base, n, sizeof base[0], vbl_qsort_helper<T>::descend);
#else
  vcl_sort(base, base+n, vcl_less<T>());
#endif
}

//: Sort an STL vector into ascending order.
// Do this using the standard comparison operations for T,
// namely operator> and operator==.  I know STL has a sort,
// but this is easier, and faster in the 21st century.
template <class T>
inline
void vbl_qsort_ascending(vcl_vector<T>& v)
{
#if VXL_STDLIB_HAS_QSORT
  vcl_qsort(&v[0], v.size(), sizeof v[0], vbl_qsort_helper<T>::ascend);
#else
  vcl_sort(v.begin(), v.end(), vcl_less<T>());
#endif
}

//: Sort an STL vector into descending order.
// Do this using the standard comparison operations for T,
// namely "operator>" and "operator==".
template <class T>
inline
void vbl_qsort_descending(vcl_vector<T>& v)
{
#if VXL_STDLIB_HAS_QSORT
  //vector<>::iterator
  vcl_qsort(&v[0], v.size(), sizeof v[0], vbl_qsort_helper<T>::descend);
#else
  vcl_sort(v.begin(), v.end(), vcl_greater<T>());
#endif
}

//: Sort STL vector.
template <class T>
inline
void vbl_qsort(vcl_vector<T>& v, int (*compare)(T const& a, T const& b))
{
#if VXL_STDLIB_HAS_QSORT
  //vector<>::iterator
  vcl_qsort(&v[0], v.size(), sizeof v[0], (vbl_qsort_compare_t)compare);
#else
  vcl_cerr << "Sorry, this type of qsort has not been implemented\n";
#endif
}

#define VBL_QSORT_INSTANTIATE(T) \
VCL_INSTANTIATE_INLINE(void vbl_qsort_ascending(T*,int));\
VCL_INSTANTIATE_INLINE(void vbl_qsort_descending(T*,int))

#define VBL_QSORT_INSTANTIATE_vector(T) \
VCL_INSTANTIATE_INLINE(void vbl_qsort(vcl_vector<T >& v, \
                        int (*compare)(T const& a, T const& b)))

#endif // vbl_qsort_h_
