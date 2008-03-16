// This is brl/bbas/bsta/bsta_gaussian_indep.h
#ifndef bsta_gaussian_indep_h_
#define bsta_gaussian_indep_h_
//:
// \file
// \brief A Gaussian distribution, independent in each dimension
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 1/25/06
//
// \verbatim
//  Modifications
//    Jan 21 2008  -  Matt Leotta  -  Rename probability to prob_density and
//                                    add probability integration over a box
// \endverbatim

#include "bsta_gaussian.h"
#include <vnl/vnl_vector_fixed.h>
#include <vcl_iostream.h>

//: A Gaussian distribution, independent in each dimension
// Thus, the covariance matrix is diagonal
template <class T, unsigned n>
class bsta_gaussian_indep : public bsta_gaussian<T,n>
{
 public:
  //: Constructor
  bsta_gaussian_indep<T,n>()
   : bsta_gaussian<T,n>(), diag_covar_(T(0)), det_covar_(T(0)) {}

  //: Constructor
  bsta_gaussian_indep<T,n>(const vnl_vector_fixed<T,n>& mean,
                             const vnl_vector_fixed<T,n>& covar)
   : bsta_gaussian<T,n>(mean), diag_covar_(covar), det_covar_(T(-1))
  {compute_det();}

  //: The diagonal covariance of the distribution
  const vnl_vector_fixed<T,n>& diag_covar() const
  { return diag_covar_; }

  //: Set the diagonal covariance of the distribution
  void set_covar(const vnl_vector_fixed<T,n>& diag_covar)
  { diag_covar_ = diag_covar; compute_det(); }

  //: The probability density at this sample given square mahalanobis distance
  T dist_prob_density(const T& sqr_mahal_dist) const
  {
    if (det_covar_ <= 0)
      return T(0);
    return static_cast<T>(vcl_sqrt(1/(det_covar_*two_pi_power<n>::value()))
         * vcl_exp(-sqr_mahal_dist/2));
  }

  //: The probability density at this sample
  T prob_density(const vnl_vector_fixed<T,n>& pt) const
  {
    return dist_prob_density(sqr_mahalanobis_dist(pt));
  }

  //: The probability that a sample lies inside a n-d box
  T probability(const vnl_vector_fixed<T,n>& min_pt,
                const vnl_vector_fixed<T,n>& max_pt) const;

  //: The squared Mahalanobis distance to this point
  T sqr_mahalanobis_dist(const vnl_vector_fixed<T,n>& pt) const;

  //: Compute the determinant of the covariance matrix
  T det_covar() const { return det_covar_; }

 protected:
  //: The diagonal covariance matrix stored as a vector
  vnl_vector_fixed<T,n> diag_covar_;

  //: The cached covariance determinant
  T det_covar_;

 private:
  //: compute the determinant of the covariance
  void compute_det();
};

template <class T , unsigned n>
inline vcl_ostream& operator<< (vcl_ostream& os,
                                bsta_gaussian_indep<T, n> const& g)
{
  os << "gauss_indep:mean(" << g.mean() << ")\n"
     << "gauss_indep:covar(" << g.diag_covar() << ")\n";
  return os;
}

#endif // bsta_gaussian_indep_h_
