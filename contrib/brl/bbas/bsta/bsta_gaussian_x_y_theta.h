// This is brcv/seg/bsta/bsta_gaussian_x_y_theta.h
#ifndef bsta_gaussian_x_y_theta_h_
#define bsta_gaussian_x_y_theta_h_

//:
// \file
// \brief A Gaussian distribution with a full covariance matrix
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 1/25/06
//
// \verbatim
//  Modifications
// \endverbatim

#include "bsta_gaussian.h"
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>


//: A Gaussian distribution with a full covariance matrix

class bsta_gaussian_x_y_theta : public bsta_gaussian<float,3>
{
  public:
    //: Constructor
    bsta_gaussian_x_y_theta():
      bsta_gaussian<float,3>(), covar_(float(0)),
      det_covar_(float(0)) {}

    //: Constructor
    bsta_gaussian_x_y_theta(const vnl_vector_fixed<float,3>& mean,
                              const vnl_matrix_fixed<float,3,3>& covar):
      bsta_gaussian<float,3>(mean), covar_(covar),
      det_covar_(float(-1))
    {compute_det();}

    //: Destructor
    ~bsta_gaussian_x_y_theta() {  }

    //: The covariance matrix of the distribution
    const vnl_matrix_fixed<float,3,3>& covar() const { return covar_; }

    //: Set the covariance matrix of the distribution
    void set_covar(const vnl_matrix_fixed<float,3,3>& covar);

    //: The probability of this sample given square mahalanobis distance
    float dist_probability(const float& sqr_mahal_dist) const
    {
      if(det_covar_ <= 0)
        return 0;
      return static_cast<float>(vcl_sqrt(1/(det_covar_*two_pi_power<3>::value()))
           * vcl_exp(-sqr_mahal_dist/2));
    }

    //: The probability of this sample
    float probability(const vnl_vector_fixed<float,3>& pt) const
    {
      return dist_probability(sqr_mahalanobis_dist(pt));
    }

    //: The squared Mahalanobis distance to this point
    float sqr_mahalanobis_dist(const vnl_vector_fixed<float,3>& pt) const;

    //: Compute the determinant of the covariance matrix
    float det_covar() const { return det_covar_; }

    //: The inverse covariance matrix of the distribution
    const vnl_matrix_fixed<float,3,3> inv_covar() const;
float  compute_sqr_mahalanobis(vnl_vector_fixed<float,3> d,vnl_matrix_fixed<float,3,3> invcovar) const;
    float sum(float angle1, float angle2) const;
    float difference(float angle1, float angle2) const;

  protected:
    //: The covariance matrix
    vnl_matrix_fixed<float,3,3> covar_;

    //: The cached covariance determinant
    float det_covar_;

  private:
    //: compute the determinant of the covariance
    void compute_det();

    //: cache for inverse of the covariance matrix
    //vnl_matrix_fixed<float,3,3>* inv_covar_;
};


#endif // bsta_gaussian_x_y_theta_h_
