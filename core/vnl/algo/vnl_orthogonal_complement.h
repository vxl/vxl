#ifndef vnl_orthogonal_complement_h_
#define vnl_orthogonal_complement_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
*/

// Convenience functions for computing the orthogonal
// complement to a linear subspace.

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>

//: return a matrix whose columns span is the orthogonal
// complement of v.
template <class T>
vnl_matrix<T> vnl_orthogonal_complement(vnl_vector<T> const &v);

// //: return a matrix whose column span is the orthogonal 
// // complement of the column span of M.
// template <typename T>
// vnl_matrix<T> vnl_orthogonal_complement(vnl_matrix<T> const &M);

#endif // vnl_orthogonal_complement_h_
