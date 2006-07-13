/* toms/trans.f -- translated by f2c (version 20050501).
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

/*<       SUBROUTINE TRANS(A, M, N, MN, MOVE, IWRK, IOK)                     >*/
/* Subroutine */ int trans_(real *a, integer *m, integer *n, integer *mn, 
	integer *move, integer *iwrk, integer *iok)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Local variables */
    real b, c__, d__;
    integer i__, j, k, i1, i2, j1, n1, im, i1c, i2c, ir0, ir1, ir2, kmi, 
	    max__, ncount;

/* ***** */
/*  ALGORITHM 380 - REVISED */
/* ***** */
/*  A IS A ONE-DIMENSIONAL ARRAY OF LENGTH MN=M*N, WHICH */
/*  CONTAINS THE MXN MATRIX TO BE TRANSPOSED (STORED */
/*  COLUMWISE). MOVE IS A ONE-DIMENSIONAL ARRAY OF LENGTH IWRK */
/*  USED TO STORE INFORMATION TO SPEED UP THE PROCESS.  THE */
/*  VALUE IWRK=(M+N)/2 IS RECOMMENDED. IOK INDICATES THE */
/*  SUCCESS OR FAILURE OF THE ROUTINE. */
/*  NORMAL RETURN  IOK=0 */
/*  ERRORS         IOK=-1 ,MN NOT EQUAL TO M*N */
/*                 IOK=-2 ,IWRK NEGATIVE OR ZERO */
/*                 IOK.GT.0, (SHOULD NEVER OCCUR),IN THIS CASE */
/*  WE SET IOK EQUAL TO THE FINAL VALUE OF I WHEN THE SEARCH */
/*  IS COMPLETED BUT SOME LOOPS HAVE NOT BEEN MOVED */
/*  NOTE * MOVE(I) WILL STAY ZERO FOR FIXED POINTS */
/*<       DIMENSION A(MN), MOVE(IWRK) >*/
/* CHECK ARGUMENTS AND INITIALIZE. */
/*<       IF (M.LT.2 .OR. N.LT.2) GO TO 120 >*/
    /* Parameter adjustments */
    --a;
    --move;

    /* Function Body */
    if (*m < 2 || *n < 2) {
	goto L120;
    }
/*<       IF (MN.NE.M*N) GO TO 180 >*/
    if (*mn != *m * *n) {
	goto L180;
    }
/*<       IF (IWRK.LT.1) GO TO 190 >*/
    if (*iwrk < 1) {
	goto L190;
    }
/*<       IF (M.EQ.N) GO TO 130 >*/
    if (*m == *n) {
	goto L130;
    }
/*<       NCOUNT = 2 >*/
    ncount = 2;
/*<       K = MN - 1 >*/
    k = *mn - 1;
/*<       DO 10 I=1,IWRK >*/
    i__1 = *iwrk;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<         MOVE(I) = 0 >*/
	move[i__] = 0;
/*<    10 CONTINUE >*/
/* L10: */
    }
/*<       IF (M.LT.3 .OR. N.LT.3) GO TO 30 >*/
    if (*m < 3 || *n < 3) {
	goto L30;
    }
/* CALCULATE THE NUMBER OF FIXED POINTS, EUCLIDS ALGORITHM */
/* FOR GCD(M-1,N-1). */
/*<       IR2 = M - 1 >*/
    ir2 = *m - 1;
/*<       IR1 = N - 1 >*/
    ir1 = *n - 1;
/*<    20 IR0 = MOD(IR2,IR1) >*/
L20:
    ir0 = ir2 % ir1;
/*<       IR2 = IR1 >*/
    ir2 = ir1;
/*<       IR1 = IR0 >*/
    ir1 = ir0;
/*<       IF (IR0.NE.0) GO TO 20 >*/
    if (ir0 != 0) {
	goto L20;
    }
/*<       NCOUNT = NCOUNT + IR2 - 1 >*/
    ncount = ncount + ir2 - 1;
/* SET INITIAL VALUES FOR SEARCH */
/*<    30 I = 1 >*/
L30:
    i__ = 1;
/*<       IM = M >*/
    im = *m;
/* AT LEAST ONE LOOP MUST BE RE-ARRANGED */
/*<       GO TO 80 >*/
    goto L80;
/* SEARCH FOR LOOPS TO REARRANGE */
/*<    40 MAX = K - I >*/
L40:
    max__ = k - i__;
/*<       I = I + 1 >*/
    ++i__;
/*<       IF (I.GT.MAX) GO TO 160 >*/
    if (i__ > max__) {
	goto L160;
    }
/*<       IM = IM + M >*/
    im += *m;
/*<       IF (IM.GT.K) IM = IM - K >*/
    if (im > k) {
	im -= k;
    }
/*<       I2 = IM >*/
    i2 = im;
/*<       IF (I.EQ.I2) GO TO 40 >*/
    if (i__ == i2) {
	goto L40;
    }
