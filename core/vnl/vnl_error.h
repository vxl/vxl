// This is core/vnl/vnl_error.h
#ifndef vnl_error_h_
#define vnl_error_h_
//:
//  \file
//  \author fsm
#include "vnl/vnl_export.h"

//: Raise exception for invalid index.
extern VNL_EXPORT void
vnl_error_vector_index(const char * fcn, int index);

//: Raise exception for invalid dimension.
extern VNL_EXPORT void
vnl_error_vector_dimension(const char * fcn, int l1, int l2);

//: Raise exception for using class objects, or chars in (...).
extern VNL_EXPORT void
vnl_error_vector_va_arg(int n);

//: Raise exception for invalid row index.
extern VNL_EXPORT void
vnl_error_matrix_row_index(const char * fcn, unsigned r);

//: Raise exception for invalid col index.
extern VNL_EXPORT void
vnl_error_matrix_col_index(const char * fcn, unsigned c);

//: Raise exception for invalid dimensions.
extern VNL_EXPORT void
vnl_error_matrix_dimension(const char * fcn, int r1, int c1, int r2, int c2);

//: Raise exception for a nonsquare matrix.
extern VNL_EXPORT void
vnl_error_matrix_nonsquare(const char * fcn);

//: Raise exception for using class objects, or chars in (...).
extern VNL_EXPORT void
vnl_error_matrix_va_arg(int n);

//: Template overloads accepting non-int integral types to avoid narrowing casts at call sites.
//  These forward to the original int-based functions after asserting values are in range.
#include <cassert>
#include <climits>
#include <type_traits>

template <typename T,
          typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, int>::value, int>::type = 0>
inline void
vnl_error_vector_index(const char * fcn, T index)
{
  assert(static_cast<long long>(index) >= static_cast<long long>(INT_MIN));
  assert(static_cast<long long>(index) <= static_cast<long long>(INT_MAX));
  vnl_error_vector_index(fcn, static_cast<int>(index));
}

template <typename T1, typename T2,
          typename std::enable_if<std::is_integral<T1>::value && std::is_integral<T2>::value &&
                                  !(std::is_same<T1, int>::value && std::is_same<T2, int>::value), int>::type = 0>
inline void
vnl_error_vector_dimension(const char * fcn, T1 l1, T2 l2)
{
  assert(static_cast<long long>(l1) >= static_cast<long long>(INT_MIN));
  assert(static_cast<long long>(l1) <= static_cast<long long>(INT_MAX));
  assert(static_cast<long long>(l2) >= static_cast<long long>(INT_MIN));
  assert(static_cast<long long>(l2) <= static_cast<long long>(INT_MAX));
  vnl_error_vector_dimension(fcn, static_cast<int>(l1), static_cast<int>(l2));
}

template <typename T1, typename T2, typename T3, typename T4,
          typename std::enable_if<std::is_integral<T1>::value && std::is_integral<T2>::value &&
                                  std::is_integral<T3>::value && std::is_integral<T4>::value &&
                                  !(std::is_same<T1, int>::value && std::is_same<T2, int>::value &&
                                    std::is_same<T3, int>::value && std::is_same<T4, int>::value), int>::type = 0>
inline void
vnl_error_matrix_dimension(const char * fcn, T1 r1, T2 c1, T3 r2, T4 c2)
{
  assert(static_cast<long long>(r1) >= static_cast<long long>(INT_MIN));
  assert(static_cast<long long>(r1) <= static_cast<long long>(INT_MAX));
  assert(static_cast<long long>(c1) >= static_cast<long long>(INT_MIN));
  assert(static_cast<long long>(c1) <= static_cast<long long>(INT_MAX));
  assert(static_cast<long long>(r2) >= static_cast<long long>(INT_MIN));
  assert(static_cast<long long>(r2) <= static_cast<long long>(INT_MAX));
  assert(static_cast<long long>(c2) >= static_cast<long long>(INT_MIN));
  assert(static_cast<long long>(c2) <= static_cast<long long>(INT_MAX));
  vnl_error_matrix_dimension(fcn, static_cast<int>(r1), static_cast<int>(c1), static_cast<int>(r2), static_cast<int>(c2));
}

#endif // vnl_error_h_
