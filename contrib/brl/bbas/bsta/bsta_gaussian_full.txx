// This is brcv/seg/bsta/bsta_gaussian_full.txx
#ifndef bsta_gaussian_full_txx_
#define bsta_gaussian_full_txx_

//:
// \file

#include "bsta_gaussian_full.h"
#include <vcl_cassert.h>
#include <vcl_algorithm.h>
#include <vcl_limits.h>

#include <vnl/algo/vnl_svd.h>
#include <vnl/algo/vnl_determinant.h>
#include <vnl/vnl_inverse.h>


namespace {

//: Unroll the mahalanobis distance off diagonal terms
template <class T, unsigned n, unsigned i, unsigned j>
struct compute_sqr_mahalanobis_helper
{
  static inline T value(const vnl_vector_fixed<T,n>& d,
                        const vnl_matrix_fixed<T,n,n>& inv_covar)
  {
    return 2*d[i-1]*d[j-1]*inv_covar(i-1,j-1)
        + compute_sqr_mahalanobis_helper<T,n,i,j-1>::value(d,inv_covar);
  }
};

//: base case
// this is partial specialization: expect MSVC6 to complain
template <class T, unsigned n, unsigned i>
struct compute_sqr_mahalanobis_helper<T,n,i,0>
{
  static inline T value(const vnl_vector_fixed<T,n>& d,
                        const vnl_matrix_fixed<T,n,n>& inv_covar)
  { return 0; }
};

//: Unroll the mahalanobis distance calculation
template <class T, unsigned n, unsigned i>
struct compute_sqr_mahalanobis
{
static inline T value(const vnl_vector_fixed<T,n>& d,
                      const vnl_matrix_fixed<T,n,n>& inv_covar)
  {
    return d[i-1]*d[i-1]*inv_covar(i-1,i-1)
        + compute_sqr_mahalanobis_helper<T,n,i,i-1>::value(d,inv_covar)
        + compute_sqr_mahalanobis<T,n,i-1>::value(d,inv_covar);
  }
};

//: base case
// this is partial specialization: expect MSVC6 to complain
template <class T, unsigned n>
struct compute_sqr_mahalanobis<T,n,0>
{
  static inline T value(const vnl_vector_fixed<T,n>& d,
                        const vnl_matrix_fixed<T,n,n>& inv_covar)
  { return 0; }
};


//: compute the inverse
template <class T, unsigned n>
inline vnl_matrix_fixed<T,n,n>* make_inverse(const vnl_matrix_fixed<T,n,n>& M)
{
  return new vnl_matrix_fixed<T,n,n>(vnl_svd_inverse(vnl_matrix<T>(M)));
}


template <class T>
inline vnl_matrix_fixed<T,4,4>* make_inverse(const vnl_matrix_fixed<T,4,4>& M)
{ return new vnl_matrix_fixed<T,4,4>(vnl_inverse(M)); }

template <class T>
    inline vnl_matrix_fixed<T,3,3>* make_inverse(const vnl_matrix_fixed<T,3,3>& M)
{ return new vnl_matrix_fixed<T,3,3>(vnl_inverse(M)); }

template <class T>
    inline vnl_matrix_fixed<T,2,2>* make_inverse(const vnl_matrix_fixed<T,2,2>& M)
{ return new vnl_matrix_fixed<T,2,2>(vnl_inverse(M)); }

template <class T>
    inline vnl_matrix_fixed<T,1,1>* make_inverse(const vnl_matrix_fixed<T,1,1>& M)
{ return new vnl_matrix_fixed<T,1,1>(vnl_inverse(M)); }

};



//: The squared Mahalanobis distance to this point
template <class T, unsigned int n>
T
bsta_gaussian_full<T,n>::sqr_mahalanobis_dist(const vnl_vector_fixed<T,n>& pt) const
{
  if(det_covar_<=T(0))
    return vcl_numeric_limits<T>::infinity();
  vnl_vector_fixed<T,n> d = bsta_gaussian<T,n>::mean_-pt;
  //assert((sqr_mahalanobis_dist<T,n,n>::value(d,diag_covar_)) > 0);
  return compute_sqr_mahalanobis<T,n,n>::value(d,inv_covar());
}


//: The squared Mahalanobis distance to this point
template <class T, unsigned int n>
void
bsta_gaussian_full<T,n>::compute_det()
{
  det_covar_ = vnl_determinant(covar_);
}


//: Update the covariance (and clear cached values)
template <class T, unsigned int n>
void 
bsta_gaussian_full<T,n>::set_covar(const vnl_matrix_fixed<T,n,n>& covar) 
{ 
  delete inv_covar_; 
  inv_covar_ = NULL;
  covar_ = covar;
  compute_det();
}


//: Return the inverse of the covariance matrix
// \note this matrix is cached and updated only when needed
template <class T, unsigned int n>
const vnl_matrix_fixed<T,n,n>&
bsta_gaussian_full<T,n>::inv_covar() const
{
  if(!inv_covar_){ 
    vnl_matrix_fixed<T,n,n> C = covar_;
    if(det_covar_ == T(0)){
      // if the matrix is singular we can add a small lambda*I
      // before inverting to avoid divide by zero
      // Is this the best way to handle this?
      T lambda = T(0);
      for(unsigned i=0; i<n; ++i)
        lambda = vcl_max(lambda,C(i,i));
      lambda *= 1e-4f;  
      for(unsigned i=0; i<n; ++i)
        C(i,i) += lambda;
    }
    inv_covar_ = make_inverse(C);
  }
  return *inv_covar_;
}


#define DBSTA_GAUSSIAN_FIXED_INSTANTIATE(T,n) \
template class bsta_gaussian_full<T,n >;


#endif // bsta_gaussian_full_txx_
