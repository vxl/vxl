/* slapy2.f -- translated by f2c (version of 4 June 1993  1:43:59).
   You must link the resulting object file with the libraries:
        -lf2c -lm   (in that order)
*/

#include "f2c.h"

real slapy2_(real *x, real *y)
{
    /* Builtin functions */
    double sqrt(doublereal);
#define sqrtf(f) ((float)sqrt((double)(f)))

    /* Local variables */
    static real xabs, yabs, w, z;

/*  -- LAPACK auxiliary routine (version 2.0) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd., */
/*     Courant Institute, Argonne National Lab, and Rice University */
/*     October 31, 1992 */

/*  Purpose                                                               */
/*  =======                                                               */
/*                                                                        */
/*  SLAPY2 returns sqrt(x**2+y**2), taking care not to cause unnecessary  */
/*  overflow.                                                             */
/*                                                                        */
/*  Arguments                                                             */
/*  =========                                                             */
/*                                                                        */
/*  X       (input) REAL                                                  */
/*  Y       (input) REAL                                                  */
/*          X and Y specify the values x and y.                           */
/*                                                                        */
/*  ===================================================================== */

    xabs = abs(*x);
    yabs = abs(*y);
    w = max(xabs,yabs);
    z = min(xabs,yabs);
    if (z == 0.f) {
        return w;
    } else {
        z /= w;
        return w * sqrtf(z * z + 1.f);
    }
} /* slapy2_ */