/*<       IF (I.GT.IWRK) GO TO 60 >*/
    if (i__ > *iwrk) {
	goto L60;
    }
/*<       IF (MOVE(I).EQ.0) GO TO 80 >*/
    if (move[i__] == 0) {
	goto L80;
    }
/*<       GO TO 40 >*/
    goto L40;
/*<    50 I2 = M*I1 - K*(I1/N) >*/
L50:
    i2 = *m * i1 - k * (i1 / *n);
/*<    60 IF (I2.LE.I .OR. I2.GE.MAX) GO TO 70 >*/
L60:
    if (i2 <= i__ || i2 >= max__) {
	goto L70;
    }
/*<       I1 = I2 >*/
    i1 = i2;
/*<       GO TO 50 >*/
    goto L50;
/*<    70 IF (I2.NE.I) GO TO 40 >*/
L70:
    if (i2 != i__) {
	goto L40;
    }
/* REARRANGE THE ELEMENTS OF A LOOP AND ITS COMPANION LOOP */
/*<    80 I1 = I >*/
L80:
    i1 = i__;
/*<       KMI = K - I >*/
    kmi = k - i__;
/*<       B = A(I1+1) >*/
    b = a[i1 + 1];
/*<       I1C = KMI >*/
    i1c = kmi;
/*<       C = A(I1C+1) >*/
    c__ = a[i1c + 1];
/*<    90 I2 = M*I1 - K*(I1/N) >*/
L90:
    i2 = *m * i1 - k * (i1 / *n);
/*<       I2C = K - I2 >*/
    i2c = k - i2;
/*<       IF (I1.LE.IWRK) MOVE(I1) = 2 >*/
    if (i1 <= *iwrk) {
	move[i1] = 2;
    }
/*<       IF (I1C.LE.IWRK) MOVE(I1C) = 2 >*/
    if (i1c <= *iwrk) {
	move[i1c] = 2;
    }
/*<       NCOUNT = NCOUNT + 2 >*/
    ncount += 2;
/*<       IF (I2.EQ.I) GO TO 110 >*/
    if (i2 == i__) {
	goto L110;
    }
/*<       IF (I2.EQ.KMI) GO TO 100 >*/
    if (i2 == kmi) {
	goto L100;
    }
/*<       A(I1+1) = A(I2+1) >*/
    a[i1 + 1] = a[i2 + 1];
/*<       A(I1C+1) = A(I2C+1) >*/
    a[i1c + 1] = a[i2c + 1];
/*<       I1 = I2 >*/
    i1 = i2;
/*<       I1C = I2C >*/
    i1c = i2c;
/*<       GO TO 90 >*/
    goto L90;
/* FINAL STORE AND TEST FOR FINISHED */
/*<   100 D = B >*/
L100:
    d__ = b;
/*<       B = C >*/
    b = c__;
/*<       C = D >*/
    c__ = d__;
/*<   110 A(I1+1) = B >*/
L110:
    a[i1 + 1] = b;
/*<       A(I1C+1) = C >*/
    a[i1c + 1] = c__;
/*<       IF (NCOUNT.LT.MN) GO TO 40 >*/
    if (ncount < *mn) {
	goto L40;
    }
/* NORMAL RETURN */
/*<   120 IOK = 0 >*/
L120:
    *iok = 0;
/*<       RETURN >*/
    return 0;
/* IF MATRIX IS SQUARE,EXCHANGE ELEMENTS A(I,J) AND A(J,I). */
/*<   130 N1 = N - 1 >*/
L130:
    n1 = *n - 1;
/*<       DO 150 I=1,N1 >*/
    i__1 = n1;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<         J1 = I + 1 >*/
	j1 = i__ + 1;
/*<         DO 140 J=J1,N >*/
	i__2 = *n;
	for (j = j1; j <= i__2; ++j) {
/*<           I1 = I + (J-1)*N >*/
	    i1 = i__ + (j - 1) * *n;
/*<           I2 = J + (I-1)*M >*/
	    i2 = j + (i__ - 1) * *m;
/*<           B = A(I1) >*/
	    b = a[i1];
/*<           A(I1) = A(I2) >*/
	    a[i1] = a[i2];
/*<           A(I2) = B >*/
	    a[i2] = b;
/*<   140   CONTINUE >*/
/* L140: */
	}
/*<   150 CONTINUE >*/
/* L150: */
    }
/*<       GO TO 120 >*/
    goto L120;
/* ERROR RETURNS. */
/*<   160 IOK = I >*/
L160:
    *iok = i__;
/*<   170 RETURN >*/
L170:
    return 0;
/*<   180 IOK = -1 >*/
L180:
    *iok = -1;
/*<       GO TO 170 >*/
    goto L170;
/*<   190 IOK = -2 >*/
L190:
    *iok = -2;
/*<       GO TO 170 >*/
    goto L170;
/*<       END >*/
} /* trans_ */

#ifdef __cplusplus
	}
#endif
