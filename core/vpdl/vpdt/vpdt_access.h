// This is core/vpdl/vpdt/vpdt_access.h
#ifndef vpdt_traits_h_
#define vpdt_traits_h_
//:
// \file
// \author Matthew Leotta
// \brief Overloaded functions to allow uniform API access to various field types
// \date March 5, 2009
//
// Since the same template code may apply to both scalars and vectors,
// we need a standard set of functions to treat scalars as a 1-d vector.
// Likewise, we need uniform access to variable and fixed size vectors.
// This includes functions to access dimension, set dimension, access elements,
// and more.
//
// \verbatim
//  Modifications
//   <None yet>
// \endverbatim

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vpdl/vpdt/vpdt_eigen_sym_matrix.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//==============================================================================
// vpdt_size

//: Access the size of a vnl_vector
template <class T>
inline unsigned int vpdt_size(const vnl_vector<T>& v) { return v.size(); }

//: Access the size of a square vnl_matrix
template <class T>
inline unsigned int vpdt_size(const vnl_matrix<T>& m)
{
  assert(m.cols() == m.rows());
  return m.cols();
}

//: Access the size of a vnl_vector_fixed
template <class T, unsigned int n>
inline unsigned int vpdt_size(const vnl_vector_fixed<T,n>& /*v*/) { return n; }

//: Access the size of a square vnl_matrix_fixed
template <class T, unsigned int n>
inline unsigned int vpdt_size(const vnl_matrix_fixed<T,n,n>& /*m*/) { return n; }

//: Access the size of a scalar
inline unsigned int vpdt_size(float /*v*/) { return 1; }
inline unsigned int vpdt_size(double /*v*/) { return 1; }

//==============================================================================
// vpdt_set_size

//: Set the size of a vnl_vector
template <class T>
inline void vpdt_set_size(vnl_vector<T>& v, unsigned int s) { v.set_size(s); }

//: Set the size of a square vnl_matrix
template <class T>
inline void vpdt_set_size(vnl_matrix<T>& m, unsigned int s) { m.set_size(s,s); }

//: Default case, do nothing
template <class T>
inline void vpdt_set_size(T& /*v*/, unsigned int /*s*/) {}

//==============================================================================
// vpdt_fill

//: Fill a vnl_vector
template <class T>
inline void vpdt_fill(vnl_vector<T>& v, const T& val) { v.fill(val); }

//: Fill a square vnl_matrix
template <class T>
inline void vpdt_fill(vnl_matrix<T>& m, const T& val) { m.fill(val); }

//: Fill a vnl_vector_fixed
template <class T, unsigned int n>
inline void vpdt_fill(vnl_vector_fixed<T,n>& v, const T& val) { v.fill(val); }

//: Fill a square vnl_matrix_fixed
template <class T, unsigned int n>
inline void vpdt_fill(vnl_matrix_fixed<T,n,n>& m, const T& val) { m.fill(val); }

//: Default case, assignment
template <class T>
inline void vpdt_fill(T& v, const T& val) { v = val; }

//==============================================================================
// vpdt_index (vector)

//: Index into a vnl_vector
template <class T>
inline T& vpdt_index(vnl_vector<T>& v, unsigned int i) { assert(i < v.size()); return v[i]; }
//: Index into a vnl_vector (const)
template <class T>
inline const T& vpdt_index(const vnl_vector<T>& v, unsigned int i) { assert(i < v.size()); return v[i]; }

//: Index into a vnl_vector_fixed
template <class T, unsigned int n>
inline T& vpdt_index(vnl_vector_fixed<T,n>& v, unsigned int i) { assert(i < n); return v[i]; }
//: Index into a vnl_vector_fixed (const)
template <class T, unsigned int n>
inline const T& vpdt_index(const vnl_vector_fixed<T,n>& v, unsigned int i) { assert(i < n); return v[i]; }

//: Index into a scalar
template <class T>
inline T& vpdt_index(T& v, unsigned int /*i*/) { return v; }
//: Index into a scalar (const)
template <class T>
inline const T& vpdt_index(const T& v, unsigned int /*i*/) { return v; }

//==============================================================================
// vpdt_index (matrix)

//: Index into a vnl_matrix
template <class T>
inline T& vpdt_index(vnl_matrix<T>& v, unsigned int i, unsigned int j) { assert(i < v.rows() && j < v.columns()); return v(i,j); }
//: Index into a vnl_matrix (const)
template <class T>
inline const T& vpdt_index(const vnl_matrix<T>& v, unsigned int i, unsigned int j) { assert(i < v.rows() && j < v.columns()); return v(i,j); }

//: Index into a vnl_matrix_fixed
template <class T, unsigned int n>
inline T& vpdt_index(vnl_matrix_fixed<T,n,n>& v, unsigned int i, unsigned int j) { assert(i < n && j < n); return v(i,j); }
//: Index into a vnl_matrix_fixed (const)
template <class T, unsigned int n>
inline const T& vpdt_index(const vnl_matrix_fixed<T,n,n>& v, unsigned int i, unsigned int j) { assert(i < n && j < n); return v(i,j); }

//: Index into a scalar
template <class T>
inline T& vpdt_index(T& v, unsigned int /*i*/, unsigned int /*j*/) { return v; }
//: Index into a scalar (const)
template <class T>
inline const T& vpdt_index(const T& v, unsigned int /*i*/, unsigned int /*j*/) { return v; }


//==============================================================================
// misc

//: vnl defines outer_product for vectors but not scalars
inline float outer_product(const float& v1, const float& v2) { return v1*v2; }
inline double outer_product(const double& v1, const double& v2) { return v1*v2; }


#endif // vpdt_traits_h_
