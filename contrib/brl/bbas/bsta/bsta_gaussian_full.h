// This is brl/bbas/bsta/bsta_gaussian_full.h
#ifndef bsta_gaussian_full_h_
#define bsta_gaussian_full_h_
//:
// \file
// \brief A Gaussian distribution with a full covariance matrix
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date January 25, 2006
//
// \verbatim
//  Modifications
//    Jan 21 2008  -  Matt Leotta  -  Rename probability to prob_density and
//                                    add probability integration over a box
// \endverbatim

#include <iostream>
#include "bsta_gaussian.h"
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vcl_compiler.h>

//: A Gaussian distribution with a full covariance matrix
template <class T, unsigned n>
class bsta_gaussian_full : public bsta_gaussian<T,n>
{
 public:
  //: the covariance type
  typedef vnl_matrix_fixed<T,n,n> covar_type;

  //: Constructor
  bsta_gaussian_full()
  : bsta_gaussian<T,n>(), covar_(T(0)), det_covar_(T(0)), inv_covar_(nullptr) {}

  //: Constructor
  bsta_gaussian_full(const vnl_vector_fixed<T,n>& mean,
                     const vnl_matrix_fixed<T,n,n>& covar)
  : bsta_gaussian<T,n>(mean), covar_(covar), det_covar_(T(-1)), inv_covar_(nullptr)
  { compute_det(); }

  //: Destructor
  ~bsta_gaussian_full() { delete inv_covar_; }

  //: The covariance matrix of the distribution
  const covar_type& covar() const { return covar_; }

  //: Set the covariance matrix of the distribution
  void set_covar(const covar_type& covar);

  //: The probability density at this sample given square mahalanobis distance
  T dist_prob_density(const T& sqr_mahal_dist) const
  {
    if (det_covar_ <= 0)
      return T(0);
    return static_cast<T>(std::sqrt(1/(det_covar_*two_pi_power<n>::value()))
           * std::exp(-sqr_mahal_dist/2));
  }

  //: The probability density at this sample
  T prob_density(const vnl_vector_fixed<T,n>& pt) const
  {
    return dist_prob_density(sqr_mahalanobis_dist(pt));
  }

  //: The probability integrated over a box
  T probability(const vnl_vector_fixed<T,n>& /*min_pt*/,
                const vnl_vector_fixed<T,n>& /*max_pt*/) const
  {
    // TODO - This stub needs implementation
    return -1;
  }

  //: The squared Mahalanobis distance to this point
  T sqr_mahalanobis_dist(const vnl_vector_fixed<T,n>& pt) const;

  //: Compute the determinant of the covariance matrix
  T det_covar() const { return det_covar_; }

  //: The inverse covariance matrix of the distribution
  const covar_type& inv_covar() const;

 protected:
  //: The covariance matrix
  covar_type covar_;

  //: The cached covariance determinant
  T det_covar_;

 private:
  //: compute the determinant of the covariance
  void compute_det();

  //: cache for inverse of the covariance matrix
  mutable vnl_matrix_fixed<T,n,n>* inv_covar_;
};

template <class T , unsigned n>
inline std::ostream& operator<< (std::ostream& os,
                                bsta_gaussian_full<T, n> const& g)
{
  return
  os << "gauss_full:mean(" << g.mean() << ")\n"
     << "gauss_full:covar(\n" << g.covar() << ")\n";
}

#endif // bsta_gaussian_full_h_
