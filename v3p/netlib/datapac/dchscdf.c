/* datapac/dchscdf.f -- translated by f2c (version 20050501).
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

#include <stdio.h>

/*<       SUBROUTINE DCHSCDF(X,NU,CDF) >*/
/* Subroutine */ int dchscdf_(doublereal *x, integer *nu, doublereal *cdf)
{
    /* Initialized data */

    integer nucut = 1000;
    doublereal b43 = 17.;
    doublereal pi = 3.14159265358979;
    doublereal dpower = .33333333333333;
    doublereal b11 = .33333333333333;
    doublereal b21 = -.02777777777778;
    doublereal b31 = -6.1728395061e-4;
    doublereal b32 = -13.;
    doublereal b41 = 1.8004115226e-4;
    doublereal b42 = 6.;

    /* System generated locals */
    integer i__1;
    doublereal d__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), e_wsfe(), do_fio(integer *, char *, ftnlen);
    double sqrt(doublereal), exp(doublereal), pow_dd(doublereal *, doublereal 
	    *), log(doublereal);

    /* Local variables */
    integer i__;
    doublereal u, z__, d1, d2, d3, ai, sd, dw, dx, chi, anu, dnu;
    integer ipr;
    doublereal sum, cdfn, danu;
    integer imin, imax;
    doublereal term, term0, term1, term2, term3, term4, dcdfn, dfact, amean;
    integer ibran;
    doublereal spchi;
    integer ievodd;
    extern /* Subroutine */ int dnorcdf_(doublereal *, doublereal *);

/*     PURPOSE--THIS SUBROUTINE COMPUTES THE CUMULATIVE DISTRIBUTION */
/*              FUNCTION VALUE FOR THE CHI-SQUARED DISTRIBUTION */
/*              WITH INTEGER DEGREES OF FREEDOM PARAMETER = NU. */
/*              THIS DISTRIBUTION IS DEFINED FOR ALL NON-NEGATIVE X. */
/*              THE PROBABILITY DENSITY FUNCTION IS GIVEN */
/*              IN THE REFERENCES BELOW. */
/*     INPUT  ARGUMENTS--X      = THE SINGLE PRECISION VALUE AT */
/*                                WHICH THE CUMULATIVE DISTRIBUTION */
/*                                FUNCTION IS TO BE EVALUATED. */
/*                                X SHOULD BE NON-NEGATIVE. */
/*                     --NU     = THE INTEGER NUMBER OF DEGREES */
/*                                OF FREEDOM. */
/*                                NU SHOULD BE POSITIVE. */
/*     OUTPUT ARGUMENTS--CDF    = THE SINGLE PRECISION CUMULATIVE */
/*                                DISTRIBUTION FUNCTION VALUE. */
/*     OUTPUT--THE SINGLE PRECISION CUMULATIVE DISTRIBUTION */
/*             FUNCTION VALUE CDF FOR THE CHI-SQUARED DISTRIBUTION */
/*             WITH DEGREES OF FREEDOM PARAMETER = NU. */
/*     PRINTING--NONE UNLESS AN INPUT ARGUMENT ERROR CONDITION EXISTS. */
/*     RESTRICTIONS--X SHOULD BE NON-NEGATIVE. */
/*                 --NU SHOULD BE A POSITIVE INTEGER VARIABLE. */
/*     OTHER DATAPAC   SUBROUTINES NEEDED--DNORCDF. */
/*     FORTRAN LIBRARY SUBROUTINES NEEDED--DSQRT, DEXP. */
/*     MODE OF INTERNAL OPERATIONS--DOUBLE PRECISION. */
/*     LANGUAGE--ANSI FORTRAN. */
/*     REFERENCES--NATIONAL BUREAU OF STANDARDS APPLIED MATHEMATICS */
/*                 SERIES 55, 1964, PAGE 941, FORMULAE 26.4.4 AND 26.4.5. */
/*               --JOHNSON AND KOTZ, CONTINUOUS UNIVARIATE */
/*                 DISTRIBUTIONS--1, 1970, PAGE 176, */
/*                 FORMULA 28, AND PAGE 180, FORMULA 33.1. */
/*               --OWEN, HANDBOOK OF STATISTICAL TABLES, */
/*                 1962, PAGES 50-55. */
/*               --PEARSON AND HARTLEY, BIOMETRIKA TABLES */
/*                 FOR STATISTICIANS, VOLUME 1, 1954, */
/*                 PAGES 122-131. */
/*     WRITTEN BY--JAMES J. FILLIBEN */
/*                 STATISTICAL ENGINEERING LABORATORY (205.03) */
/*                 NATIONAL BUREAU OF STANDARDS */
/*                 WASHINGTON, D. C. 20234 */
/*                 PHONE:  301-921-2315 */
/*     ORIGINAL VERSION--JUNE      1972. */
/*     UPDATED         --MAY       1974. */
/*     UPDATED         --SEPTEMBER 1975. */
/*     UPDATED         --NOVEMBER  1975. */
/*     UPDATED         --OCTOBER   1976. */

