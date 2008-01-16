// This is brcv/seg/bsta/bsta_gaussian_indep.h
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
// \endverbatim

#include "bsta_gaussian.h"
#include <vnl/vnl_vector_fixed.h>


//: A Gaussian distribution, independent in each dimension 
// Thus, the covariance matrix is diagonal
template <class T, unsigned n>
class bsta_gaussian_indep : public bsta_gaussian<T,n>
{
  public:
    //: Constructor
    bsta_gaussian_indep<T,n>():
      bsta_gaussian<T,n>(), diag_covar_(T(0)), det_covar_(T(0)) {}

    //: Constructor
    bsta_gaussian_indep<T,n>(const vnl_vector_fixed<T,n>& mean,
                               const vnl_vector_fixed<T,n>& covar):
      bsta_gaussian<T,n>(mean), diag_covar_(covar), det_covar_(T(-1))
    {compute_det();}

    //: The diagonal covariance of the distribution
    const vnl_vector_fixed<T,n>& diag_covar() const
    { return diag_covar_; }

    //: Set the diagonal covariance of the distribution
    void set_covar(const vnl_vector_fixed<T,n>& diag_covar)
    { diag_covar_ = diag_covar; compute_det(); }

    //: The probability of this sample given square mahalanobis distance
    T dist_probability(const T& sqr_mahal_dist) const
    {
      if(det_covar_ <= 0)
        return T(0);
      return static_cast<T>(vcl_sqrt(1/(det_covar_*two_pi_power<n>::value()))
           * vcl_exp(-sqr_mahal_dist/2));
    }

    //: The probability of this sample
    T probability(const vnl_vector_fixed<T,n>& pt) const
    {
      return dist_probability(sqr_mahalanobis_dist(pt));
    }

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



#endif // bsta_gaussian_indep_h_
