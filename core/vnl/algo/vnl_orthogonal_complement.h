#ifndef vnl_orthogonal_complement_h_
#define vnl_orthogonal_complement_h_
#ifdef __GNUC__
#pragma interface
#endif

//:
// \file
// \brief For computing the orthogonal complement to a linear subspace.
// \author fsm@robots.ox.ac.uk
//
// \verbatim
// Modifications
// 4/4/01 LSB(Manchester) Tidied documentation
// \endverbatim

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>

//: Return a matrix whose columns span is the orthogonal
// complement of v.
template <class T>
vnl_matrix<T> vnl_orthogonal_complement(vnl_vector<T> const &v);

#if 0
//: Return a matrix whose column span is the orthogonal
// complement of the column span of M.
template <typename T>
vnl_matrix<T> vnl_orthogonal_complement(vnl_matrix<T> const &M);
#endif

#endif // vnl_orthogonal_complement_h_