/* --------------------------------------------------------------------- */

/*<       DOUBLE PRECISION X >*/
/*<       INTEGER NU >*/
/*<       DOUBLE PRECISION CDF >*/
/*<       DOUBLE PRECISION DX,PI,CHI,SUM,TERM,AI,DCDFN >*/
/*<       DOUBLE PRECISION DNU >*/
/*<       DOUBLE PRECISION DSQRT,DEXP >*/
/*<       DOUBLE PRECISION DLOG >*/
/*<       DOUBLE PRECISION DFACT,DPOWER >*/
/*<       DOUBLE PRECISION DW >*/
/*<       DOUBLE PRECISION D1,D2,D3 >*/
/*<       DOUBLE PRECISION TERM0,TERM1,TERM2,TERM3,TERM4 >*/
/*<       DOUBLE PRECISION B11 >*/
/*<       DOUBLE PRECISION B21 >*/
/*<       DOUBLE PRECISION B31,B32 >*/
/*<       DOUBLE PRECISION B41,B42,B43 >*/
/*<       DATA NUCUT/1000/ >*/
/*<       DATA PI/3.14159265358979D0/ >*/
/*<       DATA DPOWER/0.33333333333333D0/ >*/
/*<       DATA B11/0.33333333333333D0/ >*/
/*<       DATA B21/-0.02777777777778D0/ >*/
/*<       DATA B31/-0.00061728395061D0/ >*/
/*<       DATA B32/-13.0D0/ >*/
/*<       DATA B41/0.00018004115226D0/ >*/
/*<       DATA B42/6.0D0/ >*/
/*<       DATA B43/17.0D0/ >*/

/*<       IPR=6 >*/
    ipr = 6;

/*     CHECK THE INPUT ARGUMENTS FOR ERRORS */

/*<       IF(NU.LE.0)GOTO50 >*/
    if (*nu <= 0) {
	goto L50;
    }
/*<       IF(X.LT.0.0)GOTO55 >*/
    if (*x < (float)0.) {
	goto L55;
    }
/*<       GOTO90 >*/
    goto L90;
/*<    50 WRITE(IPR,15)  >*/
/*
   15 FORMAT(1H , 91H***** FATAL ERROR--THE SECOND INPUT ARGUMENT TO THE
     1 DCHSCDF SUBROUTINE IS NON-POSITIVE *****)
*/
L50:
    fprintf(stderr,
            "(***** FATAL ERROR--THE SECOND INPUT ARGUMENT TO THE "
            "DCHSCDF SUBROUTINE IS NON-POSITIVE *****)\n");
/*<       WRITE(IPR,47)NU >*/
/*
   47 FORMAT(1H , 35H***** THE VALUE OF THE ARGUMENT IS ,I8   ,6H *****)
*/
    fprintf(stderr,
            "(***** THE VALUE OF THE ARGUMENT IS %ld *****)\n", *nu);
/*<       CDF=0.0 >*/
    *cdf = (float)0.;
