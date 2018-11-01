// This is core/vbl/vbl_qsort.h
#ifndef vbl_qsort_h_
#define vbl_qsort_h_
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


#include <algorithm>
#include <vector>
#include <cstdlib>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
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
  std::qsort(base, n, sizeof base[0], vbl_qsort_helper<T>::ascend);
}

//: Sort a C array into descending order.
//  Do this using the standard comparison operations for T,
//  namely "operator>" and "operator==".
template <class T>
inline
void vbl_qsort_descending(T* base, int n)
{
  std::qsort(base, n, sizeof base[0], vbl_qsort_helper<T>::descend);
}

//: Sort an STL vector into ascending order.
// Do this using the standard comparison operations for T,
// namely operator> and operator==.  I know STL has a sort,
// but this is easier, and faster in the 21st century.
template <class T>
inline
void vbl_qsort_ascending(std::vector<T>& v)
{
  std::qsort(&v[0], v.size(), sizeof v[0], vbl_qsort_helper<T>::ascend);
}

//: Sort an STL vector into descending order.
// Do this using the standard comparison operations for T,
// namely "operator>" and "operator==".
template <class T>
inline
void vbl_qsort_descending(std::vector<T>& v)
{
  std::qsort(&v[0], v.size(), sizeof v[0], vbl_qsort_helper<T>::descend);
}

//: Sort STL vector.
template <class T>
inline
void vbl_qsort(std::vector<T>& v, int (*compare)(T const& a, T const& b))
{
  std::qsort(&v[0], v.size(), sizeof v[0], (vbl_qsort_compare_t)compare);
}

#define VBL_QSORT_INSTANTIATE(T) \
/*template void vbl_qsort_ascending((T)*,int); */\
/*template void vbl_qsort_descending((T)*,int) */

#define VBL_QSORT_INSTANTIATE_vector(T) \
/* template void vbl_qsort(std::vector<T >& v, \
                        int (*compare)(T const& a, T const& b)) */

#endif // vbl_qsort_h_
