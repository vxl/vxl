/* toms/rpoly.f -- translated by f2c (version 20050501).
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

/* Common Block Declarations */

Extern struct {
    doublereal p[101], qp[101], k[101], qk[101], svk[101], sr, si, u, v, a, b,
	     c__, d__, a1, a2, a3, a6, a7, e, f, g, h__, szr, szi, lzr, lzi;
    real eta, are, mre;
    integer n, nn;
} global_;

#define global_1 global_

/* Table of constant values */

static doublereal c_b41 = 1.;

/*<    >*/
/* Subroutine */ int rpoly_(doublereal *op, integer *degree, doublereal *
	zeror, doublereal *zeroi, logical *fail)
{
    /* System generated locals */
    integer i__1;
    real r__1, r__2;
    doublereal d__1;

    /* Builtin functions */
    double log(doublereal), pow_di(doublereal *, integer *), exp(doublereal);

    /* Local variables */
    integer i__, j, l;
    doublereal t;
    real x;
    doublereal aa, bb, cc;
    real df, ff;
    integer jj;
    real sc, lo, dx, pt[101], xm;
    integer nz;
    real xx, yy;
    integer nm1;
    real bnd, min__, max__;
    integer cnt;
    real xxx, base;
    extern /* Subroutine */ int quad_(doublereal *, doublereal *, doublereal *
	    , doublereal *, doublereal *, doublereal *, doublereal *);
    doublereal temp[101];
    real cosr, sinr, infin;
    logical zerok;
    doublereal factor;
    real smalno;
    extern /* Subroutine */ int fxshfr_(integer *, integer *);

/* FINDS THE ZEROS OF A REAL POLYNOMIAL */
/* OP  - DOUBLE PRECISION VECTOR OF COEFFICIENTS IN */
/*       ORDER OF DECREASING POWERS. */
/* DEGREE   - INTEGER DEGREE OF POLYNOMIAL. */
/* ZEROR, ZEROI - OUTPUT DOUBLE PRECISION VECTORS OF */
/*                REAL AND IMAGINARY PARTS OF THE */
/*                ZEROS. */
/* FAIL  - OUTPUT LOGICAL PARAMETER, TRUE ONLY IF */
/*         LEADING COEFFICIENT IS ZERO OR IF RPOLY */
/*         HAS FOUND FEWER THAN DEGREE ZEROS. */
/*         IN THE LATTER CASE DEGREE IS RESET TO */
/*         THE NUMBER OF ZEROS FOUND. */
/* TO CHANGE THE SIZE OF POLYNOMIALS WHICH CAN BE */
/* SOLVED, RESET THE DIMENSIONS OF THE ARRAYS IN THE */
/* COMMON AREA AND IN THE FOLLOWING DECLARATIONS. */
/* THE SUBROUTINE USES SINGLE PRECISION CALCULATIONS */
/* FOR SCALING, BOUNDS AND ERROR CALCULATIONS. ALL */
/* CALCULATIONS FOR THE ITERATIONS ARE DONE IN DOUBLE */
/* PRECISION. */
/*<    >*/
/*<    >*/
/*<       REAL ETA, ARE, MRE >*/
/*<       INTEGER N, NN >*/
/*<    >*/
/*<    >*/
/*<       INTEGER DEGREE, CNT, NZ, I, J, JJ, NM1 >*/
/*<       LOGICAL FAIL, ZEROK >*/
/* THE FOLLOWING STATEMENTS SET MACHINE CONSTANTS USED */
/* IN VARIOUS PARTS OF THE PROGRAM. THE MEANING OF THE */
/* FOUR CONSTANTS ARE... */
/* ETA     THE MAXIMUM RELATIVE REPRESENTATION ERROR */
/*         WHICH CAN BE DESCRIBED AS THE SMALLEST */
/*         POSITIVE FLOATING POINT NUMBER SUCH THAT */
/*         1.D0+ETA IS GREATER THAN 1. */
/* INFINY  THE LARGEST FLOATING-POINT NUMBER. */
/* SMALNO  THE SMALLEST POSITIVE FLOATING-POINT NUMBER */
/*         IF THE EXPONENT RANGE DIFFERS IN SINGLE AND */
/*         DOUBLE PRECISION THEN SMALNO AND INFIN */
/*         SHOULD INDICATE THE SMALLER RANGE. */
/* BASE    THE BASE OF THE FLOATING-POINT NUMBER */
/*         SYSTEM USED. */
/* THE VALUES BELOW CORRESPOND TO THE BURROUGHS B6700 */
/*<       BASE = 8. >*/
    /* Parameter adjustments */
    --zeroi;
    --zeror;
    --op;

    /* Function Body */
    base = (float)8.;
/*<       ETA = .5*BASE**(1-26) >*/
/* Computing 25th power */
    r__1 = 1 / base, r__2 = r__1, r__1 *= r__1, r__1 *= r__1, r__1 *= r__1,
	    r__2 *= r__1;
    global_1.eta = r__2 * (r__1 * r__1) * (float).5;
/*<       INFIN = 4.3E68 >*/
    infin = (float)4.3e68;
/*<       SMALNO = 1.0E-45 >*/
    smalno = (float)1e-45;
/* ARE AND MRE REFER TO THE UNIT ERROR IN + AND * */
/* RESPECTIVELY. THEY ARE ASSUMED TO BE THE SAME AS */
/* ETA. */
/*<       ARE = ETA >*/
    global_1.are = global_1.eta;
/*<       MRE = ETA >*/
    global_1.mre = global_1.eta;
/*<       LO = SMALNO/ETA >*/
    lo = smalno / global_1.eta;
/* INITIALIZATION OF CONSTANTS FOR SHIFT ROTATION */
/*<       XX = .70710678 >*/
    xx = (float).70710678;
/*<       YY = -XX >*/
    yy = -xx;
/*<       COSR = -.069756474 >*/
    cosr = (float)-.069756474;
/*<       SINR = .99756405 >*/
    sinr = (float).99756405;
/*<       FAIL = .FALSE. >*/
    *fail = FALSE_;
/*<       N = DEGREE >*/
    global_1.n = *degree;
/*<       NN = N + 1 >*/
    global_1.nn = global_1.n + 1;
/* ALGORITHM FAILS IF THE LEADING COEFFICIENT IS ZERO. */
/*<       IF (OP(1).NE.0.D0) GO TO 10 >*/
    if (op[1] != 0.) {
	goto L10;
    }
/*<       FAIL = .TRUE. >*/
    *fail = TRUE_;
/*<       DEGREE = 0 >*/
    *degree = 0;
/*<       RETURN >*/
    return 0;
/* REMOVE THE ZEROS AT THE ORIGIN IF ANY */
/*<    10 IF (OP(NN).NE.0.0D0) GO TO 20 >*/
L10:
    if (op[global_1.nn] != 0.) {
	goto L20;
    }
/*<       J = DEGREE - N + 1 >*/
    j = *degree - global_1.n + 1;
/*<       ZEROR(J) = 0.D0 >*/
    zeror[j] = 0.;
/*<       ZEROI(J) = 0.D0 >*/
    zeroi[j] = 0.;
/*<       NN = NN - 1 >*/
    --global_1.nn;
/*<       N = N - 1 >*/
    --global_1.n;
/*<       GO TO 10 >*/
    goto L10;
/* MAKE A COPY OF THE COEFFICIENTS */
/*<    20 DO 30 I=1,NN >*/
L20:
    i__1 = global_1.nn;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<         P(I) = OP(I) >*/
	global_1.p[i__ - 1] = op[i__];
/*<    30 CONTINUE >*/
/* L30: */
    }
/* START THE ALGORITHM FOR ONE ZERO */
/*<    40 IF (N.GT.2) GO TO 60 >*/
L40:
    if (global_1.n > 2) {
	goto L60;
    }
/*<       IF (N.LT.1) RETURN >*/
    if (global_1.n < 1) {
	return 0;
    }
/* CALCULATE THE FINAL ZERO OR PAIR OF ZEROS */
/*<       IF (N.EQ.2) GO TO 50 >*/
    if (global_1.n == 2) {
	goto L50;
    }
