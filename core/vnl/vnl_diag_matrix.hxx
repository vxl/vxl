// This is core/vnl/vnl_diag_matrix.hxx
#ifndef vnl_diag_matrix_hxx_
#define vnl_diag_matrix_hxx_
//:
// \file

#include <iostream>
#include "vnl_diag_matrix.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//: Return inv(D) * b.
template <class T>
vnl_vector<T> vnl_diag_matrix<T>::solve(vnl_vector<T> const& b) const
{
  const unsigned len = this->size();
  vnl_vector<T> ret(len);
  for (unsigned i = 0; i < len; ++i)
    ret[i] = b(i) / this->operator()(i);
  return ret;
}

//: Return inv(D) * b.
template <class T>
void vnl_diag_matrix<T>::solve(vnl_vector<T> const& b, vnl_vector<T>* out) const
{
  const unsigned len = this->size();
  for (unsigned i = 0; i < len; ++i)
    (*out)[i] = b[i] / this->operator()(i);
}

//: Print in MATLAB diag([1 2 3]) form.
template <class T>
std::ostream& operator<< (std::ostream& s, const vnl_diag_matrix<T>& D)
{
  s << "diag([ ";
  for (unsigned i=0; i<D.rows(); ++i)
    s << D(i,i) << ' ';
  return s << "])";
}

#undef VNL_DIAG_MATRIX_INSTANTIATE
#define VNL_DIAG_MATRIX_INSTANTIATE(T) \
template class VNL_EXPORT vnl_diag_matrix<T >; \
/*template VNL_EXPORT vnl_matrix<T > operator* (vnl_matrix<T > const &, vnl_diag_matrix<T > const &); */\
/*template VNL_EXPORT vnl_matrix<T > operator* (vnl_diag_matrix<T > const &, vnl_matrix<T > const &); */\
/*template VNL_EXPORT vnl_matrix<T > operator+ (vnl_matrix<T > const &, vnl_diag_matrix<T > const &); */\
/*template VNL_EXPORT vnl_matrix<T > operator+ (vnl_diag_matrix<T > const &, vnl_matrix<T > const &); */\
/*template VNL_EXPORT vnl_matrix<T > operator- (vnl_matrix<T > const &, vnl_diag_matrix<T > const &); */\
/*template VNL_EXPORT vnl_matrix<T > operator- (vnl_diag_matrix<T > const &, vnl_matrix<T > const &); */\
/*template VNL_EXPORT vnl_vector<T > operator* (const vnl_vector<T >&, vnl_diag_matrix<T > const &); */\
/*template VNL_EXPORT vnl_vector<T > operator* (vnl_diag_matrix<T > const &, const vnl_vector<T >&); */\
template VNL_EXPORT std::ostream& operator<< (std::ostream& s, vnl_diag_matrix<T > const &)

//template bool epsilon_equals (vnl_diag_matrix<T > const & , vnl_diag_matrix<T > const & , double)

#endif // vnl_diag_matrix_hxx_
