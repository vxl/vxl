#include "rrel_misc.h"
#include <vcl_cmath.h>
#include <vnl/vnl_math.h>

// Chebychev approximation to erfc --- the complement of the error
// function.  Taken from Numerical Recipes in C.

double
rrel_misc_erfcc( double x )
{
    double t,z,ans;

    z=vcl_fabs(x);
    t=1.0/(1.0+0.5*z);
    ans=t*vcl_exp(-z*z-1.26551223+t*(1.00002368+t*(0.37409196+t*(0.09678418+
           t*(-0.18628806+t*(0.27886807+t*(-1.13520398+t*(1.48851587+
                 t*(-0.82215223+t*0.17087277)))))))));
    return  x >= 0.0 ? ans : 2.0-ans;
}

//
// Robert W. Cox from the Biophysics Research Institute at the Medical
// College of Wisconsin provided the following routine for computing
// the inverse CDF for a standardized Gaussian.  This function is
// based off of a rational polynomial approximation to the inverse
// Gaussian CDF which can be found in
//
// M. Abramowitz and I.A. Stegun. Handbook of Mathematical Functions
// with Formulas, Graphs, and Mathematical Tables.  John Wiley & Sons.
// New York. Equation 26.2.23. pg. 933. 1972.
//
// Since the initial approximation only provides an estimate within
// 4.5 E-4 of the true value, 3 Newton-Raphson iterations are used
// to refine the approximation.
//
// Let
// $Q(x) = (1/\sqrt{2 pi}) \Int_{x}^{\infty} e^{-t^2/2} dt
//       = 0.5 * erfc(x/\sqrt{2})$
//
// Given p, this function computes x such that $Q(x) = p$, for $0 < p < 1$
//
// Note that the Gaussian CDF is defined as
// $P(x) = (1/\sqrt{2 pi}) \Int_{-\infty}^{x} e^{-t^2/2} dt
//       = 1 - Q(x)$
//
// This function has been modified to compute the inverse of $P(x)$ instead
// of $Q(x)$.
//
double
rrel_misc_gaussian_cdf_inv( double p )
{
   double dp , dx , dt , ddq , dq ;
   int    newt ;

   dp = (p <= 0.5) ? p : 1.0-p;   // make between 0 and 0.5

   // if original value is invalid, return +infinity or -infinity
   // changed from original code to reflect the fact that the
   // the inverse of P(x) not Q(x) is desired.
   //
   // Original line: used for inverse of Q(x)
   // if ( dp <= 0.0 ){ dx = 13.0 ;  return (p <= 0.5) ? (dx) : (-dx); }

   // replaced with this if construct for the inverse of P(x)
   if (p <= 0.0)
     return -vnl_huge_val(double(0));
   else if (p >= 1.0)
     return vnl_huge_val(double(0));


   //  Step 1:  use 26.2.23 from Abramowitz and Stegun

   dt = vcl_sqrt( -2.0 * vcl_log(dp) ) ;
   dx = dt
     - ((.010328e+0*dt + .802853e+0)*dt + 2.515517e+0)
     /(((.001308e+0*dt + .189269e+0)*dt + 1.432788e+0)*dt + 1.e+0) ;

   //  Step 2:  do 3 Newton steps to improve this

   for ( newt=0 ; newt < 3 ; newt++ ){
     dq  = 0.5e+0 * rrel_misc_erfcc( dx / 1.414213562373095e+0 ) - dp ;
     ddq = vcl_exp( -0.5e+0 * dx * dx ) / 2.506628274631000e+0 ;
     dx  = dx + dq / ddq ;
   }

   // original line when computing the inverse of Q(x)
   // return (p <= 0.5) ? dx : (-dx)   // return with correct sign
   //
   // Note that P(-x) = Q(x), so whatever x was calculated for Q(x) = p,
   // we simply need to return the negative of x to get P(xp) = p.
   //
   return (p <= 0.5) ? (-dx) : dx; // return with correct sign
}