/*<       RETURN >*/
    return 0;
/*<    55 WRITE(IPR,4) >*/
/*
    4 FORMAT(1H , 96H***** NON-FATAL DIAGNOSTIC--THE FIRST  INPUT ARGUME
     1NT TO THE DCHSCDF SUBROUTINE IS NEGATIVE *****)
*/
L55:
    fprintf(stderr,
            "(***** NON-FATAL DIAGNOSTIC--THE FIRST INPUT ARGUMENT TO THE "
            "DCHSCDF SUBROUTINE IS NEGATIVE *****)\n");
/*<       WRITE(IPR,46)X >*/
/*
   46 FORMAT(1H , 35H***** THE VALUE OF THE ARGUMENT IS ,E15.8,6H *****)
*/
    fprintf(stderr,
            "(***** THE VALUE OF THE ARGUMENT IS %f *****)\n", *x);
/*<       CDF=0.0 >*/
    *cdf = (float)0.;
/*<       RETURN >*/
    return 0;
/*<    90 CONTINUE >*/
L90:
/*<    >*/
/*<    >*/
/*<    46 FORMAT(1H , 35H***** THE VALUE OF THE ARGUMENT IS ,E15.8,6H *****) >*/
/*<    47 FORMAT(1H , 35H***** THE VALUE OF THE ARGUMENT IS ,I8   ,6H *****) >*/

/* -----START POINT----------------------------------------------------- */

/*<       DX=X >*/
    dx = *x;
/*<       ANU=NU >*/
    anu = (doublereal) (*nu);
/*<       DNU=NU >*/
    dnu = (doublereal) (*nu);

/*     IF X IS NON-POSITIVE, SET CDF = 0.0 AND RETURN. */
/*     IF NU IS SMALLER THAN 10 AND X IS MORE THAN 200 */
/*     STANDARD DEVIATIONS BELOW THE MEAN, */
/*     SET CDF = 0.0 AND RETURN. */
/*     IF NU IS 10 OR LARGER AND X IS MORE THAN 100 */
/*     STANDARD DEVIATIONS BELOW THE MEAN, */
/*     SET CDF = 0.0 AND RETURN. */
/*     IF NU IS SMALLER THAN 10 AND X IS MORE THAN 200 */
/*     STANDARD DEVIATIONS ABOVE THE MEAN, */
/*     SET CDF = 1.0 AND RETURN. */
/*     IF NU IS 10 OR LARGER AND X IS MORE THAN 100 */
/*     STANDARD DEVIATIONS ABOVE THE MEAN, */
/*     SET CDF = 1.0 AND RETURN. */

/*<       IF(X.LE.0.0)GOTO105 >*/
    if (*x <= (float)0.) {
	goto L105;
    }
/*<       AMEAN=ANU >*/
    amean = anu;
/*<       SD=SQRT(2.0*ANU) >*/
    sd = sqrt(anu * (float)2.);
/*<       Z=(X-AMEAN)/SD >*/
    z__ = (*x - amean) / sd;
/*<       IF(NU.LT.10.AND.Z.LT.-200.0)GOTO105 >*/
    if (*nu < 10 && z__ < (float)-200.) {
	goto L105;
    }
/*<       IF(NU.GE.10.AND.Z.LT.-100.0)GOTO105 >*/
    if (*nu >= 10 && z__ < (float)-100.) {
	goto L105;
    }
/*<       IF(NU.LT.10.AND.Z.GT.200.0)GOTO107 >*/
    if (*nu < 10 && z__ > (float)200.) {
	goto L107;
    }
/*<       IF(NU.GE.10.AND.Z.GT.100.0)GOTO107 >*/
    if (*nu >= 10 && z__ > (float)100.) {
	goto L107;
    }
/*<       GOTO109 >*/
    goto L109;
/*<   105 CDF=0.0 >*/
L105:
    *cdf = (float)0.;
/*<       RETURN >*/
    return 0;
/*<   107 CDF=1.0 >*/
L107:
    *cdf = (float)1.;
