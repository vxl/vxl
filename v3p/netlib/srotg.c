/* srotg.f -- translated by f2c (version of 23 April 1993  18:34:30).
   You must link the resulting object file with the libraries:
        -lf2c -lm   (in that order)
*/

#include "f2c.h"

/* Table of constant values */

static real c_b4 = 1.f;

/* Subroutine */ void srotg_(sa, sb, c, s)
real *sa, *sb, *c, *s;
{
    /* Builtin functions */
    double sqrt(), r_sign();
#define sqrtf(f) ((float)sqrt((double)(f)))

    /* Local variables */
    static real r, scale, z, roe;

/*     construct givens plane rotation.        */
/*     jack dongarra, linpack, 3/11/78.        */

    scale = (float)dabs(*sa) + (float)dabs(*sb);
    if (scale == 0.f) {
        *c = 1.f; *s = 0.f;
        *sa = *sb = 0.f;
    }
    else {
        roe = *sb;
        if (dabs(*sa) > dabs(*sb)) {
            roe = *sa;
        }
        r = *sa / scale;
        z = *sb / scale;
        r = scale * sqrtf(r * r + z * z);
        r *= (float)r_sign(&c_b4, &roe);
        *c = *sa / r;
        *s = *sb / r;
        z = 1.f;
        if (dabs(*sa) > dabs(*sb)) {
            z = *s;
        }
        if (dabs(*sb) >= dabs(*sa) && *c != 0.f) {
            z = 1.f / *c;
        }
        *sa = r;
        *sb = z;
    }
} /* srotg_ */
