// This is brcv/seg/bsta/bsta_gaussian_angles_1d.cxx

//:
// \file

#include "bsta_gaussian_angles_1d.h"
#include <vcl_cassert.h>
#include <vcl_algorithm.h>
#include <vcl_limits.h>





//: Unroll the mahalanobis distance off diagonal terms
//: The squared Mahalanobis distance to this point
float
bsta_gaussian_angles_1d::sqr_mahalanobis_dist(const float& angle) const
{
  if(var_<=0.0)
    return vcl_numeric_limits<float>::infinity();
  float d = difference(angle,mean_);
  //float d = mean_-angle;
  return d*d/var_;
}


float 
bsta_gaussian_angles_1d::sum(float angle1, float angle2) const
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
bsta_gaussian_angles_1d::difference(float angle1, float angle2) const
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
