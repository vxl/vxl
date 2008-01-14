// This is brcv/seg/bsta/bsta_gaussian_x_y_theta.cxx

//:
// \file

#include "bsta_gaussian_x_y_theta.h"
#include <vcl_cassert.h>
#include <vcl_algorithm.h>
#include <vcl_limits.h>

#include <vnl/algo/vnl_svd.h>
#include <vnl/algo/vnl_determinant.h>
#include <vnl/vnl_inverse.h>



float 
bsta_gaussian_x_y_theta::compute_sqr_mahalanobis(vnl_vector_fixed<float,3> d,vnl_matrix_fixed<float,3,3> invcovar) const
{
  return dot_product<float,3>(d,invcovar*d);
};







//: The squared Mahalanobis distance to this point
float 
bsta_gaussian_x_y_theta::sqr_mahalanobis_dist(const vnl_vector_fixed<float,3>& pt) const
{
  if(det_covar_<=0.0)
    return vcl_numeric_limits<float>::infinity();
  vnl_vector_fixed<float,3> d;
  d[0]= -bsta_gaussian<float,3>::mean_[0]+pt[0];
  d[1]= -bsta_gaussian<float,3>::mean_[1]+pt[1];
  d[2]=difference(pt[2],bsta_gaussian<float,3>::mean_[2]);
  return compute_sqr_mahalanobis(d,inv_covar());
}


//: The squared Mahalanobis distance to this point
void
bsta_gaussian_x_y_theta::compute_det()
{
  det_covar_ = vnl_determinant(covar_);
}


//: Update the covariance (and clear cached values)
void 
bsta_gaussian_x_y_theta::set_covar(const vnl_matrix_fixed<float,3,3>& covar) 
{ 
  covar_ = covar;
  compute_det();
}


//: Return the inverse of the covariance matrix
// \note this matrix is cached and updated only when needed

const vnl_matrix_fixed<float,3,3>
bsta_gaussian_x_y_theta::inv_covar() const
{
    vnl_matrix_fixed<float,3,3> C = covar_;
    if(det_covar_ == 0.0){
      // if the matrix is singular we can add a small lambda*I
      // before inverting to avoid divide by zero
      // Is this the best way to handle this?
      float lambda = 0.0;
      for(unsigned i=0; i<3; ++i)
        lambda = vcl_max(lambda,C(i,i));
      lambda *= 1e-4f;  
      for(unsigned i=0; i<3; ++i)
        C(i,i) += lambda;
    }
    vnl_matrix_fixed<float,3,3> inv_covar_ = vnl_inverse<float>(C);
    return inv_covar_;
}




float 
bsta_gaussian_x_y_theta::sum(float angle1, float angle2) const
{
    if(angle1>=0  && angle1<=2*vnl_math::pi && angle2>=0  && angle2<=2*vnl_math::pi)
    {
        if(angle1+angle2>2*vnl_math::pi)
        {
            return static_cast<float>(angle1+angle2-2*vnl_math::pi);
        }
        else
            return angle1+angle2;
    }
    else 
        return angle1+angle2;
}
float 
bsta_gaussian_x_y_theta::difference(float angle1, float angle2) const
{
    if(angle1>=0  && angle1<=2*vnl_math::pi && angle2>=0  && angle2<=2*vnl_math::pi)
    {

        if(vcl_abs(angle1-angle2)>vnl_math::pi)
        {
            if(      angle1-angle2<0) 
                return static_cast<float>(angle1-angle2+2*vnl_math::pi);
            else
                return static_cast<float>(angle1-angle2-2*vnl_math::pi);
        }
        else
            return angle1-angle2;
    }
    else
        return angle1-angle2;
    
}