/*<       RETURN >*/
    return 0;
/*<   109 CONTINUE >*/
L109:

/*     DISTINGUISH BETWEEN 3 SEPARATE REGIONS */
/*     OF THE (X,NU) SPACE. */
/*     BRANCH TO THE PROPER COMPUTATIONAL METHOD */
/*     DEPENDING ON THE REGION. */
/*     NUCUT HAS THE VALUE 1000. */

/*<       IF(NU.LT.NUCUT)GOTO1000  >*/
    if (*nu < nucut) {
	goto L1000;
    }
/*<       IF(NU.GE.NUCUT.AND.X.LE.ANU)GOTO2000 >*/
    if (*nu >= nucut && *x <= anu) {
	goto L2000;
    }
/*<       IF(NU.GE.NUCUT.AND.X.GT.ANU)GOTO3000 >*/
    if (*nu >= nucut && *x > anu) {
	goto L3000;
    }
/*<       IBRAN=1 >*/
    ibran = 1;
/*<       WRITE(IPR,99)IBRAN >*/
/*
   99 FORMAT(1H ,42H*****INTERNAL ERROR IN DCHSCDF SUBROUTINE--,
     146HIMPOSSIBLE BRANCH CONDITION AT BRANCH POINT = ,I8) 
*/
    fprintf(stderr,
            "(*****INTERNAL ERROR IN DCHSCDF SUBROUTINE "
            "-- IMPOSSIBLE BRANCH CONDITION AT BRANCH POINT %ld)\n",
            ibran);
/*<    >*/
/*<       RETURN >*/
    return 0;

/*     TREAT THE SMALL AND MODERATE DEGREES OF FREEDOM CASE */
/*     (THAT IS, WHEN NU IS SMALLER THAN 1000). */
/*     METHOD UTILIZED--EXACT FINITE SUM */
/*     (SEE AMS 55, PAGE 941, FORMULAE 26.4.4 AND 26.4.5). */

/*<  1000 CONTINUE >*/
L1000:
/*<       CHI=DSQRT(DX)  >*/
    chi = sqrt(dx);
/*<       IEVODD=NU-2*(NU/2) >*/
    ievodd = *nu - (*nu / 2 << 1);
/*<       IF(IEVODD.EQ.0)GOTO120 >*/
    if (ievodd == 0) {
	goto L120;
    }

/*<       SUM=0.0D0 >*/
    sum = 0.;
/*<       TERM=1.0/CHI >*/
    term = (float)1. / chi;
/*<       IMIN=1 >*/
    imin = 1;
/*<       IMAX=NU-1 >*/
    imax = *nu - 1;
/*<       GOTO130 >*/
    goto L130;

/*<   120 SUM=1.0D0 >*/
L120:
    sum = 1.;
/*<       TERM=1.0D0 >*/
    term = 1.;
/*<       IMIN=2 >*/
    imin = 2;
/*<       IMAX=NU-2 >*/
    imax = *nu - 2;

/*<   130 IF(IMIN.GT.IMAX)GOTO160  >*/
L130:
    if (imin > imax) {
	goto L160;
    }
/*<       DO100I=IMIN,IMAX,2 >*/
    i__1 = imax;
    for (i__ = imin; i__ <= i__1; i__ += 2) {
/*<       AI=I >*/
	ai = (doublereal) i__;
/*<       TERM=TERM*(DX/AI) >*/
	term *= dx / ai;
/*<       SUM=SUM+TERM >*/
	sum += term;
/*<   100 CONTINUE >*/
/* L100: */
    }
/*<   160 CONTINUE >*/
L160:

/*<       SUM=SUM*DEXP(-DX/2.0D0)  >*/
    sum *= exp(-dx / 2.);
/*<       IF(IEVODD.EQ.0)GOTO170 >*/
    if (ievodd == 0) {
	goto L170;
    }
/*<       SUM=(DSQRT(2.0D0/PI))*SUM >*/
    sum = sqrt(2. / pi) * sum;
/*<       SPCHI=CHI >*/
    spchi = chi;
