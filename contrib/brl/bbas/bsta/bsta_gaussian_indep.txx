// This is brcv/seg/bsta/bsta_gaussian_indep.txx
#ifndef bsta_gaussian_indep_txx_
#define bsta_gaussian_indep_txx_

//:
// \file

#include "bsta_gaussian_indep.h"
#include <vcl_cassert.h>
#include <vcl_limits.h>


namespace {

//: Unroll the mahalanobis distance calculation
template <class T, unsigned n, unsigned index>
struct compute_sqr_mahalanobis
{
  static inline T value(const vnl_vector_fixed<T,n>& d,
                        const vnl_vector_fixed<T,n>& covar)
  {
    return d[index-1]*d[index-1]/covar[index-1]
        + compute_sqr_mahalanobis<T,n,index-1>::value(d,covar);
  }
};

//: base case
// this is partial specialization: expect MSVC6 to complain
template <class T, unsigned n>
struct compute_sqr_mahalanobis<T,n,0>
{
  static inline T value(const vnl_vector_fixed<T,n>& d,
                        const vnl_vector_fixed<T,n>& covar)
  { return 0; }
};


//: Unroll the determinant calculation
template <class T, unsigned n, unsigned index>
struct determinant
{
  static inline T value(const vnl_vector_fixed<T,n>& covar)
  {
    return covar[index-1]*determinant<T,n,index-1>::value(covar);
  }
};

//: base case
// this is partial specialization: expect MSVC6 to complain
template <class T, unsigned n>
struct determinant<T,n,0>
{
  static inline T value(const vnl_vector_fixed<T,n>& covar)
  { return 1; }
};

};



//: The squared Mahalanobis distance to this point
template <class T, unsigned int n>
T
bsta_gaussian_indep<T,n>::sqr_mahalanobis_dist(const vnl_vector_fixed<T,n>& pt) const 
{
  if(det_covar_<=T(0))
    return vcl_numeric_limits<T>::infinity();
  vnl_vector_fixed<T,n> d = bsta_gaussian<T,n>::mean_-pt;
  return compute_sqr_mahalanobis<T,n,n>::value(d,diag_covar_);
}


//: The squared Mahalanobis distance to this point
template <class T, unsigned int n>
void
bsta_gaussian_indep<T,n>::compute_det()
{
  det_covar_ = determinant<T,n,n>::value(diag_covar_);
}



#define DBSTA_GAUSSIAN_INDEP_INSTANTIATE(T,n) \
template class bsta_gaussian_indep<T,n >;


#endif // bsta_gaussian_indep_txx_
