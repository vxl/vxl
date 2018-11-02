#include "rgrl_rotation.h"
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_math.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

void rgrl_rotation_3d_to_matrix(double const theta, double const alpha, double const phi,
                                vnl_matrix<double> & R)
{
  assert( R.rows()==3 && R.cols()==3 );
  double cos_a = std::cos(alpha), sin_a = std::sin(alpha),
         cos_t = std::cos(theta), sin_t = std::sin(theta),
         cos_p = std::cos(phi),   sin_p = std::sin(phi);
  R(0,0) =cos_a*cos_t ;                   R(0,1) = -cos_a*sin_t;                   R(0,2)=sin_a;
  R(1,0) =cos_t*sin_a*sin_p+cos_p*sin_t;  R(1,1) = -sin_a*sin_p*sin_t+cos_p*cos_t; R(1,2)=-cos_a*sin_p;
  R(2,0)=-cos_p*cos_t*sin_a+sin_p*sin_t;  R(2,1) = cos_p*sin_a*sin_t+cos_t*sin_p;  R(2,2)=cos_a*cos_p;
}

void rgrl_rotation_3d_from_matrix(vnl_matrix<double> const& R,
                                  double& phi, double& alpha, double& theta)
{
  assert( R.rows()==3 && R.cols()==3 );
  alpha = std::asin( R(0,2) );

  if (R(0,0) * std::cos(alpha) > 0)
  {
    theta = std::atan( -1 * R(0,1)/R(0,0) );
  }
  else
  {
    theta = std::atan( -1 * R(0,1)/R(0,0) ) + vnl_math::pi;
  }

  if (R(2,2) * std::cos(alpha) > 0 )
  {
    phi = std::atan( -1 * R(1,2) / R(2,2) );
  }
  else
  {
    phi = std::atan( -1 * R(1,2) / R(2,2) ) + vnl_math::pi;
  }

}

void rgrl_rotation_2d_to_matrix(double const theta, vnl_matrix<double> & R)
{
  assert( R.rows()==2 && R.cols()==2 );
  R(0,0) = std::cos(theta);    R(0,1) = std::sin(theta);
  R(1,0) = -std::sin(theta);   R(1,1) = std::cos(theta);
}

void rgrl_rotation_2d_from_matrix(vnl_matrix<double> const& R, double &theta)
{
  assert( R.rows()==2 && R.cols()==2 );
  theta = std::asin( R(0,1) );
}
