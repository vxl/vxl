//-*- c++ -*-------------------------------------------------------------------
#ifndef vbl_qsort_h_
#define vbl_qsort_h_
#ifdef __GNUC__
#pragma interface
#endif
// Author: awf@robots.ox.ac.uk
// Created: 15 Mar 00

//: Collection of common predicates for library sort routines

#include <vcl/vcl_cstdlib.h>
#include <vcl/vcl_vector.h>

int vbl_qsort_double_ascending(double const&, double const&);
int vbl_qsort_double_descending(double const&, double const&);

int vbl_qsort_int_ascending(int const&, int const&);
int vbl_qsort_int_descending(int const&, int const&);

typedef int (*vbl_qsort_compare_t)(const void* a, const void* b);

template <class T> 
struct vbl_qsort_helper {
  static int ascend(const void* a, const void* b) {
    const T& ta = *((const T*)a);
    const T& tb = *((const T*)b);
    if (tb > ta)
      return -1;
    if (tb == ta)
      return 0;
    return 1;
  }
  static int descend(const void* a, const void* b) {
    return 1 - ascend(a,b);
  }
};

// -- Sort a C array into ascending order, using the standard comparison
// operations for T, namely operator> and operator==.
template <class T>
inline
void vbl_qsort_ascending(T* base, int n)
{
  qsort(base, n, sizeof base[0], vbl_qsort_helper<T>::ascend);
}

// -- Sort a C array into descending order, using the standard comparison
// operations for T, namely "operator>" and "operator==".
template <class T>
inline
void vbl_qsort_descending(T* base, int n)
{
  qsort(base, n, sizeof base[0], vbl_qsort_helper<T>::ascend);
}

// -- Sort an STL vector into ascending order, using the standard comparison
// operations for T, namely operator> and operator==.  I know STL has a sort,
// but this is easier, and faster in the 20th century.
template <class T>
inline
void vbl_qsort_ascending(vcl_vector<T>& v)
{
  qsort(v.begin(), v.size(), sizeof v[0], vbl_qsort_helper<T>::ascend);
}

// -- Sort an STL vector into descending order, using the standard comparison
// operations for T, namely "operator>" and "operator==".
template <class T>
inline
void vbl_qsort_descending(vcl_vector<T>& v)
{
  qsort(v.begin(), v.size(), sizeof v[0], vbl_qsort_helper<T>::descend);
}

// -- Sort STL vector.
template <class T>
inline
void vbl_qsort(vcl_vector<T>& v, int (*compare)(T const& a, T const& b))
{
  qsort(v.begin(), v.size(), sizeof v[0], (vbl_qsort_compare_t)compare);
}

#define VBL_QSORT_INSTANTIATE(T)\
template struct vbl_qsort_helper<T>;\
VCL_INSTANTIATE_INLINE(void vbl_qsort_ascending(T*,int));\
VCL_INSTANTIATE_INLINE(void vbl_qsort_descending(T*,int));

#define VBL_QSORT_INSTANTIATE_vector(T)\
VCL_INSTANTIATE_INLINE(void vbl_qsort(vcl_vector<T>& v, int (*compare)(T const& a, T const& b)))

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vbl_qsort.
