// This is bbas/bnl/bnl_fresnel.cxx
// :
// \file

#include "bnl_fresnel.h"
#include <vcl_complex.h>
#include <vnl/vnl_math.h>
#include <vcl_iostream.h>

// Fresnel Integral computation adapted from Numerical recipes in C
// eps   is the relative error;
// maxit is the maximum number of iterations allowed;
// fpmin is a number near the smallest representable floating-point number;
// xmin  is the dividing line between using the series and continued fraction.

#define eps    6.0e-8
#define maxit  100
#define fpmin  1.0e-30
#define xmin   2.0 // = 1.5 in euler.cpp version, changed to match with NR book's code

// : Fresnel integral
// FresnelC = int(cos(pi/2 * t^2), t = 0..x);
// FresnelS = int(sin(pi/2 * t^2), t = 0..x);
// Return: fres_cos is Fresnel cosin integral, fres_sin is Frenel sine integral
// accuracy: 1e-8
void bnl_fresnel_integral(double x, double* fresnel_cos, double* fresnel_sin)
{
  // the computation fails when x is outside [-1e8, 1e8]
  // Fortunately, fresnel integrals converge when
  // x goes to +-infinity
  if( x > 1e8 || x < -1e8 )
    {
    *fresnel_cos = (x > 1e8) ? 0.5 : -0.5;
    *fresnel_sin = (x > 1e8) ? 0.5 : -0.5;
    return;
    }

  bool                odd;
  int                 k, n;
  double              a, ax, fact, pix2, sign, sum, sumc, sums, term, test;
  vcl_complex<double> b, cc, d, h, del, cs;
  double              fcos, fsin;

  ax = vcl_fabs(x);
  if( ax < vcl_sqrt(fpmin) )
    {
    // Special case: avoid failure of convergence test because of undeflow.
    *fresnel_sin = 0.0;
    *fresnel_cos = ax;
    return;
    }

  if( ax <= xmin )
    {
    // Evaluate both series simultaneously.
    sum = sums = 0.0;
    sumc = ax;
    sign = 1.0;
    fact = (vnl_math::pi_over_2)*ax * ax;
    odd = true;
    term = ax;
    n = 3;
    for( k = 1; k <= maxit; k++ )
      {
      term *= fact / k;
      sum  += sign * term / n;
      test = vcl_fabs(sum) * eps;
      if( odd )
        {
        sign = -sign;
        sums = sum;
        sum = sumc;
        }
      else
        {
        sumc = sum;
        sum = sums;
        }

      if( term < test )
        {
        break;
        }
      odd = !odd;
      n += 2;
      }

    if( k > maxit )
      {
      vcl_cerr << "series failed in fresnel" << vcl_endl;
      }
    fsin = sums;
    fcos = sumc;
    }
  else
    {
    // Evaluate continued fraction by modified Lentz's method
    pix2 = vnl_math::pi * ax * ax;
    b = vcl_complex<double>(1.0, -pix2);
    cc = vcl_complex<double>(1.0 / fpmin, 0.0);
    d = h = vcl_complex<double>(1.0, 0.0) / b;
    n = -1;
    for( k = 2; k <= maxit; k++ )
      {
      n += 2;
      a = -n * (n + 1);
      b = b + vcl_complex<double>(4.0, 0.0);
      d = (vcl_complex<double>(1.0, 0.0) / ( (a * d) + b) );

      // Denominators cannot be zero
      cc = (b + (vcl_complex<double>(a, 0.0) / cc) );
      del = (cc * d);
      h = h * del;
      if( (vcl_fabs(del.real() - 1.0) + vcl_fabs(del.imag() ) ) < eps )
        {
        break;
        }
      }
    if( k > maxit )
      {
      vcl_cerr << "cf failed in frenel" << vcl_endl;
      }

    h = vcl_complex<double>(ax, -ax) * h;
    cs = vcl_complex<double>(0.5, 0.5) * (vcl_complex<double>(1.0, 0.0)
                                          - vcl_complex<double>(vcl_cos(0.5 * pix2), vcl_sin(0.5 * pix2) ) * h );
    fcos = cs.real();
    fsin = cs.imag();
    }

  if( x < 0 )   // use antisymmetry
    {
    fcos = -fcos;
    fsin = -fsin;
    }
  *fresnel_cos = fcos;
  *fresnel_sin = fsin;
  return;
}
