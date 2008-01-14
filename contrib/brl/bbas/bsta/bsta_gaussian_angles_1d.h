// This is brcv/seg/bsta/bsta_gaussian_angles_1d.h
#ifndef bsta_gaussian_angles_1d_h_
#define bsta_gaussian_angles_1d_h_

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


//: assuming angle from 0 to 2*pi
//: A Gaussian distribution with a full covariance matrix
class bsta_gaussian_angles_1d : public bsta_gaussian<float,1>
{
  public:
    //: Constructor
      bsta_gaussian_angles_1d():bsta_gaussian<float,1>(), var_((0)),lo_(0),
		  high_(static_cast<float>(2* vnl_math::pi))
      {
      }
      bsta_gaussian_angles_1d(float mean,float var, float lo=0, 
		  float high=static_cast<float>(2*vnl_math::pi))
          :bsta_gaussian<float,1>(mean), var_(var),lo_(lo), high_(high)
      {
      }
      //: Destructor
      ~bsta_gaussian_angles_1d() { }

    //: The covariance matrix of the distribution
    const float& var() const { return var_; }

    //: Set the variance of the distribution
    void set_var(float  var){var_=var;}

    //: The probability of this sample given square mahalanobis distance
    float dist_probability(const float& sqr_mahal_dist) const
    {
      if(var_ <= 0)
        return float(0);
      return static_cast<float>(vcl_sqrt(1/(var_*two_pi_power<1>::value()))
           * vcl_exp(-sqr_mahal_dist/2));
    }

    //: The probability of this sample
    float probability(const float& pt) const
    {
      return dist_probability(sqr_mahalanobis_dist(pt));
    }

    //: The squared Mahalanobis distance to this point
    float sqr_mahalanobis_dist(const float& pt) const;

    float sum(float angle1, float angle2) const;
    float difference(float angle1, float angle2) const;

  protected:
    //: The covariance matrix
    float var_;
    float lo_;
    float high_;
};



#endif // bsta_gaussian_angles_1d_h_
