#ifdef __GNUC__
#pragma implementation
#endif

#include "vsl_conic_as_params.h"

#include <vnl/vnl_math.h>

//: Construct from geometric parameters center, radii (may be negative or zero
// to signify hyperbola or parabola), and counterclockwise angle between Rx axis
// and X axis.
vsl_conic_as_params::vsl_conic_as_params(double cx, double cy, double rx, double ry, double theta)
{
  double Rx=(rx != 0) ? (vnl_math_sgn(rx)/(rx*rx)) : 0;
  double Ry=(ry != 0) ? (vnl_math_sgn(ry)/(ry*ry)) : 0;

  theta = -theta;
  double cost = vcl_cos(theta);
  double sint = vcl_sin(theta);
  double cost2 = cost*cost;
  double sint2 = sint*sint;
  double sin2t = 2*sint*cost;
  /* Axx */(*this)[0] = cost2*Rx + sint2*Ry;
  /* Axy */(*this)[1] = sin2t*(Ry-Rx);
  /* Ayy */(*this)[2] = Rx*sint2 + Ry*cost2;
  /* Ax  */(*this)[3] = sin2t*(Rx - Ry)*cy - 2*cx*(Ry*sint*sint + Rx*cost*cost);
  /* Ay  */(*this)[4] = sin2t*(Rx - Ry)*cx - 2*cy*(Ry*cost*cost + Rx*sint*sint);
  /* Ao  */(*this)[5] = Ry*vnl_math_sqr(cx*sint + cy*cost) + Rx*vnl_math_sqr(cx*cost - cy*sint) - 1;
}
