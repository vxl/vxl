/* dsetgpfa.f -- translated by f2c (version 19951025).
   You must link the resulting object file with the libraries:
        -lf2c -lm   (in that order)
*/

#include "f2c.h"

/* Table of constant values */

static integer c__2 = 2;
static integer c__3 = 3;
static integer c__5 = 5;

/*        SUBROUTINE 'SETGPFA' */
/*        SETUP ROUTINE FOR SELF-SORTING IN-PLACE */
/*            GENERALIZED PRIME FACTOR (COMPLEX) FFT [GPFA] */

/*        CALL SETGPFA(TRIGS,N) */

/*        INPUT : */
/*        ----- */
/*        N IS THE LENGTH OF THE TRANSFORMS. N MUST BE OF THE FORM: */
/*          ----------------------------------- */
/*            N = (2**IP) * (3**IQ) * (5**IR) */
/*          ----------------------------------- */

/*        OUTPUT: */
/*        ------ */
/*        TRIGS IS A TABLE OF TWIDDLE FACTORS, */
/*          OF LENGTH 2*IPQR (REAL) WORDS, WHERE: */
/*          -------------------------------------- */
/*            IPQR = (2**IP) + (3**IQ) + (5**IR) */
/*          -------------------------------------- */

/*        WRITTEN BY CLIVE TEMPERTON 1990 */

/* ---------------------------------------------------------------------- */

/*<       SUBROUTINE SETGPFA(TRIGS,N,IRES,INFO) >*/
/*<       DIMENSION TRIGS(*) >*/
/*<       DIMENSION NJ(3) >*/
/*<       DIMENSION IRES(3) >*/
/*     DECOMPOSE N INTO FACTORS 2,3,5 */
/*     ------------------------------ */
/*<       NN = N >*/
/*<       IFAC = 2 >*/
/*<       DO 30 LL = 1 , 3 >*/
/*<       KK = 0 >*/
/*<    10 CONTINUE >*/
/*<       IF (MOD(NN,IFAC).NE.0) GO TO 20 >*/
/*<       KK = KK + 1 >*/
/*<       NN = NN / IFAC >*/
/*<       GO TO 10 >*/
/*<    20 CONTINUE >*/
/*<       NJ(LL) = KK >*/
/*<       IFAC = IFAC + LL >*/
/*<    30 CONTINUE >*/
/*<       IF (NN.NE.1) THEN >*/
/*<          INFO = -1 >*/
/*<       ENDIF >*/
/*<       IP = NJ(1) >*/
/*<       IQ = NJ(2) >*/
/*<       IR = NJ(3) >*/
/*<       IRES(1) = IP >*/
/*<       IRES(2) = IQ >*/
/*<       IRES (3) = IR >*/
/*     COMPUTE LIST OF ROTATED TWIDDLE FACTORS */
/*     --------------------------------------- */
/*<       NJ(1) = 2**IP >*/
/*<       NJ(2) = 3**IQ >*/
/*<       NJ(3) = 5**IR >*/
/*<       TWOPI = 4.0 * ASIN(1.0) >*/
/*<       I = 1 >*/
/*<       DO 60 LL = 1 , 3 >*/
/*<       NI = NJ(LL) >*/
/*<       IF (NI.EQ.1) GO TO 60 >*/
/*<       DEL = TWOPI / FLOAT(NI) >*/
/*<       IROT = N / NI >*/
/*<       KINK = MOD(IROT,NI) >*/
/*<       KK = 0 >*/
/*<       DO 50 K = 1 , NI >*/
/*<       ANGLE = FLOAT(KK) * DEL >*/
/*<       TRIGS(I) = COS(ANGLE) >*/
/*<       TRIGS(I+1) = SIN(ANGLE) >*/
/*<       I = I + 2 >*/
/*<       KK = KK + KINK >*/
/*<       IF (KK.GT.NI) KK = KK - NI >*/
/*<    60 CONTINUE >*/
/*<       INFO = 0 >*/

/* Subroutine */
void dsetgpfa_(doublereal *trigs, integer *n, integer *ires, integer *info)
{
    /* Builtin functions */
    integer pow_ii(integer *, integer *);
    double asin(doublereal), cos(doublereal), sin(doublereal);

    /* Local variables */
    static integer ifac, kink, irot, i__, k;
    static doublereal angle, twopi;
    static integer kk, ni, nj[3], ll, nn;
    static doublereal del;

    /* Function Body */

/*     DECOMPOSE N INTO FACTORS 2,3,5 */
/*     ------------------------------ */
    nn = *n;
    ifac = 2;

    for (ll = 0; ll < 3; ++ll) {
        kk = 0;
        while (nn % ifac == 0) {
            ++kk;
            nn /= ifac;
        }
        ires[ll] = kk;
        ifac += ll+1; /* which makes ifac 3 and 5 in the next 2 runs */
    }

    if (nn != 1) {
        *info = -1;
        return;
    }

/*     COMPUTE LIST OF ROTATED TWIDDLE FACTORS */
/*     --------------------------------------- */
    nj[0] = pow_ii(&c__2, ires);
    nj[1] = pow_ii(&c__3, ires+1);
    nj[2] = pow_ii(&c__5, ires+2);

    twopi = asin(1.) * 4.;
    i__ = 0;

    for (ll = 0; ll < 3; ++ll) {
        ni = nj[ll];
        if (ni == 1) {
            continue; /* next ll */
        }
        del = twopi / (doublereal) ni;
        irot = *n / ni;
        kink = irot % ni;
        kk = 0;

        for (k = 1; k <= ni; ++k) {
            angle = (doublereal) kk * del;
            trigs[i__] = cos(angle);
            trigs[i__+1] = sin(angle);

            i__ += 2;
            kk += kink;
            if (kk > ni) {
                kk -= ni;
            }
        }
    }

    *info = 0;
} /* setgpfa_ */