/*<       CALL DNORCDF(SPCHI,CDFN)  >*/
    dnorcdf_(&spchi, &cdfn);
/*<       DCDFN=CDFN >*/
    dcdfn = cdfn;
/*<       SUM=SUM+2.0D0*(1.0D0-DCDFN) >*/
    sum += (1. - dcdfn) * 2.;
/*<   170 CDF=1.0D0-SUM  >*/
L170:
    *cdf = 1. - sum;
/*<       RETURN >*/
    return 0;

/*     TREAT THE CASE WHEN NU IS LARGE */
/*     (THAT IS, WHEN NU IS EQUAL TO OR GREATER THAN 1000) */
/*     AND X IS LESS THAN OR EQUAL TO NU. */
/*     METHOD UTILIZED--WILSON-HILFERTY APPROXIMATION */
/*     (SEE JOHNSON AND KOTZ, VOLUME 1, PAGE 176, FORMULA 28). */

/*<  2000 CONTINUE >*/
L2000:
/*<       DFACT=4.5D0*DNU >*/
    dfact = dnu * 4.5;
/*<       U=(((DX/DNU)**DPOWER)-1.0D0+(1.0D0/DFACT))*DSQRT(DFACT) >*/
    d__1 = dx / dnu;
    u = (pow_dd(&d__1, &dpower) - 1. + 1. / dfact) * sqrt(dfact);
/*<       CALL DNORCDF(U,CDFN) >*/
    dnorcdf_(&u, &cdfn);
/*<       CDF=CDFN >*/
    *cdf = cdfn;
/*<       RETURN >*/
    return 0;

/*     TREAT THE CASE WHEN NU IS LARGE */
/*     (THAT IS, WHEN NU IS EQUAL TO OR GREATER THAN 1000) */
/*     AND X IS LARGER THAN NU. */
/*     METHOD UTILIZED--HILL'S ASYMPTOTIC EXPANSION */
/*     (SEE JOHNSON AND KOTZ, VOLUME 1, PAGE 180, FORMULA 33.1). */

/*<  3000 CONTINUE >*/
L3000:
/*<       DW=DSQRT(DX-DNU-DNU*DLOG(DX/DNU))  >*/
    dw = sqrt(dx - dnu - dnu * log(dx / dnu));
/*<       DANU=DSQRT(2.0D0/DNU) >*/
    danu = sqrt(2. / dnu);
/*<       D1=DW >*/
    d1 = dw;
/*<       D2=DW**2 >*/
/* Computing 2nd power */
    d__1 = dw;
    d2 = d__1 * d__1;
/*<       D3=DW**3 >*/
/* Computing 3rd power */
    d__1 = dw;
    d3 = d__1 * (d__1 * d__1);
/*<       TERM0=DW >*/
    term0 = dw;
/*<       TERM1=B11*DANU >*/
    term1 = b11 * danu;
/*<       TERM2=B21*D1*(DANU**2) >*/
/* Computing 2nd power */
    d__1 = danu;
    term2 = b21 * d1 * (d__1 * d__1);
/*<       TERM3=B31*(D2+B32)*(DANU**3) >*/
/* Computing 3rd power */
    d__1 = danu;
    term3 = b31 * (d2 + b32) * (d__1 * (d__1 * d__1));
/*<       TERM4=B41*(B42*D3+B43*D1)*(DANU**4) >*/
/* Computing 4th power */
    d__1 = danu, d__1 *= d__1;
    term4 = b41 * (b42 * d3 + b43 * d1) * (d__1 * d__1);
/*<       U=TERM0+TERM1+TERM2+TERM3+TERM4 >*/
    u = term0 + term1 + term2 + term3 + term4;
/*<       CALL DNORCDF(U,CDFN) >*/
    dnorcdf_(&u, &cdfn);
/*<       CDF=CDFN >*/
    *cdf = cdfn;
/*<       RETURN >*/
    return 0;

/*<       END  >*/
} /* dchscdf_ */

#ifdef __cplusplus
	}
#endif
