/* arpack/second.f -- translated by f2c (version 20090411).
   You must link the resulting object file with libf2c:
        on Microsoft Windows system, link with libf2c.lib;
        on Linux or Unix systems, link with .../path/to/libf2c.a -lm
        or, if you install libf2c.a in a standard place, with -lf2c -lm
        -- in that order, at the end of the command line, as in
                cc *.o -lf2c -lm
        Source for libf2c is in /netlib/f2c/libf2c.zip, e.g.,

                http://www.netlib.org/f2c/libf2c.zip
*/

#ifdef __cplusplus
extern "C" {
#endif
#include "v3p_netlib.h"

/*<       SUBROUTINE SECOND( T ) >*/
/* Subroutine */ int second_(real *t)
{
    real t1;


/*<       REAL       T >*/

/*  -- LAPACK auxiliary routine (preliminary version) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd., */
/*     Courant Institute, Argonne National Lab, and Rice University */
/*     July 26, 1991 */

/*  Purpose */
/*  ======= */

/*  SECOND returns the user time for a process in seconds. */
/*  This version gets the time from the system function ETIME. */

/*     .. Local Scalars .. */
/*<       REAL               T1 >*/
/*     .. */
/*     .. Local Arrays .. */
/*<       REAL               TARRAY( 2 ) >*/
/*     .. */
/*     .. Intrinsic Functions .. */
/*      REAL               ETIME */
/*      EXTERNAL           ETIME */
/*      INTRINSIC          CPU_TIME */
/*     .. */
/*     .. Executable Statements .. */

/*      T1 = ETIME( TARRAY ) */
/*      T  = TARRAY( 1 ) */
/*      T1 = CPU_TIME( T ) */
/*<       T1 = 0.0 >*/
    t1 = (float)0.;
/*<       RETURN >*/
    return 0;

/*     End of SECOND */

/*<       END >*/
} /* second_ */

#ifdef __cplusplus
        }
#endif