/*<       ZEROR(DEGREE) = -P(2)/P(1) >*/
    zeror[*degree] = -global_1.p[1] / global_1.p[0];
/*<       ZEROI(DEGREE) = 0.0D0 >*/
    zeroi[*degree] = 0.;
/*<       RETURN >*/
    return 0;
/*<    >*/
L50:
    quad_(global_1.p, &global_1.p[1], &global_1.p[2], &zeror[*degree - 1], &
	    zeroi[*degree - 1], &zeror[*degree], &zeroi[*degree]);
/*<       RETURN >*/
    return 0;
/* FIND LARGEST AND SMALLEST MODULI OF COEFFICIENTS. */
/*<    60 MAX = 0. >*/
L60:
    max__ = (float)0.;
/*<       MIN = INFIN >*/
    min__ = infin;
/*<       DO 70 I=1,NN >*/
    i__1 = global_1.nn;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<         X = ABS(SNGL(P(I))) >*/
	x = (r__1 = (real) global_1.p[i__ - 1], dabs(r__1));
/*<         IF (X.GT.MAX) MAX = X >*/
	if (x > max__) {
	    max__ = x;
	}
/*<         IF (X.NE.0. .AND. X.LT.MIN) MIN = X >*/
	if (x != (float)0. && x < min__) {
	    min__ = x;
	}
/*<    70 CONTINUE >*/
/* L70: */
    }
/* SCALE IF THERE ARE LARGE OR VERY SMALL COEFFICIENTS */
/* COMPUTES A SCALE FACTOR TO MULTIPLY THE */
/* COEFFICIENTS OF THE POLYNOMIAL. THE SCALING IS DONE */
/* TO AVOID OVERFLOW AND TO AVOID UNDETECTED UNDERFLOW */
/* INTERFERING WITH THE CONVERGENCE CRITERION. */
/* THE FACTOR IS A POWER OF THE BASE */
/*<       SC = LO/MIN >*/
    sc = lo / min__;
/*<       IF (SC.GT.1.0) GO TO 80 >*/
    if (sc > (float)1.) {
	goto L80;
    }
/*<       IF (MAX.LT.10.) GO TO 110 >*/
    if (max__ < (float)10.) {
	goto L110;
    }
/*<       IF (SC.EQ.0.) SC = SMALNO >*/
    if (sc == (float)0.) {
	sc = smalno;
    }
/*<       GO TO 90 >*/
    goto L90;
/*<    80 IF (INFIN/SC.LT.MAX) GO TO 110 >*/
L80:
    if (infin / sc < max__) {
	goto L110;
    }
/*<    90 L = ALOG(SC)/ALOG(BASE) + .5 >*/
L90:
    l = log(sc) / log(base) + (float).5;
/*<       FACTOR = (BASE*1.0D0)**L >*/
    d__1 = base * 1.;
    factor = pow_di(&d__1, &l);
/*<       IF (FACTOR.EQ.1.D0) GO TO 110 >*/
    if (factor == 1.) {
	goto L110;
    }
/*<       DO 100 I=1,NN >*/
    i__1 = global_1.nn;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<         P(I) = FACTOR*P(I) >*/
	global_1.p[i__ - 1] = factor * global_1.p[i__ - 1];
/*<   100 CONTINUE >*/
/* L100: */
    }
/* COMPUTE LOWER BOUND ON MODULI OF ZEROS. */
/*<   110 DO 120 I=1,NN >*/
L110:
    i__1 = global_1.nn;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<         PT(I) = ABS(SNGL(P(I))) >*/
	pt[i__ - 1] = (r__1 = (real) global_1.p[i__ - 1], dabs(r__1));
/*<   120 CONTINUE >*/
/* L120: */
    }
/*<       PT(NN) = -PT(NN) >*/
    pt[global_1.nn - 1] = -pt[global_1.nn - 1];
/* COMPUTE UPPER ESTIMATE OF BOUND */
/*<       X = EXP((ALOG(-PT(NN))-ALOG(PT(1)))/FLOAT(N)) >*/
    x = exp((log(-pt[global_1.nn - 1]) - log(pt[0])) / (real) global_1.n);
/*<       IF (PT(N).EQ.0.) GO TO 130 >*/
    if (pt[global_1.n - 1] == (float)0.) {
	goto L130;
    }
/* IF NEWTON STEP AT THE ORIGIN IS BETTER, USE IT. */
/*<       XM = -PT(NN)/PT(N) >*/
    xm = -pt[global_1.nn - 1] / pt[global_1.n - 1];
/*<       IF (XM.LT.X) X = XM >*/
    if (xm < x) {
	x = xm;
    }
/* CHOP THE INTERVAL (0,X) UNTIL FF .LE. 0 */
/*<   130 XM = X*.1 >*/
L130:
    xm = x * (float).1;
/*<       FF = PT(1) >*/
    ff = pt[0];
/*<       DO 140 I=2,NN >*/
    i__1 = global_1.nn;
    for (i__ = 2; i__ <= i__1; ++i__) {
/*<         FF = FF*XM + PT(I) >*/
	ff = ff * xm + pt[i__ - 1];
/*<   140 CONTINUE >*/
/* L140: */
    }
/*<       IF (FF.LE.0.) GO TO 150 >*/
    if (ff <= (float)0.) {
	goto L150;
    }
/*<       X = XM >*/
    x = xm;
/*<       GO TO 130 >*/
    goto L130;
/*<   150 DX = X >*/
L150:
    dx = x;
/* DO NEWTON ITERATION UNTIL X CONVERGES TO TWO */
/* DECIMAL PLACES */
/*<   160 IF (ABS(DX/X).LE..005) GO TO 180 >*/
L160:
    if ((r__1 = dx / x, dabs(r__1)) <= (float).005) {
	goto L180;
    }
/*<       FF = PT(1) >*/
    ff = pt[0];
/*<       DF = FF >*/
    df = ff;
/*<       DO 170 I=2,N >*/
    i__1 = global_1.n;
    for (i__ = 2; i__ <= i__1; ++i__) {
/*<         FF = FF*X + PT(I) >*/
	ff = ff * x + pt[i__ - 1];
/*<         DF = DF*X + FF >*/
	df = df * x + ff;
/*<   170 CONTINUE >*/
/* L170: */
    }
/*<       FF = FF*X + PT(NN) >*/
    ff = ff * x + pt[global_1.nn - 1];
/*<       DX = FF/DF >*/
    dx = ff / df;
/*<       X = X - DX >*/
    x -= dx;
/*<       GO TO 160 >*/
    goto L160;
/*<   180 BND = X >*/
L180:
    bnd = x;
/* COMPUTE THE DERIVATIVE AS THE INITIAL K POLYNOMIAL */
/* AND DO 5 STEPS WITH NO SHIFT */
/*<       NM1 = N - 1 >*/
    nm1 = global_1.n - 1;
/*<       DO 190 I=2,N >*/
    i__1 = global_1.n;
    for (i__ = 2; i__ <= i__1; ++i__) {
/*<         K(I) = FLOAT(NN-I)*P(I)/FLOAT(N) >*/
	global_1.k[i__ - 1] = (real) (global_1.nn - i__) * global_1.p[i__ - 1]
		 / (real) global_1.n;
/*<   190 CONTINUE >*/
/* L190: */
    }
/*<       K(1) = P(1) >*/
    global_1.k[0] = global_1.p[0];
/*<       AA = P(NN) >*/
    aa = global_1.p[global_1.nn - 1];
/*<       BB = P(N) >*/
    bb = global_1.p[global_1.n - 1];
/*<       ZEROK = K(N).EQ.0.D0 >*/
    zerok = global_1.k[global_1.n - 1] == 0.;
