#ifndef vnl_resize_h_
#define vnl_resize_h_
/*
  fsm@robots.ox.ac.uk
*/

#include <vcl/vcl_compiler.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>

//
template <class T> void vnl_resize(vnl_vector<T> &v, unsigned newsize);
template <class T> void vnl_resize(vnl_matrix<T> &M, unsigned newrows, unsigned newcols);

//
template <class T> void vnl_assign(vnl_vector<T> &lhs, vnl_vector<T> const &rhs);
template <class T> void vnl_assign(vnl_matrix<T> &lhs, vnl_matrix<T> const &rhs);

#endif
