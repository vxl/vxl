/* temperton/gpfa.f -- translated by f2c (version 20050501).
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

/* Table of constant values */

static integer c__2 = 2;
static integer c__3 = 3;

/*        SUBROUTINE 'GPFA' */
/*        SELF-SORTING IN-PLACE GENERALIZED PRIME FACTOR (COMPLEX) FFT */

/*        *** THIS IS THE ALL-FORTRAN VERSION *** */
/*            ------------------------------- */

/*        CALL GPFA(A,B,TRIGS,INC,JUMP,N,LOT,ISIGN,INFO) */

/*        A IS FIRST REAL INPUT/OUTPUT VECTOR */
/*        B IS FIRST IMAGINARY INPUT/OUTPUT VECTOR */
/*        TRIGS IS A TABLE OF TWIDDLE FACTORS, PRECALCULATED */
/*              BY CALLING SUBROUTINE 'SETGPFA' */
/*        INC IS THE INCREMENT WITHIN EACH DATA VECTOR */
/*        JUMP IS THE INCREMENT BETWEEN DATA VECTORS */
/*        N IS THE LENGTH OF THE TRANSFORMS: */
/*          ----------------------------------- */
/*            N = (2**IP) * (3**IQ) * (5**IR) */
/*          ----------------------------------- */
/*        LOT IS THE NUMBER OF TRANSFORMS */
/*        ISIGN = +1 FOR FORWARD TRANSFORM */
/*              = -1 FOR INVERSE TRANSFORM */
/*        INFO = SET TO 0 ON SUCCESS AND 1 ON FAILURE */

/*        WRITTEN BY CLIVE TEMPERTON */
/*        RECHERCHE EN PREVISION NUMERIQUE */
/*        ATMOSPHERIC ENVIRONMENT SERVICE, CANADA */

/* ---------------------------------------------------------------------- */

/*        DEFINITION OF TRANSFORM */
/*        ----------------------- */

/*        X(J) = SUM(K=0,...,N-1)(C(K)*EXP(ISIGN*2*I*J*K*PI/N)) */

/* --------------------------------------------------------------------- */

/*        FOR A MATHEMATICAL DEVELOPMENT OF THE ALGORITHM USED, */
/*        SEE: */

/*        C TEMPERTON : "A GENERALIZED PRIME FACTOR FFT ALGORITHM */
/*          FOR ANY N = (2**P)(3**Q)(5**R)", */
/*          SIAM J. SCI. STAT. COMP., MAY 1992. */

/* ---------------------------------------------------------------------- */

/*<       SUBROUTINE GPFA(A,B,TRIGS,INC,JUMP,N,LOT,ISIGN,INFO) >*/
/* Subroutine */ int gpfa_(real *a, real *b, real *trigs, integer *inc, 
	integer *jump, integer *n, integer *lot, integer *isign, integer *
	info)
{
    /* Builtin functions */
    integer pow_ii(integer *, integer *);

    /* Local variables */
    integer i__, kk, nj[3], ll, ip, iq, nn, ir, ifac;
    extern /* Subroutine */ int gpfa2f_(real *, real *, real *, integer *, 
	    integer *, integer *, integer *, integer *, integer *), gpfa3f_(
	    real *, real *, real *, integer *, integer *, integer *, integer *
	    , integer *, integer *), gpfa5f_(real *, real *, real *, integer *
	    , integer *, integer *, integer *, integer *, integer *);


/*<       REAL A(*), B(*), TRIGS(*) >*/
/*<       INTEGER INC, JUMP, N, LOT, ISIGN, INFO >*/
/*<       DIMENSION NJ(3) >*/
/*<       INFO = 0 >*/
    /* Parameter adjustments */
    --trigs;
    --b;
    --a;

    /* Function Body */
    *info = 0;

/*     DECOMPOSE N INTO FACTORS 2,3,5 */
/*     ------------------------------ */
/*<       NN = N >*/
    nn = *n;
/*<       IFAC = 2 >*/
    ifac = 2;

/*<       DO 30 LL = 1 , 3 >*/
    for (ll = 1; ll <= 3; ++ll) {
/*<       KK = 0 >*/
	kk = 0;
/*<    10 CONTINUE >*/
L10:
/*<       IF (MOD(NN,IFAC).NE.0) GO TO 20 >*/
	if (nn % ifac != 0) {
	    goto L20;
	}
/*<       KK = KK + 1 >*/
	++kk;
/*<       NN = NN / IFAC >*/
	nn /= ifac;
/*<       GO TO 10 >*/
	goto L10;
/*<    20 CONTINUE >*/
L20:
/*<       NJ(LL) = KK >*/
	nj[ll - 1] = kk;
/*<       IFAC = IFAC + LL >*/
	ifac += ll;
/*<    30 CONTINUE >*/
/* L30: */
    }

/*<       IF (NN.NE.1) THEN >*/
    if (nn != 1) {
/*        WRITE(6,40) N */
/*  40    FORMAT(' *** WARNING!!!',I10,' IS NOT A LEGAL VALUE OF N ***') */
/*<          INFO = 1 >*/
	*info = 1;
/*<          RETURN >*/
	return 0;
/*<       ENDIF >*/
    }

/*<       IP = NJ(1) >*/
    ip = nj[0];
/*<       IQ = NJ(2) >*/
    iq = nj[1];
/*<       IR = NJ(3) >*/
    ir = nj[2];

/*     COMPUTE THE TRANSFORM */
/*     --------------------- */
/*<       I = 1 >*/
    i__ = 1;
/*<       IF (IP.GT.0) THEN >*/
    if (ip > 0) {
/*<          CALL GPFA2F(A,B,TRIGS,INC,JUMP,N,IP,LOT,ISIGN) >*/
	gpfa2f_(&a[1], &b[1], &trigs[1], inc, jump, n, &ip, lot, isign);
/*<          I = I + 2 * ( 2**IP) >*/
	i__ += pow_ii(&c__2, &ip) << 1;
/*<       ENDIF >*/
    }
/*<       IF (IQ.GT.0) THEN >*/
    if (iq > 0) {
/*<          CALL GPFA3F(A,B,TRIGS(I),INC,JUMP,N,IQ,LOT,ISIGN) >*/
	gpfa3f_(&a[1], &b[1], &trigs[i__], inc, jump, n, &iq, lot, isign);
/*<          I = I + 2 * (3**IQ) >*/
	i__ += pow_ii(&c__3, &iq) << 1;
/*<       ENDIF >*/
    }
/*<       IF (IR.GT.0) THEN >*/
    if (ir > 0) {
/*<          CALL GPFA5F(A,B,TRIGS(I),INC,JUMP,N,IR,LOT,ISIGN) >*/
	gpfa5f_(&a[1], &b[1], &trigs[i__], inc, jump, n, &ir, lot, isign);
/*<       ENDIF >*/
    }

/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* gpfa_ */

#ifdef __cplusplus
	}
#endif