/*<       DO 230 JJ=1,5 >*/
    for (jj = 1; jj <= 5; ++jj) {
/*<         CC = K(N) >*/
	cc = global_1.k[global_1.n - 1];
/*<         IF (ZEROK) GO TO 210 >*/
	if (zerok) {
	    goto L210;
	}
/* USE SCALED FORM OF RECURRENCE IF VALUE OF K AT 0 IS */
/* NONZERO */
/*<         T = -AA/CC >*/
	t = -aa / cc;
/*<         DO 200 I=1,NM1 >*/
	i__1 = nm1;
	for (i__ = 1; i__ <= i__1; ++i__) {
/*<           J = NN - I >*/
	    j = global_1.nn - i__;
/*<           K(J) = T*K(J-1) + P(J) >*/
	    global_1.k[j - 1] = t * global_1.k[j - 2] + global_1.p[j - 1];
/*<   200   CONTINUE >*/
/* L200: */
	}
/*<         K(1) = P(1) >*/
	global_1.k[0] = global_1.p[0];
/*<         ZEROK = DABS(K(N)).LE.DABS(BB)*ETA*10. >*/
	zerok = (d__1 = global_1.k[global_1.n - 1], abs(d__1)) <= abs(bb) *
		global_1.eta * (float)10.;
/*<         GO TO 230 >*/
	goto L230;
/* USE UNSCALED FORM OF RECURRENCE */
/*<   210   DO 220 I=1,NM1 >*/
L210:
	i__1 = nm1;
	for (i__ = 1; i__ <= i__1; ++i__) {
/*<           J = NN - I >*/
	    j = global_1.nn - i__;
/*<           K(J) = K(J-1) >*/
	    global_1.k[j - 1] = global_1.k[j - 2];
/*<   220   CONTINUE >*/
/* L220: */
	}
/*<         K(1) = 0.D0 >*/
	global_1.k[0] = 0.;
/*<         ZEROK = K(N).EQ.0.D0 >*/
	zerok = global_1.k[global_1.n - 1] == 0.;
/*<   230 CONTINUE >*/
L230:
	;
    }
/* SAVE K FOR RESTARTS WITH NEW SHIFTS */
/*<       DO 240 I=1,N >*/
    i__1 = global_1.n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<         TEMP(I) = K(I) >*/
	temp[i__ - 1] = global_1.k[i__ - 1];
/*<   240 CONTINUE >*/
/* L240: */
    }
/* LOOP TO SELECT THE QUADRATIC  CORRESPONDING TO EACH */
/* NEW SHIFT */
/*<       DO 280 CNT=1,20 >*/
    for (cnt = 1; cnt <= 20; ++cnt) {
/* QUADRATIC CORRESPONDS TO A DOUBLE SHIFT TO A */
/* NON-REAL POINT AND ITS COMPLEX CONJUGATE. THE POINT */
/* HAS MODULUS BND AND AMPLITUDE ROTATED BY 94 DEGREES */
/* FROM THE PREVIOUS SHIFT */
/*<         XXX = COSR*XX - SINR*YY >*/
	xxx = cosr * xx - sinr * yy;
/*<         YY = SINR*XX + COSR*YY >*/
	yy = sinr * xx + cosr * yy;
/*<         XX = XXX >*/
	xx = xxx;
/*<         SR = BND*XX >*/
	global_1.sr = bnd * xx;
/*<         SI = BND*YY >*/
	global_1.si = bnd * yy;
/*<         U = -2.0D0*SR >*/
	global_1.u = global_1.sr * -2.;
/*<         V = BND >*/
	global_1.v = bnd;
/* SECOND STAGE CALCULATION, FIXED QUADRATIC */
/*<         CALL FXSHFR(20*CNT, NZ) >*/
	i__1 = cnt * 20;
	fxshfr_(&i__1, &nz);
/*<         IF (NZ.EQ.0) GO TO 260 >*/
	if (nz == 0) {
	    goto L260;
	}
/* THE SECOND STAGE JUMPS DIRECTLY TO ONE OF THE THIRD */
/* STAGE ITERATIONS AND RETURNS HERE IF SUCCESSFUL. */
/* DEFLATE THE POLYNOMIAL, STORE THE ZERO OR ZEROS AND */
/* RETURN TO THE MAIN ALGORITHM. */
/*<         J = DEGREE - N + 1 >*/
	j = *degree - global_1.n + 1;
/*<         ZEROR(J) = SZR >*/
	zeror[j] = global_1.szr;
/*<         ZEROI(J) = SZI >*/
	zeroi[j] = global_1.szi;
/*<         NN = NN - NZ >*/
	global_1.nn -= nz;
/*<         N = NN - 1 >*/
	global_1.n = global_1.nn - 1;
/*<         DO 250 I=1,NN >*/
	i__1 = global_1.nn;
	for (i__ = 1; i__ <= i__1; ++i__) {
/*<           P(I) = QP(I) >*/
	    global_1.p[i__ - 1] = global_1.qp[i__ - 1];
/*<   250   CONTINUE >*/
/* L250: */
	}
/*<         IF (NZ.EQ.1) GO TO 40 >*/
	if (nz == 1) {
	    goto L40;
	}
/*<         ZEROR(J+1) = LZR >*/
	zeror[j + 1] = global_1.lzr;
/*<         ZEROI(J+1) = LZI >*/
	zeroi[j + 1] = global_1.lzi;
/*<         GO TO 40 >*/
	goto L40;
/* IF THE ITERATION IS UNSUCCESSFUL ANOTHER QUADRATIC */
/* IS CHOSEN AFTER RESTORING K */
/*<   260   DO 270 I=1,N >*/
L260:
	i__1 = global_1.n;
	for (i__ = 1; i__ <= i__1; ++i__) {
/*<           K(I) = TEMP(I) >*/
	    global_1.k[i__ - 1] = temp[i__ - 1];
/*<   270   CONTINUE >*/
/* L270: */
	}
/*<   280 CONTINUE >*/
/* L280: */
    }
/* RETURN WITH FAILURE IF NO CONVERGENCE WITH 20 */
/* SHIFTS */
/*<       FAIL = .TRUE. >*/
    *fail = TRUE_;
/*<       DEGREE = DEGREE - N >*/
    *degree -= global_1.n;
/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* rpoly_ */

