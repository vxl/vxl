// This is brcv/seg/bsta/bsta_gaussian_sphere.h
#ifndef bsta_gaussian_sphere_h_
#define bsta_gaussian_sphere_h_

//:
// \file
// \brief A (hyper-)spherical Gaussian distribution (i.e. single variance)
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 1/25/06
//
// \verbatim
//  Modifications
// \endverbatim

#include "bsta_gaussian.h"


//: A (hyper-)spherical Gaussian distribution
// Thus, the covariance matrix is the identity times a scalar variance
template <class T, unsigned n>
class bsta_gaussian_sphere : public bsta_gaussian<T,n>
{
  public:
    typedef typename bsta_gaussian<T,n>::vector_type _vector;
    //: Constructor
    bsta_gaussian_sphere<T,n>():
      bsta_gaussian<T,n>(), var_(T(0)), det_covar_(T(0)) {}

    //: Constructor
    bsta_gaussian_sphere<T,n>(const _vector& mean,
                                const T& var):
      bsta_gaussian<T,n>(mean), var_(var), det_covar_(T(-1))
    {compute_det();}

    //: The variance of the distribution
    const T& var() const { return var_; }

    //: Set the variance of the distribution
    void set_var(const T& var) { var_ = var; compute_det(); }

    //: The probability of this sample given square mahalanobis distance
    T dist_probability(const T& sqr_mahal_dist) const
    {
      if(det_covar_ <= 0)
        return T(0);
      return static_cast<float>(vcl_sqrt(1/(det_covar_*two_pi_power<n>::value()))
           * vcl_exp(-sqr_mahal_dist/2));
    }

    //: The probability of this sample
    T probability(const _vector& pt) const
    {
      return dist_probability(sqr_mahalanobis_dist(pt));
    }

    //: The squared Mahalanobis distance to this point
    T sqr_mahalanobis_dist(const _vector& pt) const;

    //: Compute the determinant of the covariance matrix
    T det_covar() const { return det_covar_; }

  protected:
    //: The variance
    T var_;

    //: The cached covariance determinant
    T det_covar_;

  private:
    //: compute the determinant of the covariance
    void compute_det();
};
//template <class T>
//class bsta_gaussian_sphere<T,1> : public bsta_gaussian<T,1>
//{
//  public:
//    typedef typename bsta_gaussian<T,1>::vector_type _vector;
//    //: Constructor
//    bsta_gaussian_sphere<T,1>():
//      bsta_gaussian<T,1>(), var_(T(0)), det_covar_(T(0)) {}
//
//    //: Constructor
//    bsta_gaussian_sphere<T,1>(const _vector& mean,
//                                const T& var):
//      bsta_gaussian<T,1>(mean), var_(var), det_covar_(T(-1))
//    {compute_det();}
//
//    //: The variance of the distribution
//    const T& var() const { return var_; }
//
//    //: Set the variance of the distribution
//    void set_var(const T& var) { var_ = var; compute_det(); }
//
//    //: The probability of this sample given square mahalanobis distance
//    T dist_probability(const T& sqr_mahal_dist) const
//    {
//      if(det_covar_ <= 0)
//        return T(0);
//      return vcl_sqrt(1/(det_covar_*two_pi_power<1>::value()))
//           * vcl_exp(-sqr_mahal_dist/2);
//    }
//
//    //: The probability of this sample
//    T probability(const _vector& pt) const
//    {
//      return dist_probability(sqr_mahalanobis_dist(pt));
//    }
//
//    //: The squared Mahalanobis distance to this point
//    T sqr_mahalanobis_dist(const _vector& pt) const;
//
//    //: Compute the determinant of the covariance matrix
//    T det_covar() const { return det_covar_; }
//
//  protected:
//    //: The variance
//    T var_;
//
//    //: The cached covariance determinant
//    T det_covar_;
//
//  private:
//    //: compute the determinant of the covariance
//    void compute_det();
//};
//
//
//
#endif // bsta_gaussian_sphere_h_