/*<       SUBROUTINE FXSHFR(L2, NZ)                                          >*/
/* Subroutine */ int fxshfr_(integer *l2, integer *nz)
{
    /* System generated locals */
    integer i__1, i__2;
    real r__1;

    /* Local variables */
    integer i__, j;
    doublereal s, ui, vi;
    real ss, ts, tv, vv, oss, ots, otv, tss, ovv;
    doublereal svu, svv;
    real tvv;
    integer type__;
    logical stry, vtry;
    integer iflag;
    real betas, betav;
    logical spass;
    extern /* Subroutine */ int nextk_(integer *);
    logical vpass;
    extern /* Subroutine */ int calcsc_(integer *), realit_(doublereal *,
	    integer *, integer *), quadsd_(integer *, doublereal *,
	    doublereal *, doublereal *, doublereal *, doublereal *,
	    doublereal *), quadit_(doublereal *, doublereal *, integer *),
	    newest_(integer *, doublereal *, doublereal *);

/* COMPUTES UP TO  L2  FIXED SHIFT K-POLYNOMIALS, */
/* TESTING FOR CONVERGENCE IN THE LINEAR OR QUADRATIC */
/* CASE. INITIATES ONE OF THE VARIABLE SHIFT */
/* ITERATIONS AND RETURNS WITH THE NUMBER OF ZEROS */
/* FOUND. */
/* L2 - LIMIT OF FIXED SHIFT STEPS */
/* NZ - NUMBER OF ZEROS FOUND */
/*<    >*/
/*<    >*/
/*<       REAL ETA, ARE, MRE >*/
/*<       INTEGER N, NN >*/
/*<       DOUBLE PRECISION SVU, SVV, UI, VI, S >*/
/*<    >*/
/*<       INTEGER L2, NZ, TYPE, I, J, IFLAG >*/
/*<       LOGICAL VPASS, SPASS, VTRY, STRY >*/
/*<       NZ = 0 >*/
    *nz = 0;
/*<       BETAV = .25 >*/
    betav = (float).25;
/*<       BETAS = .25 >*/
    betas = (float).25;
/*<       OSS = SR >*/
    oss = global_1.sr;
/*<       OVV = V >*/
    ovv = global_1.v;
/* EVALUATE POLYNOMIAL BY SYNTHETIC DIVISION */
/*<       CALL QUADSD(NN, U, V, P, QP, A, B) >*/
    quadsd_(&global_1.nn, &global_1.u, &global_1.v, global_1.p, global_1.qp, &
	    global_1.a, &global_1.b);
/*<       CALL CALCSC(TYPE) >*/
    calcsc_(&type__);
/*<       DO 80 J=1,L2 >*/
    i__1 = *l2;
    for (j = 1; j <= i__1; ++j) {
/* CALCULATE NEXT K POLYNOMIAL AND ESTIMATE V */
/*<         CALL NEXTK(TYPE) >*/
	nextk_(&type__);
/*<         CALL CALCSC(TYPE) >*/
	calcsc_(&type__);
/*<         CALL NEWEST(TYPE, UI, VI) >*/
	newest_(&type__, &ui, &vi);
/*<         VV = VI >*/
	vv = vi;
/* ESTIMATE S */
/*<         SS = 0. >*/
	ss = (float)0.;
/*<         IF (K(N).NE.0.D0) SS = -P(NN)/K(N) >*/
	if (global_1.k[global_1.n - 1] != 0.) {
	    ss = -global_1.p[global_1.nn - 1] / global_1.k[global_1.n - 1];
	}
/*<         TV = 1. >*/
	tv = (float)1.;
/*<         TS = 1. >*/
	ts = (float)1.;
/*<         IF (J.EQ.1 .OR. TYPE.EQ.3) GO TO 70 >*/
	if (j == 1 || type__ == 3) {
	    goto L70;
	}
/* COMPUTE RELATIVE MEASURES OF CONVERGENCE OF S AND V */
/* SEQUENCES */
/*<         IF (VV.NE.0.) TV = ABS((VV-OVV)/VV) >*/
	if (vv != (float)0.) {
	    tv = (r__1 = (vv - ovv) / vv, dabs(r__1));
	}
/*<         IF (SS.NE.0.) TS = ABS((SS-OSS)/SS) >*/
	if (ss != (float)0.) {
	    ts = (r__1 = (ss - oss) / ss, dabs(r__1));
	}
/* IF DECREASING, MULTIPLY TWO MOST RECENT */
/* CONVERGENCE MEASURES */
/*<         TVV = 1. >*/
	tvv = (float)1.;
/*<         IF (TV.LT.OTV) TVV = TV*OTV >*/
	if (tv < otv) {
	    tvv = tv * otv;
	}
/*<         TSS = 1. >*/
	tss = (float)1.;
/*<         IF (TS.LT.OTS) TSS = TS*OTS >*/
	if (ts < ots) {
	    tss = ts * ots;
	}
/* COMPARE WITH CONVERGENCE CRITERIA */
/*<         VPASS = TVV.LT.BETAV >*/
	vpass = tvv < betav;
/*<         SPASS = TSS.LT.BETAS >*/
	spass = tss < betas;
/*<         IF (.NOT.(SPASS .OR. VPASS)) GO TO 70 >*/
	if (! (spass || vpass)) {
	    goto L70;
	}
/* AT LEAST ONE SEQUENCE HAS PASSED THE CONVERGENCE */
/* TEST. STORE VARIABLES BEFORE ITERATING */
/*<         SVU = U >*/
	svu = global_1.u;
/*<         SVV = V >*/
	svv = global_1.v;
/*<         DO 10 I=1,N >*/
	i__2 = global_1.n;
	for (i__ = 1; i__ <= i__2; ++i__) {
/*<           SVK(I) = K(I) >*/
	    global_1.svk[i__ - 1] = global_1.k[i__ - 1];
/*<    10   CONTINUE >*/
/* L10: */
	}
/*<         S = SS >*/
	s = ss;
/* CHOOSE ITERATION ACCORDING TO THE FASTEST */
/* CONVERGING SEQUENCE */
/*<         VTRY = .FALSE. >*/
	vtry = FALSE_;
/*<         STRY = .FALSE. >*/
	stry = FALSE_;
/*<    >*/
	if (spass && (! vpass || tss < tvv)) {
	    goto L40;
	}
/*<    20   CALL QUADIT(UI, VI, NZ) >*/
L20:
	quadit_(&ui, &vi, nz);
/*<         IF (NZ.GT.0) RETURN >*/
	if (*nz > 0) {
	    return 0;
	}
/* QUADRATIC ITERATION HAS FAILED. FLAG THAT IT HAS */
/* BEEN TRIED AND DECREASE THE CONVERGENCE CRITERION. */
/*<         VTRY = .TRUE. >*/
	vtry = TRUE_;
/*<         BETAV = BETAV*.25 >*/
	betav *= (float).25;
/* TRY LINEAR ITERATION IF IT HAS NOT BEEN TRIED AND */
/* THE S SEQUENCE IS CONVERGING */
/*<         IF (STRY .OR. (.NOT.SPASS)) GO TO 50 >*/
	if (stry || ! spass) {
	    goto L50;
	}
/*<         DO 30 I=1,N >*/
	i__2 = global_1.n;
	for (i__ = 1; i__ <= i__2; ++i__) {
/*<           K(I) = SVK(I) >*/
	    global_1.k[i__ - 1] = global_1.svk[i__ - 1];
/*<    30   CONTINUE >*/
/* L30: */
	}
/*<    40   CALL REALIT(S, NZ, IFLAG) >*/
L40:
	realit_(&s, nz, &iflag);
/*<         IF (NZ.GT.0) RETURN >*/
	if (*nz > 0) {
	    return 0;
	}
/* LINEAR ITERATION HAS FAILED. FLAG THAT IT HAS BEEN */
/* TRIED AND DECREASE THE CONVERGENCE CRITERION */
/*<         STRY = .TRUE. >*/
	stry = TRUE_;
/*<         BETAS = BETAS*.25 >*/
	betas *= (float).25;
/*<         IF (IFLAG.EQ.0) GO TO 50 >*/
	if (iflag == 0) {
	    goto L50;
	}
/* IF LINEAR ITERATION SIGNALS AN ALMOST DOUBLE REAL */
/* ZERO ATTEMPT QUADRATIC INTERATION */
/*<         UI = -(S+S) >*/
	ui = -(s + s);
/*<         VI = S*S >*/
	vi = s * s;
/*<         GO TO 20 >*/
	goto L20;
/* RESTORE VARIABLES */
/*<    50   U = SVU >*/
L50:
	global_1.u = svu;
/*<         V = SVV >*/
	global_1.v = svv;
/*<         DO 60 I=1,N >*/
	i__2 = global_1.n;
	for (i__ = 1; i__ <= i__2; ++i__) {
/*<           K(I) = SVK(I) >*/
	    global_1.k[i__ - 1] = global_1.svk[i__ - 1];
/*<    60   CONTINUE >*/
/* L60: */
	}
/* TRY QUADRATIC ITERATION IF IT HAS NOT BEEN TRIED */
/* AND THE V SEQUENCE IS CONVERGING */
/*<         IF (VPASS .AND. (.NOT.VTRY)) GO TO 20 >*/
	if (vpass && ! vtry) {
	    goto L20;
	}
/* RECOMPUTE QP AND SCALAR VALUES TO CONTINUE THE */
/* SECOND STAGE */
/*<         CALL QUADSD(NN, U, V, P, QP, A, B) >*/
	quadsd_(&global_1.nn, &global_1.u, &global_1.v, global_1.p,
		global_1.qp, &global_1.a, &global_1.b);
/*<         CALL CALCSC(TYPE) >*/
	calcsc_(&type__);
/*<    70   OVV = VV >*/
L70:
	ovv = vv;
/*<         OSS = SS >*/
	oss = ss;
/*<         OTV = TV >*/
	otv = tv;
/*<         OTS = TS >*/
	ots = ts;
/*<    80 CONTINUE >*/
/* L80: */
    }
/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* fxshfr_ */

/*<       SUBROUTINE QUADIT(UU, VV, NZ)                                      >*/
/* Subroutine */ int quadit_(doublereal *uu, doublereal *vv, integer *nz)
{
    /* System generated locals */
    integer i__1;
    real r__1, r__2;
    doublereal d__1, d__2;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    integer i__, j;
    real t, ee;
    doublereal ui, vi;
    real mp, zm, omp;
    extern /* Subroutine */ int quad_(doublereal *, doublereal *, doublereal *
	    , doublereal *, doublereal *, doublereal *, doublereal *);
    integer type__;
    logical tried;
    extern /* Subroutine */ int nextk_(integer *), calcsc_(integer *),
	    quadsd_(integer *, doublereal *, doublereal *, doublereal *,
	    doublereal *, doublereal *, doublereal *), newest_(integer *,
	    doublereal *, doublereal *);
    real relstp;

/* VARIABLE-SHIFT K-POLYNOMIAL ITERATION FOR A */
/* QUADRATIC FACTOR CONVERGES ONLY IF THE ZEROS ARE */
/* EQUIMODULAR OR NEARLY SO. */
/* UU,VV - COEFFICIENTS OF STARTING QUADRATIC */
/* NZ - NUMBER OF ZERO FOUND */
/*<    >*/
/*<    >*/
/*<       REAL ETA, ARE, MRE >*/
/*<       INTEGER N, NN >*/
/*<       DOUBLE PRECISION UI, VI, UU, VV, DABS >*/
/*<       REAL MS, MP, OMP, EE, RELSTP, T, ZM >*/
/*<       INTEGER NZ, TYPE, I, J >*/
/*<       LOGICAL TRIED >*/
/*<       NZ = 0 >*/
    *nz = 0;
/*<       TRIED = .FALSE. >*/
    tried = FALSE_;
/*<       U = UU >*/
    global_1.u = *uu;
/*<       V = VV >*/
    global_1.v = *vv;
/*<       J = 0 >*/
    j = 0;
/* MAIN LOOP */
/*<    10 CALL QUAD(1.D0, U, V, SZR, SZI, LZR, LZI) >*/
L10:
    quad_(&c_b41, &global_1.u, &global_1.v, &global_1.szr, &global_1.szi, &
	    global_1.lzr, &global_1.lzi);
/* RETURN IF ROOTS OF THE QUADRATIC ARE REAL AND NOT */
/* CLOSE TO MULTIPLE OR NEARLY EQUAL AND  OF OPPOSITE */
/* SIGN */
/*<    >*/
    if ((d__1 = abs(global_1.szr) - abs(global_1.lzr), abs(d__1)) > abs(
	    global_1.lzr) * .01) {
	return 0;
    }
/* EVALUATE POLYNOMIAL BY QUADRATIC SYNTHETIC DIVISION */
/*<       CALL QUADSD(NN, U, V, P, QP, A, B) >*/
    quadsd_(&global_1.nn, &global_1.u, &global_1.v, global_1.p, global_1.qp, &
	    global_1.a, &global_1.b);
/*<       MP = DABS(A-SZR*B) + DABS(SZI*B) >*/
    mp = (d__1 = global_1.a - global_1.szr * global_1.b, abs(d__1)) + (d__2 =
	    global_1.szi * global_1.b, abs(d__2));
/* COMPUTE A RIGOROUS  BOUND ON THE ROUNDING ERROR IN */
/* EVALUTING P */
/*<       ZM = SQRT(ABS(SNGL(V))) >*/
    zm = sqrt((r__1 = (real) global_1.v, dabs(r__1)));
/*<       EE = 2.*ABS(SNGL(QP(1))) >*/
    ee = (r__1 = (real) global_1.qp[0], dabs(r__1)) * (float)2.;
/*<       T = -SZR*B >*/
    t = -global_1.szr * global_1.b;
/*<       DO 20 I=2,N >*/
    i__1 = global_1.n;
    for (i__ = 2; i__ <= i__1; ++i__) {
/*<         EE = EE*ZM + ABS(SNGL(QP(I))) >*/
	ee = ee * zm + (r__1 = (real) global_1.qp[i__ - 1], dabs(r__1));
/*<    20 CONTINUE >*/
/* L20: */
    }
/*<       EE = EE*ZM + ABS(SNGL(A)+T) >*/
    ee = ee * zm + (r__1 = (real) global_1.a + t, dabs(r__1));
/*<    >*/
    ee = (global_1.mre * (float)5. + global_1.are * (float)4.) * ee - (
	    global_1.mre * (float)5. + global_1.are * (float)2.) * ((r__2 = (
	    real) global_1.a + t, dabs(r__2)) + (r__1 = (real) global_1.b,
	    dabs(r__1)) * zm) + global_1.are * (float)2. * dabs(t);
/* ITERATION HAS CONVERGED SUFFICIENTLY IF THE */
/* POLYNOMIAL VALUE IS LESS THAN 20 TIMES THIS BOUND */
/*<       IF (MP.GT.20.*EE) GO TO 30 >*/
    if (mp > ee * (float)20.) {
	goto L30;
    }
/*<       NZ = 2 >*/
    *nz = 2;
/*<       RETURN >*/
    return 0;
/*<    30 J = J + 1 >*/
L30:
    ++j;
/* STOP ITERATION AFTER 20 STEPS */
/*<       IF (J.GT.20) RETURN >*/
    if (j > 20) {
	return 0;
    }
/*<       IF (J.LT.2) GO TO 50 >*/
    if (j < 2) {
	goto L50;
    }
/*<    >*/
    if (relstp > (float).01 || mp < omp || tried) {
	goto L50;
    }
/* A CLUSTER APPEARS TO BE STALLING THE CONVERGENCE. */
/* FIVE FIXED SHIFT STEPS ARE TAKEN WITH A U,V CLOSE */
/* TO THE CLUSTER */
/*<       IF (RELSTP.LT.ETA) RELSTP = ETA >*/
    if (relstp < global_1.eta) {
	relstp = global_1.eta;
    }
/*<       RELSTP = SQRT(RELSTP) >*/
    relstp = sqrt(relstp);
/*<       U = U - U*RELSTP >*/
    global_1.u -= global_1.u * relstp;
/*<       V = V + V*RELSTP >*/
    global_1.v += global_1.v * relstp;
/*<       CALL QUADSD(NN, U, V, P, QP, A, B) >*/
    quadsd_(&global_1.nn, &global_1.u, &global_1.v, global_1.p, global_1.qp, &
	    global_1.a, &global_1.b);
/*<       DO 40 I=1,5 >*/
    for (i__ = 1; i__ <= 5; ++i__) {
/*<         CALL CALCSC(TYPE) >*/
	calcsc_(&type__);
/*<         CALL NEXTK(TYPE) >*/
	nextk_(&type__);
/*<    40 CONTINUE >*/
/* L40: */
    }
/*<       TRIED = .TRUE. >*/
    tried = TRUE_;
/*<       J = 0 >*/
    j = 0;
/*<    50 OMP = MP >*/
L50:
    omp = mp;
/* CALCULATE NEXT K POLYNOMIAL AND NEW U AND V */
/*<       CALL CALCSC(TYPE) >*/
    calcsc_(&type__);
/*<       CALL NEXTK(TYPE) >*/
    nextk_(&type__);
/*<       CALL CALCSC(TYPE) >*/
    calcsc_(&type__);
/*<       CALL NEWEST(TYPE, UI, VI) >*/
    newest_(&type__, &ui, &vi);
/* IF VI IS ZERO THE ITERATION IS NOT CONVERGING */
/*<       IF (VI.EQ.0.D0) RETURN >*/
    if (vi == 0.) {
	return 0;
    }
/*<       RELSTP = DABS((VI-V)/VI) >*/
    relstp = (d__1 = (vi - global_1.v) / vi, abs(d__1));
/*<       U = UI >*/
    global_1.u = ui;
/*<       V = VI >*/
    global_1.v = vi;
/*<       GO TO 10 >*/
    goto L10;
/*<       END >*/
} /* quadit_ */

/*<       SUBROUTINE REALIT(SSS, NZ, IFLAG)                                  >*/
/* Subroutine */ int realit_(doublereal *sss, integer *nz, integer *iflag)
{
    /* System generated locals */
    integer i__1;
    real r__1;
    doublereal d__1;

    /* Local variables */
    integer i__, j;
    doublereal s, t;
    real ee, mp, ms;
    doublereal kv, pv;
    integer nm1;
    real omp;

/* VARIABLE-SHIFT H POLYNOMIAL ITERATION FOR A REAL */
/* ZERO. */
/* SSS   - STARTING ITERATE */
/* NZ    - NUMBER OF ZERO FOUND */
/* IFLAG - FLAG TO INDICATE A PAIR OF ZEROS NEAR REAL */
/*         AXIS. */
/*<    >*/
/*<    >*/
/*<       REAL ETA, ARE, MRE >*/
/*<       INTEGER N, NN >*/
/*<       DOUBLE PRECISION PV, KV, T, S, SSS, DABS >*/
/*<       REAL MS, MP, OMP, EE >*/
/*<       INTEGER NZ, IFLAG, I, J, NM1 >*/
/*<       NM1 = N - 1 >*/
    nm1 = global_1.n - 1;
/*<       NZ = 0 >*/
    *nz = 0;
/*<       S = SSS >*/
    s = *sss;
/*<       IFLAG = 0 >*/
    *iflag = 0;
/*<       J = 0 >*/
    j = 0;
/* MAIN LOOP */
/*<    10 PV = P(1) >*/
L10:
    pv = global_1.p[0];
/* EVALUATE P AT S */
/*<       QP(1) = PV >*/
    global_1.qp[0] = pv;
/*<       DO 20 I=2,NN >*/
    i__1 = global_1.nn;
    for (i__ = 2; i__ <= i__1; ++i__) {
/*<         PV = PV*S + P(I) >*/
	pv = pv * s + global_1.p[i__ - 1];
/*<         QP(I) = PV >*/
	global_1.qp[i__ - 1] = pv;
/*<    20 CONTINUE >*/
/* L20: */
    }
/*<       MP = DABS(PV) >*/
    mp = abs(pv);
/* COMPUTE A RIGOROUS BOUND ON THE ERROR IN EVALUATING */
/* P */
/*<       MS = DABS(S) >*/
    ms = abs(s);
/*<       EE = (MRE/(ARE+MRE))*ABS(SNGL(QP(1))) >*/
    ee = global_1.mre / (global_1.are + global_1.mre) * (r__1 = (real)
	    global_1.qp[0], dabs(r__1));
/*<       DO 30 I=2,NN >*/
    i__1 = global_1.nn;
    for (i__ = 2; i__ <= i__1; ++i__) {
/*<         EE = EE*MS + ABS(SNGL(QP(I))) >*/
	ee = ee * ms + (r__1 = (real) global_1.qp[i__ - 1], dabs(r__1));
/*<    30 CONTINUE >*/
/* L30: */
    }
/* ITERATION HAS CONVERGED SUFFICIENTLY IF THE */
/* POLYNOMIAL VALUE IS LESS THAN 20 TIMES THIS BOUND */
/*<       IF (MP.GT.20.*((ARE+MRE)*EE-MRE*MP)) GO TO 40 >*/
    if (mp > ((global_1.are + global_1.mre) * ee - global_1.mre * mp) * (
	    float)20.) {
	goto L40;
    }
/*<       NZ = 1 >*/
    *nz = 1;
/*<       SZR = S >*/
    global_1.szr = s;
/*<       SZI = 0.D0 >*/
    global_1.szi = 0.;
/*<       RETURN >*/
    return 0;
/*<    40 J = J + 1 >*/
L40:
    ++j;
/* STOP ITERATION AFTER 10 STEPS */
/*<       IF (J.GT.10) RETURN >*/
    if (j > 10) {
	return 0;
    }
/*<       IF (J.LT.2) GO TO 50 >*/
    if (j < 2) {
	goto L50;
    }
/*<    >*/
    if (abs(t) > (d__1 = s - t, abs(d__1)) * (float).001 || mp <= omp) {
	goto L50;
    }
/* A CLUSTER OF ZEROS NEAR THE REAL AXIS HAS BEEN */
/* ENCOUNTERED RETURN WITH IFLAG SET TO INITIATE A */
/* QUADRATIC ITERATION */
/*<       IFLAG = 1 >*/
    *iflag = 1;
/*<       SSS = S >*/
    *sss = s;
/*<       RETURN >*/
    return 0;
/* RETURN IF THE POLYNOMIAL VALUE HAS INCREASED */
/* SIGNIFICANTLY */
/*<    50 OMP = MP >*/
L50:
    omp = mp;
/* COMPUTE T, THE NEXT POLYNOMIAL, AND THE NEW ITERATE */
/*<       KV = K(1) >*/
    kv = global_1.k[0];
/*<       QK(1) = KV >*/
    global_1.qk[0] = kv;
/*<       DO 60 I=2,N >*/
    i__1 = global_1.n;
    for (i__ = 2; i__ <= i__1; ++i__) {
/*<         KV = KV*S + K(I) >*/
	kv = kv * s + global_1.k[i__ - 1];
/*<         QK(I) = KV >*/
	global_1.qk[i__ - 1] = kv;
/*<    60 CONTINUE >*/
/* L60: */
    }
/*<       IF (DABS(KV).LE.DABS(K(N))*10.*ETA) GO TO 80 >*/
    if (abs(kv) <= (d__1 = global_1.k[global_1.n - 1], abs(d__1)) * (float)
	    10. * global_1.eta) {
	goto L80;
    }
/* USE THE SCALED FORM OF THE RECURRENCE IF THE VALUE */
/* OF K AT S IS NONZERO */
/*<       T = -PV/KV >*/
    t = -pv / kv;
/*<       K(1) = QP(1) >*/
    global_1.k[0] = global_1.qp[0];
/*<       DO 70 I=2,N >*/
    i__1 = global_1.n;
    for (i__ = 2; i__ <= i__1; ++i__) {
/*<         K(I) = T*QK(I-1) + QP(I) >*/
	global_1.k[i__ - 1] = t * global_1.qk[i__ - 2] + global_1.qp[i__ - 1];
/*<    70 CONTINUE >*/
/* L70: */
    }
/*<       GO TO 100 >*/
    goto L100;
/* USE UNSCALED FORM */
/*<    80 K(1) = 0.0D0 >*/
L80:
    global_1.k[0] = 0.;
/*<       DO 90 I=2,N >*/
    i__1 = global_1.n;
    for (i__ = 2; i__ <= i__1; ++i__) {
/*<         K(I) = QK(I-1) >*/
	global_1.k[i__ - 1] = global_1.qk[i__ - 2];
/*<    90 CONTINUE >*/
/* L90: */
    }
/*<   100 KV = K(1) >*/
L100:
    kv = global_1.k[0];
/*<       DO 110 I=2,N >*/
    i__1 = global_1.n;
    for (i__ = 2; i__ <= i__1; ++i__) {
/*<         KV = KV*S + K(I) >*/
	kv = kv * s + global_1.k[i__ - 1];
/*<   110 CONTINUE >*/
/* L110: */
    }
/*<       T = 0.D0 >*/
    t = 0.;
/*<       IF (DABS(KV).GT.DABS(K(N))*10.*ETA) T = -PV/KV >*/
    if (abs(kv) > (d__1 = global_1.k[global_1.n - 1], abs(d__1)) * (float)10.
	    * global_1.eta) {
	t = -pv / kv;
    }
/*<       S = S + T >*/
    s += t;
/*<       GO TO 10 >*/
    goto L10;
/*<       END >*/
} /* realit_ */

/*<       SUBROUTINE CALCSC(TYPE)                                            >*/
/* Subroutine */ int calcsc_(integer *type__)
{
    /* System generated locals */
    doublereal d__1;

    /* Local variables */
    extern /* Subroutine */ int quadsd_(integer *, doublereal *, doublereal *,
	     doublereal *, doublereal *, doublereal *, doublereal *);

/* THIS ROUTINE CALCULATES SCALAR QUANTITIES USED TO */
/* COMPUTE THE NEXT K POLYNOMIAL AND NEW ESTIMATES OF */
/* THE QUADRATIC COEFFICIENTS. */
/* TYPE - INTEGER VARIABLE SET HERE INDICATING HOW THE */
/* CALCULATIONS ARE NORMALIZED TO AVOID OVERFLOW */
/*<    >*/
/*<    >*/
/*<       REAL ETA, ARE, MRE >*/
/*<       INTEGER N, NN >*/
/*<       DOUBLE PRECISION DABS >*/
/*<       INTEGER TYPE >*/
/* SYNTHETIC DIVISION OF K BY THE QUADRATIC 1,U,V */
/*<       CALL QUADSD(N, U, V, K, QK, C, D) >*/
    quadsd_(&global_1.n, &global_1.u, &global_1.v, global_1.k, global_1.qk, &
	    global_1.c__, &global_1.d__);
/*<       IF (DABS(C).GT.DABS(K(N))*100.*ETA) GO TO 10 >*/
    if (abs(global_1.c__) > (d__1 = global_1.k[global_1.n - 1], abs(d__1)) * (
	    float)100. * global_1.eta) {
	goto L10;
    }
/*<       IF (DABS(D).GT.DABS(K(N-1))*100.*ETA) GO TO 10 >*/
    if (abs(global_1.d__) > (d__1 = global_1.k[global_1.n - 2], abs(d__1)) * (
	    float)100. * global_1.eta) {
	goto L10;
    }
/*<       TYPE = 3 >*/
    *type__ = 3;
/* TYPE=3 INDICATES THE QUADRATIC IS ALMOST A FACTOR */
/* OF K */
/*<       RETURN >*/
    return 0;
/*<    10 IF (DABS(D).LT.DABS(C)) GO TO 20 >*/
L10:
    if (abs(global_1.d__) < abs(global_1.c__)) {
	goto L20;
    }
/*<       TYPE = 2 >*/
    *type__ = 2;
/* TYPE=2 INDICATES THAT ALL FORMULAS ARE DIVIDED BY D */
/*<       E = A/D >*/
    global_1.e = global_1.a / global_1.d__;
/*<       F = C/D >*/
    global_1.f = global_1.c__ / global_1.d__;
/*<       G = U*B >*/
    global_1.g = global_1.u * global_1.b;
/*<       H = V*B >*/
    global_1.h__ = global_1.v * global_1.b;
/*<       A3 = (A+G)*E + H*(B/D) >*/
    global_1.a3 = (global_1.a + global_1.g) * global_1.e + global_1.h__ * (
	    global_1.b / global_1.d__);
/*<       A1 = B*F - A >*/
    global_1.a1 = global_1.b * global_1.f - global_1.a;
/*<       A7 = (F+U)*A + H >*/
    global_1.a7 = (global_1.f + global_1.u) * global_1.a + global_1.h__;
/*<       RETURN >*/
    return 0;
/*<    20 TYPE = 1 >*/
L20:
    *type__ = 1;
/* TYPE=1 INDICATES THAT ALL FORMULAS ARE DIVIDED BY C */
/*<       E = A/C >*/
    global_1.e = global_1.a / global_1.c__;
/*<       F = D/C >*/
    global_1.f = global_1.d__ / global_1.c__;
/*<       G = U*E >*/
    global_1.g = global_1.u * global_1.e;
/*<       H = V*B >*/
    global_1.h__ = global_1.v * global_1.b;
/*<       A3 = A*E + (H/C+G)*B >*/
    global_1.a3 = global_1.a * global_1.e + (global_1.h__ / global_1.c__ +
	    global_1.g) * global_1.b;
/*<       A1 = B - A*(D/C) >*/
    global_1.a1 = global_1.b - global_1.a * (global_1.d__ / global_1.c__);
/*<       A7 = A + G*D + H*F >*/
    global_1.a7 = global_1.a + global_1.g * global_1.d__ + global_1.h__ *
	    global_1.f;
/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* calcsc_ */

/*<       SUBROUTINE NEXTK(TYPE)                                             >*/
/* Subroutine */ int nextk_(integer *type__)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    integer i__;
    doublereal temp;

/* COMPUTES THE NEXT K POLYNOMIALS USING SCALARS */
/* COMPUTED IN CALCSC */
/*<    >*/
/*<    >*/
/*<       REAL ETA, ARE, MRE >*/
/*<       INTEGER N, NN >*/
/*<       DOUBLE PRECISION TEMP, DABS >*/
/*<       INTEGER TYPE >*/
/*<       IF (TYPE.EQ.3) GO TO 40 >*/
    if (*type__ == 3) {
	goto L40;
    }
/*<       TEMP = A >*/
    temp = global_1.a;
/*<       IF (TYPE.EQ.1) TEMP = B >*/
    if (*type__ == 1) {
	temp = global_1.b;
    }
/*<       IF (DABS(A1).GT.DABS(TEMP)*ETA*10.) GO TO 20 >*/
    if (abs(global_1.a1) > abs(temp) * global_1.eta * (float)10.) {
	goto L20;
    }
/* IF A1 IS NEARLY ZERO THEN USE A SPECIAL FORM OF THE */
/* RECURRENCE */
/*<       K(1) = 0.D0 >*/
    global_1.k[0] = 0.;
/*<       K(2) = -A7*QP(1) >*/
    global_1.k[1] = -global_1.a7 * global_1.qp[0];
/*<       DO 10 I=3,N >*/
    i__1 = global_1.n;
    for (i__ = 3; i__ <= i__1; ++i__) {
/*<         K(I) = A3*QK(I-2) - A7*QP(I-1) >*/
	global_1.k[i__ - 1] = global_1.a3 * global_1.qk[i__ - 3] -
		global_1.a7 * global_1.qp[i__ - 2];
/*<    10 CONTINUE >*/
/* L10: */
    }
/*<       RETURN >*/
    return 0;
/* USE SCALED FORM OF THE RECURRENCE */
/*<    20 A7 = A7/A1 >*/
L20:
    global_1.a7 /= global_1.a1;
/*<       A3 = A3/A1 >*/
    global_1.a3 /= global_1.a1;
/*<       K(1) = QP(1) >*/
    global_1.k[0] = global_1.qp[0];
/*<       K(2) = QP(2) - A7*QP(1) >*/
    global_1.k[1] = global_1.qp[1] - global_1.a7 * global_1.qp[0];
/*<       DO 30 I=3,N >*/
    i__1 = global_1.n;
    for (i__ = 3; i__ <= i__1; ++i__) {
/*<         K(I) = A3*QK(I-2) - A7*QP(I-1) + QP(I) >*/
	global_1.k[i__ - 1] = global_1.a3 * global_1.qk[i__ - 3] -
		global_1.a7 * global_1.qp[i__ - 2] + global_1.qp[i__ - 1];
/*<    30 CONTINUE >*/
/* L30: */
    }
/*<       RETURN >*/
    return 0;
/* USE UNSCALED FORM OF THE RECURRENCE IF TYPE IS 3 */
/*<    40 K(1) = 0.D0 >*/
L40:
    global_1.k[0] = 0.;
/*<       K(2) = 0.D0 >*/
    global_1.k[1] = 0.;
/*<       DO 50 I=3,N >*/
    i__1 = global_1.n;
    for (i__ = 3; i__ <= i__1; ++i__) {
/*<         K(I) = QK(I-2) >*/
	global_1.k[i__ - 1] = global_1.qk[i__ - 3];
/*<    50 CONTINUE >*/
/* L50: */
    }
/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* nextk_ */

/*<       SUBROUTINE NEWEST(TYPE, UU, VV)                                    >*/
/* Subroutine */ int newest_(integer *type__, doublereal *uu, doublereal *vv)
{
    doublereal a4, a5, b1, b2, c1, c2, c3, c4, temp;

/* COMPUTE NEW ESTIMATES OF THE QUADRATIC COEFFICIENTS */
/* USING THE SCALARS COMPUTED IN CALCSC. */
/*<    >*/
/*<    >*/
/*<       REAL ETA, ARE, MRE >*/
/*<       INTEGER N, NN >*/
/*<    >*/
/*<       INTEGER TYPE >*/
/* USE FORMULAS APPROPRIATE TO SETTING OF TYPE. */
/*<       IF (TYPE.EQ.3) GO TO 30 >*/
    if (*type__ == 3) {
	goto L30;
    }
/*<       IF (TYPE.EQ.2) GO TO 10 >*/
    if (*type__ == 2) {
	goto L10;
    }
/*<       A4 = A + U*B + H*F >*/
    a4 = global_1.a + global_1.u * global_1.b + global_1.h__ * global_1.f;
/*<       A5 = C + (U+V*F)*D >*/
    a5 = global_1.c__ + (global_1.u + global_1.v * global_1.f) * global_1.d__;
/*<       GO TO 20 >*/
    goto L20;
/*<    10 A4 = (A+G)*F + H >*/
L10:
    a4 = (global_1.a + global_1.g) * global_1.f + global_1.h__;
/*<       A5 = (F+U)*C + V*D >*/
    a5 = (global_1.f + global_1.u) * global_1.c__ + global_1.v * global_1.d__;
/* EVALUATE NEW QUADRATIC COEFFICIENTS. */
/*<    20 B1 = -K(N)/P(NN) >*/
L20:
    b1 = -global_1.k[global_1.n - 1] / global_1.p[global_1.nn - 1];
/*<       B2 = -(K(N-1)+B1*P(N))/P(NN) >*/
    b2 = -(global_1.k[global_1.n - 2] + b1 * global_1.p[global_1.n - 1]) /
	    global_1.p[global_1.nn - 1];
/*<       C1 = V*B2*A1 >*/
    c1 = global_1.v * b2 * global_1.a1;
/*<       C2 = B1*A7 >*/
    c2 = b1 * global_1.a7;
/*<       C3 = B1*B1*A3 >*/
    c3 = b1 * b1 * global_1.a3;
/*<       C4 = C1 - C2 - C3 >*/
    c4 = c1 - c2 - c3;
/*<       TEMP = A5 + B1*A4 - C4 >*/
    temp = a5 + b1 * a4 - c4;
/*<       IF (TEMP.EQ.0.D0) GO TO 30 >*/
    if (temp == 0.) {
	goto L30;
    }
/*<       UU = U - (U*(C3+C2)+V*(B1*A1+B2*A7))/TEMP >*/
    *uu = global_1.u - (global_1.u * (c3 + c2) + global_1.v * (b1 *
	    global_1.a1 + b2 * global_1.a7)) / temp;
/*<       VV = V*(1.+C4/TEMP) >*/
    *vv = global_1.v * (c4 / temp + (float)1.);
/*<       RETURN >*/
    return 0;
/* IF TYPE=3 THE QUADRATIC IS ZEROED */
/*<    30 UU = 0.D0 >*/
L30:
    *uu = 0.;
/*<       VV = 0.D0 >*/
    *vv = 0.;
/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* newest_ */

/*<       SUBROUTINE QUADSD(NN, U, V, P, Q, A, B)                            >*/
/* Subroutine */ int quadsd_(integer *nn, doublereal *u, doublereal *v,
	doublereal *p, doublereal *q, doublereal *a, doublereal *b)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    doublereal c__;
    integer i__;

/* DIVIDES P BY THE QUADRATIC  1,U,V  PLACING THE */
/* QUOTIENT IN Q AND THE REMAINDER IN A,B */
/*<       DOUBLE PRECISION P(NN), Q(NN), U, V, A, B, C >*/
/*<       INTEGER I >*/
/*<       B = P(1) >*/
    /* Parameter adjustments */
    --q;
    --p;

    /* Function Body */
    *b = p[1];
/*<       Q(1) = B >*/
    q[1] = *b;
/*<       A = P(2) - U*B >*/
    *a = p[2] - *u * *b;
/*<       Q(2) = A >*/
    q[2] = *a;
/*<       DO 10 I=3,NN >*/
    i__1 = *nn;
    for (i__ = 3; i__ <= i__1; ++i__) {
/*<         C = P(I) - U*A - V*B >*/
	c__ = p[i__] - *u * *a - *v * *b;
/*<         Q(I) = C >*/
	q[i__] = c__;
/*<         B = A >*/
	*b = *a;
/*<         A = C >*/
	*a = c__;
/*<    10 CONTINUE >*/
/* L10: */
    }
/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* quadsd_ */

/*<       SUBROUTINE QUAD(A, B1, C, SR, SI, LR, LI)                          >*/
/* Subroutine */ int quad_(doublereal *a, doublereal *b1, doublereal *c__,
	doublereal *sr, doublereal *si, doublereal *lr, doublereal *li)
{
    /* System generated locals */
    doublereal d__1;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    doublereal b, d__, e;

/* CALCULATE THE ZEROS OF THE QUADRATIC A*Z**2+B1*Z+C. */
/* THE QUADRATIC FORMULA, MODIFIED TO AVOID */
/* OVERFLOW, IS USED TO FIND THE LARGER ZERO IF THE */
/* ZEROS ARE REAL AND BOTH ZEROS ARE COMPLEX. */
/* THE SMALLER REAL ZERO IS FOUND DIRECTLY FROM THE */
/* PRODUCT OF THE ZEROS C/A. */
/*<    >*/
/*<       IF (A.NE.0.D0) GO TO 20 >*/
    if (*a != 0.) {
	goto L20;
    }
/*<       SR = 0.D0 >*/
    *sr = 0.;
/*<       IF (B1.NE.0.D0) SR = -C/B1 >*/
    if (*b1 != 0.) {
	*sr = -(*c__) / *b1;
    }
/*<       LR = 0.D0 >*/
    *lr = 0.;
/*<    10 SI = 0.D0 >*/
L10:
    *si = 0.;
/*<       LI = 0.D0 >*/
    *li = 0.;
/*<       RETURN >*/
    return 0;
/*<    20 IF (C.NE.0.D0) GO TO 30 >*/
L20:
    if (*c__ != 0.) {
	goto L30;
    }
/*<       SR = 0.D0 >*/
    *sr = 0.;
/*<       LR = -B1/A >*/
    *lr = -(*b1) / *a;
/*<       GO TO 10 >*/
    goto L10;
/* COMPUTE DISCRIMINANT AVOIDING OVERFLOW */
/*<    30 B = B1/2.D0 >*/
L30:
    b = *b1 / 2.;
/*<       IF (DABS(B).LT.DABS(C)) GO TO 40 >*/
    if (abs(b) < abs(*c__)) {
	goto L40;
    }
/*<       E = 1.D0 - (A/B)*(C/B) >*/
    e = 1. - *a / b * (*c__ / b);
/*<       D = DSQRT(DABS(E))*DABS(B) >*/
    d__ = sqrt((abs(e))) * abs(b);
/*<       GO TO 50 >*/
    goto L50;
/*<    40 E = A >*/
L40:
    e = *a;
/*<       IF (C.LT.0.D0) E = -A >*/
    if (*c__ < 0.) {
	e = -(*a);
    }
/*<       E = B*(B/DABS(C)) - E >*/
    e = b * (b / abs(*c__)) - e;
/*<       D = DSQRT(DABS(E))*DSQRT(DABS(C)) >*/
    d__ = sqrt((abs(e))) * sqrt((abs(*c__)));
/*<    50 IF (E.LT.0.D0) GO TO 60 >*/
L50:
    if (e < 0.) {
	goto L60;
    }
/* REAL ZEROS */
/*<       IF (B.GE.0.D0) D = -D >*/
    if (b >= 0.) {
	d__ = -d__;
    }
/*<       LR = (-B+D)/A >*/
    *lr = (-b + d__) / *a;
/*<       SR = 0.D0 >*/
    *sr = 0.;
/*<       IF (LR.NE.0.D0) SR = (C/LR)/A >*/
    if (*lr != 0.) {
	*sr = *c__ / *lr / *a;
    }
/*<       GO TO 10 >*/
    goto L10;
/* COMPLEX CONJUGATE ZEROS */
/*<    60 SR = -B/A >*/
L60:
    *sr = -b / *a;
/*<       LR = SR >*/
    *lr = *sr;
/*<       SI = DABS(D/A) >*/
    *si = (d__1 = d__ / *a, abs(d__1));
/*<       LI = -SI >*/
    *li = -(*si);
/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* quad_ */

#ifdef __cplusplus
	}
#endif
