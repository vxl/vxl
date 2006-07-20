/* itpack/dsrc2c.f -- translated by f2c (version 20050501).
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
    integer in, is, isym, itmax, level, nout;
} itcom1_;

#define itcom1_1 itcom1_

Extern struct {
    logical adapt, betadt, caseii, halt, partad;
} itcom2_;

#define itcom2_1 itcom2_

Extern struct {
    doublereal bdelnm, betab, cme, delnnm, delsnm, ff, gamma, omega, qa, qt, 
	    rho, rrr, sige, sme, specr, spr, drelpr, stptst, udnm, zeta;
} itcom3_;

#define itcom3_1 itcom3_

/* Table of constant values */

static integer c__1 = 1;
static integer c__0 = 0;
static doublereal c_b21 = 0.;
static integer c__2 = 2;
static integer c__3 = 3;
static integer c__4 = 4;
static doublereal c_b286 = 1.;
static integer c__5 = 5;
static integer c__6 = 6;
static integer c__7 = 7;

/*<       SUBROUTINE JCG (NN,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IERR) >*/
/* Subroutine */ int jcg_(integer *nn, integer *ia, integer *ja, doublereal *
	a, doublereal *rhs, doublereal *u, integer *iwksp, integer *nw, 
	doublereal *wksp, integer *iparm, doublereal *rparm, integer *ierr)
{
    /* Format strings */
    static char fmt_10[] = "(\0020\002///1x,\002BEGINNING OF ITPACK SOLUTION\
 MODULE  JCG\002)";
    static char fmt_40[] = "(\0020\002,\002*** W A R N I N G ************\
\002/\0020\002,\002    IN ITPACK ROUTINE JCG\002/\002 \002,\002    RPARM(1) =\
\002,d10.3,\002 (ZETA)\002/\002 \002,\002    A VALUE THIS SMALL MAY HINDER C\
ONVERGENCE \002/\002 \002,\002    SINCE MACHINE PRECISION DRELPR =\002,d10.3/\
\002 \002,\002    ZETA RESET TO \002,d10.3)";
    static char fmt_60[] = "(\0020\002,\002*** F A T A L     E R R O R *****\
*******\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE JCG \002/\002 \002\
,\002    INVALID MATRIX DIMENSION, N =\002,i8)";
    static char fmt_80[] = "(\0020\002,\002*** F A T A L     E R R O R *****\
*******\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE JCG \002/\002 \002\
,\002    ERROR DETECTED IN SUBROUTINE  SBELM \002/\002 \002,\002    WHICH RE\
MOVES ROWS AND COLUMNS OF SYSTEM \002/\002 \002,\002    WHEN DIAGONAL ENTRY \
TOO LARGE  \002/\002 \002,\002    IER = \002,i5,5x,\002 RPARM(8) = \002,d10.\
3,\002 (TOL)\002)";
    static char fmt_100[] = "(\0020\002,\002*** F A T A L     E R R O R ****\
********\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE JCG \002/\002 \002,\
\002    NOT ENOUGH WORKSPACE AT \002,i10/\002 \002,\002    SET IPARM(8) =\
\002,i10,\002 (NW)\002)";
    static char fmt_120[] = "(\0020\002,\002*** F A T A L     E R R O R ****\
********\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE JCG \002/\002 \002,\
\002    ERROR DETECTED IN SUBROUTINE  PRBNDX\002/\002 \002,\002    WHICH COM\
PUTES THE RED-BLACK INDEXING\002/\002 \002,\002    IER = \002,i5,\002 IPARM(\
9) = \002,i5,\002 (NB)\002)";
    static char fmt_140[] = "(/10x,\002ORDER OF BLACK SUBSYSTEM = \002,i5\
,\002 (NB)\002)";
    static char fmt_150[] = "(\0020\002,\002*** F A T A L     E R R O R ****\
********\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE JCG \002/\002 \002,\
\002    ERROR DETECTED IN SUBROUTINE  PERMAT\002/\002 \002,\002    WHICH DOE\
S THE RED-BLACK PERMUTATION\002/\002 \002,\002    IER = \002,i5)";
    static char fmt_180[] = "(\0020\002,\002*** F A T A L     E R R O R ****\
********\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE JCG \002/\002 \002,\
\002    ERROR DETECTED IN SUBROUTINE  SCAL  \002/\002 \002,\002    WHICH SCA\
LES THE SYSTEM   \002/\002 \002,\002    IER = \002,i5)";
    static char fmt_200[] = "(///1x,\002IN THE FOLLOWING, RHO AND GAMMA AR\
E\002,\002 ACCELERATION PARAMETERS\002)";
    static char fmt_210[] = "(1x,\002CME IS THE ESTIMATE OF THE LARGEST EIGE\
NVALUE OF\002,\002 THE JACOBI MATRIX\002)";
    static char fmt_270[] = "(\0020\002,\002*** W A R N I N G ***********\
*\002/\0020\002,\002    IN ITPACK ROUTINE JCG\002/\002 \002,\002    FAILURE \
TO CONVERGE IN\002,i5,\002 ITERATIONS\002)";
    static char fmt_300[] = "(/1x,\002JCG  HAS CONVERGED IN \002,i5,\002 ITE\
RATIONS\002)";
    static char fmt_320[] = "(\0020\002,\002*** F A T A L     E R R O R ****\
********\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE JCG \002/\002 \002,\
\002    ERROR DETECTED IN SUBROUTINE  PERMAT\002/\002 \002,\002    WHICH UND\
OES THE RED-BLACK PERMUTATION   \002/\002 \002,\002    IER = \002,i5)";

    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), e_wsfe(), do_fio(integer *, char *, ftnlen);

    /* Local variables */
    integer n, n3, nb, ib1, ib2, ib3, ib4, ib5, ier;
    doublereal tol;
    extern /* Subroutine */ int pjac_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *), scal_(integer *, 
	    integer *, integer *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, integer *, integer *, integer *);
    doublereal temp;
    integer loop;
    doublereal time1, time2;
    real timi1, timj1, timi2, timj2;
    extern /* Subroutine */ int itjcg_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *), sbelm_(integer *, 
	    integer *, integer *, doublereal *, doublereal *, integer *, 
	    doublereal *, doublereal *, integer *, integer *, integer *, 
	    integer *);
    integer idgts;
    extern /* Subroutine */ int vfill_(integer *, doublereal *, doublereal *),
	     dcopy_(integer *, doublereal *, integer *, doublereal *, integer 
	    *);
    extern doublereal timer_(real *);
    real dummy;
    extern /* Subroutine */ int vevmw_(integer *, doublereal *, doublereal *);
    doublereal digit1, digit2;
    integer itmax1;
    extern /* Subroutine */ int echall_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, integer *, doublereal *, integer *), 
	    pervec_(integer *, doublereal *, integer *), ivfill_(integer *, 
	    integer *, integer *);
    integer ierper;
    extern /* Subroutine */ int echout_(integer *, doublereal *, integer *), 
	    permat_(integer *, integer *, integer *, doublereal *, integer *, 
	    integer *, integer *, integer *, integer *, integer *), unscal_(
	    integer *, integer *, integer *, doublereal *, doublereal *, 
	    doublereal *, doublereal *), prbndx_(integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *), perror_(integer *, integer *, integer *, doublereal *,
	     doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, integer *);

    /* Fortran I/O blocks */
    static cilist io___1 = { 0, 0, 0, fmt_10, 0 };
    static cilist io___7 = { 0, 0, 0, fmt_40, 0 };
    static cilist io___12 = { 0, 0, 0, fmt_60, 0 };
    static cilist io___14 = { 0, 0, 0, fmt_80, 0 };
    static cilist io___20 = { 0, 0, 0, fmt_100, 0 };
    static cilist io___23 = { 0, 0, 0, fmt_120, 0 };
    static cilist io___24 = { 0, 0, 0, fmt_140, 0 };
    static cilist io___25 = { 0, 0, 0, fmt_150, 0 };
    static cilist io___26 = { 0, 0, 0, fmt_180, 0 };
    static cilist io___27 = { 0, 0, 0, fmt_200, 0 };
    static cilist io___28 = { 0, 0, 0, fmt_210, 0 };
    static cilist io___33 = { 0, 0, 0, fmt_270, 0 };
    static cilist io___34 = { 0, 0, 0, fmt_300, 0 };
    static cilist io___36 = { 0, 0, 0, fmt_320, 0 };



/*     ITPACK 2C MAIN SUBROUTINE  JCG  (JACOBI CONJUGATE GRADIENT) */
/*     EACH OF THE MAIN SUBROUTINES: */
/*           JCG, JSI, SOR, SSORCG, SSORSI, RSCG, RSSI */
/*     CAN BE USED INDEPENDENTLY OF THE OTHERS */

/* ... FUNCTION: */

/*          THIS SUBROUTINE, JCG, DRIVES THE JACOBI CONJUGATE */
/*          GRADIENT ALGORITHM. */

/* ... PARAMETER LIST: */

/*          N      INPUT INTEGER.  DIMENSION OF THE MATRIX. (= NN) */
/*          IA,JA  INPUT INTEGER VECTORS.  THE TWO INTEGER ARRAYS OF */
/*                 THE SPARSE MATRIX REPRESENTATION. */
/*          A      INPUT D.P. VECTOR.  THE D.P. ARRAY OF THE SPARSE */
/*                 MATRIX REPRESENTATION. */
/*          RHS    INPUT D.P. VECTOR.  CONTAINS THE RIGHT HAND SIDE */
/*                 OF THE MATRIX PROBLEM. */
/*          U      INPUT/OUTPUT D.P. VECTOR.  ON INPUT, U CONTAINS THE */
/*                 INITIAL GUESS TO THE SOLUTION. ON OUTPUT, IT CONTAINS */
/*                 THE LATEST ESTIMATE TO THE SOLUTION. */
/*          IWKSP  INTEGER VECTOR WORKSPACE OF LENGTH 3*N */
/*          NW     INPUT INTEGER.  LENGTH OF AVAILABLE WKSP.  ON OUTPUT, */
/*                 IPARM(8) IS AMOUNT USED. */
/*          WKSP   D.P. VECTOR USED FOR WORKING SPACE.  JACOBI CONJUGATE */
/*                 GRADIENT NEEDS THIS TO BE IN LENGTH AT LEAST */
/*                 4*N + 2*ITMAX,  IF ISYM = 0  (SYMMETRIC STORAGE) */
/*                 4*N + 4*ITMAX,  IF ISYM = 1  (NONSYMMETRIC STORAGE) */
/*                 HERE ITMAX = IPARM(1) AND ISYM = IPARM(5) */
/*                 (ITMAX IS THE MAXIMUM ALLOWABLE NUMBER OF ITERATIONS) */
/*          IPARM  INTEGER VECTOR OF LENGTH 12.  ALLOWS USER TO SPECIFY */
/*                 SOME INTEGER PARAMETERS WHICH AFFECT THE METHOD. */
/*          RPARM  D.P. VECTOR OF LENGTH 12. ALLOWS USER TO SPECIFY SOME */
/*                 D.P. PARAMETERS WHICH AFFECT THE METHOD. */
/*          IER    OUTPUT INTEGER.  ERROR FLAG. (= IERR) */

/* ... JCG SUBPROGRAM REFERENCES: */

/*          FROM ITPACK    BISRCH, CHGCON, DETERM, DFAULT, ECHALL, */
/*                         ECHOUT, EIGVNS, EIGVSS, EQRT1S, ITERM, TIMER, */
/*                         ITJCG, IVFILL, PARCON, PERMAT, */
/*                         PERROR, PERVEC, PJAC, PMULT, PRBNDX, */
/*                         PSTOP, QSORT, DAXPY, SBELM, SCAL, DCOPY, */
/*                         DDOT, SUM3, UNSCAL, VEVMW, VFILL, VOUT, */
/*                         WEVMW, ZBRENT */
/*          SYSTEM         DABS, DLOG10, DBLE(AMAX0), DMAX1, MOD, DSQRT */

/*     VERSION:  ITPACK 2C (MARCH 1982) */

/*     CODE WRITTEN BY:  DAVID KINCAID, ROGER GRIMES, JOHN RESPESS */
/*                       CENTER FOR NUMERICAL ANALYSIS */
/*                       UNIVERSITY OF TEXAS */
/*                       AUSTIN, TX  78712 */
/*                       (512) 471-1242 */

/*     FOR ADDITIONAL DETAILS ON THE */
/*          (A) SUBROUTINE SEE TOMS ARTICLE 1982 */
/*          (B) ALGORITHM  SEE CNA REPORT 150 */

/*     BASED ON THEORY BY:  DAVID YOUNG, DAVID KINCAID, LOU HAGEMAN */

/*     REFERENCE THE BOOK:  APPLIED ITERATIVE METHODS */
/*                          L. HAGEMAN, D. YOUNG */
/*                          ACADEMIC PRESS, 1981 */

/*     ************************************************** */
/*     *               IMPORTANT NOTE                   * */
/*     *                                                * */
/*     *      WHEN INSTALLING ITPACK ROUTINES ON A      * */
/*     *  DIFFERENT COMPUTER, RESET SOME OF THE VALUES  * */
/*     *  IN  SUBROUTNE DFAULT.   MOST IMPORTANT ARE    * */
/*     *                                                * */
/*     *   DRELPR      MACHINE RELATIVE PRECISION       * */
/*     *   RPARM(1)    STOPPING CRITERION               * */
/*     *                                                * */
/*     *   ALSO CHANGE SYSTEM-DEPENDENT ROUTINE         * */
/*     *   SECOND USED IN TIMER                         * */
/*     *                                                * */
/*     ************************************************** */

/*     SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER IA(1),JA(1),IWKSP(1),IPARM(12),NN,NW,IERR >*/
/*<       DOUBLE PRECISION A(1),RHS(NN),U(NN),WKSP(NW),RPARM(12) >*/

/*     SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER IB1,IB2,IB3,IB4,IB5,IDGTS,IER,IERPER,ITMAX1,LOOP,N,NB,N3 >*/
/*<       DOUBLE PRECISION DIGIT1,DIGIT2,TEMP,TIME1,TIME2,TOL >*/

/* **** BEGIN: ITPACK COMMON */

/*<       INTEGER IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/
/*<       COMMON /ITCOM1/ IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/

/*<       LOGICAL ADAPT,BETADT,CASEII,HALT,PARTAD >*/
/*<       COMMON /ITCOM2/ ADAPT,BETADT,CASEII,HALT,PARTAD >*/

/*<    >*/
/*<    >*/

/* **** END  : ITPACK COMMON */

/* ... VARIABLES IN COMMON BLOCK - ITCOM1 */

/*     IN     - ITERATION NUMBER */
/*     IS     - ITERATION NUMBER WHEN PARAMETERS LAST CHANGED */
/*     ISYM   - SYMMETRIC/NONSYMMETRIC STORAGE FORMAT SWITCH */
/*     ITMAX  - MAXIMUM NUMBER OF ITERATIONS ALLOWED */
/*     LEVEL  - LEVEL OF OUTPUT CONTROL SWITCH */
/*     NOUT   - OUTPUT UNIT NUMBER */

/* ... VARIABLES IN COMMON BLOCK - ITCOM2 */

/*     ADAPT  - FULLY ADAPTIVE PROCEDURE SWITCH */
/*     BETADT - SWITCH FOR ADAPTIVE DETERMINATION OF BETA */
/*     CASEII - ADAPTIVE PROCEDURE CASE SWITCH */
/*     HALT   - STOPPING TEST SWITCH */
/*     PARTAD - PARTIALLY ADAPTIVE PROCEDURE SWITCH */

/* ... VARIABLES IN COMMON BLOCK - ITCOM3 */

/*     BDELNM - TWO NORM OF B TIMES DELTA-SUPER-N */
/*     BETAB  - ESTIMATE FOR THE SPECTRAL RADIUS OF LU MATRIX */
/*     CME    - ESTIMATE OF LARGEST EIGENVALUE */
/*     DELNNM - INNER PRODUCT OF PSEUDO-RESIDUAL AT ITERATION N */
/*     DELSNM - INNER PRODUCT OF PSEUDO-RESIDUAL AT ITERATION S */
/*     FF     - ADAPTIVE PROCEDURE DAMPING FACTOR */
/*     GAMMA  - ACCELERATION PARAMETER */
/*     OMEGA  - OVERRELAXATION PARAMETER FOR SOR AND SSOR */
/*     QA     - PSEUDO-RESIDUAL RATIO */
/*     QT     - VIRTUAL SPECTRAL RADIUS */
/*     RHO    - ACCELERATION PARAMETER */
/*     RRR    - ADAPTIVE PARAMETER */
/*     SIGE   - PARAMETER SIGMA-SUB-E */
/*     SME    - ESTIMATE OF SMALLEST EIGENVALUE */
/*     SPECR  - SPECTRAL RADIUS ESTIMATE FOR SSOR */
/*     DRELPR - MACHINE RELATIVE PRECISION */
/*     STPTST - STOPPING PARAMETER */
/*     UDNM   - TWO NORM OF U */
/*     ZETA   - STOPPING CRITERION */

/* ... INITIALIZE COMMON BLOCKS */

/*<       LEVEL = IPARM(2) >*/
    /* Parameter adjustments */
    --u;
    --rhs;
    --ia;
    --ja;
    --a;
    --iwksp;
    --wksp;
    --iparm;
    --rparm;

    /* Function Body */
    itcom1_1.level = iparm[2];
/*<       NOUT = IPARM(4) >*/
    itcom1_1.nout = iparm[4];
/*<       IF (LEVEL.GE.1) WRITE (NOUT,10) >*/
    if (itcom1_1.level >= 1) {
	io___1.ciunit = itcom1_1.nout;
	s_wsfe(&io___1);
	e_wsfe();
    }
/*<    10 FORMAT ('0'///1X,'BEGINNING OF ITPACK SOLUTION MODULE  JCG') >*/
/*<       IER = 0 >*/
    ier = 0;
/*<       IF (IPARM(1).LE.0) RETURN >*/
    if (iparm[1] <= 0) {
	return 0;
    }
/*<       N = NN >*/
    n = *nn;
/*<       IF (IPARM(11).EQ.0) TIMJ1 = TIMER(DUMMY) >*/
    if (iparm[11] == 0) {
	timj1 = timer_(&dummy);
    }
/*<       IF (LEVEL.GE.3) GO TO 20 >*/
    if (itcom1_1.level >= 3) {
	goto L20;
    }
/*<       CALL ECHOUT (IPARM,RPARM,1) >*/
    echout_(&iparm[1], &rparm[1], &c__1);
/*<       GO TO 30 >*/
    goto L30;
/*<    20 CALL ECHALL (N,IA,JA,A,RHS,IPARM,RPARM,1) >*/
L20:
    echall_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &iparm[1], &rparm[1], &c__1);
/*<    30 TEMP = 5.0D2*DRELPR >*/
L30:
    temp = itcom3_1.drelpr * 500.;
/*<       IF (ZETA.GE.TEMP) GO TO 50 >*/
    if (itcom3_1.zeta >= temp) {
	goto L50;
    }
/*<       IF (LEVEL.GE.1) WRITE (NOUT,40) ZETA,DRELPR,TEMP >*/
    if (itcom1_1.level >= 1) {
	io___7.ciunit = itcom1_1.nout;
	s_wsfe(&io___7);
	do_fio(&c__1, (char *)&itcom3_1.zeta, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&itcom3_1.drelpr, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&temp, (ftnlen)sizeof(doublereal));
	e_wsfe();
    }
/*<    >*/
/*<       ZETA = TEMP >*/
    itcom3_1.zeta = temp;
/*<    50 CONTINUE >*/
L50:
/*<       TIME1 = RPARM(9) >*/
    time1 = rparm[9];
/*<       TIME2 = RPARM(10) >*/
    time2 = rparm[10];
/*<       DIGIT1 = RPARM(11) >*/
    digit1 = rparm[11];
/*<       DIGIT2 = RPARM(12) >*/
    digit2 = rparm[12];

/* ... VERIFY N */

/*<       IF (N.GT.0) GO TO 70 >*/
    if (n > 0) {
	goto L70;
    }
/*<       IER = 11 >*/
    ier = 11;
/*<       IF (LEVEL.GE.0) WRITE (NOUT,60) N >*/
    if (itcom1_1.level >= 0) {
	io___12.ciunit = itcom1_1.nout;
	s_wsfe(&io___12);
	do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 370 >*/
    goto L370;
/*<    70 CONTINUE >*/
L70:

/* ... REMOVE ROWS AND COLUMNS IF REQUESTED */

/*<       IF (IPARM(10).EQ.0) GO TO 90 >*/
    if (iparm[10] == 0) {
	goto L90;
    }
/*<       TOL = RPARM(8) >*/
    tol = rparm[8];
/*<       CALL IVFILL (N,IWKSP,0) >*/
    ivfill_(&n, &iwksp[1], &c__0);
/*<       CALL VFILL (N,WKSP,0.0D0) >*/
    vfill_(&n, &wksp[1], &c_b21);
/*<       CALL SBELM (N,IA,JA,A,RHS,IWKSP,WKSP,TOL,ISYM,LEVEL,NOUT,IER) >*/
    sbelm_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &iwksp[1], &wksp[1], &tol, &
	    itcom1_1.isym, &itcom1_1.level, &itcom1_1.nout, &ier);
/*<       IF (IER.EQ.0) GO TO 90 >*/
    if (ier == 0) {
	goto L90;
    }
/*<       IF (LEVEL.GE.0) WRITE (NOUT,80) IER,TOL >*/
    if (itcom1_1.level >= 0) {
	io___14.ciunit = itcom1_1.nout;
	s_wsfe(&io___14);
	do_fio(&c__1, (char *)&ier, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&tol, (ftnlen)sizeof(doublereal));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 370 >*/
    goto L370;

/* ... INITIALIZE WKSP BASE ADDRESSES. */

/*<    90 IB1 = 1 >*/
L90:
    ib1 = 1;
/*<       IB2 = IB1+N >*/
    ib2 = ib1 + n;
/*<       IB3 = IB2+N >*/
    ib3 = ib2 + n;
/*<       IB4 = IB3+N >*/
    ib4 = ib3 + n;
/*<       IB5 = IB4+N >*/
    ib5 = ib4 + n;
/*<       IPARM(8) = 4*N+2*ITMAX >*/
    iparm[8] = (n << 2) + (itcom1_1.itmax << 1);
/*<       IF (ISYM.NE.0) IPARM(8) = IPARM(8)+2*ITMAX >*/
    if (itcom1_1.isym != 0) {
	iparm[8] += itcom1_1.itmax << 1;
    }
/*<       IF (NW.GE.IPARM(8)) GO TO 110 >*/
    if (*nw >= iparm[8]) {
	goto L110;
    }
/*<       IER = 12 >*/
    ier = 12;
/*<       IF (LEVEL.GE.0) WRITE (NOUT,100) NW,IPARM(8) >*/
    if (itcom1_1.level >= 0) {
	io___20.ciunit = itcom1_1.nout;
	s_wsfe(&io___20);
	do_fio(&c__1, (char *)&(*nw), (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&iparm[8], (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 370 >*/
    goto L370;

/* ... PERMUTE TO  RED-BLACK SYSTEM IF REQUESTED */

/*<   110 NB = IPARM(9) >*/
L110:
    nb = iparm[9];
/*<       IF (NB.LT.0) GO TO 170 >*/
    if (nb < 0) {
	goto L170;
    }
/*<       N3 = 3*N >*/
    n3 = n * 3;
/*<       CALL IVFILL (N3,IWKSP,0) >*/
    ivfill_(&n3, &iwksp[1], &c__0);
/*<       CALL PRBNDX (N,NB,IA,JA,IWKSP,IWKSP(IB2),LEVEL,NOUT,IER) >*/
    prbndx_(&n, &nb, &ia[1], &ja[1], &iwksp[1], &iwksp[ib2], &itcom1_1.level, 
	    &itcom1_1.nout, &ier);
/*<       IF (IER.EQ.0) GO TO 130 >*/
    if (ier == 0) {
	goto L130;
    }
/*<       IF (LEVEL.GE.0) WRITE (NOUT,120) IER,NB >*/
    if (itcom1_1.level >= 0) {
	io___23.ciunit = itcom1_1.nout;
	s_wsfe(&io___23);
	do_fio(&c__1, (char *)&ier, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&nb, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 370 >*/
    goto L370;

/* ... PERMUTE MATRIX AND RHS */

/*<   130 IF (LEVEL.GE.2) WRITE (NOUT,140) NB >*/
L130:
    if (itcom1_1.level >= 2) {
	io___24.ciunit = itcom1_1.nout;
	s_wsfe(&io___24);
	do_fio(&c__1, (char *)&nb, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<   140 FORMAT (/10X,'ORDER OF BLACK SUBSYSTEM = ',I5,' (NB)') >*/
/*<       CALL PERMAT (N,IA,JA,A,IWKSP,IWKSP(IB3),ISYM,LEVEL,NOUT,IER) >*/
    permat_(&n, &ia[1], &ja[1], &a[1], &iwksp[1], &iwksp[ib3], &itcom1_1.isym,
	     &itcom1_1.level, &itcom1_1.nout, &ier);
/*<       IF (IER.EQ.0) GO TO 160 >*/
    if (ier == 0) {
	goto L160;
    }
/*<       IF (LEVEL.GE.0) WRITE (NOUT,150) IER >*/
    if (itcom1_1.level >= 0) {
	io___25.ciunit = itcom1_1.nout;
	s_wsfe(&io___25);
	do_fio(&c__1, (char *)&ier, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 370 >*/
    goto L370;
/*<   160 CALL PERVEC (N,RHS,IWKSP) >*/
L160:
    pervec_(&n, &rhs[1], &iwksp[1]);
/*<       CALL PERVEC (N,U,IWKSP) >*/
    pervec_(&n, &u[1], &iwksp[1]);

/* ... SCALE LINEAR SYSTEM, U, AND RHS BY THE SQUARE ROOT OF THE */
/* ... DIAGONAL ELEMENTS. */

/*<   170 CONTINUE >*/
L170:
/*<       CALL VFILL (IPARM(8),WKSP,0.0D0) >*/
    vfill_(&iparm[8], &wksp[1], &c_b21);
/*<       CALL SCAL (N,IA,JA,A,RHS,U,WKSP,LEVEL,NOUT,IER) >*/
    scal_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &u[1], &wksp[1], &
	    itcom1_1.level, &itcom1_1.nout, &ier);
/*<       IF (IER.EQ.0) GO TO 190 >*/
    if (ier == 0) {
	goto L190;
    }
/*<       IF (LEVEL.GE.0) WRITE (NOUT,180) IER >*/
    if (itcom1_1.level >= 0) {
	io___26.ciunit = itcom1_1.nout;
	s_wsfe(&io___26);
	do_fio(&c__1, (char *)&ier, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 370 >*/
    goto L370;
/*<   190 IF (LEVEL.LE.2) GO TO 220 >*/
L190:
    if (itcom1_1.level <= 2) {
	goto L220;
    }
/*<       WRITE (NOUT,200) >*/
    io___27.ciunit = itcom1_1.nout;
    s_wsfe(&io___27);
    e_wsfe();
/*<    >*/
/*<       IF (ADAPT) WRITE (NOUT,210) >*/
    if (itcom2_1.adapt) {
	io___28.ciunit = itcom1_1.nout;
	s_wsfe(&io___28);
	e_wsfe();
    }
/*<    >*/
/*<   220 IF (IPARM(11).NE.0) GO TO 230 >*/
L220:
    if (iparm[11] != 0) {
	goto L230;
    }
/*<       TIMI1 = TIMER(DUMMY) >*/
    timi1 = timer_(&dummy);

/* ... COMPUTE INITIAL PSEUDO-RESIDUAL */

/*<   230 CONTINUE >*/
L230:
/*<       CALL DCOPY (N,RHS,1,WKSP(IB2),1) >*/
    dcopy_(&n, &rhs[1], &c__1, &wksp[ib2], &c__1);
/*<       CALL PJAC (N,IA,JA,A,U,WKSP(IB2)) >*/
    pjac_(&n, &ia[1], &ja[1], &a[1], &u[1], &wksp[ib2]);
/*<       CALL VEVMW (N,WKSP(IB2),U) >*/
    vevmw_(&n, &wksp[ib2], &u[1]);

/* ... ITERATION SEQUENCE */

/*<       ITMAX1 = ITMAX+1 >*/
    itmax1 = itcom1_1.itmax + 1;
/*<       DO 250 LOOP = 1,ITMAX1 >*/
    i__1 = itmax1;
    for (loop = 1; loop <= i__1; ++loop) {
/*<          IN = LOOP-1 >*/
	itcom1_1.in = loop - 1;
/*<          IF (MOD(IN,2).EQ.1) GO TO 240 >*/
	if (itcom1_1.in % 2 == 1) {
	    goto L240;
	}

/* ... CODE FOR THE EVEN ITERATIONS. */

/*     U           = U(IN)             WKSP(IB2) = DEL(IN) */
/*     WKSP(IB1)   = U(IN-1)           WKSP(IB3) = DEL(IN-1) */

/*<    >*/
	itjcg_(&n, &ia[1], &ja[1], &a[1], &u[1], &wksp[ib1], &wksp[ib2], &
		wksp[ib3], &wksp[ib4], &wksp[ib5]);

/*<          IF (HALT) GO TO 280 >*/
	if (itcom2_1.halt) {
	    goto L280;
	}
/*<          GO TO 250 >*/
	goto L250;

/* ... CODE FOR THE ODD ITERATIONS. */

/*     U           = U(IN-1)           WKSP(IB2) = DEL(IN-1) */
/*     WKSP(IB1)   = U(IN)             WKSP(IB3) = DEL(IN) */

/*<    >*/
L240:
	itjcg_(&n, &ia[1], &ja[1], &a[1], &wksp[ib1], &u[1], &wksp[ib3], &
		wksp[ib2], &wksp[ib4], &wksp[ib5]);

/*<          IF (HALT) GO TO 280 >*/
	if (itcom2_1.halt) {
	    goto L280;
	}
/*<   250 CONTINUE >*/
L250:
	;
    }

/* ... ITMAX HAS BEEN REACHED */

/*<       IF (IPARM(11).NE.0) GO TO 260 >*/
    if (iparm[11] != 0) {
	goto L260;
    }
/*<       TIMI2 = TIMER(DUMMY) >*/
    timi2 = timer_(&dummy);
/*<       TIME1 = DBLE(TIMI2-TIMI1) >*/
    time1 = (doublereal) (timi2 - timi1);
/*<   260 IER = 13 >*/
L260:
    ier = 13;
/*<       IF (LEVEL.GE.1) WRITE (NOUT,270) ITMAX >*/
    if (itcom1_1.level >= 1) {
	io___33.ciunit = itcom1_1.nout;
	s_wsfe(&io___33);
	do_fio(&c__1, (char *)&itcom1_1.itmax, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       IF (IPARM(3).EQ.0) RPARM(1) = STPTST >*/
    if (iparm[3] == 0) {
	rparm[1] = itcom3_1.stptst;
    }
/*<       GO TO 310 >*/
    goto L310;

/* ... METHOD HAS CONVERGED */

/*<   280 IF (IPARM(11).NE.0) GO TO 290 >*/
L280:
    if (iparm[11] != 0) {
	goto L290;
    }
/*<       TIMI2 = TIMER(DUMMY) >*/
    timi2 = timer_(&dummy);
/*<       TIME1 = DBLE(TIMI2-TIMI1) >*/
    time1 = (doublereal) (timi2 - timi1);
/*<   290 IF (LEVEL.GE.1) WRITE (NOUT,300) IN >*/
L290:
    if (itcom1_1.level >= 1) {
	io___34.ciunit = itcom1_1.nout;
	s_wsfe(&io___34);
	do_fio(&c__1, (char *)&itcom1_1.in, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<   300 FORMAT (/1X,'JCG  HAS CONVERGED IN ',I5,' ITERATIONS') >*/

/* ... PUT SOLUTION INTO U IF NOT ALREADY THERE. */

/*<   310 CONTINUE >*/
L310:
/*<       IF (MOD(IN,2).EQ.1) CALL DCOPY (N,WKSP(IB1),1,U,1) >*/
    if (itcom1_1.in % 2 == 1) {
	dcopy_(&n, &wksp[ib1], &c__1, &u[1], &c__1);
    }

/* ... UNSCALE THE MATRIX, SOLUTION, AND RHS VECTORS. */

/*<       CALL UNSCAL (N,IA,JA,A,RHS,U,WKSP) >*/
    unscal_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &u[1], &wksp[1]);

/* ... UN-PERMUTE MATRIX,RHS, AND SOLUTION */

/*<       IF (IPARM(9).LT.0) GO TO 340 >*/
    if (iparm[9] < 0) {
	goto L340;
    }
/*<    >*/
    permat_(&n, &ia[1], &ja[1], &a[1], &iwksp[ib2], &iwksp[ib3], &
	    itcom1_1.isym, &itcom1_1.level, &itcom1_1.nout, &ierper);
/*<       IF (IERPER.EQ.0) GO TO 330 >*/
    if (ierper == 0) {
	goto L330;
    }
/*<       IF (LEVEL.GE.0) WRITE (NOUT,320) IERPER >*/
    if (itcom1_1.level >= 0) {
	io___36.ciunit = itcom1_1.nout;
	s_wsfe(&io___36);
	do_fio(&c__1, (char *)&ierper, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       IF (IER.EQ.0) IER = IERPER >*/
    if (ier == 0) {
	ier = ierper;
    }
/*<       GO TO 370 >*/
    goto L370;
/*<   330 CALL PERVEC (N,RHS,IWKSP(IB2)) >*/
L330:
    pervec_(&n, &rhs[1], &iwksp[ib2]);
/*<       CALL PERVEC (N,U,IWKSP(IB2)) >*/
    pervec_(&n, &u[1], &iwksp[ib2]);

/* ... OPTIONAL ERROR ANALYSIS */

/*<   340 IDGTS = IPARM(12) >*/
L340:
    idgts = iparm[12];
/*<       IF (IDGTS.LT.0) GO TO 350 >*/
    if (idgts < 0) {
	goto L350;
    }
/*<       IF (IPARM(2).LE.0) IDGTS = 0 >*/
    if (iparm[2] <= 0) {
	idgts = 0;
    }
/*<       CALL PERROR (N,IA,JA,A,RHS,U,WKSP,DIGIT1,DIGIT2,IDGTS) >*/
    perror_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &u[1], &wksp[1], &digit1, &
	    digit2, &idgts);

/* ... SET RETURN PARAMETERS IN IPARM AND RPARM */

/*<   350 IPARM(8) = IPARM(8)-2*(ITMAX-IN) >*/
L350:
    iparm[8] -= itcom1_1.itmax - itcom1_1.in << 1;
/*<       IF (IPARM(11).NE.0) GO TO 360 >*/
    if (iparm[11] != 0) {
	goto L360;
    }
/*<       TIMJ2 = TIMER(DUMMY) >*/
    timj2 = timer_(&dummy);
/*<       TIME2 = DBLE(TIMJ2-TIMJ1) >*/
    time2 = (doublereal) (timj2 - timj1);
/*<   360 IF (ISYM.NE.0) IPARM(8) = IPARM(8)-2*(ITMAX-IN) >*/
L360:
    if (itcom1_1.isym != 0) {
	iparm[8] -= itcom1_1.itmax - itcom1_1.in << 1;
    }
/*<       IF (IPARM(3).NE.0) GO TO 370 >*/
    if (iparm[3] != 0) {
	goto L370;
    }
/*<       IPARM(1) = IN >*/
    iparm[1] = itcom1_1.in;
/*<       IPARM(9) = NB >*/
    iparm[9] = nb;
/*<       RPARM(2) = CME >*/
    rparm[2] = itcom3_1.cme;
/*<       RPARM(3) = SME >*/
    rparm[3] = itcom3_1.sme;
/*<       RPARM(9) = TIME1 >*/
    rparm[9] = time1;
/*<       RPARM(10) = TIME2 >*/
    rparm[10] = time2;
/*<       RPARM(11) = DIGIT1 >*/
    rparm[11] = digit1;
/*<       RPARM(12) = DIGIT2 >*/
    rparm[12] = digit2;

/*<   370 CONTINUE >*/
L370:
/*<       IERR = IER >*/
    *ierr = ier;
/*<       IF (LEVEL.GE.3) CALL ECHALL (N,IA,JA,A,RHS,IPARM,RPARM,2) >*/
    if (itcom1_1.level >= 3) {
	echall_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &iparm[1], &rparm[1], &
		c__2);
    }

/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* jcg_ */

/*<       SUBROUTINE JSI (NN,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IERR) >*/
/* Subroutine */ int jsi_(integer *nn, integer *ia, integer *ja, doublereal *
	a, doublereal *rhs, doublereal *u, integer *iwksp, integer *nw, 
	doublereal *wksp, integer *iparm, doublereal *rparm, integer *ierr)
{
    /* Format strings */
    static char fmt_10[] = "(\0020\002///1x,\002BEGINNING OF ITPACK SOLUTION\
 MODULE  JSI\002)";
    static char fmt_40[] = "(\0020\002,\002*** W A R N I N G ************\
\002/\0020\002,\002    IN ITPACK ROUTINE JSI\002/\002 \002,\002    RPARM(1) =\
\002,d10.3,\002 (ZETA)\002/\002 \002,\002    A VALUE THIS SMALL MAY HINDER C\
ONVERGENCE \002/\002 \002,\002    SINCE MACHINE PRECISION DRELPR =\002,d10.3/\
\002 \002,\002    ZETA RESET TO \002,d10.3)";
    static char fmt_60[] = "(\0020\002,\002*** F A T A L     E R R O R *****\
*******\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE JSI \002/\002 \002\
,\002    INVALID MATRIX DIMENSION, N =\002,i8)";
    static char fmt_80[] = "(\0020\002,\002*** F A T A L     E R R O R *****\
*******\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE JSI \002/\002 \002\
,\002    ERROR DETECTED IN SUBROUTINE  SBELM \002/\002 \002,\002    WHICH RE\
MOVES ROWS AND COLUMNS OF SYSTEM \002/\002 \002,\002    WHEN DIAGONAL ENTRY \
TOO LARGE  \002/\002 \002,\002    IER = \002,i5,5x,\002 RPARM(8) = \002,d10.\
3,\002 (TOL)\002)";
    static char fmt_100[] = "(\0020\002,\002*** F A T A L     E R R O R ****\
********\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE JSI \002/\002 \002,\
\002    NOT ENOUGH WORKSPACE AT \002,i10/\002 \002,\002    SET IPARM(8) =\
\002,i10,\002 (NW)\002)";
    static char fmt_120[] = "(\0020\002,\002*** F A T A L     E R R O R ****\
********\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE JSI \002/\002 \002,\
\002    ERROR DETECTED IN SUBROUTINE  PRBNDX\002/\002 \002,\002    WHICH COM\
PUTES THE RED-BLACK INDEXING\002/\002 \002,\002    IER = \002,i5,\002 IPARM(\
9) = \002,i5,\002 (NB)\002)";
    static char fmt_140[] = "(/10x,\002ORDER OF BLACK SUBSYSTEM = \002,i5\
,\002 (NB)\002)";
    static char fmt_150[] = "(\0020\002,\002*** F A T A L     E R R O R ****\
********\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE JSI \002/\002 \002,\
\002    ERROR DETECTED IN SUBROUTINE  PERMAT\002/\002 \002,\002    WHICH DOE\
S THE RED-BLACK PERMUTATION\002/\002 \002,\002    IER = \002,i5)";
    static char fmt_180[] = "(\0020\002,\002*** F A T A L     E R R O R ****\
********\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE JSI \002/\002 \002,\
\002    ERROR DETECTED IN SUBROUTINE  SCAL  \002/\002 \002,\002    WHICH SCA\
LES THE SYSTEM   \002/\002 \002,\002    IER = \002,i5)";
    static char fmt_200[] = "(///1x,\002IN THE FOLLOWING, RHO AND GAMMA AR\
E\002,\002 ACCELERATION PARAMETERS\002)";
    static char fmt_260[] = "(\0020\002,\002*** W A R N I N G ***********\
*\002/\0020\002,\002    IN ITPACK ROUTINE JSI\002/\002 \002,\002    FAILURE \
TO CONVERGE IN\002,i5,\002 ITERATIONS\002)";
    static char fmt_290[] = "(/1x,\002JSI  HAS CONVERGED IN \002,i5,\002 ITE\
RATIONS\002)";
    static char fmt_310[] = "(\0020\002,\002*** F A T A L     E R R O R ****\
********\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE JSI \002/\002 \002,\
\002    ERROR DETECTED IN SUBROUTINE  PERMAT\002/\002 \002,\002    WHICH UND\
OES THE RED-BLACK PERMUTATION   \002/\002 \002,\002    IER = \002,i5)";

    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), e_wsfe(), do_fio(integer *, char *, ftnlen);

    /* Local variables */
    integer n, n3, nb, ib1, ib2, ib3, ier;
    doublereal tol;
    extern /* Subroutine */ int scal_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *,
	     integer *, integer *);
    integer icnt;
    doublereal temp;
    integer loop;
    doublereal time1, time2;
    real timi1, timj1, timi2, timj2;
    extern /* Subroutine */ int sbelm_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, integer *, doublereal *, doublereal *,
	     integer *, integer *, integer *, integer *);
    integer idgts;
    extern /* Subroutine */ int vfill_(integer *, doublereal *, doublereal *),
	     dcopy_(integer *, doublereal *, integer *, doublereal *, integer 
	    *);
    extern doublereal timer_(real *);
    extern /* Subroutine */ int itjsi_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, integer *);
    real dummy;
    doublereal digit1, digit2;
    integer itmax1;
    extern /* Subroutine */ int echall_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, integer *, doublereal *, integer *), 
	    pervec_(integer *, doublereal *, integer *), ivfill_(integer *, 
	    integer *, integer *);
    integer ierper;
    extern /* Subroutine */ int echout_(integer *, doublereal *, integer *), 
	    permat_(integer *, integer *, integer *, doublereal *, integer *, 
	    integer *, integer *, integer *, integer *, integer *), unscal_(
	    integer *, integer *, integer *, doublereal *, doublereal *, 
	    doublereal *, doublereal *), prbndx_(integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *), perror_(integer *, integer *, integer *, doublereal *,
	     doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, integer *);

    /* Fortran I/O blocks */
    static cilist io___39 = { 0, 0, 0, fmt_10, 0 };
    static cilist io___45 = { 0, 0, 0, fmt_40, 0 };
    static cilist io___50 = { 0, 0, 0, fmt_60, 0 };
    static cilist io___52 = { 0, 0, 0, fmt_80, 0 };
    static cilist io___56 = { 0, 0, 0, fmt_100, 0 };
    static cilist io___59 = { 0, 0, 0, fmt_120, 0 };
    static cilist io___60 = { 0, 0, 0, fmt_140, 0 };
    static cilist io___61 = { 0, 0, 0, fmt_150, 0 };
    static cilist io___62 = { 0, 0, 0, fmt_180, 0 };
    static cilist io___63 = { 0, 0, 0, fmt_200, 0 };
    static cilist io___69 = { 0, 0, 0, fmt_260, 0 };
    static cilist io___70 = { 0, 0, 0, fmt_290, 0 };
    static cilist io___72 = { 0, 0, 0, fmt_310, 0 };



/*     ITPACK 2C MAIN SUBROUTINE  JSI  (JACOBI SEMI-ITERATIVE) */
/*     EACH OF THE MAIN SUBROUTINES: */
/*           JCG, JSI, SOR, SSORCG, SSORSI, RSCG, RSSI */
/*     CAN BE USED INDEPENDENTLY OF THE OTHERS */

/* ... FUNCTION: */

/*          THIS SUBROUTINE, JSI, DRIVES THE JACOBI SEMI- */
/*          ITERATION ALGORITHM. */

/* ... PARAMETER LIST: */

/*          N      INPUT INTEGER.  DIMENSION OF THE MATRIX. (= NN) */
/*          IA,JA  INPUT INTEGER VECTORS.  THE TWO INTEGER ARRAYS OF */
/*                 THE SPARSE MATRIX REPRESENTATION. */
/*          A      INPUT D.P. VECTOR.  THE D.P. ARRAY OF THE SPARSE */
/*                 MATRIX REPRESENTATION. */
/*          RHS    INPUT D.P. VECTOR.  CONTAINS THE RIGHT HAND SIDE */
/*                 OF THE MATRIX PROBLEM. */
/*          U      INPUT/OUTPUT D.P. VECTOR.  ON INPUT, U CONTAINS THE */
/*                 INITIAL GUESS TO THE SOLUTION. ON OUTPUT, IT CONTAINS */
/*                 THE LATEST ESTIMATE TO THE SOLUTION. */
/*          IWKSP  INTEGER VECTOR WORKSPACE OF LENGTH 3*N */
/*          NW     INPUT INTEGER.  LENGTH OF AVAILABLE WKSP.  ON OUTPUT, */
/*                 IPARM(8) IS AMOUNT USED. */
/*          WKSP   D.P. VECTOR USED FOR WORKING SPACE.  JACOBI SI */
/*                 NEEDS THIS TO BE IN LENGTH AT LEAST */
/*                 2*N */
/*          IPARM  INTEGER VECTOR OF LENGTH 12.  ALLOWS USER TO SPECIFY */
/*                 SOME INTEGER PARAMETERS WHICH AFFECT THE METHOD. */
/*          RPARM  D.P. VECTOR OF LENGTH 12.  ALLOWS USER TO SPECIFY SOME */
/*                 D.P. PARAMETERS WHICH AFFECT THE METHOD. */
/*          IER    OUTPUT INTEGER.  ERROR FLAG. (= IERR) */

/* ... JSI SUBPROGRAM REFERENCES: */

/*          FROM ITPACK   BISRCH, CHEBY, CHGSI, CHGSME, DFAULT, ECHALL, */
/*                        ECHOUT, ITERM, TIMER, ITJSI, IVFILL, PAR */
/*                        PERMAT, PERROR, PERVEC, PJAC, PMULT, PRBNDX, */
/*                        PSTOP, PVTBV, QSORT, DAXPY, SBELM, SCAL, */
/*                        DCOPY, DDOT, SUM3, TSTCHG, UNSCAL, VEVMW, */
/*                        VFILL, VOUT, WEVMW */
/*          SYSTEM        DABS, DLOG10, DBLE(AMAX0), DMAX1, DBLE(FLOAT), */
/*                        MOD,DSQRT */

/*     VERSION:  ITPACK 2C (MARCH 1982) */

/*     CODE WRITTEN BY:  DAVID KINCAID, ROGER GRIMES, JOHN RESPESS */
/*                       CENTER FOR NUMERICAL ANALYSIS */
/*                       UNIVERSITY OF TEXAS */
/*                       AUSTIN, TX  78712 */
/*                       (512) 471-1242 */

/*     FOR ADDITIONAL DETAILS ON THE */
/*          (A) SUBROUTINE SEE TOMS ARTICLE 1982 */
/*          (B) ALGORITHM  SEE CNA REPORT 150 */

/*     BASED ON THEORY BY:  DAVID YOUNG, DAVID KINCAID, LOU HAGEMAN */

/*     REFERENCE THE BOOK:  APPLIED ITERATIVE METHODS */
/*                          L. HAGEMAN, D. YOUNG */
/*                          ACADEMIC PRESS, 1981 */

/*     ************************************************** */
/*     *               IMPORTANT NOTE                   * */
/*     *                                                * */
/*     *      WHEN INSTALLING ITPACK ROUTINES ON A      * */
/*     *  DIFFERENT COMPUTER, RESET SOME OF THE VALUES  * */
/*     *  IN  SUBROUTNE DFAULT.   MOST IMPORTANT ARE    * */
/*     *                                                * */
/*     *   DRELPR      MACHINE RELATIVE PRECISION       * */
/*     *   RPARM(1)    STOPPING CRITERION               * */
/*     *                                                * */
/*     *   ALSO CHANGE SYSTEM-DEPENDENT ROUTINE         * */
/*     *   SECOND USED IN TIMER                         * */
/*     *                                                * */
/*     ************************************************** */

/*     SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER IA(1),JA(1),IWKSP(1),IPARM(12),NN,NW,IERR >*/
/*<       DOUBLE PRECISION A(1),RHS(NN),U(NN),WKSP(NW),RPARM(12) >*/

/*     SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER IB1,IB2,IB3,ICNT,IDGTS,IER,IERPER,ITMAX1,LOOP,N,NB,N3 >*/
/*<       DOUBLE PRECISION DIGIT1,DIGIT2,TEMP,TIME1,TIME2,TOL >*/

/* *** BEGIN: ITPACK COMMON */

/*<       INTEGER IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/
/*<       COMMON /ITCOM1/ IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/

/*<       LOGICAL ADAPT,BETADT,CASEII,HALT,PARTAD >*/
/*<       COMMON /ITCOM2/ ADAPT,BETADT,CASEII,HALT,PARTAD >*/

/*<    >*/
/*<    >*/

/* *** END  : ITPACK COMMON */

/* ... VARIABLES IN COMMON BLOCK - ITCOM1 */

/*     IN     - ITERATION NUMBER */
/*     IS     - ITERATION NUMBER WHEN PARAMETERS LAST CHANGED */
/*     ISYM   - SYMMETRIC/NONSYMMETRIC STORAGE FORMAT SWITCH */
/*     ITMAX  - MAXIMUM NUMBER OF ITERATIONS ALLOWED */
/*     LEVEL  - LEVEL OF OUTPUT CONTROL SWITCH */
/*     NOUT   - OUTPUT UNIT NUMBER */

/* ... VARIABLES IN COMMON BLOCK - ITCOM2 */

/*     ADAPT  - FULLY ADAPTIVE PROCEDURE SWITCH */
/*     BETADT - SWITCH FOR ADAPTIVE DETERMINATION OF BETA */
/*     CASEII - ADAPTIVE PROCEDURE CASE SWITCH */
/*     HALT   - STOPPING TEST SWITCH */
/*     PARTAD - PARTIALLY ADAPTIVE PROCEDURE SWITCH */

/* ... VARIABLES IN COMMON BLOCK - ITCOM3 */

/*     BDELNM - TWO NORM OF B TIMES DELTA-SUPER-N */
/*     BETAB  - ESTIMATE FOR THE SPECTRAL RADIUS OF LU MATRIX */
/*     CME    - ESTIMATE OF LARGEST EIGENVALUE */
/*     DELNNM - INNER PRODUCT OF PSEUDO-RESIDUAL AT ITERATION N */
/*     DELSNM - INNER PRODUCT OF PSEUDO-RESIDUAL AT ITERATION S */
/*     FF     - ADAPTIVE PROCEDURE DAMPING FACTOR */
/*     GAMMA  - ACCELERATION PARAMETER */
/*     OMEGA  - OVERRELAXATION PARAMETER FOR SOR AND SSOR */
/*     QA     - PSEUDO-RESIDUAL RATIO */
/*     QT     - VIRTUAL SPECTRAL RADIUS */
/*     RHO    - ACCELERATION PARAMETER */
/*     RRR    - ADAPTIVE PARAMETER */
/*     SIGE   - PARAMETER SIGMA-SUB-E */
/*     SME    - ESTIMATE OF SMALLEST EIGENVALUE */
/*     SPECR  - SPECTRAL RADIUS ESTIMATE FOR SSOR */
/*     DRELPR - MACHINE RELATIVE PRECISION */
/*     STPTST - STOPPING PARAMETER */
/*     UDNM   - TWO NORM OF U */
/*     ZETA   - STOPPING CRITERION */

/* ... INITIALIZE COMMON BLOCKS */

/*<       LEVEL = IPARM(2) >*/
    /* Parameter adjustments */
    --u;
    --rhs;
    --ia;
    --ja;
    --a;
    --iwksp;
    --wksp;
    --iparm;
    --rparm;

    /* Function Body */
    itcom1_1.level = iparm[2];
/*<       NOUT = IPARM(4) >*/
    itcom1_1.nout = iparm[4];
/*<       IF (LEVEL.GE.1) WRITE (NOUT,10) >*/
    if (itcom1_1.level >= 1) {
	io___39.ciunit = itcom1_1.nout;
	s_wsfe(&io___39);
	e_wsfe();
    }
/*<    10 FORMAT ('0'///1X,'BEGINNING OF ITPACK SOLUTION MODULE  JSI') >*/
/*<       IER = 0 >*/
    ier = 0;
/*<       IF (IPARM(1).LE.0) RETURN >*/
    if (iparm[1] <= 0) {
	return 0;
    }
/*<       N = NN >*/
    n = *nn;
/*<       IF (IPARM(11).EQ.0) TIMJ1 = TIMER(DUMMY) >*/
    if (iparm[11] == 0) {
	timj1 = timer_(&dummy);
    }
/*<       IF (LEVEL.GE.3) GO TO 20 >*/
    if (itcom1_1.level >= 3) {
	goto L20;
    }
/*<       CALL ECHOUT (IPARM,RPARM,2) >*/
    echout_(&iparm[1], &rparm[1], &c__2);
/*<       GO TO 30 >*/
    goto L30;
/*<    20 CALL ECHALL (N,IA,JA,A,RHS,IPARM,RPARM,1) >*/
L20:
    echall_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &iparm[1], &rparm[1], &c__1);
/*<    30 TEMP = 5.0D2*DRELPR >*/
L30:
    temp = itcom3_1.drelpr * 500.;
/*<       IF (ZETA.GE.TEMP) GO TO 50 >*/
    if (itcom3_1.zeta >= temp) {
	goto L50;
    }
/*<       IF (LEVEL.GE.1) WRITE (NOUT,40) ZETA,DRELPR,TEMP >*/
    if (itcom1_1.level >= 1) {
	io___45.ciunit = itcom1_1.nout;
	s_wsfe(&io___45);
	do_fio(&c__1, (char *)&itcom3_1.zeta, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&itcom3_1.drelpr, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&temp, (ftnlen)sizeof(doublereal));
	e_wsfe();
    }
/*<    >*/
/*<       ZETA = TEMP >*/
    itcom3_1.zeta = temp;
/*<    50 CONTINUE >*/
L50:
/*<       TIME1 = RPARM(9) >*/
    time1 = rparm[9];
/*<       TIME2 = RPARM(10) >*/
    time2 = rparm[10];
/*<       DIGIT1 = RPARM(11) >*/
    digit1 = rparm[11];
/*<       DIGIT2 = RPARM(12) >*/
    digit2 = rparm[12];

/* ... VERIFY N */

/*<       IF (N.GT.0) GO TO 70 >*/
    if (n > 0) {
	goto L70;
    }
/*<       IER = 21 >*/
    ier = 21;
/*<       IF (LEVEL.GE.0) WRITE (NOUT,60) N >*/
    if (itcom1_1.level >= 0) {
	io___50.ciunit = itcom1_1.nout;
	s_wsfe(&io___50);
	do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 360 >*/
    goto L360;
/*<    70 CONTINUE >*/
L70:

/* ... REMOVE ROWS AND COLUMNS IF REQUESTED */

/*<       IF (IPARM(10).EQ.0) GO TO 90 >*/
    if (iparm[10] == 0) {
	goto L90;
    }
/*<       TOL = RPARM(8) >*/
    tol = rparm[8];
/*<       CALL IVFILL (N,IWKSP,0) >*/
    ivfill_(&n, &iwksp[1], &c__0);
/*<       CALL VFILL (N,WKSP,0.0D0) >*/
    vfill_(&n, &wksp[1], &c_b21);
/*<       CALL SBELM (N,IA,JA,A,RHS,IWKSP,WKSP,TOL,ISYM,LEVEL,NOUT,IER) >*/
    sbelm_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &iwksp[1], &wksp[1], &tol, &
	    itcom1_1.isym, &itcom1_1.level, &itcom1_1.nout, &ier);
/*<       IF (IER.EQ.0) GO TO 90 >*/
    if (ier == 0) {
	goto L90;
    }
/*<       IF (LEVEL.GE.0) WRITE (NOUT,80) IER,TOL >*/
    if (itcom1_1.level >= 0) {
	io___52.ciunit = itcom1_1.nout;
	s_wsfe(&io___52);
	do_fio(&c__1, (char *)&ier, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&tol, (ftnlen)sizeof(doublereal));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 360 >*/
    goto L360;

/* ... INITIALIZE WKSP BASE ADDRESSES. */

/*<    90 IB1 = 1 >*/
L90:
    ib1 = 1;
/*<       IB2 = IB1+N >*/
    ib2 = ib1 + n;
/*<       IB3 = IB2+N >*/
    ib3 = ib2 + n;
/*<       IPARM(8) = 2*N >*/
    iparm[8] = n << 1;
/*<       IF (NW.GE.IPARM(8)) GO TO 110 >*/
    if (*nw >= iparm[8]) {
	goto L110;
    }
/*<       IER = 22 >*/
    ier = 22;
/*<       IF (LEVEL.GE.0) WRITE (NOUT,100) NW,IPARM(8) >*/
    if (itcom1_1.level >= 0) {
	io___56.ciunit = itcom1_1.nout;
	s_wsfe(&io___56);
	do_fio(&c__1, (char *)&(*nw), (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&iparm[8], (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 360 >*/
    goto L360;

/* ... PERMUTE TO  RED-BLACK SYSTEM IF REQUESTED */

/*<   110 NB = IPARM(9) >*/
L110:
    nb = iparm[9];
/*<       IF (NB.LT.0) GO TO 170 >*/
    if (nb < 0) {
	goto L170;
    }
/*<       N3 = 3*N >*/
    n3 = n * 3;
/*<       CALL IVFILL (N3,IWKSP,0) >*/
    ivfill_(&n3, &iwksp[1], &c__0);
/*<       CALL PRBNDX (N,NB,IA,JA,IWKSP,IWKSP(IB2),LEVEL,NOUT,IER) >*/
    prbndx_(&n, &nb, &ia[1], &ja[1], &iwksp[1], &iwksp[ib2], &itcom1_1.level, 
	    &itcom1_1.nout, &ier);
/*<       IF (IER.EQ.0) GO TO 130 >*/
    if (ier == 0) {
	goto L130;
    }
/*<       IF (LEVEL.GE.0) WRITE (NOUT,120) IER,NB >*/
    if (itcom1_1.level >= 0) {
	io___59.ciunit = itcom1_1.nout;
	s_wsfe(&io___59);
	do_fio(&c__1, (char *)&ier, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&nb, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 360 >*/
    goto L360;

/* ... PERMUTE MATRIX AND RHS */

/*<   130 IF (LEVEL.GE.2) WRITE (NOUT,140) NB >*/
L130:
    if (itcom1_1.level >= 2) {
	io___60.ciunit = itcom1_1.nout;
	s_wsfe(&io___60);
	do_fio(&c__1, (char *)&nb, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<   140 FORMAT (/10X,'ORDER OF BLACK SUBSYSTEM = ',I5,' (NB)') >*/
/*<       CALL PERMAT (N,IA,JA,A,IWKSP,IWKSP(IB3),ISYM,LEVEL,NOUT,IER) >*/
    permat_(&n, &ia[1], &ja[1], &a[1], &iwksp[1], &iwksp[ib3], &itcom1_1.isym,
	     &itcom1_1.level, &itcom1_1.nout, &ier);
/*<       IF (IER.EQ.0) GO TO 160 >*/
    if (ier == 0) {
	goto L160;
    }
/*<       IF (LEVEL.GE.0) WRITE (NOUT,150) IER >*/
    if (itcom1_1.level >= 0) {
	io___61.ciunit = itcom1_1.nout;
	s_wsfe(&io___61);
	do_fio(&c__1, (char *)&ier, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 360 >*/
    goto L360;
/*<   160 CALL PERVEC (N,RHS,IWKSP) >*/
L160:
    pervec_(&n, &rhs[1], &iwksp[1]);
/*<       CALL PERVEC (N,U,IWKSP) >*/
    pervec_(&n, &u[1], &iwksp[1]);

/* ... SCALE LINEAR SYSTEM, U, AND RHS BY THE SQUARE ROOT OF THE */
/* ... DIAGONAL ELEMENTS. */

/*<   170 CONTINUE >*/
L170:
/*<       CALL VFILL (IPARM(8),WKSP,0.0D0) >*/
    vfill_(&iparm[8], &wksp[1], &c_b21);
/*<       CALL SCAL (N,IA,JA,A,RHS,U,WKSP,LEVEL,NOUT,IER) >*/
    scal_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &u[1], &wksp[1], &
	    itcom1_1.level, &itcom1_1.nout, &ier);
/*<       IF (IER.EQ.0) GO TO 190 >*/
    if (ier == 0) {
	goto L190;
    }
/*<       IF (LEVEL.GE.0) WRITE (NOUT,180) IER >*/
    if (itcom1_1.level >= 0) {
	io___62.ciunit = itcom1_1.nout;
	s_wsfe(&io___62);
	do_fio(&c__1, (char *)&ier, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 360 >*/
    goto L360;
/*<   190 IF (LEVEL.LE.2) GO TO 210 >*/
L190:
    if (itcom1_1.level <= 2) {
	goto L210;
    }
/*<       WRITE (NOUT,200) >*/
    io___63.ciunit = itcom1_1.nout;
    s_wsfe(&io___63);
    e_wsfe();
/*<    >*/
/*<   210 IF (IPARM(11).NE.0) GO TO 220 >*/
L210:
    if (iparm[11] != 0) {
	goto L220;
    }
/*<       TIMI1 = TIMER(DUMMY) >*/
    timi1 = timer_(&dummy);

/* ... ITERATION SEQUENCE */

/*<   220 ITMAX1 = ITMAX+1 >*/
L220:
    itmax1 = itcom1_1.itmax + 1;
/*<       DO 240 LOOP = 1,ITMAX1 >*/
    i__1 = itmax1;
    for (loop = 1; loop <= i__1; ++loop) {
/*<          IN = LOOP-1 >*/
	itcom1_1.in = loop - 1;
/*<          IF (MOD(IN,2).EQ.1) GO TO 230 >*/
	if (itcom1_1.in % 2 == 1) {
	    goto L230;
	}

/* ... CODE FOR THE EVEN ITERATIONS. */

/*     U           = U(IN) */
/*     WKSP(IB1)   = U(IN-1) */

/*<          CALL ITJSI (N,IA,JA,A,RHS,U,WKSP(IB1),WKSP(IB2),ICNT) >*/
	itjsi_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &u[1], &wksp[ib1], &wksp[
		ib2], &icnt);

/*<          IF (HALT) GO TO 270 >*/
	if (itcom2_1.halt) {
	    goto L270;
	}
/*<          GO TO 240 >*/
	goto L240;

/* ... CODE FOR THE ODD ITERATIONS. */

/*     U           = U(IN-1) */
/*     WKSP(IB1)   = U(IN) */

/*<   230    CALL ITJSI (N,IA,JA,A,RHS,WKSP(IB1),U,WKSP(IB2),ICNT) >*/
L230:
	itjsi_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &wksp[ib1], &u[1], &wksp[
		ib2], &icnt);

/*<          IF (HALT) GO TO 270 >*/
	if (itcom2_1.halt) {
	    goto L270;
	}
/*<   240 CONTINUE >*/
L240:
	;
    }

/* ... ITMAX HAS BEEN REACHED */

/*<       IF (IPARM(11).NE.0) GO TO 250 >*/
    if (iparm[11] != 0) {
	goto L250;
    }
/*<       TIMI2 = TIMER(DUMMY) >*/
    timi2 = timer_(&dummy);
/*<       TIME1 = DBLE(TIMI2-TIMI1) >*/
    time1 = (doublereal) (timi2 - timi1);
/*<   250 IER = 23 >*/
L250:
    ier = 23;
/*<       IF (LEVEL.GE.1) WRITE (NOUT,260) ITMAX >*/
    if (itcom1_1.level >= 1) {
	io___69.ciunit = itcom1_1.nout;
	s_wsfe(&io___69);
	do_fio(&c__1, (char *)&itcom1_1.itmax, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       IF (IPARM(3).EQ.0) RPARM(1) = STPTST >*/
    if (iparm[3] == 0) {
	rparm[1] = itcom3_1.stptst;
    }
/*<       GO TO 300 >*/
    goto L300;

/* ... METHOD HAS CONVERGED */

/*<   270 IF (IPARM(11).NE.0) GO TO 280 >*/
L270:
    if (iparm[11] != 0) {
	goto L280;
    }
/*<       TIMI2 = TIMER(DUMMY) >*/
    timi2 = timer_(&dummy);
/*<       TIME1 = DBLE(TIMI2-TIMI1) >*/
    time1 = (doublereal) (timi2 - timi1);
/*<   280 IF (LEVEL.GE.1) WRITE (NOUT,290) IN >*/
L280:
    if (itcom1_1.level >= 1) {
	io___70.ciunit = itcom1_1.nout;
	s_wsfe(&io___70);
	do_fio(&c__1, (char *)&itcom1_1.in, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<   290 FORMAT (/1X,'JSI  HAS CONVERGED IN ',I5,' ITERATIONS') >*/

/* ... PUT SOLUTION INTO U IF NOT ALREADY THERE. */

/*<   300 CONTINUE >*/
L300:
/*<       IF (MOD(IN,2).EQ.1) CALL DCOPY (N,WKSP(IB1),1,U,1) >*/
    if (itcom1_1.in % 2 == 1) {
	dcopy_(&n, &wksp[ib1], &c__1, &u[1], &c__1);
    }

/* ... UNSCALE THE MATRIX, SOLUTION, AND RHS VECTORS. */

/*<       CALL UNSCAL (N,IA,JA,A,RHS,U,WKSP) >*/
    unscal_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &u[1], &wksp[1]);

/* ... UN-PERMUTE MATRIX,RHS, AND SOLUTION */

/*<       IF (IPARM(9).LT.0) GO TO 330 >*/
    if (iparm[9] < 0) {
	goto L330;
    }
/*<    >*/
    permat_(&n, &ia[1], &ja[1], &a[1], &iwksp[ib2], &iwksp[ib3], &
	    itcom1_1.isym, &itcom1_1.level, &itcom1_1.nout, &ierper);
/*<       IF (IERPER.EQ.0) GO TO 320 >*/
    if (ierper == 0) {
	goto L320;
    }
/*<       IF (LEVEL.GE.0) WRITE (NOUT,310) IERPER >*/
    if (itcom1_1.level >= 0) {
	io___72.ciunit = itcom1_1.nout;
	s_wsfe(&io___72);
	do_fio(&c__1, (char *)&ierper, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       IF (IER.EQ.0) IER = IERPER >*/
    if (ier == 0) {
	ier = ierper;
    }
/*<       GO TO 360 >*/
    goto L360;
/*<   320 CALL PERVEC (N,RHS,IWKSP(IB2)) >*/
L320:
    pervec_(&n, &rhs[1], &iwksp[ib2]);
/*<       CALL PERVEC (N,U,IWKSP(IB2)) >*/
    pervec_(&n, &u[1], &iwksp[ib2]);

/* ... OPTIONAL ERROR ANALYSIS */

/*<   330 IDGTS = IPARM(12) >*/
L330:
    idgts = iparm[12];
/*<       IF (IDGTS.LT.0) GO TO 340 >*/
    if (idgts < 0) {
	goto L340;
    }
/*<       IF (IPARM(2).LE.0) IDGTS = 0 >*/
    if (iparm[2] <= 0) {
	idgts = 0;
    }
/*<       CALL PERROR (N,IA,JA,A,RHS,U,WKSP,DIGIT1,DIGIT2,IDGTS) >*/
    perror_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &u[1], &wksp[1], &digit1, &
	    digit2, &idgts);

/* ... SET RETURN PARAMETERS IN IPARM AND RPARM */

/*<   340 IF (IPARM(11).NE.0) GO TO 350 >*/
L340:
    if (iparm[11] != 0) {
	goto L350;
    }
/*<       TIMJ2 = TIMER(DUMMY) >*/
    timj2 = timer_(&dummy);
/*<       TIME2 = DBLE(TIMJ2-TIMJ1) >*/
    time2 = (doublereal) (timj2 - timj1);
/*<   350 IF (IPARM(3).NE.0) GO TO 360 >*/
L350:
    if (iparm[3] != 0) {
	goto L360;
    }
/*<       IPARM(1) = IN >*/
    iparm[1] = itcom1_1.in;
/*<       IPARM(9) = NB >*/
    iparm[9] = nb;
/*<       RPARM(2) = CME >*/
    rparm[2] = itcom3_1.cme;
/*<       RPARM(3) = SME >*/
    rparm[3] = itcom3_1.sme;
/*<       RPARM(9) = TIME1 >*/
    rparm[9] = time1;
/*<       RPARM(10) = TIME2 >*/
    rparm[10] = time2;
/*<       RPARM(11) = DIGIT1 >*/
    rparm[11] = digit1;
/*<       RPARM(12) = DIGIT2 >*/
    rparm[12] = digit2;

/*<   360 CONTINUE >*/
L360:
/*<       IERR = IER >*/
    *ierr = ier;
/*<       IF (LEVEL.GE.3) CALL ECHALL (N,IA,JA,A,RHS,IPARM,RPARM,2) >*/
    if (itcom1_1.level >= 3) {
	echall_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &iparm[1], &rparm[1], &
		c__2);
    }

/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* jsi_ */

/*<       SUBROUTINE SOR (NN,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IERR) >*/
/* Subroutine */ int sor_(integer *nn, integer *ia, integer *ja, doublereal *
	a, doublereal *rhs, doublereal *u, integer *iwksp, integer *nw, 
	doublereal *wksp, integer *iparm, doublereal *rparm, integer *ierr)
{
    /* Format strings */
    static char fmt_10[] = "(\0020\002///1x,\002BEGINNING OF ITPACK SOLUTION\
 MODULE  SOR\002)";
    static char fmt_40[] = "(\0020\002,\002*** W A R N I N G ************\
\002/\0020\002,\002    IN ITPACK ROUTINE SOR\002/\002 \002,\002    RPARM(1) =\
\002,d10.3,\002 (ZETA)\002/\002 \002,\002    A VALUE THIS SMALL MAY HINDER C\
ONVERGENCE \002/\002 \002,\002    SINCE MACHINE PRECISION DRELPR =\002,d10.3/\
\002 \002,\002    ZETA RESET TO \002,d10.3)";
    static char fmt_60[] = "(\0020\002,\002*** F A T A L     E R R O R *****\
*******\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE SOR \002/\002 \002\
,\002    INVALID MATRIX DIMENSION, N =\002,i8)";
    static char fmt_80[] = "(\0020\002,\002*** F A T A L     E R R O R *****\
*******\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE SOR \002/\002 \002\
,\002    ERROR DETECTED IN SUBROUTINE  SBELM \002/\002 \002,\002    WHICH RE\
MOVES ROWS AND COLUMNS OF SYSTEM \002/\002 \002,\002    WHEN DIAGONAL ENTRY \
TOO LARGE  \002/\002 \002,\002    IER = \002,i5,5x,\002 RPARM(8) = \002,d10.\
3,\002 (TOL)\002)";
    static char fmt_100[] = "(\0020\002,\002*** F A T A L     E R R O R ****\
********\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE SOR \002/\002 \002,\
\002    NOT ENOUGH WORKSPACE AT \002,i10/\002 \002,\002    SET IPARM(8) =\
\002,i10,\002 (NW)\002)";
    static char fmt_120[] = "(\0020\002,\002*** F A T A L     E R R O R ****\
********\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE SOR \002/\002 \002,\
\002    ERROR DETECTED IN SUBROUTINE  PRBNDX\002/\002 \002,\002    WHICH COM\
PUTES THE RED-BLACK INDEXING\002/\002 \002,\002    IER = \002,i5,\002 IPARM(\
9) = \002,i5,\002 (NB)\002)";
    static char fmt_140[] = "(/10x,\002ORDER OF BLACK SUBSYSTEM = \002,i5\
,\002 (NB)\002)";
    static char fmt_150[] = "(\0020\002,\002*** F A T A L     E R R O R ****\
********\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE SOR \002/\002 \002,\
\002    ERROR DETECTED IN SUBROUTINE  PERMAT\002/\002 \002,\002    WHICH DOE\
S THE RED-BLACK PERMUTATION\002/\002 \002,\002    IER = \002,i5)";
    static char fmt_180[] = "(\0020\002,\002*** F A T A L     E R R O R ****\
********\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE SOR \002/\002 \002,\
\002    ERROR DETECTED IN SUBROUTINE  SCAL  \002/\002 \002,\002    WHICH SCA\
LES THE SYSTEM   \002/\002 \002,\002    IER = \002,i5)";
    static char fmt_200[] = "(///1x,\002CME IS THE ESTIMATE OF THE LARGEST E\
IGENVALUE OF\002,\002 THE JACOBI MATRIX\002)";
    static char fmt_210[] = "(1x,\002OMEGA IS THE RELAXATION FACTOR\002)";
    static char fmt_260[] = "(\0020\002,\002*** W A R N I N G ***********\
*\002/\0020\002,\002    IN ITPACK ROUTINE SOR\002/\002 \002,\002    FAILURE \
TO CONVERGE IN\002,i5,\002 ITERATIONS\002)";
    static char fmt_290[] = "(/1x,\002SOR  HAS CONVERGED IN \002,i5,\002 ITE\
RATIONS\002)";
    static char fmt_310[] = "(\0020\002,\002*** F A T A L     E R R O R ****\
********\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE SOR \002/\002 \002,\
\002    ERROR DETECTED IN SUBROUTINE  PERMAT\002/\002 \002,\002    WHICH UND\
OES THE RED-BLACK PERMUTATION   \002/\002 \002,\002    IER = \002,i5)";

    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), e_wsfe(), do_fio(integer *, char *, ftnlen);

    /* Local variables */
    integer n, n3, nb, ib1, ib2, ib3, ier;
    doublereal tol;
    extern /* Subroutine */ int scal_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *,
	     integer *, integer *);
    doublereal temp;
    integer loop;
    doublereal time1, time2;
    real timi1, timj1, timi2, timj2;
    extern /* Subroutine */ int sbelm_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, integer *, doublereal *, doublereal *,
	     integer *, integer *, integer *, integer *);
    integer idgts;
    extern /* Subroutine */ int vfill_(integer *, doublereal *, doublereal *);
    extern doublereal timer_(real *);
    real dummy;
    extern /* Subroutine */ int itsor_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, doublereal *);
    doublereal digit1, digit2;
    integer itmax1;
    extern /* Subroutine */ int echall_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, integer *, doublereal *, integer *), 
	    pervec_(integer *, doublereal *, integer *), ivfill_(integer *, 
	    integer *, integer *);
    integer ierper;
    extern /* Subroutine */ int echout_(integer *, doublereal *, integer *), 
	    permat_(integer *, integer *, integer *, doublereal *, integer *, 
	    integer *, integer *, integer *, integer *, integer *), unscal_(
	    integer *, integer *, integer *, doublereal *, doublereal *, 
	    doublereal *, doublereal *), prbndx_(integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *), perror_(integer *, integer *, integer *, doublereal *,
	     doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, integer *);

    /* Fortran I/O blocks */
    static cilist io___75 = { 0, 0, 0, fmt_10, 0 };
    static cilist io___81 = { 0, 0, 0, fmt_40, 0 };
    static cilist io___86 = { 0, 0, 0, fmt_60, 0 };
    static cilist io___88 = { 0, 0, 0, fmt_80, 0 };
    static cilist io___92 = { 0, 0, 0, fmt_100, 0 };
    static cilist io___95 = { 0, 0, 0, fmt_120, 0 };
    static cilist io___96 = { 0, 0, 0, fmt_140, 0 };
    static cilist io___97 = { 0, 0, 0, fmt_150, 0 };
    static cilist io___98 = { 0, 0, 0, fmt_180, 0 };
    static cilist io___99 = { 0, 0, 0, fmt_200, 0 };
    static cilist io___100 = { 0, 0, 0, fmt_210, 0 };
    static cilist io___105 = { 0, 0, 0, fmt_260, 0 };
    static cilist io___106 = { 0, 0, 0, fmt_290, 0 };
    static cilist io___108 = { 0, 0, 0, fmt_310, 0 };



/*     ITPACK 2C MAIN SUBROUTINE  SOR  (SUCCESSIVE OVERRELATION) */
/*     EACH OF THE MAIN SUBROUTINES: */
/*           JCG, JSI, SOR, SSORCG, SSORSI, RSCG, RSSI */
/*     CAN BE USED INDEPENDENTLY OF THE OTHERS */

/* ... FUNCTION: */

/*          THIS SUBROUTINE, SOR, DRIVES THE  SUCCESSIVE */
/*          OVERRELAXATION ALGORITHM. */

/* ... PARAMETER LIST: */

/*          N      INPUT INTEGER.  DIMENSION OF THE MATRIX. (= NN) */
/*          IA,JA  INPUT INTEGER VECTORS.  THE TWO INTEGER ARRAYS OF */
/*                 THE SPARSE MATRIX REPRESENTATION. */
/*          A      INPUT D.P. VECTOR.  THE D.P. ARRAY OF THE SPARSE */
/*                 MATRIX REPRESENTATION */
/*          RHS    INPUT D.P. VECTOR.  CONTAINS THE RIGHT HAND SIDE */
/*                 OF THE MATRIX PROBLEM. */
/*          U      INPUT/OUTPUT D.P. VECTOR.  ON INPUT, U CONTAINS THE */
/*                 INITIAL GUESS TO THE SOLUTION. ON OUTPUT, IT CONTAINS */
/*                 THE LATEST ESTIMATE TO THE SOLUTION. */
/*          IWKSP  INTEGER VECTOR WORKSPACE OF LENGTH 3*N */
/*          NW     INPUT INTEGER.  LENGTH OF AVAILABLE WKSP.  ON OUTPUT, */
/*                 IPARM(8) IS AMOUNT USED. */
/*          WKSP   D.P. VECTOR USED FOR WORKING SPACE.  SOR NEEDS THIS */
/*                 TO BE IN LENGTH AT LEAST  N */
/*          IPARM  INTEGER VECTOR OF LENGTH 12.  ALLOWS USER TO SPECIFY */
/*                 SOME INTEGER PARAMETERS WHICH AFFECT THE METHOD. */
/*          RPARM  D.P. VECTOR OF LENGTH 12. ALLOWS USER TO SPECIFY SOME */
/*                 D.P. PARAMETERS WHICH AFFECT THE METHOD. */
/*          IER    OUTPUT INTEGER.  ERROR FLAG. (= IERR) */

/* ... SOR SUBPROGRAM REFERENCES: */

/*          FROM ITPACK   BISRCH, DFAULT, ECHALL, ECHOUT, IPSTR, ITERM, */
/*                        TIMER, ITSOR, IVFILL, PERMAT, PERROR, */
/*                        PERVEC, PFSOR1, PMULT, PRBNDX, PSTOP, QSORT, */
/*                        SBELM, SCAL, DCOPY, DDOT, TAU, UNSCAL, VFILL, */
/*                        VOUT, WEVMW */
/*          SYSTEM        DABS, DLOG10, DBLE(AMAX0), DMAX1, DBLE(FLOAT), */
/*                        DSQRT */

/*     VERSION:  ITPACK 2C (MARCH 1982) */

/*     CODE WRITTEN BY:  DAVID KINCAID, ROGER GRIMES, JOHN RESPESS */
/*                       CENTER FOR NUMERICAL ANALYSIS */
/*                       UNIVERSITY OF TEXAS */
/*                       AUSTIN, TX  78712 */
/*                       (512) 471-1242 */

/*     FOR ADDITIONAL DETAILS ON THE */
/*          (A) SUBROUTINE SEE TOMS ARTICLE 1982 */
/*          (B) ALGORITHM  SEE CNA REPORT 150 */

/*     BASED ON THEORY BY:  DAVID YOUNG, DAVID KINCAID, LOU HAGEMAN */

/*     REFERENCE THE BOOK:  APPLIED ITERATIVE METHODS */
/*                          L. HAGEMAN, D. YOUNG */
/*                          ACADEMIC PRESS, 1981 */

/*     ************************************************** */
/*     *               IMPORTANT NOTE                   * */
/*     *                                                * */
/*     *      WHEN INSTALLING ITPACK ROUTINES ON A      * */
/*     *  DIFFERENT COMPUTER, RESET SOME OF THE VALUES  * */
/*     *  IN  SUBROUTNE DFAULT.   MOST IMPORTANT ARE    * */
/*     *                                                * */
/*     *   DRELPR      MACHINE RELATIVE PRECISION       * */
/*     *   RPARM(1)    STOPPING CRITERION               * */
/*     *                                                * */
/*     *   ALSO CHANGE SYSTEM-DEPENDENT ROUTINE         * */
/*     *   SECOND USED IN TIMER                         * */
/*     *                                                * */
/*     ************************************************** */

/*     SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER IA(1),JA(1),IWKSP(1),IPARM(12),NN,NW,IERR >*/
/*<       DOUBLE PRECISION A(1),RHS(NN),U(NN),WKSP(NW),RPARM(12) >*/

/*     SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER IB1,IB2,IB3,IDGTS,IER,IERPER,ITMAX1,LOOP,N,NB,N3 >*/
/*<       DOUBLE PRECISION DIGIT1,DIGIT2,TEMP,TIME1,TIME2,TOL >*/

/* *** BEGIN: ITPACK COMMON */

/*<       INTEGER IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/
/*<       COMMON /ITCOM1/ IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/

/*<       LOGICAL ADAPT,BETADT,CASEII,HALT,PARTAD >*/
/*<       COMMON /ITCOM2/ ADAPT,BETADT,CASEII,HALT,PARTAD >*/

/*<    >*/
/*<    >*/

/* *** END  : ITPACK COMMON */

/* ... VARIABLES IN COMMON BLOCK - ITCOM1 */

/*     IN     - ITERATION NUMBER */
/*     IS     - ITERATION NUMBER WHEN PARAMETERS LAST CHANGED */
/*     ISYM   - SYMMETRIC/NONSYMMETRIC STORAGE FORMAT SWITCH */
/*     ITMAX  - MAXIMUM NUMBER OF ITERATIONS ALLOWED */
/*     LEVEL  - LEVEL OF OUTPUT CONTROL SWITCH */
/*     NOUT   - OUTPUT UNIT NUMBER */

/* ... VARIABLES IN COMMON BLOCK - ITCOM2 */

/*     ADAPT  - FULLY ADAPTIVE PROCEDURE SWITCH */
/*     BETADT - SWITCH FOR ADAPTIVE DETERMINATION OF BETA */
/*     CASEII - ADAPTIVE PROCEDURE CASE SWITCH */
/*     HALT   - STOPPING TEST SWITCH */
/*     PARTAD - PARTIALLY ADAPTIVE PROCEDURE SWITCH */

/* ... VARIABLES IN COMMON BLOCK - ITCOM3 */

/*     BDELNM - TWO NORM OF B TIMES DELTA-SUPER-N */
/*     BETAB  - ESTIMATE FOR THE SPECTRAL RADIUS OF LU MATRIX */
/*     CME    - ESTIMATE OF LARGEST EIGENVALUE */
/*     DELNNM - INNER PRODUCT OF PSEUDO-RESIDUAL AT ITERATION N */
/*     DELSNM - INNER PRODUCT OF PSEUDO-RESIDUAL AT ITERATION S */
/*     FF     - ADAPTIVE PROCEDURE DAMPING FACTOR */
/*     GAMMA  - ACCELERATION PARAMETER */
/*     OMEGA  - OVERRELAXATION PARAMETER FOR SOR AND SSOR */
/*     QA     - PSEUDO-RESIDUAL RATIO */
/*     QT     - VIRTUAL SPECTRAL RADIUS */
/*     RHO    - ACCELERATION PARAMETER */
/*     RRR    - ADAPTIVE PARAMETER */
/*     SIGE   - PARAMETER SIGMA-SUB-E */
/*     SME    - ESTIMATE OF SMALLEST EIGENVALUE */
/*     SPECR  - SPECTRAL RADIUS ESTIMATE FOR SSOR */
/*     DRELPR - MACHINE RELATIVE PRECISION */
/*     STPTST - STOPPING PARAMETER */
/*     UDNM   - TWO NORM OF U */
/*     ZETA   - STOPPING CRITERION */

/* ... INITIALIZE COMMON BLOCKS */

/*<       LEVEL = IPARM(2) >*/
    /* Parameter adjustments */
    --u;
    --rhs;
    --ia;
    --ja;
    --a;
    --iwksp;
    --wksp;
    --iparm;
    --rparm;

    /* Function Body */
    itcom1_1.level = iparm[2];
/*<       NOUT = IPARM(4) >*/
    itcom1_1.nout = iparm[4];
/*<       IF (LEVEL.GE.1) WRITE (NOUT,10) >*/
    if (itcom1_1.level >= 1) {
	io___75.ciunit = itcom1_1.nout;
	s_wsfe(&io___75);
	e_wsfe();
    }
/*<    10 FORMAT ('0'///1X,'BEGINNING OF ITPACK SOLUTION MODULE  SOR') >*/
/*<       IER = 0 >*/
    ier = 0;
/*<       IF (IPARM(1).LE.0) RETURN >*/
    if (iparm[1] <= 0) {
	return 0;
    }
/*<       N = NN >*/
    n = *nn;
/*<       IF (IPARM(11).EQ.0) TIMJ1 = TIMER(DUMMY) >*/
    if (iparm[11] == 0) {
	timj1 = timer_(&dummy);
    }
/*<       IF (LEVEL.GE.3) GO TO 20 >*/
    if (itcom1_1.level >= 3) {
	goto L20;
    }
/*<       CALL ECHOUT (IPARM,RPARM,3) >*/
    echout_(&iparm[1], &rparm[1], &c__3);
/*<       GO TO 30 >*/
    goto L30;
/*<    20 CALL ECHALL (N,IA,JA,A,RHS,IPARM,RPARM,1) >*/
L20:
    echall_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &iparm[1], &rparm[1], &c__1);
/*<    30 TEMP = 5.0D2*DRELPR >*/
L30:
    temp = itcom3_1.drelpr * 500.;
/*<       IF (ZETA.GE.TEMP) GO TO 50 >*/
    if (itcom3_1.zeta >= temp) {
	goto L50;
    }
/*<       IF (LEVEL.GE.1) WRITE (NOUT,40) ZETA,DRELPR,TEMP >*/
    if (itcom1_1.level >= 1) {
	io___81.ciunit = itcom1_1.nout;
	s_wsfe(&io___81);
	do_fio(&c__1, (char *)&itcom3_1.zeta, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&itcom3_1.drelpr, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&temp, (ftnlen)sizeof(doublereal));
	e_wsfe();
    }
/*<    >*/
/*<       ZETA = TEMP >*/
    itcom3_1.zeta = temp;
/*<    50 CONTINUE >*/
L50:
/*<       TIME1 = RPARM(9) >*/
    time1 = rparm[9];
/*<       TIME2 = RPARM(10) >*/
    time2 = rparm[10];
/*<       DIGIT1 = RPARM(11) >*/
    digit1 = rparm[11];
/*<       DIGIT2 = RPARM(12) >*/
    digit2 = rparm[12];

/* ... VERIFY N */

/*<       IF (N.GT.0) GO TO 70 >*/
    if (n > 0) {
	goto L70;
    }
/*<       IER = 31 >*/
    ier = 31;
/*<       IF (LEVEL.GE.0) WRITE (NOUT,60) N >*/
    if (itcom1_1.level >= 0) {
	io___86.ciunit = itcom1_1.nout;
	s_wsfe(&io___86);
	do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 360 >*/
    goto L360;
/*<    70 CONTINUE >*/
L70:

/* ... REMOVE ROWS AND COLUMNS IF REQUESTED */

/*<       IF (IPARM(10).EQ.0) GO TO 90 >*/
    if (iparm[10] == 0) {
	goto L90;
    }
/*<       TOL = RPARM(8) >*/
    tol = rparm[8];
/*<       CALL IVFILL (N,IWKSP,0) >*/
    ivfill_(&n, &iwksp[1], &c__0);
/*<       CALL VFILL (N,WKSP,0.0D0) >*/
    vfill_(&n, &wksp[1], &c_b21);
/*<       CALL SBELM (N,IA,JA,A,RHS,IWKSP,WKSP,TOL,ISYM,LEVEL,NOUT,IER) >*/
    sbelm_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &iwksp[1], &wksp[1], &tol, &
	    itcom1_1.isym, &itcom1_1.level, &itcom1_1.nout, &ier);
/*<       IF (IER.EQ.0) GO TO 90 >*/
    if (ier == 0) {
	goto L90;
    }
/*<       IF (LEVEL.GE.0) WRITE (NOUT,80) IER,TOL >*/
    if (itcom1_1.level >= 0) {
	io___88.ciunit = itcom1_1.nout;
	s_wsfe(&io___88);
	do_fio(&c__1, (char *)&ier, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&tol, (ftnlen)sizeof(doublereal));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 360 >*/
    goto L360;

/* ... INITIALIZE WKSP BASE ADDRESSES. */

/*<    90 IB1 = 1 >*/
L90:
    ib1 = 1;
/*<       IB2 = IB1+N >*/
    ib2 = ib1 + n;
/*<       IB3 = IB2+N >*/
    ib3 = ib2 + n;
/*<       IPARM(8) = N >*/
    iparm[8] = n;
/*<       IF (NW.GE.IPARM(8)) GO TO 110 >*/
    if (*nw >= iparm[8]) {
	goto L110;
    }
/*<       IER = 32 >*/
    ier = 32;
/*<       IF (LEVEL.GE.0) WRITE (NOUT,100) NW,IPARM(8) >*/
    if (itcom1_1.level >= 0) {
	io___92.ciunit = itcom1_1.nout;
	s_wsfe(&io___92);
	do_fio(&c__1, (char *)&(*nw), (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&iparm[8], (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 360 >*/
    goto L360;

/* ... PERMUTE TO  RED-BLACK SYSTEM IF REQUESTED */

/*<   110 NB = IPARM(9) >*/
L110:
    nb = iparm[9];
/*<       IF (NB.LT.0) GO TO 170 >*/
    if (nb < 0) {
	goto L170;
    }
/*<       N3 = 3*N >*/
    n3 = n * 3;
/*<       CALL IVFILL (N3,IWKSP,0) >*/
    ivfill_(&n3, &iwksp[1], &c__0);
/*<       CALL PRBNDX (N,NB,IA,JA,IWKSP,IWKSP(IB2),LEVEL,NOUT,IER) >*/
    prbndx_(&n, &nb, &ia[1], &ja[1], &iwksp[1], &iwksp[ib2], &itcom1_1.level, 
	    &itcom1_1.nout, &ier);
/*<       IF (IER.EQ.0) GO TO 130 >*/
    if (ier == 0) {
	goto L130;
    }
/*<       IF (LEVEL.GE.0) WRITE (NOUT,120) IER,NB >*/
    if (itcom1_1.level >= 0) {
	io___95.ciunit = itcom1_1.nout;
	s_wsfe(&io___95);
	do_fio(&c__1, (char *)&ier, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&nb, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 360 >*/
    goto L360;

/* ... PERMUTE MATRIX AND RHS */

/*<   130 IF (LEVEL.GE.2) WRITE (NOUT,140) NB >*/
L130:
    if (itcom1_1.level >= 2) {
	io___96.ciunit = itcom1_1.nout;
	s_wsfe(&io___96);
	do_fio(&c__1, (char *)&nb, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<   140 FORMAT (/10X,'ORDER OF BLACK SUBSYSTEM = ',I5,' (NB)') >*/
/*<       CALL PERMAT (N,IA,JA,A,IWKSP,IWKSP(IB3),ISYM,LEVEL,NOUT,IER) >*/
    permat_(&n, &ia[1], &ja[1], &a[1], &iwksp[1], &iwksp[ib3], &itcom1_1.isym,
	     &itcom1_1.level, &itcom1_1.nout, &ier);
/*<       IF (IER.EQ.0) GO TO 160 >*/
    if (ier == 0) {
	goto L160;
    }
/*<       IF (LEVEL.GE.0) WRITE (NOUT,150) IER >*/
    if (itcom1_1.level >= 0) {
	io___97.ciunit = itcom1_1.nout;
	s_wsfe(&io___97);
	do_fio(&c__1, (char *)&ier, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 360 >*/
    goto L360;
/*<   160 CALL PERVEC (N,RHS,IWKSP) >*/
L160:
    pervec_(&n, &rhs[1], &iwksp[1]);
/*<       CALL PERVEC (N,U,IWKSP) >*/
    pervec_(&n, &u[1], &iwksp[1]);

/* ... SCALE LINEAR SYSTEM, U, AND RHS BY THE SQUARE ROOT OF THE */
/* ... DIAGONAL ELEMENTS. */

/*<   170 CONTINUE >*/
L170:
/*<       CALL VFILL (IPARM(8),WKSP,0.0D0) >*/
    vfill_(&iparm[8], &wksp[1], &c_b21);
/*<       CALL SCAL (N,IA,JA,A,RHS,U,WKSP,LEVEL,NOUT,IER) >*/
    scal_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &u[1], &wksp[1], &
	    itcom1_1.level, &itcom1_1.nout, &ier);
/*<       IF (IER.EQ.0) GO TO 190 >*/
    if (ier == 0) {
	goto L190;
    }
/*<       IF (LEVEL.GE.0) WRITE (NOUT,180) IER >*/
    if (itcom1_1.level >= 0) {
	io___98.ciunit = itcom1_1.nout;
	s_wsfe(&io___98);
	do_fio(&c__1, (char *)&ier, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 360 >*/
    goto L360;
/*<   190 IF (LEVEL.LE.2) GO TO 220 >*/
L190:
    if (itcom1_1.level <= 2) {
	goto L220;
    }
/*<       IF (ADAPT) WRITE (NOUT,200) >*/
    if (itcom2_1.adapt) {
	io___99.ciunit = itcom1_1.nout;
	s_wsfe(&io___99);
	e_wsfe();
    }
/*<    >*/
/*<       WRITE (NOUT,210) >*/
    io___100.ciunit = itcom1_1.nout;
    s_wsfe(&io___100);
    e_wsfe();
/*<   210 FORMAT (1X,'OMEGA IS THE RELAXATION FACTOR') >*/
/*<   220 IF (IPARM(11).NE.0) GO TO 230 >*/
L220:
    if (iparm[11] != 0) {
	goto L230;
    }
/*<       TIMI1 = TIMER(DUMMY) >*/
    timi1 = timer_(&dummy);

/* ... ITERATION SEQUENCE */

/*<   230 ITMAX1 = ITMAX+1 >*/
L230:
    itmax1 = itcom1_1.itmax + 1;
/*<       DO 240 LOOP = 1,ITMAX1 >*/
    i__1 = itmax1;
    for (loop = 1; loop <= i__1; ++loop) {
/*<          IN = LOOP-1 >*/
	itcom1_1.in = loop - 1;

/* ... CODE FOR ONE ITERATION. */

/*     U           = U(IN) */

/*<          CALL ITSOR (N,IA,JA,A,RHS,U,WKSP(IB1)) >*/
	itsor_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &u[1], &wksp[ib1]);

/*<          IF (HALT) GO TO 270 >*/
	if (itcom2_1.halt) {
	    goto L270;
	}
/*<   240 CONTINUE >*/
/* L240: */
    }

/* ... ITMAX HAS BEEN REACHED */

/*<       IF (IPARM(11).NE.0) GO TO 250 >*/
    if (iparm[11] != 0) {
	goto L250;
    }
/*<       TIMI2 = TIMER(DUMMY) >*/
    timi2 = timer_(&dummy);
/*<       TIME1 = DBLE(TIMI2-TIMI1) >*/
    time1 = (doublereal) (timi2 - timi1);
/*<   250 IF (LEVEL.GE.1) WRITE (NOUT,260) ITMAX >*/
L250:
    if (itcom1_1.level >= 1) {
	io___105.ciunit = itcom1_1.nout;
	s_wsfe(&io___105);
	do_fio(&c__1, (char *)&itcom1_1.itmax, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       IER = 33 >*/
    ier = 33;
/*<       IF (IPARM(3).EQ.0) RPARM(1) = STPTST >*/
    if (iparm[3] == 0) {
	rparm[1] = itcom3_1.stptst;
    }
/*<       GO TO 300 >*/
    goto L300;

/* ... METHOD HAS CONVERGED */

/*<   270 IF (IPARM(11).NE.0) GO TO 280 >*/
L270:
    if (iparm[11] != 0) {
	goto L280;
    }
/*<       TIMI2 = TIMER(DUMMY) >*/
    timi2 = timer_(&dummy);
/*<       TIME1 = DBLE(TIMI2-TIMI1) >*/
    time1 = (doublereal) (timi2 - timi1);
/*<   280 IF (LEVEL.GE.1) WRITE (NOUT,290) IN >*/
L280:
    if (itcom1_1.level >= 1) {
	io___106.ciunit = itcom1_1.nout;
	s_wsfe(&io___106);
	do_fio(&c__1, (char *)&itcom1_1.in, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<   290 FORMAT (/1X,'SOR  HAS CONVERGED IN ',I5,' ITERATIONS') >*/

/* ... UNSCALE THE MATRIX, SOLUTION, AND RHS VECTORS. */

/*<   300 CALL UNSCAL (N,IA,JA,A,RHS,U,WKSP) >*/
L300:
    unscal_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &u[1], &wksp[1]);

/* ... UN-PERMUTE MATRIX,RHS, AND SOLUTION */

/*<       IF (IPARM(9).LT.0) GO TO 330 >*/
    if (iparm[9] < 0) {
	goto L330;
    }
/*<    >*/
    permat_(&n, &ia[1], &ja[1], &a[1], &iwksp[ib2], &iwksp[ib3], &
	    itcom1_1.isym, &itcom1_1.level, &itcom1_1.nout, &ierper);
/*<       IF (IERPER.EQ.0) GO TO 320 >*/
    if (ierper == 0) {
	goto L320;
    }
/*<       IF (LEVEL.GE.0) WRITE (NOUT,310) IERPER >*/
    if (itcom1_1.level >= 0) {
	io___108.ciunit = itcom1_1.nout;
	s_wsfe(&io___108);
	do_fio(&c__1, (char *)&ierper, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       IF (IER.EQ.0) IER = IERPER >*/
    if (ier == 0) {
	ier = ierper;
    }
/*<       GO TO 360 >*/
    goto L360;
/*<   320 CALL PERVEC (N,RHS,IWKSP(IB2)) >*/
L320:
    pervec_(&n, &rhs[1], &iwksp[ib2]);
/*<       CALL PERVEC (N,U,IWKSP(IB2)) >*/
    pervec_(&n, &u[1], &iwksp[ib2]);

/* ... OPTIONAL ERROR ANALYSIS */

/*<   330 IDGTS = IPARM(12) >*/
L330:
    idgts = iparm[12];
/*<       IF (IDGTS.LT.0) GO TO 340 >*/
    if (idgts < 0) {
	goto L340;
    }
/*<       IF (IPARM(2).LE.0) IDGTS = 0 >*/
    if (iparm[2] <= 0) {
	idgts = 0;
    }
/*<       CALL PERROR (N,IA,JA,A,RHS,U,WKSP,DIGIT1,DIGIT2,IDGTS) >*/
    perror_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &u[1], &wksp[1], &digit1, &
	    digit2, &idgts);

/* ... SET RETURN PARAMETERS IN IPARM AND RPARM */

/*<   340 IF (IPARM(11).NE.0) GO TO 350 >*/
L340:
    if (iparm[11] != 0) {
	goto L350;
    }
/*<       TIMJ2 = TIMER(DUMMY) >*/
    timj2 = timer_(&dummy);
/*<       TIME2 = DBLE(TIMJ2-TIMJ1) >*/
    time2 = (doublereal) (timj2 - timj1);
/*<   350 IF (IPARM(3).NE.0) GO TO 360 >*/
L350:
    if (iparm[3] != 0) {
	goto L360;
    }
/*<       IPARM(1) = IN >*/
    iparm[1] = itcom1_1.in;
/*<       IPARM(9) = NB >*/
    iparm[9] = nb;
/*<       RPARM(2) = CME >*/
    rparm[2] = itcom3_1.cme;
/*<       RPARM(3) = SME >*/
    rparm[3] = itcom3_1.sme;
/*<       RPARM(5) = OMEGA >*/
    rparm[5] = itcom3_1.omega;
/*<       RPARM(9) = TIME1 >*/
    rparm[9] = time1;
/*<       RPARM(10) = TIME2 >*/
    rparm[10] = time2;
/*<       RPARM(11) = DIGIT1 >*/
    rparm[11] = digit1;
/*<       RPARM(12) = DIGIT2 >*/
    rparm[12] = digit2;

/*<   360 CONTINUE >*/
L360:
/*<       IERR = IER >*/
    *ierr = ier;
/*<       IF (LEVEL.GE.3) CALL ECHALL (N,IA,JA,A,RHS,IPARM,RPARM,2) >*/
    if (itcom1_1.level >= 3) {
	echall_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &iparm[1], &rparm[1], &
		c__2);
    }

/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* sor_ */

/*<    >*/
/* Subroutine */ int ssorcg_(integer *nn, integer *ia, integer *ja, 
	doublereal *a, doublereal *rhs, doublereal *u, integer *iwksp, 
	integer *nw, doublereal *wksp, integer *iparm, doublereal *rparm, 
	integer *ierr)
{
    /* Format strings */
    static char fmt_10[] = "(\0020\002///1x,\002BEGINNING OF ITPACK SOLUTION\
 MODULE  SSORCG\002)";
    static char fmt_40[] = "(\0020\002,\002*** W A R N I N G ************\
\002/\0020\002,\002    IN ITPACK ROUTINE SSORCG\002/\002 \002,\002    RPARM(\
1) =\002,d10.3,\002 (ZETA)\002/\002 \002,\002    A VALUE THIS SMALL MAY HIND\
ER CONVERGENCE \002/\002 \002,\002    SINCE MACHINE PRECISION DRELPR =\002,d\
10.3/\002 \002,\002    ZETA RESET TO \002,d10.3)";
    static char fmt_60[] = "(\0020\002,\002*** F A T A L     E R R O R *****\
*******\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE SSORCG \002/\002\
 \002,\002    INVALID MATRIX DIMENSION, N =\002,i8)";
    static char fmt_80[] = "(\0020\002,\002*** F A T A L     E R R O R *****\
*******\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE SSORCG \002/\002\
 \002,\002    ERROR DETECTED IN SUBROUTINE  SBELM \002/\002 \002,\002    WHI\
CH REMOVES ROWS AND COLUMNS OF SYSTEM \002/\002 \002,\002    WHEN DIAGONAL E\
NTRY TOO LARGE  \002/\002 \002,\002    IER = \002,i5,5x,\002 RPARM(8) = \002\
,d10.3,\002 (TOL)\002)";
    static char fmt_100[] = "(\0020\002,\002*** F A T A L     E R R O R ****\
********\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE SSORCG \002/\002\
 \002,\002    NOT ENOUGH WORKSPACE AT \002,i10/\002 \002,\002    SET IPARM(8\
) =\002,i10,\002 (NW)\002)";
    static char fmt_120[] = "(\0020\002,\002*** F A T A L     E R R O R ****\
********\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE SSORCG \002/\002\
 \002,\002    ERROR DETECTED IN SUBROUTINE  PRBNDX\002/\002 \002,\002    WHI\
CH COMPUTES THE RED-BLACK INDEXING\002/\002 \002,\002    IER = \002,i5,\002 \
IPARM(9) = \002,i5,\002 (NB)\002)";
    static char fmt_140[] = "(/10x,\002ORDER OF BLACK SUBSYSTEM = \002,i5\
,\002 (NB)\002)";
    static char fmt_150[] = "(\0020\002,\002*** F A T A L     E R R O R ****\
********\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE SSORCG \002/\002\
 \002,\002    ERROR DETECTED IN SUBROUTINE  PERMAT\002/\002 \002,\002    WHI\
CH DOES THE RED-BLACK PERMUTATION\002/\002 \002,\002    IER = \002,i5)";
    static char fmt_180[] = "(\0020\002,\002*** F A T A L     E R R O R ****\
********\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE SSORCG \002/\002\
 \002,\002    ERROR DETECTED IN SUBROUTINE  SCAL  \002/\002 \002,\002    WHI\
CH SCALES THE SYSTEM   \002/\002 \002,\002    IER = \002,i5)";
    static char fmt_200[] = "(///1x,\002IN THE FOLLOWING, RHO AND GAMMA AR\
E\002,\002 ACCELERATION PARAMETERS\002)";
    static char fmt_210[] = "(1x,\002S-PRIME IS AN INITIAL ESTIMATE FOR NEW \
CME\002)";
    static char fmt_290[] = "(\0020\002,\002*** W A R N I N G ***********\
*\002/\0020\002,\002    IN ITPACK ROUTINE SSORCG\002/\002 \002,\002    FAILU\
RE TO CONVERGE IN\002,i5,\002 ITERATIONS\002)";
    static char fmt_320[] = "(/1x,\002SSORCG  HAS CONVERGED IN \002,i5,\002 \
ITERATIONS\002)";
    static char fmt_340[] = "(\0020\002,\002*** F A T A L     E R R O R ****\
********\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE SSORCG \002/\002\
 \002,\002    ERROR DETECTED IN SUBROUTINE  PERMAT\002/\002 \002,\002    WHI\
CH UNDOES THE RED-BLACK PERMUTATION   \002/\002 \002,\002    IER = \002,i5)";

    /* System generated locals */
    integer i__1;
    doublereal d__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), e_wsfe(), do_fio(integer *, char *, ftnlen);

    /* Local variables */
    integer n, n3, nb, ib1, ib2, ib3, ib4, ib5, ib6, ib7, ier;
    doublereal tol;
    extern /* Subroutine */ int scal_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *,
	     integer *, integer *), omeg_(doublereal *, integer *);
    doublereal temp;
    integer loop;
    doublereal time1, time2;
    real timi1, timj1, timi2, timj2;
    extern doublereal pbeta_(integer *, integer *, integer *, doublereal *, 
	    doublereal *, doublereal *, doublereal *);
    extern /* Subroutine */ int sbelm_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, integer *, doublereal *, doublereal *,
	     integer *, integer *, integer *, integer *);
    integer idgts;
    extern /* Subroutine */ int vfill_(integer *, doublereal *, doublereal *),
	     dcopy_(integer *, doublereal *, integer *, doublereal *, integer 
	    *);
    extern doublereal timer_(real *);
    extern /* Subroutine */ int pfsor_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *);
    real dummy;
    extern /* Subroutine */ int vevmw_(integer *, doublereal *, doublereal *);
    doublereal digit1, digit2;
    integer itmax1;
    extern /* Subroutine */ int echall_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, integer *, doublereal *, integer *);
    doublereal betnew;
    extern /* Subroutine */ int ivfill_(integer *, integer *, integer *);
    integer ierper;
    extern /* Subroutine */ int echout_(integer *, doublereal *, integer *), 
	    permat_(integer *, integer *, integer *, doublereal *, integer *, 
	    integer *, integer *, integer *, integer *, integer *), pervec_(
	    integer *, doublereal *, integer *), itsrcg_(integer *, integer *,
	     integer *, doublereal *, doublereal *, doublereal *, doublereal *
	    , doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *), unscal_(integer *, integer *, 
	    integer *, doublereal *, doublereal *, doublereal *, doublereal *)
	    , prbndx_(integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *), perror_(integer *, 
	    integer *, integer *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, integer *);

    /* Fortran I/O blocks */
    static cilist io___111 = { 0, 0, 0, fmt_10, 0 };
    static cilist io___117 = { 0, 0, 0, fmt_40, 0 };
    static cilist io___122 = { 0, 0, 0, fmt_60, 0 };
    static cilist io___124 = { 0, 0, 0, fmt_80, 0 };
    static cilist io___132 = { 0, 0, 0, fmt_100, 0 };
    static cilist io___135 = { 0, 0, 0, fmt_120, 0 };
    static cilist io___136 = { 0, 0, 0, fmt_140, 0 };
    static cilist io___137 = { 0, 0, 0, fmt_150, 0 };
    static cilist io___138 = { 0, 0, 0, fmt_180, 0 };
    static cilist io___139 = { 0, 0, 0, fmt_200, 0 };
    static cilist io___140 = { 0, 0, 0, fmt_210, 0 };
    static cilist io___146 = { 0, 0, 0, fmt_290, 0 };
    static cilist io___147 = { 0, 0, 0, fmt_320, 0 };
    static cilist io___149 = { 0, 0, 0, fmt_340, 0 };



/*     ITPACK 2C MAIN SUBROUTINE  SSORCG  (SYMMETRIC SUCCESSIVE OVER- */
/*                                        RELAXATION CONJUGATE GRADIENT) */
/*     EACH OF THE MAIN SUBROUTINES: */
/*           JCG, JSI, SOR, SSORCG, SSORSI, RSCG, RSSI */
/*     CAN BE USED INDEPENDENTLY OF THE OTHERS */

/* ... FUNCTION: */

/*          THIS SUBROUTINE, SSORCG, DRIVES THE  SYMMETRIC SOR-CG */
/*          ALGORITHM. */

/* ... PARAMETER LIST: */

/*          N      INPUT INTEGER.  DIMENSION OF THE MATRIX. (= NN) */
/*          IA,JA  INPUT INTEGER VECTORS.  THE TWO INTEGER ARRAYS OF */
/*                 THE SPARSE MATRIX REPRESENTATION. */
/*          A      INPUT D.P. VECTOR.  THE D.P. ARRAY OF THE SPARSE */
/*                 MATRIX REPRESENTATION. */
/*          RHS    INPUT D.P. VECTOR.  CONTAINS THE RIGHT HAND SIDE */
/*                 OF THE MATRIX PROBLEM. */
/*          U      INPUT/OUTPUT D.P. VECTOR.  ON INPUT, U CONTAINS THE */
/*                 INITIAL GUESS TO THE SOLUTION. ON OUTPUT, IT CONTAINS */
/*                 THE LATEST ESTIMATE TO THE SOLUTION. */
/*          IWKSP  INTEGER VECTOR WORKSPACE OF LENGTH 3*N */
/*          NW     INPUT INTEGER.  LENGTH OF AVAILABLE WKSP.  ON OUTPUT, */
/*                 IPARM(8) IS AMOUNT USED. */
/*          WKSP   D.P. VECTOR USED FOR WORKING SPACE.  SSOR-CG */
/*                 NEEDS TO BE IN LENGTH AT LEAST */
/*                 6*N + 2*ITMAX,  IF IPARM(5)=0  (SYMMETRIC STORAGE) */
/*                 6*N + 4*ITMAX,  IF IPARM(5)=1  (NONSYMMETRIC STORAGE) */
/*          IPARM  INTEGER VECTOR OF LENGTH 12.  ALLOWS USER TO SPECIFY */
/*                 SOME INTEGER PARAMETERS WHICH AFFECT THE METHOD.  IF */
/*          RPARM  D.P. VECTOR OF LENGTH 12. ALLOWS USER TO SPECIFY SOME */
/*                 D.P. PARAMETERS WHICH AFFECT THE METHOD. */
/*          IER    OUTPUT INTEGER.  ERROR FLAG. (= IERR) */

/* ... SSORCG SUBPROGRAM REFERENCES: */

/*          FROM ITPACK    BISRCH, CHGCON, DETERM, DFAULT, ECHALL, */
/*                         ECHOUT, EIGVNS, EIGVSS, EQRT1S, ITERM, TIMER, */
/*                         ITSRCG, IVFILL, OMEG, OMGCHG, OMGSTR, */
/*                         PARCON, PBETA, PBSOR, PERMAT, PERROR, */
/*                         PERVEC, PFSOR, PJAC, PMULT, PRBNDX, PSTOP, PVT */
/*                         QSORT, SBELM, SCAL, DCOPY, DDOT, SUM3, */
/*                         UNSCAL, VEVMW, VEVPW, VFILL, VOUT, WEVMW, */
/*                         ZBRENT */
/*          SYSTEM         DABS, DLOG, DLOG10, DBLE(AMAX0), DMAX1, AMIN1, */
/*                         MOD, DSQRT */

/*     VERSION:  ITPACK 2C (MARCH 1982) */

/*     CODE WRITTEN BY:  DAVID KINCAID, ROGER GRIMES, JOHN RESPESS */
/*                       CENTER FOR NUMERICAL ANALYSIS */
/*                       UNIVERSITY OF TEXAS */
/*                       AUSTIN, TX  78712 */
/*                       (512) 471-1242 */

/*     FOR ADDITIONAL DETAILS ON THE */
/*          (A) SUBROUTINE SEE TOMS ARTICLE 1982 */
/*          (B) ALGORITHM  SEE CNA REPORT 150 */

/*     BASED ON THEORY BY:  DAVID YOUNG, DAVID KINCAID, LOU HAGEMAN */

/*     REFERENCE THE BOOK:  APPLIED ITERATIVE METHODS */
/*                          L. HAGEMAN, D. YOUNG */
/*                          ACADEMIC PRESS, 1981 */

/*     ************************************************** */
/*     *               IMPORTANT NOTE                   * */
/*     *                                                * */
/*     *      WHEN INSTALLING ITPACK ROUTINES ON A      * */
/*     *  DIFFERENT COMPUTER, RESET SOME OF THE VALUES  * */
/*     *  IN  SUBROUTNE DFAULT.   MOST IMPORTANT ARE    * */
/*     *                                                * */
/*     *   DRELPR      MACHINE RELATIVE PRECISION       * */
/*     *   RPARM(1)    STOPPING CRITERION               * */
/*     *                                                * */
/*     *   ALSO CHANGE SYSTEM-DEPENDENT ROUTINE         * */
/*     *   SECOND USED IN TIMER                         * */
/*     *                                                * */
/*     ************************************************** */

/*     SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER IA(1),JA(1),IWKSP(1),IPARM(12),NN,NW,IERR >*/
/*<       DOUBLE PRECISION A(1),RHS(NN),U(NN),WKSP(NW),RPARM(12) >*/

/*     SPECIFICATIONS FOR LOCAL VARIABLES */

/*<    >*/
/*<       DOUBLE PRECISION BETNEW,DIGIT1,DIGIT2,PBETA,TEMP,TIME1,TIME2,TOL >*/

/* *** BEGIN: ITPACK COMMON */

/*<       INTEGER IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/
/*<       COMMON /ITCOM1/ IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/

/*<       LOGICAL ADAPT,BETADT,CASEII,HALT,PARTAD >*/
/*<       COMMON /ITCOM2/ ADAPT,BETADT,CASEII,HALT,PARTAD >*/

/*<    >*/
/*<    >*/

/* *** END  : ITPACK COMMON */

/* ... VARIABLES IN COMMON BLOCK - ITCOM1 */

/*     IN     - ITERATION NUMBER */
/*     IS     - ITERATION NUMBER WHEN PARAMETERS LAST CHANGED */
/*     ISYM   - SYMMETRIC/NONSYMMETRIC STORAGE FORMAT SWITCH */
/*     ITMAX  - MAXIMUM NUMBER OF ITERATIONS ALLOWED */
/*     LEVEL  - LEVEL OF OUTPUT CONTROL SWITCH */
/*     NOUT   - OUTPUT UNIT NUMBER */

/* ... VARIABLES IN COMMON BLOCK - ITCOM2 */

/*     ADAPT  - FULLY ADAPTIVE PROCEDURE SWITCH */
/*     BETADT - SWITCH FOR ADAPTIVE DETERMINATION OF BETA */
/*     CASEII - ADAPTIVE PROCEDURE CASE SWITCH */
/*     HALT   - STOPPING TEST SWITCH */
/*     PARTAD - PARTIALLY ADAPTIVE PROCEDURE SWITCH */

/* ... VARIABLES IN COMMON BLOCK - ITCOM3 */

/*     BDELNM - TWO NORM OF B TIMES DELTA-SUPER-N */
/*     BETAB  - ESTIMATE FOR THE SPECTRAL RADIUS OF LU MATRIX */
/*     CME    - ESTIMATE OF LARGEST EIGENVALUE */
/*     DELNNM - INNER PRODUCT OF PSEUDO-RESIDUAL AT ITERATION N */
/*     DELSNM - INNER PRODUCT OF PSEUDO-RESIDUAL AT ITERATION S */
/*     FF     - ADAPTIVE PROCEDURE DAMPING FACTOR */
/*     GAMMA  - ACCELERATION PARAMETER */
/*     OMEGA  - OVERRELAXATION PARAMETER FOR SOR AND SSOR */
/*     QA     - PSEUDO-RESIDUAL RATIO */
/*     QT     - VIRTUAL SPECTRAL RADIUS */
/*     RHO    - ACCELERATION PARAMETER */
/*     RRR    - ADAPTIVE PARAMETER */
/*     SIGE   - PARAMETER SIGMA-SUB-E */
/*     SME    - ESTIMATE OF SMALLEST EIGENVALUE */
/*     SPECR  - SPECTRAL RADIUS ESTIMATE FOR SSOR */
/*     DRELPR - MACHINE RELATIVE PRECISION */
/*     STPTST - STOPPING PARAMETER */
/*     UDNM   - TWO NORM OF U */
/*     ZETA   - STOPPING CRITERION */

/* ... INITIALIZE COMMON BLOCKS */

/*<       LEVEL = IPARM(2) >*/
    /* Parameter adjustments */
    --u;
    --rhs;
    --ia;
    --ja;
    --a;
    --iwksp;
    --wksp;
    --iparm;
    --rparm;

    /* Function Body */
    itcom1_1.level = iparm[2];
/*<       NOUT = IPARM(4) >*/
    itcom1_1.nout = iparm[4];
/*<       IF (IPARM(9).GE.0) IPARM(6) = 2 >*/
    if (iparm[9] >= 0) {
	iparm[6] = 2;
    }
/*<       IF (LEVEL.GE.1) WRITE (NOUT,10) >*/
    if (itcom1_1.level >= 1) {
	io___111.ciunit = itcom1_1.nout;
	s_wsfe(&io___111);
	e_wsfe();
    }
/*<    10 FORMAT ('0'///1X,'BEGINNING OF ITPACK SOLUTION MODULE  SSORCG') >*/
/*<       IER = 0 >*/
    ier = 0;
/*<       IF (IPARM(1).LE.0) RETURN >*/
    if (iparm[1] <= 0) {
	return 0;
    }
/*<       N = NN >*/
    n = *nn;
/*<       IF (IPARM(11).EQ.0) TIMJ1 = TIMER(DUMMY) >*/
    if (iparm[11] == 0) {
	timj1 = timer_(&dummy);
    }
/*<       IF (LEVEL.GE.3) GO TO 20 >*/
    if (itcom1_1.level >= 3) {
	goto L20;
    }
/*<       CALL ECHOUT (IPARM,RPARM,4) >*/
    echout_(&iparm[1], &rparm[1], &c__4);
/*<       GO TO 30 >*/
    goto L30;
/*<    20 CALL ECHALL (N,IA,JA,A,RHS,IPARM,RPARM,1) >*/
L20:
    echall_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &iparm[1], &rparm[1], &c__1);
/*<    30 TEMP = 5.0D2*DRELPR >*/
L30:
    temp = itcom3_1.drelpr * 500.;
/*<       IF (ZETA.GE.TEMP) GO TO 50 >*/
    if (itcom3_1.zeta >= temp) {
	goto L50;
    }
/*<       IF (LEVEL.GE.1) WRITE (NOUT,40) ZETA,DRELPR,TEMP >*/
    if (itcom1_1.level >= 1) {
	io___117.ciunit = itcom1_1.nout;
	s_wsfe(&io___117);
	do_fio(&c__1, (char *)&itcom3_1.zeta, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&itcom3_1.drelpr, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&temp, (ftnlen)sizeof(doublereal));
	e_wsfe();
    }
/*<    >*/
/*<       ZETA = TEMP >*/
    itcom3_1.zeta = temp;
/*<    50 CONTINUE >*/
L50:
/*<       TIME1 = RPARM(9) >*/
    time1 = rparm[9];
/*<       TIME2 = RPARM(10) >*/
    time2 = rparm[10];
/*<       DIGIT1 = RPARM(11) >*/
    digit1 = rparm[11];
/*<       DIGIT2 = RPARM(12) >*/
    digit2 = rparm[12];

/* ... VERIFY N */

/*<       IF (N.GT.0) GO TO 70 >*/
    if (n > 0) {
	goto L70;
    }
/*<       IER = 41 >*/
    ier = 41;
/*<       IF (LEVEL.GE.0) WRITE (NOUT,60) N >*/
    if (itcom1_1.level >= 0) {
	io___122.ciunit = itcom1_1.nout;
	s_wsfe(&io___122);
	do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 390 >*/
    goto L390;
/*<    70 CONTINUE >*/
L70:

/* ... REMOVE ROWS AND COLUMNS IF REQUESTED */

/*<       IF (IPARM(10).EQ.0) GO TO 90 >*/
    if (iparm[10] == 0) {
	goto L90;
    }
/*<       TOL = RPARM(8) >*/
    tol = rparm[8];
/*<       CALL IVFILL (N,IWKSP,0) >*/
    ivfill_(&n, &iwksp[1], &c__0);
/*<       CALL VFILL (N,WKSP,0.0D0) >*/
    vfill_(&n, &wksp[1], &c_b21);
/*<       CALL SBELM (N,IA,JA,A,RHS,IWKSP,WKSP,TOL,ISYM,LEVEL,NOUT,IER) >*/
    sbelm_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &iwksp[1], &wksp[1], &tol, &
	    itcom1_1.isym, &itcom1_1.level, &itcom1_1.nout, &ier);
/*<       IF (IER.EQ.0) GO TO 90 >*/
    if (ier == 0) {
	goto L90;
    }
/*<       IF (LEVEL.GE.0) WRITE (NOUT,80) IER,TOL >*/
    if (itcom1_1.level >= 0) {
	io___124.ciunit = itcom1_1.nout;
	s_wsfe(&io___124);
	do_fio(&c__1, (char *)&ier, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&tol, (ftnlen)sizeof(doublereal));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 390 >*/
    goto L390;

/* ... INITIALIZE WKSP BASE ADDRESSES. */

/*<    90 IB1 = 1 >*/
L90:
    ib1 = 1;
/*<       IB2 = IB1+N >*/
    ib2 = ib1 + n;
/*<       IB3 = IB2+N >*/
    ib3 = ib2 + n;
/*<       IB4 = IB3+N >*/
    ib4 = ib3 + n;
/*<       IB5 = IB4+N >*/
    ib5 = ib4 + n;
/*<       IB6 = IB5+N >*/
    ib6 = ib5 + n;
/*<       IB7 = IB6+N >*/
    ib7 = ib6 + n;
/*<       IPARM(8) = 6*N+2*ITMAX >*/
    iparm[8] = n * 6 + (itcom1_1.itmax << 1);
/*<       IF (ISYM.NE.0) IPARM(8) = IPARM(8)+2*ITMAX >*/
    if (itcom1_1.isym != 0) {
	iparm[8] += itcom1_1.itmax << 1;
    }
/*<       IF (NW.GE.IPARM(8)) GO TO 110 >*/
    if (*nw >= iparm[8]) {
	goto L110;
    }
/*<       IER = 42 >*/
    ier = 42;
/*<       IF (LEVEL.GE.0) WRITE (NOUT,100) NW,IPARM(8) >*/
    if (itcom1_1.level >= 0) {
	io___132.ciunit = itcom1_1.nout;
	s_wsfe(&io___132);
	do_fio(&c__1, (char *)&(*nw), (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&iparm[8], (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 390 >*/
    goto L390;

/* ... PERMUTE TO  RED-BLACK SYSTEM IF REQUESTED */

/*<   110 NB = IPARM(9) >*/
L110:
    nb = iparm[9];
/*<       IF (NB.LT.0) GO TO 170 >*/
    if (nb < 0) {
	goto L170;
    }
/*<       N3 = 3*N >*/
    n3 = n * 3;
/*<       CALL IVFILL (N3,IWKSP,0) >*/
    ivfill_(&n3, &iwksp[1], &c__0);
/*<       CALL PRBNDX (N,NB,IA,JA,IWKSP,IWKSP(IB2),LEVEL,NOUT,IER) >*/
    prbndx_(&n, &nb, &ia[1], &ja[1], &iwksp[1], &iwksp[ib2], &itcom1_1.level, 
	    &itcom1_1.nout, &ier);
/*<       IF (IER.EQ.0) GO TO 130 >*/
    if (ier == 0) {
	goto L130;
    }
/*<       IF (LEVEL.GE.0) WRITE (NOUT,120) IER,NB >*/
    if (itcom1_1.level >= 0) {
	io___135.ciunit = itcom1_1.nout;
	s_wsfe(&io___135);
	do_fio(&c__1, (char *)&ier, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&nb, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 390 >*/
    goto L390;

/* ... PERMUTE MATRIX AND RHS */

/*<   130 IF (LEVEL.GE.2) WRITE (NOUT,140) NB >*/
L130:
    if (itcom1_1.level >= 2) {
	io___136.ciunit = itcom1_1.nout;
	s_wsfe(&io___136);
	do_fio(&c__1, (char *)&nb, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<   140 FORMAT (/10X,'ORDER OF BLACK SUBSYSTEM = ',I5,' (NB)') >*/
/*<       CALL PERMAT (N,IA,JA,A,IWKSP,IWKSP(IB3),ISYM,LEVEL,NOUT,IER) >*/
    permat_(&n, &ia[1], &ja[1], &a[1], &iwksp[1], &iwksp[ib3], &itcom1_1.isym,
	     &itcom1_1.level, &itcom1_1.nout, &ier);
/*<       IF (IER.EQ.0) GO TO 160 >*/
    if (ier == 0) {
	goto L160;
    }
/*<       IF (LEVEL.GE.0) WRITE (NOUT,150) IER >*/
    if (itcom1_1.level >= 0) {
	io___137.ciunit = itcom1_1.nout;
	s_wsfe(&io___137);
	do_fio(&c__1, (char *)&ier, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 390 >*/
    goto L390;
/*<   160 CALL PERVEC (N,RHS,IWKSP) >*/
L160:
    pervec_(&n, &rhs[1], &iwksp[1]);
/*<       CALL PERVEC (N,U,IWKSP) >*/
    pervec_(&n, &u[1], &iwksp[1]);

/* ... SCALE LINEAR SYSTEM, U, AND RHS BY THE SQUARE ROOT OF THE */
/* ... DIAGONAL ELEMENTS. */

/*<   170 CONTINUE >*/
L170:
/*<       CALL VFILL (IPARM(8),WKSP,0.0D0) >*/
    vfill_(&iparm[8], &wksp[1], &c_b21);
/*<       CALL SCAL (N,IA,JA,A,RHS,U,WKSP,LEVEL,NOUT,IER) >*/
    scal_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &u[1], &wksp[1], &
	    itcom1_1.level, &itcom1_1.nout, &ier);
/*<       IF (IER.EQ.0) GO TO 190 >*/
    if (ier == 0) {
	goto L190;
    }
/*<       IF (LEVEL.GE.0) WRITE (NOUT,180) IER >*/
    if (itcom1_1.level >= 0) {
	io___138.ciunit = itcom1_1.nout;
	s_wsfe(&io___138);
	do_fio(&c__1, (char *)&ier, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 390 >*/
    goto L390;
/*<   190 IF (LEVEL.LE.2) GO TO 220 >*/
L190:
    if (itcom1_1.level <= 2) {
	goto L220;
    }
/*<       WRITE (NOUT,200) >*/
    io___139.ciunit = itcom1_1.nout;
    s_wsfe(&io___139);
    e_wsfe();
/*<    >*/
/*<       WRITE (NOUT,210) >*/
    io___140.ciunit = itcom1_1.nout;
    s_wsfe(&io___140);
    e_wsfe();
/*<   210 FORMAT (1X,'S-PRIME IS AN INITIAL ESTIMATE FOR NEW CME') >*/
/*<   220 IF (IPARM(11).NE.0) GO TO 230 >*/
L220:
    if (iparm[11] != 0) {
	goto L230;
    }
/*<       TIMI1 = TIMER(DUMMY) >*/
    timi1 = timer_(&dummy);

/* ... SPECIAL PROCEDURE FOR FULLY ADAPTIVE CASE. */

/*<   230 CONTINUE >*/
L230:
/*<       IF (.NOT.ADAPT) GO TO 250 >*/
    if (! itcom2_1.adapt) {
	goto L250;
    }
/*<       IF (.NOT.BETADT) GO TO 240 >*/
    if (! itcom2_1.betadt) {
	goto L240;
    }
/*<       CALL VFILL (N,WKSP(IB1),1.D0) >*/
    vfill_(&n, &wksp[ib1], &c_b286);
/*<    >*/
    betnew = pbeta_(&n, &ia[1], &ja[1], &a[1], &wksp[ib1], &wksp[ib2], &wksp[
	    ib3]) / (doublereal) ((real) n);
/*<       BETAB = DMAX1(BETAB,.25D0,BETNEW) >*/
/* Computing MAX */
    d__1 = max(itcom3_1.betab,.25);
    itcom3_1.betab = max(d__1,betnew);
/*<   240 CALL OMEG (0.D0,1) >*/
L240:
    omeg_(&c_b21, &c__1);
/*<       IS = 0 >*/
    itcom1_1.is = 0;

/* ... INITIALIZE FORWARD PSEUDO-RESIDUAL */

/*<   250 CALL DCOPY (N,RHS,1,WKSP(IB1),1) >*/
L250:
    dcopy_(&n, &rhs[1], &c__1, &wksp[ib1], &c__1);
/*<       CALL DCOPY (N,U,1,WKSP(IB2),1) >*/
    dcopy_(&n, &u[1], &c__1, &wksp[ib2], &c__1);
/*<       CALL PFSOR (N,IA,JA,A,WKSP(IB2),WKSP(IB1)) >*/
    pfsor_(&n, &ia[1], &ja[1], &a[1], &wksp[ib2], &wksp[ib1]);
/*<       CALL VEVMW (N,WKSP(IB2),U) >*/
    vevmw_(&n, &wksp[ib2], &u[1]);

/* ... ITERATION SEQUENCE */

/*<       ITMAX1 = ITMAX+1 >*/
    itmax1 = itcom1_1.itmax + 1;
/*<       DO 270 LOOP = 1,ITMAX1 >*/
    i__1 = itmax1;
    for (loop = 1; loop <= i__1; ++loop) {
/*<          IN = LOOP-1 >*/
	itcom1_1.in = loop - 1;
/*<          IF (MOD(IN,2).EQ.1) GO TO 260 >*/
	if (itcom1_1.in % 2 == 1) {
	    goto L260;
	}

/* ... CODE FOR THE EVEN ITERATIONS. */

/*     U           = U(IN)       WKSP(IB2) = C(IN) */
/*     WKSP(IB1)   = U(IN-1)     WKSP(IB3) = C(IN-1) */

/*<    >*/
	itsrcg_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &u[1], &wksp[ib1], &wksp[
		ib2], &wksp[ib3], &wksp[ib4], &wksp[ib5], &wksp[ib6], &wksp[
		ib7]);

/*<          IF (HALT) GO TO 300 >*/
	if (itcom2_1.halt) {
	    goto L300;
	}
/*<          GO TO 270 >*/
	goto L270;

/* ... CODE FOR THE ODD ITERATIONS. */

/*     U           = U(IN-1)     WKSP(IB2) = C(IN-1) */
/*     WKSP(IB1)   = U(IN)       WKSP(IB3) =C(IN) */

/*<    >*/
L260:
	itsrcg_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &wksp[ib1], &u[1], &wksp[
		ib3], &wksp[ib2], &wksp[ib4], &wksp[ib5], &wksp[ib6], &wksp[
		ib7]);

/*<          IF (HALT) GO TO 300 >*/
	if (itcom2_1.halt) {
	    goto L300;
	}
/*<   270 CONTINUE >*/
L270:
	;
    }

/* ... ITMAX HAS BEEN REACHED */

/*<       IF (IPARM(11).NE.0) GO TO 280 >*/
    if (iparm[11] != 0) {
	goto L280;
    }
/*<       TIMI2 = TIMER(DUMMY) >*/
    timi2 = timer_(&dummy);
/*<       TIME1 = DBLE(TIMI2-TIMI1) >*/
    time1 = (doublereal) (timi2 - timi1);
/*<   280 IF (LEVEL.GE.1) WRITE (NOUT,290) ITMAX >*/
L280:
    if (itcom1_1.level >= 1) {
	io___146.ciunit = itcom1_1.nout;
	s_wsfe(&io___146);
	do_fio(&c__1, (char *)&itcom1_1.itmax, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       IER = 43 >*/
    ier = 43;
/*<       IF (IPARM(3).EQ.0) RPARM(1) = STPTST >*/
    if (iparm[3] == 0) {
	rparm[1] = itcom3_1.stptst;
    }
/*<       GO TO 330 >*/
    goto L330;

/* ... METHOD HAS CONVERGED */

/*<   300 IF (IPARM(11).NE.0) GO TO 310 >*/
L300:
    if (iparm[11] != 0) {
	goto L310;
    }
/*<       TIMI2 = TIMER(DUMMY) >*/
    timi2 = timer_(&dummy);
/*<       TIME1 = DBLE(TIMI2-TIMI1) >*/
    time1 = (doublereal) (timi2 - timi1);
/*<   310 IF (LEVEL.GE.1) WRITE (NOUT,320) IN >*/
L310:
    if (itcom1_1.level >= 1) {
	io___147.ciunit = itcom1_1.nout;
	s_wsfe(&io___147);
	do_fio(&c__1, (char *)&itcom1_1.in, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<   320 FORMAT (/1X,'SSORCG  HAS CONVERGED IN ',I5,' ITERATIONS') >*/

/* ... PUT SOLUTION INTO U IF NOT ALREADY THERE. */

/*<   330 CONTINUE >*/
L330:
/*<       IF (MOD(IN,2).EQ.1) CALL DCOPY (N,WKSP(IB1),1,U,1) >*/
    if (itcom1_1.in % 2 == 1) {
	dcopy_(&n, &wksp[ib1], &c__1, &u[1], &c__1);
    }

/* ... UNSCALE THE MATRIX, SOLUTION, AND RHS VECTORS. */

/*<       CALL UNSCAL (N,IA,JA,A,RHS,U,WKSP) >*/
    unscal_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &u[1], &wksp[1]);

/* ... UN-PERMUTE MATRIX,RHS, AND SOLUTION */

/*<       IF (IPARM(9).LT.0) GO TO 360 >*/
    if (iparm[9] < 0) {
	goto L360;
    }
/*<    >*/
    permat_(&n, &ia[1], &ja[1], &a[1], &iwksp[ib2], &iwksp[ib3], &
	    itcom1_1.isym, &itcom1_1.level, &itcom1_1.nout, &ierper);
/*<       IF (IERPER.EQ.0) GO TO 350 >*/
    if (ierper == 0) {
	goto L350;
    }
/*<       IF (LEVEL.GE.0) WRITE (NOUT,340) IERPER >*/
    if (itcom1_1.level >= 0) {
	io___149.ciunit = itcom1_1.nout;
	s_wsfe(&io___149);
	do_fio(&c__1, (char *)&ierper, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       IF (IER.EQ.0) IER = IERPER >*/
    if (ier == 0) {
	ier = ierper;
    }
/*<       GO TO 390 >*/
    goto L390;
/*<   350 CALL PERVEC (N,RHS,IWKSP(IB2)) >*/
L350:
    pervec_(&n, &rhs[1], &iwksp[ib2]);
/*<       CALL PERVEC (N,U,IWKSP(IB2)) >*/
    pervec_(&n, &u[1], &iwksp[ib2]);

/* ... OPTIONAL ERROR ANALYSIS */

/*<   360 IDGTS = IPARM(12) >*/
L360:
    idgts = iparm[12];
/*<       IF (IDGTS.LT.0) GO TO 370 >*/
    if (idgts < 0) {
	goto L370;
    }
/*<       IF (IPARM(2).LE.0) IDGTS = 0 >*/
    if (iparm[2] <= 0) {
	idgts = 0;
    }
/*<       CALL PERROR (N,IA,JA,A,RHS,U,WKSP,DIGIT1,DIGIT2,IDGTS) >*/
    perror_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &u[1], &wksp[1], &digit1, &
	    digit2, &idgts);

/* ... SET RETURN PARAMETERS IN IPARM AND RPARM */

/*<   370 IF (IPARM(11).NE.0) GO TO 380 >*/
L370:
    if (iparm[11] != 0) {
	goto L380;
    }
/*<       TIMJ2 = TIMER(DUMMY) >*/
    timj2 = timer_(&dummy);
/*<       TIME2 = DBLE(TIMJ2-TIMJ1) >*/
    time2 = (doublereal) (timj2 - timj1);
/*<   380 IPARM(8) = IPARM(8)-2*(ITMAX-IN) >*/
L380:
    iparm[8] -= itcom1_1.itmax - itcom1_1.in << 1;
/*<       IF (ISYM.NE.0) IPARM(8) = IPARM(8)-2*(ITMAX-IN) >*/
    if (itcom1_1.isym != 0) {
	iparm[8] -= itcom1_1.itmax - itcom1_1.in << 1;
    }
/*<       IF (IPARM(3).NE.0) GO TO 390 >*/
    if (iparm[3] != 0) {
	goto L390;
    }
/*<       IPARM(1) = IN >*/
    iparm[1] = itcom1_1.in;
/*<       IPARM(9) = NB >*/
    iparm[9] = nb;
/*<       RPARM(2) = CME >*/
    rparm[2] = itcom3_1.cme;
/*<       RPARM(3) = SME >*/
    rparm[3] = itcom3_1.sme;
/*<       RPARM(5) = OMEGA >*/
    rparm[5] = itcom3_1.omega;
/*<       RPARM(6) = SPECR >*/
    rparm[6] = itcom3_1.specr;
/*<       RPARM(7) = BETAB >*/
    rparm[7] = itcom3_1.betab;
/*<       RPARM(9) = TIME1 >*/
    rparm[9] = time1;
/*<       RPARM(10) = TIME2 >*/
    rparm[10] = time2;
/*<       RPARM(11) = DIGIT1 >*/
    rparm[11] = digit1;
/*<       RPARM(12) = DIGIT2 >*/
    rparm[12] = digit2;

/*<   390 CONTINUE >*/
L390:
/*<       IERR = IER >*/
    *ierr = ier;
/*<       IF (LEVEL.GE.3) CALL ECHALL (N,IA,JA,A,RHS,IPARM,RPARM,2) >*/
    if (itcom1_1.level >= 3) {
	echall_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &iparm[1], &rparm[1], &
		c__2);
    }

/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* ssorcg_ */

/*<    >*/
/* Subroutine */ int ssorsi_(integer *nn, integer *ia, integer *ja, 
	doublereal *a, doublereal *rhs, doublereal *u, integer *iwksp, 
	integer *nw, doublereal *wksp, integer *iparm, doublereal *rparm, 
	integer *ierr)
{
    /* Format strings */
    static char fmt_10[] = "(\0020\002///1x,\002BEGINNING OF ITPACK SOLUTION\
 MODULE  SSORSI\002)";
    static char fmt_40[] = "(\0020\002,\002*** W A R N I N G ************\
\002/\0020\002,\002    IN ITPACK ROUTINE SSORSI\002/\002 \002,\002    RPARM(\
1) =\002,d10.3,\002 (ZETA)\002/\002 \002,\002    A VALUE THIS SMALL MAY HIND\
ER CONVERGENCE \002/\002 \002,\002    SINCE MACHINE PRECISION DRELPR =\002,d\
10.3/\002 \002,\002    ZETA RESET TO \002,d10.3)";
    static char fmt_60[] = "(\0020\002,\002*** F A T A L     E R R O R *****\
*******\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE SSORSI \002/\002\
 \002,\002    INVALID MATRIX DIMENSION, N =\002,i8)";
    static char fmt_80[] = "(\0020\002,\002*** F A T A L     E R R O R *****\
*******\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE SSORSI \002/\002\
 \002,\002    ERROR DETECTED IN SUBROUTINE  SBELM \002/\002 \002,\002    WHI\
CH REMOVES ROWS AND COLUMNS OF SYSTEM \002/\002 \002,\002    WHEN DIAGONAL E\
NTRY TOO LARGE  \002/\002 \002,\002    IER = \002,i5,5x,\002 RPARM(8) = \002\
,d10.3,\002 (TOL)\002)";
    static char fmt_100[] = "(\0020\002,\002*** F A T A L     E R R O R ****\
********\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE SSORSI \002/\002\
 \002,\002    NOT ENOUGH WORKSPACE AT \002,i10/\002 \002,\002    SET IPARM(8\
) =\002,i10,\002 (NW)\002)";
    static char fmt_120[] = "(\0020\002,\002*** F A T A L     E R R O R ****\
********\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE SSORSI \002/\002\
 \002,\002    ERROR DETECTED IN SUBROUTINE  PRBNDX\002/\002 \002,\002    WHI\
CH COMPUTES THE RED-BLACK INDEXING\002/\002 \002,\002    IER = \002,i5,\002 \
IPARM(9) = \002,i5,\002 (NB)\002)";
    static char fmt_140[] = "(/10x,\002ORDER OF BLACK SUBSYSTEM = \002,i5\
,\002 (NB)\002)";
    static char fmt_150[] = "(\0020\002,\002*** F A T A L     E R R O R ****\
********\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE SSORSI \002/\002\
 \002,\002    ERROR DETECTED IN SUBROUTINE  PERMAT\002/\002 \002,\002    WHI\
CH DOES THE RED-BLACK PERMUTATION\002/\002 \002,\002    IER = \002,i5)";
    static char fmt_180[] = "(\0020\002,\002*** F A T A L     E R R O R ****\
********\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE SSORSI \002/\002\
 \002,\002    ERROR DETECTED IN SUBROUTINE  SCAL  \002/\002 \002,\002    WHI\
CH SCALES THE SYSTEM   \002/\002 \002,\002    IER = \002,i5)";
    static char fmt_200[] = "(///1x,\002IN THE FOLLOWING, RHO AND GAMMA AR\
E\002,\002 ACCELERATION PARAMETERS\002)";
    static char fmt_280[] = "(\0020\002,\002*** W A R N I N G ***********\
*\002/\0020\002,\002    IN ITPACK ROUTINE SSORSI\002/\002 \002,\002    FAILU\
RE TO CONVERGE IN\002,i5,\002 ITERATIONS\002)";
    static char fmt_310[] = "(/1x,\002SSORSI  HAS CONVERGED IN \002,i5,\002 \
ITERATIONS\002)";
    static char fmt_330[] = "(\0020\002,\002*** F A T A L     E R R O R ****\
********\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE SSORSI \002/\002\
 \002,\002    ERROR DETECTED IN SUBROUTINE  PERMAT\002/\002 \002,\002    WHI\
CH UNDOES THE RED-BLACK PERMUTATION   \002/\002 \002,\002    IER = \002,i5)";

    /* System generated locals */
    integer i__1;
    doublereal d__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), e_wsfe(), do_fio(integer *, char *, ftnlen);

    /* Local variables */
    integer n, n3, nb, ib1, ib2, ib3, ib4, ib5, ier;
    doublereal tol;
    extern /* Subroutine */ int scal_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *,
	     integer *, integer *), omeg_(doublereal *, integer *);
    doublereal temp;
    integer loop;
    doublereal time1, time2;
    real timi1, timj1, timi2, timj2;
    extern doublereal pbeta_(integer *, integer *, integer *, doublereal *, 
	    doublereal *, doublereal *, doublereal *);
    extern /* Subroutine */ int sbelm_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, integer *, doublereal *, doublereal *,
	     integer *, integer *, integer *, integer *);
    integer idgts;
    extern /* Subroutine */ int vfill_(integer *, doublereal *, doublereal *),
	     dcopy_(integer *, doublereal *, integer *, doublereal *, integer 
	    *);
    extern doublereal timer_(real *);
    real dummy;
    doublereal digit1, digit2;
    integer itmax1;
    extern /* Subroutine */ int echall_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, integer *, doublereal *, integer *);
    doublereal betnew;
    extern /* Subroutine */ int ivfill_(integer *, integer *, integer *);
    integer ierper;
    extern /* Subroutine */ int echout_(integer *, doublereal *, integer *), 
	    permat_(integer *, integer *, integer *, doublereal *, integer *, 
	    integer *, integer *, integer *, integer *, integer *), pervec_(
	    integer *, doublereal *, integer *), unscal_(integer *, integer *,
	     integer *, doublereal *, doublereal *, doublereal *, doublereal *
	    ), prbndx_(integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *), perror_(integer *, 
	    integer *, integer *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, integer *), itsrsi_(
	    integer *, integer *, integer *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *);

    /* Fortran I/O blocks */
    static cilist io___152 = { 0, 0, 0, fmt_10, 0 };
    static cilist io___158 = { 0, 0, 0, fmt_40, 0 };
    static cilist io___163 = { 0, 0, 0, fmt_60, 0 };
    static cilist io___165 = { 0, 0, 0, fmt_80, 0 };
    static cilist io___171 = { 0, 0, 0, fmt_100, 0 };
    static cilist io___174 = { 0, 0, 0, fmt_120, 0 };
    static cilist io___175 = { 0, 0, 0, fmt_140, 0 };
    static cilist io___176 = { 0, 0, 0, fmt_150, 0 };
    static cilist io___177 = { 0, 0, 0, fmt_180, 0 };
    static cilist io___178 = { 0, 0, 0, fmt_200, 0 };
    static cilist io___184 = { 0, 0, 0, fmt_280, 0 };
    static cilist io___185 = { 0, 0, 0, fmt_310, 0 };
    static cilist io___187 = { 0, 0, 0, fmt_330, 0 };



/*     ITPACK 2C MAIN SUBROUTINE  SSORSI  (SYMMETRIC SUCCESSIVE RELAX- */
/*                                         ATION SEMI-ITERATION) */
/*     EACH OF THE MAIN SUBROUTINES: */
/*           JCG, JSI, SOR, SSORCG, SSORSI, RSCG, RSSI */
/*     CAN BE USED INDEPENDENTLY OF THE OTHERS */

/* ... FUNCTION: */

/*          THIS SUBROUTINE, SSORSI, DRIVES THE  SYMMETRIC SOR-SI */
/*          ALGORITHM. */

/* ... PARAMETER LIST: */

/*          N      INPUT INTEGER.  DIMENSION OF THE MATRIX. (= NN) */
/*          IA,JA  INPUT INTEGER VECTORS.  THE TWO INTEGER ARRAYS OF */
/*                 THE SPARSE MATRIX REPRESENTATION. */
/*          A      INPUT D.P. VECTOR.  THE D.P. ARRAY OF THE SPARSE */
/*                 MATRIX REPRESENTATION. */
/*          RHS    INPUT D.P. VECTOR.  CONTAINS THE RIGHT HAND SIDE */
/*                 OF THE MATRIX PROBLEM. */
/*          U      INPUT/OUTPUT D.P. VECTOR.  ON INPUT, U CONTAINS THE */
/*                 INITIAL GUESS TO THE SOLUTION. ON OUTPUT, IT CONTAINS */
/*                 THE LATEST ESTIMATE TO THE SOLUTION. */
/*          IWKSP  INTEGER VECTOR WORKSPACE OF LENGTH 3*N */
/*          NW     INPUT INTEGER.  LENGTH OF AVAILABLE WKSP.  ON OUTPUT, */
/*                 IPARM(8) IS AMOUNT USED. */
/*          WKSP   D.P. VECTOR USED FOR WORKING SPACE.  SSORSI */
/*                 NEEDS THIS TO BE IN LENGTH AT LEAST  5*N */
/*          IPARM  INTEGER VECTOR OF LENGTH 12.  ALLOWS USER TO SPECIFY */
/*                 SOME INTEGER PARAMETERS WHICH AFFECT THE METHOD.  IF */
/*          RPARM  D.P. VECTOR OF LENGTH 12. ALLOWS USER TO SPECIFY SOME */
/*                 D.P. PARAMETERS WHICH AFFECT THE METHOD. */
/*          IER    OUTPUT INTEGER.  ERROR FLAG. (= IERR) */

/* ... SSORSI SUBPROGRAM REFERENCES: */

/*          FROM ITPACK    BISRCH, CHEBY, CHGSI, DFAULT, ECHALL, ECHOUT, */
/*                         ITERM, TIMER, ITSRSI, IVFILL, OMEG, */
/*                         OMGSTR, PARSI, PBETA, PERMAT, PERROR, */
/*                         PERVEC, PFSOR, PMULT, PRBNDX, PSSOR1, */
/*                         PSTOP, PVTBV, QSORT, SBELM, SCAL, DCOPY, */
/*                         DDOT, SUM3, TSTCHG, UNSCAL, VEVPW, VFILL, */
/*                         VOUT, WEVMW */
/*          SYSTEM         DABS, DLOG, DLOG10, DBLE(AMAX0), DMAX1, DBLE(F */
/*                         MOD, DSQRT */

/*     VERSION:  ITPACK 2C (MARCH 1982) */

/*     CODE WRITTEN BY:  DAVID KINCAID, ROGER GRIMES, JOHN RESPESS */
/*                       CENTER FOR NUMERICAL ANALYSIS */
/*                       UNIVERSITY OF TEXAS */
/*                       AUSTIN, TX  78712 */
/*                       (512) 471-1242 */

/*     FOR ADDITIONAL DETAILS ON THE */
/*          (A) SUBROUTINE SEE TOMS ARTICLE 1982 */
/*          (B) ALGORITHM  SEE CNA REPORT 150 */

/*     BASED ON THEORY BY:  DAVID YOUNG, DAVID KINCAID, LOU HAGEMAN */

/*     REFERENCE THE BOOK:  APPLIED ITERATIVE METHODS */
/*                          L. HAGEMAN, D. YOUNG */
/*                          ACADEMIC PRESS, 1981 */

/*     ************************************************** */
/*     *               IMPORTANT NOTE                   * */
/*     *                                                * */
/*     *      WHEN INSTALLING ITPACK ROUTINES ON A      * */
/*     *  DIFFERENT COMPUTER, RESET SOME OF THE VALUES  * */
/*     *  IN  SUBROUTNE DFAULT.   MOST IMPORTANT ARE    * */
/*     *                                                * */
/*     *   DRELPR      MACHINE RELATIVE PRECISION       * */
/*     *   RPARM(1)    STOPPING CRITERION               * */
/*     *                                                * */
/*     *   ALSO CHANGE SYSTEM-DEPENDENT ROUTINE         * */
/*     *   SECOND USED IN TIMER                         * */
/*     *                                                * */
/*     ************************************************** */

/*     SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER IA(1),JA(1),IWKSP(1),IPARM(12),NN,NW,IERR >*/
/*<       DOUBLE PRECISION A(1),RHS(NN),U(NN),WKSP(NW),RPARM(12) >*/

/*     SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER IB1,IB2,IB3,IB4,IB5,IDGTS,IER,IERPER,ITMAX1,LOOP,N,NB,N3 >*/
/*<       DOUBLE PRECISION BETNEW,DIGIT1,DIGIT2,PBETA,TEMP,TIME1,TIME2,TOL >*/

/* *** BEGIN: ITPACK COMMON */

/*<       INTEGER IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/
/*<       COMMON /ITCOM1/ IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/

/*<       LOGICAL ADAPT,BETADT,CASEII,HALT,PARTAD >*/
/*<       COMMON /ITCOM2/ ADAPT,BETADT,CASEII,HALT,PARTAD >*/

/*<    >*/
/*<    >*/

/* *** END  : ITPACK COMMON */

/* ... VARIABLES IN COMMON BLOCK - ITCOM1 */

/*     IN     - ITERATION NUMBER */
/*     ISYM   - SYMMETRIC/NONSYMMETRIC STORAGE FORMAT SWITCH */
/*     IS     - ITERATION NUMBER WHEN PARAMETERS LAST CHANGED */
/*     ITMAX  - MAXIMUM NUMBER OF ITERATIONS ALLOWED */
/*     LEVEL  - LEVEL OF OUTPUT CONTROL SWITCH */
/*     NOUT   - OUTPUT UNIT NUMBER */

/* ... VARIABLES IN COMMON BLOCK - ITCOM2 */

/*     ADAPT  - FULLY ADAPTIVE PROCEDURE SWITCH */
/*     BETADT - SWITCH FOR ADAPTIVE DETERMINATION OF BETA */
/*     CASEII - ADAPTIVE PROCEDURE CASE SWITCH */
/*     HALT   - STOPPING TEST SWITCH */
/*     PARTAD - PARTIALLY ADAPTIVE PROCEDURE SWITCH */

/* ... VARIABLES IN COMMON BLOCK - ITCOM3 */

/*     BDELNM - TWO NORM OF B TIMES DELTA-SUPER-N */
/*     BETAB  - ESTIMATE FOR THE SPECTRAL RADIUS OF LU MATRIX */
/*     CME    - ESTIMATE OF LARGEST EIGENVALUE */
/*     DELNNM - INNER PRODUCT OF PSEUDO-RESIDUAL AT ITERATION N */
/*     DELSNM - INNER PRODUCT OF PSEUDO-RESIDUAL AT ITERATION S */
/*     FF     - ADAPTIVE PROCEDURE DAMPING FACTOR */
/*     GAMMA  - ACCELERATION PARAMETER */
/*     OMEGA  - OVERRELAXATION PARAMETER FOR SOR AND SSOR */
/*     QA     - PSEUDO-RESIDUAL RATIO */
/*     QT     - VIRTUAL SPECTRAL RADIUS */
/*     RHO    - ACCELERATION PARAMETER */
/*     RRR    - ADAPTIVE PARAMETER */
/*     SIGE   - PARAMETER SIGMA-SUB-E */
/*     SME    - ESTIMATE OF SMALLEST EIGENVALUE */
/*     SPECR  - SPECTRAL RADIUS ESTIMATE FOR SSOR */
/*     DRELPR - MACHINE RELATIVE PRECISION */
/*     STPTST - STOPPING PARAMETER */
/*     UDNM   - TWO NORM OF U */
/*     ZETA   - STOPPING CRITERION */

/* ... INITIALIZE COMMON BLOCKS */

/*<       LEVEL = IPARM(2) >*/
    /* Parameter adjustments */
    --u;
    --rhs;
    --ia;
    --ja;
    --a;
    --iwksp;
    --wksp;
    --iparm;
    --rparm;

    /* Function Body */
    itcom1_1.level = iparm[2];
/*<       NOUT = IPARM(4) >*/
    itcom1_1.nout = iparm[4];
/*<       IF (IPARM(9).GE.0) IPARM(6) = 2 >*/
    if (iparm[9] >= 0) {
	iparm[6] = 2;
    }
/*<       IF (LEVEL.GE.1) WRITE (NOUT,10) >*/
    if (itcom1_1.level >= 1) {
	io___152.ciunit = itcom1_1.nout;
	s_wsfe(&io___152);
	e_wsfe();
    }
/*<    10 FORMAT ('0'///1X,'BEGINNING OF ITPACK SOLUTION MODULE  SSORSI') >*/
/*<       IER = 0 >*/
    ier = 0;
/*<       IF (IPARM(1).LE.0) RETURN >*/
    if (iparm[1] <= 0) {
	return 0;
    }
/*<       N = NN >*/
    n = *nn;
/*<       IF (IPARM(11).EQ.0) TIMJ1 = TIMER(DUMMY) >*/
    if (iparm[11] == 0) {
	timj1 = timer_(&dummy);
    }
/*<       IF (LEVEL.GE.3) GO TO 20 >*/
    if (itcom1_1.level >= 3) {
	goto L20;
    }
/*<       CALL ECHOUT (IPARM,RPARM,5) >*/
    echout_(&iparm[1], &rparm[1], &c__5);
/*<       GO TO 30 >*/
    goto L30;
/*<    20 CALL ECHALL (N,IA,JA,A,RHS,IPARM,RPARM,1) >*/
L20:
    echall_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &iparm[1], &rparm[1], &c__1);
/*<    30 TEMP = 5.0D2*DRELPR >*/
L30:
    temp = itcom3_1.drelpr * 500.;
/*<       IF (ZETA.GE.TEMP) GO TO 50 >*/
    if (itcom3_1.zeta >= temp) {
	goto L50;
    }
/*<       IF (LEVEL.GE.1) WRITE (NOUT,40) ZETA,DRELPR,TEMP >*/
    if (itcom1_1.level >= 1) {
	io___158.ciunit = itcom1_1.nout;
	s_wsfe(&io___158);
	do_fio(&c__1, (char *)&itcom3_1.zeta, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&itcom3_1.drelpr, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&temp, (ftnlen)sizeof(doublereal));
	e_wsfe();
    }
/*<    >*/
/*<       ZETA = TEMP >*/
    itcom3_1.zeta = temp;
/*<    50 CONTINUE >*/
L50:
/*<       TIME1 = RPARM(9) >*/
    time1 = rparm[9];
/*<       TIME2 = RPARM(10) >*/
    time2 = rparm[10];
/*<       DIGIT1 = RPARM(11) >*/
    digit1 = rparm[11];
/*<       DIGIT2 = RPARM(12) >*/
    digit2 = rparm[12];

/* ... VERIFY N */

/*<       IF (N.GT.0) GO TO 70 >*/
    if (n > 0) {
	goto L70;
    }
/*<       IER = 51 >*/
    ier = 51;
/*<       IF (LEVEL.GE.0) WRITE (NOUT,60) N >*/
    if (itcom1_1.level >= 0) {
	io___163.ciunit = itcom1_1.nout;
	s_wsfe(&io___163);
	do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 380 >*/
    goto L380;
/*<    70 CONTINUE >*/
L70:

/* ... REMOVE ROWS AND COLUMNS IF REQUESTED */

/*<       IF (IPARM(10).EQ.0) GO TO 90 >*/
    if (iparm[10] == 0) {
	goto L90;
    }
/*<       TOL = RPARM(8) >*/
    tol = rparm[8];
/*<       CALL IVFILL (N,IWKSP,0) >*/
    ivfill_(&n, &iwksp[1], &c__0);
/*<       CALL VFILL (N,WKSP,0.0D0) >*/
    vfill_(&n, &wksp[1], &c_b21);
/*<       CALL SBELM (N,IA,JA,A,RHS,IWKSP,WKSP,TOL,ISYM,LEVEL,NOUT,IER) >*/
    sbelm_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &iwksp[1], &wksp[1], &tol, &
	    itcom1_1.isym, &itcom1_1.level, &itcom1_1.nout, &ier);
/*<       IF (IER.EQ.0) GO TO 90 >*/
    if (ier == 0) {
	goto L90;
    }
/*<       IF (LEVEL.GE.0) WRITE (NOUT,80) IER,TOL >*/
    if (itcom1_1.level >= 0) {
	io___165.ciunit = itcom1_1.nout;
	s_wsfe(&io___165);
	do_fio(&c__1, (char *)&ier, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&tol, (ftnlen)sizeof(doublereal));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 380 >*/
    goto L380;

/* ... INITIALIZE WKSP BASE ADDRESSES. */

/*<    90 IB1 = 1 >*/
L90:
    ib1 = 1;
/*<       IB2 = IB1+N >*/
    ib2 = ib1 + n;
/*<       IB3 = IB2+N >*/
    ib3 = ib2 + n;
/*<       IB4 = IB3+N >*/
    ib4 = ib3 + n;
/*<       IB5 = IB4+N >*/
    ib5 = ib4 + n;
/*<       IPARM(8) = 5*N >*/
    iparm[8] = n * 5;
/*<       IF (NW.GE.IPARM(8)) GO TO 110 >*/
    if (*nw >= iparm[8]) {
	goto L110;
    }
/*<       IER = 52 >*/
    ier = 52;
/*<       IF (LEVEL.GE.0) WRITE (NOUT,100) NW,IPARM(8) >*/
    if (itcom1_1.level >= 0) {
	io___171.ciunit = itcom1_1.nout;
	s_wsfe(&io___171);
	do_fio(&c__1, (char *)&(*nw), (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&iparm[8], (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/

/* ... PERMUTE TO  RED-BLACK SYSTEM IF REQUESTED */

/*<   110 NB = IPARM(9) >*/
L110:
    nb = iparm[9];
/*<       IF (NB.LT.0) GO TO 170 >*/
    if (nb < 0) {
	goto L170;
    }
/*<       N3 = 3*N >*/
    n3 = n * 3;
/*<       CALL IVFILL (N3,IWKSP,0) >*/
    ivfill_(&n3, &iwksp[1], &c__0);
/*<       CALL PRBNDX (N,NB,IA,JA,IWKSP,IWKSP(IB2),LEVEL,NOUT,IER) >*/
    prbndx_(&n, &nb, &ia[1], &ja[1], &iwksp[1], &iwksp[ib2], &itcom1_1.level, 
	    &itcom1_1.nout, &ier);
/*<       IF (IER.EQ.0) GO TO 130 >*/
    if (ier == 0) {
	goto L130;
    }
/*<       IF (LEVEL.GE.0) WRITE (NOUT,120) IER,NB >*/
    if (itcom1_1.level >= 0) {
	io___174.ciunit = itcom1_1.nout;
	s_wsfe(&io___174);
	do_fio(&c__1, (char *)&ier, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&nb, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 380 >*/
    goto L380;

/* ... PERMUTE MATRIX AND RHS */

/*<   130 IF (LEVEL.GE.2) WRITE (NOUT,140) NB >*/
L130:
    if (itcom1_1.level >= 2) {
	io___175.ciunit = itcom1_1.nout;
	s_wsfe(&io___175);
	do_fio(&c__1, (char *)&nb, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<   140 FORMAT (/10X,'ORDER OF BLACK SUBSYSTEM = ',I5,' (NB)') >*/
/*<       CALL PERMAT (N,IA,JA,A,IWKSP,IWKSP(IB3),ISYM,LEVEL,NOUT,IER) >*/
    permat_(&n, &ia[1], &ja[1], &a[1], &iwksp[1], &iwksp[ib3], &itcom1_1.isym,
	     &itcom1_1.level, &itcom1_1.nout, &ier);
/*<       IF (IER.EQ.0) GO TO 160 >*/
    if (ier == 0) {
	goto L160;
    }
/*<       IF (LEVEL.GE.0) WRITE (NOUT,150) IER >*/
    if (itcom1_1.level >= 0) {
	io___176.ciunit = itcom1_1.nout;
	s_wsfe(&io___176);
	do_fio(&c__1, (char *)&ier, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 380 >*/
    goto L380;
/*<   160 CALL PERVEC (N,RHS,IWKSP) >*/
L160:
    pervec_(&n, &rhs[1], &iwksp[1]);
/*<       CALL PERVEC (N,U,IWKSP) >*/
    pervec_(&n, &u[1], &iwksp[1]);

/* ... SCALE LINEAR SYSTEM, U, AND RHS BY THE SQUARE ROOT OF THE */
/* ... DIAGONAL ELEMENTS. */

/*<   170 CONTINUE >*/
L170:
/*<       CALL VFILL (IPARM(8),WKSP,0.0D0) >*/
    vfill_(&iparm[8], &wksp[1], &c_b21);
/*<       CALL SCAL (N,IA,JA,A,RHS,U,WKSP,LEVEL,NOUT,IER) >*/
    scal_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &u[1], &wksp[1], &
	    itcom1_1.level, &itcom1_1.nout, &ier);
/*<       IF (IER.EQ.0) GO TO 190 >*/
    if (ier == 0) {
	goto L190;
    }
/*<       IF (LEVEL.GE.0) WRITE (NOUT,180) IER >*/
    if (itcom1_1.level >= 0) {
	io___177.ciunit = itcom1_1.nout;
	s_wsfe(&io___177);
	do_fio(&c__1, (char *)&ier, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 380 >*/
    goto L380;
/*<   190 IF (LEVEL.LE.2) GO TO 210 >*/
L190:
    if (itcom1_1.level <= 2) {
	goto L210;
    }
/*<       WRITE (NOUT,200) >*/
    io___178.ciunit = itcom1_1.nout;
    s_wsfe(&io___178);
    e_wsfe();
/*<    >*/
/*<   210 IF (IPARM(11).NE.0) GO TO 220 >*/
L210:
    if (iparm[11] != 0) {
	goto L220;
    }
/*<       TIMI1 = TIMER(DUMMY) >*/
    timi1 = timer_(&dummy);

/* ... SPECIAL PROCEDURE FOR FULLY ADAPTIVE CASE. */

/*<   220 CONTINUE >*/
L220:
/*<       IF (.NOT.ADAPT) GO TO 240 >*/
    if (! itcom2_1.adapt) {
	goto L240;
    }
/*<       IF (.NOT.BETADT) GO TO 230 >*/
    if (! itcom2_1.betadt) {
	goto L230;
    }
/*<       CALL VFILL (N,WKSP(IB1),1.D0) >*/
    vfill_(&n, &wksp[ib1], &c_b286);
/*<    >*/
    betnew = pbeta_(&n, &ia[1], &ja[1], &a[1], &wksp[ib1], &wksp[ib2], &wksp[
	    ib3]) / (doublereal) ((real) n);
/*<       BETAB = DMAX1(BETAB,.25D0,BETNEW) >*/
/* Computing MAX */
    d__1 = max(itcom3_1.betab,.25);
    itcom3_1.betab = max(d__1,betnew);
/*<   230 CALL OMEG (0.D0,1) >*/
L230:
    omeg_(&c_b21, &c__1);
/*<       IS = 0 >*/
    itcom1_1.is = 0;

/* ... ITERATION SEQUENCE */

/*<   240 ITMAX1 = ITMAX+1 >*/
L240:
    itmax1 = itcom1_1.itmax + 1;
/*<       DO 260 LOOP = 1,ITMAX1 >*/
    i__1 = itmax1;
    for (loop = 1; loop <= i__1; ++loop) {
/*<          IN = LOOP-1 >*/
	itcom1_1.in = loop - 1;
/*<          IF (MOD(IN,2).EQ.1) GO TO 250 >*/
	if (itcom1_1.in % 2 == 1) {
	    goto L250;
	}

/* ... CODE FOR THE EVEN ITERATIONS. */

/*     U           = U(IN) */
/*     WKSP(IB1)   = U(IN-1) */

/*<    >*/
	itsrsi_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &u[1], &wksp[ib1], &wksp[
		ib2], &wksp[ib3], &wksp[ib4], &wksp[ib5]);

/*<          IF (HALT) GO TO 290 >*/
	if (itcom2_1.halt) {
	    goto L290;
	}
/*<          GO TO 260 >*/
	goto L260;

/* ... CODE FOR THE ODD ITERATIONS. */

/*     U           = U(IN-1) */
/*     WKSP(IB1)   = U(IN) */

/*<    >*/
L250:
	itsrsi_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &wksp[ib1], &u[1], &wksp[
		ib2], &wksp[ib3], &wksp[ib4], &wksp[ib5]);

/*<          IF (HALT) GO TO 290 >*/
	if (itcom2_1.halt) {
	    goto L290;
	}
/*<   260 CONTINUE >*/
L260:
	;
    }

/* ... ITMAX HAS BEEN REACHED */

/*<       IF (IPARM(11).NE.0) GO TO 270 >*/
    if (iparm[11] != 0) {
	goto L270;
    }
/*<       TIMI2 = TIMER(DUMMY) >*/
    timi2 = timer_(&dummy);
/*<       TIME1 = DBLE(TIMI2-TIMI1) >*/
    time1 = (doublereal) (timi2 - timi1);
/*<   270 IF (LEVEL.GE.1) WRITE (NOUT,280) ITMAX >*/
L270:
    if (itcom1_1.level >= 1) {
	io___184.ciunit = itcom1_1.nout;
	s_wsfe(&io___184);
	do_fio(&c__1, (char *)&itcom1_1.itmax, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       IER = 53 >*/
    ier = 53;
/*<       IF (IPARM(3).EQ.0) RPARM(1) = STPTST >*/
    if (iparm[3] == 0) {
	rparm[1] = itcom3_1.stptst;
    }
/*<       GO TO 320 >*/
    goto L320;

/* ... METHOD HAS CONVERGED */

/*<   290 IF (IPARM(11).NE.0) GO TO 300 >*/
L290:
    if (iparm[11] != 0) {
	goto L300;
    }
/*<       TIMI2 = TIMER(DUMMY) >*/
    timi2 = timer_(&dummy);
/*<       TIME1 = DBLE(TIMI2-TIMI1) >*/
    time1 = (doublereal) (timi2 - timi1);
/*<   300 IF (LEVEL.GE.1) WRITE (NOUT,310) IN >*/
L300:
    if (itcom1_1.level >= 1) {
	io___185.ciunit = itcom1_1.nout;
	s_wsfe(&io___185);
	do_fio(&c__1, (char *)&itcom1_1.in, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<   310 FORMAT (/1X,'SSORSI  HAS CONVERGED IN ',I5,' ITERATIONS') >*/

/* ... PUT SOLUTION INTO U IF NOT ALREADY THERE. */

/*<   320 CONTINUE >*/
L320:
/*<       IF (MOD(IN,2).EQ.1) CALL DCOPY (N,WKSP(IB1),1,U,1) >*/
    if (itcom1_1.in % 2 == 1) {
	dcopy_(&n, &wksp[ib1], &c__1, &u[1], &c__1);
    }

/* ... UNSCALE THE MATRIX, SOLUTION, AND RHS VECTORS. */

/*<       CALL UNSCAL (N,IA,JA,A,RHS,U,WKSP) >*/
    unscal_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &u[1], &wksp[1]);

/* ... UN-PERMUTE MATRIX,RHS, AND SOLUTION */

/*<       IF (IPARM(9).LT.0) GO TO 350 >*/
    if (iparm[9] < 0) {
	goto L350;
    }
/*<    >*/
    permat_(&n, &ia[1], &ja[1], &a[1], &iwksp[ib2], &iwksp[ib3], &
	    itcom1_1.isym, &itcom1_1.level, &itcom1_1.nout, &ierper);
/*<       IF (IERPER.EQ.0) GO TO 340 >*/
    if (ierper == 0) {
	goto L340;
    }
/*<       IF (LEVEL.GE.0) WRITE (NOUT,330) IERPER >*/
    if (itcom1_1.level >= 0) {
	io___187.ciunit = itcom1_1.nout;
	s_wsfe(&io___187);
	do_fio(&c__1, (char *)&ierper, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       IF (IER.EQ.0) IER = IERPER >*/
    if (ier == 0) {
	ier = ierper;
    }
/*<       GO TO 380 >*/
    goto L380;
/*<   340 CALL PERVEC (N,RHS,IWKSP(IB2)) >*/
L340:
    pervec_(&n, &rhs[1], &iwksp[ib2]);
/*<       CALL PERVEC (N,U,IWKSP(IB2)) >*/
    pervec_(&n, &u[1], &iwksp[ib2]);

/* ... OPTIONAL ERROR ANALYSIS */

/*<   350 IDGTS = IPARM(12) >*/
L350:
    idgts = iparm[12];
/*<       IF (IDGTS.LT.0) GO TO 360 >*/
    if (idgts < 0) {
	goto L360;
    }
/*<       IF (IPARM(2).LE.0) IDGTS = 0 >*/
    if (iparm[2] <= 0) {
	idgts = 0;
    }
/*<       CALL PERROR (N,IA,JA,A,RHS,U,WKSP,DIGIT1,DIGIT2,IDGTS) >*/
    perror_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &u[1], &wksp[1], &digit1, &
	    digit2, &idgts);

/* ... SET RETURN PARAMETERS IN IPARM AND RPARM */

/*<   360 IF (IPARM(11).NE.0) GO TO 370 >*/
L360:
    if (iparm[11] != 0) {
	goto L370;
    }
/*<       TIMJ2 = TIMER(DUMMY) >*/
    timj2 = timer_(&dummy);
/*<       TIME2 = DBLE(TIMJ2-TIMJ1) >*/
    time2 = (doublereal) (timj2 - timj1);
/*<   370 IF (IPARM(3).NE.0) GO TO 380 >*/
L370:
    if (iparm[3] != 0) {
	goto L380;
    }
/*<       IPARM(1) = IN >*/
    iparm[1] = itcom1_1.in;
/*<       IPARM(9) = NB >*/
    iparm[9] = nb;
/*<       RPARM(2) = CME >*/
    rparm[2] = itcom3_1.cme;
/*<       RPARM(3) = SME >*/
    rparm[3] = itcom3_1.sme;
/*<       RPARM(5) = OMEGA >*/
    rparm[5] = itcom3_1.omega;
/*<       RPARM(6) = SPECR >*/
    rparm[6] = itcom3_1.specr;
/*<       RPARM(7) = BETAB >*/
    rparm[7] = itcom3_1.betab;
/*<       RPARM(9) = TIME1 >*/
    rparm[9] = time1;
/*<       RPARM(10) = TIME2 >*/
    rparm[10] = time2;
/*<       RPARM(11) = DIGIT1 >*/
    rparm[11] = digit1;
/*<       RPARM(12) = DIGIT2 >*/
    rparm[12] = digit2;

/*<   380 CONTINUE >*/
L380:
/*<       IERR = IER >*/
    *ierr = ier;
/*<       IF (LEVEL.GE.3) CALL ECHALL (N,IA,JA,A,RHS,IPARM,RPARM,2) >*/
    if (itcom1_1.level >= 3) {
	echall_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &iparm[1], &rparm[1], &
		c__2);
    }

/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* ssorsi_ */

/*<       SUBROUTINE RSCG (NN,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IERR) >*/
/* Subroutine */ int rscg_(integer *nn, integer *ia, integer *ja, doublereal *
	a, doublereal *rhs, doublereal *u, integer *iwksp, integer *nw, 
	doublereal *wksp, integer *iparm, doublereal *rparm, integer *ierr)
{
    /* Format strings */
    static char fmt_10[] = "(\0020\002///1x,\002BEGINNING OF ITPACK SOLUTION\
 MODULE  RSCG\002)";
    static char fmt_40[] = "(\0020\002,\002*** W A R N I N G ************\
\002/\0020\002,\002    IN ITPACK ROUTINE RSCG\002/\002 \002,\002    RPARM(1)\
 =\002,d10.3,\002 (ZETA)\002/\002 \002,\002    A VALUE THIS SMALL MAY HINDER\
 CONVERGENCE \002/\002 \002,\002    SINCE MACHINE PRECISION DRELPR =\002,d10\
.3/\002 \002,\002    ZETA RESET TO \002,d10.3)";
    static char fmt_60[] = "(\0020\002,\002*** F A T A L     E R R O R *****\
*******\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE RSCG \002/\002 \002,\
\002    INVALID MATRIX DIMENSION, N =\002,i8)";
    static char fmt_80[] = "(\0020\002,\002*** F A T A L     E R R O R *****\
*******\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE RSCG \002/\002 \002,\
\002    ERROR DETECTED IN SUBROUTINE  SBELM \002/\002 \002,\002    WHICH REM\
OVES ROWS AND COLUMNS OF SYSTEM \002/\002 \002,\002    WHEN DIAGONAL ENTRY T\
OO LARGE  \002/\002 \002,\002    IER = \002,i5,5x,\002 RPARM(8) = \002,d10.3,\
\002 (TOL)\002)";
    static char fmt_100[] = "(\0020\002,\002*** F A T A L     E R R O R ****\
********\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE RSCG \002/\002 \
\002,\002    ERROR DETECTED IN SUBROUTINE  PRBNDX\002/\002 \002,\002    WHIC\
H COMPUTES THE RED-BLACK INDEXING\002/\002 \002,\002    IER = \002,i5,\002 I\
PARM(9) = \002,i5,\002 (NB)\002)";
    static char fmt_120[] = "(/10x,\002ERROR DETECTED IN RED-BLACK SUBSYSTEM\
 INDEX\002/10x,\002IER =\002,i5,\002 IPARM(9) =\002,i5,\002 (NB)\002)";
    static char fmt_140[] = "(/10x,\002 IPARM(9) = \002,i5,\002 IMPLIES MATR\
IX IS DIAGONAL\002/10x,\002 NB RESET TO \002,i5)";
    static char fmt_160[] = "(/10x,\002ORDER OF BLACK SUBSYSTEM = \002,i5\
,\002 (NB)\002)";
    static char fmt_170[] = "(\0020\002,\002*** F A T A L     E R R O R ****\
********\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE RSCG \002/\002 \
\002,\002    ERROR DETECTED IN SUBROUTINE  PERMAT\002/\002 \002,\002    WHIC\
H DOES THE RED-BLACK PERMUTATION\002/\002 \002,\002    IER = \002,i5)";
    static char fmt_200[] = "(\0020\002,\002*** F A T A L     E R R O R ****\
********\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE RSCG \002/\002 \
\002,\002    NOT ENOUGH WORKSPACE AT \002,i10/\002 \002,\002    SET IPARM(8)\
 =\002,i10,\002 (NW)\002)";
    static char fmt_220[] = "(\0020\002,\002*** F A T A L     E R R O R ****\
********\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE RSCG \002/\002 \
\002,\002    ERROR DETECTED IN SUBROUTINE  SCAL  \002/\002 \002,\002    WHIC\
H SCALES THE SYSTEM   \002/\002 \002,\002    IER = \002,i5)";
    static char fmt_240[] = "(///1x,\002IN THE FOLLOWING, RHO AND GAMMA AR\
E\002,\002 ACCELERATION PARAMETERS\002)";
    static char fmt_250[] = "(1x,\002CME IS THE ESTIMATE OF THE LARGEST EIGE\
NVALUE OF\002,\002 THE JACOBI MATRIX\002)";
    static char fmt_320[] = "(\0020\002,\002*** W A R N I N G ***********\
*\002/\0020\002,\002    IN ITPACK ROUTINE RSCG\002/\002 \002,\002    FAILURE\
 TO CONVERGE IN\002,i5,\002 ITERATIONS\002)";
    static char fmt_350[] = "(/1x,\002RSCG  HAS CONVERGED IN \002,i5,\002 IT\
ERATIONS\002)";
    static char fmt_380[] = "(\0020\002,\002*** F A T A L     E R R O R ****\
********\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE RSCG \002/\002 \
\002,\002    ERROR DETECTED IN SUBROUTINE  PERMAT\002/\002 \002,\002    WHIC\
H UNDOES THE RED-BLACK PERMUTATION   \002/\002 \002,\002    IER = \002,i5)";

    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), e_wsfe(), do_fio(integer *, char *, ftnlen);

    /* Local variables */
    integer n, n3, nb, nr, ib1, ib2, ib3, ib4, ib5, jb3, ier;
    doublereal tol;
    integer nrp1;
    extern /* Subroutine */ int scal_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *,
	     integer *, integer *);
    doublereal temp;
    integer loop;
    doublereal time1, time2;
    real timi1, timj1, timi2, timj2;
    extern /* Subroutine */ int sbelm_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, integer *, doublereal *, doublereal *,
	     integer *, integer *, integer *, integer *);
    integer idgts;
    extern /* Subroutine */ int vfill_(integer *, doublereal *, doublereal *),
	     dcopy_(integer *, doublereal *, integer *, doublereal *, integer 
	    *);
    extern doublereal timer_(real *);
    real dummy;
    extern /* Subroutine */ int vevmw_(integer *, doublereal *, doublereal *);
    doublereal digit1, digit2;
    integer itmax1;
    extern /* Subroutine */ int echall_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, integer *, doublereal *, integer *), 
	    pervec_(integer *, doublereal *, integer *), ivfill_(integer *, 
	    integer *, integer *);
    integer ierper;
    extern /* Subroutine */ int echout_(integer *, doublereal *, integer *), 
	    permat_(integer *, integer *, integer *, doublereal *, integer *, 
	    integer *, integer *, integer *, integer *, integer *), unscal_(
	    integer *, integer *, integer *, doublereal *, doublereal *, 
	    doublereal *, doublereal *), itrscg_(integer *, integer *, 
	    integer *, integer *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *), prbndx_(
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *), prsblk_(integer *, integer *, 
	    integer *, integer *, doublereal *, doublereal *, doublereal *), 
	    prsred_(integer *, integer *, integer *, integer *, doublereal *, 
	    doublereal *, doublereal *), perror_(integer *, integer *, 
	    integer *, doublereal *, doublereal *, doublereal *, doublereal *,
	     doublereal *, doublereal *, integer *);

    /* Fortran I/O blocks */
    static cilist io___190 = { 0, 0, 0, fmt_10, 0 };
    static cilist io___196 = { 0, 0, 0, fmt_40, 0 };
    static cilist io___201 = { 0, 0, 0, fmt_60, 0 };
    static cilist io___203 = { 0, 0, 0, fmt_80, 0 };
    static cilist io___209 = { 0, 0, 0, fmt_100, 0 };
    static cilist io___210 = { 0, 0, 0, fmt_120, 0 };
    static cilist io___211 = { 0, 0, 0, fmt_140, 0 };
    static cilist io___212 = { 0, 0, 0, fmt_160, 0 };
    static cilist io___213 = { 0, 0, 0, fmt_170, 0 };
    static cilist io___219 = { 0, 0, 0, fmt_200, 0 };
    static cilist io___220 = { 0, 0, 0, fmt_220, 0 };
    static cilist io___221 = { 0, 0, 0, fmt_240, 0 };
    static cilist io___222 = { 0, 0, 0, fmt_250, 0 };
    static cilist io___227 = { 0, 0, 0, fmt_320, 0 };
    static cilist io___228 = { 0, 0, 0, fmt_350, 0 };
    static cilist io___230 = { 0, 0, 0, fmt_380, 0 };



/*     ITPACK 2C MAIN SUBROUTINE  RSCG  (REDUCED SYSTEM CONJUGATE */
/*                                       GRADIENT) */
/*     EACH OF THE MAIN SUBROUTINES: */
/*           JCG, JSI, SOR, SSORCG, SSORSI, RSCG, RSSI */
/*     CAN BE USED INDEPENDENTLY OF THE OTHERS */

/* ... FUNCTION: */

/*          THIS SUBROUTINE, RSCG, DRIVES THE  REDUCED SYSTEM CG */
/*          ALGORITHM. */

/* ... PARAMETER LIST: */

/*          N     INPUT INTEGER.  DIMENSION OF THE MATRIX. (= NN) */
/*                 IN THE RED-BLACK MATRIX. */
/*          IA,JA  INPUT INTEGER VECTORS.  THE TWO INTEGER ARRAYS OF */
/*                 THE SPARSE MATRIX REPRESENTATION. */
/*          A      INPUT D.P. VECTOR.  THE D.P. ARRAY OF THE SPARSE */
/*                 MATRIX REPRESENTATION. */
/*          RHS    INPUT D.P. VECTOR.  CONTAINS THE RIGHT HAND SIDE */
/*                 OF THE MATRIX PROBLEM. */
/*          U      INPUT/OUTPUT D.P. VECTOR.  ON INPUT, U CONTAINS THE */
/*                 INITIAL GUESS TO THE SOLUTION. ON OUTPUT, IT CONTAINS */
/*                 THE LATEST ESTIMATE TO THE SOLUTION. */
/*          IWKSP  INTEGER VECTOR WORKSPACE OF LENGTH 3*N */
/*          NW     INPUT INTEGER.  LENGTH OF AVAILABLE WKSP.  ON OUTPUT, */
/*                 IPARM(8) IS AMOUNT USED. */
/*          WKSP   D.P. VECTOR USED FOR WORKING SPACE.  RSCG NEEDS */
/*                 THIS TO BE IN LENGTH AT LEAST */
/*                 N+3*NB+2*ITMAX, IF IPARM(5)=0  (SYMMETRIC STORAGE) */
/*                 N+3*NB+4*ITMAX, IF IPARM(5)=1  (NONSYMMETRIC STORAGE) */
/*                 HERE NB IS THE ORDER OF THE BLACK SUBSYSTEM */
/*          IPARM  INTEGER VECTOR OF LENGTH 12.  ALLOWS USER TO SPECIFY */
/*                 SOME INTEGER PARAMETERS WHICH AFFECT THE METHOD.  IF */
/*          RPARM  D.P. VECTOR OF LENGTH 12. ALLOWS USER TO SPECIFY SOME */
/*                 D.P. PARAMETERS WHICH AFFECT THE METHOD. */
/*          IER    OUTPUT INTEGER. ERROR FLAG. (= IERR) */

/* ... RSCG SUBPROGRAM REFERENCES: */

/*          FROM ITPACK    BISRCH, CHGCON, DETERM, DFAULT, ECHALL, */
/*                         ECHOUT, EIGVNS, EIGVSS, EQRT1S, ITERM, TIMER */
/*                         ITRSCG, IVFILL, PARCON, PERMAT, */
/*                         PERROR, PERVEC, PMULT, PRBNDX, PRSBLK, */
/*                         PRSRED, PSTOP, QSORT, SBELM, SCAL, DCOPY, */
/*                         DDOT, SUM3, UNSCAL, VFILL, VOUT, WEVMW, */
/*                         ZBRENT */
/*          SYSTEM         DABS, DLOG10, DBLE(AMAX0), DMAX1, MOD, DSQRT */

/*     VERSION:  ITPACK 2C (MARCH 1982) */

/*     CODE WRITTEN BY:  DAVID KINCAID, ROGER GRIMES, JOHN RESPESS */
/*                       CENTER FOR NUMERICAL ANALYSIS */
/*                       UNIVERSITY OF TEXAS */
/*                       AUSTIN, TX  78712 */
/*                       (512) 471-1242 */

/*     FOR ADDITIONAL DETAILS ON THE */
/*          (A) SUBROUTINE SEE TOMS ARTICLE 1982 */
/*          (B) ALGORITHM  SEE CNA REPORT 150 */

/*     BASED ON THEORY BY:  DAVID YOUNG, DAVID KINCAID, LOU HAGEMAN */

/*     REFERENCE THE BOOK:  APPLIED ITERATIVE METHODS */
/*                          L. HAGEMAN, D. YOUNG */
/*                          ACADEMIC PRESS, 1981 */

/*     ************************************************** */
/*     *               IMPORTANT NOTE                   * */
/*     *                                                * */
/*     *      WHEN INSTALLING ITPACK ROUTINES ON A      * */
/*     *  DIFFERENT COMPUTER, RESET SOME OF THE VALUES  * */
/*     *  IN  SUBROUTNE DFAULT.   MOST IMPORTANT ARE    * */
/*     *                                                * */
/*     *   DRELPR      MACHINE RELATIVE PRECISION       * */
/*     *   RPARM(1)    STOPPING CRITERION               * */
/*     *                                                * */
/*     *   ALSO CHANGE SYSTEM-DEPENDENT ROUTINE         * */
/*     *   SECOND USED IN TIMER                         * */
/*     *                                                * */
/*     ************************************************** */

/*     SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER IA(1),JA(1),IWKSP(1),IPARM(12),NN,NW,IERR >*/
/*<       DOUBLE PRECISION A(1),RHS(NN),U(NN),WKSP(NW),RPARM(12) >*/

/*     SPECIFICATIONS FOR LOCAL VARIABLES */

/*<    >*/
/*<       DOUBLE PRECISION DIGIT1,DIGIT2,TEMP,TIME1,TIME2,TOL >*/

/* *** BEGIN: ITPACK COMMON */

/*<       INTEGER IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/
/*<       COMMON /ITCOM1/ IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/

/*<       LOGICAL ADAPT,BETADT,CASEII,HALT,PARTAD >*/
/*<       COMMON /ITCOM2/ ADAPT,BETADT,CASEII,HALT,PARTAD >*/

/*<    >*/
/*<    >*/

/* *** END  : ITPACK COMMON */

/* ... VARIABLES IN COMMON BLOCK - ITCOM1 */

/*     IN     - ITERATION NUMBER */
/*     IS     - ITERATION NUMBER WHEN PARAMETERS LAST CHANGED */
/*     ISYM   - SYMMETRIC/NONSYMMETRIC STORAGE FORMAT SWITCH */
/*     ITMAX  - MAXIMUM NUMBER OF ITERATIONS ALLOWED */
/*     LEVEL  - LEVEL OF OUTPUT CONTROL SWITCH */
/*     NOUT   - OUTPUT UNIT NUMBER */

/* ... VARIABLES IN COMMON BLOCK - ITCOM2 */

/*     ADAPT  - FULLY ADAPTIVE PROCEDURE SWITCH */
/*     BETADT - SWITCH FOR ADAPTIVE DETERMINATION OF BETA */
/*     CASEII - ADAPTIVE PROCEDURE CASE SWITCH */
/*     HALT   - STOPPING TEST SWITCH */
/*     PARTAD - PARTIALLY ADAPTIVE PROCEDURE SWITCH */

/* ... VARIABLES IN COMMON BLOCK - ITCOM3 */

/*     BDELNM - TWO NORM OF B TIMES DELTA-SUPER-N */
/*     BETAB  - ESTIMATE FOR THE SPECTRAL RADIUS OF LU MATRIX */
/*     CME    - ESTIMATE OF LARGEST EIGENVALUE */
/*     DELNNM - INNER PRODUCT OF PSEUDO-RESIDUAL AT ITERATION N */
/*     DELSNM - INNER PRODUCT OF PSEUDO-RESIDUAL AT ITERATION S */
/*     FF     - ADAPTIVE PROCEDURE DAMPING FACTOR */
/*     GAMMA  - ACCELERATION PARAMETER */
/*     OMEGA  - OVERRELAXATION PARAMETER FOR SOR AND SSOR */
/*     QA     - PSEUDO-RESIDUAL RATIO */
/*     QT     - VIRTUAL SPECTRAL RADIUS */
/*     RHO    - ACCELERATION PARAMETER */
/*     RRR    - ADAPTIVE PARAMETER */
/*     SIGE   - PARAMETER SIGMA-SUB-E */
/*     SME    - ESTIMATE OF SMALLEST EIGENVALUE */
/*     SPECR  - SPECTRAL RADIUS ESTIMATE FOR SSOR */
/*     DRELPR - MACHINE RELATIVE PRECISION */
/*     STPTST - STOPPING PARAMETER */
/*     UDNM   - TWO NORM OF U */
/*     ZETA   - STOPPING CRITERION */

/* ... INITIALIZE COMMON BLOCKS */

/*<       LEVEL = IPARM(2) >*/
    /* Parameter adjustments */
    --u;
    --rhs;
    --ia;
    --ja;
    --a;
    --iwksp;
    --wksp;
    --iparm;
    --rparm;

    /* Function Body */
    itcom1_1.level = iparm[2];
/*<       NOUT = IPARM(4) >*/
    itcom1_1.nout = iparm[4];
/*<       IF (LEVEL.GE.1) WRITE (NOUT,10) >*/
    if (itcom1_1.level >= 1) {
	io___190.ciunit = itcom1_1.nout;
	s_wsfe(&io___190);
	e_wsfe();
    }
/*<    10 FORMAT ('0'///1X,'BEGINNING OF ITPACK SOLUTION MODULE  RSCG') >*/
/*<       IER = 0 >*/
    ier = 0;
/*<       IF (IPARM(1).LE.0) RETURN >*/
    if (iparm[1] <= 0) {
	return 0;
    }
/*<       N = NN >*/
    n = *nn;
/*<       IF (IPARM(11).EQ.0) TIMJ1 = TIMER(DUMMY) >*/
    if (iparm[11] == 0) {
	timj1 = timer_(&dummy);
    }
/*<       IF (LEVEL.GE.3) GO TO 20 >*/
    if (itcom1_1.level >= 3) {
	goto L20;
    }
/*<       CALL ECHOUT (IPARM,RPARM,6) >*/
    echout_(&iparm[1], &rparm[1], &c__6);
/*<       GO TO 30 >*/
    goto L30;
/*<    20 CALL ECHALL (N,IA,JA,A,RHS,IPARM,RPARM,1) >*/
L20:
    echall_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &iparm[1], &rparm[1], &c__1);
/*<    30 TEMP = 5.0D2*DRELPR >*/
L30:
    temp = itcom3_1.drelpr * 500.;
/*<       IF (ZETA.GE.TEMP) GO TO 50 >*/
    if (itcom3_1.zeta >= temp) {
	goto L50;
    }
/*<       IF (LEVEL.GE.1) WRITE (NOUT,40) ZETA,DRELPR,TEMP >*/
    if (itcom1_1.level >= 1) {
	io___196.ciunit = itcom1_1.nout;
	s_wsfe(&io___196);
	do_fio(&c__1, (char *)&itcom3_1.zeta, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&itcom3_1.drelpr, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&temp, (ftnlen)sizeof(doublereal));
	e_wsfe();
    }
/*<    >*/
/*<       ZETA = TEMP >*/
    itcom3_1.zeta = temp;
/*<    50 CONTINUE >*/
L50:
/*<       TIME1 = RPARM(9) >*/
    time1 = rparm[9];
/*<       TIME2 = RPARM(10) >*/
    time2 = rparm[10];
/*<       DIGIT1 = RPARM(11) >*/
    digit1 = rparm[11];
/*<       DIGIT2 = RPARM(12) >*/
    digit2 = rparm[12];

/* ... VERIFY N */

/*<       IF (N.GT.0) GO TO 70 >*/
    if (n > 0) {
	goto L70;
    }
/*<       IER = 61 >*/
    ier = 61;
/*<       IF (LEVEL.GE.0) WRITE (NOUT,60) N >*/
    if (itcom1_1.level >= 0) {
	io___201.ciunit = itcom1_1.nout;
	s_wsfe(&io___201);
	do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 430 >*/
    goto L430;
/*<    70 CONTINUE >*/
L70:

/* ... REMOVE ROWS AND COLUMNS IF REQUESTED */

/*<       IF (IPARM(10).EQ.0) GO TO 90 >*/
    if (iparm[10] == 0) {
	goto L90;
    }
/*<       TOL = RPARM(8) >*/
    tol = rparm[8];
/*<       CALL IVFILL (N,IWKSP,0) >*/
    ivfill_(&n, &iwksp[1], &c__0);
/*<       CALL VFILL (N,WKSP,0.0D0) >*/
    vfill_(&n, &wksp[1], &c_b21);
/*<       CALL SBELM (N,IA,JA,A,RHS,IWKSP,WKSP,TOL,ISYM,LEVEL,NOUT,IER) >*/
    sbelm_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &iwksp[1], &wksp[1], &tol, &
	    itcom1_1.isym, &itcom1_1.level, &itcom1_1.nout, &ier);
/*<       IF (IER.EQ.0) GO TO 90 >*/
    if (ier == 0) {
	goto L90;
    }
/*<       IF (LEVEL.GE.0) WRITE (NOUT,80) IER,TOL >*/
    if (itcom1_1.level >= 0) {
	io___203.ciunit = itcom1_1.nout;
	s_wsfe(&io___203);
	do_fio(&c__1, (char *)&ier, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&tol, (ftnlen)sizeof(doublereal));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 430 >*/
    goto L430;

/* ... INITIALIZE WKSP BASE ADDRESSES. */

/*<    90 IB1 = 1 >*/
L90:
    ib1 = 1;
/*<       IB2 = IB1+N >*/
    ib2 = ib1 + n;
/*<       JB3 = IB2+N >*/
    jb3 = ib2 + n;

/* ... PERMUTE TO  RED-BLACK SYSTEM IF POSSIBLE */

/*<       NB = IPARM(9) >*/
    nb = iparm[9];
/*<       IF (NB.GE.0) GO TO 110 >*/
    if (nb >= 0) {
	goto L110;
    }
/*<       N3 = 3*N >*/
    n3 = n * 3;
/*<       CALL IVFILL (N3,IWKSP,0) >*/
    ivfill_(&n3, &iwksp[1], &c__0);
/*<       CALL PRBNDX (N,NB,IA,JA,IWKSP,IWKSP(IB2),LEVEL,NOUT,IER) >*/
    prbndx_(&n, &nb, &ia[1], &ja[1], &iwksp[1], &iwksp[ib2], &itcom1_1.level, 
	    &itcom1_1.nout, &ier);
/*<       IF (IER.EQ.0) GO TO 110 >*/
    if (ier == 0) {
	goto L110;
    }
/*<       IF (LEVEL.GE.0) WRITE (NOUT,100) IER,NB >*/
    if (itcom1_1.level >= 0) {
	io___209.ciunit = itcom1_1.nout;
	s_wsfe(&io___209);
	do_fio(&c__1, (char *)&ier, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&nb, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 430 >*/
    goto L430;
/*<   110 IF (NB.GE.0.AND.NB.LE.N) GO TO 130 >*/
L110:
    if (nb >= 0 && nb <= n) {
	goto L130;
    }
/*<       IER = 64 >*/
    ier = 64;
/*<       IF (LEVEL.GE.1) WRITE (NOUT,120) IER,NB >*/
    if (itcom1_1.level >= 1) {
	io___210.ciunit = itcom1_1.nout;
	s_wsfe(&io___210);
	do_fio(&c__1, (char *)&ier, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&nb, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 430 >*/
    goto L430;
/*<   130 IF (NB.NE.0.AND.NB.NE.N) GO TO 150 >*/
L130:
    if (nb != 0 && nb != n) {
	goto L150;
    }
/*<       NB = N/2 >*/
    nb = n / 2;
/*<       IF (LEVEL.GE.2.AND.IPARM(9).GE.0) WRITE (NOUT,140) IPARM(9),NB >*/
    if (itcom1_1.level >= 2 && iparm[9] >= 0) {
	io___211.ciunit = itcom1_1.nout;
	s_wsfe(&io___211);
	do_fio(&c__1, (char *)&iparm[9], (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&nb, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/

/* ... PERMUTE MATRIX AND RHS */

/*<   150 IF (IPARM(9).GE.0) GO TO 190 >*/
L150:
    if (iparm[9] >= 0) {
	goto L190;
    }
/*<       IF (LEVEL.GE.2) WRITE (NOUT,160) NB >*/
    if (itcom1_1.level >= 2) {
	io___212.ciunit = itcom1_1.nout;
	s_wsfe(&io___212);
	do_fio(&c__1, (char *)&nb, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<   160 FORMAT (/10X,'ORDER OF BLACK SUBSYSTEM = ',I5,' (NB)') >*/
/*<       CALL PERMAT (N,IA,JA,A,IWKSP,IWKSP(JB3),ISYM,LEVEL,NOUT,IER) >*/
    permat_(&n, &ia[1], &ja[1], &a[1], &iwksp[1], &iwksp[jb3], &itcom1_1.isym,
	     &itcom1_1.level, &itcom1_1.nout, &ier);
/*<       IF (IER.EQ.0) GO TO 180 >*/
    if (ier == 0) {
	goto L180;
    }
/*<       IF (LEVEL.GE.0) WRITE (NOUT,170) IER >*/
    if (itcom1_1.level >= 0) {
	io___213.ciunit = itcom1_1.nout;
	s_wsfe(&io___213);
	do_fio(&c__1, (char *)&ier, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 430 >*/
    goto L430;
/*<   180 CALL PERVEC (N,RHS,IWKSP) >*/
L180:
    pervec_(&n, &rhs[1], &iwksp[1]);
/*<       CALL PERVEC (N,U,IWKSP) >*/
    pervec_(&n, &u[1], &iwksp[1]);

/* ... FINISH WKSP BASE ADDRESSES */

/*<   190 IB3 = IB2+NB >*/
L190:
    ib3 = ib2 + nb;
/*<       IB4 = IB3+NB >*/
    ib4 = ib3 + nb;
/*<       IB5 = IB4+NB >*/
    ib5 = ib4 + nb;
/*<       NR = N-NB >*/
    nr = n - nb;
/*<       NRP1 = NR+1 >*/
    nrp1 = nr + 1;
/*<       IPARM(8) = N+3*NB+2*ITMAX >*/
    iparm[8] = n + nb * 3 + (itcom1_1.itmax << 1);
/*<       IF (ISYM.NE.0) IPARM(8) = IPARM(8)+2*ITMAX >*/
    if (itcom1_1.isym != 0) {
	iparm[8] += itcom1_1.itmax << 1;
    }
/*<       IF (NW.GE.IPARM(8)) GO TO 210 >*/
    if (*nw >= iparm[8]) {
	goto L210;
    }
/*<       IER = 62 >*/
    ier = 62;
/*<       IF (LEVEL.GE.0) WRITE (NOUT,200) NW,IPARM(8) >*/
    if (itcom1_1.level >= 0) {
	io___219.ciunit = itcom1_1.nout;
	s_wsfe(&io___219);
	do_fio(&c__1, (char *)&(*nw), (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&iparm[8], (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 430 >*/
    goto L430;

/* ... SCALE LINEAR SYSTEM, U, AND RHS BY THE SQUARE ROOT OF THE */
/* ... DIAGONAL ELEMENTS. */

/*<   210 CONTINUE >*/
L210:
/*<       CALL VFILL (IPARM(8),WKSP,0.0D0) >*/
    vfill_(&iparm[8], &wksp[1], &c_b21);
/*<       CALL SCAL (N,IA,JA,A,RHS,U,WKSP,LEVEL,NOUT,IER) >*/
    scal_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &u[1], &wksp[1], &
	    itcom1_1.level, &itcom1_1.nout, &ier);
/*<       IF (IER.EQ.0) GO TO 230 >*/
    if (ier == 0) {
	goto L230;
    }
/*<       IF (LEVEL.GE.0) WRITE (NOUT,220) IER >*/
    if (itcom1_1.level >= 0) {
	io___220.ciunit = itcom1_1.nout;
	s_wsfe(&io___220);
	do_fio(&c__1, (char *)&ier, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 430 >*/
    goto L430;
/*<   230 IF (LEVEL.LE.2) GO TO 260 >*/
L230:
    if (itcom1_1.level <= 2) {
	goto L260;
    }
/*<       WRITE (NOUT,240) >*/
    io___221.ciunit = itcom1_1.nout;
    s_wsfe(&io___221);
    e_wsfe();
/*<    >*/
/*<       IF (ADAPT) WRITE (NOUT,250) >*/
    if (itcom2_1.adapt) {
	io___222.ciunit = itcom1_1.nout;
	s_wsfe(&io___222);
	e_wsfe();
    }
/*<    >*/
/*<   260 IF (IPARM(11).NE.0) GO TO 270 >*/
L260:
    if (iparm[11] != 0) {
	goto L270;
    }
/*<       TIMI1 = TIMER(DUMMY) >*/
    timi1 = timer_(&dummy);

/* ... INITIALIZE FORWARD PSEUDO-RESIDUAL */

/*<   270 CONTINUE >*/
L270:
/*<       IF (N.GT.1) GO TO 280 >*/
    if (n > 1) {
	goto L280;
    }
/*<       U(1) = RHS(1) >*/
    u[1] = rhs[1];
/*<       GO TO 330 >*/
    goto L330;
/*<   280 CALL DCOPY (NR,RHS,1,WKSP(IB1),1) >*/
L280:
    dcopy_(&nr, &rhs[1], &c__1, &wksp[ib1], &c__1);
/*<       CALL PRSRED (NB,NR,IA,JA,A,U(NRP1),WKSP(IB1)) >*/
    prsred_(&nb, &nr, &ia[1], &ja[1], &a[1], &u[nrp1], &wksp[ib1]);
/*<       CALL DCOPY (NB,RHS(NRP1),1,WKSP(IB2),1) >*/
    dcopy_(&nb, &rhs[nrp1], &c__1, &wksp[ib2], &c__1);
/*<       CALL PRSBLK (NB,NR,IA,JA,A,WKSP(IB1),WKSP(IB2)) >*/
    prsblk_(&nb, &nr, &ia[1], &ja[1], &a[1], &wksp[ib1], &wksp[ib2]);
/*<       CALL VEVMW (NB,WKSP(IB2),U(NRP1)) >*/
    vevmw_(&nb, &wksp[ib2], &u[nrp1]);

/* ... ITERATION SEQUENCE */

/*<       ITMAX1 = ITMAX+1 >*/
    itmax1 = itcom1_1.itmax + 1;
/*<       DO 300 LOOP = 1,ITMAX1 >*/
    i__1 = itmax1;
    for (loop = 1; loop <= i__1; ++loop) {
/*<          IN = LOOP-1 >*/
	itcom1_1.in = loop - 1;
/*<          IF (MOD(IN,2).EQ.1) GO TO 290 >*/
	if (itcom1_1.in % 2 == 1) {
	    goto L290;
	}

/* ... CODE FOR THE EVEN ITERATIONS. */

/*     U           = U(IN)       WKSP(IB2) = D(IN) */
/*     WKSP(IB1)   = U(IN-1)     WKSP(IB3) = D(IN-1) */

/*<    >*/
	itrscg_(&n, &nb, &ia[1], &ja[1], &a[1], &u[1], &wksp[ib1], &wksp[ib2],
		 &wksp[ib3], &wksp[ib4], &wksp[ib5]);

/*<          IF (HALT) GO TO 330 >*/
	if (itcom2_1.halt) {
	    goto L330;
	}
/*<          GO TO 300 >*/
	goto L300;

/* ... CODE FOR THE ODD ITERATIONS. */

/*     U           = U(IN-1)     WKSP(IB2) = D(IN-1) */
/*     WKSP(IB1)   = U(IN)       WKSP(IB3) = D(IN) */

/*<    >*/
L290:
	itrscg_(&n, &nb, &ia[1], &ja[1], &a[1], &wksp[ib1], &u[1], &wksp[ib3],
		 &wksp[ib2], &wksp[ib4], &wksp[ib5]);

/*<          IF (HALT) GO TO 330 >*/
	if (itcom2_1.halt) {
	    goto L330;
	}
/*<   300 CONTINUE >*/
L300:
	;
    }

/* ... ITMAX HAS BEEN REACHED */

/*<       IF (IPARM(11).NE.0) GO TO 310 >*/
    if (iparm[11] != 0) {
	goto L310;
    }
/*<       TIMI2 = TIMER(DUMMY) >*/
    timi2 = timer_(&dummy);
/*<       TIME1 = DBLE(TIMI2-TIMI1) >*/
    time1 = (doublereal) (timi2 - timi1);
/*<   310 IF (LEVEL.GE.1) WRITE (NOUT,320) ITMAX >*/
L310:
    if (itcom1_1.level >= 1) {
	io___227.ciunit = itcom1_1.nout;
	s_wsfe(&io___227);
	do_fio(&c__1, (char *)&itcom1_1.itmax, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       IER = 63 >*/
    ier = 63;
/*<       IF (IPARM(3).EQ.0) RPARM(1) = STPTST >*/
    if (iparm[3] == 0) {
	rparm[1] = itcom3_1.stptst;
    }
/*<       GO TO 360 >*/
    goto L360;

/* ... METHOD HAS CONVERGED */

/*<   330 IF (IPARM(11).NE.0) GO TO 340 >*/
L330:
    if (iparm[11] != 0) {
	goto L340;
    }
/*<       TIMI2 = TIMER(DUMMY) >*/
    timi2 = timer_(&dummy);
/*<       TIME1 = DBLE(TIMI2-TIMI1) >*/
    time1 = (doublereal) (timi2 - timi1);
/*<   340 IF (LEVEL.GE.1) WRITE (NOUT,350) IN >*/
L340:
    if (itcom1_1.level >= 1) {
	io___228.ciunit = itcom1_1.nout;
	s_wsfe(&io___228);
	do_fio(&c__1, (char *)&itcom1_1.in, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<   350 FORMAT (/1X,'RSCG  HAS CONVERGED IN ',I5,' ITERATIONS') >*/

/* ... PUT SOLUTION INTO U IF NOT ALREADY THERE. */

/*<   360 CONTINUE >*/
L360:
/*<       IF (N.EQ.1) GO TO 370 >*/
    if (n == 1) {
	goto L370;
    }
/*<       IF (MOD(IN,2).EQ.1) CALL DCOPY (N,WKSP(IB1),1,U,1) >*/
    if (itcom1_1.in % 2 == 1) {
	dcopy_(&n, &wksp[ib1], &c__1, &u[1], &c__1);
    }
/*<       CALL DCOPY (NR,RHS,1,U,1) >*/
    dcopy_(&nr, &rhs[1], &c__1, &u[1], &c__1);
/*<       CALL PRSRED (NB,NR,IA,JA,A,U(NRP1),U) >*/
    prsred_(&nb, &nr, &ia[1], &ja[1], &a[1], &u[nrp1], &u[1]);

/* ... UNSCALE THE MATRIX, SOLUTION, AND RHS VECTORS. */

/*<   370 CALL UNSCAL (N,IA,JA,A,RHS,U,WKSP) >*/
L370:
    unscal_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &u[1], &wksp[1]);

/* ... UN-PERMUTE MATRIX,RHS, AND SOLUTION */

/*<       IF (IPARM(9).GE.0) GO TO 400 >*/
    if (iparm[9] >= 0) {
	goto L400;
    }
/*<    >*/
    permat_(&n, &ia[1], &ja[1], &a[1], &iwksp[ib2], &iwksp[jb3], &
	    itcom1_1.isym, &itcom1_1.level, &itcom1_1.nout, &ierper);
/*<       IF (IERPER.EQ.0) GO TO 390 >*/
    if (ierper == 0) {
	goto L390;
    }
/*<       IF (LEVEL.GE.0) WRITE (NOUT,380) IERPER >*/
    if (itcom1_1.level >= 0) {
	io___230.ciunit = itcom1_1.nout;
	s_wsfe(&io___230);
	do_fio(&c__1, (char *)&ierper, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       IF (IER.EQ.0) IER = IERPER >*/
    if (ier == 0) {
	ier = ierper;
    }
/*<       GO TO 430 >*/
    goto L430;
/*<   390 CALL PERVEC (N,RHS,IWKSP(IB2)) >*/
L390:
    pervec_(&n, &rhs[1], &iwksp[ib2]);
/*<       CALL PERVEC (N,U,IWKSP(IB2)) >*/
    pervec_(&n, &u[1], &iwksp[ib2]);

/* ... OPTIONAL ERROR ANALYSIS */

/*<   400 IDGTS = IPARM(12) >*/
L400:
    idgts = iparm[12];
/*<       IF (IDGTS.LT.0) GO TO 410 >*/
    if (idgts < 0) {
	goto L410;
    }
/*<       IF (IPARM(2).LE.0) IDGTS = 0 >*/
    if (iparm[2] <= 0) {
	idgts = 0;
    }
/*<       CALL PERROR (N,IA,JA,A,RHS,U,WKSP,DIGIT1,DIGIT2,IDGTS) >*/
    perror_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &u[1], &wksp[1], &digit1, &
	    digit2, &idgts);

/* ... SET RETURN PARAMETERS IN IPARM AND RPARM */

/*<   410 IF (IPARM(11).NE.0) GO TO 420 >*/
L410:
    if (iparm[11] != 0) {
	goto L420;
    }
/*<       TIMJ2 = TIMER(DUMMY) >*/
    timj2 = timer_(&dummy);
/*<       TIME2 = DBLE(TIMJ2-TIMJ1) >*/
    time2 = (doublereal) (timj2 - timj1);
/*<   420 IPARM(8) = IPARM(8)-2*(ITMAX-IN) >*/
L420:
    iparm[8] -= itcom1_1.itmax - itcom1_1.in << 1;
/*<       IF (ISYM.NE.0) IPARM(8) = IPARM(8)-2*(ITMAX-IN) >*/
    if (itcom1_1.isym != 0) {
	iparm[8] -= itcom1_1.itmax - itcom1_1.in << 1;
    }
/*<       IF (IPARM(3).NE.0) GO TO 430 >*/
    if (iparm[3] != 0) {
	goto L430;
    }
/*<       IPARM(1) = IN >*/
    iparm[1] = itcom1_1.in;
/*<       IPARM(9) = NB >*/
    iparm[9] = nb;
/*<       RPARM(2) = CME >*/
    rparm[2] = itcom3_1.cme;
/*<       RPARM(3) = SME >*/
    rparm[3] = itcom3_1.sme;
/*<       RPARM(9) = TIME1 >*/
    rparm[9] = time1;
/*<       RPARM(10) = TIME2 >*/
    rparm[10] = time2;
/*<       RPARM(11) = DIGIT1 >*/
    rparm[11] = digit1;
/*<       RPARM(12) = DIGIT2 >*/
    rparm[12] = digit2;

/*<   430 CONTINUE >*/
L430:
/*<       IERR = IER >*/
    *ierr = ier;
/*<       IF (LEVEL.GE.3) CALL ECHALL (N,IA,JA,A,RHS,IPARM,RPARM,2) >*/
    if (itcom1_1.level >= 3) {
	echall_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &iparm[1], &rparm[1], &
		c__2);
    }

/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* rscg_ */

/*<       SUBROUTINE RSSI (NN,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IERR) >*/
/* Subroutine */ int rssi_(integer *nn, integer *ia, integer *ja, doublereal *
	a, doublereal *rhs, doublereal *u, integer *iwksp, integer *nw, 
	doublereal *wksp, integer *iparm, doublereal *rparm, integer *ierr)
{
    /* Format strings */
    static char fmt_10[] = "(\0020\002///1x,\002BEGINNING OF ITPACK SOLUTION\
 MODULE  RSSI\002)";
    static char fmt_40[] = "(\0020\002,\002*** W A R N I N G ************\
\002/\0020\002,\002    IN ITPACK ROUTINE RSSI\002/\002 \002,\002    RPARM(1)\
 =\002,d10.3,\002 (ZETA)\002/\002 \002,\002    A VALUE THIS SMALL MAY HINDER\
 CONVERGENCE \002/\002 \002,\002    SINCE MACHINE PRECISION DRELPR =\002,d10\
.3/\002 \002,\002    ZETA RESET TO \002,d10.3)";
    static char fmt_60[] = "(\0020\002,\002*** F A T A L     E R R O R *****\
*******\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE RSSI \002/\002 \002,\
\002    INVALID MATRIX DIMENSION, N =\002,i8)";
    static char fmt_80[] = "(\0020\002,\002*** F A T A L     E R R O R *****\
*******\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE RSSI \002/\002 \002,\
\002    ERROR DETECTED IN SUBROUTINE  SBELM \002/\002 \002,\002    WHICH REM\
OVES ROWS AND COLUMNS OF SYSTEM \002/\002 \002,\002    WHEN DIAGONAL ENTRY T\
OO LARGE  \002/\002 \002,\002    IER = \002,i5,5x,\002 RPARM(8) = \002,d10.3,\
\002 (TOL)\002)";
    static char fmt_100[] = "(\0020\002,\002*** F A T A L     E R R O R ****\
********\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE RSSI \002/\002 \
\002,\002    ERROR DETECTED IN SUBROUTINE  PRBNDX\002/\002 \002,\002    WHIC\
H COMPUTES THE RED-BLACK INDEXING\002/\002 \002,\002    IER = \002,i5,\002 I\
PARM(9) = \002,i5,\002 (NB)\002)";
    static char fmt_120[] = "(/10x,\002ERROR DETECTED IN RED-BLACK SUBSYSTEM\
 INDEX\002/10x,\002IER =\002,i5,\002 IPARM(9) =\002,i5,\002 (NB)\002)";
    static char fmt_140[] = "(/10x,\002 IPARM(9) = \002,i5,\002 IMPLIES MATR\
IX IS DIAGONAL\002/10x,\002 NB RESET TO \002,i5)";
    static char fmt_160[] = "(/10x,\002ORDER OF BLACK SUBSYSTEM = \002,i5\
,\002 (NB)\002)";
    static char fmt_170[] = "(\0020\002,\002*** F A T A L     E R R O R ****\
********\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE RSSI \002/\002 \
\002,\002    ERROR DETECTED IN SUBROUTINE  PERMAT\002/\002 \002,\002    WHIC\
H DOES THE RED-BLACK PERMUTATION\002/\002 \002,\002    IER = \002,i5)";
    static char fmt_200[] = "(\0020\002,\002*** F A T A L     E R R O R ****\
********\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE RSSI \002/\002 \
\002,\002    NOT ENOUGH WORKSPACE AT \002,i10/\002 \002,\002    SET IPARM(8)\
 =\002,i10,\002 (NW)\002)";
    static char fmt_220[] = "(\0020\002,\002*** F A T A L     E R R O R ****\
********\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE RSSI \002/\002 \
\002,\002    ERROR DETECTED IN SUBROUTINE  SCAL  \002/\002 \002,\002    WHIC\
H SCALES THE SYSTEM   \002/\002 \002,\002    IER = \002,i5)";
    static char fmt_240[] = "(///1x,\002IN THE FOLLOWING, RHO AND GAMMA AR\
E\002,\002 ACCELERATION PARAMETERS\002)";
    static char fmt_310[] = "(\0020\002,\002*** W A R N I N G ***********\
*\002/\0020\002,\002    IN ITPACK ROUTINE RSSI\002/\002 \002,\002    FAILURE\
 TO CONVERGE IN\002,i5,\002 ITERATIONS\002)";
    static char fmt_340[] = "(/1x,\002RSSI  HAS CONVERGED IN \002,i5,\002 IT\
ERATIONS\002)";
    static char fmt_370[] = "(\0020\002,\002*** F A T A L     E R R O R ****\
********\002/\0020\002,\002    CALLED FROM ITPACK ROUTINE RSSI \002/\002 \
\002,\002    ERROR DETECTED IN SUBROUTINE  PERMAT\002/\002 \002,\002    WHIC\
H UNDOES THE RED-BLACK PERMUTATION   \002/\002 \002,\002    IER = \002,i5)";

    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), e_wsfe(), do_fio(integer *, char *, ftnlen);

    /* Local variables */
    integer n, n3, nb, nr, ib1, ib2, jb3, ier;
    doublereal tol;
    integer nrp1;
    extern /* Subroutine */ int scal_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *,
	     integer *, integer *);
    doublereal temp;
    integer loop;
    doublereal time1, time2;
    real timi1, timj1, timi2, timj2;
    extern /* Subroutine */ int sbelm_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, integer *, doublereal *, doublereal *,
	     integer *, integer *, integer *, integer *);
    integer idgts;
    extern /* Subroutine */ int vfill_(integer *, doublereal *, doublereal *),
	     dcopy_(integer *, doublereal *, integer *, doublereal *, integer 
	    *);
    extern doublereal timer_(real *);
    real dummy;
    doublereal digit1, digit2;
    integer itmax1;
    extern /* Subroutine */ int echall_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, integer *, doublereal *, integer *), 
	    pervec_(integer *, doublereal *, integer *), ivfill_(integer *, 
	    integer *, integer *);
    integer ierper;
    extern /* Subroutine */ int echout_(integer *, doublereal *, integer *), 
	    permat_(integer *, integer *, integer *, doublereal *, integer *, 
	    integer *, integer *, integer *, integer *, integer *), unscal_(
	    integer *, integer *, integer *, doublereal *, doublereal *, 
	    doublereal *, doublereal *), prbndx_(integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *), prsred_(integer *, integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *), perror_(integer *, 
	    integer *, integer *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, integer *), itrssi_(
	    integer *, integer *, integer *, integer *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *);

    /* Fortran I/O blocks */
    static cilist io___233 = { 0, 0, 0, fmt_10, 0 };
    static cilist io___239 = { 0, 0, 0, fmt_40, 0 };
    static cilist io___244 = { 0, 0, 0, fmt_60, 0 };
    static cilist io___246 = { 0, 0, 0, fmt_80, 0 };
    static cilist io___252 = { 0, 0, 0, fmt_100, 0 };
    static cilist io___253 = { 0, 0, 0, fmt_120, 0 };
    static cilist io___254 = { 0, 0, 0, fmt_140, 0 };
    static cilist io___255 = { 0, 0, 0, fmt_160, 0 };
    static cilist io___256 = { 0, 0, 0, fmt_170, 0 };
    static cilist io___259 = { 0, 0, 0, fmt_200, 0 };
    static cilist io___260 = { 0, 0, 0, fmt_220, 0 };
    static cilist io___261 = { 0, 0, 0, fmt_240, 0 };
    static cilist io___266 = { 0, 0, 0, fmt_310, 0 };
    static cilist io___267 = { 0, 0, 0, fmt_340, 0 };
    static cilist io___269 = { 0, 0, 0, fmt_370, 0 };



/*     ITPACK 2C MAIN SUBROUTINE  RSSI  (REDUCED SYSTEM SEMI-ITERATIVE) */
/*     EACH OF THE MAIN SUBROUTINES: */
/*           JCG, JSI, SOR, SSORCG, SSORSI, RSCG, RSSI */
/*     CAN BE USED INDEPENDENTLY OF THE OTHERS */

/* ... FUNCTION: */

/*          THIS SUBROUTINE, RSSI, DRIVES THE  REDUCED SYSTEM SI */
/*          ALGORITHM. */

/* ... PARAMETER LIST: */

/*          N     INPUT INTEGER.  DIMENSION OF THE MATRIX. (= NN) */
/*          IA,JA  INPUT INTEGER VECTORS.  THE TWO INTEGER ARRAYS OF */
/*                 THE SPARSE MATRIX REPRESENTATION. */
/*          A      INPUT D.P. VECTOR.  THE D.P. ARRAY OF THE SPARSE */
/*                 MATRIX REPRESENTATION. */
/*          RHS    INPUT D.P. VECTOR.  CONTAINS THE RIGHT HAND SIDE */
/*                 OF THE MATRIX PROBLEM. */
/*          U      INPUT/OUTPUT D.P. VECTOR.  ON INPUT, U CONTAINS THE */
/*                 INITIAL GUESS TO THE SOLUTION. ON OUTPUT, IT CONTAINS */
/*                 THE LATEST ESTIMATE TO THE SOLUTION. */
/*          IWKSP  INTEGER VECTOR WORKSPACE OF LENGTH 3*N */
/*          NW     INPUT INTEGER.  LENGTH OF AVAILABLE WKSP.  ON OUTPUT, */
/*                 IPARM(8) IS AMOUNT USED. */
/*          WKSP   D.P. VECTOR USED FOR WORKING SPACE.  RSSI */
/*                 NEEDS THIS TO BE IN LENGTH AT LEAST  N + NB */
/*                 HERE NB IS THE ORDER OF THE BLACK SUBSYSTEM */
/*          IPARM  INTEGER VECTOR OF LENGTH 12.  ALLOWS USER TO SPECIFY */
/*                 SOME INTEGER PARAMETERS WHICH AFFECT THE METHOD.  IF */
/*          RPARM  D.P. VECTOR OF LENGTH 12. ALLOWS USER TO SPECIFY SOME */
/*                 D.P. PARAMETERS WHICH AFFECT THE METHOD. */
/*          IER     OUTPUT INTEGER.  ERROR FLAG. (= IERR) */

/* ... RSSI SUBPROGRAM REFERENCES: */

/*          FROM ITPACK    BISRCH, CHEBY, CHGSI, DFAULT, ECHALL, */
/*                         ECHOUT, ITERM, TIMER, ITRSSI, IVFILL, */
/*                         PARSI, PERMAT, PERROR, PERVEC, PMULT, */
/*                         PRBNDX, PRSBLK, PRSRED, PSTOP, QSORT, */
/*                         DAXPY, SBELM, SCAL, DCOPY, DDOT, SUM3, */
/*                         TSTCHG, UNSCAL, VEVMW, VFILL, VOUT, */
/*                         WEVMW */
/*          SYSTEM         DABS, DLOG10, DBLE(AMAX0), DMAX1, DBLE(FLOAT), */
/*                         DSQRT */

/*     VERSION:  ITPACK 2C (MARCH 1982) */

/*     CODE WRITTEN BY:  DAVID KINCAID, ROGER GRIMES, JOHN RESPESS */
/*                       CENTER FOR NUMERICAL ANALYSIS */
/*                       UNIVERSITY OF TEXAS */
/*                       AUSTIN, TX  78712 */
/*                       (512) 471-1242 */

/*     FOR ADDITIONAL DETAILS ON THE */
/*          (A) SUBROUTINE SEE TOMS ARTICLE 1982 */
/*          (B) ALGORITHM  SEE CNA REPORT 150 */

/*     BASED ON THEORY BY:  DAVID YOUNG, DAVID KINCAID, LOU HAGEMAN */

/*     REFERENCE THE BOOK:  APPLIED ITERATIVE METHODS */
/*                          L. HAGEMAN, D. YOUNG */
/*                          ACADEMIC PRESS, 1981 */

/*     ************************************************** */
/*     *               IMPORTANT NOTE                   * */
/*     *                                                * */
/*     *      WHEN INSTALLING ITPACK ROUTINES ON A      * */
/*     *  DIFFERENT COMPUTER, RESET SOME OF THE VALUES  * */
/*     *  IN  SUBROUTNE DFAULT.   MOST IMPORTANT ARE    * */
/*     *                                                * */
/*     *   DRELPR      MACHINE RELATIVE PRECISION       * */
/*     *   RPARM(1)    STOPPING CRITERION               * */
/*     *                                                * */
/*     *   ALSO CHANGE SYSTEM-DEPENDENT ROUTINE         * */
/*     *   SECOND USED IN TIMER                         * */
/*     *                                                * */
/*     ************************************************** */

/*     SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER IA(1),JA(1),IWKSP(1),IPARM(12),NN,NW,IERR >*/
/*<       DOUBLE PRECISION A(1),RHS(NN),U(NN),WKSP(NW),RPARM(12) >*/

/*     SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER IB1,IB2,IDGTS,IER,IERPER,ITMAX1,JB3,LOOP,N,NB,NR,NRP1,N3 >*/
/*<       DOUBLE PRECISION DIGIT1,DIGIT2,TEMP,TIME1,TIME2,TOL >*/

/* *** BEGIN: ITPACK COMMON */

/*<       INTEGER IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/
/*<       COMMON /ITCOM1/ IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/

/*<       LOGICAL ADAPT,BETADT,CASEII,HALT,PARTAD >*/
/*<       COMMON /ITCOM2/ ADAPT,BETADT,CASEII,HALT,PARTAD >*/

/*<    >*/
/*<    >*/

/* *** END  : ITPACK COMMON */

/* ... VARIABLES IN COMMON BLOCK - ITCOM1 */

/*     IN     - ITERATION NUMBER */
/*     IS     - ITERATION NUMBER WHEN PARAMETERS LAST CHANGED */
/*     ISYM   - SYMMETRIC/NONSYMMETRIC STORAGE FORMAT SWITCH */
/*     ITMAX  - MAXIMUM NUMBER OF ITERATIONS ALLOWED */
/*     LEVEL  - LEVEL OF OUTPUT CONTROL SWITCH */
/*     NOUT   - OUTPUT UNIT NUMBER */

/* ... VARIABLES IN COMMON BLOCK - ITCOM2 */

/*     ADAPT  - FULLY ADAPTIVE PROCEDURE SWITCH */
/*     BETADT - SWITCH FOR ADAPTIVE DETERMINATION OF BETA */
/*     CASEII - ADAPTIVE PROCEDURE CASE SWITCH */
/*     HALT   - STOPPING TEST SWITCH */
/*     PARTAD - PARTIALLY ADAPTIVE PROCEDURE SWITCH */

/* ... VARIABLES IN COMMON BLOCK - ITCOM3 */

/*     BDELNM - TWO NORM OF B TIMES DELTA-SUPER-N */
/*     BETAB  - ESTIMATE FOR THE SPECTRAL RADIUS OF LU MATRIX */
/*     CME    - ESTIMATE OF LARGEST EIGENVALUE */
/*     DELNNM - INNER PRODUCT OF PSEUDO-RESIDUAL AT ITERATION N */
/*     DELSNM - INNER PRODUCT OF PSEUDO-RESIDUAL AT ITERATION S */
/*     FF     - ADAPTIVE PROCEDURE DAMPING FACTOR */
/*     GAMMA  - ACCELERATION PARAMETER */
/*     OMEGA  - OVERRELAXATION PARAMETER FOR SOR AND SSOR */
/*     QA     - PSEUDO-RESIDUAL RATIO */
/*     QT     - VIRTUAL SPECTRAL RADIUS */
/*     RHO    - ACCELERATION PARAMETER */
/*     RRR    - ADAPTIVE PARAMETER */
/*     SIGE   - PARAMETER SIGMA-SUB-E */
/*     SME    - ESTIMATE OF SMALLEST EIGENVALUE */
/*     SPECR  - SPECTRAL RADIUS ESTIMATE FOR SSOR */
/*     DRELPR - MACHINE RELATIVE PRECISION */
/*     STPTST - STOPPING PARAMETER */
/*     UDNM   - TWO NORM OF U */
/*     ZETA   - STOPPING CRITERION */

/* ... INITIALIZE COMMON BLOCKS */

/*<       LEVEL = IPARM(2) >*/
    /* Parameter adjustments */
    --u;
    --rhs;
    --ia;
    --ja;
    --a;
    --iwksp;
    --wksp;
    --iparm;
    --rparm;

    /* Function Body */
    itcom1_1.level = iparm[2];
/*<       NOUT = IPARM(4) >*/
    itcom1_1.nout = iparm[4];
/*<       IF (LEVEL.GE.1) WRITE (NOUT,10) >*/
    if (itcom1_1.level >= 1) {
	io___233.ciunit = itcom1_1.nout;
	s_wsfe(&io___233);
	e_wsfe();
    }
/*<    10 FORMAT ('0'///1X,'BEGINNING OF ITPACK SOLUTION MODULE  RSSI') >*/
/*<       IER = 0 >*/
    ier = 0;
/*<       IF (IPARM(1).LE.0) RETURN >*/
    if (iparm[1] <= 0) {
	return 0;
    }
/*<       N = NN >*/
    n = *nn;
/*<       IF (IPARM(11).EQ.0) TIMJ1 = TIMER(DUMMY) >*/
    if (iparm[11] == 0) {
	timj1 = timer_(&dummy);
    }
/*<       IF (LEVEL.GE.3) GO TO 20 >*/
    if (itcom1_1.level >= 3) {
	goto L20;
    }
/*<       CALL ECHOUT (IPARM,RPARM,7) >*/
    echout_(&iparm[1], &rparm[1], &c__7);
/*<       GO TO 30 >*/
    goto L30;
/*<    20 CALL ECHALL (N,IA,JA,A,RHS,IPARM,RPARM,1) >*/
L20:
    echall_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &iparm[1], &rparm[1], &c__1);
/*<    30 TEMP = 5.0D2*DRELPR >*/
L30:
    temp = itcom3_1.drelpr * 500.;
/*<       IF (ZETA.GE.TEMP) GO TO 50 >*/
    if (itcom3_1.zeta >= temp) {
	goto L50;
    }
/*<       IF (LEVEL.GE.1) WRITE (NOUT,40) ZETA,DRELPR,TEMP >*/
    if (itcom1_1.level >= 1) {
	io___239.ciunit = itcom1_1.nout;
	s_wsfe(&io___239);
	do_fio(&c__1, (char *)&itcom3_1.zeta, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&itcom3_1.drelpr, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&temp, (ftnlen)sizeof(doublereal));
	e_wsfe();
    }
/*<    >*/
/*<       ZETA = TEMP >*/
    itcom3_1.zeta = temp;
/*<    50 CONTINUE >*/
L50:
/*<       TIME1 = RPARM(9) >*/
    time1 = rparm[9];
/*<       TIME2 = RPARM(10) >*/
    time2 = rparm[10];
/*<       DIGIT1 = RPARM(11) >*/
    digit1 = rparm[11];
/*<       DIGIT2 = RPARM(12) >*/
    digit2 = rparm[12];

/* ... VERIFY N */

/*<       IF (N.GT.0) GO TO 70 >*/
    if (n > 0) {
	goto L70;
    }
/*<       IER = 71 >*/
    ier = 71;
/*<       IF (LEVEL.GE.0) WRITE (NOUT,60) N >*/
    if (itcom1_1.level >= 0) {
	io___244.ciunit = itcom1_1.nout;
	s_wsfe(&io___244);
	do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 420 >*/
    goto L420;
/*<    70 CONTINUE >*/
L70:

/* ... REMOVE ROWS AND COLUMNS IF REQUESTED */

/*<       IF (IPARM(10).EQ.0) GO TO 90 >*/
    if (iparm[10] == 0) {
	goto L90;
    }
/*<       TOL = RPARM(8) >*/
    tol = rparm[8];
/*<       CALL IVFILL (N,IWKSP,0) >*/
    ivfill_(&n, &iwksp[1], &c__0);
/*<       CALL VFILL (N,WKSP,0.0D0) >*/
    vfill_(&n, &wksp[1], &c_b21);
/*<       CALL SBELM (N,IA,JA,A,RHS,IWKSP,WKSP,TOL,ISYM,LEVEL,NOUT,IER) >*/
    sbelm_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &iwksp[1], &wksp[1], &tol, &
	    itcom1_1.isym, &itcom1_1.level, &itcom1_1.nout, &ier);
/*<       IF (IER.EQ.0) GO TO 90 >*/
    if (ier == 0) {
	goto L90;
    }
/*<       IF (LEVEL.GE.0) WRITE (NOUT,80) IER,TOL >*/
    if (itcom1_1.level >= 0) {
	io___246.ciunit = itcom1_1.nout;
	s_wsfe(&io___246);
	do_fio(&c__1, (char *)&ier, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&tol, (ftnlen)sizeof(doublereal));
	e_wsfe();
    }
/*<    >*/

/* ... INITIALIZE WKSP BASE ADDRESSES. */

/*<    90 IB1 = 1 >*/
L90:
    ib1 = 1;
/*<       IB2 = IB1+N >*/
    ib2 = ib1 + n;
/*<       JB3 = IB2+N >*/
    jb3 = ib2 + n;

/* ... PERMUTE TO  RED-BLACK SYSTEM IF POSSIBLE */

/*<       NB = IPARM(9) >*/
    nb = iparm[9];
/*<       IF (NB.GE.0) GO TO 110 >*/
    if (nb >= 0) {
	goto L110;
    }
/*<       N3 = 3*N >*/
    n3 = n * 3;
/*<       CALL IVFILL (N3,IWKSP,0) >*/
    ivfill_(&n3, &iwksp[1], &c__0);
/*<       CALL PRBNDX (N,NB,IA,JA,IWKSP,IWKSP(IB2),LEVEL,NOUT,IER) >*/
    prbndx_(&n, &nb, &ia[1], &ja[1], &iwksp[1], &iwksp[ib2], &itcom1_1.level, 
	    &itcom1_1.nout, &ier);
/*<       IF (IER.EQ.0) GO TO 110 >*/
    if (ier == 0) {
	goto L110;
    }
/*<       IF (LEVEL.GE.0) WRITE (NOUT,100) IER,NB >*/
    if (itcom1_1.level >= 0) {
	io___252.ciunit = itcom1_1.nout;
	s_wsfe(&io___252);
	do_fio(&c__1, (char *)&ier, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&nb, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 420 >*/
    goto L420;
/*<   110 IF (NB.GE.0.AND.NB.LE.N) GO TO 130 >*/
L110:
    if (nb >= 0 && nb <= n) {
	goto L130;
    }
/*<       IER = 74 >*/
    ier = 74;
/*<       IF (LEVEL.GE.1) WRITE (NOUT,120) IER,NB >*/
    if (itcom1_1.level >= 1) {
	io___253.ciunit = itcom1_1.nout;
	s_wsfe(&io___253);
	do_fio(&c__1, (char *)&ier, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&nb, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 420 >*/
    goto L420;
/*<   130 IF (NB.NE.0.AND.NB.NE.N) GO TO 150 >*/
L130:
    if (nb != 0 && nb != n) {
	goto L150;
    }
/*<       NB = N/2 >*/
    nb = n / 2;
/*<       IF (LEVEL.GE.2.AND.IPARM(9).GE.0) WRITE (NOUT,140) IPARM(9),NB >*/
    if (itcom1_1.level >= 2 && iparm[9] >= 0) {
	io___254.ciunit = itcom1_1.nout;
	s_wsfe(&io___254);
	do_fio(&c__1, (char *)&iparm[9], (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&nb, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/

/* ... PERMUTE MATRIX AND RHS */

/*<   150 IF (IPARM(9).GE.0) GO TO 190 >*/
L150:
    if (iparm[9] >= 0) {
	goto L190;
    }
/*<       IF (LEVEL.GE.2) WRITE (NOUT,160) NB >*/
    if (itcom1_1.level >= 2) {
	io___255.ciunit = itcom1_1.nout;
	s_wsfe(&io___255);
	do_fio(&c__1, (char *)&nb, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<   160 FORMAT (/10X,'ORDER OF BLACK SUBSYSTEM = ',I5,' (NB)') >*/
/*<       CALL PERMAT (N,IA,JA,A,IWKSP,IWKSP(JB3),ISYM,LEVEL,NOUT,IER) >*/
    permat_(&n, &ia[1], &ja[1], &a[1], &iwksp[1], &iwksp[jb3], &itcom1_1.isym,
	     &itcom1_1.level, &itcom1_1.nout, &ier);
/*<       IF (IER.EQ.0) GO TO 180 >*/
    if (ier == 0) {
	goto L180;
    }
/*<       IF (LEVEL.GE.0) WRITE (NOUT,170) IER >*/
    if (itcom1_1.level >= 0) {
	io___256.ciunit = itcom1_1.nout;
	s_wsfe(&io___256);
	do_fio(&c__1, (char *)&ier, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 420 >*/
    goto L420;
/*<   180 CALL PERVEC (N,RHS,IWKSP) >*/
L180:
    pervec_(&n, &rhs[1], &iwksp[1]);
/*<       CALL PERVEC (N,U,IWKSP) >*/
    pervec_(&n, &u[1], &iwksp[1]);

/* ... INITIALIZE WKSP BASE ADDRESSES */

/*<   190 NR = N-NB >*/
L190:
    nr = n - nb;

/*<       NRP1 = NR+1 >*/
    nrp1 = nr + 1;
/*<       IPARM(8) = N+NB >*/
    iparm[8] = n + nb;
/*<       IF (NW.GE.IPARM(8)) GO TO 210 >*/
    if (*nw >= iparm[8]) {
	goto L210;
    }
/*<       IER = 72 >*/
    ier = 72;
/*<       IF (LEVEL.GE.0) WRITE (NOUT,200) NW,IPARM(8) >*/
    if (itcom1_1.level >= 0) {
	io___259.ciunit = itcom1_1.nout;
	s_wsfe(&io___259);
	do_fio(&c__1, (char *)&(*nw), (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&iparm[8], (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 420 >*/
    goto L420;

/* ... SCALE LINEAR SYSTEM, U, AND RHS BY THE SQUARE ROOT OF THE */
/* ... DIAGONAL ELEMENTS. */

/*<   210 CONTINUE >*/
L210:
/*<       CALL VFILL (IPARM(8),WKSP,0.0D0) >*/
    vfill_(&iparm[8], &wksp[1], &c_b21);
/*<       CALL SCAL (N,IA,JA,A,RHS,U,WKSP,LEVEL,NOUT,IER) >*/
    scal_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &u[1], &wksp[1], &
	    itcom1_1.level, &itcom1_1.nout, &ier);
/*<       IF (IER.EQ.0) GO TO 230 >*/
    if (ier == 0) {
	goto L230;
    }
/*<       IF (LEVEL.GE.0) WRITE (NOUT,220) IER >*/
    if (itcom1_1.level >= 0) {
	io___260.ciunit = itcom1_1.nout;
	s_wsfe(&io___260);
	do_fio(&c__1, (char *)&ier, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 420 >*/
    goto L420;
/*<   230 IF (LEVEL.LE.2) GO TO 250 >*/
L230:
    if (itcom1_1.level <= 2) {
	goto L250;
    }
/*<       WRITE (NOUT,240) >*/
    io___261.ciunit = itcom1_1.nout;
    s_wsfe(&io___261);
    e_wsfe();
/*<    >*/
/*<   250 IF (IPARM(11).NE.0) GO TO 260 >*/
L250:
    if (iparm[11] != 0) {
	goto L260;
    }
/*<       TIMI1 = TIMER(DUMMY) >*/
    timi1 = timer_(&dummy);

/* ... ITERATION SEQUENCE */

/*<   260 IF (N.GT.1) GO TO 270 >*/
L260:
    if (n > 1) {
	goto L270;
    }
/*<       U(1) = RHS(1) >*/
    u[1] = rhs[1];
/*<       GO TO 320 >*/
    goto L320;
/*<   270 ITMAX1 = ITMAX+1 >*/
L270:
    itmax1 = itcom1_1.itmax + 1;
/*<       DO 290 LOOP = 1,ITMAX1 >*/
    i__1 = itmax1;
    for (loop = 1; loop <= i__1; ++loop) {
/*<          IN = LOOP-1 >*/
	itcom1_1.in = loop - 1;
/*<          IF (MOD(IN,2).EQ.1) GO TO 280 >*/
	if (itcom1_1.in % 2 == 1) {
	    goto L280;
	}

/* ... CODE FOR THE EVEN ITERATIONS. */

/*     U           = U(IN) */
/*     WKSP(IB1)   = U(IN-1) */

/*<          CALL ITRSSI (N,NB,IA,JA,A,RHS,U,WKSP(IB1),WKSP(IB2)) >*/
	itrssi_(&n, &nb, &ia[1], &ja[1], &a[1], &rhs[1], &u[1], &wksp[ib1], &
		wksp[ib2]);

/*<          IF (HALT) GO TO 320 >*/
	if (itcom2_1.halt) {
	    goto L320;
	}
/*<          GO TO 290 >*/
	goto L290;

/* ... CODE FOR THE ODD ITERATIONS. */

/*     U           = U(IN-1) */
/*     WKSP(IB1)   = U(IN) */

/*<   280    CALL ITRSSI (N,NB,IA,JA,A,RHS,WKSP(IB1),U,WKSP(IB2)) >*/
L280:
	itrssi_(&n, &nb, &ia[1], &ja[1], &a[1], &rhs[1], &wksp[ib1], &u[1], &
		wksp[ib2]);

/*<          IF (HALT) GO TO 320 >*/
	if (itcom2_1.halt) {
	    goto L320;
	}
/*<   290 CONTINUE >*/
L290:
	;
    }

/* ... ITMAX HAS BEEN REACHED */

/*<       IF (IPARM(11).NE.0) GO TO 300 >*/
    if (iparm[11] != 0) {
	goto L300;
    }
/*<       TIMI2 = TIMER(DUMMY) >*/
    timi2 = timer_(&dummy);
/*<       TIME1 = DBLE(TIMI2-TIMI1) >*/
    time1 = (doublereal) (timi2 - timi1);
/*<   300 IF (LEVEL.GE.1) WRITE (NOUT,310) ITMAX >*/
L300:
    if (itcom1_1.level >= 1) {
	io___266.ciunit = itcom1_1.nout;
	s_wsfe(&io___266);
	do_fio(&c__1, (char *)&itcom1_1.itmax, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       IER = 73 >*/
    ier = 73;
/*<       IF (IPARM(3).EQ.0) RPARM(1) = STPTST >*/
    if (iparm[3] == 0) {
	rparm[1] = itcom3_1.stptst;
    }
/*<       GO TO 350 >*/
    goto L350;

/* ... METHOD HAS CONVERGED */

/*<   320 IF (IPARM(11).NE.0) GO TO 330 >*/
L320:
    if (iparm[11] != 0) {
	goto L330;
    }
/*<       TIMI2 = TIMER(DUMMY) >*/
    timi2 = timer_(&dummy);
/*<       TIME1 = DBLE(TIMI2-TIMI1) >*/
    time1 = (doublereal) (timi2 - timi1);
/*<   330 IF (LEVEL.GE.1) WRITE (NOUT,340) IN >*/
L330:
    if (itcom1_1.level >= 1) {
	io___267.ciunit = itcom1_1.nout;
	s_wsfe(&io___267);
	do_fio(&c__1, (char *)&itcom1_1.in, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<   340 FORMAT (/1X,'RSSI  HAS CONVERGED IN ',I5,' ITERATIONS') >*/

/* ... PUT SOLUTION INTO U IF NOT ALREADY THERE. */

/*<   350 CONTINUE >*/
L350:
/*<       IF (N.EQ.1) GO TO 360 >*/
    if (n == 1) {
	goto L360;
    }
/*<       IF (MOD(IN,2).EQ.1) CALL DCOPY (N,WKSP(IB1),1,U,1) >*/
    if (itcom1_1.in % 2 == 1) {
	dcopy_(&n, &wksp[ib1], &c__1, &u[1], &c__1);
    }
/*<       CALL DCOPY (NR,RHS,1,U,1) >*/
    dcopy_(&nr, &rhs[1], &c__1, &u[1], &c__1);
/*<       CALL PRSRED (NB,NR,IA,JA,A,U(NRP1),U) >*/
    prsred_(&nb, &nr, &ia[1], &ja[1], &a[1], &u[nrp1], &u[1]);

/* ... UNSCALE THE MATRIX, SOLUTION, AND RHS VECTORS. */

/*<   360 CALL UNSCAL (N,IA,JA,A,RHS,U,WKSP) >*/
L360:
    unscal_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &u[1], &wksp[1]);

/* ... UN-PERMUTE MATRIX,RHS, AND SOLUTION */

/*<       IF (IPARM(9).GE.0) GO TO 390 >*/
    if (iparm[9] >= 0) {
	goto L390;
    }
/*<    >*/
    permat_(&n, &ia[1], &ja[1], &a[1], &iwksp[ib2], &iwksp[jb3], &
	    itcom1_1.isym, &itcom1_1.level, &itcom1_1.nout, &ierper);
/*<       IF (IERPER.EQ.0) GO TO 380 >*/
    if (ierper == 0) {
	goto L380;
    }
/*<       IF (LEVEL.GE.0) WRITE (NOUT,370) IERPER >*/
    if (itcom1_1.level >= 0) {
	io___269.ciunit = itcom1_1.nout;
	s_wsfe(&io___269);
	do_fio(&c__1, (char *)&ierper, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       IF (IER.EQ.0) IER = IERPER >*/
    if (ier == 0) {
	ier = ierper;
    }
/*<       GO TO 420 >*/
    goto L420;
/*<   380 CALL PERVEC (N,RHS,IWKSP(IB2)) >*/
L380:
    pervec_(&n, &rhs[1], &iwksp[ib2]);
/*<       CALL PERVEC (N,U,IWKSP(IB2)) >*/
    pervec_(&n, &u[1], &iwksp[ib2]);

/* ... OPTIONAL ERROR ANALYSIS */

/*<   390 IDGTS = IPARM(12) >*/
L390:
    idgts = iparm[12];
/*<       IF (IDGTS.LT.0) GO TO 400 >*/
    if (idgts < 0) {
	goto L400;
    }
/*<       IF (IPARM(2).LE.0) IDGTS = 0 >*/
    if (iparm[2] <= 0) {
	idgts = 0;
    }
/*<       CALL PERROR (N,IA,JA,A,RHS,U,WKSP,DIGIT1,DIGIT2,IDGTS) >*/
    perror_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &u[1], &wksp[1], &digit1, &
	    digit2, &idgts);

/* ... SET RETURN PARAMETERS IN IPARM AND RPARM */

/*<   400 IF (IPARM(11).NE.0) GO TO 410 >*/
L400:
    if (iparm[11] != 0) {
	goto L410;
    }
/*<       TIMJ2 = TIMER(DUMMY) >*/
    timj2 = timer_(&dummy);
/*<       TIME2 = DBLE(TIMJ2-TIMJ1) >*/
    time2 = (doublereal) (timj2 - timj1);
/*<   410 IF (IPARM(3).NE.0) GO TO 420 >*/
L410:
    if (iparm[3] != 0) {
	goto L420;
    }
/*<       IPARM(1) = IN >*/
    iparm[1] = itcom1_1.in;
/*<       IPARM(9) = NB >*/
    iparm[9] = nb;
/*<       RPARM(2) = CME >*/
    rparm[2] = itcom3_1.cme;
/*<       RPARM(3) = SME >*/
    rparm[3] = itcom3_1.sme;
/*<       RPARM(9) = TIME1 >*/
    rparm[9] = time1;
/*<       RPARM(10) = TIME2 >*/
    rparm[10] = time2;
/*<       RPARM(11) = DIGIT1 >*/
    rparm[11] = digit1;
/*<       RPARM(12) = DIGIT2 >*/
    rparm[12] = digit2;

/*<   420 CONTINUE >*/
L420:
/*<       IERR = IER >*/
    *ierr = ier;
/*<       IF (LEVEL.GE.3) CALL ECHALL (N,IA,JA,A,RHS,IPARM,RPARM,2) >*/
    if (itcom1_1.level >= 3) {
	echall_(&n, &ia[1], &ja[1], &a[1], &rhs[1], &iparm[1], &rparm[1], &
		c__2);
    }

/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* rssi_ */

/*<       SUBROUTINE ITJCG (NN,IA,JA,A,U,U1,D,D1,DTWD,TRI) >*/
/* Subroutine */ int itjcg_(integer *nn, integer *ia, integer *ja, doublereal 
	*a, doublereal *u, doublereal *u1, doublereal *d__, doublereal *d1, 
	doublereal *dtwd, doublereal *tri)
{
    integer n;
    doublereal c1, c2, c3, c4;
    logical q1;
    doublereal con;
    extern /* Subroutine */ int sum3_(integer *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *), pjac_(
	    integer *, integer *, integer *, doublereal *, doublereal *, 
	    doublereal *);
    extern doublereal ddot_(integer *, doublereal *, integer *, doublereal *, 
	    integer *);
    doublereal dnrm;
    extern /* Subroutine */ int vfill_(integer *, doublereal *, doublereal *),
	     iterm_(integer *, doublereal *, doublereal *, doublereal *, 
	    integer *);
    doublereal dtnrm;
    extern /* Subroutine */ int pstop_(integer *, doublereal *, doublereal *, 
	    doublereal *, integer *, logical *), chgcon_(doublereal *, 
	    doublereal *, doublereal *, integer *);
    doublereal gamold;
    extern /* Subroutine */ int parcon_(doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, integer *);
    doublereal rhoold, rhotmp;


/* ... FUNCTION: */

/*          THIS SUBROUTINE, ITJCG, PERFORMS ONE ITERATION OF THE */
/*          JACOBI CONJUGATE GRADIENT ALGORITHM.  IT IS CALLED BY JCG. */

/* ... PARAMETER LIST: */

/*          N      INPUT INTEGER.  DIMENSION OF THE MATRIX. (= NN) */
/*          IA,JA  INPUT INTEGER VECTORS.  CONTAINS INFORMATION DEFINING */
/*                 THE SPARSE MATRIX REPRESENTATION. */
/*          A      INPUT D.P. VECTOR. CONTAINS THE NONZERO VALUES OF THE */
/*                 LINEAR SYSTEM. */
/*          U      INPUT D.P. VECTOR.  CONTAINS THE VALUE OF THE */
/*                 SOLUTION VECTOR AT THE END OF IN ITERATIONS. */
/*          U1     INPUT/OUTPUT D.P. VECTOR.  ON INPUT, IT CONTAINS */
/*                 THE VALUE OF THE SOLUTION AT THE END OF THE IN-1 */
/*                 ITERATION.  ON OUTPUT, IT WILL CONTAIN THE NEWEST */
/*                 ESTIMATE FOR THE SOLUTION VECTOR. */
/*          D      INPUT D.P. VECTOR.  CONTAINS THE PSEUDO-RESIDUAL */
/*                 VECTOR AFTER IN ITERATIONS. */
/*          D1     INPUT/OUTPUT D.P. VECTOR.  ON INPUT, D1 CONTAINS */
/*                 THE PSEUDO-RESIDUAL VECTOR AFTER IN-1 ITERATIONS.  ON */
/*                 OUTPUT, IT WILL CONTAIN THE NEWEST PSEUDO-RESIDUAL */
/*                 VECTOR. */
/*          DTWD   D.P. ARRAY.  USED IN THE COMPUTATIONS OF THE */
/*                 ACCELERATION PARAMETER GAMMA AND THE NEW PSEUDO- */
/*                 RESIDUAL. */
/*          TRI    D.P. ARRAY.  STORES THE TRIDIAGONAL MATRIX ASSOCIATED */
/*                 WITH THE EIGENVALUES OF THE CONJUGATE GRADIENT */
/*                 POLYNOMIAL. */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER IA(1),JA(1),NN >*/
/*<       DOUBLE PRECISION A(1),U(NN),U1(NN),D(NN),D1(NN),DTWD(NN),TRI(2,1) >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER N >*/
/*<       DOUBLE PRECISION CON,C1,C2,C3,C4,DNRM,DTNRM,GAMOLD,RHOOLD,RHOTMP >*/
/*<       LOGICAL Q1 >*/

/* ... SPECIFICATIONS FOR FUNCTION SUBPROGRAMS */

/*<       DOUBLE PRECISION DDOT >*/

/* *** BEGIN: ITPACK COMMON */

/*<       INTEGER IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/
/*<       COMMON /ITCOM1/ IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/

/*<       LOGICAL ADAPT,BETADT,CASEII,HALT,PARTAD >*/
/*<       COMMON /ITCOM2/ ADAPT,BETADT,CASEII,HALT,PARTAD >*/

/*<    >*/
/*<    >*/

/* *** END  : ITPACK COMMON */

/*     DESCRIPTION OF VARIABLES IN COMMON BLOCKS IN SUBROUTINE JCG */

/* ... COMPUTE NEW ESTIMATE FOR CME IF ADAPT = .TRUE. */

/*<       IF (ADAPT) CALL CHGCON (TRI,GAMOLD,RHOOLD,1) >*/
    /* Parameter adjustments */
    --dtwd;
    --d1;
    --d__;
    --u1;
    --u;
    --ia;
    --ja;
    --a;
    tri -= 3;

    /* Function Body */
    if (itcom2_1.adapt) {
	chgcon_(&tri[3], &gamold, &rhoold, &c__1);
    }

/* ... TEST FOR STOPPING */

/*<       N = NN >*/
    n = *nn;
/*<       DELNNM = DDOT(N,D,1,D,1) >*/
    itcom3_1.delnnm = ddot_(&n, &d__[1], &c__1, &d__[1], &c__1);
/*<       DNRM = DELNNM >*/
    dnrm = itcom3_1.delnnm;
/*<       CON = CME >*/
    con = itcom3_1.cme;
/*<       CALL PSTOP (N,U,DNRM,CON,1,Q1) >*/
    pstop_(&n, &u[1], &dnrm, &con, &c__1, &q1);
/*<       IF (HALT) GO TO 30 >*/
    if (itcom2_1.halt) {
	goto L30;
    }

/* ... COMPUTE RHO AND GAMMA - ACCELERATION PARAMETERS */

/*<       CALL VFILL (N,DTWD,0.D0) >*/
    vfill_(&n, &dtwd[1], &c_b21);
/*<       CALL PJAC (N,IA,JA,A,D,DTWD) >*/
    pjac_(&n, &ia[1], &ja[1], &a[1], &d__[1], &dtwd[1]);
/*<       DTNRM = DDOT(N,D,1,DTWD,1) >*/
    dtnrm = ddot_(&n, &d__[1], &c__1, &dtwd[1], &c__1);
/*<       IF (ISYM.EQ.0) GO TO 10 >*/
    if (itcom1_1.isym == 0) {
	goto L10;
    }
/*<       RHOTMP = DDOT(N,DTWD,1,D1,1) >*/
    rhotmp = ddot_(&n, &dtwd[1], &c__1, &d1[1], &c__1);
/*<       CALL PARCON (DTNRM,C1,C2,C3,C4,GAMOLD,RHOTMP,1) >*/
    parcon_(&dtnrm, &c1, &c2, &c3, &c4, &gamold, &rhotmp, &c__1);
/*<       RHOOLD = RHOTMP >*/
    rhoold = rhotmp;
/*<       GO TO 20 >*/
    goto L20;
/*<    10 CALL PARCON (DTNRM,C1,C2,C3,C4,GAMOLD,RHOOLD,1) >*/
L10:
    parcon_(&dtnrm, &c1, &c2, &c3, &c4, &gamold, &rhoold, &c__1);

/* ... COMPUTE U(IN+1) AND D(IN+1) */

/*<    20 CALL SUM3 (N,C1,D,C2,U,C3,U1) >*/
L20:
    sum3_(&n, &c1, &d__[1], &c2, &u[1], &c3, &u1[1]);
/*<       CALL SUM3 (N,C1,DTWD,C4,D,C3,D1) >*/
    sum3_(&n, &c1, &dtwd[1], &c4, &d__[1], &c3, &d1[1]);

/* ... OUTPUT INTERMEDIATE INFORMATION */

/*<    30 CALL ITERM (N,A,U,DTWD,1) >*/
L30:
    iterm_(&n, &a[1], &u[1], &dtwd[1], &c__1);

/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* itjcg_ */

/*<       SUBROUTINE ITJSI (NN,IA,JA,A,RHS,U,U1,D,ICNT) >*/
/* Subroutine */ int itjsi_(integer *nn, integer *ia, integer *ja, doublereal 
	*a, doublereal *rhs, doublereal *u, doublereal *u1, doublereal *d__, 
	integer *icnt)
{
    integer n;
    doublereal c1, c2, c3;
    logical q1;
    doublereal con;
    extern /* Subroutine */ int sum3_(integer *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *), pjac_(
	    integer *, integer *, integer *, doublereal *, doublereal *, 
	    doublereal *);
    extern doublereal ddot_(integer *, doublereal *, integer *, doublereal *, 
	    integer *);
    doublereal dnrm;
    extern /* Subroutine */ int chgsi_(doublereal *, integer *), dcopy_(
	    integer *, doublereal *, integer *, doublereal *, integer *), 
	    parsi_(doublereal *, doublereal *, doublereal *, integer *), 
	    iterm_(integer *, doublereal *, doublereal *, doublereal *, 
	    integer *);
    doublereal dtnrm;
    extern /* Subroutine */ int daxpy_(integer *, doublereal *, doublereal *, 
	    integer *, doublereal *, integer *);
    extern doublereal pvtbv_(integer *, integer *, integer *, doublereal *, 
	    doublereal *);
    extern /* Subroutine */ int vevmw_(integer *, doublereal *, doublereal *),
	     pstop_(integer *, doublereal *, doublereal *, doublereal *, 
	    integer *, logical *);
    extern logical chgsme_(doublereal *, integer *);
    doublereal oldnrm;
    extern logical tstchg_(integer *);


/* ... FUNCTION: */

/*          THIS SUBROUTINE, ITJSI, PERFORMS ONE ITERATION OF THE */
/*          JACOBI SEMI-ITERATIVE ALGORITHM.  IT IS CALLED BY JSI. */

/* ... PARAMETER LIST: */

/*          N      INPUT INTEGER.  DIMENSION OF THE MATRIX. (= NN) */
/*          IA,JA  INPUT INTEGER VECTORS.  THE TWO INTEGER ARRAYS OF */
/*                 THE SPARSE MATRIX REPRESENTATION. */
/*          A      INPUT D.P. VECTOR.  THE D.P. ARRAY OF THE SPARSE */
/*                 MATRIX REPRESENTATION. */
/*          RHS    INPUT D.P. VECTOR.  CONTAINS THE RIGHT HAND SIDE */
/*                 OF THE MATRIX PROBLEM. */
/*          U      INPUT D.P. VECTOR.  CONTAINS THE ESTIMATE FOR THE */
/*                 SOLUTION VECTOR AFTER IN ITERATIONS. */
/*          U1     INPUT/OUTPUT D.P. VECTOR.  ON INPUT, U1 CONTAINS THE */
/*                 SOLUTION VECTOR AFTER IN-1 ITERATIONS.  ON OUTPUT, */
/*                 IT WILL CONTAIN THE NEWEST ESTIMATE FOR THE SOLUTION */
/*                 VECTOR. */
/*          D      D.P. ARRAY.  D IS USED FOR THE COMPUTATION OF THE */
/*                 PSEUDO-RESIDUAL ARRAY FOR THE CURRENT ITERATION. */
/*          ICNT   NUMBER OF ITERATIONS SINCE LAST CHANGE OF SME */

/* ... SPECIFICATIONS OF ARGUMENTS */

/*<       INTEGER IA(1),JA(1),NN,ICNT >*/
/*<       DOUBLE PRECISION A(1),RHS(NN),U(NN),U1(NN),D(NN) >*/

/* ... SPECIFICATIONS OF LOCAL VARIABLES */

/*<       INTEGER N >*/
/*<       DOUBLE PRECISION CON,C1,C2,C3,DNRM,DTNRM,OLDNRM >*/
/*<       LOGICAL Q1 >*/

/* ... SPECIFICATIONS OF FUNCTION SUBPROGRAMS */

/*<       DOUBLE PRECISION DDOT,PVTBV >*/
/*<       LOGICAL TSTCHG,CHGSME >*/

/* *** BEGIN: ITPACK COMMON */

/*<       INTEGER IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/
/*<       COMMON /ITCOM1/ IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/

/*<       LOGICAL ADAPT,BETADT,CASEII,HALT,PARTAD >*/
/*<       COMMON /ITCOM2/ ADAPT,BETADT,CASEII,HALT,PARTAD >*/

/*<    >*/
/*<    >*/

/* *** END  : ITPACK COMMON */

/*     DESCRIPTION OF VARIABLES IN COMMON BLOCKS IN SUBROUTINE JSI */

/*<       N = NN >*/
    /* Parameter adjustments */
    --d__;
    --u1;
    --u;
    --rhs;
    --ia;
    --ja;
    --a;

    /* Function Body */
    n = *nn;
/*<       IF (IN.EQ.0) ICNT = 0 >*/
    if (itcom1_1.in == 0) {
	*icnt = 0;
    }

/* ... COMPUTE PSEUDO-RESIDUALS */

/*<       CALL DCOPY (N,RHS,1,D,1) >*/
    dcopy_(&n, &rhs[1], &c__1, &d__[1], &c__1);
/*<       CALL PJAC (N,IA,JA,A,U,D) >*/
    pjac_(&n, &ia[1], &ja[1], &a[1], &u[1], &d__[1]);
/*<       CALL VEVMW (N,D,U) >*/
    vevmw_(&n, &d__[1], &u[1]);

/* ... STOPPING AND ADAPTIVE CHANGE TESTS */

/*<       OLDNRM = DELNNM >*/
    oldnrm = itcom3_1.delnnm;
/*<       DELNNM = DDOT(N,D,1,D,1) >*/
    itcom3_1.delnnm = ddot_(&n, &d__[1], &c__1, &d__[1], &c__1);
/*<       DNRM = DELNNM >*/
    dnrm = itcom3_1.delnnm;
/*<       CON = CME >*/
    con = itcom3_1.cme;
/*<       CALL PSTOP (N,U,DNRM,CON,1,Q1) >*/
    pstop_(&n, &u[1], &dnrm, &con, &c__1, &q1);
/*<       IF (HALT) GO TO 40 >*/
    if (itcom2_1.halt) {
	goto L40;
    }
/*<       IF (.NOT.ADAPT) GO TO 30 >*/
    if (! itcom2_1.adapt) {
	goto L30;
    }
/*<       IF (.NOT.TSTCHG(1)) GO TO 10 >*/
    if (! tstchg_(&c__1)) {
	goto L10;
    }

/* ... CHANGE ITERATIVE PARAMETERS (CME) */

/*<       DTNRM = PVTBV(N,IA,JA,A,D) >*/
    dtnrm = pvtbv_(&n, &ia[1], &ja[1], &a[1], &d__[1]);
/*<       CALL CHGSI (DTNRM,1) >*/
    chgsi_(&dtnrm, &c__1);
/*<       IF (.NOT.ADAPT) GO TO 30 >*/
    if (! itcom2_1.adapt) {
	goto L30;
    }
/*<       GO TO 20 >*/
    goto L20;

/* ... TEST IF SME NEEDS TO BE CHANGED AND CHANGE IF NECESSARY. */

/*<    10 CONTINUE >*/
L10:
/*<       IF (CASEII) GO TO 30 >*/
    if (itcom2_1.caseii) {
	goto L30;
    }
/*<       IF (.NOT.CHGSME(OLDNRM,ICNT)) GO TO 30 >*/
    if (! chgsme_(&oldnrm, icnt)) {
	goto L30;
    }
/*<       ICNT = 0 >*/
    *icnt = 0;

/* ... COMPUTE U(IN+1) AFTER CHANGE OF PARAMETERS */

/*<    20 CALL DCOPY (N,U,1,U1,1) >*/
L20:
    dcopy_(&n, &u[1], &c__1, &u1[1], &c__1);
/*<       CALL DAXPY (N,GAMMA,D,1,U1,1) >*/
    daxpy_(&n, &itcom3_1.gamma, &d__[1], &c__1, &u1[1], &c__1);
/*<       GO TO 40 >*/
    goto L40;

/* ... COMPUTE U(IN+1) WITHOUT CHANGE OF PARAMETERS */

/*<    30 CALL PARSI (C1,C2,C3,1) >*/
L30:
    parsi_(&c1, &c2, &c3, &c__1);
/*<       CALL SUM3 (N,C1,D,C2,U,C3,U1) >*/
    sum3_(&n, &c1, &d__[1], &c2, &u[1], &c3, &u1[1]);

/* ... OUTPUT INTERMEDIATE INFORMATION */

/*<    40 CALL ITERM (N,A,U,D,2) >*/
L40:
    iterm_(&n, &a[1], &u[1], &d__[1], &c__2);

/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* itjsi_ */

/*<       SUBROUTINE ITSOR (NN,IA,JA,A,RHS,U,WK) >*/
/* Subroutine */ int itsor_(integer *nn, integer *ia, integer *ja, doublereal 
	*a, doublereal *rhs, doublereal *u, doublereal *wk)
{
    /* System generated locals */
    integer i__1, i__2;
    doublereal d__1, d__2;

    /* Builtin functions */
    double sqrt(doublereal), pow_dd(doublereal *, doublereal *);

    /* Local variables */
    doublereal h__;
    integer n;
    logical q1;
    integer ip;
    extern doublereal tau_(integer *);
    integer iss;
    doublereal dnrm;
    integer iphat;
    extern /* Subroutine */ int dcopy_(integer *, doublereal *, integer *, 
	    doublereal *, integer *), iterm_(integer *, doublereal *, 
	    doublereal *, doublereal *, integer *);
    extern integer ipstr_(doublereal *);
    extern /* Subroutine */ int pstop_(integer *, doublereal *, doublereal *, 
	    doublereal *, integer *, logical *);
    doublereal spcrm1;
    extern /* Subroutine */ int pfsor1_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *);
    logical change;
    doublereal omegap;
    integer ipstar;


/* ... FUNCTION: */

/*          THIS SUBROUTINE, ITSOR, PERFORMS ONE ITERATION OF THE */
/*          SUCCESSIVE OVERRELAXATION ALGORITHM.  IT IS CALLED BY SOR. */

/* ... PARAMETER LIST: */

/*          N      INPUT INTEGER.  DIMENSION OF THE MATRIX. (= NN) */
/*          IA,JA  INPUT INTEGER VECTORS.  THE TWO INTEGER ARRAYS OF */
/*                 THE SPARSE MATRIX REPRESENTATION. */
/*          A      INPUT D.P. VECTOR.  THE D.P. ARRAY OF THE SPARSE */
/*                 MATRIX REPRESENTATION. */
/*          RHS    INPUT D.P. VECTOR.  CONTAINS THE RIGHT HAND SIDE */
/*                 OF THE MATRIX PROBLEM. */
/*          U      INPUT/OUTPUT D.P. VECTOR.  ON INPUT, U CONTAINS THE */
/*                 SOLUTION VECTOR AFTER IN ITERATIONS.  ON OUTPUT, */
/*                 IT WILL CONTAIN THE NEWEST ESTIMATE FOR THE SOLUTION */
/*                 VECTOR. */
/*          WK     D.P. ARRAY.  WORK VECTOR OF LENGTH N. */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER IA(1),JA(1),NN >*/
/*<       DOUBLE PRECISION A(1),RHS(NN),U(NN),WK(NN) >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER IP,IPHAT,IPSTAR,ISS,N >*/
/*<       DOUBLE PRECISION DNRM,H,OMEGAP,SPCRM1 >*/
/*<       LOGICAL CHANGE,Q1 >*/

/*<       DOUBLE PRECISION TAU >*/

/* *** BEGIN: ITPACK COMMON */

/*<       INTEGER IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/
/*<       COMMON /ITCOM1/ IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/

/*<       LOGICAL ADAPT,BETADT,CASEII,HALT,PARTAD >*/
/*<       COMMON /ITCOM2/ ADAPT,BETADT,CASEII,HALT,PARTAD >*/

/*<    >*/
/*<    >*/

/* *** END  : ITPACK COMMON */

/*     DESCRIPTION OF VARIABLES IN COMMON BLOCKS IN SUBROUTINE SOR */

/* ... SET INITIAL PARAMETERS NOT ALREADY SET */

/*<       N = NN >*/
    /* Parameter adjustments */
    --wk;
    --u;
    --rhs;
    --ia;
    --ja;
    --a;

    /* Function Body */
    n = *nn;
/*<       IF (IN.NE.0) GO TO 20 >*/
    if (itcom1_1.in != 0) {
	goto L20;
    }
/*<       CALL PSTOP (N,U,0.D0,0.D0,0,Q1) >*/
    pstop_(&n, &u[1], &c_b21, &c_b21, &c__0, &q1);
/*<       IF (ADAPT) GO TO 10 >*/
    if (itcom2_1.adapt) {
	goto L10;
    }
/*<       CHANGE = .FALSE. >*/
    change = FALSE_;
/*<       IP = 0 >*/
    ip = 0;
/*<       IPHAT = 2 >*/
    iphat = 2;
/*<       ISS = 0 >*/
    iss = 0;
/*<       GO TO 30 >*/
    goto L30;

/*<    10 CHANGE = .TRUE. >*/
L10:
    change = TRUE_;
/*<       IP = 0 >*/
    ip = 0;
/*<       OMEGAP = OMEGA >*/
    omegap = itcom3_1.omega;
/*<       OMEGA = 1.D0 >*/
    itcom3_1.omega = 1.;
/*<       ISS = 0 >*/
    iss = 0;
/*<       IPHAT = 2 >*/
    iphat = 2;
/*<       IPSTAR = 4 >*/
    ipstar = 4;
/*<       IF (OMEGAP.LE.1.D0) CHANGE = .FALSE. >*/
    if (omegap <= 1.) {
	change = FALSE_;
    }

/* ... RESET OMEGA, IPHAT, AND IPSTAR (CIRCLE A IN FLOWCHART) */

/*<    20 IF (.NOT.CHANGE) GO TO 30 >*/
L20:
    if (! change) {
	goto L30;
    }
/*<       CHANGE = .FALSE. >*/
    change = FALSE_;
/*<       IS = IS+1 >*/
    ++itcom1_1.is;
/*<       IP = 0 >*/
    ip = 0;
/*<       ISS = 0 >*/
    iss = 0;
/*<       OMEGA = DMIN1(OMEGAP,TAU(IS)) >*/
/* Computing MIN */
    d__1 = omegap, d__2 = tau_(&itcom1_1.is);
    itcom3_1.omega = min(d__1,d__2);
/*<       IPHAT = MAX0(3,IFIX(SNGL((OMEGA-1.D0)/(2.D0-OMEGA)))) >*/
/* Computing MAX */
    i__1 = 3, i__2 = (integer) ((real) ((itcom3_1.omega - 1.) / (2. - 
	    itcom3_1.omega)));
    iphat = max(i__1,i__2);
/*<       IPSTAR = IPSTR(OMEGA) >*/
    ipstar = ipstr_(&itcom3_1.omega);

/* ... COMPUTE U (IN + 1) AND NORM OF DEL(S,P) - CIRCLE B IN FLOW CHART */

/*<    30 CONTINUE >*/
L30:
/*<       DELSNM = DELNNM >*/
    itcom3_1.delsnm = itcom3_1.delnnm;
/*<       SPCRM1 = SPECR >*/
    spcrm1 = itcom3_1.specr;
/*<       CALL DCOPY (N,RHS,1,WK,1) >*/
    dcopy_(&n, &rhs[1], &c__1, &wk[1], &c__1);
/*<       CALL PFSOR1 (N,IA,JA,A,U,WK) >*/
    pfsor1_(&n, &ia[1], &ja[1], &a[1], &u[1], &wk[1]);
/*<       IF (DELNNM.EQ.0.D0) GO TO 40 >*/
    if (itcom3_1.delnnm == 0.) {
	goto L40;
    }
/*<       IF (IN.NE.0) SPECR = DELNNM/DELSNM >*/
    if (itcom1_1.in != 0) {
	itcom3_1.specr = itcom3_1.delnnm / itcom3_1.delsnm;
    }
/*<       IF (IP.LT.IPHAT) GO TO 70 >*/
    if (ip < iphat) {
	goto L70;
    }

/* ... STOPPING TEST, SET H */

/*<       IF (SPECR.GE.1.D0) GO TO 70 >*/
    if (itcom3_1.specr >= 1.) {
	goto L70;
    }
/*<       IF (.NOT.(SPECR.GT.(OMEGA-1.D0))) GO TO 40 >*/
    if (! (itcom3_1.specr > itcom3_1.omega - 1.)) {
	goto L40;
    }
/*<       H = SPECR >*/
    h__ = itcom3_1.specr;
/*<       GO TO 50 >*/
    goto L50;
/*<    40 ISS = ISS+1 >*/
L40:
    ++iss;
/*<       H = OMEGA-1.D0 >*/
    h__ = itcom3_1.omega - 1.;

/* ... PERFORM STOPPING TEST. */

/*<    50 CONTINUE >*/
L50:
/*<       DNRM = DELNNM**2 >*/
/* Computing 2nd power */
    d__1 = itcom3_1.delnnm;
    dnrm = d__1 * d__1;
/*<       CALL PSTOP (N,U,DNRM,H,1,Q1) >*/
    pstop_(&n, &u[1], &dnrm, &h__, &c__1, &q1);
/*<       IF (HALT) GO TO 70 >*/
    if (itcom2_1.halt) {
	goto L70;
    }

/* ... METHOD HAS NOT CONVERGED YET, TEST FOR CHANGING OMEGA */

/*<       IF (.NOT.ADAPT) GO TO 70 >*/
    if (! itcom2_1.adapt) {
	goto L70;
    }
/*<       IF (IP.LT.IPSTAR) GO TO 70 >*/
    if (ip < ipstar) {
	goto L70;
    }
/*<       IF (OMEGA.GT.1.D0) GO TO 60 >*/
    if (itcom3_1.omega > 1.) {
	goto L60;
    }
/*<       CME = DSQRT(DABS(SPECR)) >*/
    itcom3_1.cme = sqrt((abs(itcom3_1.specr)));
/*<       OMEGAP = 2.D0/(1.D0+DSQRT(DABS(1.D0-SPECR))) >*/
    omegap = 2. / (sqrt((d__1 = 1. - itcom3_1.specr, abs(d__1))) + 1.);
/*<       CHANGE = .TRUE. >*/
    change = TRUE_;
/*<       GO TO 70 >*/
    goto L70;
/*<    60 IF (ISS.NE.0) GO TO 70 >*/
L60:
    if (iss != 0) {
	goto L70;
    }
/*<       IF (SPECR.LE.(OMEGA-1.D0)**FF) GO TO 70 >*/
    d__1 = itcom3_1.omega - 1.;
    if (itcom3_1.specr <= pow_dd(&d__1, &itcom3_1.ff)) {
	goto L70;
    }
/*<       IF ((SPECR+5.D-5).LE.SPCRM1) GO TO 70 >*/
    if (itcom3_1.specr + 5e-5 <= spcrm1) {
	goto L70;
    }

/* ... CHANGE PARAMETERS */

/*<       CME = (SPECR+OMEGA-1.D0)/(DSQRT(DABS(SPECR))*OMEGA) >*/
    itcom3_1.cme = (itcom3_1.specr + itcom3_1.omega - 1.) / (sqrt((abs(
	    itcom3_1.specr))) * itcom3_1.omega);
/*<       OMEGAP = 2.D0/(1.D0+DSQRT(DABS(1.D0-CME*CME))) >*/
    omegap = 2. / (sqrt((d__1 = 1. - itcom3_1.cme * itcom3_1.cme, abs(d__1))) 
	    + 1.);
/*<       CHANGE = .TRUE. >*/
    change = TRUE_;

/* ... OUTPUT INTERMEDIATE INFORMATION */

/*<    70 CALL ITERM (N,A,U,WK,3) >*/
L70:
    iterm_(&n, &a[1], &u[1], &wk[1], &c__3);
/*<       IP = IP+1 >*/
    ++ip;

/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* itsor_ */

/*<       SUBROUTINE ITSRCG (NN,IA,JA,A,RHS,U,U1,C,C1,D,DL,WK,TRI) >*/
/* Subroutine */ int itsrcg_(integer *nn, integer *ia, integer *ja, 
	doublereal *a, doublereal *rhs, doublereal *u, doublereal *u1, 
	doublereal *c__, doublereal *c1, doublereal *d__, doublereal *dl, 
	doublereal *wk, doublereal *tri)
{
    /* System generated locals */
    doublereal d__1;

    /* Local variables */
    integer n;
    logical q1;
    doublereal t1, t2, t3, t4, con;
    extern /* Subroutine */ int sum3_(integer *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *), pjac_(
	    integer *, integer *, integer *, doublereal *, doublereal *, 
	    doublereal *), omeg_(doublereal *, integer *);
    extern doublereal ddot_(integer *, doublereal *, integer *, doublereal *, 
	    integer *);
    doublereal dnrm;
    extern doublereal pbeta_(integer *, integer *, integer *, doublereal *, 
	    doublereal *, doublereal *, doublereal *);
    extern /* Subroutine */ int vfill_(integer *, doublereal *, doublereal *),
	     dcopy_(integer *, doublereal *, integer *, doublereal *, integer 
	    *), iterm_(integer *, doublereal *, doublereal *, doublereal *, 
	    integer *), pbsor_(integer *, integer *, integer *, doublereal *, 
	    doublereal *, doublereal *), pfsor_(integer *, integer *, integer 
	    *, doublereal *, doublereal *, doublereal *);
    extern doublereal pvtbv_(integer *, integer *, integer *, doublereal *, 
	    doublereal *);
    extern /* Subroutine */ int vevmw_(integer *, doublereal *, doublereal *),
	     wevmw_(integer *, doublereal *, doublereal *), pstop_(integer *, 
	    doublereal *, doublereal *, doublereal *, integer *, logical *), 
	    vevpw_(integer *, doublereal *, doublereal *), chgcon_(doublereal 
	    *, doublereal *, doublereal *, integer *);
    doublereal gamold;
    extern logical omgchg_(integer *);
    extern /* Subroutine */ int parcon_(doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, integer *);
    doublereal betnew, rhoold, rhotmp;
    extern logical omgstr_(integer *);


/* ... FUNCTION: */

/*          THIS SUBROUTINE, ITSRCG, PERFORMS ONE ITERATION OF THE */
/*          SYMMETRIC SOR CONJUGATE GRADIENT ALGORITHM.  IT IS CALLED BY */
/*          SSORCG. */

/* ... PARAMETER LIST: */

/*          N      INPUT INTEGER.  DIMENSION OF THE MATRIX. (= NN) */
/*          IA,JA  INPUT INTEGER VECTORS.  THE TWO INTEGER ARRAYS OF */
/*                 THE SPARSE MATRIX REPRESENTATION. */
/*          A      INPUT D.P. VECTOR.  THE D.P. ARRAY OF THE SPARSE */
/*                 MATRIX REPRESENTATION. */
/*          RHS    INPUT D.P. VECTOR.  CONTAINS THE RIGHT HAND SIDE */
/*                 OF THE MATRIX PROBLEM. */
/*          U      INPUT D.P. VECTOR.  CONTAINS THE ESTIMATE OF THE */
/*                 SOLUTION VECTOR AFTER IN ITERATIONS. */
/*          U1     INPUT/OUTPUT D.P. VECTOR.  ON INPUT, U1 CONTAINS THE */
/*                 THE ESTIMATE FOR THE SOLUTION AFTER IN-1 ITERATIONS. */
/*                 ON OUTPUT, U1 CONTAINS THE UPDATED ESTIMATE. */
/*          C      INPUT D.P. VECTOR.  CONTAINS THE FORWARD RESIDUAL */
/*                 AFTER IN ITERATIONS. */
/*          C1     INPUT/OUTPUT D.P. VECTOR.  ON INPUT, C1 CONTAINS */
/*                 THE FORWARD RESIDUAL AFTER IN-1 ITERATIONS.  ON */
/*                 OUTPUT, C1 CONTAINS THE UPDATED FORWARD RESIDUAL. */
/*          D      D.P. VECTOR.  IS USED TO COMPUTE THE BACKWARD PSEUDO- */
/*                 RESIDUAL VECTOR FOR THE CURRENT ITERATION. */
/*          DL     D.P. VECTOR.  IS USED IN THE COMPUTATIONS OF THE */
/*                 ACCELERATION PARAMETERS. */
/*          WK     D.P. VECTOR.  WORKING SPACE OF LENGTH N. */
/*          TRI    D.P. VECTOR. STORES THE TRIDIAGONAL MATRIX ASSOCIATED */
/*                 WITH THE CONJUGATE GRADIENT ACCELERATION. */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER IA(1),JA(1),NN >*/
/*<    >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER N >*/
/*<       DOUBLE PRECISION BETNEW,CON,DNRM,GAMOLD,RHOOLD,RHOTMP,T1,T2,T3,T4 >*/
/*<       LOGICAL Q1 >*/

/* ... SPECIFICATIONS FOR FUNCTION SUBPROGRAMS */

/*<       DOUBLE PRECISION DDOT,PBETA,PVTBV >*/
/*<       LOGICAL OMGCHG,OMGSTR >*/

/* *** BEGIN: ITPACK COMMON */

/*<       INTEGER IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/
/*<       COMMON /ITCOM1/ IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/

/*<       LOGICAL ADAPT,BETADT,CASEII,HALT,PARTAD >*/
/*<       COMMON /ITCOM2/ ADAPT,BETADT,CASEII,HALT,PARTAD >*/

/*<    >*/
/*<    >*/

/* *** END  : ITPACK COMMON */

/*     DESCRIPTION OF VARIABLES IN COMMON BLOCKS IN SUBROUTINE SSORCG */

/* ... CALCULATE S-PRIME FOR ADAPTIVE PROCEDURE. */

/*<       N = NN >*/
    /* Parameter adjustments */
    --wk;
    --dl;
    --d__;
    --c1;
    --c__;
    --u1;
    --u;
    --rhs;
    --ia;
    --ja;
    --a;
    tri -= 3;

    /* Function Body */
    n = *nn;
/*<       IF (ADAPT.OR.PARTAD) CALL CHGCON (TRI,GAMOLD,RHOOLD,3) >*/
    if (itcom2_1.adapt || itcom2_1.partad) {
	chgcon_(&tri[3], &gamold, &rhoold, &c__3);
    }

/* ... COMPUTE BACKWARD RESIDUAL */

/*<       CALL DCOPY (N,RHS,1,WK,1) >*/
    dcopy_(&n, &rhs[1], &c__1, &wk[1], &c__1);
/*<       CALL DCOPY (N,C,1,D,1) >*/
    dcopy_(&n, &c__[1], &c__1, &d__[1], &c__1);
/*<       CALL VEVPW (N,D,U) >*/
    vevpw_(&n, &d__[1], &u[1]);
/*<       CALL PBSOR (N,IA,JA,A,D,WK) >*/
    pbsor_(&n, &ia[1], &ja[1], &a[1], &d__[1], &wk[1]);
/*<       CALL VEVMW (N,D,U) >*/
    vevmw_(&n, &d__[1], &u[1]);

/* ... COMPUTE ACCELERATION PARAMETERS AND THEN U(IN+1) (IN U1) */

/*<       CALL DCOPY (N,D,1,DL,1) >*/
    dcopy_(&n, &d__[1], &c__1, &dl[1], &c__1);
/*<       CALL VFILL (N,WK,0.D0) >*/
    vfill_(&n, &wk[1], &c_b21);
/*<       CALL PFSOR (N,IA,JA,A,DL,WK) >*/
    pfsor_(&n, &ia[1], &ja[1], &a[1], &dl[1], &wk[1]);
/*<       CALL WEVMW (N,D,DL) >*/
    wevmw_(&n, &d__[1], &dl[1]);
/*<       DELNNM = DDOT(N,C,1,C,1) >*/
    itcom3_1.delnnm = ddot_(&n, &c__[1], &c__1, &c__[1], &c__1);
/*<       IF (DELNNM.EQ.0.D0) GO TO 30 >*/
    if (itcom3_1.delnnm == 0.) {
	goto L30;
    }
/*<       DNRM = DDOT(N,C,1,DL,1) >*/
    dnrm = ddot_(&n, &c__[1], &c__1, &dl[1], &c__1);
/*<       IF (DNRM.EQ.0.D0) GO TO 30 >*/
    if (dnrm == 0.) {
	goto L30;
    }
/*<       IF (ISYM.EQ.0) GO TO 10 >*/
    if (itcom1_1.isym == 0) {
	goto L10;
    }
/*<       RHOTMP = DDOT(N,C,1,C1,1)-DDOT(N,DL,1,C1,1) >*/
    rhotmp = ddot_(&n, &c__[1], &c__1, &c1[1], &c__1) - ddot_(&n, &dl[1], &
	    c__1, &c1[1], &c__1);
/*<       CALL PARCON (DNRM,T1,T2,T3,T4,GAMOLD,RHOTMP,3) >*/
    parcon_(&dnrm, &t1, &t2, &t3, &t4, &gamold, &rhotmp, &c__3);
/*<       RHOOLD = RHOTMP >*/
    rhoold = rhotmp;
/*<       GO TO 20 >*/
    goto L20;
/*<    10 CALL PARCON (DNRM,T1,T2,T3,T4,GAMOLD,RHOOLD,3) >*/
L10:
    parcon_(&dnrm, &t1, &t2, &t3, &t4, &gamold, &rhoold, &c__3);
/*<    20 CALL SUM3 (N,T1,D,T2,U,T3,U1) >*/
L20:
    sum3_(&n, &t1, &d__[1], &t2, &u[1], &t3, &u1[1]);

/* ... TEST FOR STOPPING */

/*<    30 BDELNM = DDOT(N,D,1,D,1) >*/
L30:
    itcom3_1.bdelnm = ddot_(&n, &d__[1], &c__1, &d__[1], &c__1);
/*<       DNRM = BDELNM >*/
    dnrm = itcom3_1.bdelnm;
/*<       CON = SPECR >*/
    con = itcom3_1.specr;
/*<       CALL PSTOP (N,U,DNRM,CON,1,Q1) >*/
    pstop_(&n, &u[1], &dnrm, &con, &c__1, &q1);
/*<       IF (HALT) GO TO 100 >*/
    if (itcom2_1.halt) {
	goto L100;
    }

/* ... IF NON- OR PARTIALLY-ADAPTIVE, COMPUTE C(IN+1) AND EXIT. */

/*<       IF (ADAPT) GO TO 40 >*/
    if (itcom2_1.adapt) {
	goto L40;
    }
/*<       CALL SUM3 (N,-T1,DL,T2,C,T3,C1) >*/
    d__1 = -t1;
    sum3_(&n, &d__1, &dl[1], &t2, &c__[1], &t3, &c1[1]);
/*<       GO TO 100 >*/
    goto L100;

/* ... FULLY ADAPTIVE PROCEDURE */

/*<    40 CONTINUE >*/
L40:
/*<       IF (OMGSTR(1)) GO TO 90 >*/
    if (omgstr_(&c__1)) {
	goto L90;
    }
/*<       IF (OMGCHG(1)) GO TO 50 >*/
    if (omgchg_(&c__1)) {
	goto L50;
    }

/* ... PARAMETERS HAVE BEEN UNCHANGED.  COMPUTE C(IN+1) AND EXIT. */

/*<       CALL SUM3 (N,-T1,DL,T2,C,T3,C1) >*/
    d__1 = -t1;
    sum3_(&n, &d__1, &dl[1], &t2, &c__[1], &t3, &c1[1]);
/*<       GO TO 100 >*/
    goto L100;

/* ... IT HAS BEEN DECIDED TO CHANGE PARAMETERS */
/*        (1) COMPUTE NEW BETAB IF BETADT = .TRUE. */

/*<    50 CONTINUE >*/
L50:
/*<       IF (.NOT.BETADT) GO TO 60 >*/
    if (! itcom2_1.betadt) {
	goto L60;
    }
/*<       BETNEW = PBETA(N,IA,JA,A,D,WK,C1)/BDELNM >*/
    betnew = pbeta_(&n, &ia[1], &ja[1], &a[1], &d__[1], &wk[1], &c1[1]) / 
	    itcom3_1.bdelnm;
/*<       BETAB = DMAX1(BETAB,.25D0,BETNEW) >*/
/* Computing MAX */
    d__1 = max(itcom3_1.betab,.25);
    itcom3_1.betab = max(d__1,betnew);

/* ...    (2) COMPUTE NEW CME, OMEGA, AND SPECR */

/*<    60 CONTINUE >*/
L60:
/*<       IF (CASEII) GO TO 70 >*/
    if (itcom2_1.caseii) {
	goto L70;
    }
/*<       DNRM = PVTBV(N,IA,JA,A,D) >*/
    dnrm = pvtbv_(&n, &ia[1], &ja[1], &a[1], &d__[1]);
/*<       GO TO 80 >*/
    goto L80;
/*<    70 CALL VFILL (N,WK,0.D0) >*/
L70:
    vfill_(&n, &wk[1], &c_b21);
/*<       CALL PJAC (N,IA,JA,A,D,WK) >*/
    pjac_(&n, &ia[1], &ja[1], &a[1], &d__[1], &wk[1]);
/*<       DNRM = DDOT(N,WK,1,WK,1) >*/
    dnrm = ddot_(&n, &wk[1], &c__1, &wk[1], &c__1);
/*<    80 CALL OMEG (DNRM,3) >*/
L80:
    omeg_(&dnrm, &c__3);

/* ...    (3) COMPUTE NEW FORWARD RESIDUAL SINCE OMEGA HAS BEEN CHANGED. */

/*<    90 CONTINUE >*/
L90:
/*<       CALL DCOPY (N,RHS,1,WK,1) >*/
    dcopy_(&n, &rhs[1], &c__1, &wk[1], &c__1);
/*<       CALL DCOPY (N,U1,1,C1,1) >*/
    dcopy_(&n, &u1[1], &c__1, &c1[1], &c__1);
/*<       CALL PFSOR (N,IA,JA,A,C1,WK) >*/
    pfsor_(&n, &ia[1], &ja[1], &a[1], &c1[1], &wk[1]);
/*<       CALL VEVMW (N,C1,U1) >*/
    vevmw_(&n, &c1[1], &u1[1]);

/* ... OUTPUT INTERMEDIATE RESULTS. */

/*<   100 CALL ITERM (N,A,U,WK,4) >*/
L100:
    iterm_(&n, &a[1], &u[1], &wk[1], &c__4);

/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* itsrcg_ */

/*<       SUBROUTINE ITSRSI (NN,IA,JA,A,RHS,U,U1,C,D,CTWD,WK) >*/
/* Subroutine */ int itsrsi_(integer *nn, integer *ia, integer *ja, 
	doublereal *a, doublereal *rhs, doublereal *u, doublereal *u1, 
	doublereal *c__, doublereal *d__, doublereal *ctwd, doublereal *wk)
{
    /* System generated locals */
    doublereal d__1;

    /* Local variables */
    integer n;
    doublereal c1, c2, c3;
    logical q1;
    doublereal con;
    extern /* Subroutine */ int sum3_(integer *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *), pjac_(
	    integer *, integer *, integer *, doublereal *, doublereal *, 
	    doublereal *), omeg_(doublereal *, integer *);
    extern doublereal ddot_(integer *, doublereal *, integer *, doublereal *, 
	    integer *);
    doublereal dnrm;
    extern doublereal pbeta_(integer *, integer *, integer *, doublereal *, 
	    doublereal *, doublereal *, doublereal *);
    extern /* Subroutine */ int chgsi_(doublereal *, integer *), vfill_(
	    integer *, doublereal *, doublereal *), dcopy_(integer *, 
	    doublereal *, integer *, doublereal *, integer *), parsi_(
	    doublereal *, doublereal *, doublereal *, integer *), iterm_(
	    integer *, doublereal *, doublereal *, doublereal *, integer *), 
	    pfsor_(integer *, integer *, integer *, doublereal *, doublereal *
	    , doublereal *);
    extern doublereal pvtbv_(integer *, integer *, integer *, doublereal *, 
	    doublereal *);
    extern /* Subroutine */ int vevmw_(integer *, doublereal *, doublereal *),
	     pstop_(integer *, doublereal *, doublereal *, doublereal *, 
	    integer *, logical *), vevpw_(integer *, doublereal *, doublereal 
	    *), pssor1_(integer *, integer *, integer *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *);
    doublereal betnew;
    extern logical tstchg_(integer *), omgstr_(integer *);


/* ... FUNCTION: */

/*          THIS SUBROUTINE, ITSRSI, PERFORMS ONE ITERATION OF THE */
/*          SYMMETRIC SOR SEMI-ITERATION ALGORITHM.  IT IS CALLED BY */
/*          SSORSI. */

/* ... PARAMETER LIST: */

/*          N      INPUT INTEGER.  DIMENSION OF THE MATRIX. (= NN) */
/*          IA,JA  INPUT INTEGER VECTORS.  THE TWO INTEGER ARRAYS OF */
/*                 THE SPARSE MATRIX REPRESENTATION. */
/*          A      INPUT D.P. VECTOR.  THE D.P. ARRAY OF THE SPARSE */
/*                 MATRIX REPRESENTATION. */
/*          RHS    INPUT D.P. VECTOR.  CONTAINS THE RIGHT HAND SIDE */
/*                 OF THE MATRIX PROBLEM. */
/*          U      INPUT D.P. VECTOR.  CONTAINS THE ESTIMATE OF THE */
/*                 SOLUTION VECTOR AFTER IN ITERATIONS. */
/*          U1     INPUT/OUTPUT D.P. VECTOR.  ON INPUT, U1 CONTAINS THE */
/*                 THE ESTIMATE FOR THE SOLUTION AFTER IN-1 ITERATIONS. */
/*                 ON OUTPUT, U1 CONTAINS THE UPDATED ESTIMATE. */
/*          C      D.P. VECTOR.  IS USED TO COMPUTE THE FORWARD PSEUDO- */
/*                 RESIDUAL VECTOR FOR THE CURRENT ITERATION. */
/*          D      D.P. VECTOR.  IS USED TO COMPUTE THE BACKWARD PSEUDO- */
/*                 RESIDUAL VECTOR FOR THE CURRENT ITERATION. */
/*          CTWD   D.P. VECTOR.  IS USED IN THE COMPUTATIONS OF THE */
/*                 ACCELERATION PARAMETERS. */
/*          WK     D.P. VECTOR.  WORKING SPACE OF LENGTH N. */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER IA(1),JA(1),NN >*/
/*<    >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER N >*/
/*<       DOUBLE PRECISION BETNEW,CON,C1,C2,C3,DNRM >*/
/*<       LOGICAL Q1 >*/

/* ... SPECIFICATIONS FOR FUNCTION SUBPROGRAMS */

/*<       DOUBLE PRECISION DDOT,PBETA,PVTBV >*/
/*<       LOGICAL OMGSTR,TSTCHG >*/

/* *** BEGIN: ITPACK COMMON */

/*<       INTEGER IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/
/*<       COMMON /ITCOM1/ IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/

/*<       LOGICAL ADAPT,BETADT,CASEII,HALT,PARTAD >*/
/*<       COMMON /ITCOM2/ ADAPT,BETADT,CASEII,HALT,PARTAD >*/

/*<    >*/
/*<    >*/

/* *** END  : ITPACK COMMON */

/*     DESCRIPTION OF VARIABLES IN COMMON BLOCKS IN SUBROUTINE SSORSI */

/* ... COMPUTE PSEUDO-RESIDUALS (FORWARD AND BACKWARD) */

/*<       N = NN >*/
    /* Parameter adjustments */
    --wk;
    --ctwd;
    --d__;
    --c__;
    --u1;
    --u;
    --rhs;
    --ia;
    --ja;
    --a;

    /* Function Body */
    n = *nn;
/*<       CALL DCOPY (N,RHS,1,WK,1) >*/
    dcopy_(&n, &rhs[1], &c__1, &wk[1], &c__1);
/*<       CALL DCOPY (N,U,1,CTWD,1) >*/
    dcopy_(&n, &u[1], &c__1, &ctwd[1], &c__1);
/*<       CALL PSSOR1 (N,IA,JA,A,CTWD,WK,C,D) >*/
    pssor1_(&n, &ia[1], &ja[1], &a[1], &ctwd[1], &wk[1], &c__[1], &d__[1]);

/* ... COMPUTE U(IN+1) -- CONTAINED IN THE VECTOR U1. */

/*<       CALL PARSI (C1,C2,C3,3) >*/
    parsi_(&c1, &c2, &c3, &c__3);
/*<       CALL SUM3 (N,C1,D,C2,U,C3,U1) >*/
    sum3_(&n, &c1, &d__[1], &c2, &u[1], &c3, &u1[1]);

/* ... TEST FOR STOPPING */

/*<       BDELNM = DDOT(N,D,1,D,1) >*/
    itcom3_1.bdelnm = ddot_(&n, &d__[1], &c__1, &d__[1], &c__1);
/*<       DNRM = BDELNM >*/
    dnrm = itcom3_1.bdelnm;
/*<       CON = SPECR >*/
    con = itcom3_1.specr;
/*<       CALL PSTOP (N,U,DNRM,CON,1,Q1) >*/
    pstop_(&n, &u[1], &dnrm, &con, &c__1, &q1);
/*<       IF (HALT.OR..NOT.(ADAPT.OR.PARTAD)) GO TO 40 >*/
    if (itcom2_1.halt || ! (itcom2_1.adapt || itcom2_1.partad)) {
	goto L40;
    }

/* ... ADAPTIVE PROCEDURE */

/*<       IF (OMGSTR(1)) GO TO 40 >*/
    if (omgstr_(&c__1)) {
	goto L40;
    }
/*<       DELNNM = DDOT(N,C,1,C,1) >*/
    itcom3_1.delnnm = ddot_(&n, &c__[1], &c__1, &c__[1], &c__1);
/*<       IF (IN.EQ.IS) DELSNM = DELNNM >*/
    if (itcom1_1.in == itcom1_1.is) {
	itcom3_1.delsnm = itcom3_1.delnnm;
    }
/*<       IF (IN.EQ.0.OR..NOT.TSTCHG(1)) GO TO 40 >*/
    if (itcom1_1.in == 0 || ! tstchg_(&c__1)) {
	goto L40;
    }

/* ... IT HAS BEEN DECIDED TO CHANGE PARAMETERS. */
/* ...    (1) COMPUTE CTWD */

/*<       CALL DCOPY (N,D,1,CTWD,1) >*/
    dcopy_(&n, &d__[1], &c__1, &ctwd[1], &c__1);
/*<       CALL VFILL (N,WK,0.D0) >*/
    vfill_(&n, &wk[1], &c_b21);
/*<       CALL PFSOR (N,IA,JA,A,CTWD,WK) >*/
    pfsor_(&n, &ia[1], &ja[1], &a[1], &ctwd[1], &wk[1]);
/*<       CALL VEVPW (N,CTWD,C) >*/
    vevpw_(&n, &ctwd[1], &c__[1]);
/*<       CALL VEVMW (N,CTWD,D) >*/
    vevmw_(&n, &ctwd[1], &d__[1]);

/* ...    (2) COMPUTE NEW SPECTRAL RADIUS FOR CURRENT OMEGA. */

/*<       DNRM = DDOT(N,C,1,CTWD,1) >*/
    dnrm = ddot_(&n, &c__[1], &c__1, &ctwd[1], &c__1);
/*<       CALL CHGSI (DNRM,3) >*/
    chgsi_(&dnrm, &c__3);
/*<       IF (.NOT.ADAPT) GO TO 40 >*/
    if (! itcom2_1.adapt) {
	goto L40;
    }

/* ...    (3) COMPUTE NEW BETAB IF BETADT = .TRUE. */

/*<       IF (.NOT.BETADT) GO TO 10 >*/
    if (! itcom2_1.betadt) {
	goto L10;
    }
/*<       BETNEW = PBETA(N,IA,JA,A,D,WK,CTWD)/BDELNM >*/
    betnew = pbeta_(&n, &ia[1], &ja[1], &a[1], &d__[1], &wk[1], &ctwd[1]) / 
	    itcom3_1.bdelnm;
/*<       BETAB = DMAX1(BETAB,.25D0,BETNEW) >*/
/* Computing MAX */
    d__1 = max(itcom3_1.betab,.25);
    itcom3_1.betab = max(d__1,betnew);

/* ...    (4) COMPUTE NEW CME, OMEGA, AND SPECR. */

/*<    10 CONTINUE >*/
L10:
/*<       IF (CASEII) GO TO 20 >*/
    if (itcom2_1.caseii) {
	goto L20;
    }
/*<       DNRM = PVTBV(N,IA,JA,A,D) >*/
    dnrm = pvtbv_(&n, &ia[1], &ja[1], &a[1], &d__[1]);
/*<       GO TO 30 >*/
    goto L30;
/*<    20 CALL VFILL (N,WK,0.D0) >*/
L20:
    vfill_(&n, &wk[1], &c_b21);
/*<       CALL PJAC (N,IA,JA,A,D,WK) >*/
    pjac_(&n, &ia[1], &ja[1], &a[1], &d__[1], &wk[1]);
/*<       DNRM = DDOT(N,WK,1,WK,1) >*/
    dnrm = ddot_(&n, &wk[1], &c__1, &wk[1], &c__1);
/*<    30 CALL OMEG (DNRM,3) >*/
L30:
    omeg_(&dnrm, &c__3);

/* ... OUTPUT INTERMEDIATE INFORMATION */

/*<    40 CALL ITERM (N,A,U,WK,5) >*/
L40:
    iterm_(&n, &a[1], &u[1], &wk[1], &c__5);

/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* itsrsi_ */

/*<       SUBROUTINE ITRSCG (N,NNB,IA,JA,A,UB,UB1,DB,DB1,WB,TRI) >*/
/* Subroutine */ int itrscg_(integer *n, integer *nnb, integer *ia, integer *
	ja, doublereal *a, doublereal *ub, doublereal *ub1, doublereal *db, 
	doublereal *db1, doublereal *wb, doublereal *tri)
{
    doublereal c1, c2, c3, c4;
    logical q1;
    integer nb, nr;
    doublereal con;
    integer nrp1;
    extern /* Subroutine */ int sum3_(integer *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *);
    extern doublereal ddot_(integer *, doublereal *, integer *, doublereal *, 
	    integer *);
    doublereal dnrm;
    extern /* Subroutine */ int vfill_(integer *, doublereal *, doublereal *),
	     iterm_(integer *, doublereal *, doublereal *, doublereal *, 
	    integer *), pstop_(integer *, doublereal *, doublereal *, 
	    doublereal *, integer *, logical *), chgcon_(doublereal *, 
	    doublereal *, doublereal *, integer *);
    doublereal gamold;
    extern /* Subroutine */ int parcon_(doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, integer *);
    doublereal rhoold;
    extern /* Subroutine */ int prsblk_(integer *, integer *, integer *, 
	    integer *, doublereal *, doublereal *, doublereal *), prsred_(
	    integer *, integer *, integer *, integer *, doublereal *, 
	    doublereal *, doublereal *);
    doublereal rhotmp;


/* ... FUNCTION: */

/*          THIS SUBROUTINE, ITRSCG, PERFORMS ONE ITERATION OF THE */
/*          REDUCED SYSTEM CONJUGATE GRADIENT ALGORITHM.  IT IS */
/*          CALLED BY RSCG. */

/* ... PARAMETER LIST: */

/*          N      INPUT INTEGER.  DIMENSION OF THE MATRIX. */
/*          NB     INPUT INTEGER.  CONTAINS THE NUMBER OF BLACK POINTS */
/*                 IN THE RED-BLACK MATRIX. (= NNB) */
/*          IA,JA  INPUT INTEGER VECTORS.  THE TWO INTEGER ARRAYS OF */
/*                 THE SPARSE MATRIX REPRESENTATION. */
/*          A      INPUT D.P. VECTOR.  THE D.P. ARRAY OF THE SPARSE */
/*                 MATRIX REPRESENTATION. */
/*          UB     INPUT D.P. VECTOR.  CONTAINS THE ESTIMATE FOR THE */
/*                 SOLUTION ON THE BLACK POINTS AFTER IN ITERATIONS. */
/*          UB1    INPUT/OUTPUT D.P. VECTOR.  ON INPUT, UB1 CONTAINS THE */
/*                 SOLUTION VECTOR AFTER IN-1 ITERATIONS.  ON OUTPUT, */
/*                 IT WILL CONTAIN THE NEWEST ESTIMATE FOR THE SOLUTION */
/*                 VECTOR.  THIS IS ONLY FOR THE BLACK POINTS. */
/*          DB     INPUT D.P. ARRAY.  DB CONTAINS THE VALUE OF THE */
/*                 CURRENT PSEUDO-RESIDUAL ON THE BLACK POINTS. */
/*          DB1    INPUT/OUTPUT D.P. ARRAY.  DB1 CONTAINS THE PSEUDO- */
/*                 RESIDUAL ON THE BLACK POINTS FOR THE IN-1 ITERATION */
/*                 ON INPUT.  ON OUTPUT, IT IS FOR THE IN+1 ITERATION. */
/*          WB     D.P. ARRAY.  WB IS USED FOR COMPUTATIONS INVOLVING */
/*                 BLACK VECTORS. */
/*          TRI    D.P. ARRAY.  STORES THE TRIDIAGONAL MATRIX ASSOCIATED */
/*                 WITH CONJUGATE GRADIENT ACCELERATION. */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER IA(1),JA(1),N,NNB >*/
/*<       DOUBLE PRECISION A(1),UB(N),UB1(N),DB(NNB),DB1(N),WB(NNB),TRI(2,1) >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER NB,NR,NRP1 >*/
/*<       DOUBLE PRECISION CON,C1,C2,C3,C4,DNRM,GAMOLD,RHOOLD,RHOTMP >*/
/*<       LOGICAL Q1 >*/

/* ... SPECIFICATIONS FOR FUNCTION SUBPROGRAMS */

/*<       DOUBLE PRECISION DDOT >*/

/* *** BEGIN: ITPACK COMMON */

/*<       INTEGER IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/
/*<       COMMON /ITCOM1/ IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/

/*<       LOGICAL ADAPT,BETADT,CASEII,HALT,PARTAD >*/
/*<       COMMON /ITCOM2/ ADAPT,BETADT,CASEII,HALT,PARTAD >*/

/*<    >*/
/*<    >*/

/* *** END  : ITPACK COMMON */

/*     DESCRIPTION OF VARIABLES IN COMMON BLOCKS IN SUBROUTINE RSCG */

/* ... COMPUTE NEW ESTIMATE FOR CME IF ADAPT = .TRUE. */

/*<       NB = NNB >*/
    /* Parameter adjustments */
    --db1;
    --ub1;
    --ub;
    --wb;
    --db;
    --ia;
    --ja;
    --a;
    tri -= 3;

    /* Function Body */
    nb = *nnb;
/*<       NR = N-NB >*/
    nr = *n - nb;
/*<       NRP1 = NR+1 >*/
    nrp1 = nr + 1;
/*<       IF (ADAPT) CALL CHGCON (TRI,GAMOLD,RHOOLD,2) >*/
    if (itcom2_1.adapt) {
	chgcon_(&tri[3], &gamold, &rhoold, &c__2);
    }

/* ... TEST FOR STOPPING */

/*<       DELNNM = DDOT(NB,DB,1,DB,1) >*/
    itcom3_1.delnnm = ddot_(&nb, &db[1], &c__1, &db[1], &c__1);
/*<       DNRM = DELNNM >*/
    dnrm = itcom3_1.delnnm;
/*<       CON = CME >*/
    con = itcom3_1.cme;
/*<       CALL PSTOP (NB,UB(NRP1),DNRM,CON,2,Q1) >*/
    pstop_(&nb, &ub[nrp1], &dnrm, &con, &c__2, &q1);
/*<       IF (HALT) GO TO 30 >*/
    if (itcom2_1.halt) {
	goto L30;
    }

/* ... COMPUTE ACCELERATION PARAMETERS */

/*<       CALL VFILL (NR,UB1,0.D0) >*/
    vfill_(&nr, &ub1[1], &c_b21);
/*<       CALL PRSRED (NB,NR,IA,JA,A,DB,UB1) >*/
    prsred_(&nb, &nr, &ia[1], &ja[1], &a[1], &db[1], &ub1[1]);
/*<       CALL VFILL (NB,WB,0.D0) >*/
    vfill_(&nb, &wb[1], &c_b21);
/*<       CALL PRSBLK (NB,NR,IA,JA,A,UB1,WB) >*/
    prsblk_(&nb, &nr, &ia[1], &ja[1], &a[1], &ub1[1], &wb[1]);
/*<       DNRM = DDOT(NB,DB,1,WB,1) >*/
    dnrm = ddot_(&nb, &db[1], &c__1, &wb[1], &c__1);
/*<       IF (ISYM.EQ.0) GO TO 10 >*/
    if (itcom1_1.isym == 0) {
	goto L10;
    }
/*<       RHOTMP = DDOT(NB,WB,1,DB1,1) >*/
    rhotmp = ddot_(&nb, &wb[1], &c__1, &db1[1], &c__1);
/*<       CALL PARCON (DNRM,C1,C2,C3,C4,GAMOLD,RHOTMP,2) >*/
    parcon_(&dnrm, &c1, &c2, &c3, &c4, &gamold, &rhotmp, &c__2);
/*<       RHOOLD = RHOTMP >*/
    rhoold = rhotmp;
/*<       GO TO 20 >*/
    goto L20;
/*<    10 CALL PARCON (DNRM,C1,C2,C3,C4,GAMOLD,RHOOLD,2) >*/
L10:
    parcon_(&dnrm, &c1, &c2, &c3, &c4, &gamold, &rhoold, &c__2);

/* ... COMPUTE UB(IN+1) AND DB(IN+1) */

/*<    20 CALL SUM3 (NB,C1,DB,C2,UB(NRP1),C3,UB1(NRP1)) >*/
L20:
    sum3_(&nb, &c1, &db[1], &c2, &ub[nrp1], &c3, &ub1[nrp1]);
/*<       CALL SUM3 (NB,C1,WB,C4,DB,C3,DB1) >*/
    sum3_(&nb, &c1, &wb[1], &c4, &db[1], &c3, &db1[1]);

/* ... OUTPUT INTERMEDIATE INFORMATION */

/*<    30 CALL ITERM (NB,A(NRP1),UB(NRP1),WB,6) >*/
L30:
    iterm_(&nb, &a[nrp1], &ub[nrp1], &wb[1], &c__6);

/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* itrscg_ */

/*<       SUBROUTINE ITRSSI (N,NNB,IA,JA,A,RHS,UB,UB1,DB) >*/
/* Subroutine */ int itrssi_(integer *n, integer *nnb, integer *ia, integer *
	ja, doublereal *a, doublereal *rhs, doublereal *ub, doublereal *ub1, 
	doublereal *db)
{
    doublereal c1, c2, c3;
    logical q1;
    integer nb, nr, nrp1;
    extern /* Subroutine */ int sum3_(integer *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *);
    extern doublereal ddot_(integer *, doublereal *, integer *, doublereal *, 
	    integer *);
    doublereal dnrm;
    extern /* Subroutine */ int chgsi_(doublereal *, integer *), vfill_(
	    integer *, doublereal *, doublereal *), dcopy_(integer *, 
	    doublereal *, integer *, doublereal *, integer *), parsi_(
	    doublereal *, doublereal *, doublereal *, integer *), iterm_(
	    integer *, doublereal *, doublereal *, doublereal *, integer *), 
	    daxpy_(integer *, doublereal *, doublereal *, integer *, 
	    doublereal *, integer *);
    doublereal const__;
    extern /* Subroutine */ int vevmw_(integer *, doublereal *, doublereal *),
	     pstop_(integer *, doublereal *, doublereal *, doublereal *, 
	    integer *, logical *);
    extern logical tstchg_(integer *);
    extern /* Subroutine */ int prsblk_(integer *, integer *, integer *, 
	    integer *, doublereal *, doublereal *, doublereal *), prsred_(
	    integer *, integer *, integer *, integer *, doublereal *, 
	    doublereal *, doublereal *);


/* ... FUNCTION: */

/*          THIS SUBROUTINE, ITRSSI, PERFORMS ONE ITERATION OF THE */
/*          REDUCED SYSTEM SEMI-ITERATION ALGORITHM.  IT IS */
/*          CALLED BY RSSI. */

/* ... PARAMETER LIST: */

/*          N      INPUT INTEGER.  DIMENSION OF THE MATRIX. */
/*          NB     INPUT INTEGER.  CONTAINS THE NUMBER OF BLACK POINTS */
/*                 IN THE RED-BLACK MATRIX. (= NNB) */
/*          IA,JA  INPUT INTEGER VECTORS.  THE TWO INTEGER ARRAYS OF */
/*                 THE SPARSE MATRIX REPRESENTATION. */
/*          A      INPUT D.P. VECTOR.  THE D.P. ARRAY OF THE SPARSE */
/*                 MATRIX REPRESENTATION. */
/*          RHS    INPUT D.P. VECTOR.  CONTAINS THE RIGHT HAND SIDE */
/*                 OF THE MATRIX PROBLEM. */
/*          UB     INPUT D.P. VECTOR.  CONTAINS THE ESTIMATE FOR THE */
/*                 SOLUTION ON THE BLACK POINTS AFTER IN ITERATIONS. */
/*          UB1    INPUT/OUTPUT D.P. VECTOR.  ON INPUT, UB1 CONTAINS THE */
/*                 SOLUTION VECTOR AFTER IN-1 ITERATIONS.  ON OUTPUT, */
/*                 IT WILL CONTAIN THE NEWEST ESTIMATE FOR THE SOLUTION */
/*                 VECTOR.  THIS IS ONLY FOR THE BLACK POINTS. */
/*          DB     INPUT D.P. ARRAY.  DB CONTAINS THE VALUE OF THE */
/*                 CURRENT PSEUDO-RESIDUAL ON THE BLACK POINTS. */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER IA(1),JA(1),N,NNB >*/
/*<       DOUBLE PRECISION A(1),RHS(N),UB(N),UB1(N),DB(NNB) >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER NB,NR,NRP1 >*/
/*<       DOUBLE PRECISION CONST,C1,C2,C3,DNRM >*/
/*<       LOGICAL Q1 >*/

/* ... SPECIFICATIONS FOR FUNCTION SUBPROGRAMS */

/*<       DOUBLE PRECISION DDOT >*/
/*<       LOGICAL TSTCHG >*/

/* *** BEGIN: ITPACK COMMON */

/*<       INTEGER IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/
/*<       COMMON /ITCOM1/ IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/

/*<       LOGICAL ADAPT,BETADT,CASEII,HALT,PARTAD >*/
/*<       COMMON /ITCOM2/ ADAPT,BETADT,CASEII,HALT,PARTAD >*/

/*<    >*/
/*<    >*/

/* *** END  : ITPACK COMMON */

/*     DESCRIPTION OF VARIABLES IN COMMON BLOCKS IN SUBROUTINE RSSI */

/* ... COMPUTE UR(IN) INTO UB */

/*<       NB = NNB >*/
    /* Parameter adjustments */
    --ub1;
    --ub;
    --rhs;
    --db;
    --ia;
    --ja;
    --a;

    /* Function Body */
    nb = *nnb;
/*<       NR = N-NB >*/
    nr = *n - nb;
/*<       NRP1 = NR+1 >*/
    nrp1 = nr + 1;
/*<       CALL DCOPY (NR,RHS,1,UB,1) >*/
    dcopy_(&nr, &rhs[1], &c__1, &ub[1], &c__1);
/*<       CALL PRSRED (NB,NR,IA,JA,A,UB(NRP1),UB) >*/
    prsred_(&nb, &nr, &ia[1], &ja[1], &a[1], &ub[nrp1], &ub[1]);

/* ... COMPUTE PSEUDO-RESIDUAL, DB(IN) */

/*<       CALL DCOPY (NB,RHS(NRP1),1,DB,1) >*/
    dcopy_(&nb, &rhs[nrp1], &c__1, &db[1], &c__1);
/*<       CALL PRSBLK (NB,NR,IA,JA,A,UB,DB) >*/
    prsblk_(&nb, &nr, &ia[1], &ja[1], &a[1], &ub[1], &db[1]);
/*<       CALL VEVMW (NB,DB,UB(NRP1)) >*/
    vevmw_(&nb, &db[1], &ub[nrp1]);

/* ... TEST FOR STOPPING */

/*<       DELNNM = DDOT(NB,DB,1,DB,1) >*/
    itcom3_1.delnnm = ddot_(&nb, &db[1], &c__1, &db[1], &c__1);
/*<       DNRM = DELNNM >*/
    dnrm = itcom3_1.delnnm;
/*<       CONST = CME >*/
    const__ = itcom3_1.cme;
/*<       CALL PSTOP (NB,UB(NRP1),DNRM,CONST,2,Q1) >*/
    pstop_(&nb, &ub[nrp1], &dnrm, &const__, &c__2, &q1);
/*<       IF (HALT) GO TO 20 >*/
    if (itcom2_1.halt) {
	goto L20;
    }
/*<       IF (.NOT.ADAPT) GO TO 10 >*/
    if (! itcom2_1.adapt) {
	goto L10;
    }

/* ... TEST TO CHANGE PARAMETERS */

/*<       IF (.NOT.TSTCHG(2)) GO TO 10 >*/
    if (! tstchg_(&c__2)) {
	goto L10;
    }

/* ... CHANGE PARAMETERS */

/*<       CALL VFILL (NR,UB1,0.D0) >*/
    vfill_(&nr, &ub1[1], &c_b21);
/*<       CALL PRSRED (NB,NR,IA,JA,A,DB,UB1) >*/
    prsred_(&nb, &nr, &ia[1], &ja[1], &a[1], &db[1], &ub1[1]);
/*<       DNRM = DDOT(NR,UB1,1,UB1,1) >*/
    dnrm = ddot_(&nr, &ub1[1], &c__1, &ub1[1], &c__1);
/*<       CALL CHGSI (DNRM,2) >*/
    chgsi_(&dnrm, &c__2);
/*<       IF (.NOT.ADAPT) GO TO 10 >*/
    if (! itcom2_1.adapt) {
	goto L10;
    }

/* ... COMPUTE UB(N+1) AFTER CHANGING PARAMETERS */

/*<       CALL DCOPY (NB,UB(NRP1),1,UB1(NRP1),1) >*/
    dcopy_(&nb, &ub[nrp1], &c__1, &ub1[nrp1], &c__1);
/*<       CALL DAXPY (NB,GAMMA,DB,1,UB1(NRP1),1) >*/
    daxpy_(&nb, &itcom3_1.gamma, &db[1], &c__1, &ub1[nrp1], &c__1);
/*<       GO TO 20 >*/
    goto L20;

/* ... COMPUTE UB(N+1) WITHOUT CHANGE OF PARAMETERS */

/*<    10 CALL PARSI (C1,C2,C3,2) >*/
L10:
    parsi_(&c1, &c2, &c3, &c__2);
/*<       CALL SUM3 (NB,C1,DB,C2,UB(NRP1),C3,UB1(NRP1)) >*/
    sum3_(&nb, &c1, &db[1], &c2, &ub[nrp1], &c3, &ub1[nrp1]);

/* ... OUTPUT INTERMEDIATE INFORMATION */

/*<    20 CALL ITERM (NB,A(NRP1),UB(NRP1),DB,7) >*/
L20:
    iterm_(&nb, &a[nrp1], &ub[nrp1], &db[1], &c__7);

/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* itrssi_ */

/*<       INTEGER FUNCTION BISRCH (N,K,L) >*/
integer bisrch_(integer *n, integer *k, integer *l)
{
    /* System generated locals */
    integer ret_val;

    /* Local variables */
    integer jmid, jleft, jright;


/* ... BISRCH IS AN INTEGER FUNCTION WHICH USES A BISECTION SEARCH */
/*     TO FIND THE ENTRY J IN THE ARRAY K SUCH THAT THE VALUE L IS */
/*     GREATER THAN OR EQUAL TO K(J) AND STRICTLY LESS THAN K(J+1). */

/* ... PARAMETER LIST: */

/*          N      INTEGER LENGTH OF VECTOR K */
/*          K      INTEGER VECTOR */
/*          L      INTEGER CONSTANT SUCH THAT  K(J) .GE. L .LT. K(J+1) */
/*                 WITH J RETURNED AS VALUE OF INTEGER FUNCTION BISRCH */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER N,L,K(N) >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER JLEFT,JMID,JRIGHT >*/

/*<       JLEFT = 1 >*/
    /* Parameter adjustments */
    --k;

    /* Function Body */
    jleft = 1;
/*<       JRIGHT = N >*/
    jright = *n;
/*<       IF (N.EQ.2) GO TO 40 >*/
    if (*n == 2) {
	goto L40;
    }
/*<       JMID = (N+1)/2 >*/
    jmid = (*n + 1) / 2;

/*<    10 IF (L.GE.K(JMID)) GO TO 20 >*/
L10:
    if (*l >= k[jmid]) {
	goto L20;
    }

/* ...... L .GE. K(LEFT)  AND  L .LT. K(JMID) */

/*<       JRIGHT = JMID >*/
    jright = jmid;
/*<       GO TO 30 >*/
    goto L30;

/* ...... L .GE. K(JMID)  AND  L .LT. K(JRIGHT) */

/*<    20 JLEFT = JMID >*/
L20:
    jleft = jmid;

/* ...... TEST FOR CONVERGENCE */

/*<    30 IF (JRIGHT-JLEFT.EQ.1) GO TO 40 >*/
L30:
    if (jright - jleft == 1) {
	goto L40;
    }
/*<       JMID = JLEFT+(JRIGHT-JLEFT+1)/2 >*/
    jmid = jleft + (jright - jleft + 1) / 2;
/*<       GO TO 10 >*/
    goto L10;

/* ...... BISECTION SEARCH FINISHED */

/*<    40 BISRCH = JLEFT >*/
L40:
    ret_val = jleft;

/*<       RETURN >*/
    return ret_val;
/*<       END >*/
} /* bisrch_ */

/*<       DOUBLE PRECISION FUNCTION CHEBY (QA,QT,RRR,IP,CME,SME) >*/
doublereal cheby_(doublereal *qa, doublereal *qt, doublereal *rrr, integer *
	ip, doublereal *cme, doublereal *sme)
{
    /* System generated locals */
    doublereal ret_val, d__1, d__2, d__3;

    /* Builtin functions */
    double sqrt(doublereal), pow_di(doublereal *, integer *), pow_dd(
	    doublereal *, doublereal *);

    /* Local variables */
    doublereal x, y, z__;


/*     COMPUTES THE SOLUTION TO THE CHEBYSHEV EQUATION */

/* ... PARAMETER LIST: */

/*          QA     RATIO OF PSEUDO-RESIDUALS */
/*          QT     VIRTUAL SPECTRAL RADIUS */
/*          RRR    ADAPTIVE PARAMETER */
/*          IP     NUMBER OF ITERATIONS SINCE LAST CHANGE OF */
/*                     PARAMETERS */
/*          CME,   ESTIMATES FOR THE LARGEST AND SMALLEST EIGEN- */
/*          SME      VALUES OF THE ITERATION MATRIX */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER IP >*/
/*<       DOUBLE PRECISION CME,QA,QT,RRR,SME >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       DOUBLE PRECISION X,Y,Z >*/

/*<       Z = .5D0*(QA+DSQRT(DABS(QA**2-QT**2)))*(1.D0+RRR**IP) >*/
/* Computing 2nd power */
    d__2 = *qa;
/* Computing 2nd power */
    d__3 = *qt;
    z__ = (*qa + sqrt((d__1 = d__2 * d__2 - d__3 * d__3, abs(d__1)))) * .5 * (
	    pow_di(rrr, ip) + 1.);
/*<       X = Z**(1.D0/DBLE(FLOAT(IP))) >*/
    d__1 = 1. / (doublereal) ((real) (*ip));
    x = pow_dd(&z__, &d__1);
/*<       Y = (X+RRR/X)/(1.D0+RRR) >*/
    y = (x + *rrr / x) / (*rrr + 1.);

/*<       CHEBY = .5D0*(CME+SME+Y*(2.D0-CME-SME)) >*/
    ret_val = (*cme + *sme + y * (2. - *cme - *sme)) * .5;

/*<       RETURN >*/
    return ret_val;
/*<       END >*/
} /* cheby_ */

/*<       SUBROUTINE CHGCON (TRI,GAMOLD,RHOOLD,IBMTH) >*/
/* Subroutine */ int chgcon_(doublereal *tri, doublereal *gamold, doublereal *
	rhoold, integer *ibmth)
{
    /* Format strings */
    static char fmt_70[] = "(/10x,\002DIFFICULTY IN COMPUTATION OF MAXIMUM E\
IGENVALUE\002/15x,\002OF ITERATION MATRIX\002/10x,\002SUBROUTINE ZBRENT RETU\
RNED IER =\002,i5)";
    static char fmt_90[] = "(/10x,\002DIFFICULTY IN COMPUTATION OF MAXIMUM E\
IGENVALUE\002/15x,\002OF ITERATION MATRIX\002/10x,\002SUBROUTINE EQRT1S RETU\
RNED IER =\002,i5)";
    static char fmt_140[] = "(/10x,\002ESTIMATE OF MAXIMUM EIGENVALUE OF JAC\
OBI   \002/15x,\002MATRIX (CME) NOT ACCURATE\002/10x,\002ADAPTIVE PROCEDURE \
TURNED OFF AT ITERATION \002,i5/10x,\002FINAL ESTIMATE OF MAXIMUM EIGENVALUE\
 =\002,d15.7/)";

    /* System generated locals */
    doublereal d__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();
    double sqrt(doublereal);

    /* Local variables */
    integer ip, ib2, ib3;
    doublereal end;
    integer ier;
    doublereal cmold, start;
    extern doublereal eigvns_(integer *, doublereal *, doublereal *, 
	    doublereal *, integer *), eigvss_(integer *, doublereal *, 
	    doublereal *, doublereal *, integer *, integer *);

    /* Fortran I/O blocks */
    static cilist io___357 = { 0, 0, 0, fmt_70, 0 };
    static cilist io___360 = { 0, 0, 0, fmt_90, 0 };
    static cilist io___361 = { 0, 0, 0, fmt_140, 0 };



/*     COMPUTES THE NEW ESTIMATE FOR THE LARGEST EIGENVALUE FOR */
/*     CONJUGATE GRADIENT ACCELERATION. */

/* ... PARAMETER LIST: */

/*          TRI    TRIDIAGONAL MATRIX ASSOCIATED WITH THE EIGENVALUES */
/*                    OF THE CONJUGATE GRADIENT POLYNOMIAL */
/*          GAMOLD */
/*            AND */
/*          RHOOLD PREVIOUS VALUES OF ACCELERATION PARAMETERS */
/*          IBMTH  INDICATOR OF BASIC METHOD BEING ACCELERATED BY CG */
/*                      IBMTH = 1,  JACOBI */
/*                            = 2,  REDUCED SYSTEM */
/*                            = 3,  SSOR */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER IBMTH >*/
/*<       DOUBLE PRECISION TRI(2,1),GAMOLD,RHOOLD >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER IB2,IB3,IER,IP >*/
/*<       DOUBLE PRECISION CMOLD,END,START,EIGVSS,EIGVNS >*/

/* *** BEGIN: ITPACK COMMON */

/*<       INTEGER IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/
/*<       COMMON /ITCOM1/ IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/

/*<       LOGICAL ADAPT,BETADT,CASEII,HALT,PARTAD >*/
/*<       COMMON /ITCOM2/ ADAPT,BETADT,CASEII,HALT,PARTAD >*/

/*<    >*/
/*<    >*/

/* *** END  : ITPACK COMMON */

/*     DESCRIPTION OF VARIABLES IN COMMON BLOCKS IN MAIN SUBROUTINE */

/*<       GO TO (10,20,30), IBMTH >*/
    /* Parameter adjustments */
    tri -= 3;

    /* Function Body */
    switch (*ibmth) {
	case 1:  goto L10;
	case 2:  goto L20;
	case 3:  goto L30;
    }

/* ... JACOBI CONJUGATE GRADIENT */

/*<    10 START = CME >*/
L10:
    start = itcom3_1.cme;
/*<       IP = IN >*/
    ip = itcom1_1.in;
/*<       GO TO 40 >*/
    goto L40;

/* ... REDUCED SYSTEM CG */

/*<    20 START = CME**2 >*/
L20:
/* Computing 2nd power */
    d__1 = itcom3_1.cme;
    start = d__1 * d__1;
/*<       IP = IN >*/
    ip = itcom1_1.in;
/*<       GO TO 40 >*/
    goto L40;

/* ... SSOR CG */

/*<    30 IF (ADAPT) START = SPR >*/
L30:
    if (itcom2_1.adapt) {
	start = itcom3_1.spr;
    }
/*<       IF (.NOT.ADAPT) START = SPECR >*/
    if (! itcom2_1.adapt) {
	start = itcom3_1.specr;
    }
/*<       IP = IN-IS >*/
    ip = itcom1_1.in - itcom1_1.is;

/* ... DEFINE THE MATRIX */

/*<    40 IF (IP.GE.2) GO TO 60 >*/
L40:
    if (ip >= 2) {
	goto L60;
    }
/*<       IF (IP.EQ.1) GO TO 50 >*/
    if (ip == 1) {
	goto L50;
    }

/* ... IP = 0 */

/*<       END = 0.D0 >*/
    end = 0.;
/*<       CMOLD = 0.D0 >*/
    cmold = 0.;
/*<       GO TO 110 >*/
    goto L110;

/* ... IP = 1 */

/*<    50 END = 1.D0-1.D0/GAMMA >*/
L50:
    end = 1. - 1. / itcom3_1.gamma;
/*<       TRI(1,1) = END >*/
    tri[3] = end;
/*<       TRI(2,1) = 0.D0 >*/
    tri[4] = 0.;
/*<       GO TO 110 >*/
    goto L110;

/* ... IP > 1 */

/*<    60 IF ((IP.GT.2).AND.(DABS(START-CMOLD).LE.ZETA*START)) GO TO 120 >*/
L60:
    if (ip > 2 && (d__1 = start - cmold, abs(d__1)) <= itcom3_1.zeta * start) 
	    {
	goto L120;
    }
/*<       CMOLD = START >*/
    cmold = start;

/* ... COMPUTE THE LARGEST EIGENVALUE */

/*<       TRI(1,IP) = 1.D0-1.D0/GAMMA >*/
    tri[(ip << 1) + 1] = 1. - 1. / itcom3_1.gamma;
/*<       TRI(2,IP) = (RHO-1.D0)/(RHO*RHOOLD*GAMMA*GAMOLD) >*/
    tri[(ip << 1) + 2] = (itcom3_1.rho - 1.) / (itcom3_1.rho * *rhoold * 
	    itcom3_1.gamma * *gamold);
/*<       IF (ISYM.NE.0) GO TO 80 >*/
    if (itcom1_1.isym != 0) {
	goto L80;
    }
/*<       END = EIGVSS(IP,TRI,START,ZETA,ITMAX,IER) >*/
    end = eigvss_(&ip, &tri[3], &start, &itcom3_1.zeta, &itcom1_1.itmax, &ier)
	    ;
/*<       IF (IER.EQ.0) GO TO 100 >*/
    if (ier == 0) {
	goto L100;
    }
/*<       IF (LEVEL.GE.2) WRITE (NOUT,70) IER >*/
    if (itcom1_1.level >= 2) {
	io___357.ciunit = itcom1_1.nout;
	s_wsfe(&io___357);
	do_fio(&c__1, (char *)&ier, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 100 >*/
    goto L100;
/*<    80 IB2 = 1+IP >*/
L80:
    ib2 = ip + 1;
/*<       IB3 = IB2+IP/2+1 >*/
    ib3 = ib2 + ip / 2 + 1;
/*<       END = EIGVNS(IP,TRI,TRI(1,IB2),TRI(1,IB3),IER) >*/
    end = eigvns_(&ip, &tri[3], &tri[(ib2 << 1) + 1], &tri[(ib3 << 1) + 1], &
	    ier);
/*<       IF (IER.EQ.0) GO TO 100 >*/
    if (ier == 0) {
	goto L100;
    }
/*<       IF (LEVEL.GE.2) WRITE (NOUT,90) IER >*/
    if (itcom1_1.level >= 2) {
	io___360.ciunit = itcom1_1.nout;
	s_wsfe(&io___360);
	do_fio(&c__1, (char *)&ier, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<   100 CONTINUE >*/
L100:
/*<       IF (IER.NE.0) GO TO 130 >*/
    if (ier != 0) {
	goto L130;
    }

/* ... SET SPECTRAL RADIUS FOR THE VARIOUS METHODS */

/*<   110 IF (IBMTH.EQ.1) CME = END >*/
L110:
    if (*ibmth == 1) {
	itcom3_1.cme = end;
    }
/*<       IF (IBMTH.EQ.2) CME = DSQRT(DABS(END)) >*/
    if (*ibmth == 2) {
	itcom3_1.cme = sqrt((abs(end)));
    }
/*<       IF (IBMTH.EQ.3.AND.ADAPT) SPR = END >*/
    if (*ibmth == 3 && itcom2_1.adapt) {
	itcom3_1.spr = end;
    }
/*<       IF (IBMTH.EQ.3.AND..NOT.ADAPT) SPECR = END >*/
    if (*ibmth == 3 && ! itcom2_1.adapt) {
	itcom3_1.specr = end;
    }
/*<       RETURN >*/
    return 0;

/* ... RELATIVE CHANGE IN CME IS LESS THAN ZETA.  THEREFORE STOP */
/*     CHANGING. */

/*<   120 ADAPT = .FALSE. >*/
L120:
    itcom2_1.adapt = FALSE_;
/*<       PARTAD = .FALSE. >*/
    itcom2_1.partad = FALSE_;
/*<       RETURN >*/
    return 0;

/* ... ESTIMATE FOR CME > 1.D0.  THEREFORE NEED TO STOP ADAPTIVE */
/*     PROCEDURE AND KEEP OLD VALUE OF CME. */

/*<   130 ADAPT = .FALSE. >*/
L130:
    itcom2_1.adapt = FALSE_;
/*<       PARTAD = .FALSE. >*/
    itcom2_1.partad = FALSE_;
/*<       IF (LEVEL.GE.2) WRITE (NOUT,140) IN,START >*/
    if (itcom1_1.level >= 2) {
	io___361.ciunit = itcom1_1.nout;
	s_wsfe(&io___361);
	do_fio(&c__1, (char *)&itcom1_1.in, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&start, (ftnlen)sizeof(doublereal));
	e_wsfe();
    }
/*<    >*/

/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* chgcon_ */

/*<       SUBROUTINE CHGSI (DTNRM,IBMTH) >*/
/* Subroutine */ int chgsi_(doublereal *dtnrm, integer *ibmth)
{
    /* Format strings */
    static char fmt_90[] = "(/30x,\002PARAMETERS WERE CHANGED AT ITERATION N\
O.\002,i5/35x,\002SOLUTION TO CHEBYSHEV EQN.       =\002,d15.7/35x,\002SOLUT\
ION TO RAYLEIGH QUOTIENT    =\002,d15.7/35x,\002NEW ESTIMATE FOR CME        \
     =\002,d15.7/35x,\002NEW ESTIMATE FOR GAMMA           =\002,d15.7/35x\
,\002NEW ESTIMATE FOR SPECTRAL RADIUS =\002,d15.7/)";
    static char fmt_110[] = "(/10x,\002ESTIMATE OF MAXIMUM EIGENVALUE OF JAC\
OBI   \002/15x,\002MATRIX (CME) TOO LARGE\002/10x,\002ADAPTIVE PROCEDURE TUR\
NED OFF AT ITERATION \002,i5/10x,\002FINAL ESTIMATE OF MAXIMUM EIGENVALUE \
=\002,d15.7/)";
    static char fmt_100[] = "(/30x,\002PARAMETERS WERE CHANGED AT ITERATION \
NO.\002,i5/35x,\002SOLUTION TO CHEBYSHEV EQN.       =\002,d15.7/35x,\002SOLU\
TION TO RAYLEIGH QUOTIENT    =\002,d15.7/35x,\002NEW ESTIMATE FOR CME       \
      =\002,d15.7/35x,\002NEW ESTIMATE FOR SPECTRAL RADIUS =\002,d15.7/)";

    /* System generated locals */
    integer i__1;
    doublereal d__1, d__2;

    /* Builtin functions */
    double sqrt(doublereal);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    doublereal zm1, zm2;
    extern doublereal cheby_(doublereal *, doublereal *, doublereal *, 
	    integer *, doublereal *, doublereal *);
    doublereal cmold;

    /* Fortran I/O blocks */
    static cilist io___365 = { 0, 0, 0, fmt_90, 0 };
    static cilist io___366 = { 0, 0, 0, fmt_110, 0 };
    static cilist io___367 = { 0, 0, 0, fmt_90, 0 };
    static cilist io___368 = { 0, 0, 0, fmt_110, 0 };
    static cilist io___369 = { 0, 0, 0, fmt_100, 0 };



/* ... COMPUTES NEW CHEBYSHEV ACCELERATION PARAMETERS ADAPTIVELY. */

/* ... PARAMETER LIST: */

/*          DTNRM  NUMERATOR OF RAYLEIGH QUOTIENT */
/*          IBMTH  INDICATOR OF BASIC METHOD BEING ACCELERATED BY SI */
/*                      IBMTH = 1,   JACOBI */
/*                            = 2,   REDUCED SYSTEM */
/*                            = 3,   SYMMETRIC SOR */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER IBMTH >*/
/*<       DOUBLE PRECISION DTNRM >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       DOUBLE PRECISION CMOLD,ZM1,ZM2 >*/

/* ... SPECIFICATIONS FOR FUNCTION SUBPROGRAMS */

/*<       DOUBLE PRECISION CHEBY >*/

/* *** BEGIN: ITPACK COMMON */

/*<       INTEGER IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/
/*<       COMMON /ITCOM1/ IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/

/*<       LOGICAL ADAPT,BETADT,CASEII,HALT,PARTAD >*/
/*<       COMMON /ITCOM2/ ADAPT,BETADT,CASEII,HALT,PARTAD >*/

/*<    >*/
/*<    >*/

/* *** END  : ITPACK COMMON */

/*     DESCRIPTION OF VARIABLES IN COMMON BLOCKS IN MAIN SUBROUTINE */

/*<       GO TO (10,30,50), IBMTH >*/
    switch (*ibmth) {
	case 1:  goto L10;
	case 2:  goto L30;
	case 3:  goto L50;
    }

/*     --------------------- */
/* ... JACOBI SEMI-ITERATIVE */
/*     --------------------- */

/* ... CHEBYSHEV EQUATION */

/*<    10 CONTINUE >*/
L10:
/*<       IF (IN.EQ.0) ZM1 = CME >*/
    if (itcom1_1.in == 0) {
	zm1 = itcom3_1.cme;
    }
/*<       IF (IN.NE.0) ZM1 = CHEBY(QA,QT,RRR,IN-IS,CME,SME) >*/
    if (itcom1_1.in != 0) {
	i__1 = itcom1_1.in - itcom1_1.is;
	zm1 = cheby_(&itcom3_1.qa, &itcom3_1.qt, &itcom3_1.rrr, &i__1, &
		itcom3_1.cme, &itcom3_1.sme);
    }

/* ... RAYLEIGH QUOTIENT */

/*<       ZM2 = DTNRM/DELNNM >*/
    zm2 = *dtnrm / itcom3_1.delnnm;

/* ... COMPUTATION OF ITERATIVE PARAMETERS */

/*<       CMOLD = CME >*/
    cmold = itcom3_1.cme;
/*<       CME = DMAX1(ZM1,ZM2,CMOLD) >*/
/* Computing MAX */
    d__1 = max(zm1,zm2);
    itcom3_1.cme = max(d__1,cmold);
/*<       IF (CME.GE.1.D0) GO TO 20 >*/
    if (itcom3_1.cme >= 1.) {
	goto L20;
    }
/*<       IF (CASEII) SME = -CME >*/
    if (itcom2_1.caseii) {
	itcom3_1.sme = -itcom3_1.cme;
    }
/*<       SIGE = (CME-SME)/(2.D0-CME-SME) >*/
    itcom3_1.sige = (itcom3_1.cme - itcom3_1.sme) / (2. - itcom3_1.cme - 
	    itcom3_1.sme);
/*<       GAMMA = 2.D0/(2.D0-CME-SME) >*/
    itcom3_1.gamma = 2. / (2. - itcom3_1.cme - itcom3_1.sme);
/*<    >*/
    itcom3_1.rrr = (1. - sqrt((d__1 = 1. - itcom3_1.sige * itcom3_1.sige, abs(
	    d__1)))) / (sqrt((d__2 = 1. - itcom3_1.sige * itcom3_1.sige, abs(
	    d__2))) + 1.);
/*<       IS = IN >*/
    itcom1_1.is = itcom1_1.in;
/*<       DELSNM = DELNNM >*/
    itcom3_1.delsnm = itcom3_1.delnnm;
/*<       RHO = 1.D0 >*/
    itcom3_1.rho = 1.;
/*<       IF (LEVEL.GE.2) WRITE (NOUT,90) IN,ZM1,ZM2,CME,GAMMA,CME >*/
    if (itcom1_1.level >= 2) {
	io___365.ciunit = itcom1_1.nout;
	s_wsfe(&io___365);
	do_fio(&c__1, (char *)&itcom1_1.in, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&zm1, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&zm2, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&itcom3_1.cme, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&itcom3_1.gamma, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&itcom3_1.cme, (ftnlen)sizeof(doublereal));
	e_wsfe();
    }
/*<       RETURN >*/
    return 0;

/* ... ADAPTIVE PROCEDURE FAILED FOR JACOBI SI */

/*<    20 CME = CMOLD >*/
L20:
    itcom3_1.cme = cmold;
/*<       ADAPT = .FALSE. >*/
    itcom2_1.adapt = FALSE_;
/*<       IF (LEVEL.GE.2) WRITE (NOUT,110) IN,CME >*/
    if (itcom1_1.level >= 2) {
	io___366.ciunit = itcom1_1.nout;
	s_wsfe(&io___366);
	do_fio(&c__1, (char *)&itcom1_1.in, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&itcom3_1.cme, (ftnlen)sizeof(doublereal));
	e_wsfe();
    }
/*<       RETURN >*/
    return 0;

/*     ----------------------------- */
/* ... REDUCED SYSTEM SEMI-ITERATIVE */
/*     ----------------------------- */

/* ... CHEBYSHEV EQUATION */

/*<    30 CONTINUE >*/
L30:
/*<       IF (IN.EQ.0) ZM1 = CME >*/
    if (itcom1_1.in == 0) {
	zm1 = itcom3_1.cme;
    }
/*<       IF (IN.NE.0) ZM1 = CHEBY(QA,QT,RRR,2*(IN-IS),0.D0,0.D0) >*/
    if (itcom1_1.in != 0) {
	i__1 = itcom1_1.in - itcom1_1.is << 1;
	zm1 = cheby_(&itcom3_1.qa, &itcom3_1.qt, &itcom3_1.rrr, &i__1, &c_b21,
		 &c_b21);
    }

/* ... RAYLEIGH QUOTIENT */

/*<       ZM2 = DSQRT(DABS(DTNRM/DELNNM)) >*/
    zm2 = sqrt((d__1 = *dtnrm / itcom3_1.delnnm, abs(d__1)));

/* ... COMPUTATION OF NEW ITERATIVE PARAMETERS */

/*<       CMOLD = CME >*/
    cmold = itcom3_1.cme;
/*<       CME = DMAX1(ZM1,ZM2,CMOLD) >*/
/* Computing MAX */
    d__1 = max(zm1,zm2);
    itcom3_1.cme = max(d__1,cmold);
/*<       IF (CME.GE.1.D0) GO TO 40 >*/
    if (itcom3_1.cme >= 1.) {
	goto L40;
    }
/*<       SIGE = CME*CME/(2.D0-CME*CME) >*/
    itcom3_1.sige = itcom3_1.cme * itcom3_1.cme / (2. - itcom3_1.cme * 
	    itcom3_1.cme);
/*<       GAMMA = 2.D0/(2.D0-CME*CME) >*/
    itcom3_1.gamma = 2. / (2. - itcom3_1.cme * itcom3_1.cme);
/*<    >*/
    itcom3_1.rrr = (1. - sqrt((d__1 = 1. - itcom3_1.cme * itcom3_1.cme, abs(
	    d__1)))) / (sqrt((d__2 = 1. - itcom3_1.cme * itcom3_1.cme, abs(
	    d__2))) + 1.);
/*<       IS = IN >*/
    itcom1_1.is = itcom1_1.in;
/*<       DELSNM = DELNNM >*/
    itcom3_1.delsnm = itcom3_1.delnnm;
/*<       RHO = 1.D0 >*/
    itcom3_1.rho = 1.;
/*<       IF (LEVEL.GE.2) WRITE (NOUT,90) IN,ZM1,ZM2,CME,GAMMA,CME >*/
    if (itcom1_1.level >= 2) {
	io___367.ciunit = itcom1_1.nout;
	s_wsfe(&io___367);
	do_fio(&c__1, (char *)&itcom1_1.in, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&zm1, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&zm2, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&itcom3_1.cme, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&itcom3_1.gamma, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&itcom3_1.cme, (ftnlen)sizeof(doublereal));
	e_wsfe();
    }
/*<       RETURN >*/
    return 0;

/* ... ADAPTIVE PROCEDURE FAILED FOR REDUCED SYSTEM SI */

/*<    40 CME = CMOLD >*/
L40:
    itcom3_1.cme = cmold;
/*<       ADAPT = .FALSE. >*/
    itcom2_1.adapt = FALSE_;
/*<       IF (LEVEL.GE.2) WRITE (NOUT,110) IN,CME >*/
    if (itcom1_1.level >= 2) {
	io___368.ciunit = itcom1_1.nout;
	s_wsfe(&io___368);
	do_fio(&c__1, (char *)&itcom1_1.in, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&itcom3_1.cme, (ftnlen)sizeof(doublereal));
	e_wsfe();
    }
/*<       RETURN >*/
    return 0;

/*     ----------------------------- */
/* ... SYMMETRIC SOR SEMI-ITERATIVE */
/*     ---------------------------- */

/*<    50 CONTINUE >*/
L50:
/*<       IF (SPECR.EQ.0.D0) SPECR = .171572875D0 >*/
    if (itcom3_1.specr == 0.) {
	itcom3_1.specr = .171572875;
    }
/*<       IF (IN.EQ.0) GO TO 60 >*/
    if (itcom1_1.in == 0) {
	goto L60;
    }
/*<       ZM1 = CHEBY(QA,QT,RRR,IN-IS,SPECR,0.D0) >*/
    i__1 = itcom1_1.in - itcom1_1.is;
    zm1 = cheby_(&itcom3_1.qa, &itcom3_1.qt, &itcom3_1.rrr, &i__1, &
	    itcom3_1.specr, &c_b21);
/*<       GO TO 70 >*/
    goto L70;
/*<    60 ZM1 = SPECR >*/
L60:
    zm1 = itcom3_1.specr;
/*<       SPR = SPECR >*/
    itcom3_1.spr = itcom3_1.specr;

/* ... RAYLEIGH QUOTIENT */

/*<    70 ZM2 = DTNRM/DELNNM >*/
L70:
    zm2 = *dtnrm / itcom3_1.delnnm;

/* ... COMPUTATION OF NEW ESTIMATE FOR SPECTRAL RADIUS */

/*<       IF (ADAPT) GO TO 80 >*/
    if (itcom2_1.adapt) {
	goto L80;
    }

/* ... PARTIALLY ADAPTIVE SSOR SI */

/*<       SPECR = DMAX1(ZM1,ZM2,SPECR) >*/
/* Computing MAX */
    d__1 = max(zm1,zm2);
    itcom3_1.specr = max(d__1,itcom3_1.specr);
/*<       IS = IN+1 >*/
    itcom1_1.is = itcom1_1.in + 1;
/*<       DELSNM = DELNNM >*/
    itcom3_1.delsnm = itcom3_1.delnnm;
/*<       IF (LEVEL.GE.2) WRITE (NOUT,100) IN,ZM1,ZM2,CME,SPECR >*/
    if (itcom1_1.level >= 2) {
	io___369.ciunit = itcom1_1.nout;
	s_wsfe(&io___369);
	do_fio(&c__1, (char *)&itcom1_1.in, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&zm1, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&zm2, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&itcom3_1.cme, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&itcom3_1.specr, (ftnlen)sizeof(doublereal));
	e_wsfe();
    }
/*<       RETURN >*/
    return 0;

/* ... FULLY ADAPTIVE SSOR SI */

/*<    80 SPR = DMAX1(ZM1,ZM2,SPR) >*/
L80:
/* Computing MAX */
    d__1 = max(zm1,zm2);
    itcom3_1.spr = max(d__1,itcom3_1.spr);
/*<       RETURN >*/
    return 0;

/* ... FORMAT STATEMENTS */

/*<    >*/

/*<    >*/

/*<    >*/

/*<       END >*/
} /* chgsi_ */

/*<       LOGICAL FUNCTION CHGSME (OLDNRM,ICNT) >*/
logical chgsme_(doublereal *oldnrm, integer *icnt)
{
    /* Format strings */
    static char fmt_20[] = "(/30x,\002ESTIMATE OF SMALLEST EIGENVALUE OF JAC\
OBI\002/37x,\002MATRIX (SME) CHANGED AT ITERATION \002,i5/35x,\002FIRST ESTI\
MATE OF SME            =\002,d15.7/35x,\002SECOND ESTIMATE OF SME           =\
\002,d15.7/35x,\002NEW ESTIMATE OF SME              =\002,d15.7/)";

    /* System generated locals */
    integer i__1;
    doublereal d__1, d__2, d__3;
    logical ret_val;

    /* Builtin functions */
    double sqrt(doublereal), pow_di(doublereal *, integer *), pow_dd(
	    doublereal *, doublereal *);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    doublereal q, z__;
    integer ip;
    doublereal rn, wp, sm1, sm2;

    /* Fortran I/O blocks */
    static cilist io___377 = { 0, 0, 0, fmt_20, 0 };



/* ... THIS FUNCTION TESTS FOR JACOBI SI WHETHER SME SHOULD BE CHANGED */
/* ... WHEN CASEII = .FALSE..  IF THE TEST IS POSITIVE THE NEW VALUE */
/* ... OF SME IS COMPUTED. */

/* ... PARAMETER LIST: */

/*          OLDNRM SQUARE OF THE NORM OF THE PSEUDO-RESIDUAL */
/*                    AT THE LAST ITERATION */
/*          ICNT   NUMBER OF ITERATIONS SINCE LAST CHANGE OF */
/*                    PARAMETERS */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER ICNT >*/
/*<       DOUBLE PRECISION OLDNRM >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER IP >*/
/*<       DOUBLE PRECISION Q,RN,SM1,SM2,WP,Z >*/

/* *** BEGIN: ITPACK COMMON */

/*<       INTEGER IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/
/*<       COMMON /ITCOM1/ IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/

/*<       LOGICAL ADAPT,BETADT,CASEII,HALT,PARTAD >*/
/*<       COMMON /ITCOM2/ ADAPT,BETADT,CASEII,HALT,PARTAD >*/

/*<    >*/
/*<    >*/

/* *** END  : ITPACK COMMON */

/*     DESCRIPTION OF VARIABLES IN COMMON BLOCKS IN MAIN SUBROUTINE */

/*<       CHGSME = .FALSE. >*/
    ret_val = FALSE_;
/*<       RN = DSQRT(DELNNM/OLDNRM) >*/
    rn = sqrt(itcom3_1.delnnm / *oldnrm);
/*<       IF (.NOT.(QA.GT.1.D0.AND.RN.GT.1.D0)) RETURN >*/
    if (! (itcom3_1.qa > 1. && rn > 1.)) {
	return ret_val;
    }
/*<       IF (IN.LE.IS+2) RETURN >*/
    if (itcom1_1.in <= itcom1_1.is + 2) {
	return ret_val;
    }

/*<       ICNT = ICNT+1 >*/
    ++(*icnt);
/*<       IF (ICNT.LT.3) RETURN >*/
    if (*icnt < 3) {
	return ret_val;
    }

/* ... CHANGE SME IN J-SI ADAPTIVE PROCEDURE */

/*<       CHGSME = .TRUE. >*/
    ret_val = TRUE_;
/*<       SM1 = 0.D0 >*/
    sm1 = 0.;
/*<       SM2 = 0.D0 >*/
    sm2 = 0.;
/*<       IF (SME.GE.CME) GO TO 10 >*/
    if (itcom3_1.sme >= itcom3_1.cme) {
	goto L10;
    }

/* ... COMPUTE SM1 */

/*<       IP = IN-IS >*/
    ip = itcom1_1.in - itcom1_1.is;
/*<       Q = QA*(1.D0+RRR**IP)/(2.D0*DSQRT(RRR**IP)) >*/
    q = itcom3_1.qa * (pow_di(&itcom3_1.rrr, &ip) + 1.) / (sqrt(pow_di(&
	    itcom3_1.rrr, &ip)) * 2.);
/*<       Z = (Q+DSQRT(Q**2-1.D0))**(1.D0/DBLE(FLOAT(IP))) >*/
/* Computing 2nd power */
    d__2 = q;
    d__1 = q + sqrt(d__2 * d__2 - 1.);
    d__3 = 1. / (doublereal) ((real) ip);
    z__ = pow_dd(&d__1, &d__3);
/*<       WP = (Z**2+1.D0)/(2.D0*Z) >*/
/* Computing 2nd power */
    d__1 = z__;
    wp = (d__1 * d__1 + 1.) / (z__ * 2.);
/*<       SM1 = .5D0*(CME+SME-WP*(CME-SME)) >*/
    sm1 = (itcom3_1.cme + itcom3_1.sme - wp * (itcom3_1.cme - itcom3_1.sme)) *
	     .5;

/* ... COMPUTE SM2 */

/*<       Q = RN*(1.D0+RRR**IP)/((1.D0+RRR**(IP-1))*DSQRT(RRR)) >*/
    i__1 = ip - 1;
    q = rn * (pow_di(&itcom3_1.rrr, &ip) + 1.) / ((pow_di(&itcom3_1.rrr, &
	    i__1) + 1.) * sqrt(itcom3_1.rrr));
/*<       WP = (Q**2+1.D0)/(2.D0*Q) >*/
/* Computing 2nd power */
    d__1 = q;
    wp = (d__1 * d__1 + 1.) / (q * 2.);
/*<       SM2 = .5D0*(CME+SME-WP*(CME-SME)) >*/
    sm2 = (itcom3_1.cme + itcom3_1.sme - wp * (itcom3_1.cme - itcom3_1.sme)) *
	     .5;

/*<    10 SME = DMIN1(1.25D0*SM1,1.25D0*SM2,SME,-1.D0) >*/
L10:
/* Computing MIN */
    d__1 = sm1 * 1.25, d__2 = sm2 * 1.25, d__1 = min(d__1,d__2), d__1 = min(
	    d__1,itcom3_1.sme);
    itcom3_1.sme = min(d__1,-1.);
/*<       SIGE = (CME-SME)/(2.D0-CME-SME) >*/
    itcom3_1.sige = (itcom3_1.cme - itcom3_1.sme) / (2. - itcom3_1.cme - 
	    itcom3_1.sme);
/*<       GAMMA = 2.D0/(2.D0-CME-SME) >*/
    itcom3_1.gamma = 2. / (2. - itcom3_1.cme - itcom3_1.sme);
/*<       RRR = (1.D0-DSQRT(1.D0-SIGE**2))/(1.D0+DSQRT(1.D0-SIGE**2)) >*/
/* Computing 2nd power */
    d__1 = itcom3_1.sige;
/* Computing 2nd power */
    d__2 = itcom3_1.sige;
    itcom3_1.rrr = (1. - sqrt(1. - d__1 * d__1)) / (sqrt(1. - d__2 * d__2) + 
	    1.);
/*<       IS = IN >*/
    itcom1_1.is = itcom1_1.in;
/*<       DELSNM = DELNNM >*/
    itcom3_1.delsnm = itcom3_1.delnnm;
/*<       RHO = 1.D0 >*/
    itcom3_1.rho = 1.;

/*<       IF (LEVEL.GE.2) WRITE (NOUT,20) IN,SM1,SM2,SME >*/
    if (itcom1_1.level >= 2) {
	io___377.ciunit = itcom1_1.nout;
	s_wsfe(&io___377);
	do_fio(&c__1, (char *)&itcom1_1.in, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&sm1, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&sm2, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&itcom3_1.sme, (ftnlen)sizeof(doublereal));
	e_wsfe();
    }

/*<    >*/

/*<       RETURN >*/
    return ret_val;
/*<       END >*/
} /* chgsme_ */

/*<       DOUBLE PRECISION FUNCTION DETERM (N,TRI,XLMDA) >*/
doublereal determ_(integer *n, doublereal *tri, doublereal *xlmda)
{
    /* System generated locals */
    integer i__1;
    doublereal ret_val;

    /* Local variables */
    integer l;
    doublereal d1, d2, d3;
    integer nm1, icnt;


/*     THIS SUBROUTINE COMPUTES THE DETERMINANT OF A SYMMETRIC */
/*     TRIDIAGONAL MATRIX GIVEN BY TRI. DET(TRI - XLMDA*I) = 0 */

/* ... PARAMETER LIST */

/*          N      ORDER OF TRIDIAGONAL SYSTEM */
/*          TRI    SYMMETRIC TRIDIAGONAL MATRIX OF ORDER N */
/*          XLMDA  ARGUMENT FOR CHARACTERISTIC EQUATION */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER N >*/
/*<       DOUBLE PRECISION TRI(2,1),XLMDA >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER ICNT,L,NM1 >*/
/*<       DOUBLE PRECISION D1,D2,D3 >*/

/*<       NM1 = N-1 >*/
    /* Parameter adjustments */
    tri -= 3;

    /* Function Body */
    nm1 = *n - 1;
/*<       D2 = TRI(1,N)-XLMDA >*/
    d2 = tri[(*n << 1) + 1] - *xlmda;
/*<       D1 = D2*(TRI(1,NM1)-XLMDA)-TRI(2,N) >*/
    d1 = d2 * (tri[(nm1 << 1) + 1] - *xlmda) - tri[(*n << 1) + 2];
/*<       IF (N.EQ.2) GO TO 20 >*/
    if (*n == 2) {
	goto L20;
    }

/* ... BEGINNING OF LOOP */

/*<       DO 10 ICNT = 2,NM1 >*/
    i__1 = nm1;
    for (icnt = 2; icnt <= i__1; ++icnt) {
/*<          L = NM1-ICNT+2 >*/
	l = nm1 - icnt + 2;
/*<          D3 = D2 >*/
	d3 = d2;
/*<          D2 = D1 >*/
	d2 = d1;
/*<          D1 = (TRI(1,L-1)-XLMDA)*D2-D3*TRI(2,L) >*/
	d1 = (tri[(l - 1 << 1) + 1] - *xlmda) * d2 - d3 * tri[(l << 1) + 2];
/*<    10 CONTINUE >*/
/* L10: */
    }

/* ... DETERMINANT COMPUTED */

/*<    20 DETERM = D1 >*/
L20:
    ret_val = d1;

/*<       RETURN >*/
    return ret_val;
/*<       END >*/
} /* determ_ */

/*<       SUBROUTINE DFAULT (IPARM,RPARM) >*/
/* Subroutine */ int dfault_(integer *iparm, doublereal *rparm)
{

/* ... THIS SUBROUTINE SETS THE DEFAULT VALUES OF IPARM AND RPARM. */

/* ... PARAMETER LIST: */

/*          IPARM */
/*           AND */
/*          RPARM  ARRAYS SPECIFYING OPTIONS AND TOLERANCES */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER IPARM(12) >*/
/*<       DOUBLE PRECISION RPARM(12) >*/

/* *** BEGIN: ITPACK COMMON */

/*<       INTEGER IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/
/*<       COMMON /ITCOM1/ IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/

/*<       LOGICAL ADAPT,BETADT,CASEII,HALT,PARTAD >*/
/*<       COMMON /ITCOM2/ ADAPT,BETADT,CASEII,HALT,PARTAD >*/

/*<    >*/
/*<    >*/

/* *** END  : ITPACK COMMON */

/*     DESCRIPTION OF VARIABLES IN COMMON BLOCKS IN MAIN SUBROUTINE */

/*     DRELPR  - COMPUTER PRECISION (APPROX.) */
/*     IF INSTALLER OF PACKAGE DOES NOT KNOW DRELPR VALUE, */
/*     AN APPROXIMATE VALUE CAN BE DETERMINED FROM A SIMPLE */
/*     FORTRAN PROGRAM SUCH AS */

/*     DOUBLE PRECISION DRELPR, TEMP */
/*     DRELPR = 1.0D0 */
/*   2 DRELPR = 0.5D0*DRELPR */
/*     TEMP = DRELPR + 1.0D0 */
/*     IF(TEMP .GT. 1.0D0)  GO TO 2 */
/*     WRITE(6,3) DRELPR */
/*   3 FORMAT(5X,D15.8) */
/*     STOP */
/*     END */

/*     SOME VALUES ARE: */

/*     DRELPR = 1.26D-29  FOR CDC CYBER 170/750  (APPROX.) 2**-96 */
/*            = 2.22D-16  FOR DEC 10             (APPROX.) 2**-52 */
/*            = 7.11D-15  FOR VAX 11/780         (APPROX.) 2**-47 */
/*            = 1.14D-13  FOR IBM 370/158        (APPROX.) 2**-43 */

/*             *** SHOULD BE CHANGED FOR OTHER MACHINES *** */

/*     TO FACILITATE CONVERGENCE, RPARM(1) SHOULD BE SET TO */
/*          500.*DRELPR OR LARGER */

/*<       DRELPR = 7.11D-15 >*/
    /* Parameter adjustments */
    --rparm;
    --iparm;

    /* Function Body */
    itcom3_1.drelpr = 7.11e-15;

/*<       IPARM(1) = 100 >*/
    iparm[1] = 100;
/*<       IPARM(2) = 0 >*/
    iparm[2] = 0;
/*<       IPARM(3) = 0 >*/
    iparm[3] = 0;
/*<       IPARM(4) = 6 >*/
    iparm[4] = 6;
/*<       IPARM(5) = 0 >*/
    iparm[5] = 0;
/*<       IPARM(6) = 1 >*/
    iparm[6] = 1;
/*<       IPARM(7) = 1 >*/
    iparm[7] = 1;
/*<       IPARM(8) = 0 >*/
    iparm[8] = 0;
/*<       IPARM(9) = -1 >*/
    iparm[9] = -1;
/*<       IPARM(10) = 0 >*/
    iparm[10] = 0;
/*<       IPARM(11) = 0 >*/
    iparm[11] = 0;
/*<       IPARM(12) = 0 >*/
    iparm[12] = 0;

/*<       RPARM(1) = 0.5D-5 >*/
    rparm[1] = 5e-6;
/*<       RPARM(2) = 0.D0 >*/
    rparm[2] = 0.;
/*<       RPARM(3) = 0.D0 >*/
    rparm[3] = 0.;
/*<       RPARM(4) = .75D0 >*/
    rparm[4] = .75;
/*<       RPARM(5) = 1.D0 >*/
    rparm[5] = 1.;
/*<       RPARM(6) = 0.D0 >*/
    rparm[6] = 0.;
/*<       RPARM(7) = .25D0 >*/
    rparm[7] = .25;
/*<       RPARM(8) = 1.D2*DRELPR >*/
    rparm[8] = itcom3_1.drelpr * 100.;
/*<       RPARM(9) = 0.D0 >*/
    rparm[9] = 0.;
/*<       RPARM(10) = 0.D0 >*/
    rparm[10] = 0.;
/*<       RPARM(11) = 0.D0 >*/
    rparm[11] = 0.;
/*<       RPARM(12) = 0.D0 >*/
    rparm[12] = 0.;

/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* dfault_ */

/*<       SUBROUTINE ECHALL (NN,IA,JA,A,RHS,IPARM,RPARM,ICALL) >*/
/* Subroutine */ int echall_(integer *nn, integer *ia, integer *ja, 
	doublereal *a, doublereal *rhs, integer *iparm, doublereal *rparm, 
	integer *icall)
{
    /* Format strings */
    static char fmt_10[] = "(///30x,\002THE LINEAR SYSTEM IS AS FOLLOWS\002)";
    static char fmt_20[] = "(/2x,\002IA ARRAY\002)";
    static char fmt_30[] = "(2x,10(2x,i8))";
    static char fmt_40[] = "(/2x,\002JA ARRAY\002)";
    static char fmt_50[] = "(/2x,\002 A ARRAY\002)";
    static char fmt_60[] = "(2x,5(2x,d20.13))";
    static char fmt_70[] = "(/2x,\002RHS ARRAY\002)";
    static char fmt_90[] = "(///30x,\002INITIAL ITERATIVE PARAMETERS\002)";
    static char fmt_110[] = "(///30x,\002FINAL ITERATIVE PARAMETERS\002)";
    static char fmt_130[] = "(35x,\002IPARM(1)  =\002,i15,4x,\002(ITMAX)\002\
/35x,\002IPARM(2)  =\002,i15,4x,\002(LEVEL) \002/35x,\002IPARM(3)  =\002,i15\
,4x,\002(IRESET)\002/35x,\002IPARM(4)  =\002,i15,4x,\002(NOUT)  \002/35x,\
\002IPARM(5)  =\002,i15,4x,\002(ISYM)  \002/35x,\002IPARM(6)  =\002,i15,4x\
,\002(IADAPT)\002)";
    static char fmt_140[] = "(35x,\002IPARM(7)  =\002,i15,4x,\002(ICASE)\002\
/35x,\002IPARM(8)  =\002,i15,4x,\002(NWKSP)\002/35x,\002IPARM(9)  =\002,i15,\
4x,\002(NB)    \002/35x,\002IPARM(10) =\002,i15,4x,\002(IREMOVE)\002/35x,\
\002IPARM(11) =\002,i15,4x,\002(ITIME)\002/35x,\002IPARM(12) =\002,i15,4x\
,\002(IDGTS)\002)";
    static char fmt_150[] = "(35x,\002RPARM(1)  =\002,d15.8,4x,\002(ZETA) \
 \002/35x,\002RPARM(2)  =\002,d15.8,4x,\002(CME)   \002/35x,\002RPARM(3)  \
=\002,d15.8,4x,\002(SME)   \002/35x,\002RPARM(4)  =\002,d15.8,4x,\002(FF)    \
\002/35x,\002RPARM(5)  =\002,d15.8,4x,\002(OMEGA) \002/35x,\002RPARM(6)  \
=\002,d15.8,4x,\002(SPECR) \002)";
    static char fmt_160[] = "(35x,\002RPARM(7)  =\002,d15.8,4x,\002(BETAB)\
 \002/35x,\002RPARM(8)  =\002,d15.8,4x,\002(TOL)\002/35x,\002RPARM(9)  =\002\
,d15.8,4x,\002(TIME1)\002/35x,\002RPARM(10) =\002,d15.8,4x,\002(TIME2)\002/3\
5x,\002RPARM(11) =\002,d15.8,4x,\002(DIGIT1)\002/35x,\002RPARM(12) =\002,d15\
.8,4x,\002(DIGIT2)\002)";

    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), e_wsfe(), do_fio(integer *, char *, ftnlen);

    /* Local variables */
    integer i__, n, np1, nzro;

    /* Fortran I/O blocks */
    static cilist io___387 = { 0, 0, 0, fmt_10, 0 };
    static cilist io___388 = { 0, 0, 0, fmt_20, 0 };
    static cilist io___389 = { 0, 0, 0, fmt_30, 0 };
    static cilist io___391 = { 0, 0, 0, fmt_40, 0 };
    static cilist io___392 = { 0, 0, 0, fmt_30, 0 };
    static cilist io___393 = { 0, 0, 0, fmt_50, 0 };
    static cilist io___394 = { 0, 0, 0, fmt_60, 0 };
    static cilist io___395 = { 0, 0, 0, fmt_70, 0 };
    static cilist io___396 = { 0, 0, 0, fmt_60, 0 };
    static cilist io___397 = { 0, 0, 0, fmt_90, 0 };
    static cilist io___398 = { 0, 0, 0, fmt_110, 0 };
    static cilist io___399 = { 0, 0, 0, fmt_130, 0 };
    static cilist io___400 = { 0, 0, 0, fmt_140, 0 };
    static cilist io___401 = { 0, 0, 0, fmt_150, 0 };
    static cilist io___402 = { 0, 0, 0, fmt_160, 0 };



/* ... THIS ROUTINE INITIALIZES THE ITPACK COMMON BLOCKS FROM THE */
/* ... INFORMATION CONTAINED IN IPARM AND RPARM. ECHALL ALSO PRINTS THE */
/* ... VALUES OF ALL THE PARAMETERS IN IPARM AND RPARM. */

/* ... PARAMETER LIST: */

/*          IPARM */
/*           AND */
/*          RPARM  ARRAYS OF PARAMETERS SPECIFYING OPTIONS AND */
/*                    TOLERANCES */
/*          ICALL  INDICATOR OF WHICH PARAMETERS ARE BEING PRINTED */
/*                    ICALL = 1,  INITIAL PARAMETERS */
/*                    ICALL = 2,  FINAL PARAMETERS */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER IA(1),JA(1),IPARM(12),NN,ICALL >*/
/*<       DOUBLE PRECISION A(1),RHS(NN),RPARM(12) >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER I,N,NP1,NZRO >*/

/* *** BEGIN: ITPACK COMMON */

/*<       INTEGER IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/
/*<       COMMON /ITCOM1/ IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/

/*<       LOGICAL ADAPT,BETADT,CASEII,HALT,PARTAD >*/
/*<       COMMON /ITCOM2/ ADAPT,BETADT,CASEII,HALT,PARTAD >*/

/*<    >*/
/*<    >*/

/* *** END  : ITPACK COMMON */

/*     DESCRIPTION OF VARIABLES IN COMMON BLOCKS IN MAIN SUBROUTINE */

/*<       IF (ICALL.NE.1) GO TO 100 >*/
    /* Parameter adjustments */
    --rhs;
    --ia;
    --ja;
    --a;
    --iparm;
    --rparm;

    /* Function Body */
    if (*icall != 1) {
	goto L100;
    }
/*<       N = NN >*/
    n = *nn;
/*<       NP1 = N+1 >*/
    np1 = n + 1;
/*<       NZRO = IA(NP1)-1 >*/
    nzro = ia[np1] - 1;

/* ... INITIALIZE ITPACK COMMON */

/*<       ZETA = RPARM(1) >*/
    itcom3_1.zeta = rparm[1];
/*<       CME = RPARM(2) >*/
    itcom3_1.cme = rparm[2];
/*<       SME = RPARM(3) >*/
    itcom3_1.sme = rparm[3];
/*<       FF = RPARM(4) >*/
    itcom3_1.ff = rparm[4];
/*<       OMEGA = RPARM(5) >*/
    itcom3_1.omega = rparm[5];
/*<       SPECR = RPARM(6) >*/
    itcom3_1.specr = rparm[6];
/*<       BETAB = RPARM(7) >*/
    itcom3_1.betab = rparm[7];
/*<       ITMAX = IPARM(1) >*/
    itcom1_1.itmax = iparm[1];
/*<       LEVEL = IPARM(2) >*/
    itcom1_1.level = iparm[2];
/*<       ISYM = IPARM(5) >*/
    itcom1_1.isym = iparm[5];

/*<       ADAPT = .FALSE. >*/
    itcom2_1.adapt = FALSE_;
/*<       PARTAD = .FALSE. >*/
    itcom2_1.partad = FALSE_;
/*<       BETADT = .FALSE. >*/
    itcom2_1.betadt = FALSE_;
/*<       IF (IPARM(6).EQ.1.OR.IPARM(6).EQ.3) ADAPT = .TRUE. >*/
    if (iparm[6] == 1 || iparm[6] == 3) {
	itcom2_1.adapt = TRUE_;
    }
/*<       IF (IPARM(6).EQ.1) BETADT = .TRUE. >*/
    if (iparm[6] == 1) {
	itcom2_1.betadt = TRUE_;
    }
/*<       IF (IPARM(6).EQ.2) PARTAD = .TRUE. >*/
    if (iparm[6] == 2) {
	itcom2_1.partad = TRUE_;
    }

/*<       CASEII = .FALSE. >*/
    itcom2_1.caseii = FALSE_;
/*<       IF (IPARM(7).EQ.2) CASEII = .TRUE. >*/
    if (iparm[7] == 2) {
	itcom2_1.caseii = TRUE_;
    }
/*<       IF (CASEII) SME = -CME >*/
    if (itcom2_1.caseii) {
	itcom3_1.sme = -itcom3_1.cme;
    }
/*<       IF (.NOT.CASEII.AND.SME.EQ.0.D0) SME = -1.D0 >*/
    if (! itcom2_1.caseii && itcom3_1.sme == 0.) {
	itcom3_1.sme = -1.;
    }
/*<       SPR = SME >*/
    itcom3_1.spr = itcom3_1.sme;

/* ... SET REST OF COMMON VARIABLES TO ZERO */

/*<       IN = 0 >*/
    itcom1_1.in = 0;
/*<       IS = 0 >*/
    itcom1_1.is = 0;
/*<       HALT = .FALSE. >*/
    itcom2_1.halt = FALSE_;
/*<       BDELNM = 0.D0 >*/
    itcom3_1.bdelnm = 0.;
/*<       DELNNM = 0.D0 >*/
    itcom3_1.delnnm = 0.;
/*<       DELSNM = 0.D0 >*/
    itcom3_1.delsnm = 0.;
/*<       GAMMA = 0.D0 >*/
    itcom3_1.gamma = 0.;
/*<       QA = 0.D0 >*/
    itcom3_1.qa = 0.;
/*<       QT = 0.D0 >*/
    itcom3_1.qt = 0.;
/*<       RHO = 0.D0 >*/
    itcom3_1.rho = 0.;
/*<       RRR = 0.D0 >*/
    itcom3_1.rrr = 0.;
/*<       SIGE = 0.D0 >*/
    itcom3_1.sige = 0.;
/*<       STPTST = 0.D0 >*/
    itcom3_1.stptst = 0.;
/*<       UDNM = 0.D0 >*/
    itcom3_1.udnm = 0.;

/*<       IF (LEVEL.LE.4) GO TO 80 >*/
    if (itcom1_1.level <= 4) {
	goto L80;
    }

/*     THIS SECTION OF ECHALL CAUSES PRINTING OF THE LINEAR SYSTEM AND */
/*     THE ITERATIVE PARAMETERS */

/*<       WRITE (NOUT,10) >*/
    io___387.ciunit = itcom1_1.nout;
    s_wsfe(&io___387);
    e_wsfe();
/*<    10 FORMAT (///30X,'THE LINEAR SYSTEM IS AS FOLLOWS') >*/
/*<       WRITE (NOUT,20) >*/
    io___388.ciunit = itcom1_1.nout;
    s_wsfe(&io___388);
    e_wsfe();
/*<    20 FORMAT (/2X,'IA ARRAY') >*/
/*<       WRITE (NOUT,30) (IA(I),I=1,NP1) >*/
    io___389.ciunit = itcom1_1.nout;
    s_wsfe(&io___389);
    i__1 = np1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	do_fio(&c__1, (char *)&ia[i__], (ftnlen)sizeof(integer));
    }
    e_wsfe();
/*<    30 FORMAT (2X,10(2X,I8)) >*/
/*<       WRITE (NOUT,40) >*/
    io___391.ciunit = itcom1_1.nout;
    s_wsfe(&io___391);
    e_wsfe();
/*<    40 FORMAT (/2X,'JA ARRAY') >*/
/*<       WRITE (NOUT,30) (JA(I),I=1,NZRO) >*/
    io___392.ciunit = itcom1_1.nout;
    s_wsfe(&io___392);
    i__1 = nzro;
    for (i__ = 1; i__ <= i__1; ++i__) {
	do_fio(&c__1, (char *)&ja[i__], (ftnlen)sizeof(integer));
    }
    e_wsfe();
/*<       WRITE (NOUT,50) >*/
    io___393.ciunit = itcom1_1.nout;
    s_wsfe(&io___393);
    e_wsfe();
/*<    50 FORMAT (/2X,' A ARRAY') >*/
/*<       WRITE (NOUT,60) (A(I),I=1,NZRO) >*/
    io___394.ciunit = itcom1_1.nout;
    s_wsfe(&io___394);
    i__1 = nzro;
    for (i__ = 1; i__ <= i__1; ++i__) {
	do_fio(&c__1, (char *)&a[i__], (ftnlen)sizeof(doublereal));
    }
    e_wsfe();
/*<    60 FORMAT (2X,5(2X,D20.13)) >*/
/*<       WRITE (NOUT,70) >*/
    io___395.ciunit = itcom1_1.nout;
    s_wsfe(&io___395);
    e_wsfe();
/*<    70 FORMAT (/2X,'RHS ARRAY') >*/
/*<       WRITE (NOUT,60) (RHS(I),I=1,N) >*/
    io___396.ciunit = itcom1_1.nout;
    s_wsfe(&io___396);
    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	do_fio(&c__1, (char *)&rhs[i__], (ftnlen)sizeof(doublereal));
    }
    e_wsfe();
/*<    80 WRITE (NOUT,90) >*/
L80:
    io___397.ciunit = itcom1_1.nout;
    s_wsfe(&io___397);
    e_wsfe();
/*<    90 FORMAT (///30X,'INITIAL ITERATIVE PARAMETERS') >*/
/*<       GO TO 120 >*/
    goto L120;
/*<   100 WRITE (NOUT,110) >*/
L100:
    io___398.ciunit = itcom1_1.nout;
    s_wsfe(&io___398);
    e_wsfe();
/*<   110 FORMAT (///30X,'FINAL ITERATIVE PARAMETERS') >*/
/*<   120 WRITE (NOUT,130) IPARM(1),LEVEL,IPARM(3),NOUT,ISYM,IPARM(6) >*/
L120:
    io___399.ciunit = itcom1_1.nout;
    s_wsfe(&io___399);
    do_fio(&c__1, (char *)&iparm[1], (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&itcom1_1.level, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&iparm[3], (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&itcom1_1.nout, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&itcom1_1.isym, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&iparm[6], (ftnlen)sizeof(integer));
    e_wsfe();
/*<    >*/
/*<    >*/
    io___400.ciunit = itcom1_1.nout;
    s_wsfe(&io___400);
    do_fio(&c__1, (char *)&iparm[7], (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&iparm[8], (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&iparm[9], (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&iparm[10], (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&iparm[11], (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&iparm[12], (ftnlen)sizeof(integer));
    e_wsfe();
/*<    >*/
/*<       WRITE (NOUT,150) ZETA,CME,SME,FF,OMEGA,SPECR >*/
    io___401.ciunit = itcom1_1.nout;
    s_wsfe(&io___401);
    do_fio(&c__1, (char *)&itcom3_1.zeta, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&itcom3_1.cme, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&itcom3_1.sme, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&itcom3_1.ff, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&itcom3_1.omega, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&itcom3_1.specr, (ftnlen)sizeof(doublereal));
    e_wsfe();
/*<    >*/
/*<    >*/
    io___402.ciunit = itcom1_1.nout;
    s_wsfe(&io___402);
    do_fio(&c__1, (char *)&itcom3_1.betab, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&rparm[8], (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&rparm[9], (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&rparm[10], (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&rparm[11], (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&rparm[12], (ftnlen)sizeof(doublereal));
    e_wsfe();
/*<    >*/

/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* echall_ */

/*<       SUBROUTINE ECHOUT (IPARM,RPARM,IMTHD) >*/
/* Subroutine */ int echout_(integer *iparm, doublereal *rparm, integer *
	imthd)
{
    /* Format strings */
    static char fmt_10[] = "(///30x,\002INITIAL ITERATIVE PARAMETERS\002,3x\
,\002RELEVANT SWITCHES\002/35x,\002ISYM   =\002,i15,8x,\002IPARM(5)\002/35x\
,\002ITMAX  =\002,i15,8x,\002IPARM(1)\002/35x,\002ZETA   =\002,d15.8,8x,\002\
RPARM(1)\002/35x,\002ADAPT  =\002,l15,8x,\002IPARM(6)\002/35x,\002CASEII \
=\002,l15,8x,\002IPARM(7)\002)";
    static char fmt_30[] = "(35x,\002FF     =\002,d15.8,8x,\002RPARM(4)\002/\
35x,\002CME    =\002,d15.8,8x,\002RPARM(2)\002/35x,\002SME    =\002,d15.8,8x,\
\002RPARM(3)\002///)";
    static char fmt_50[] = "(35x,\002PARTAD =\002,l15,8x,\002IPARM(6)\002/35\
x,\002FF     =\002,d15.8,8x,\002RPARM(4)\002/35x,\002CME    =\002,d15.8,8x\
,\002RPARM(2)\002/35x,\002OMEGA  =\002,d15.8,8x,\002RPARM(5)\002/35x,\002SPE\
CR  =\002,d15.8,8x,\002RPARM(6)\002/35x,\002BETAB  =\002,d15.8,8x,\002RPARM(\
7)\002/35x,\002BETADT =\002,l15,8x,\002IPARM(6)\002///)";
    static char fmt_70[] = "(35x,\002PARTAD =\002,l15,8x,\002IPARM(6)\002/35\
x,\002CME    =\002,d15.8,8x,\002RPARM(2)\002/35x,\002OMEGA  =\002,d15.8,8x\
,\002RPARM(5)\002/35x,\002SPECR  =\002,d15.8,8x,\002RPARM(6)\002/35x,\002BET\
AB  =\002,d15.8,8x,\002RPARM(7)\002/35x,\002BETADT =\002,l15,8x,\002IPARM(6\
)\002///)";
    static char fmt_90[] = "(35x,\002CME    =\002,d15.8,8x,\002RPARM(2)\002/\
//)";

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Fortran I/O blocks */
    static cilist io___403 = { 0, 0, 0, fmt_10, 0 };
    static cilist io___404 = { 0, 0, 0, fmt_30, 0 };
    static cilist io___405 = { 0, 0, 0, fmt_50, 0 };
    static cilist io___406 = { 0, 0, 0, fmt_70, 0 };
    static cilist io___407 = { 0, 0, 0, fmt_90, 0 };



/*     THIS ROUTINE INITIALIZES THE ITPACK COMMON BLOCKS FROM THE */
/*     INFORMATION CONTAINED IN IPARM AND RPARM. */

/* ... PARAMETER LIST: */

/*          IPARM */
/*           AND */
/*          RPARM  ARRAYS OF PARAMETERS SPECIFYING OPTIONS AND */
/*                    TOLERANCES */
/*          IMTHD  INDICATOR OF METHOD */
/*                    IMTHD = 1,  JCG */
/*                    IMTHD = 2,  JSI */
/*                    IMTHD = 3,  SOR */
/*                    IMTHD = 4,  SSORCG */
/*                    IMTHD = 5,  SSORSI */
/*                    IMTHD = 6,  RSCG */
/*                    IMTHD = 7,  RSSI */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER IPARM(12),IMTHD >*/
/*<       DOUBLE PRECISION RPARM(12) >*/

/* *** BEGIN: ITPACK COMMON */

/*<       INTEGER IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/
/*<       COMMON /ITCOM1/ IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/

/*<       LOGICAL ADAPT,BETADT,CASEII,HALT,PARTAD >*/
/*<       COMMON /ITCOM2/ ADAPT,BETADT,CASEII,HALT,PARTAD >*/

/*<    >*/
/*<    >*/

/* *** END  : ITPACK COMMON */

/*     DESCRIPTION OF VARIABLES IN COMMON BLOCKS IN MAIN SUBROUTINE */

/* ... INITIALIZE ITPACK COMMON */

/*<       ZETA = RPARM(1) >*/
    /* Parameter adjustments */
    --rparm;
    --iparm;

    /* Function Body */
    itcom3_1.zeta = rparm[1];
/*<       CME = RPARM(2) >*/
    itcom3_1.cme = rparm[2];
/*<       SME = RPARM(3) >*/
    itcom3_1.sme = rparm[3];
/*<       FF = RPARM(4) >*/
    itcom3_1.ff = rparm[4];
/*<       OMEGA = RPARM(5) >*/
    itcom3_1.omega = rparm[5];
/*<       SPECR = RPARM(6) >*/
    itcom3_1.specr = rparm[6];
/*<       BETAB = RPARM(7) >*/
    itcom3_1.betab = rparm[7];
/*<       ITMAX = IPARM(1) >*/
    itcom1_1.itmax = iparm[1];
/*<       LEVEL = IPARM(2) >*/
    itcom1_1.level = iparm[2];
/*<       ISYM = IPARM(5) >*/
    itcom1_1.isym = iparm[5];

/*<       ADAPT = .FALSE. >*/
    itcom2_1.adapt = FALSE_;
/*<       PARTAD = .FALSE. >*/
    itcom2_1.partad = FALSE_;
/*<       BETADT = .FALSE. >*/
    itcom2_1.betadt = FALSE_;
/*<       IF (IPARM(6).EQ.1.OR.IPARM(6).EQ.3) ADAPT = .TRUE. >*/
    if (iparm[6] == 1 || iparm[6] == 3) {
	itcom2_1.adapt = TRUE_;
    }
/*<       IF (IPARM(6).EQ.1) BETADT = .TRUE. >*/
    if (iparm[6] == 1) {
	itcom2_1.betadt = TRUE_;
    }
/*<       IF (IPARM(6).EQ.2) PARTAD = .TRUE. >*/
    if (iparm[6] == 2) {
	itcom2_1.partad = TRUE_;
    }

/*<       CASEII = .FALSE. >*/
    itcom2_1.caseii = FALSE_;
/*<       IF (IPARM(7).EQ.2) CASEII = .TRUE. >*/
    if (iparm[7] == 2) {
	itcom2_1.caseii = TRUE_;
    }
/*<       IF (CASEII) SME = -CME >*/
    if (itcom2_1.caseii) {
	itcom3_1.sme = -itcom3_1.cme;
    }
/*<       IF (.NOT.CASEII.AND.SME.EQ.0.D0) SME = -1.D0 >*/
    if (! itcom2_1.caseii && itcom3_1.sme == 0.) {
	itcom3_1.sme = -1.;
    }
/*<       SPR = SME >*/
    itcom3_1.spr = itcom3_1.sme;

/* ... SET REST OF COMMON VARIABLES TO ZERO */

/*<       IN = 0 >*/
    itcom1_1.in = 0;
/*<       IS = 0 >*/
    itcom1_1.is = 0;
/*<       HALT = .FALSE. >*/
    itcom2_1.halt = FALSE_;
/*<       BDELNM = 0.D0 >*/
    itcom3_1.bdelnm = 0.;
/*<       DELNNM = 0.D0 >*/
    itcom3_1.delnnm = 0.;
/*<       DELSNM = 0.D0 >*/
    itcom3_1.delsnm = 0.;
/*<       GAMMA = 0.D0 >*/
    itcom3_1.gamma = 0.;
/*<       QA = 0.D0 >*/
    itcom3_1.qa = 0.;
/*<       QT = 0.D0 >*/
    itcom3_1.qt = 0.;
/*<       RHO = 0.D0 >*/
    itcom3_1.rho = 0.;
/*<       RRR = 0.D0 >*/
    itcom3_1.rrr = 0.;
/*<       SIGE = 0.D0 >*/
    itcom3_1.sige = 0.;
/*<       STPTST = 0.D0 >*/
    itcom3_1.stptst = 0.;
/*<       UDNM = 0.D0 >*/
    itcom3_1.udnm = 0.;
/*<       IF (LEVEL.LE.2) RETURN >*/
    if (itcom1_1.level <= 2) {
	return 0;
    }

/* ... THIS SECTION OF ECHOUT ECHOES THE INPUT VALUES FOR THE INITIAL */
/*     ITERATIVE PARAMETERS */

/*<       WRITE (NOUT,10) ISYM,ITMAX,ZETA,ADAPT,CASEII >*/
    io___403.ciunit = itcom1_1.nout;
    s_wsfe(&io___403);
    do_fio(&c__1, (char *)&itcom1_1.isym, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&itcom1_1.itmax, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&itcom3_1.zeta, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&itcom2_1.adapt, (ftnlen)sizeof(logical));
    do_fio(&c__1, (char *)&itcom2_1.caseii, (ftnlen)sizeof(logical));
    e_wsfe();
/*<    >*/
/*<       GO TO (80,20,100,60,40,80,20), IMTHD >*/
    switch (*imthd) {
	case 1:  goto L80;
	case 2:  goto L20;
	case 3:  goto L100;
	case 4:  goto L60;
	case 5:  goto L40;
	case 6:  goto L80;
	case 7:  goto L20;
    }

/* ... JSI, RSSI */

/*<    20 WRITE (NOUT,30) FF,CME,SME >*/
L20:
    io___404.ciunit = itcom1_1.nout;
    s_wsfe(&io___404);
    do_fio(&c__1, (char *)&itcom3_1.ff, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&itcom3_1.cme, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&itcom3_1.sme, (ftnlen)sizeof(doublereal));
    e_wsfe();
/*<    >*/
/*<       RETURN >*/
    return 0;

/* ... SSORSI */

/*<    40 WRITE (NOUT,50) PARTAD,FF,CME,OMEGA,SPECR,BETAB,BETADT >*/
L40:
    io___405.ciunit = itcom1_1.nout;
    s_wsfe(&io___405);
    do_fio(&c__1, (char *)&itcom2_1.partad, (ftnlen)sizeof(logical));
    do_fio(&c__1, (char *)&itcom3_1.ff, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&itcom3_1.cme, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&itcom3_1.omega, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&itcom3_1.specr, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&itcom3_1.betab, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&itcom2_1.betadt, (ftnlen)sizeof(logical));
    e_wsfe();
/*<    >*/
/*<       RETURN >*/
    return 0;

/* ... SSORCG */

/*<    60 WRITE (NOUT,70) PARTAD,CME,OMEGA,SPECR,BETAB,BETADT >*/
L60:
    io___406.ciunit = itcom1_1.nout;
    s_wsfe(&io___406);
    do_fio(&c__1, (char *)&itcom2_1.partad, (ftnlen)sizeof(logical));
    do_fio(&c__1, (char *)&itcom3_1.cme, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&itcom3_1.omega, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&itcom3_1.specr, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&itcom3_1.betab, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&itcom2_1.betadt, (ftnlen)sizeof(logical));
    e_wsfe();
/*<    >*/
/*<       RETURN >*/
    return 0;

/* ... JCG, RSCG */

/*<    80 IF (ADAPT) RETURN >*/
L80:
    if (itcom2_1.adapt) {
	return 0;
    }
/*<       WRITE (NOUT,90) CME >*/
    io___407.ciunit = itcom1_1.nout;
    s_wsfe(&io___407);
    do_fio(&c__1, (char *)&itcom3_1.cme, (ftnlen)sizeof(doublereal));
    e_wsfe();
/*<    90 FORMAT (35X,'CME    =',D15.8,8X,'RPARM(2)'///) >*/

/*<   100 CONTINUE >*/
L100:
/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* echout_ */

/*<       DOUBLE PRECISION FUNCTION EIGVNS (N,TRI,D,E2,IER) >*/
doublereal eigvns_(integer *n, doublereal *tri, doublereal *d__, doublereal *
	e2, integer *ier)
{
    /* System generated locals */
    integer i__1;
    doublereal ret_val, d__1;

    /* Local variables */
    integer i__;
    extern /* Subroutine */ int eqrt1s_(doublereal *, doublereal *, integer *,
	     integer *, integer *, integer *);


/*     COMPUTES THE LARGEST EIGENVALUE OF A SYMMETRIC TRIDIAGONAL MATRIX */
/*     FOR CONJUGATE GRADIENT ACCELERATION. */

/* ... PARAMETER LIST: */

/*          N      ORDER OF TRIDIAGONAL SYSTEM */
/*          TRI    SYMMETRIC TRIDIAGONAL MATRIX OF ORDER N */
/*          D      ARRAY FOR EQRT1S (NEGATIVE DIAGONAL ELEMENTS) */
/*          E2     ARRAY FOR EQRT1S (SUPER DIAGONAL ELEMENTS) */
/*          IER    ERROR FLAG: ON RETURN, IER=0 INDICATES THAT */
/*                    THE LARGEST EIGENVALUE OF TRI WAS FOUND. */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER N,IER >*/
/*<       DOUBLE PRECISION TRI(2,1),D(N),E2(N) >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER I >*/

/*<       EIGVNS = 0.D0 >*/
    /* Parameter adjustments */
    --e2;
    --d__;
    tri -= 3;

    /* Function Body */
    ret_val = 0.;

/*<       D(1) = -TRI(1,1) >*/
    d__[1] = -tri[3];
/*<       DO 10 I = 2,N >*/
    i__1 = *n;
    for (i__ = 2; i__ <= i__1; ++i__) {
/*<          D(I) = -TRI(1,I) >*/
	d__[i__] = -tri[(i__ << 1) + 1];
/*<          E2(I) = DABS(TRI(2,I)) >*/
	e2[i__] = (d__1 = tri[(i__ << 1) + 2], abs(d__1));
/*<    10 CONTINUE >*/
/* L10: */
    }

/*<       CALL EQRT1S (D,E2,N,1,0,IER) >*/
    eqrt1s_(&d__[1], &e2[1], n, &c__1, &c__0, ier);
/*<       EIGVNS = -D(1) >*/
    ret_val = -d__[1];

/*<       RETURN >*/
    return ret_val;
/*<       END >*/
} /* eigvns_ */

/*<       DOUBLE PRECISION FUNCTION EIGVSS (N,TRI,START,ZETA,ITMAX,IER) >*/
doublereal eigvss_(integer *n, doublereal *tri, doublereal *start, doublereal 
	*zeta, integer *itmax, integer *ier)
{
    /* System generated locals */
    doublereal ret_val, d__1;

    /* Builtin functions */
    double d_lg10(doublereal *);

    /* Local variables */
    doublereal a, b, eps;
    integer nsig, itmp, maxfn;
    extern /* Subroutine */ int zbrent_(integer *, doublereal *, doublereal *,
	     integer *, doublereal *, doublereal *, integer *, integer *);


/*     COMPUTES THE LARGEST EIGENVALUE OF A SYMMETRIC TRIDIAGONAL MATRIX */
/*     FOR CONJUGATE GRADIENT ACCELERATION. */
/*     MODIFIED IMSL ROUTINE ZBRENT USED. */

/* ... PARAMETER LIST: */

/*          N      ORDER OF TRIDIAGONAL SYSTEM */
/*          TRI    SYMMETRIC TRIDIAGONAL MATRIX OF ORDER N */
/*          START  INITIAL LOWER BOUND OF INTERVAL CONTAINING ROOT */
/*          ZETA   STOPPING CRITERIA */
/*          IER    ERROR FLAG: ON RETURN, IER=0 INDICATES THAT */
/*                    THE LARGEST EIGENVALUE OF TRI WAS FOUND. */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER N,ITMAX,IER >*/
/*<       DOUBLE PRECISION TRI(2,1),START,ZETA,A,B,EPS >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER MAXFN,NSIG,ITMP >*/

/*<       EIGVSS = 0.D0 >*/
    /* Parameter adjustments */
    tri -= 3;

    /* Function Body */
    ret_val = 0.;
/*<       ITMP = IFIX(SNGL(-DLOG10(DABS(ZETA)))) >*/
    d__1 = abs(*zeta);
    itmp = (integer) ((real) (-d_lg10(&d__1)));
/*<       NSIG = MAX0(ITMP,4) >*/
    nsig = max(itmp,4);
/*<       MAXFN = MAX0(ITMAX,50) >*/
    maxfn = max(*itmax,50);

/*     EPS = DMIN1(ZETA,0.5D-4) */

/*<       EPS = 0.0D0 >*/
    eps = 0.;
/*<       A = START >*/
    a = *start;
/*<       B = 1.0D0 >*/
    b = 1.;
/*<       CALL ZBRENT (N,TRI,EPS,NSIG,A,B,MAXFN,IER) >*/
    zbrent_(n, &tri[3], &eps, &nsig, &a, &b, &maxfn, ier);
/*<       EIGVSS = B >*/
    ret_val = b;

/*<       RETURN >*/
    return ret_val;
/*<       END >*/
} /* eigvss_ */

/*<       SUBROUTINE EQRT1S (D,E2,NN,M,ISW,IERR) >*/
/* Subroutine */ int eqrt1s_(doublereal *d__, doublereal *e2, integer *nn, 
	integer *m, integer *isw, integer *ierr)
{
    /* Format strings */
    static char fmt_80[] = "(\0020\002,\002*** W A R N I N G ************\
\002/\002 \002,\002    IN ITPACK ROUTINE EQRT1S  \002/\002 \002,\002    PARA\
METER ISW = 1 BUT MATRIX   \002/\002 \002,\002    NOT POSITIVE DEFINITE\002)";
    static char fmt_150[] = "(\0020\002,\002*** W A R N I N G ***********\
*\002/\0020\002,\002    IN ITPACK ROUTINE EQRT1S  \002/\002 \002,\002    SUC\
CESSIVE ITERATES TO THE\002,i10/\002 \002,\002    EIGENVALUE WERE NOT MONOTO\
NE INCREASING \002)";

    /* System generated locals */
    integer i__1, i__2;
    doublereal d__1, d__2;

    /* Builtin functions */
    double sqrt(doublereal);
    integer s_wsfe(cilist *), e_wsfe(), do_fio(integer *, char *, ftnlen);

    /* Local variables */
    doublereal f;
    integer i__, j, k, n;
    doublereal p, q, r__, s;
    integer k1, ii, jj;
    doublereal ep, qp;
    integer ier;
    doublereal err, tot, dlam, delta;

    /* Fortran I/O blocks */
    static cilist io___428 = { 0, 0, 0, fmt_80, 0 };
    static cilist io___434 = { 0, 0, 0, fmt_80, 0 };
    static cilist io___435 = { 0, 0, 0, fmt_150, 0 };



/*   MODIFIED IMSL ROUTINE NAME   - EQRT1S */

/* ----------------------------------------------------------------------- */

/*   COMPUTER            - CDC/SINGLE */

/*   LATEST REVISION     - JUNE 1, 1980 */

/*   PURPOSE             - SMALLEST OR LARGEST M EIGENVALUES OF A */
/*                           SYMMETRIC TRIDIAGONAL MATRIX */

/*   USAGE               - CALL EQRT1S (D,E2,N,M,ISW,IER) */

/*   ARGUMENTS    D      - INPUT VECTOR OF LENGTH N CONTAINING */
/*                           THE DIAGONAL ELEMENTS OF THE MATRIX.  THE */
/*                           COMPUTED EIGENVALUES REPLACE THE FIRST M */
/*                           COMPONENTS OF THE VECTOR D IN NON- */
/*                           DECREASING SEQUENCE, WHILE THE REMAINING */
/*                           COMPONENTS ARE LOST. */
/*                E2     - INPUT VECTOR OF LENGTH N CONTAINING */
/*                           THE SQUARES OF THE OFF-DIAGONAL ELEMENTS */
/*                           OF THE MATRIX.  INPUT E2 IS DESTROYED. */
/*                N      - INPUT SCALAR CONTAINING THE ORDER OF THE */
/*                           MATRIX. (= NN) */
/*                M      - INPUT SCALAR CONTAINING THE NUMBER OF */
/*                           SMALLEST EIGENVALUES DESIRED (M IS */
/*                           LESS THAN OR EQUAL TO N). */
/*                ISW    - INPUT SCALAR MEANING AS FOLLOWS - */
/*                           ISW=1 MEANS THAT THE MATRIX IS KNOWN TO BE */
/*                             POSITIVE DEFINITE. */
/*                           ISW=0 MEANS THAT THE MATRIX IS NOT KNOWN */
/*                             TO BE POSITIVE DEFINITE. */
/*                IER    - ERROR PARAMETER. (OUTPUT) (= IERR) */
/*                           WARNING ERROR */
/*                             IER = 601 INDICATES THAT SUCCESSIVE */
/*                               ITERATES TO THE K-TH EIGENVALUE WERE NOT */
/*                               MONOTONE INCREASING. THE VALUE K IS */
/*                               STORED IN E2(1). */
/*                           TERMINAL ERROR */
/*                             IER = 602 INDICATES THAT ISW=1 BUT MATRIX */
/*                               IS NOT POSITIVE DEFINITE */

/*   PRECISION/HARDWARE  - SINGLE AND DOUBLE/H32 */
/*                       - SINGLE/H36,H48,H60 */

/*   NOTATION            - INFORMATION ON SPECIAL NOTATION AND */
/*                           CONVENTIONS IS AVAILABLE IN THE MANUAL */
/*                           INTRODUCTION OR THROUGH IMSL ROUTINE UHELP */

/*   REMARKS      AS WRITTEN, THE ROUTINE COMPUTES THE M SMALLEST */
/*                EIGENVALUES. TO COMPUTE THE M LARGEST EIGENVALUES, */
/*                REVERSE THE SIGN OF EACH ELEMENT OF D BEFORE AND */
/*                AFTER CALLING THE ROUTINE. IN THIS CASE, ISW MUST */
/*                EQUAL ZERO. */

/*   COPYRIGHT           - 1980 BY IMSL, INC. ALL RIGHTS RESERVED. */

/*   WARRANTY            - IMSL WARRANTS ONLY THAT IMSL TESTING HAS BEEN */
/*                           APPLIED TO THIS CODE. NO OTHER WARRANTY, */
/*                           EXPRESSED OR IMPLIED, IS APPLICABLE. */

/* ----------------------------------------------------------------------- */

/* *** BEGIN: ITPACK COMMON */

/*<       INTEGER IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/
/*<       COMMON /ITCOM1/ IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/

/*<       LOGICAL ADAPT,BETADT,CASEII,HALT,PARTAD >*/
/*<       COMMON /ITCOM2/ ADAPT,BETADT,CASEII,HALT,PARTAD >*/

/*<    >*/
/*<    >*/

/* *** END  : ITPACK COMMON */

/*     DESCRIPTION OF VARIABLES IN COMMON BLOCK IN MAIN SUBROUTINE */

/*                                  SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER NN,M,ISW,IERR >*/
/*<       DOUBLE PRECISION D(NN),E2(NN) >*/

/*                                  SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER II,I,JJ,J,K1,K,N,IER >*/
/*<       DOUBLE PRECISION DELTA,DLAM,EP,ERR,F,P,QP,Q,R,S,TOT >*/

/*                                  DRELPR = MACHINE PRECISION */
/*                                  FIRST EXECUTABLE STATEMENT */

/*<       N = NN >*/
    /* Parameter adjustments */
    --e2;
    --d__;

    /* Function Body */
    n = *nn;
/*<       IER = 0 >*/
    ier = 0;
/*<       DLAM = 0.0D0 >*/
    dlam = 0.;
/*<       ERR = 0.0D0 >*/
    err = 0.;
/*<       S = 0.0D0 >*/
    s = 0.;

/*                                  LOOK FOR SMALL SUB-DIAGONAL ENTRIES */
/*                                  DEFINE INITIAL SHIFT FROM LOWER */
/*                                  GERSCHGORIN BOUND. */

/*<       TOT = D(1) >*/
    tot = d__[1];
/*<       Q = 0.0D0 >*/
    q = 0.;
/*<       J = 0 >*/
    j = 0;
/*<       DO 30 I = 1,N >*/
    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<          P = Q >*/
	p = q;
/*<          IF (I.EQ.1) GO TO 10 >*/
	if (i__ == 1) {
	    goto L10;
	}
/*<          IF (P.GT.DRELPR*(DABS(D(I))+DABS(D(I-1)))) GO TO 20 >*/
	if (p > itcom3_1.drelpr * ((d__1 = d__[i__], abs(d__1)) + (d__2 = d__[
		i__ - 1], abs(d__2)))) {
	    goto L20;
	}
/*<    10    E2(I) = 0.0D0 >*/
L10:
	e2[i__] = 0.;

/*                                  COUNT IF E2(I) HAS UNDERFLOWED */

/*<    20    IF (E2(I).EQ.0.D0) J = J+1 >*/
L20:
	if (e2[i__] == 0.) {
	    ++j;
	}
/*<          Q = 0.0D0 >*/
	q = 0.;
/*<          IF (I.NE.N) Q = DSQRT(DABS(E2(I+1))) >*/
	if (i__ != n) {
	    q = sqrt((d__1 = e2[i__ + 1], abs(d__1)));
	}
/*<          TOT = DMIN1(D(I)-P-Q,TOT) >*/
/* Computing MIN */
	d__1 = d__[i__] - p - q;
	tot = min(d__1,tot);
/*<    30 CONTINUE >*/
/* L30: */
    }
/*<       IF (ISW.EQ.1.AND.TOT.LT.0.0D0) GO TO 50 >*/
    if (*isw == 1 && tot < 0.) {
	goto L50;
    }
/*<       DO 40 I = 1,N >*/
    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<          D(I) = D(I)-TOT >*/
	d__[i__] -= tot;
/*<    40 CONTINUE >*/
/* L40: */
    }
/*<       GO TO 60 >*/
    goto L60;
/*<    50 TOT = 0.0D0 >*/
L50:
    tot = 0.;
/*<    60 DO 200 K = 1,M >*/
L60:
    i__1 = *m;
    for (k = 1; k <= i__1; ++k) {

/*                                  NEXT QR TRANSFORMATION */

/*<    70    TOT = TOT+S >*/
L70:
	tot += s;
/*<          DELTA = D(N)-S >*/
	delta = d__[n] - s;
/*<          I = N >*/
	i__ = n;
/*<          F = DABS(DRELPR*TOT) >*/
	f = (d__1 = itcom3_1.drelpr * tot, abs(d__1));
/*<          IF (DLAM.LT.F) DLAM = F >*/
	if (dlam < f) {
	    dlam = f;
	}
/*<          IF (DELTA.GT.DLAM) GO TO 90 >*/
	if (delta > dlam) {
	    goto L90;
	}
/*<          IF (DELTA.GE.(-DLAM)) GO TO 170 >*/
	if (delta >= -dlam) {
	    goto L170;
	}
/*<          IER = 602 >*/
	ier = 602;
/*<          IF (LEVEL.GE.1) WRITE (NOUT,80) >*/
	if (itcom1_1.level >= 1) {
	    io___428.ciunit = itcom1_1.nout;
	    s_wsfe(&io___428);
	    e_wsfe();
	}
/*<    >*/
/*<          GO TO 210 >*/
	goto L210;

/*                                  REPLACE SMALL SUB-DIAGONAL SQUARES */
/*                                  BY ZERO TO REDUCE THE INCIDENCE OF */
/*                                  UNDERFLOWS */

/*<    90    IF (K.EQ.N) GO TO 110 >*/
L90:
	if (k == n) {
	    goto L110;
	}
/*<          K1 = K+1 >*/
	k1 = k + 1;
/*<          DO 100 J = K1,N >*/
	i__2 = n;
	for (j = k1; j <= i__2; ++j) {
/*<             IF (E2(J).LE.(DRELPR*(D(J)+D(J-1)))**2) E2(J) = 0.0D0 >*/
/* Computing 2nd power */
	    d__1 = itcom3_1.drelpr * (d__[j] + d__[j - 1]);
	    if (e2[j] <= d__1 * d__1) {
		e2[j] = 0.;
	    }
/*<   100    CONTINUE >*/
/* L100: */
	}
/*<   110    F = E2(N)/DELTA >*/
L110:
	f = e2[n] / delta;
/*<          QP = DELTA+F >*/
	qp = delta + f;
/*<          P = 1.0D0 >*/
	p = 1.;
/*<          IF (K.EQ.N) GO TO 140 >*/
	if (k == n) {
	    goto L140;
	}
/*<          K1 = N-K >*/
	k1 = n - k;
/*<          DO 130 II = 1,K1 >*/
	i__2 = k1;
	for (ii = 1; ii <= i__2; ++ii) {
/*<             I = N-II >*/
	    i__ = n - ii;
/*<             Q = D(I)-S-F >*/
	    q = d__[i__] - s - f;
/*<             R = Q/QP >*/
	    r__ = q / qp;
/*<             P = P*R+1.0D0 >*/
	    p = p * r__ + 1.;
/*<             EP = F*R >*/
	    ep = f * r__;
/*<             D(I+1) = QP+EP >*/
	    d__[i__ + 1] = qp + ep;
/*<             DELTA = Q-EP >*/
	    delta = q - ep;
/*<             IF (DELTA.GT.DLAM) GO TO 120 >*/
	    if (delta > dlam) {
		goto L120;
	    }
/*<             IF (DELTA.GE.(-DLAM)) GO TO 170 >*/
	    if (delta >= -dlam) {
		goto L170;
	    }
/*<             IER = 602 >*/
	    ier = 602;
/*<             IF (LEVEL.GE.0) WRITE (NOUT,80) >*/
	    if (itcom1_1.level >= 0) {
		io___434.ciunit = itcom1_1.nout;
		s_wsfe(&io___434);
		e_wsfe();
	    }
/*<             GO TO 210 >*/
	    goto L210;
/*<   120       F = E2(I)/Q >*/
L120:
	    f = e2[i__] / q;
/*<             QP = DELTA+F >*/
	    qp = delta + f;
/*<             E2(I+1) = QP*EP >*/
	    e2[i__ + 1] = qp * ep;
/*<   130    CONTINUE >*/
/* L130: */
	}
/*<   140    D(K) = QP >*/
L140:
	d__[k] = qp;
/*<          S = QP/P >*/
	s = qp / p;
/*<          IF (TOT+S.GT.TOT) GO TO 70 >*/
	if (tot + s > tot) {
	    goto L70;
	}
/*<          IER = 601 >*/
	ier = 601;
/*<          E2(1) = K >*/
	e2[1] = (doublereal) k;
/*<          IF (LEVEL.GE.1) WRITE (NOUT,150) K >*/
	if (itcom1_1.level >= 1) {
	    io___435.ciunit = itcom1_1.nout;
	    s_wsfe(&io___435);
	    do_fio(&c__1, (char *)&k, (ftnlen)sizeof(integer));
	    e_wsfe();
	}
/*<    >*/

/*                                  SET ERROR -- IRREGULAR END */
/*                                  DEFLATE MINIMUM DIAGONAL ELEMENT */

/*<          S = 0.0D0 >*/
	s = 0.;
/*<          DELTA = QP >*/
	delta = qp;
/*<          DO 160 J = K,N >*/
	i__2 = n;
	for (j = k; j <= i__2; ++j) {
/*<             IF (D(J).GT.DELTA) GO TO 160 >*/
	    if (d__[j] > delta) {
		goto L160;
	    }
/*<             I = J >*/
	    i__ = j;
/*<             DELTA = D(J) >*/
	    delta = d__[j];
/*<   160    CONTINUE >*/
L160:
	    ;
	}

/*                                  CONVERGENCE */

/*<   170    IF (I.LT.N) E2(I+1) = E2(I)*F/QP >*/
L170:
	if (i__ < n) {
	    e2[i__ + 1] = e2[i__] * f / qp;
	}
/*<          IF (I.EQ.K) GO TO 190 >*/
	if (i__ == k) {
	    goto L190;
	}
/*<          K1 = I-K >*/
	k1 = i__ - k;
/*<          DO 180 JJ = 1,K1 >*/
	i__2 = k1;
	for (jj = 1; jj <= i__2; ++jj) {
/*<             J = I-JJ >*/
	    j = i__ - jj;
/*<             D(J+1) = D(J)-S >*/
	    d__[j + 1] = d__[j] - s;
/*<             E2(J+1) = E2(J) >*/
	    e2[j + 1] = e2[j];
/*<   180    CONTINUE >*/
/* L180: */
	}
/*<   190    D(K) = TOT >*/
L190:
	d__[k] = tot;
/*<          ERR = ERR+DABS(DELTA) >*/
	err += abs(delta);
/*<          E2(K) = ERR >*/
	e2[k] = err;
/*<   200 CONTINUE >*/
/* L200: */
    }
/*<       IF (IER.EQ.0) GO TO 220 >*/
    if (ier == 0) {
	goto L220;
    }
/*<   210 CONTINUE >*/
L210:
/*<   220 IERR = IER >*/
L220:
    *ierr = ier;
/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* eqrt1s_ */

/*<       INTEGER FUNCTION IPSTR (OMEGA) >*/
integer ipstr_(doublereal *omega)
{
    /* System generated locals */
    integer ret_val, i__1;

    /* Builtin functions */
    double pow_di(doublereal *, integer *);

    /* Local variables */
    integer ip;
    doublereal wm1;


/*     FINDS THE SMALLEST INTEGER, IPSTR, GREATER THAN 5 SUCH THAT */
/*          IPSTR * (OMEGA-1)**(IPSTR-1) .LE. 0.50. IPSTR WILL BE SET */
/*          IN LOOP. */

/* ... PARAMETER LIST: */

/*          OMEGA  RELAXATION FACTOR FOR SOR METHOD */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       DOUBLE PRECISION OMEGA >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER IP >*/
/*<       DOUBLE PRECISION WM1 >*/

/*<       WM1 = OMEGA-1.D0 >*/
    wm1 = *omega - 1.;

/*<       DO 10 IP = 6,940 >*/
    for (ip = 6; ip <= 940; ++ip) {
/*<          IF (DBLE(FLOAT(IP))*(WM1**(IP-1)).GT.0.50D0) GO TO 10 >*/
	i__1 = ip - 1;
	if ((doublereal) ((real) ip) * pow_di(&wm1, &i__1) > .5) {
	    goto L10;
	}
/*<          IPSTR = IP >*/
	ret_val = ip;
/*<          RETURN >*/
	return ret_val;
/*<    10 CONTINUE >*/
L10:
	;
    }
/*<       IPSTR = 940 >*/
    ret_val = 940;
/*<       RETURN >*/
    return ret_val;

/*<       END >*/
} /* ipstr_ */

/*<       SUBROUTINE ITERM (NN,A,U,WK,IMTHDD) >*/
/* Subroutine */ int iterm_(integer *nn, doublereal *a, doublereal *u, 
	doublereal *wk, integer *imthdd)
{
    /* Format strings */
    static char fmt_20[] = "(////15x,\002INTERMEDIATE OUTPUT AFTER EACH ITER\
ATION\002//\002 NUMBER OF\002,5x,\002CONVERGENCE\002,7x,\002CME \002,11x,\
\002RHO\002,12x,\002GAMMA\002/\002 ITERATIONS\002,4x,\002TEST \002//)";
    static char fmt_40[] = "(4x,i5,3x,4d15.7)";
    static char fmt_60[] = "(////15x,\002INTERMEDIATE OUTPUT AFTER EACH ITER\
ATION\002//\002 NUMBER OF\002,4x,\002CONVERGENCE\002,7x,\002PARAMETER CHANGE\
 TEST\002,10x,\002RHO\002,12x,\002GAMMA\002/\002 ITERATIONS\002,3x,\002TEST\
 \002,11x,\002LHS(QA)\002,7x,\002RHS(QT**FF)\002//)";
    static char fmt_80[] = "(4x,i5,3x,5d15.7)";
    static char fmt_100[] = "(4x,i5,3x,d15.7,30x,2d15.7)";
    static char fmt_120[] = "(////15x,\002INTERMEDIATE OUTPUT AFTER EACH ITE\
RATION\002//\002 NUMBER OF\002,4x,\002CONVERGENCE\002,7x,\002PARAMETER CHANG\
E TEST\002,10x,\002RHO\002/\002 ITERATIONS\002,3x,\002TEST \002,11x,\002LHS(\
QA)\002,7x,\002RHS(QT**FF)\002//)";
    static char fmt_140[] = "(4x,i5,3x,5d15.7)";
    static char fmt_160[] = "(4x,i5,3x,d15.7,30x,d15.7)";
    static char fmt_180[] = "(////15x,\002INTERMEDIATE OUTPUT AFTER EACH ITE\
RATION\002//\002 NUMBER OF\002,4x,\002CONVERGENCE\002,6x,\002CME \002,9x,\
\002OMEGA\002,7x,\002SPECTRAL\002/\002 ITERATIONS\002,3x,\002TEST\002,38x\
,\002RADIUS\002//)";
    static char fmt_200[] = "(4x,i5,3x,4d14.7)";
    static char fmt_220[] = "(////15x,\002INTERMEDIATE OUTPUT AFTER EACH ITE\
RATION\002//\002 NUMBER OF\002,4x,\002CONVERGENCE\002,3x,\002 SPECTRAL\002,6\
x,\002S-PRIME\002,9x,\002RHO\002,10x,\002GAMMA\002/\002 ITERATIONS\002,3x\
,\002TEST \002,10x,\002RADIUS\002//)";
    static char fmt_240[] = "(4x,i5,3x,5d14.7)";
    static char fmt_260[] = "(\0020\002,2x,\002ESTIMATE OF SOLUTION AT ITERA\
TION \002,i5)";
    static char fmt_280[] = "(\0020\002,2x,\002ESTIMATE OF SOLUTION AT BLACK\
 POINTS \002,\002AT ITERATION \002,i5)";
    static char fmt_310[] = "(2x,5(2x,d20.13))";
    static char fmt_320[] = "(//)";

    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), e_wsfe(), do_fio(integer *, char *, ftnlen);
    double pow_dd(doublereal *, doublereal *);

    /* Local variables */
    integer i__, n, ip;
    doublereal qtff;
    integer imthd;

    /* Fortran I/O blocks */
    static cilist io___441 = { 0, 0, 0, fmt_20, 0 };
    static cilist io___442 = { 0, 0, 0, fmt_40, 0 };
    static cilist io___443 = { 0, 0, 0, fmt_60, 0 };
    static cilist io___446 = { 0, 0, 0, fmt_80, 0 };
    static cilist io___447 = { 0, 0, 0, fmt_100, 0 };
    static cilist io___448 = { 0, 0, 0, fmt_120, 0 };
    static cilist io___449 = { 0, 0, 0, fmt_140, 0 };
    static cilist io___450 = { 0, 0, 0, fmt_160, 0 };
    static cilist io___451 = { 0, 0, 0, fmt_180, 0 };
    static cilist io___452 = { 0, 0, 0, fmt_200, 0 };
    static cilist io___453 = { 0, 0, 0, fmt_220, 0 };
    static cilist io___454 = { 0, 0, 0, fmt_240, 0 };
    static cilist io___455 = { 0, 0, 0, fmt_260, 0 };
    static cilist io___456 = { 0, 0, 0, fmt_280, 0 };
    static cilist io___458 = { 0, 0, 0, fmt_310, 0 };
    static cilist io___459 = { 0, 0, 0, fmt_320, 0 };



/*     THIS ROUTINE PRODUCES THE ITERATION SUMMARY LINE AT THE END */
/*     OF EACH ITERATION. IF LEVEL = 5, THE LATEST APPROXIMATION */
/*     TO THE SOLUTION WILL BE PRINTED. */

/* ... PARAMETER LIST: */

/*          NN     ORDER OF SYSTEM OR, FOR REDUCED SYSTEM */
/*                    ROUTINES, ORDER OF BLACK SUBSYSTEM */
/*          A      ITERATION MATRIX */
/*          U      SOLUTION ESTIMATE */
/*          WK     WORK ARRAY OF LENGTH NN */
/*          IMTHD  INDICATOR OF METHOD (=IMTHDD) */
/*                    IMTHD = 1,  JCG */
/*                    IMTHD = 2,  JSI */
/*                    IMTHD = 3,  SOR */
/*                    IMTHD = 4,  SSORCG */
/*                    IMTHD = 5,  SSORSI */
/*                    IMTHD = 6,  RSCG */
/*                    IMTHD = 7,  RSSI */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER NN,IMTHD >*/
/*<       DOUBLE PRECISION A(1),U(NN),WK(NN) >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER I,IMTHDD,IP,N >*/
/*<       DOUBLE PRECISION QTFF >*/

/* *** BEGIN: ITPACK COMMON */

/*<       INTEGER IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/
/*<       COMMON /ITCOM1/ IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/

/*<       LOGICAL ADAPT,BETADT,CASEII,HALT,PARTAD >*/
/*<       COMMON /ITCOM2/ ADAPT,BETADT,CASEII,HALT,PARTAD >*/

/*<    >*/
/*<    >*/

/* *** END  : ITPACK COMMON */

/*<       N = NN >*/
    /* Parameter adjustments */
    --wk;
    --u;
    --a;

    /* Function Body */
    n = *nn;
/*<       IMTHD = IMTHDD >*/
    imthd = *imthdd;

/* ... PRINT VARIOUS PARAMETERS AFTER EACH ITERATION */

/*<       IF (LEVEL.LT.2) RETURN >*/
    if (itcom1_1.level < 2) {
	return 0;
    }
/*<       GO TO (10,110,170,210,50,10,110), IMTHD >*/
    switch (imthd) {
	case 1:  goto L10;
	case 2:  goto L110;
	case 3:  goto L170;
	case 4:  goto L210;
	case 5:  goto L50;
	case 6:  goto L10;
	case 7:  goto L110;
    }
/*<    10 IF (IN.GT.0) GO TO 30 >*/
L10:
    if (itcom1_1.in > 0) {
	goto L30;
    }

/* ... PRINT HEADER FOR JCG AND RSCG */

/*<       WRITE (NOUT,20) >*/
    io___441.ciunit = itcom1_1.nout;
    s_wsfe(&io___441);
    e_wsfe();
/*<    >*/

/* ... PRINT SUMMARY LINE */

/*<    30 WRITE (NOUT,40) IN,STPTST,CME,RHO,GAMMA >*/
L30:
    io___442.ciunit = itcom1_1.nout;
    s_wsfe(&io___442);
    do_fio(&c__1, (char *)&itcom1_1.in, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&itcom3_1.stptst, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&itcom3_1.cme, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&itcom3_1.rho, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&itcom3_1.gamma, (ftnlen)sizeof(doublereal));
    e_wsfe();
/*<    40 FORMAT (4X,I5,3X,4D15.7) >*/
/*<       IF (LEVEL.GE.4) GO TO 250 >*/
    if (itcom1_1.level >= 4) {
	goto L250;
    }

/*<       RETURN >*/
    return 0;

/*<    50 IF (IN.GT.0) GO TO 70 >*/
L50:
    if (itcom1_1.in > 0) {
	goto L70;
    }

/* ... PRINT HEADER FOR SSOR-SI */

/*<       WRITE (NOUT,60) >*/
    io___443.ciunit = itcom1_1.nout;
    s_wsfe(&io___443);
    e_wsfe();
/*<    >*/

/* ... PRINT SUMMARY LINE */

/*<    70 IP = IN-IS >*/
L70:
    ip = itcom1_1.in - itcom1_1.is;
/*<       IF (IMTHD.EQ.7) IP = 2*IP >*/
    if (imthd == 7) {
	ip <<= 1;
    }
/*<       IF (IP.LT.3) GO TO 90 >*/
    if (ip < 3) {
	goto L90;
    }
/*<       QTFF = QT**FF >*/
    qtff = pow_dd(&itcom3_1.qt, &itcom3_1.ff);
/*<       WRITE (NOUT,80) IN,STPTST,QA,QTFF,RHO,GAMMA >*/
    io___446.ciunit = itcom1_1.nout;
    s_wsfe(&io___446);
    do_fio(&c__1, (char *)&itcom1_1.in, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&itcom3_1.stptst, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&itcom3_1.qa, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&qtff, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&itcom3_1.rho, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&itcom3_1.gamma, (ftnlen)sizeof(doublereal));
    e_wsfe();
/*<    80 FORMAT (4X,I5,3X,5D15.7) >*/
/*<       IF (LEVEL.GE.4) GO TO 250 >*/
    if (itcom1_1.level >= 4) {
	goto L250;
    }
/*<       RETURN >*/
    return 0;

/*<    90 WRITE (NOUT,100) IN,STPTST,RHO,GAMMA >*/
L90:
    io___447.ciunit = itcom1_1.nout;
    s_wsfe(&io___447);
    do_fio(&c__1, (char *)&itcom1_1.in, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&itcom3_1.stptst, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&itcom3_1.rho, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&itcom3_1.gamma, (ftnlen)sizeof(doublereal));
    e_wsfe();
/*<   100 FORMAT (4X,I5,3X,D15.7,30X,2D15.7) >*/
/*<       IF (LEVEL.GE.4) GO TO 250 >*/
    if (itcom1_1.level >= 4) {
	goto L250;
    }
/*<       RETURN >*/
    return 0;

/*<   110 IF (IN.GT.0) GO TO 130 >*/
L110:
    if (itcom1_1.in > 0) {
	goto L130;
    }

/* ... PRINT HEADER FOR J-SI AND RS-SI */

/*<       WRITE (NOUT,120) >*/
    io___448.ciunit = itcom1_1.nout;
    s_wsfe(&io___448);
    e_wsfe();
/*<    >*/

/* ... PRINT SUMMARY LINE */

/*<   130 IP = IN-IS >*/
L130:
    ip = itcom1_1.in - itcom1_1.is;
/*<       IF (IMTHD.EQ.7) IP = 2*IP >*/
    if (imthd == 7) {
	ip <<= 1;
    }
/*<       IF (IP.LT.3) GO TO 150 >*/
    if (ip < 3) {
	goto L150;
    }
/*<       QTFF = QT**FF >*/
    qtff = pow_dd(&itcom3_1.qt, &itcom3_1.ff);
/*<       WRITE (NOUT,140) IN,STPTST,QA,QTFF,RHO >*/
    io___449.ciunit = itcom1_1.nout;
    s_wsfe(&io___449);
    do_fio(&c__1, (char *)&itcom1_1.in, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&itcom3_1.stptst, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&itcom3_1.qa, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&qtff, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&itcom3_1.rho, (ftnlen)sizeof(doublereal));
    e_wsfe();
/*<   140 FORMAT (4X,I5,3X,5D15.7) >*/
/*<       IF (LEVEL.GE.4) GO TO 250 >*/
    if (itcom1_1.level >= 4) {
	goto L250;
    }
/*<       RETURN >*/
    return 0;

/*<   150 WRITE (NOUT,160) IN,STPTST,RHO >*/
L150:
    io___450.ciunit = itcom1_1.nout;
    s_wsfe(&io___450);
    do_fio(&c__1, (char *)&itcom1_1.in, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&itcom3_1.stptst, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&itcom3_1.rho, (ftnlen)sizeof(doublereal));
    e_wsfe();
/*<   160 FORMAT (4X,I5,3X,D15.7,30X,D15.7) >*/
/*<       IF (LEVEL.GE.4) GO TO 250 >*/
    if (itcom1_1.level >= 4) {
	goto L250;
    }
/*<       RETURN >*/
    return 0;

/* ... PRINT VARIOUS PARAMETERS AFTER EACH ITERATION FOR SOR. */

/*<   170 IF (IN.GT.0) GO TO 190 >*/
L170:
    if (itcom1_1.in > 0) {
	goto L190;
    }

/* ... PRINT HEADER FOR SOR */

/*<       WRITE (NOUT,180) >*/
    io___451.ciunit = itcom1_1.nout;
    s_wsfe(&io___451);
    e_wsfe();
/*<    >*/

/* ... PRINT SUMMARY LINE FOR SOR */

/*<   190 CONTINUE >*/
L190:
/*<       WRITE (NOUT,200) IN,STPTST,CME,OMEGA,SPECR >*/
    io___452.ciunit = itcom1_1.nout;
    s_wsfe(&io___452);
    do_fio(&c__1, (char *)&itcom1_1.in, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&itcom3_1.stptst, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&itcom3_1.cme, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&itcom3_1.omega, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&itcom3_1.specr, (ftnlen)sizeof(doublereal));
    e_wsfe();
/*<   200 FORMAT (4X,I5,3X,4D14.7) >*/
/*<       IF (LEVEL.GE.4) GO TO 250 >*/
    if (itcom1_1.level >= 4) {
	goto L250;
    }

/*<       RETURN >*/
    return 0;

/* ... PRINT VARIOUS PARAMETERS AFTER EACH ITERATION FOR SSOR-CG. */

/*<   210 IF (IN.GT.0) GO TO 230 >*/
L210:
    if (itcom1_1.in > 0) {
	goto L230;
    }

/* ... PRINT HEADER FOR SSOR-CG */

/*<       WRITE (NOUT,220) >*/
    io___453.ciunit = itcom1_1.nout;
    s_wsfe(&io___453);
    e_wsfe();
/*<    >*/

/* ... PRINT SUMMARY LINE FOR SSOR-CG */

/*<   230 CONTINUE >*/
L230:
/*<       WRITE (NOUT,240) IN,STPTST,SPECR,SPR,RHO,GAMMA >*/
    io___454.ciunit = itcom1_1.nout;
    s_wsfe(&io___454);
    do_fio(&c__1, (char *)&itcom1_1.in, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&itcom3_1.stptst, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&itcom3_1.specr, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&itcom3_1.spr, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&itcom3_1.rho, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&itcom3_1.gamma, (ftnlen)sizeof(doublereal));
    e_wsfe();
/*<   240 FORMAT (4X,I5,3X,5D14.7) >*/
/*<       IF (LEVEL.GE.4) GO TO 250 >*/
    if (itcom1_1.level >= 4) {
	goto L250;
    }
/*<       RETURN >*/
    return 0;

/*<   250 IF (IMTHD.GT.5) GO TO 270 >*/
L250:
    if (imthd > 5) {
	goto L270;
    }
/*<       WRITE (NOUT,260) IN >*/
    io___455.ciunit = itcom1_1.nout;
    s_wsfe(&io___455);
    do_fio(&c__1, (char *)&itcom1_1.in, (ftnlen)sizeof(integer));
    e_wsfe();
/*<   260 FORMAT ('0',2X,'ESTIMATE OF SOLUTION AT ITERATION ',I5) >*/
/*<       GO TO 290 >*/
    goto L290;
/*<   270 WRITE (NOUT,280) IN >*/
L270:
    io___456.ciunit = itcom1_1.nout;
    s_wsfe(&io___456);
    do_fio(&c__1, (char *)&itcom1_1.in, (ftnlen)sizeof(integer));
    e_wsfe();
/*<    >*/
/*<   290 DO 300 I = 1,N >*/
L290:
    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<          WK(I) = U(I)/A(I) >*/
	wk[i__] = u[i__] / a[i__];
/*<   300 CONTINUE >*/
/* L300: */
    }
/*<       WRITE (NOUT,310) (WK(I),I=1,N) >*/
    io___458.ciunit = itcom1_1.nout;
    s_wsfe(&io___458);
    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	do_fio(&c__1, (char *)&wk[i__], (ftnlen)sizeof(doublereal));
    }
    e_wsfe();
/*<   310 FORMAT (2X,5(2X,D20.13)) >*/
/*<       WRITE (NOUT,320) >*/
    io___459.ciunit = itcom1_1.nout;
    s_wsfe(&io___459);
    e_wsfe();
/*<   320 FORMAT (//) >*/

/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* iterm_ */

/*<       SUBROUTINE IVFILL (N,IV,IVAL) >*/
/* Subroutine */ int ivfill_(integer *n, integer *iv, integer *ival)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    integer i__, m, mp1;


/*     FILLS AN INTEGER VECTOR, IV, WITH AN INTEGER VALUE, IVAL. */

/* ... PARAMETER LIST: */

/*          N      INTEGER LENGTH OF VECTOR IV */
/*          IV     INTEGER VECTOR */
/*          IVAL   INTEGER CONSTANT THAT FILLS FIRST N LOCATIONS OF IV */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER N,IVAL,IV(N) >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER I,M,MP1 >*/

/*<       IF (N.LE.0) RETURN >*/
    /* Parameter adjustments */
    --iv;

    /* Function Body */
    if (*n <= 0) {
	return 0;
    }

/*     CLEAN UP LOOP SO REMAINING VECTOR LENGTH IS A MULTIPLE OF 10 */

/*<       M = MOD(N,10) >*/
    m = *n % 10;
/*<       IF (M.EQ.0) GO TO 20 >*/
    if (m == 0) {
	goto L20;
    }
/*<       DO 10 I = 1,M >*/
    i__1 = m;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<          IV(I) = IVAL >*/
	iv[i__] = *ival;
/*<    10 CONTINUE >*/
/* L10: */
    }
/*<       IF (N.LT.10) RETURN >*/
    if (*n < 10) {
	return 0;
    }

/*<    20 MP1 = M+1 >*/
L20:
    mp1 = m + 1;
/*<       DO 30 I = MP1,N,10 >*/
    i__1 = *n;
    for (i__ = mp1; i__ <= i__1; i__ += 10) {
/*<          IV(I) = IVAL >*/
	iv[i__] = *ival;
/*<          IV(I+1) = IVAL >*/
	iv[i__ + 1] = *ival;
/*<          IV(I+2) = IVAL >*/
	iv[i__ + 2] = *ival;
/*<          IV(I+3) = IVAL >*/
	iv[i__ + 3] = *ival;
/*<          IV(I+4) = IVAL >*/
	iv[i__ + 4] = *ival;
/*<          IV(I+5) = IVAL >*/
	iv[i__ + 5] = *ival;
/*<          IV(I+6) = IVAL >*/
	iv[i__ + 6] = *ival;
/*<          IV(I+7) = IVAL >*/
	iv[i__ + 7] = *ival;
/*<          IV(I+8) = IVAL >*/
	iv[i__ + 8] = *ival;
/*<          IV(I+9) = IVAL >*/
	iv[i__ + 9] = *ival;
/*<    30 CONTINUE >*/
/* L30: */
    }

/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* ivfill_ */

/*<       SUBROUTINE OMEG (DNRM,IFLAG) >*/
/* Subroutine */ int omeg_(doublereal *dnrm, integer *iflag)
{
    /* Format strings */
    static char fmt_20[] = "(/30x,\002PARAMETERS WERE CHANGED AT ITERATION N\
O.\002,i5/35x,\002NEW ESTIMATE OF BETAB            =\002,d15.7/35x,\002SOLUT\
ION TO CHEBYSHEV EQN.       =\002,d15.7/35x,\002SOLUTION TO RAYLEIGH QUOTIEN\
T    =\002,d15.7/35x,\002NEW ESTIMATE FOR CME             =\002,d15.7/35x\
,\002NEW ESTIMATE FOR OMEGA           =\002,d15.7/35x,\002NEW ESTIMATE FOR S\
PECTRAL RADIUS =\002,d15.7/)";

    /* System generated locals */
    doublereal d__1;

    /* Builtin functions */
    double sqrt(doublereal);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    doublereal zm1, zm2, temp;

    /* Fortran I/O blocks */
    static cilist io___466 = { 0, 0, 0, fmt_20, 0 };
    static cilist io___467 = { 0, 0, 0, fmt_20, 0 };



/*     COMPUTES NEW VALUES FOR  CME, OMEGA, AND SPECR FOR */
/*     FULLY ADAPTIVE SSOR METHODS. */

/* ... PARAMETER LIST: */

/*          DNRM   NUMERATOR OF RAYLEIGH QUOTIENT */
/*          IFLAG  INDICATOR OF APPROPRIATE ENTRY POINT */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER IFLAG >*/
/*<       DOUBLE PRECISION DNRM >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       DOUBLE PRECISION TEMP,ZM1,ZM2 >*/

/* *** BEGIN: ITPACK COMMON */

/*<       INTEGER IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/
/*<       COMMON /ITCOM1/ IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/

/*<       LOGICAL ADAPT,BETADT,CASEII,HALT,PARTAD >*/
/*<       COMMON /ITCOM2/ ADAPT,BETADT,CASEII,HALT,PARTAD >*/

/*<    >*/
/*<    >*/

/* *** END  : ITPACK COMMON */

/*     DESCRIPTION OF VARIABLES IN COMMON BLOCKS IN MAIN SUBROUTINE */

/*<       ZM1 = 0.D0 >*/
    zm1 = 0.;
/*<       ZM2 = 0.D0 >*/
    zm2 = 0.;
/*<       IF (IFLAG.EQ.1) GO TO 10 >*/
    if (*iflag == 1) {
	goto L10;
    }

/* ... IFLAG .NE. 1, COMPUTE NEW ESTIMATE FOR CME */

/*<    >*/
/* Computing 2nd power */
    d__1 = itcom3_1.omega;
    zm1 = ((1. - itcom3_1.spr) * (itcom3_1.betab * (d__1 * d__1) + 1.) - 
	    itcom3_1.omega * (2. - itcom3_1.omega)) / (itcom3_1.omega * (
	    itcom3_1.omega - 1. - itcom3_1.spr));

/*<       IF (.NOT.CASEII) ZM2 = DNRM/BDELNM >*/
    if (! itcom2_1.caseii) {
	zm2 = *dnrm / itcom3_1.bdelnm;
    }
/*<       IF (CASEII) ZM2 = DSQRT(DABS(DNRM/BDELNM)) >*/
    if (itcom2_1.caseii) {
	zm2 = sqrt((d__1 = *dnrm / itcom3_1.bdelnm, abs(d__1)));
    }
/*<       CME = DMAX1(CME,ZM1,ZM2) >*/
/* Computing MAX */
    d__1 = max(itcom3_1.cme,zm1);
    itcom3_1.cme = max(d__1,zm2);

/* ... IFLAG = 1, OR CONTINUATION OF IFLAG .NE. 1 */

/*        COMPUTE NEW VALUES OF OMEGA AND SPECR BASED ON CME AND BETAB */

/*<    10 IS = IN+1 >*/
L10:
    itcom1_1.is = itcom1_1.in + 1;
/*<       DELSNM = DELNNM >*/
    itcom3_1.delsnm = itcom3_1.delnnm;
/*<       IF (CME.GE.(4.D0*BETAB)) GO TO 30 >*/
    if (itcom3_1.cme >= itcom3_1.betab * 4.) {
	goto L30;
    }

/* ... CME .LT. 4.D0*BETAB */

/*<       TEMP = DSQRT(DABS(1.D0-2.D0*CME+4.D0*BETAB)) >*/
    temp = sqrt((d__1 = 1. - itcom3_1.cme * 2. + itcom3_1.betab * 4., abs(
	    d__1)));
/*<       OMEGA = DMAX1((2.D0/(1.D0+TEMP)),1.D0) >*/
/* Computing MAX */
    d__1 = 2. / (temp + 1.);
    itcom3_1.omega = max(d__1,1.);
/*<       TEMP = (1.D0-CME)/TEMP >*/
    temp = (1. - itcom3_1.cme) / temp;
/*<       SPECR = (1.D0-TEMP)/(1.D0+TEMP) >*/
    itcom3_1.specr = (1. - temp) / (temp + 1.);
/*<       IF (DABS(OMEGA-1.D0).LT.DRELPR) SPECR = 0.D0 >*/
    if ((d__1 = itcom3_1.omega - 1., abs(d__1)) < itcom3_1.drelpr) {
	itcom3_1.specr = 0.;
    }
/*<       IF (LEVEL.GE.2) WRITE (NOUT,20) IN,BETAB,ZM1,ZM2,CME,OMEGA,SPECR >*/
    if (itcom1_1.level >= 2) {
	io___466.ciunit = itcom1_1.nout;
	s_wsfe(&io___466);
	do_fio(&c__1, (char *)&itcom1_1.in, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&itcom3_1.betab, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&zm1, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&zm2, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&itcom3_1.cme, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&itcom3_1.omega, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&itcom3_1.specr, (ftnlen)sizeof(doublereal));
	e_wsfe();
    }
/*<    >*/

/*<       RETURN >*/
    return 0;

/* ... CME .GE. 4.D0*BETAB */

/* ... OMEGA-STAR WILL BE CHOSEN */

/*<    30 CME = 2.D0*DSQRT(DABS(BETAB)) >*/
L30:
    itcom3_1.cme = sqrt((abs(itcom3_1.betab))) * 2.;
/*<       OMEGA = 2.D0/(1.D0+DSQRT(DABS(1.D0-4.D0*BETAB))) >*/
    itcom3_1.omega = 2. / (sqrt((d__1 = 1. - itcom3_1.betab * 4., abs(d__1))) 
	    + 1.);
/*<       SPECR = OMEGA-1.D0 >*/
    itcom3_1.specr = itcom3_1.omega - 1.;
/*<       ADAPT = .FALSE. >*/
    itcom2_1.adapt = FALSE_;
/*<       PARTAD = .FALSE. >*/
    itcom2_1.partad = FALSE_;
/*<       IF (LEVEL.GE.2) WRITE (NOUT,20) IN,BETAB,ZM1,ZM2,CME,OMEGA,SPECR >*/
    if (itcom1_1.level >= 2) {
	io___467.ciunit = itcom1_1.nout;
	s_wsfe(&io___467);
	do_fio(&c__1, (char *)&itcom1_1.in, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&itcom3_1.betab, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&zm1, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&zm2, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&itcom3_1.cme, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&itcom3_1.omega, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&itcom3_1.specr, (ftnlen)sizeof(doublereal));
	e_wsfe();
    }

/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* omeg_ */

/*<       LOGICAL FUNCTION OMGCHG (NDUMMY) >*/
logical omgchg_(integer *ndummy)
{
    /* System generated locals */
    doublereal d__1, d__2, d__3, d__4, d__5, d__6;
    logical ret_val;

    /* Builtin functions */
    double sqrt(doublereal), log(doublereal);

    /* Local variables */
    doublereal del1, del2;


/* ... THIS FUNCTION TESTS TO SEE WHETHER OMEGA SHOULD BE CHANGED */
/* ... FOR SSOR CG METHOD. */

/* ... PARAMETER LIST: */

/*          NDUMMY ARBITRARY INTEGER PARAMETER */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER NDUMMY >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       DOUBLE PRECISION DEL1,DEL2,X >*/

/*<       DOUBLE PRECISION PHI >*/

/* *** BEGIN: ITPACK COMMON */

/*<       INTEGER IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/
/*<       COMMON /ITCOM1/ IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/

/*<       LOGICAL ADAPT,BETADT,CASEII,HALT,PARTAD >*/
/*<       COMMON /ITCOM2/ ADAPT,BETADT,CASEII,HALT,PARTAD >*/

/*<    >*/
/*<    >*/

/* *** END  : ITPACK COMMON */

/*     DESCRIPTION OF VARIABLES IN COMMON BLOCKS IN MAIN SUBROUTINE */

/* ... STATEMENT FUNCTION PHI(X) */

/*<       PHI(X) = (1.D0-DSQRT(DABS(1.D0-X)))/(1.D0+DSQRT(DABS(1.D0-X))) >*/

/*<       OMGCHG = .FALSE. >*/
    ret_val = FALSE_;
/*<       IF (IN-IS.LT.3) RETURN >*/
    if (itcom1_1.in - itcom1_1.is < 3) {
	return ret_val;
    }
/*<       IF (SPECR.EQ.0.D0) GO TO 10 >*/
    if (itcom3_1.specr == 0.) {
	goto L10;
    }
/*<       IF (SPECR.GE.SPR) RETURN >*/
    if (itcom3_1.specr >= itcom3_1.spr) {
	return ret_val;
    }
/*<       DEL1 = -DLOG(DABS(PHI(SPECR)/PHI(SPECR/SPR))) >*/
    d__3 = itcom3_1.specr / itcom3_1.spr;
    del1 = -log((d__6 = (1. - sqrt((d__1 = 1. - itcom3_1.specr, abs(d__1)))) /
	     (1. + sqrt((d__2 = 1. - itcom3_1.specr, abs(d__2)))) / ((1. - 
	    sqrt((d__4 = 1. - d__3, abs(d__4)))) / (1. + sqrt((d__5 = 1. - 
	    d__3, abs(d__5))))), abs(d__6)));
/*<       DEL2 = -DLOG(DABS(PHI(SPR))) >*/
    del2 = -log((d__3 = (1. - sqrt((d__1 = 1. - itcom3_1.spr, abs(d__1)))) / (
	    1. + sqrt((d__2 = 1. - itcom3_1.spr, abs(d__2)))), abs(d__3)));
/*<       IF ((DEL1/DEL2).GE.FF) RETURN >*/
    if (del1 / del2 >= itcom3_1.ff) {
	return ret_val;
    }

/*<    10 OMGCHG = .TRUE. >*/
L10:
    ret_val = TRUE_;

/*<       RETURN >*/
    return ret_val;
/*<       END >*/
} /* omgchg_ */

/*<       LOGICAL FUNCTION OMGSTR (NDUMMY) >*/
logical omgstr_(integer *ndummy)
{
    /* Format strings */
    static char fmt_20[] = "(/30x,\002OMEGA-STAR, AN ALTERNATE ESTIMATE O\
F\002,\002 OMEGA, WAS CHOSEN AT ITERATION\002,i5/35x,\002NEW ESTIMATE FOR CM\
E             =\002,d15.7/35x,\002NEW ESTIMATE FOR OMEGA           =\002,d15\
.7/35x,\002NEW ESTIMATE FOR SPECTRAL RADIUS =\002,d15.7/)";

    /* System generated locals */
    doublereal d__1, d__2, d__3, d__4;
    logical ret_val;

    /* Builtin functions */
    double sqrt(doublereal), log(doublereal);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    doublereal temp, temp1, omstar;

    /* Fortran I/O blocks */
    static cilist io___473 = { 0, 0, 0, fmt_20, 0 };



/*     TESTS FOR FULLY ADAPTIVE SSOR METHODS WHETHER OMEGA-STAR */
/*     SHOULD BE USED FOR OMEGA AND THE ADAPTIVE PROCESS TURNED */
/*     OFF. */

/* ... PARAMETER LIST: */

/*          NDUMMY ARBITRARY INTEGER PARAMETER */

/* ... SPECIFICATION FOR ARGUMENT */

/*<       INTEGER NDUMMY >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       DOUBLE PRECISION OMSTAR,TEMP,TEMP1,X >*/

/*<       DOUBLE PRECISION PHI >*/

/* *** BEGIN: ITPACK COMMON */

/*<       INTEGER IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/
/*<       COMMON /ITCOM1/ IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/

/*<       LOGICAL ADAPT,BETADT,CASEII,HALT,PARTAD >*/
/*<       COMMON /ITCOM2/ ADAPT,BETADT,CASEII,HALT,PARTAD >*/

/*<    >*/
/*<    >*/

/* *** END  : ITPACK COMMON */

/*     DESCRIPTION OF VARIABLES IN COMMON BLOCKS IN MAIN SUBROUTINE */

/* ... STATEMENT FUNCTION PHI(X) */

/*<       PHI(X) = (1.D0-DSQRT(DABS(1.D0-X)))/(1.D0+DSQRT(DABS(1.D0-X))) >*/

/*<       OMGSTR = .FALSE. >*/
    ret_val = FALSE_;
/*<       IF (BETAB.GE..25D0.OR..NOT.ADAPT) RETURN >*/
    if (itcom3_1.betab >= .25 || ! itcom2_1.adapt) {
	return ret_val;
    }
/*<       OMSTAR = 2.D0/(1.D0+DSQRT(DABS(1.D0-4.D0*BETAB))) >*/
    omstar = 2. / (sqrt((d__1 = 1. - itcom3_1.betab * 4., abs(d__1))) + 1.);

/* ... TEST TO CHOSE OMEGA-STAR */

/*<       IF ((OMSTAR.LE.1.D0).OR.(SPECR.LE.0.D0)) GO TO 10 >*/
    if (omstar <= 1. || itcom3_1.specr <= 0.) {
	goto L10;
    }
/*<       TEMP = DLOG(DABS(PHI(OMSTAR-1.D0))) >*/
    d__1 = omstar - 1.;
    temp = log((d__4 = (1. - sqrt((d__2 = 1. - d__1, abs(d__2)))) / (1. + 
	    sqrt((d__3 = 1. - d__1, abs(d__3)))), abs(d__4)));
/*<       TEMP1 = DLOG(DABS(PHI(SPECR))) >*/
    temp1 = log((d__3 = (1. - sqrt((d__1 = 1. - itcom3_1.specr, abs(d__1)))) /
	     (1. + sqrt((d__2 = 1. - itcom3_1.specr, abs(d__2)))), abs(d__3)))
	    ;
/*<       IF ((TEMP/TEMP1).LT.FF) RETURN >*/
    if (temp / temp1 < itcom3_1.ff) {
	return ret_val;
    }

/* ... OMEGA-STAR WAS CHOSEN */

/*<    10 OMEGA = OMSTAR >*/
L10:
    itcom3_1.omega = omstar;
/*<       SPECR = OMEGA-1.D0 >*/
    itcom3_1.specr = itcom3_1.omega - 1.;
/*<       OMGSTR = .TRUE. >*/
    ret_val = TRUE_;
/*<       ADAPT = .FALSE. >*/
    itcom2_1.adapt = FALSE_;
/*<       PARTAD = .FALSE. >*/
    itcom2_1.partad = FALSE_;
/*<       CME = 2.D0*DSQRT(DABS(BETAB)) >*/
    itcom3_1.cme = sqrt((abs(itcom3_1.betab))) * 2.;
/*<       RRR = PHI(1.D0-SPECR)**2 >*/
    d__1 = 1. - itcom3_1.specr;
/* Computing 2nd power */
    d__4 = (1. - sqrt((d__2 = 1. - d__1, abs(d__2)))) / (1. + sqrt((d__3 = 1. 
	    - d__1, abs(d__3))));
    itcom3_1.rrr = d__4 * d__4;
/*<       GAMMA = 2.D0/(2.D0-SPECR) >*/
    itcom3_1.gamma = 2. / (2. - itcom3_1.specr);
/*<       SIGE = SPECR/(2.D0-SPECR) >*/
    itcom3_1.sige = itcom3_1.specr / (2. - itcom3_1.specr);
/*<       RHO = 1.D0 >*/
    itcom3_1.rho = 1.;
/*<       IS = IN+1 >*/
    itcom1_1.is = itcom1_1.in + 1;
/*<       DELSNM = DELNNM >*/
    itcom3_1.delsnm = itcom3_1.delnnm;
/*<       IF (LEVEL.GE.2) WRITE (NOUT,20) IN,CME,OMEGA,SPECR >*/
    if (itcom1_1.level >= 2) {
	io___473.ciunit = itcom1_1.nout;
	s_wsfe(&io___473);
	do_fio(&c__1, (char *)&itcom1_1.in, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&itcom3_1.cme, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&itcom3_1.omega, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&itcom3_1.specr, (ftnlen)sizeof(doublereal));
	e_wsfe();
    }
/*<    >*/

/*<       RETURN >*/
    return ret_val;
/*<       END >*/
} /* omgstr_ */

/*<       SUBROUTINE PARCON (DTNRM,C1,C2,C3,C4,GAMOLD,RHOTMP,IBMTH) >*/
/* Subroutine */ int parcon_(doublereal *dtnrm, doublereal *c1, doublereal *
	c2, doublereal *c3, doublereal *c4, doublereal *gamold, doublereal *
	rhotmp, integer *ibmth)
{
    integer ip;
    doublereal rhoold;


/*     COMPUTES ACCELERATION PARAMETERS FOR CONJUGATE GRADIENT */
/*     ACCELERATED METHODS. */

/* ... PARAMETER LIST: */

/*          DTNRM  INNER PRODUCT OF RESIDUALS */
/*          C1     OUTPUT: RHO*GAMMA */
/*          C2     OUTPUT: RHO */
/*          C3     OUTPUT: 1-RHO */
/*          C4     OUTPUT: RHO*(1-GAMMA) */
/*          GAMOLD OUTPUT: VALUE OF GAMMA AT PRECEDING ITERATION */
/*          RHOTMP LAST ESTIMATE FOR VALUE OF RHO */
/*          IBMTH  INDICATOR OF BASIC METHOD BEING ACCELERATED BY CG */
/*                      IBMTH = 1,   JACOBI */
/*                            = 2,   REDUCED SYSTEM */
/*                            = 3,   SSOR */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER IBMTH >*/
/*<       DOUBLE PRECISION DTNRM,C1,C2,C3,C4,GAMOLD,RHOTMP >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER IP >*/
/*<       DOUBLE PRECISION RHOOLD >*/

/* *** BEGIN: ITPACK COMMON */

/*<       INTEGER IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/
/*<       COMMON /ITCOM1/ IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/

/*<       LOGICAL ADAPT,BETADT,CASEII,HALT,PARTAD >*/
/*<       COMMON /ITCOM2/ ADAPT,BETADT,CASEII,HALT,PARTAD >*/

/*<    >*/
/*<    >*/

/* *** END  : ITPACK COMMON */

/*     DESCRIPTION OF VARIABLES IN COMMON BLOCKS IN MAIN SUBROUTINE */

/*<       IP = IN-IS >*/
    ip = itcom1_1.in - itcom1_1.is;

/* ... SET RHOOLD AND GAMOLD */

/*<       RHOOLD = RHO >*/
    rhoold = itcom3_1.rho;
/*<       GAMOLD = GAMMA >*/
    *gamold = itcom3_1.gamma;

/* ... COMPUTE GAMMA (IN+1) */

/* ... FOR JACOBI OR REDUCED SYSTEM CG */

/*<       IF (IBMTH.LE.2) GAMMA = 1.D0/(1.D0-DTNRM/DELNNM) >*/
    if (*ibmth <= 2) {
	itcom3_1.gamma = 1. / (1. - *dtnrm / itcom3_1.delnnm);
    }

/* ... FOR SSOR CG */

/*<       IF (IBMTH.EQ.3) GAMMA = DELNNM/DTNRM >*/
    if (*ibmth == 3) {
	itcom3_1.gamma = itcom3_1.delnnm / *dtnrm;
    }

/* ... COMPUTE RHO (IN+1) */

/*<       RHO = 1.D0 >*/
    itcom3_1.rho = 1.;
/*<       IF (IP.EQ.0) GO TO 20 >*/
    if (ip == 0) {
	goto L20;
    }
/*<       IF (ISYM.EQ.0) GO TO 10 >*/
    if (itcom1_1.isym == 0) {
	goto L10;
    }
/*<       RHO = 1.D0/(1.D0-GAMMA*RHOTMP/DELSNM) >*/
    itcom3_1.rho = 1. / (1. - itcom3_1.gamma * *rhotmp / itcom3_1.delsnm);
/*<       GO TO 20 >*/
    goto L20;
/*<    10 RHO = 1.D0/(1.D0-GAMMA*DELNNM/(GAMOLD*DELSNM*RHOOLD)) >*/
L10:
    itcom3_1.rho = 1. / (1. - itcom3_1.gamma * itcom3_1.delnnm / (*gamold * 
	    itcom3_1.delsnm * rhoold));

/* ... COMPUTE CONSTANTS C1, C2, C3, AND C4 */

/*<    20 DELSNM = DELNNM >*/
L20:
    itcom3_1.delsnm = itcom3_1.delnnm;
/*<       RHOTMP = RHOOLD >*/
    *rhotmp = rhoold;
/*<       C1 = RHO*GAMMA >*/
    *c1 = itcom3_1.rho * itcom3_1.gamma;
/*<       C2 = RHO >*/
    *c2 = itcom3_1.rho;
/*<       C3 = 1.D0-RHO >*/
    *c3 = 1. - itcom3_1.rho;
/*<       C4 = RHO*(1.D0-GAMMA) >*/
    *c4 = itcom3_1.rho * (1. - itcom3_1.gamma);

/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* parcon_ */

/*<       SUBROUTINE PARSI (C1,C2,C3,IBMTH) >*/
/* Subroutine */ int parsi_(doublereal *c1, doublereal *c2, doublereal *c3, 
	integer *ibmth)
{
    /* System generated locals */
    doublereal d__1, d__2;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    integer ip;


/*     COMPUTES ACCELERATION PARAMETERS FOR SEMI-ITERATIVE */
/*     ACCELERATED METHODS. */

/* ... PARAMETER LIST: */

/*          C1,C2 */
/*           AND */
/*           C3    OUTPUT ACCELERATION PARAMETERS */
/*          IBMTH  INDICATOR OF BASIC METHOD BEING ACCELERATED BY SI */
/*                      IBMTH = 1, JACOBI */
/*                            = 2, REDUCED SYSTEM */
/*                            = 3, SSOR */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER IBMTH >*/
/*<       DOUBLE PRECISION C1,C2,C3 >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER IP >*/

/* *** BEGIN: ITPACK COMMON */

/*<       INTEGER IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/
/*<       COMMON /ITCOM1/ IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/

/*<       LOGICAL ADAPT,BETADT,CASEII,HALT,PARTAD >*/
/*<       COMMON /ITCOM2/ ADAPT,BETADT,CASEII,HALT,PARTAD >*/

/*<    >*/
/*<    >*/

/* *** END  : ITPACK COMMON */

/*     DESCRIPTION OF VARIABLES IN COMMON BLOCKS IN MAIN SUBROUTINE */

/*<       IP = IN-IS >*/
    ip = itcom1_1.in - itcom1_1.is;
/*<       IF (IP.EQ.0) GO TO 30 >*/
    if (ip == 0) {
	goto L30;
    }
/*<       IF (IP.EQ.1) GO TO 10 >*/
    if (ip == 1) {
	goto L10;
    }
/*<       RHO = 1.D0/(1.D0-SIGE*SIGE*RHO*.25D0) >*/
    itcom3_1.rho = 1. / (1. - itcom3_1.sige * itcom3_1.sige * itcom3_1.rho * 
	    .25);
/*<       GO TO 20 >*/
    goto L20;
/*<    10 RHO = 1.D0/(1.D0-SIGE*SIGE*.5D0) >*/
L10:
    itcom3_1.rho = 1. / (1. - itcom3_1.sige * itcom3_1.sige * .5);

/*<    20 C1 = RHO*GAMMA >*/
L20:
    *c1 = itcom3_1.rho * itcom3_1.gamma;
/*<       C2 = RHO >*/
    *c2 = itcom3_1.rho;
/*<       C3 = 1.D0-RHO >*/
    *c3 = 1. - itcom3_1.rho;

/*<       RETURN >*/
    return 0;

/* ... NONADAPTIVE INITIALIZATION FOR SEMI-ITERATIVE METHODS */

/*<    30 CONTINUE >*/
L30:
/*<       GO TO (40,50,60), IBMTH >*/
    switch (*ibmth) {
	case 1:  goto L40;
	case 2:  goto L50;
	case 3:  goto L60;
    }

/* ... JSI */

/*<    40 IF (CASEII) SME = -CME >*/
L40:
    if (itcom2_1.caseii) {
	itcom3_1.sme = -itcom3_1.cme;
    }
/*<       GAMMA = 2.D0/(2.D0-CME-SME) >*/
    itcom3_1.gamma = 2. / (2. - itcom3_1.cme - itcom3_1.sme);
/*<       SIGE = (CME-SME)/(2.D0-CME-SME) >*/
    itcom3_1.sige = (itcom3_1.cme - itcom3_1.sme) / (2. - itcom3_1.cme - 
	    itcom3_1.sme);
/*<       GO TO 70 >*/
    goto L70;

/* ... REDUCED SYSTEM SI */

/*<    50 GAMMA = 2.D0/(2.D0-CME*CME) >*/
L50:
    itcom3_1.gamma = 2. / (2. - itcom3_1.cme * itcom3_1.cme);
/*<       SIGE = CME*CME/(2.D0-CME*CME) >*/
    itcom3_1.sige = itcom3_1.cme * itcom3_1.cme / (2. - itcom3_1.cme * 
	    itcom3_1.cme);
/*<    >*/
    itcom3_1.rrr = (1. - sqrt((d__1 = 1. - itcom3_1.cme * itcom3_1.cme, abs(
	    d__1)))) / (sqrt((d__2 = 1. - itcom3_1.cme * itcom3_1.cme, abs(
	    d__2))) + 1.);
/*<       GO TO 70 >*/
    goto L70;

/* ... SSORSI */

/*<    60 GAMMA = 2.D0/(2.D0-SPECR) >*/
L60:
    itcom3_1.gamma = 2. / (2. - itcom3_1.specr);
/*<       SIGE = SPECR/(2.D0-SPECR) >*/
    itcom3_1.sige = itcom3_1.specr / (2. - itcom3_1.specr);
/*<    >*/
    itcom3_1.rrr = (1. - sqrt((d__1 = 1. - itcom3_1.sige * itcom3_1.sige, abs(
	    d__1)))) / (sqrt((d__2 = 1. - itcom3_1.sige * itcom3_1.sige, abs(
	    d__2))) + 1.);

/*<    70 RHO = 1.D0 >*/
L70:
    itcom3_1.rho = 1.;
/*<       C1 = GAMMA >*/
    *c1 = itcom3_1.gamma;
/*<       C2 = 1.D0 >*/
    *c2 = 1.;
/*<       C3 = 0.D0 >*/
    *c3 = 0.;

/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* parsi_ */

/*<       DOUBLE PRECISION FUNCTION PBETA (NN,IA,JA,A,V,W1,W2) >*/
doublereal pbeta_(integer *nn, integer *ia, integer *ja, doublereal *a, 
	doublereal *v, doublereal *w1, doublereal *w2)
{
    /* System generated locals */
    integer i__1, i__2;
    doublereal ret_val;

    /* Local variables */
    integer i__, k, n, ii, jj, nm1, jai;
    doublereal sum;
    integer jajj, ibgn, iend, itmp;
    doublereal temp1, temp2;


/*     ... COMPUTES THE NUMERATOR FOR THE COMPUTATION OF BETAB IN */
/*     ...  SSOR METHODS. */

/* ... PARAMETER LIST: */

/*          N      DIMENSION OF MATRIX (= NN) */
/*          IA,JA  INTEGER ARRAYS OF SPARSE MATRIX REPRESENTATION */
/*          A      D.P. ARRAY OF SPARSE MATRIX REPRESENTATION */
/*          W1,W2  WORKSPACE VECTORS OF LENGTH N */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER IA(1),JA(1),NN >*/
/*<       DOUBLE PRECISION A(1),V(NN),W1(NN),W2(NN) >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER I,IBGN,IEND,II,ITMP,JAI,JAJJ,JJ,K,N,NM1 >*/
/*<       DOUBLE PRECISION SUM,TEMP1,TEMP2 >*/

/* *** BEGIN: ITPACK COMMON */

/*<       INTEGER IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/
/*<       COMMON /ITCOM1/ IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/

/*<       LOGICAL ADAPT,BETADT,CASEII,HALT,PARTAD >*/
/*<       COMMON /ITCOM2/ ADAPT,BETADT,CASEII,HALT,PARTAD >*/

/*<    >*/
/*<    >*/

/* *** END  : ITPACK COMMON */

/*     DESCRIPTION OF VARIABLES IN COMMON BLOCK IN MAIN SUBROUTINE */

/*<       N = NN >*/
    /* Parameter adjustments */
    --w2;
    --w1;
    --v;
    --ia;
    --ja;
    --a;

    /* Function Body */
    n = *nn;
/*<       PBETA = 0.D0 >*/
    ret_val = 0.;
/*<       IF (ISYM.EQ.0) GO TO 110 >*/
    if (itcom1_1.isym == 0) {
	goto L110;
    }

/*     ************** NON - SYMMETRIC SECTION ******************** */

/*<       DO 10 I = 1,N >*/
    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<          W1(I) = V(I) >*/
	w1[i__] = v[i__];
/*<    10 CONTINUE >*/
/* L10: */
    }
/*<       TEMP1 = 0.D0 >*/
    temp1 = 0.;
/*<       TEMP2 = 0.D0 >*/
    temp2 = 0.;
/*<       ITMP = 2 >*/
    itmp = 2;
/*<       IBGN = IA(1) >*/
    ibgn = ia[1];
/*<       IEND = IA(ITMP)-1 >*/
    iend = ia[itmp] - 1;
/*<       IF (IEND.LT.IBGN) GO TO 30 >*/
    if (iend < ibgn) {
	goto L30;
    }
/*<       DO 20 I = IBGN,IEND >*/
    i__1 = iend;
    for (i__ = ibgn; i__ <= i__1; ++i__) {
/*<          JAI = JA(I) >*/
	jai = ja[i__];
/*<          TEMP1 = TEMP1-A(I)*W1(JAI) >*/
	temp1 -= a[i__] * w1[jai];
/*<    20 CONTINUE >*/
/* L20: */
    }
/*<    30 W1(1) = TEMP1 >*/
L30:
    w1[1] = temp1;
/*<       W2(1) = 0.D0 >*/
    w2[1] = 0.;
/*<       NM1 = N-1 >*/
    nm1 = n - 1;
/*<       DO 70 K = 2,NM1 >*/
    i__1 = nm1;
    for (k = 2; k <= i__1; ++k) {
/*<          TEMP1 = 0.D0 >*/
	temp1 = 0.;
/*<          TEMP2 = 0.D0 >*/
	temp2 = 0.;
/*<          IBGN = IA(K) >*/
	ibgn = ia[k];
/*<          IEND = IA(K+1)-1 >*/
	iend = ia[k + 1] - 1;
/*<          IF (IEND.LT.IBGN) GO TO 60 >*/
	if (iend < ibgn) {
	    goto L60;
	}
/*<          DO 50 I = IBGN,IEND >*/
	i__2 = iend;
	for (i__ = ibgn; i__ <= i__2; ++i__) {
/*<             JAI = JA(I) >*/
	    jai = ja[i__];
/*<             IF (JAI.GT.K) GO TO 40 >*/
	    if (jai > k) {
		goto L40;
	    }
/*<             TEMP2 = TEMP2-A(I)*W1(JAI) >*/
	    temp2 -= a[i__] * w1[jai];
/*<             GO TO 50 >*/
	    goto L50;
/*<    40       TEMP1 = TEMP1-A(I)*W1(JAI) >*/
L40:
	    temp1 -= a[i__] * w1[jai];
/*<    50    CONTINUE >*/
L50:
	    ;
	}
/*<    60    W1(K) = TEMP1 >*/
L60:
	w1[k] = temp1;
/*<          W2(K) = TEMP2 >*/
	w2[k] = temp2;
/*<    70 CONTINUE >*/
/* L70: */
    }
/*<       TEMP2 = 0.D0 >*/
    temp2 = 0.;
/*<       IBGN = IA(N) >*/
    ibgn = ia[n];
/*<       IEND = IA(N+1)-1 >*/
    iend = ia[n + 1] - 1;
/*<       IF (IEND.LT.IBGN) GO TO 90 >*/
    if (iend < ibgn) {
	goto L90;
    }
/*<       DO 80 I = IBGN,IEND >*/
    i__1 = iend;
    for (i__ = ibgn; i__ <= i__1; ++i__) {
/*<          JAI = JA(I) >*/
	jai = ja[i__];
/*<          TEMP2 = TEMP2-A(I)*W1(JAI) >*/
	temp2 -= a[i__] * w1[jai];
/*<    80 CONTINUE >*/
/* L80: */
    }
/*<    90 W2(N) = TEMP2 >*/
L90:
    w2[n] = temp2;
/*<       DO 100 I = 1,N >*/
    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<          PBETA = PBETA+V(I)*W2(I) >*/
	ret_val += v[i__] * w2[i__];
/*<   100 CONTINUE >*/
/* L100: */
    }
/*<       RETURN >*/
    return ret_val;

/*     **************** SYMMETRIC SECTION ************************* */

/*<   110 DO 130 II = 1,N >*/
L110:
    i__1 = n;
    for (ii = 1; ii <= i__1; ++ii) {
/*<          IBGN = IA(II) >*/
	ibgn = ia[ii];
/*<          IEND = IA(II+1)-1 >*/
	iend = ia[ii + 1] - 1;
/*<          SUM = 0.D0 >*/
	sum = 0.;
/*<          IF (IBGN.GT.IEND) GO TO 130 >*/
	if (ibgn > iend) {
	    goto L130;
	}
/*<          DO 120 JJ = IBGN,IEND >*/
	i__2 = iend;
	for (jj = ibgn; jj <= i__2; ++jj) {
/*<             JAJJ = JA(JJ) >*/
	    jajj = ja[jj];
/*<             SUM = SUM-A(JJ)*V(JAJJ) >*/
	    sum -= a[jj] * v[jajj];
/*<   120    CONTINUE >*/
/* L120: */
	}
/*<          PBETA = PBETA+SUM*SUM >*/
	ret_val += sum * sum;
/*<   130 CONTINUE >*/
L130:
	;
    }
/*<       RETURN >*/
    return ret_val;

/*<       END >*/
} /* pbeta_ */

/*<       SUBROUTINE PBSOR (NN,IA,JA,A,U,RHS) >*/
/* Subroutine */ int pbsor_(integer *nn, integer *ia, integer *ja, doublereal 
	*a, doublereal *u, doublereal *rhs)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Local variables */
    integer i__, n, ii, jj;
    doublereal ui, sum, omm1;
    integer npl1, jajj, ibgn, iend;


/*     ... THIS SUBROUTINE COMPUTES A BACKWARD SOR SWEEP. */

/* ... PARAMETER LIST: */

/*          N      ORDER OF SYSTEM (= NN) */
/*          OMEGA  RELAXATION FACTOR */
/*          IA,JA  INTEGER ARRAYS OF SPARSE MATRIX REPRESENTATION */
/*          A      D.P. ARRAY OF SPARSE MATRIX REPRESENTATION */
/*          U      LATEST ESTIMATE OF SOLUTION */
/*          RHS    RIGHT HAND SIDE OF MATRIX PROBLEM */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER IA(1),JA(1),NN >*/
/*<       DOUBLE PRECISION A(1),U(NN),RHS(NN) >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER I,IBGN,IEND,II,JAJJ,JJ,N,NPL1 >*/
/*<       DOUBLE PRECISION OMM1,SUM,UI >*/

/* *** BEGIN: ITPACK COMMON */

/*<       INTEGER IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/
/*<       COMMON /ITCOM1/ IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/

/*<       LOGICAL ADAPT,BETADT,CASEII,HALT,PARTAD >*/
/*<       COMMON /ITCOM2/ ADAPT,BETADT,CASEII,HALT,PARTAD >*/

/*<    >*/
/*<    >*/

/* *** END  : ITPACK COMMON */

/*     DESCRIPTION OF VARIABLES IN COMMON BLOCK IN MAIN SUBROUTINE */

/*<       N = NN >*/
    /* Parameter adjustments */
    --rhs;
    --u;
    --ia;
    --ja;
    --a;

    /* Function Body */
    n = *nn;
/*<       NPL1 = N+1 >*/
    npl1 = n + 1;
/*<       OMM1 = OMEGA-1.D0 >*/
    omm1 = itcom3_1.omega - 1.;
/*<       IF (ISYM.EQ.0) GO TO 40 >*/
    if (itcom1_1.isym == 0) {
	goto L40;
    }

/*     *************** NON - SYMMETRIC SECTION ********************** */

/*<       DO 30 I = 1,N >*/
    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<          II = NPL1-I >*/
	ii = npl1 - i__;
/*<          IBGN = IA(II) >*/
	ibgn = ia[ii];
/*<          IEND = IA(II+1)-1 >*/
	iend = ia[ii + 1] - 1;
/*<          SUM = RHS(II) >*/
	sum = rhs[ii];
/*<          IF (IBGN.GT.IEND) GO TO 20 >*/
	if (ibgn > iend) {
	    goto L20;
	}
/*<          DO 10 JJ = IBGN,IEND >*/
	i__2 = iend;
	for (jj = ibgn; jj <= i__2; ++jj) {
/*<             JAJJ = JA(JJ) >*/
	    jajj = ja[jj];
/*<             SUM = SUM-A(JJ)*U(JAJJ) >*/
	    sum -= a[jj] * u[jajj];
/*<    10    CONTINUE >*/
/* L10: */
	}
/*<    20    U(II) = OMEGA*SUM-OMM1*U(II) >*/
L20:
	u[ii] = itcom3_1.omega * sum - omm1 * u[ii];
/*<    30 CONTINUE >*/
/* L30: */
    }
/*<       RETURN >*/
    return 0;

/*     ***************** SYMMETRIC SECTION ************************** */

/*<    40 DO 60 II = 1,N >*/
L40:
    i__1 = n;
    for (ii = 1; ii <= i__1; ++ii) {
/*<          UI = U(II) >*/
	ui = u[ii];
/*<          IBGN = IA(II) >*/
	ibgn = ia[ii];
/*<          IEND = IA(II+1)-1 >*/
	iend = ia[ii + 1] - 1;
/*<          IF (IBGN.GT.IEND) GO TO 60 >*/
	if (ibgn > iend) {
	    goto L60;
	}
/*<          DO 50 JJ = IBGN,IEND >*/
	i__2 = iend;
	for (jj = ibgn; jj <= i__2; ++jj) {
/*<             JAJJ = JA(JJ) >*/
	    jajj = ja[jj];
/*<             RHS(JAJJ) = RHS(JAJJ)-A(JJ)*UI >*/
	    rhs[jajj] -= a[jj] * ui;
/*<    50    CONTINUE >*/
/* L50: */
	}
/*<    60 CONTINUE >*/
L60:
	;
    }

/*<       DO 90 I = 1,N >*/
    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<          II = NPL1-I >*/
	ii = npl1 - i__;
/*<          IBGN = IA(II) >*/
	ibgn = ia[ii];
/*<          IEND = IA(II+1)-1 >*/
	iend = ia[ii + 1] - 1;
/*<          SUM = RHS(II) >*/
	sum = rhs[ii];
/*<          IF (IBGN.GT.IEND) GO TO 80 >*/
	if (ibgn > iend) {
	    goto L80;
	}
/*<          DO 70 JJ = IBGN,IEND >*/
	i__2 = iend;
	for (jj = ibgn; jj <= i__2; ++jj) {
/*<             JAJJ = JA(JJ) >*/
	    jajj = ja[jj];
/*<             SUM = SUM-A(JJ)*U(JAJJ) >*/
	    sum -= a[jj] * u[jajj];
/*<    70    CONTINUE >*/
/* L70: */
	}
/*<    80    U(II) = OMEGA*SUM-OMM1*U(II) >*/
L80:
	u[ii] = itcom3_1.omega * sum - omm1 * u[ii];
/*<    90 CONTINUE >*/
/* L90: */
    }
/*<       RETURN >*/
    return 0;

/*<       END >*/
} /* pbsor_ */

/*<       SUBROUTINE PERMAT (NN,IA,JA,A,P,NEWIA,ISYM,LEVEL,NOUT,IERR) >*/
/* Subroutine */ int permat_(integer *nn, integer *ia, integer *ja, 
	doublereal *a, integer *p, integer *newia, integer *isym, integer *
	level, integer *nout, integer *ierr)
{
    /* Format strings */
    static char fmt_100[] = "(\0020\002,\002*** F A T A L     E R R O R ****\
********\002/\0020\002,\002    IN ITPACK ROUTINE PERMAT  \002/\002 \002,\002\
    NO ENTRY IN ROW \002,i10,\002 OF ORIGINAL MATRIX \002)";
    static char fmt_120[] = "(\0020\002,\002*** F A T A L     E R R O R ****\
********\002/\0020\002,\002    IN ITPACK ROUTINE PRBNDX  \002/\002 \002,\002\
    NO ENTRY IN ROW \002,i10,\002 OF PERMUTED MATRIX \002)";
    static char fmt_140[] = "(\0020\002,\002*** F A T A L     E R R O R ****\
********\002/\0020\002,\002    IN ITPACK ROUTINE QSORT   \002/\002 \002,\002\
    ERROR IN SORTING PERMUTED ROW \002,i12/\002 \002,\002    CALLED FROM ITP\
ACK ROUTINE PRBNDX   \002)";

    /* System generated locals */
    integer i__1, i__2;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    integer i__, j, k, n, ip, jp, jaj, ier, ipp, npl1, ibgn, iend;
    doublereal save;
    integer nels;
    doublereal temp;
    integer next;
    extern /* Subroutine */ int qsort_(integer *, integer *, doublereal *, 
	    integer *);
    extern integer bisrch_(integer *, integer *, integer *);

    /* Fortran I/O blocks */
    static cilist io___518 = { 0, 0, 0, fmt_100, 0 };
    static cilist io___519 = { 0, 0, 0, fmt_120, 0 };
    static cilist io___520 = { 0, 0, 0, fmt_140, 0 };



/* ********************************************************************* */

/* ... SUBROUTINE PERMAT TAKES THE SPARSE MATRIX REPRESENTATION */
/*     OF THE MATRIX STORED IN THE ARRAYS IA, JA, AND A AND */
/*     PERMUTES BOTH ROWS AND COLUMNS OVERWRITING THE PREVIOUS */
/*     STRUCTURE. */

/* ... PARAMETER LIST: */

/*         N      ORDER OF SYSTEM (= NN) */
/*         IA,JA  INTEGER ARRAYS OF THE SPARSE MATRIX REPRESENTATION */
/*         A      D.P. ARRAY OF THE SPARSE MATRIX REPRESENTATION */
/*         P      PERMUTATION VECTOR */
/*         NEWIA  INTEGER WORK VECTOR OF LENGTH N */
/*         ISYM   SYMMETRIC/NONSYMMETRIC STORAGE SWITCH */
/*         LEVEL  SWITCH CONTROLLING LEVEL OF OUTPUT */
/*         NOUT OUTPUT UNIT NUMBER */
/*         IER    OUTPUT ERROR FLAG (= IERR) */

/*                   IER =   0  NORMAL RETURN */
/*                   IER = 301  NO ENTRY IN ITH ROW OF ORIGINAL */
/*                              MATRIX. IF LEVEL IS GREATER THAN */
/*                              0, I WILL BE PRINTED */
/*                   IER = 302  THERE IS NO ENTRY IN THE ITH ROW */
/*                              OF THE PERMUTED MATRIX */
/*                   IER = 303  ERROR RETURN FROM QSORT IN */
/*                              SORTING THE ITH ROW OF THE */
/*                              PERMUTED MATRIX */
/* ... IT IS ASSUMED THAT THE I-TH ENTRY OF THE PERMUTATION VECTOR */
/*     P INDICATES THE ROW THE I-TH ROW GETS MAPPED INTO.  (I.E. */
/*     IF ( P(I) = J ) ROW I GETS MAPPED INTO ROW J.) */

/* ... THE ARRAY NEWIA IS AN INTEGER WORK VECTOR OF LENGTH N WHICH */
/*     KEEPS TRACK OF WHERE THE ROWS BEGIN IN THE PERMUTED STRUCTURE. */

/* ... PERMAT IS CAPABLE OF PERMUTING BOTH THE SYMMETRIC AND NON- */
/*     SYMMETRIC FORM OF IA, JA, AND A.  IF ( ISYM .EQ. 0 ) SYMMETRIC */
/*     FORM IS ASSUMED. */

/* ... TWO EXTERNAL MODULES ARE USED BY PERMAT.  THE FIRST IS INTEGER */
/*     FUNCTION BISRCH WHICH USES A BISECTION SEARCH ( ORDER LOG-BASE-2 */
/*     OF N+1 ) THROUGH THE ARRAY IA TO FIND THE ROW INDEX OF AN ARBI- */
/*     TRARY ENTRY EXTRACTED FROM THE ARRAY JA. THE SECOND IS SUBROUTINE */
/*     QSORT WHICH PERFORMS A QUICK SORT TO PLACE THE ENTRIES IN */
/*     THE PERMUTED ROWS IN COLUMN ORDER. */

/* ********************************************************************* */

/*<       INTEGER NN,IA(1),JA(1),P(NN),NEWIA(NN),ISYM,IERR >*/
/*<       DOUBLE PRECISION A(1) >*/

/* ... INTERNAL VARIABLES */

/*<       INTEGER BISRCH,I,IBGN,IEND,IP,IPP,J,JAJ,JP,IER,K,N,NELS,NEXT,NPL1 >*/

/*<       DOUBLE PRECISION SAVE,TEMP >*/

/* ********************************************************************* */

/* ... PREPROCESSING PHASE */

/* ...... DETERMINE THE NUMBER OF NONZEROES IN THE ROWS OF THE PERMUTED */
/*        MATRIX AND STORE THAT IN NEWIA.  THEN SWEEP THRU NEWIA TO MAKE */
/*        NEWIA(I) POINT TO THE BEGINNING OF EACH ROW IN THE PERMUTED */
/*        DATA STRUCTURE.  ALSO NEGATE ALL THE ENTRIES IN JA TO INDICATE */
/*        THAT THOSE ENTRIES HAVE NOT BEEN MOVED YET. */

/*<       N = NN >*/
    /* Parameter adjustments */
    --newia;
    --p;
    --ia;
    --ja;
    --a;

    /* Function Body */
    n = *nn;
/*<       IER = 0 >*/
    ier = 0;
/*<       NPL1 = N+1 >*/
    npl1 = n + 1;
/*<       NELS = IA(NPL1)-1 >*/
    nels = ia[npl1] - 1;
/*<       DO 10 I = 1,N >*/
    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<          NEWIA(I) = 0 >*/
	newia[i__] = 0;
/*<    10 CONTINUE >*/
/* L10: */
    }
/*<       DO 30 I = 1,N >*/
    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<          IP = P(I) >*/
	ip = p[i__];
/*<          IBGN = IA(I) >*/
	ibgn = ia[i__];
/*<          IEND = IA(I+1)-1 >*/
	iend = ia[i__ + 1] - 1;
/*<          IF (IBGN.GT.IEND) GO TO 90 >*/
	if (ibgn > iend) {
	    goto L90;
	}
/*<          DO 20 J = IBGN,IEND >*/
	i__2 = iend;
	for (j = ibgn; j <= i__2; ++j) {
/*<             IPP = IP >*/
	    ipp = ip;
/*<             JAJ = JA(J) >*/
	    jaj = ja[j];
/*<             JP = P(JAJ) >*/
	    jp = p[jaj];
/*<             IF (ISYM.EQ.0.AND.IP.GT.JP) IPP = JP >*/
	    if (*isym == 0 && ip > jp) {
		ipp = jp;
	    }
/*<             NEWIA(IPP) = NEWIA(IPP)+1 >*/
	    ++newia[ipp];
/*<             JA(J) = -JAJ >*/
	    ja[j] = -jaj;
/*<    20    CONTINUE >*/
/* L20: */
	}
/*<    30 CONTINUE >*/
/* L30: */
    }
/*<       IBGN = 1 >*/
    ibgn = 1;
/*<       DO 40 I = 1,N >*/
    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<          K = IBGN+NEWIA(I) >*/
	k = ibgn + newia[i__];
/*<          NEWIA(I) = IBGN >*/
	newia[i__] = ibgn;
/*<          IBGN = K >*/
	ibgn = k;
/*<    40 CONTINUE >*/
/* L40: */
    }

/* ...... PREPROCESSING NOW FINISHED. */

/* ...... NOW PERMUTE JA AND A.  THIS PERMUTATION WILL PERFORM THE */
/*        FOLLOWING STEPS */

/*           1.  FIND THE FIRST ENTRY IN JA NOT PERMUTED WHICH IS */
/*               INDICATED BY AN NEGATIVE VALUE IN JA */
/*           2.  COMPUTE WHICH ROW THE CURRENT ENTRY IS IN.  THIS */
/*               IS COMPUTED BY A BISECTION SEARCH THRU THE ARRAY */
/*               IA. */
/*           3.  USING THE PERMUTATION ARRAY P AND THE ARRAY NEWIA */
/*               COMPUTE WHERE THE CURRENT ENTRY IS TO BE PLACED. */
/*           4.  THEN PICK UP THE ENTRY WHERE THE CURRENT ENTRY WILL */
/*               GO.  PUT THE CURRENT ENTRY IN PLACE.  THEN MAKE THE */
/*               DISPLACED ENTRY THE CURRENT ENTRY AND LOOP TO STEP 2. */
/*           5.  THIS PROCESS WILL END WHEN THE NEXT ENTRY HAS ALREADY */
/*               BEEN MOVED.  THEN LOOP TO STEP 1. */

/*<       DO 70 J = 1,NELS >*/
    i__1 = nels;
    for (j = 1; j <= i__1; ++j) {
/*<          IF (JA(J).GT.0) GO TO 70 >*/
	if (ja[j] > 0) {
	    goto L70;
	}
/*<          JAJ = -JA(J) >*/
	jaj = -ja[j];
/*<          SAVE = A(J) >*/
	save = a[j];
/*<          NEXT = J >*/
	next = j;
/*<          JA(J) = JAJ >*/
	ja[j] = jaj;

/*<    50    JP = P(JAJ) >*/
L50:
	jp = p[jaj];
/*<          I = BISRCH(NPL1,IA,NEXT) >*/
	i__ = bisrch_(&npl1, &ia[1], &next);
/*<          IP = P(I) >*/
	ip = p[i__];
/*<          IPP = IP >*/
	ipp = ip;
/*<          IF (ISYM.NE.0.OR.IP.LE.JP) GO TO 60 >*/
	if (*isym != 0 || ip <= jp) {
	    goto L60;
	}
/*<          IPP = JP >*/
	ipp = jp;
/*<          JP = IP >*/
	jp = ip;
/*<    60    NEXT = NEWIA(IPP) >*/
L60:
	next = newia[ipp];

/*<          TEMP = SAVE >*/
	temp = save;
/*<          SAVE = A(NEXT) >*/
	save = a[next];
/*<          A(NEXT) = TEMP >*/
	a[next] = temp;

/*<          JAJ = -JA(NEXT) >*/
	jaj = -ja[next];
/*<          JA(NEXT) = JP >*/
	ja[next] = jp;
/*<          NEWIA(IPP) = NEWIA(IPP)+1 >*/
	++newia[ipp];
/*<          IF (JAJ.GT.0) GO TO 50 >*/
	if (jaj > 0) {
	    goto L50;
	}

/*<    70 CONTINUE >*/
L70:
	;
    }

/* ...... THE MATRIX IS NOW PERMUTED BUT THE ROWS MAY NOT BE IN */
/*        ORDER.  THE REMAINDER OF THIS SUBROUTINE PERFORMS */
/*        A QUICK SORT ON EACH ROW TO SORT THE ENTRIES IN */
/*        COLUMN ORDER.  THE IA ARRAY IS ALSO CORRECTED FROM */
/*        INFORMATION STORED IN THE NEWIA ARRAY.  NEWIA(I) NOW */
/*        POINTS TO THE FIRST ENTRY OF ROW I+1. */

/*<       IA(1) = 1 >*/
    ia[1] = 1;
/*<       DO 80 I = 1,N >*/
    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<          IA(I+1) = NEWIA(I) >*/
	ia[i__ + 1] = newia[i__];
/*<          K = IA(I+1)-IA(I) >*/
	k = ia[i__ + 1] - ia[i__];
/*<          IF (K.EQ.1) GO TO 80 >*/
	if (k == 1) {
	    goto L80;
	}
/*<          IF (K.LT.1) GO TO 110 >*/
	if (k < 1) {
	    goto L110;
	}

/*<          IBGN = IA(I) >*/
	ibgn = ia[i__];
/*<          CALL QSORT (K,JA(IBGN),A(IBGN),IER) >*/
	qsort_(&k, &ja[ibgn], &a[ibgn], &ier);
/*<          IF (IER.NE.0) GO TO 130 >*/
	if (ier != 0) {
	    goto L130;
	}

/*<    80 CONTINUE >*/
L80:
	;
    }

/* ...... END OF MATRIX PERMUTATION */

/*<       GO TO 150 >*/
    goto L150;

/* ... ERROR TRAPS */

/* ...... NO ENTRY IN ROW I IN THE ORIGINAL SYSTEM */

/*<    90 IER = 301 >*/
L90:
    ier = 301;
/*<       IF (LEVEL.GE.0) WRITE (NOUT,100) I >*/
    if (*level >= 0) {
	io___518.ciunit = *nout;
	s_wsfe(&io___518);
	do_fio(&c__1, (char *)&i__, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 150 >*/
    goto L150;

/* ...... NO ENTRY IN ROW I IN THE PERMUTED SYSTEM */

/*<   110 IER = 302 >*/
L110:
    ier = 302;
/*<       IF (LEVEL.GE.0) WRITE (NOUT,120) I >*/
    if (*level >= 0) {
	io___519.ciunit = *nout;
	s_wsfe(&io___519);
	do_fio(&c__1, (char *)&i__, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 150 >*/
    goto L150;

/* ...... ERROR RETURN FROM SUBROUTINE QSORT */

/*<   130 IER = 303 >*/
L130:
    ier = 303;
/*<       IF (LEVEL.GE.0) WRITE (NOUT,140) I >*/
    if (*level >= 0) {
	io___520.ciunit = *nout;
	s_wsfe(&io___520);
	do_fio(&c__1, (char *)&i__, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/

/*<   150 CONTINUE >*/
L150:
/*<       IERR = IER >*/
    *ierr = ier;
/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* permat_ */

/*<       SUBROUTINE PERROR (NN,IA,JA,A,RHS,U,W,DIGTT1,DIGTT2,IDGTTS) >*/
/* Subroutine */ int perror_(integer *nn, integer *ia, integer *ja, 
	doublereal *a, doublereal *rhs, doublereal *u, doublereal *w, 
	doublereal *digtt1, doublereal *digtt2, integer *idgtts)
{
    /* Format strings */
    static char fmt_30[] = "(/6x,\002APPROX. NO. OF DIGITS (EST. REL. ERROR)\
 =\002,f5.1,2x,\002(DIGIT1)\002/3x,\002APPROX. NO. OF DIGITS (EST. REL. RESI\
DUAL) =\002,f5.1,2x,\002(DIGIT2)\002)";

    /* System generated locals */
    doublereal d__1;

    /* Builtin functions */
    double d_lg10(doublereal *);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    integer n;
    extern doublereal ddot_(integer *, doublereal *, integer *, doublereal *, 
	    integer *);
    doublereal bnrm, temp, rnrm;
    extern /* Subroutine */ int vout_(integer *, doublereal *, integer *, 
	    integer *);
    integer idgts;
    extern /* Subroutine */ int pmult_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *), wevmw_(integer *, 
	    doublereal *, doublereal *);
    doublereal digit1, digit2;

    /* Fortran I/O blocks */
    static cilist io___528 = { 0, 0, 0, fmt_30, 0 };



/*     PERROR COMPUTES THE RESIDUAL, R = RHS - A*U.  THE USER */
/*     ALSO HAS THE OPTION OF PRINTING THE RESIDUAL AND/OR THE */
/*     UNKNOWN VECTOR DEPENDING ON IDGTS. */

/* ... PARAMETER LIST: */

/*          N      DIMENSION OF MATRIX (= NN) */
/*          IA,JA  INTEGER ARRAYS OF SPARSE MATRIX REPRESENTATION */
/*          A      D.P. ARRAY OF SPARSE MATRIX REPRESENTATION */
/*          RHS    RIGHT HAND SIDE OF MATRIX PROBLEM */
/*          U      LATEST ESTIMATE OF SOLUTION */
/*          W      WORKSPACE VECTOR */
/*          DIGIT1 OUTPUT: MEASURE OF ACCURACY OF STOPPING TEST (= DIGTT1 */
/*          DIGIT2 OUTPUT: MEASURE OF ACCURACY OF SOLUTION (= DIGTT2) */
/*          IDGTS   PARAMETER CONTROLING LEVEL OF OUTPUT (= IDGTTS) */
/*                    IF IDGTS < 1 OR IDGTS > 4, THEN NO OUTPUT. */
/*                            = 1, THEN NUMBER OF DIGITS IS PRINTED, PRO- */
/*                                 VIDED LEVEL .GE. 1 */
/*                            = 2, THEN SOLUTION VECTOR IS PRINTED, PRO- */
/*                                 VIDED LEVEL .GE. 1 */
/*                            = 3, THEN RESIDUAL VECTOR IS PRINTED, PRO- */
/*                                 VIDED LEVEL .GE. 1 */
/*                            = 4, THEN BOTH VECTORS ARE PRINTED, PRO- */
/*                                 VIDED LEVEL .GE. 1 */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER IA(1),JA(1),NN,IDGTTS >*/
/*<       DOUBLE PRECISION A(1),RHS(NN),U(NN),W(NN),DIGTT1,DIGTT2 >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER IDGTS,N >*/
/*<       DOUBLE PRECISION BNRM,DIGIT1,DIGIT2,RNRM,TEMP >*/

/* ... SPECIFICATIONS FOR FUNCTION SUBPROGRAMS */

/*<       DOUBLE PRECISION DDOT >*/

/* *** BEGIN: ITPACK COMMON */

/*<       INTEGER IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/
/*<       COMMON /ITCOM1/ IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/

/*<       LOGICAL ADAPT,BETADT,CASEII,HALT,PARTAD >*/
/*<       COMMON /ITCOM2/ ADAPT,BETADT,CASEII,HALT,PARTAD >*/

/*<    >*/
/*<    >*/

/* *** END  : ITPACK COMMON */

/*     DESCRIPTION OF VARIABLES IN COMMON BLOCK IN MAIN SUBROUTINE */

/*<       N = NN >*/
    /* Parameter adjustments */
    --w;
    --u;
    --rhs;
    --ia;
    --ja;
    --a;

    /* Function Body */
    n = *nn;
/*<       IDGTS = IDGTTS >*/
    idgts = *idgtts;
/*<       DIGIT1 = 0.D0 >*/
    digit1 = 0.;
/*<       DIGIT2 = 0.D0 >*/
    digit2 = 0.;
/*<       IF (N.LE.0) GO TO 40 >*/
    if (n <= 0) {
	goto L40;
    }

/*<       DIGIT1 = -DLOG10(DABS(DRELPR)) >*/
    d__1 = abs(itcom3_1.drelpr);
    digit1 = -d_lg10(&d__1);
/*<       IF (STPTST.GT.0.D0) DIGIT1 = -DLOG10(DABS(STPTST)) >*/
    if (itcom3_1.stptst > 0.) {
	d__1 = abs(itcom3_1.stptst);
	digit1 = -d_lg10(&d__1);
    }
/*<       BNRM = DDOT(N,RHS,1,RHS,1) >*/
    bnrm = ddot_(&n, &rhs[1], &c__1, &rhs[1], &c__1);
/*<       IF (BNRM.EQ.0.D0) GO TO 10 >*/
    if (bnrm == 0.) {
	goto L10;
    }
/*<       CALL PMULT (N,IA,JA,A,U,W) >*/
    pmult_(&n, &ia[1], &ja[1], &a[1], &u[1], &w[1]);
/*<       CALL WEVMW (N,RHS,W) >*/
    wevmw_(&n, &rhs[1], &w[1]);
/*<       RNRM = DDOT(N,W,1,W,1) >*/
    rnrm = ddot_(&n, &w[1], &c__1, &w[1], &c__1);
/*<       TEMP = RNRM/BNRM >*/
    temp = rnrm / bnrm;
/*<       IF (TEMP.EQ.0.D0) GO TO 10 >*/
    if (temp == 0.) {
	goto L10;
    }
/*<       DIGIT2 = -DLOG10(DABS(TEMP))/2.D0 >*/
    d__1 = abs(temp);
    digit2 = -d_lg10(&d__1) / 2.;
/*<       GO TO 20 >*/
    goto L20;

/*<    10 DIGIT2 = -DLOG10(DABS(DRELPR)) >*/
L10:
    d__1 = abs(itcom3_1.drelpr);
    digit2 = -d_lg10(&d__1);

/*<    20 IF ((IDGTS.LT.1).OR.(LEVEL.LE.0)) GO TO 40 >*/
L20:
    if (idgts < 1 || itcom1_1.level <= 0) {
	goto L40;
    }
/*<       WRITE (NOUT,30) DIGIT1,DIGIT2 >*/
    io___528.ciunit = itcom1_1.nout;
    s_wsfe(&io___528);
    do_fio(&c__1, (char *)&digit1, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&digit2, (ftnlen)sizeof(doublereal));
    e_wsfe();
/*<    >*/

/*<       IF (IDGTS.LE.1.OR.IDGTS.GT.4) GO TO 40 >*/
    if (idgts <= 1 || idgts > 4) {
	goto L40;
    }
/*<       IF (IDGTS.NE.3) CALL VOUT (N,U,2,NOUT) >*/
    if (idgts != 3) {
	vout_(&n, &u[1], &c__2, &itcom1_1.nout);
    }
/*<       IF (IDGTS.GE.3) CALL VOUT (N,W,1,NOUT) >*/
    if (idgts >= 3) {
	vout_(&n, &w[1], &c__1, &itcom1_1.nout);
    }

/*<    40 CONTINUE >*/
L40:
/*<       DIGTT1 = DIGIT1 >*/
    *digtt1 = digit1;
/*<       DIGTT2 = DIGIT2 >*/
    *digtt2 = digit2;
/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* perror_ */

/*<       SUBROUTINE PERVEC (N,V,P) >*/
/* Subroutine */ int pervec_(integer *n, doublereal *v, integer *p)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    integer ii, now;
    doublereal save, temp;
    integer next;


/*     THIS SUBROUTINE PERMUTES A D.P. VECTOR AS DICTATED BY THE */
/*     PERMUTATION VECTOR, P.  IF P(I) = J, THEN V(J) GETS V(I). */

/* ... PARAMETER LIST: */

/*          V      D.P. VECTOR OF LENGTH N */
/*          P     INTEGER PERMUTATION VECTOR */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER N,P(N) >*/
/*<       DOUBLE PRECISION V(N) >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER II,NEXT,NOW >*/
/*<       DOUBLE PRECISION SAVE,TEMP >*/

/*<       IF (N.LE.0) RETURN >*/
    /* Parameter adjustments */
    --p;
    --v;

    /* Function Body */
    if (*n <= 0) {
	return 0;
    }

/*<       DO 20 II = 1,N >*/
    i__1 = *n;
    for (ii = 1; ii <= i__1; ++ii) {
/*<          IF (P(II).LT.0) GO TO 20 >*/
	if (p[ii] < 0) {
	    goto L20;
	}

/*<          NEXT = P(II) >*/
	next = p[ii];
/*<          SAVE = V(II) >*/
	save = v[ii];

/*<    10    CONTINUE >*/
L10:
/*<          IF (P(NEXT).LT.0) GO TO 20 >*/
	if (p[next] < 0) {
	    goto L20;
	}
/*<          TEMP = SAVE >*/
	temp = save;
/*<          SAVE = V(NEXT) >*/
	save = v[next];
/*<          V(NEXT) = TEMP >*/
	v[next] = temp;

/*<          NOW = NEXT >*/
	now = next;
/*<          NEXT = P(NOW) >*/
	next = p[now];
/*<          P(NOW) = -NEXT >*/
	p[now] = -next;
/*<          GO TO 10 >*/
	goto L10;

/*<    20 CONTINUE >*/
L20:
	;
    }

/*<       DO 30 II = 1,N >*/
    i__1 = *n;
    for (ii = 1; ii <= i__1; ++ii) {
/*<          P(II) = -P(II) >*/
	p[ii] = -p[ii];
/*<    30 CONTINUE >*/
/* L30: */
    }

/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* pervec_ */

/*<       SUBROUTINE PFSOR (NN,IA,JA,A,U,RHS) >*/
/* Subroutine */ int pfsor_(integer *nn, integer *ia, integer *ja, doublereal 
	*a, doublereal *u, doublereal *rhs)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Local variables */
    integer n, ii, jj;
    doublereal ui, sum, omm1;
    integer jajj, ibgn, iend;


/*         THIS SUBROUTINE COMPUTES A FORWARD SOR SWEEP. */

/* ... PARAMETER LIST: */

/*         N       ORDER OF SYSTEM (= NN) */
/*          OMEGA  RELAXATION FACTOR */
/*          IA,JA  INTEGER ARRAYS OF SPARSE MATRIX REPRESENTATION */
/*          A      D.P. ARRAY OF SPARSE MATRIX REPRESENTATION */
/*          U      LATEST ESTIMATE OF SOLUTION */
/*          RHS    RIGHT HAND SIDE OF MATRIX PROBLEM */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER IA(1),JA(1),NN >*/
/*<       DOUBLE PRECISION A(1),U(NN),RHS(NN) >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER IBGN,IEND,II,JAJJ,JJ,N >*/
/*<       DOUBLE PRECISION OMM1,SUM,UI >*/

/* *** BEGIN: ITPACK COMMON */

/*<       INTEGER IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/
/*<       COMMON /ITCOM1/ IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/

/*<       LOGICAL ADAPT,BETADT,CASEII,HALT,PARTAD >*/
/*<       COMMON /ITCOM2/ ADAPT,BETADT,CASEII,HALT,PARTAD >*/

/*<    >*/
/*<    >*/

/* *** END  : ITPACK COMMON */

/*     DESCRIPTION OF VARIABLES IN COMMON BLOCK IN MAIN SUBROUTINE */

/*<       N = NN >*/
    /* Parameter adjustments */
    --rhs;
    --u;
    --ia;
    --ja;
    --a;

    /* Function Body */
    n = *nn;
/*<       OMM1 = OMEGA-1.D0 >*/
    omm1 = itcom3_1.omega - 1.;
/*<       IF (ISYM.EQ.0) GO TO 40 >*/
    if (itcom1_1.isym == 0) {
	goto L40;
    }

/*     *********** NON - SYMMETRIC SECTION ********************* */

/*<       DO 30 II = 1,N >*/
    i__1 = n;
    for (ii = 1; ii <= i__1; ++ii) {
/*<          IBGN = IA(II) >*/
	ibgn = ia[ii];
/*<          IEND = IA(II+1)-1 >*/
	iend = ia[ii + 1] - 1;
/*<          SUM = RHS(II) >*/
	sum = rhs[ii];
/*<          IF (IBGN.GT.IEND) GO TO 20 >*/
	if (ibgn > iend) {
	    goto L20;
	}
/*<          DO 10 JJ = IBGN,IEND >*/
	i__2 = iend;
	for (jj = ibgn; jj <= i__2; ++jj) {
/*<             JAJJ = JA(JJ) >*/
	    jajj = ja[jj];
/*<             SUM = SUM-A(JJ)*U(JAJJ) >*/
	    sum -= a[jj] * u[jajj];
/*<    10    CONTINUE >*/
/* L10: */
	}
/*<    20    UI = OMEGA*SUM-OMM1*U(II) >*/
L20:
	ui = itcom3_1.omega * sum - omm1 * u[ii];
/*<          U(II) = UI >*/
	u[ii] = ui;
/*<    30 CONTINUE >*/
/* L30: */
    }
/*<       RETURN >*/
    return 0;

/*     ************* SYMMETRIC SECTION ************************* */

/*<    40 DO 80 II = 1,N >*/
L40:
    i__1 = n;
    for (ii = 1; ii <= i__1; ++ii) {
/*<          IBGN = IA(II) >*/
	ibgn = ia[ii];
/*<          IEND = IA(II+1)-1 >*/
	iend = ia[ii + 1] - 1;
/*<          SUM = RHS(II) >*/
	sum = rhs[ii];
/*<          IF (IBGN.GT.IEND) GO TO 60 >*/
	if (ibgn > iend) {
	    goto L60;
	}
/*<          DO 50 JJ = IBGN,IEND >*/
	i__2 = iend;
	for (jj = ibgn; jj <= i__2; ++jj) {
/*<             JAJJ = JA(JJ) >*/
	    jajj = ja[jj];
/*<             SUM = SUM-A(JJ)*U(JAJJ) >*/
	    sum -= a[jj] * u[jajj];
/*<    50    CONTINUE >*/
/* L50: */
	}
/*<    60    UI = OMEGA*SUM-OMM1*U(II) >*/
L60:
	ui = itcom3_1.omega * sum - omm1 * u[ii];
/*<          U(II) = UI >*/
	u[ii] = ui;
/*<          IF (IBGN.GT.IEND) GO TO 80 >*/
	if (ibgn > iend) {
	    goto L80;
	}
/*<          DO 70 JJ = IBGN,IEND >*/
	i__2 = iend;
	for (jj = ibgn; jj <= i__2; ++jj) {
/*<             JAJJ = JA(JJ) >*/
	    jajj = ja[jj];
/*<             RHS(JAJJ) = RHS(JAJJ)-A(JJ)*UI >*/
	    rhs[jajj] -= a[jj] * ui;
/*<    70    CONTINUE >*/
/* L70: */
	}
/*<    80 CONTINUE >*/
L80:
	;
    }
/*<       RETURN >*/
    return 0;

/*<       END >*/
} /* pfsor_ */

/*<       SUBROUTINE PFSOR1 (NN,IA,JA,A,U,RHS) >*/
/* Subroutine */ int pfsor1_(integer *nn, integer *ia, integer *ja, 
	doublereal *a, doublereal *u, doublereal *rhs)
{
    /* System generated locals */
    integer i__1, i__2;
    doublereal d__1;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    integer n, ii, jj;
    doublereal ui, sum, omm1;
    integer jajj, ibgn, iend;
    doublereal sumd;


/*         THIS SUBROUTINE COMPUTES A FORWARD SOR SWEEP ON U AND */
/*         COMPUTES THE NORM OF THE PSEUDO-RESIDUAL VECTOR. */

/* ... PARAMETER LIST: */

/*          N      ORDER OF SYSTEM (= NN) */
/*          OMEGA  RELAXATION FACTOR */
/*          IA,JA  INTEGER ARRAYS OF SPARSE MATRIX REPRESENTATION */
/*          A      D.P. ARRAY OF SPARSE MATRIX REPRESENTATION */
/*          U      LATEST ESTIMATE OF SOLUTION */
/*          RHS    RIGHT HAND SIDE OF MATRIX PROBLEM */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER IA(1),JA(1),NN >*/
/*<       DOUBLE PRECISION A(1),U(NN),RHS(NN) >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER IBGN,IEND,II,JAJJ,JJ,N >*/
/*<       DOUBLE PRECISION OMM1,SUM,SUMD,UI >*/

/* *** BEGIN: ITPACK COMMON */

/*<       INTEGER IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/
/*<       COMMON /ITCOM1/ IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/

/*<       LOGICAL ADAPT,BETADT,CASEII,HALT,PARTAD >*/
/*<       COMMON /ITCOM2/ ADAPT,BETADT,CASEII,HALT,PARTAD >*/

/*<    >*/
/*<    >*/

/* *** END  : ITPACK COMMON */

/*     DESCRIPTION OF VARIABLES IN COMMON BLOCK IN MAIN SUBROUTINE */

/*<       N = NN >*/
    /* Parameter adjustments */
    --rhs;
    --u;
    --ia;
    --ja;
    --a;

    /* Function Body */
    n = *nn;
/*<       OMM1 = OMEGA-1.D0 >*/
    omm1 = itcom3_1.omega - 1.;
/*<       SUMD = 0.D0 >*/
    sumd = 0.;
/*<       IF (ISYM.EQ.0) GO TO 40 >*/
    if (itcom1_1.isym == 0) {
	goto L40;
    }

/*     **************** NON - SYMMETRIC SECTION ****************** */

/*<       DO 30 II = 1,N >*/
    i__1 = n;
    for (ii = 1; ii <= i__1; ++ii) {
/*<          IBGN = IA(II) >*/
	ibgn = ia[ii];
/*<          IEND = IA(II+1)-1 >*/
	iend = ia[ii + 1] - 1;
/*<          SUM = RHS(II) >*/
	sum = rhs[ii];
/*<          IF (IBGN.GT.IEND) GO TO 20 >*/
	if (ibgn > iend) {
	    goto L20;
	}
/*<          DO 10 JJ = IBGN,IEND >*/
	i__2 = iend;
	for (jj = ibgn; jj <= i__2; ++jj) {
/*<             JAJJ = JA(JJ) >*/
	    jajj = ja[jj];
/*<             SUM = SUM-A(JJ)*U(JAJJ) >*/
	    sum -= a[jj] * u[jajj];
/*<    10    CONTINUE >*/
/* L10: */
	}
/*<    20    CONTINUE >*/
L20:
/*<          UI = OMEGA*SUM-OMM1*U(II) >*/
	ui = itcom3_1.omega * sum - omm1 * u[ii];
/*<          SUMD = SUMD+(UI-U(II))**2 >*/
/* Computing 2nd power */
	d__1 = ui - u[ii];
	sumd += d__1 * d__1;
/*<          U(II) = UI >*/
	u[ii] = ui;
/*<    30 CONTINUE >*/
/* L30: */
    }
/*<       GO TO 90 >*/
    goto L90;

/*     *************** SYMMETRIC SECTION ************************ */

/*<    40 DO 80 II = 1,N >*/
L40:
    i__1 = n;
    for (ii = 1; ii <= i__1; ++ii) {
/*<          IBGN = IA(II) >*/
	ibgn = ia[ii];
/*<          IEND = IA(II+1)-1 >*/
	iend = ia[ii + 1] - 1;
/*<          SUM = RHS(II) >*/
	sum = rhs[ii];
/*<          IF (IBGN.GT.IEND) GO TO 60 >*/
	if (ibgn > iend) {
	    goto L60;
	}
/*<          DO 50 JJ = IBGN,IEND >*/
	i__2 = iend;
	for (jj = ibgn; jj <= i__2; ++jj) {
/*<             JAJJ = JA(JJ) >*/
	    jajj = ja[jj];
/*<             SUM = SUM-A(JJ)*U(JAJJ) >*/
	    sum -= a[jj] * u[jajj];
/*<    50    CONTINUE >*/
/* L50: */
	}
/*<    60    CONTINUE >*/
L60:
/*<          UI = OMEGA*SUM-OMM1*U(II) >*/
	ui = itcom3_1.omega * sum - omm1 * u[ii];
/*<          SUMD = SUMD+(UI-U(II))**2 >*/
/* Computing 2nd power */
	d__1 = ui - u[ii];
	sumd += d__1 * d__1;
/*<          U(II) = UI >*/
	u[ii] = ui;
/*<          IF (IBGN.GT.IEND) GO TO 80 >*/
	if (ibgn > iend) {
	    goto L80;
	}
/*<          DO 70 JJ = IBGN,IEND >*/
	i__2 = iend;
	for (jj = ibgn; jj <= i__2; ++jj) {
/*<             JAJJ = JA(JJ) >*/
	    jajj = ja[jj];
/*<             RHS(JAJJ) = RHS(JAJJ)-A(JJ)*UI >*/
	    rhs[jajj] -= a[jj] * ui;
/*<    70    CONTINUE >*/
/* L70: */
	}
/*<    80 CONTINUE >*/
L80:
	;
    }

/*<    90 DELNNM = DSQRT(SUMD) >*/
L90:
    itcom3_1.delnnm = sqrt(sumd);
/*<       RETURN >*/
    return 0;

/*<       END >*/
} /* pfsor1_ */

/*<       SUBROUTINE PJAC (NN,IA,JA,A,U,RHS) >*/
/* Subroutine */ int pjac_(integer *nn, integer *ia, integer *ja, doublereal *
	a, doublereal *u, doublereal *rhs)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Local variables */
    integer n, ii, jj;
    doublereal uii;
    integer jajj, ibgn, iend;
    doublereal rhsii;


/*     ... THIS SUBROUTINE PERFORMS ONE JACOBI ITERATION. */

/* ... PARAMETER LIST: */

/*          N      DIMENSION OF MATRIX (= NN) */
/*          IA,JA  INTEGER ARRAYS OF SPARSE MATRIX REPRESENTATION */
/*          A      D.P. ARRAY OF SPARSE MATRIX REPRESENTATION */
/*          U      ESTIMATE OF SOLUTION OF A MATRIX PROBLEM */
/*          RHS    ON INPUT: CONTAINS THE RIGHT HAND SIDE OF */
/*                    A MATRIX PROBLEM */
/*                 ON OUTPUT: CONTAINS A*U + RHS */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER IA(1),JA(1),NN >*/
/*<       DOUBLE PRECISION A(1),U(NN),RHS(NN) >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER IBGN,IEND,II,JAJJ,JJ,N >*/
/*<       DOUBLE PRECISION RHSII,UII >*/

/* *** BEGIN: ITPACK COMMON */

/*<       INTEGER IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/
/*<       COMMON /ITCOM1/ IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/

/*<       LOGICAL ADAPT,BETADT,CASEII,HALT,PARTAD >*/
/*<       COMMON /ITCOM2/ ADAPT,BETADT,CASEII,HALT,PARTAD >*/

/*<    >*/
/*<    >*/

/* *** END  : ITPACK COMMON */

/*     DESCRIPTION OF VARIABLES IN COMMON BLOCK IN MAIN SUBROUTINE */

/*<       N = NN >*/
    /* Parameter adjustments */
    --rhs;
    --u;
    --ia;
    --ja;
    --a;

    /* Function Body */
    n = *nn;
/*<       IF (ISYM.EQ.0) GO TO 30 >*/
    if (itcom1_1.isym == 0) {
	goto L30;
    }

/*     *************** NON - SYMMETRIC SECTION **************** */

/*<       DO 20 II = 1,N >*/
    i__1 = n;
    for (ii = 1; ii <= i__1; ++ii) {
/*<          IBGN = IA(II) >*/
	ibgn = ia[ii];
/*<          IEND = IA(II+1)-1 >*/
	iend = ia[ii + 1] - 1;
/*<          IF (IBGN.GT.IEND) GO TO 20 >*/
	if (ibgn > iend) {
	    goto L20;
	}
/*<          RHSII = RHS(II) >*/
	rhsii = rhs[ii];
/*<          DO 10 JJ = IBGN,IEND >*/
	i__2 = iend;
	for (jj = ibgn; jj <= i__2; ++jj) {
/*<             JAJJ = JA(JJ) >*/
	    jajj = ja[jj];
/*<             RHSII = RHSII-A(JJ)*U(JAJJ) >*/
	    rhsii -= a[jj] * u[jajj];
/*<    10    CONTINUE >*/
/* L10: */
	}
/*<          RHS(II) = RHSII >*/
	rhs[ii] = rhsii;
/*<    20 CONTINUE >*/
L20:
	;
    }
/*<       RETURN >*/
    return 0;

/*     ************** SYMMETRIC SECTION ********************** */

/*<    30 DO 50 II = 1,N >*/
L30:
    i__1 = n;
    for (ii = 1; ii <= i__1; ++ii) {
/*<          IBGN = IA(II) >*/
	ibgn = ia[ii];
/*<          IEND = IA(II+1)-1 >*/
	iend = ia[ii + 1] - 1;
/*<          IF (IBGN.GT.IEND) GO TO 50 >*/
	if (ibgn > iend) {
	    goto L50;
	}
/*<          RHSII = RHS(II) >*/
	rhsii = rhs[ii];
/*<          UII = U(II) >*/
	uii = u[ii];
/*<          DO 40 JJ = IBGN,IEND >*/
	i__2 = iend;
	for (jj = ibgn; jj <= i__2; ++jj) {
/*<             JAJJ = JA(JJ) >*/
	    jajj = ja[jj];
/*<             RHSII = RHSII-A(JJ)*U(JAJJ) >*/
	    rhsii -= a[jj] * u[jajj];
/*<             RHS(JAJJ) = RHS(JAJJ)-A(JJ)*UII >*/
	    rhs[jajj] -= a[jj] * uii;
/*<    40    CONTINUE >*/
/* L40: */
	}
/*<          RHS(II) = RHSII >*/
	rhs[ii] = rhsii;
/*<    50 CONTINUE >*/
L50:
	;
    }
/*<       RETURN >*/
    return 0;

/*<       END >*/
} /* pjac_ */

/*<       SUBROUTINE PMULT (NN,IA,JA,A,U,W) >*/
/* Subroutine */ int pmult_(integer *nn, integer *ia, integer *ja, doublereal 
	*a, doublereal *u, doublereal *w)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Local variables */
    integer n, ii, jj;
    doublereal uii, wii, sum;
    integer jajj, ibgn, iend;
    extern /* Subroutine */ int vfill_(integer *, doublereal *, doublereal *);


/*     ... THIS SUBROUTINE PERFORMS ONE MATRIX-VECTOR MULTIPLICATION. */

/* ... PARAMETER LIST: */

/*          N      DIMENSION OF MATRIX (= NN) */
/*          IA,JA  INTEGER ARRAYS OF SPARSE MATRIX REPRESENTATION */
/*          A      D.P. ARRAY OF SPARSE MATRIX REPRESENTATION */
/*          U      LATEST ESTIMATE OF SOLUTION */
/*          W      ON RETURN W CONTAINS A*U */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER IA(1),JA(1),NN >*/
/*<       DOUBLE PRECISION A(1),U(NN),W(NN) >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER IBGN,IEND,II,JJ,N >*/
/*<       DOUBLE PRECISION SUM,UII,WII >*/

/* *** BEGIN: ITPACK COMMON */

/*<       INTEGER IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/
/*<       COMMON /ITCOM1/ IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/

/*<       LOGICAL ADAPT,BETADT,CASEII,HALT,PARTAD >*/
/*<       COMMON /ITCOM2/ ADAPT,BETADT,CASEII,HALT,PARTAD >*/

/*<    >*/
/*<    >*/

/* *** END  : ITPACK COMMON */

/*     DESCRIPTION OF VARIABLES IN COMMON BLOCK IN MAIN SUBROUTINE */

/*<       N = NN >*/
    /* Parameter adjustments */
    --w;
    --u;
    --ia;
    --ja;
    --a;

    /* Function Body */
    n = *nn;
/*<       IF (N.LE.0) RETURN >*/
    if (n <= 0) {
	return 0;
    }
/*<       IF (ISYM.EQ.0) GO TO 40 >*/
    if (itcom1_1.isym == 0) {
	goto L40;
    }

/*     *************** NON - SYMMETRIC SECTION ********************** */

/*<       DO 30 II = 1,N >*/
    i__1 = n;
    for (ii = 1; ii <= i__1; ++ii) {
/*<          IBGN = IA(II) >*/
	ibgn = ia[ii];
/*<          IEND = IA(II+1)-1 >*/
	iend = ia[ii + 1] - 1;
/*<          SUM = 0.0D0 >*/
	sum = 0.;
/*<          IF (IBGN.GT.IEND) GO TO 20 >*/
	if (ibgn > iend) {
	    goto L20;
	}
/*<          DO 10 JJ = IBGN,IEND >*/
	i__2 = iend;
	for (jj = ibgn; jj <= i__2; ++jj) {
/*<             JAJJ = JA(JJ) >*/
	    jajj = ja[jj];
/*<             SUM = SUM+A(JJ)*U(JAJJ) >*/
	    sum += a[jj] * u[jajj];
/*<    10    CONTINUE >*/
/* L10: */
	}
/*<    20    W(II) = SUM >*/
L20:
	w[ii] = sum;
/*<    30 CONTINUE >*/
/* L30: */
    }
/*<       RETURN >*/
    return 0;

/*     ***************** SYMMETRIC SECTION ************************** */

/*<    40 CALL VFILL (N,W,0.D0) >*/
L40:
    vfill_(&n, &w[1], &c_b21);
/*<       DO 70 II = 1,N >*/
    i__1 = n;
    for (ii = 1; ii <= i__1; ++ii) {
/*<          IBGN = IA(II) >*/
	ibgn = ia[ii];
/*<          IEND = IA(II+1)-1 >*/
	iend = ia[ii + 1] - 1;
/*<          UII = U(II) >*/
	uii = u[ii];
/*<          WII = W(II) >*/
	wii = w[ii];
/*<          IF (IBGN.GT.IEND) GO TO 60 >*/
	if (ibgn > iend) {
	    goto L60;
	}
/*<          DO 50 JJ = IBGN,IEND >*/
	i__2 = iend;
	for (jj = ibgn; jj <= i__2; ++jj) {
/*<             JAJJ = JA(JJ) >*/
	    jajj = ja[jj];
/*<             WII = WII+A(JJ)*U(JAJJ) >*/
	    wii += a[jj] * u[jajj];
/*<             W(JAJJ) = W(JAJJ)+A(JJ)*UII >*/
	    w[jajj] += a[jj] * uii;
/*<    50    CONTINUE >*/
/* L50: */
	}
/*<    60    W(II) = WII >*/
L60:
	w[ii] = wii;
/*<    70 CONTINUE >*/
/* L70: */
    }
/*<       RETURN >*/
    return 0;

/*<       END >*/
} /* pmult_ */

/*<       SUBROUTINE PRBNDX (NN,NBLACK,IA,JA,P,IP,LEVEL,NOUT,IER) >*/
/* Subroutine */ int prbndx_(integer *nn, integer *nblack, integer *ia, 
	integer *ja, integer *p, integer *ip, integer *level, integer *nout, 
	integer *ier)
{
    /* Format strings */
    static char fmt_170[] = "(\0020\002,\002*** F A T A L     E R R O R ****\
********\002/\0020\002,\002    IN ITPACK ROUTINE PRBNDX  \002/\002 \002,\002\
    RED-BLACK INDEXING NOT POSSIBLE\002)";

    /* System generated locals */
    integer i__1, i__2, i__3, i__4;

    /* Builtin functions */
    integer i_sign(integer *, integer *), s_wsfe(cilist *), e_wsfe();

    /* Local variables */
    integer i__, j, k, n, old, ibgn, iend, nred, last, next, type__, first, 
	    young, curtyp, nxttyp;

    /* Fortran I/O blocks */
    static cilist io___585 = { 0, 0, 0, fmt_170, 0 };



/* ************************************************************** */

/*     THIS SUBROUTINE COMPUTES THE RED-BLACK PERMUTATION */
/*     VECTORS P ( AND ITS INVERSE IP ) IF POSSIBLE. */

/*     THE ALGORITHM IS TO MARK THE FIRST NODE AS RED (ARBITRARY). */
/*     ALL OF ITS ADJACENT NODES ARE MARKED BLACK AND PLACED IN */
/*     A STACK.  THE REMAINDER OF THE CODE PULLS THE FIRST NODE */
/*     OFF THE TOP OF THE STACK AND TRIES TO TYPE ITS ADJACENT NODES. */
/*     THE TYPING OF THE ADJACENT POINT IS A FIVE WAY CASE STATEMENT */
/*     WHICH IS WELL COMMENTED BELOW (SEE DO LOOP 100). */

/*     THE ARRAY P IS USED BOTH TO KEEP TRACK OF THE COLOR OF A NODE */
/*     (RED NODE IS POSITIVE, BLACK IS NEGATIVE) BUT ALSO THE FATHER */
/*     NODE THAT CAUSED THE COLOR MARKING OF THAT POINT.  SINCE */
/*     COMPLETE INFORMATION ON THE ADJACENCY STRUCTURE IS HARD TO COME */
/*     BY THIS FORMS A LINK TO ENABLE THE COLOR CHANGE OF A PARTIAL */
/*     TREE WHEN A RECOVERABLE COLOR CONFLICT OCCURS. */

/*     THE ARRAY IP IS USED AS A STACK TO POINT TO THE SET OF NODES */
/*     LEFT TO BE TYPED THAT ARE KNOWN TO BE ADJACENT TO THE CURRENT */
/*     FATHER NODE. */

/* ********************************************************************* */

/*     INPUT PARAMETERS */

/*        N      NUMBER OF NODES.  (INTEGER, SCALAR) (= NN) */

/*        IA,JA  ADJACENCY STRUCTURE ARRAYS.  CAN BE EITHER THE */
/*               SYMMETRIC OR NONSYMMETRIC FORM.  IT IS ASSUMED */
/*               THAT FOR EVERY ROW WHERE ONLY ONE ELEMENT IS */
/*               STORED THAT ELEMENT CORRESPONDS TO THE DIAGONAL */
/*               ENTRY.  THE DIAGONAL DOES NOT HAVE TO BE THE FIRST */
/*               ENTRY STORED.  (INTEGER, ARRAYS) */
/*        LEVEL  SWITCH FOR PRINTING */
/*        NOUT OUTPUT TAPE NUMBER */

/*     OUTPUT PARAMETERS */

/*        NBLACK NUMBER OF BLACK NODES.  NUMBER OF RED NODES IS */
/*               N - NBLACK.  (INTEGER, SCALAR) */

/*        P, IP  PERMUTATION AND INVERSE PERMUTATION VECTORS. */
/*               (INTEGER, ARRAYS EACH OF LENGTH N) */

/*        IER    ERROR FLAG. (INTEGER, SCALAR) */

/*               IER = 0, NORMAL RETURN.  INDEXING PERFORMED */
/*                        SUCCESSFULLY */
/*               IER =201, RED-BLACK INDEXING NOT POSSIBLE. */

/* ******************************************************************** */

/*<       INTEGER NN,NBLACK,IA(1),JA(1),P(NN),IP(NN),IER >*/

/*<    >*/

/* ----------------------------------------------------------------------- */

/*<       N = NN >*/
    /* Parameter adjustments */
    --ip;
    --p;
    --ia;
    --ja;

    /* Function Body */
    n = *nn;
/*<       IER = 0 >*/
    *ier = 0;

/*        IF ( N .LE. 0 ) GO TO 8000 */

/*<       DO 10 I = 1,N >*/
    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<          P(I) = 0 >*/
	p[i__] = 0;
/*<          IP(I) = 0 >*/
	ip[i__] = 0;
/*<    10 CONTINUE >*/
/* L10: */
    }

/* ... HANDLE THE FIRST SET OF POINTS UNTIL SOME ADJACENT POINTS */
/* ... ARE FOUND */

/*<       FIRST = 1 >*/
    first = 1;

/*<    20 P(FIRST) = FIRST >*/
L20:
    p[first] = first;
/*<       IF (IA(FIRST+1)-IA(FIRST).GT.1) GO TO 40 >*/
    if (ia[first + 1] - ia[first] > 1) {
	goto L40;
    }

/* ... SEARCH FOR NEXT ENTRY THAT HAS NOT BEEN MARKED */

/*<       IF (FIRST.EQ.N) GO TO 130 >*/
    if (first == n) {
	goto L130;
    }
/*<       IBGN = FIRST+1 >*/
    ibgn = first + 1;
/*<       DO 30 I = IBGN,N >*/
    i__1 = n;
    for (i__ = ibgn; i__ <= i__1; ++i__) {
/*<          IF (P(I).NE.0) GO TO 30 >*/
	if (p[i__] != 0) {
	    goto L30;
	}
/*<          FIRST = I >*/
	first = i__;
/*<          GO TO 20 >*/
	goto L20;
/*<    30 CONTINUE >*/
L30:
	;
    }
/*<       GO TO 130 >*/
    goto L130;

/* ... FIRST SET OF ADJACENT POINTS FOUND */

/*<    40 NEXT = 1 >*/
L40:
    next = 1;
/*<       LAST = 1 >*/
    last = 1;
/*<       IP(1) = FIRST >*/
    ip[1] = first;

/* ... LOOP OVER LABELED POINTS INDICATED IN THE STACK STORED IN */
/* ... THE ARRAY IP */

/*<    50 K = IP(NEXT) >*/
L50:
    k = ip[next];
/*<       CURTYP = P(K) >*/
    curtyp = p[k];
/*<       NXTTYP = -CURTYP >*/
    nxttyp = -curtyp;
/*<       IBGN = IA(K) >*/
    ibgn = ia[k];
/*<       IEND = IA(K+1)-1 >*/
    iend = ia[k + 1] - 1;
/*<       IF (IBGN.GT.IEND) GO TO 110 >*/
    if (ibgn > iend) {
	goto L110;
    }
/*<       DO 100 I = IBGN,IEND >*/
    i__1 = iend;
    for (i__ = ibgn; i__ <= i__1; ++i__) {
/*<          J = JA(I) >*/
	j = ja[i__];
/*<          TYPE = P(J) >*/
	type__ = p[j];
/*<          IF (J.EQ.K) GO TO 100 >*/
	if (j == k) {
	    goto L100;
	}

/* ================================================================== */

/*     THE FOLLOWING IS A FIVE WAY CASE STATEMENT DEALING WITH THE */
/*     LABELING OF THE ADJACENT NODE. */

/* ... CASE I.  IF THE ADJACENT NODE HAS ALREADY BEEN LABELED WITH */
/*              LABEL EQUAL TO NXTTYP, THEN SKIP TO THE NEXT ADJACENT */
/*              NODE. */

/*<          IF (TYPE.EQ.NXTTYP) GO TO 100 >*/
	if (type__ == nxttyp) {
	    goto L100;
	}

/* ... CASE II.  IF THE ADJACENT NODE HAS NOT BEEN LABELED YET LABEL */
/*               IT WITH NXTTYP AND ENTER IT IN THE STACK */

/*<          IF (TYPE.NE.0) GO TO 60 >*/
	if (type__ != 0) {
	    goto L60;
	}
/*<          LAST = LAST+1 >*/
	++last;
/*<          IP(LAST) = J >*/
	ip[last] = j;
/*<          P(J) = NXTTYP >*/
	p[j] = nxttyp;
/*<          GO TO 100 >*/
	goto L100;

/* ... CASE III.  IF THE ADJACENT NODE HAS ALREADY BEEN LABELED WITH */
/*                OPPOSITE COLOR AND THE SAME FATHER SEED, THEN THERE */
/*                IS AN IRRECOVERABLE COLOR CONFLICT. */

/*<    60    IF (TYPE.EQ.CURTYP) GO TO 160 >*/
L60:
	if (type__ == curtyp) {
	    goto L160;
	}

/* ... CASE IV.  IF THE ADJACENT NODE HAS THE RIGHT COLOR AND A DIFFERENT */
/*               FATHER NODE, THEN CHANGE ALL NODES OF THE YOUNGEST FATHE */
/*               NODE TO POINT TO THE OLDEST FATHER SEED AND RETAIN THE */
/*               SAME COLORS. */

/*<          IF (TYPE*NXTTYP.LT.1) GO TO 80 >*/
	if (type__ * nxttyp < 1) {
	    goto L80;
	}
/*<          OLD = MIN0(IABS(TYPE),IABS(NXTTYP)) >*/
/* Computing MIN */
	i__2 = abs(type__), i__3 = abs(nxttyp);
	old = min(i__2,i__3);
/*<          YOUNG = MAX0(IABS(TYPE),IABS(NXTTYP)) >*/
/* Computing MAX */
	i__2 = abs(type__), i__3 = abs(nxttyp);
	young = max(i__2,i__3);
/*<          DO 70 J = YOUNG,N >*/
	i__2 = n;
	for (j = young; j <= i__2; ++j) {
/*<             IF (IABS(P(J)).EQ.YOUNG) P(J) = ISIGN(OLD,P(J)) >*/
	    if ((i__3 = p[j], abs(i__3)) == young) {
		p[j] = i_sign(&old, &p[j]);
	    }
/*<    70    CONTINUE >*/
/* L70: */
	}
/*<          CURTYP = P(K) >*/
	curtyp = p[k];
/*<          NXTTYP = -CURTYP >*/
	nxttyp = -curtyp;
/*<          GO TO 100 >*/
	goto L100;

/* ... CASE V.  IF THE ADJACENT NODE HAS THE WRONG COLOR AND A DIFFERENT */
/*              FATHER NODE, THEN CHANGE ALL NODES OF THE YOUNGEST FATHER */
/*              NODE TO POINT TO THE OLDEST FATHER NODE ALONG WITH */
/*              CHANGING THEIR COLORS.  SINCE UNTIL THIS TIME THE */
/*              YOUNGEST FATHER NODE TREE HAS BEEN INDEPENDENT NO OTHER */
/*              COLOR CONFLICTS WILL ARISE FROM THIS CHANGE. */

/*<    80    OLD = MIN0(IABS(TYPE),IABS(NXTTYP)) >*/
L80:
/* Computing MIN */
	i__2 = abs(type__), i__3 = abs(nxttyp);
	old = min(i__2,i__3);
/*<          YOUNG = MAX0(IABS(TYPE),IABS(NXTTYP)) >*/
/* Computing MAX */
	i__2 = abs(type__), i__3 = abs(nxttyp);
	young = max(i__2,i__3);
/*<          DO 90 J = YOUNG,N >*/
	i__2 = n;
	for (j = young; j <= i__2; ++j) {
/*<             IF (IABS(P(J)).EQ.YOUNG) P(J) = ISIGN(OLD,-P(J)) >*/
	    if ((i__3 = p[j], abs(i__3)) == young) {
		i__4 = -p[j];
		p[j] = i_sign(&old, &i__4);
	    }
/*<    90    CONTINUE >*/
/* L90: */
	}
/*<          CURTYP = P(K) >*/
	curtyp = p[k];
/*<          NXTTYP = -CURTYP >*/
	nxttyp = -curtyp;

/* ... END OF CASE STATEMENT */

/* ================================================================== */

/*<   100 CONTINUE >*/
L100:
	;
    }

/* ... ADVANCE TO NEXT NODE IN THE STACK */

/*<   110 NEXT = NEXT+1 >*/
L110:
    ++next;
/*<       IF (NEXT.LE.LAST) GO TO 50 >*/
    if (next <= last) {
	goto L50;
    }

/* ... ALL NODES IN THE STACK HAVE BEEN REMOVED */

/* ... CHECK FOR NODES NOT LABELED.  IF ANY ARE FOUND */
/* ... START THE LABELING PROCESS AGAIN AT THE FIRST */
/* ... NODE FOUND THAT IS NOT LABELED. */

/*<       IBGN = FIRST+1 >*/
    ibgn = first + 1;
/*<       DO 120 I = IBGN,N >*/
    i__1 = n;
    for (i__ = ibgn; i__ <= i__1; ++i__) {
/*<          IF (P(I).NE.0) GO TO 120 >*/
	if (p[i__] != 0) {
	    goto L120;
	}
/*<          FIRST = I >*/
	first = i__;
/*<          GO TO 20 >*/
	goto L20;
/*<   120 CONTINUE >*/
L120:
	;
    }

/* =================================================================== */

/* ... ALL NODES ARE NOW TYPED EITHER RED OR BLACK */

/* ... GENERATE PERMUTATION VECTORS */

/*<   130 NRED = 0 >*/
L130:
    nred = 0;
/*<       NBLACK = 0 >*/
    *nblack = 0;
/*<       DO 150 I = 1,N >*/
    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<          IF (P(I).LT.0) GO TO 140 >*/
	if (p[i__] < 0) {
	    goto L140;
	}

/*       RED POINT */

/*<          NRED = NRED+1 >*/
	++nred;
/*<          IP(NRED) = I >*/
	ip[nred] = i__;
/*<          P(I) = NRED >*/
	p[i__] = nred;
/*<          GO TO 150 >*/
	goto L150;

/*     BLACK POINT */

/*<   140    NBLACK = NBLACK+1 >*/
L140:
	++(*nblack);
/*<          J = N-NBLACK+1 >*/
	j = n - *nblack + 1;
/*<          IP(J) = I >*/
	ip[j] = i__;
/*<          P(I) = J >*/
	p[i__] = j;

/*<   150 CONTINUE >*/
L150:
	;
    }

/* ... SUCCESSFUL RED-BLACK ORDERING COMPLETED */

/*<       GO TO 180 >*/
    goto L180;

/* ........ ERROR TRAPS */

/* ...... N .LE. 0 */

/* 8000    IER = 200 */
/*        GO TO 9000 */

/* ...... TYPE CONFLICT */

/*<   160 IER = 201 >*/
L160:
    *ier = 201;
/*<       IF (LEVEL.GE.0) WRITE (NOUT,170) >*/
    if (*level >= 0) {
	io___585.ciunit = *nout;
	s_wsfe(&io___585);
	e_wsfe();
    }
/*<    >*/

/* ... RETURN */

/*<   180 CONTINUE >*/
L180:
/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* prbndx_ */

/*<       SUBROUTINE PRSBLK (NNB,NNR,IA,JA,A,UR,VB) >*/
/* Subroutine */ int prsblk_(integer *nnb, integer *nnr, integer *ia, integer 
	*ja, doublereal *a, doublereal *ur, doublereal *vb)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Local variables */
    integer i__, j, nb, nr, jaj, inr;
    doublereal uri, sum;
    integer ibgn, iend;


/* ... COMPUTE A BLACK-RS SWEEP ON A RED VECTOR INTO A BLACK VECTOR */

/* ... PARAMETER LIST: */

/*         NB      NUMBER OF BLACK POINTS (= NNB) */
/*         NR      NUMBER OF RED POINTS (= NNR) */
/*          IA,JA  INTEGER ARRAYS OF SPARSE MATRIX REPRESENTATION */
/*          A      D.P. ARRAY OF SPARSE MATRIX REPRESENTATION */
/*          UR     ESTIMATE OF RED SOLUTION VECTOR */
/*          VB     OUTPUT: PRESENT ESTIMATE OF BLACK SOLUTION */
/*                    VECTOR */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER IA(1),JA(1),NNB,NNR >*/
/*<       DOUBLE PRECISION A(1),UR(NNR),VB(NNB) >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER I,IBGN,IEND,INR,J,JAJ,NB,NR >*/
/*<       DOUBLE PRECISION SUM,URI >*/

/* *** BEGIN: ITPACK COMMON */

/*<       INTEGER IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/
/*<       COMMON /ITCOM1/ IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/

/*<       LOGICAL ADAPT,BETADT,CASEII,HALT,PARTAD >*/
/*<       COMMON /ITCOM2/ ADAPT,BETADT,CASEII,HALT,PARTAD >*/

/*<    >*/
/*<    >*/

/* *** END  : ITPACK COMMON */

/*     DESCRIPTION OF VARIABLES IN COMMON BLOCK IN MAIN SUBROUTINE */

/*<       NB = NNB >*/
    /* Parameter adjustments */
    --vb;
    --ur;
    --ia;
    --ja;
    --a;

    /* Function Body */
    nb = *nnb;
/*<       NR = NNR >*/
    nr = *nnr;
/*<       IF (ISYM.EQ.0) GO TO 30 >*/
    if (itcom1_1.isym == 0) {
	goto L30;
    }

/*     *************** NON - SYMMETRIC SECTION ********************** */

/*<       DO 20 I = 1,NB >*/
    i__1 = nb;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<          INR = I+NR >*/
	inr = i__ + nr;
/*<          IBGN = IA(INR) >*/
	ibgn = ia[inr];
/*<          IEND = IA(INR+1)-1 >*/
	iend = ia[inr + 1] - 1;
/*<          SUM = VB(I) >*/
	sum = vb[i__];
/*<          IF (IBGN.GT.IEND) GO TO 20 >*/
	if (ibgn > iend) {
	    goto L20;
	}
/*<          DO 10 J = IBGN,IEND >*/
	i__2 = iend;
	for (j = ibgn; j <= i__2; ++j) {
/*<             JAJ = JA(J) >*/
	    jaj = ja[j];
/*<             SUM = SUM-A(J)*UR(JAJ) >*/
	    sum -= a[j] * ur[jaj];
/*<    10    CONTINUE >*/
/* L10: */
	}
/*<          VB(I) = SUM >*/
	vb[i__] = sum;
/*<    20 CONTINUE >*/
L20:
	;
    }
/*<       RETURN >*/
    return 0;

/*     ***************** SYMMETRIC SECTION ************************** */

/*<    30 DO 50 I = 1,NR >*/
L30:
    i__1 = nr;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<          IBGN = IA(I) >*/
	ibgn = ia[i__];
/*<          IEND = IA(I+1)-1 >*/
	iend = ia[i__ + 1] - 1;
/*<          IF (IBGN.GT.IEND) GO TO 50 >*/
	if (ibgn > iend) {
	    goto L50;
	}
/*<          URI = UR(I) >*/
	uri = ur[i__];
/*<          DO 40 J = IBGN,IEND >*/
	i__2 = iend;
	for (j = ibgn; j <= i__2; ++j) {
/*<             JAJ = JA(J)-NR >*/
	    jaj = ja[j] - nr;
/*<             VB(JAJ) = VB(JAJ)-A(J)*URI >*/
	    vb[jaj] -= a[j] * uri;
/*<    40    CONTINUE >*/
/* L40: */
	}
/*<    50 CONTINUE >*/
L50:
	;
    }

/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* prsblk_ */

/*<       SUBROUTINE PRSRED (NNB,NNR,IA,JA,A,UB,VR) >*/
/* Subroutine */ int prsred_(integer *nnb, integer *nnr, integer *ia, integer 
	*ja, doublereal *a, doublereal *ub, doublereal *vr)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Local variables */
    integer nb, ii, jj, nr;
    doublereal sum;
    integer jajj, ibgn, iend;


/* ... COMPUTES A RED-RS SWEEP ON A BLACK VECTOR INTO A RED VECTOR. */

/* ... PARAMETER LIST: */

/*         NB      NUMBER OF BLACK POINTS (= NNR) */
/*         NR      NUMBER OF RED POINTS (= NNB) */
/*          IA,JA  INTEGER ARRAYS OF SPARSE MATRIX REPRESENTATION */
/*          A      D.P. ARRAY OF SPARSE MATRIX REPRESENTATION */
/*          UB     PRESENT ESTIMATE OF BLACK SOLUTION VECTOR */
/*          VR     OUTPUT: PRESENT ESTIMATE OF RED SOLUTION VECTOR */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER IA(1),JA(1),NNB,NNR >*/
/*<       DOUBLE PRECISION A(1),UB(NNB),VR(NNR) >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER IBGN,IEND,II,JAJJ,JJ,NB,NR >*/
/*<       DOUBLE PRECISION SUM >*/

/*<       NB = NNB >*/
    /* Parameter adjustments */
    --ub;
    --vr;
    --ia;
    --ja;
    --a;

    /* Function Body */
    nb = *nnb;
/*<       NR = NNR >*/
    nr = *nnr;
/*<       DO 20 II = 1,NR >*/
    i__1 = nr;
    for (ii = 1; ii <= i__1; ++ii) {
/*<          IBGN = IA(II) >*/
	ibgn = ia[ii];
/*<          IEND = IA(II+1)-1 >*/
	iend = ia[ii + 1] - 1;
/*<          IF (IBGN.GT.IEND) GO TO 20 >*/
	if (ibgn > iend) {
	    goto L20;
	}
/*<          SUM = VR(II) >*/
	sum = vr[ii];
/*<          DO 10 JJ = IBGN,IEND >*/
	i__2 = iend;
	for (jj = ibgn; jj <= i__2; ++jj) {
/*<             JAJJ = JA(JJ)-NR >*/
	    jajj = ja[jj] - nr;
/*<             SUM = SUM-A(JJ)*UB(JAJJ) >*/
	    sum -= a[jj] * ub[jajj];
/*<    10    CONTINUE >*/
/* L10: */
	}
/*<          VR(II) = SUM >*/
	vr[ii] = sum;
/*<    20 CONTINUE >*/
L20:
	;
    }

/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* prsred_ */

/*<       SUBROUTINE PSSOR1 (NN,IA,JA,A,U,RHS,FR,BR) >*/
/* Subroutine */ int pssor1_(integer *nn, integer *ia, integer *ja, 
	doublereal *a, doublereal *u, doublereal *rhs, doublereal *fr, 
	doublereal *br)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Local variables */
    integer i__, n, ii, jj;
    doublereal uii, sum, omm1;
    integer npl1, jajj, ibgn, iend;


/*     ... COMPUTES COMPLETE SSOR SWEEP ON U.  U IS OVERWRITTEN */
/*     ... WITH THE NEW ITERANT, FR AND BR WILL CONTAIN */
/*     ... THE FORWARD AND BACKWARD RESIDUALS ON OUTPUT. */

/* ... PARAMETER LIST: */

/*         N       ORDER OF SYSTEM (= NN) */
/*          OMEGA  RELAXATION FACTOR */
/*          IA,JA  INTEGER ARRAYS OF SPARSE MATRIX REPRESENTATION */
/*          A      D.P. ARRAY OF SPARSE MATRIX REPRESENTATION */
/*          U      ESTIMATE OF SOLUTION */
/*          RHS    RIGHT HAND SIDE OF MATRIX PROBLEM */
/*          FR,BR  OUTPUT: FORWARD AND BACKWARD RESIDUALS RESPECTIVELY */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER IA(1),JA(1),NN >*/
/*<       DOUBLE PRECISION A(1),U(NN),RHS(NN),FR(NN),BR(NN) >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER I,IBGN,IEND,II,JAJJ,JJ,N,NPL1 >*/
/*<       DOUBLE PRECISION OMM1,SUM,UII >*/

/* *** BEGIN: ITPACK COMMON */

/*<       INTEGER IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/
/*<       COMMON /ITCOM1/ IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/

/*<       LOGICAL ADAPT,BETADT,CASEII,HALT,PARTAD >*/
/*<       COMMON /ITCOM2/ ADAPT,BETADT,CASEII,HALT,PARTAD >*/

/*<    >*/
/*<    >*/

/* *** END  : ITPACK COMMON */

/*     DESCRIPTION OF VARIABLES IN COMMON BLOCK IN MAIN SUBROUTINE */

/*<       N = NN >*/
    /* Parameter adjustments */
    --br;
    --fr;
    --rhs;
    --u;
    --ia;
    --ja;
    --a;

    /* Function Body */
    n = *nn;
/*<       NPL1 = N+1 >*/
    npl1 = n + 1;
/*<       OMM1 = OMEGA-1.D0 >*/
    omm1 = itcom3_1.omega - 1.;
/*<       IF (ISYM.EQ.0) GO TO 40 >*/
    if (itcom1_1.isym == 0) {
	goto L40;
    }

/*     *************** NON - SYMMETRIC SECTION ********************** */

/*     ... FORWARD SWEEP */

/*<       DO 30 II = 1,N >*/
    i__1 = n;
    for (ii = 1; ii <= i__1; ++ii) {
/*<          BR(II) = U(II) >*/
	br[ii] = u[ii];
/*<          IBGN = IA(II) >*/
	ibgn = ia[ii];
/*<          IEND = IA(II+1)-1 >*/
	iend = ia[ii + 1] - 1;
/*<          SUM = RHS(II) >*/
	sum = rhs[ii];
/*<          IF (IBGN.GT.IEND) GO TO 20 >*/
	if (ibgn > iend) {
	    goto L20;
	}
/*<          DO 10 JJ = IBGN,IEND >*/
	i__2 = iend;
	for (jj = ibgn; jj <= i__2; ++jj) {
/*<             JAJJ = JA(JJ) >*/
	    jajj = ja[jj];
/*<             SUM = SUM-A(JJ)*U(JAJJ) >*/
	    sum -= a[jj] * u[jajj];
/*<    10    CONTINUE >*/
/* L10: */
	}
/*<    20    UII = OMEGA*SUM-OMM1*U(II) >*/
L20:
	uii = itcom3_1.omega * sum - omm1 * u[ii];
/*<          FR(II) = UII-U(II) >*/
	fr[ii] = uii - u[ii];
/*<          U(II) = UII >*/
	u[ii] = uii;
/*<    30 CONTINUE >*/
/* L30: */
    }
/*<       GO TO 90 >*/
    goto L90;

/*     ***************** SYMMETRIC SECTION ************************** */

/*     ... FORWARD SWEEP */

/*<    40 DO 80 II = 1,N >*/
L40:
    i__1 = n;
    for (ii = 1; ii <= i__1; ++ii) {
/*<          BR(II) = U(II) >*/
	br[ii] = u[ii];
/*<          IBGN = IA(II) >*/
	ibgn = ia[ii];
/*<          IEND = IA(II+1)-1 >*/
	iend = ia[ii + 1] - 1;
/*<          SUM = RHS(II) >*/
	sum = rhs[ii];
/*<          IF (IBGN.GT.IEND) GO TO 60 >*/
	if (ibgn > iend) {
	    goto L60;
	}
/*<          DO 50 JJ = IBGN,IEND >*/
	i__2 = iend;
	for (jj = ibgn; jj <= i__2; ++jj) {
/*<             JAJJ = JA(JJ) >*/
	    jajj = ja[jj];
/*<             SUM = SUM-A(JJ)*U(JAJJ) >*/
	    sum -= a[jj] * u[jajj];
/*<    50    CONTINUE >*/
/* L50: */
	}
/*<    60    UII = OMEGA*SUM-OMM1*U(II) >*/
L60:
	uii = itcom3_1.omega * sum - omm1 * u[ii];
/*<          FR(II) = UII-U(II) >*/
	fr[ii] = uii - u[ii];
/*<          U(II) = UII >*/
	u[ii] = uii;
/*<          IF (IBGN.GT.IEND) GO TO 80 >*/
	if (ibgn > iend) {
	    goto L80;
	}
/*<          DO 70 JJ = IBGN,IEND >*/
	i__2 = iend;
	for (jj = ibgn; jj <= i__2; ++jj) {
/*<             JAJJ = JA(JJ) >*/
	    jajj = ja[jj];
/*<             RHS(JAJJ) = RHS(JAJJ)-A(JJ)*UII >*/
	    rhs[jajj] -= a[jj] * uii;
/*<    70    CONTINUE >*/
/* L70: */
	}
/*<    80 CONTINUE >*/
L80:
	;
    }

/*     ... BACKWARD SWEEP */

/*<    90 DO 120 I = 1,N >*/
L90:
    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<          II = NPL1-I >*/
	ii = npl1 - i__;
/*<          IBGN = IA(II) >*/
	ibgn = ia[ii];
/*<          IEND = IA(II+1)-1 >*/
	iend = ia[ii + 1] - 1;
/*<          UII = RHS(II) >*/
	uii = rhs[ii];
/*<          IF (IBGN.GT.IEND) GO TO 110 >*/
	if (ibgn > iend) {
	    goto L110;
	}
/*<          DO 100 JJ = IBGN,IEND >*/
	i__2 = iend;
	for (jj = ibgn; jj <= i__2; ++jj) {
/*<             JAJJ = JA(JJ) >*/
	    jajj = ja[jj];
/*<             UII = UII-A(JJ)*U(JAJJ) >*/
	    uii -= a[jj] * u[jajj];
/*<   100    CONTINUE >*/
/* L100: */
	}
/*<   110    U(II) = OMEGA*UII-OMM1*U(II) >*/
L110:
	u[ii] = itcom3_1.omega * uii - omm1 * u[ii];
/*<          BR(II) = U(II)-BR(II) >*/
	br[ii] = u[ii] - br[ii];
/*<   120 CONTINUE >*/
/* L120: */
    }

/*<       RETURN >*/
    return 0;

/*<       END >*/
} /* pssor1_ */

/*<       SUBROUTINE PSTOP (N,U,DNRM,CCON,IFLAG,Q1) >*/
/* Subroutine */ int pstop_(integer *n, doublereal *u, doublereal *dnrm, 
	doublereal *ccon, integer *iflag, logical *q1)
{
    /* System generated locals */
    doublereal d__1;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    doublereal tl, tr, con;
    extern doublereal ddot_(integer *, doublereal *, integer *, doublereal *, 
	    integer *);
    doublereal uold;


/*     THIS SUBROUTINE PERFORMS A TEST TO SEE IF THE ITERATIVE */
/*     METHOD HAS CONVERGED TO A SOLUTION INSIDE THE ERROR */
/*     TOLERANCE, ZETA. */

/* ... PARAMETER LIST: */

/*          N      ORDER OF SYSTEM */
/*          U      PRESENT SOLUTION ESTIMATE */
/*          DNRM   INNER PRODUCT OF PSEUDO-RESIDUALS AT PRECEDING */
/*                    ITERATION */
/*          CON    STOPPING TEST PARAMETER (= CCON) */
/*          IFLAG  STOPPING TEST INTEGER FLAG */
/*                    IFLAG = 0,  SOR ITERATION ZERO */
/*                    IFLAG = 1,  NON-RS METHOD */
/*                    IFLAG = 2,  RS METHOD */
/*          Q1     STOPPING TEST LOGICAL FLAG */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER N,IFLAG >*/
/*<       DOUBLE PRECISION U(N),DNRM,CCON >*/
/*<       LOGICAL Q1 >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       DOUBLE PRECISION CON,TL,TR,UOLD >*/

/* ... SPECIFICATIONS FOR ARGUMENT SUBROUTINES */

/*<       DOUBLE PRECISION DDOT >*/

/* *** BEGIN: ITPACK COMMON */

/*<       INTEGER IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/
/*<       COMMON /ITCOM1/ IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/

/*<       LOGICAL ADAPT,BETADT,CASEII,HALT,PARTAD >*/
/*<       COMMON /ITCOM2/ ADAPT,BETADT,CASEII,HALT,PARTAD >*/

/*<    >*/
/*<    >*/

/* *** END  : ITPACK COMMON */

/*     DESCRIPTION OF VARIABLES IN COMMON BLOCK IN MAIN SUBROUTINE */

/*<       CON = CCON >*/
    /* Parameter adjustments */
    --u;

    /* Function Body */
    con = *ccon;
/*<       HALT = .FALSE. >*/
    itcom2_1.halt = FALSE_;

/*     SPECIAL PROCEDURE FOR ZEROTH ITERATION */

/*<       IF (IN.GE.1) GO TO 10 >*/
    if (itcom1_1.in >= 1) {
	goto L10;
    }
/*<       Q1 = .FALSE. >*/
    *q1 = FALSE_;
/*<       UDNM = 1.D0 >*/
    itcom3_1.udnm = 1.;
/*<       STPTST = 1.D3 >*/
    itcom3_1.stptst = 1e3;
/*<       IF (IFLAG.LE.0) RETURN >*/
    if (*iflag <= 0) {
	return 0;
    }

/* ... TEST IF UDNM NEEDS TO BE RECOMPUTED */

/*<    10 CONTINUE >*/
L10:
/*<       IF (Q1) GO TO 20 >*/
    if (*q1) {
	goto L20;
    }
/*<       IF ((IN.GT.5).AND.(MOD(IN,5).NE.0)) GO TO 20 >*/
    if (itcom1_1.in > 5 && itcom1_1.in % 5 != 0) {
	goto L20;
    }
/*<       UOLD = UDNM >*/
    uold = itcom3_1.udnm;
/*<       UDNM = DDOT(N,U,1,U,1) >*/
    itcom3_1.udnm = ddot_(n, &u[1], &c__1, &u[1], &c__1);
/*<       IF (UDNM.EQ.0.D0) UDNM = 1.D0 >*/
    if (itcom3_1.udnm == 0.) {
	itcom3_1.udnm = 1.;
    }
/*<       IF ((IN.GT.5).AND.(DABS(UDNM-UOLD).LE.UDNM*ZETA)) Q1 = .TRUE. >*/
    if (itcom1_1.in > 5 && (d__1 = itcom3_1.udnm - uold, abs(d__1)) <= 
	    itcom3_1.udnm * itcom3_1.zeta) {
	*q1 = TRUE_;
    }

/* ... COMPUTE STOPPING TEST */

/*<    20 TR = DSQRT(UDNM) >*/
L20:
    tr = sqrt(itcom3_1.udnm);
/*<       TL = 1.D0 >*/
    tl = 1.;
/*<       IF (CON.EQ.1.D0) GO TO 40 >*/
    if (con == 1.) {
	goto L40;
    }
/*<       IF (IFLAG.EQ.2) GO TO 30 >*/
    if (*iflag == 2) {
	goto L30;
    }
/*<       TL = DSQRT(DNRM) >*/
    tl = sqrt(*dnrm);
/*<       TR = TR*(1.D0-CON) >*/
    tr *= 1. - con;
/*<       GO TO 40 >*/
    goto L40;
/*<    30 TL = DSQRT(2.D0*DNRM) >*/
L30:
    tl = sqrt(*dnrm * 2.);
/*<       TR = TR*(1.D0-CON*CON) >*/
    tr *= 1. - con * con;
/*<    40 STPTST = TL/TR >*/
L40:
    itcom3_1.stptst = tl / tr;
/*<       IF (TL.GE.TR*ZETA) RETURN >*/
    if (tl >= tr * itcom3_1.zeta) {
	return 0;
    }
/*<       HALT = .TRUE. >*/
    itcom2_1.halt = TRUE_;

/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* pstop_ */

/*<       DOUBLE PRECISION FUNCTION PVTBV (N,IA,JA,A,V) >*/
doublereal pvtbv_(integer *n, integer *ia, integer *ja, doublereal *a, 
	doublereal *v)
{
    /* System generated locals */
    integer i__1, i__2;
    doublereal ret_val;

    /* Local variables */
    integer ii, jj;
    doublereal sum;
    integer jajj, ibgn, iend;
    doublereal sumr;


/*     THIS FUNCTION COMPUTES  (V**T)*A*V. */

/* ... PARAMETER LIST: */

/*          N      DIMENSION OF MATRIX */
/*          IA,JA  INTEGER ARRAYS OF SPARSE MATRIX REPRESENTATION */
/*          A      D.P. ARRAY OF SPARSE MATRIX REPRESENTATION */
/*          V      D.P. VECTOR OF LENGTH N */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER IA(1),JA(1),N >*/
/*<       DOUBLE PRECISION A(1),V(N) >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER IBGN,IEND,II,JAJJ,JJ >*/
/*<       DOUBLE PRECISION SUM,SUMR >*/

/* *** BEGIN: ITPACK COMMON */

/*<       INTEGER IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/
/*<       COMMON /ITCOM1/ IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/

/*<       LOGICAL ADAPT,BETADT,CASEII,HALT,PARTAD >*/
/*<       COMMON /ITCOM2/ ADAPT,BETADT,CASEII,HALT,PARTAD >*/

/*<    >*/
/*<    >*/

/* *** END  : ITPACK COMMON */

/*     DESCRIPTION OF VARIABLES IN COMMON BLOCK IN MAIN SUBROUTINE */

/*<       PVTBV = 0.D0 >*/
    /* Parameter adjustments */
    --v;
    --ia;
    --ja;
    --a;

    /* Function Body */
    ret_val = 0.;
/*<       SUM = 0.D0 >*/
    sum = 0.;
/*<       DO 20 II = 1,N >*/
    i__1 = *n;
    for (ii = 1; ii <= i__1; ++ii) {
/*<          IBGN = IA(II) >*/
	ibgn = ia[ii];
/*<          IEND = IA(II+1)-1 >*/
	iend = ia[ii + 1] - 1;
/*<          IF (IBGN.GT.IEND) GO TO 20 >*/
	if (ibgn > iend) {
	    goto L20;
	}
/*<          SUMR = 0.D0 >*/
	sumr = 0.;
/*<          DO 10 JJ = IBGN,IEND >*/
	i__2 = iend;
	for (jj = ibgn; jj <= i__2; ++jj) {
/*<             JAJJ = JA(JJ) >*/
	    jajj = ja[jj];
/*<             SUMR = SUMR-A(JJ)*V(JAJJ) >*/
	    sumr -= a[jj] * v[jajj];
/*<    10    CONTINUE >*/
/* L10: */
	}
/*<          SUM = SUM+V(II)*SUMR >*/
	sum += v[ii] * sumr;
/*<    20 CONTINUE >*/
L20:
	;
    }

/*<       IF (ISYM.EQ.0) SUM = 2.D0*SUM >*/
    if (itcom1_1.isym == 0) {
	sum *= 2.;
    }
/*<       PVTBV = SUM >*/
    ret_val = sum;

/*<       RETURN >*/
    return ret_val;
/*<       END >*/
} /* pvtbv_ */

/*<       SUBROUTINE QSORT (NN,KEY,DATA,ERROR) >*/
/* Subroutine */ int qsort_(integer *nn, integer *key, doublereal *data, 
	integer *error)
{
    /* Initialized data */

    static integer tiny = 9;
    static integer stklen = 30;

    /* System generated locals */
    integer i__1, i__2;

    /* Local variables */
    doublereal d__;
    integer i__, j, k, n, v, jm1, ip1, top;
    logical done;
    integer left, llen, rlen, lfrh2, stack[30], right;
    extern /* Subroutine */ int ivfill_(integer *, integer *, integer *);


/*     ================================================================== */

/*     Q U I C K S O R T */

/*         IN THE STYLE OF THE CACM PAPER BY BOB SEDGEWICK, OCTOBER 1978 */

/*     INPUT: */
/*         N    -- NUMBER OF ELEMENTS TO BE SORTED (= NN) */
/*         KEY  -- AN ARRAY OF LENGTH  N  CONTAINING THE VALUES */
/*                 WHICH ARE TO BE SORTED */
/*         DATA -- A SECOND ARRAY OF LENGTH  N  CONTAINING DATA */
/*                 ASSOCIATED WITH THE INDIVIDUAL KEYS. */

/*     OUTPUT: */
/*         KEY  -- WILL BE ARRANGED SO THAT VALUES ARE IN INCREASING */
/*                 ORDER */
/*         DATA -- REARRANGED TO CORRESPOND TO REARRANGED KEYS */
/*         ERROR -- WILL BE ZERO UNLESS YOUR INPUT FILE WAS OF TRULY */
/*                  ENORMOUS LENGTH, IN WHICH CASE IT WILL BE EQUAL TO 1. */

/*     ================================================================== */

/*<       INTEGER NN,ERROR,KEY(NN) >*/
/*<       DOUBLE PRECISION DATA(NN) >*/

/*     ------------------------ */

/*<       INTEGER TOP,LEFT,RIGHT,I,J,TINY,V,K,IP1,JM1,LLEN,RLEN,N >*/
/*<       LOGICAL DONE >*/
/*<       DOUBLE PRECISION D >*/
/*<       INTEGER STKLEN,STACK(30) >*/

/*<       DATA TINY,STKLEN / 9,30 / >*/
    /* Parameter adjustments */
    --data;
    --key;

    /* Function Body */

/*     ----------------------------------- */

/*     ... PROGRAM IS A DIRECT TRANSLATION INTO FORTRAN OF SEDGEWICK^S */
/*         PROGRAM 2, WHICH IS NON-RECURSIVE, IGNORES FILES OF LENGTH */
/*         LESS THAN 'TINY' DURING PARTITIONING, AND USES MEDIAN OF THREE */
/*         PARTITIONING. */

/*<       N = NN >*/
    n = *nn;
/*<       IF (N.EQ.1) RETURN >*/
    if (n == 1) {
	return 0;
    }
/*<       IF (N.LE.0) GO TO 240 >*/
    if (n <= 0) {
	goto L240;
    }

/*<       ERROR = 0 >*/
    *error = 0;
/*<       TOP = 1 >*/
    top = 1;
/*<       LEFT = 1 >*/
    left = 1;
/*<       RIGHT = N >*/
    right = n;
/*<       DONE = (N.LE.TINY) >*/
    done = n <= tiny;

/*<       IF (DONE) GO TO 150 >*/
    if (done) {
	goto L150;
    }
/*<       CALL IVFILL (STKLEN,STACK,0) >*/
    ivfill_(&stklen, stack, &c__0);

/*     =========================================================== */
/*     QUICKSORT -- PARTITION THE FILE UNTIL NO SUBFILE REMAINS OF */
/*     LENGTH GREATER THAN 'TINY' */
/*     =========================================================== */

/*     ... WHILE NOT DONE DO ... */

/*<    10 IF (DONE) GO TO 150 >*/
L10:
    if (done) {
	goto L150;
    }

/*         ... FIND MEDIAN OF LEFT, RIGHT AND MIDDLE ELEMENTS OF CURRENT */
/*             SUBFILE, WHICH IS  KEY(LEFT), ..., KEY(RIGHT) */

/*<       LFRH2 = (LEFT+RIGHT)/2 >*/
    lfrh2 = (left + right) / 2;
/*<       K = KEY(LFRH2) >*/
    k = key[lfrh2];
/*<       D = DATA(LFRH2) >*/
    d__ = data[lfrh2];
/*<       KEY(LFRH2) = KEY(LEFT) >*/
    key[lfrh2] = key[left];
/*<       DATA(LFRH2) = DATA(LEFT) >*/
    data[lfrh2] = data[left];
/*<       KEY(LEFT) = K >*/
    key[left] = k;
/*<       DATA(LEFT) = D >*/
    data[left] = d__;

/*<       IF (KEY(LEFT+1).LE.KEY(RIGHT)) GO TO 20 >*/
    if (key[left + 1] <= key[right]) {
	goto L20;
    }
/*<       K = KEY(LEFT+1) >*/
    k = key[left + 1];
/*<       D = DATA(LEFT+1) >*/
    d__ = data[left + 1];
/*<       KEY(LEFT+1) = KEY(RIGHT) >*/
    key[left + 1] = key[right];
/*<       DATA(LEFT+1) = DATA(RIGHT) >*/
    data[left + 1] = data[right];
/*<       KEY(RIGHT) = K >*/
    key[right] = k;
/*<       DATA(RIGHT) = D >*/
    data[right] = d__;

/*<    20 IF (KEY(LEFT).LE.KEY(RIGHT)) GO TO 30 >*/
L20:
    if (key[left] <= key[right]) {
	goto L30;
    }
/*<       K = KEY(LEFT) >*/
    k = key[left];
/*<       D = DATA(LEFT) >*/
    d__ = data[left];
/*<       KEY(LEFT) = KEY(RIGHT) >*/
    key[left] = key[right];
/*<       DATA(LEFT) = DATA(RIGHT) >*/
    data[left] = data[right];
/*<       KEY(RIGHT) = K >*/
    key[right] = k;
/*<       DATA(RIGHT) = D >*/
    data[right] = d__;

/*<    30 IF (KEY(LEFT+1).LE.KEY(LEFT)) GO TO 40 >*/
L30:
    if (key[left + 1] <= key[left]) {
	goto L40;
    }
/*<       K = KEY(LEFT+1) >*/
    k = key[left + 1];
/*<       D = DATA(LEFT+1) >*/
    d__ = data[left + 1];
/*<       KEY(LEFT+1) = KEY(LEFT) >*/
    key[left + 1] = key[left];
/*<       DATA(LEFT+1) = DATA(LEFT) >*/
    data[left + 1] = data[left];
/*<       KEY(LEFT) = K >*/
    key[left] = k;
/*<       DATA(LEFT) = D >*/
    data[left] = d__;

/*<    40 V = KEY(LEFT) >*/
L40:
    v = key[left];

/*         ... V IS NOW THE MEDIAN VALUE OF THE THREE KEYS.  NOW MOVE */
/*             FROM THE LEFT AND RIGHT ENDS SIMULTANEOUSLY, EXCHANGING */
/*             KEYS AND DATA UNTIL ALL KEYS LESS THAN  V  ARE PACKED TO */
/*             THE LEFT, ALL KEYS LARGER THAN  V  ARE PACKED TO THE */
/*             RIGHT. */

/*<       I = LEFT+1 >*/
    i__ = left + 1;
/*<       J = RIGHT >*/
    j = right;

/*         LOOP */
/*             REPEAT I = I+1 UNTIL KEY(I) >= V; */
/*             REPEAT J = J-1 UNTIL KEY(J) <= V; */
/*         EXIT IF J < I; */
/*             << EXCHANGE KEYS I AND J >> */
/*         END */

/*<    50 CONTINUE >*/
L50:
/*<    60 I = I+1 >*/
L60:
    ++i__;
/*<       IF (KEY(I).LT.V) GO TO 60 >*/
    if (key[i__] < v) {
	goto L60;
    }

/*<    70 J = J-1 >*/
L70:
    --j;
/*<       IF (KEY(J).GT.V) GO TO 70 >*/
    if (key[j] > v) {
	goto L70;
    }

/*<       IF (J.LT.I) GO TO 80 >*/
    if (j < i__) {
	goto L80;
    }
/*<       K = KEY(I) >*/
    k = key[i__];
/*<       D = DATA(I) >*/
    d__ = data[i__];
/*<       KEY(I) = KEY(J) >*/
    key[i__] = key[j];
/*<       DATA(I) = DATA(J) >*/
    data[i__] = data[j];
/*<       KEY(J) = K >*/
    key[j] = k;
/*<       DATA(J) = D >*/
    data[j] = d__;
/*<       GO TO 50 >*/
    goto L50;

/*<    80 K = KEY(LEFT) >*/
L80:
    k = key[left];
/*<       D = DATA(LEFT) >*/
    d__ = data[left];
/*<       KEY(LEFT) = KEY(J) >*/
    key[left] = key[j];
/*<       DATA(LEFT) = DATA(J) >*/
    data[left] = data[j];
/*<       KEY(J) = K >*/
    key[j] = k;
/*<       DATA(J) = D >*/
    data[j] = d__;

/*         ... WE HAVE NOW PARTITIONED THE FILE INTO TWO SUBFILES, */
/*             ONE IS (LEFT ... J-1)  AND THE OTHER IS (I...RIGHT). */
/*             PROCESS THE SMALLER NEXT.  STACK THE LARGER ONE. */

/*<       LLEN = J-LEFT >*/
    llen = j - left;
/*<       RLEN = RIGHT-I+1 >*/
    rlen = right - i__ + 1;
/*<       IF (MAX0(LLEN,RLEN).GT.TINY) GO TO 100 >*/
    if (max(llen,rlen) > tiny) {
	goto L100;
    }

/*             ... BOTH SUBFILES ARE TINY, SO UNSTACK NEXT LARGER FILE */

/*<       IF (TOP.EQ.1) GO TO 90 >*/
    if (top == 1) {
	goto L90;
    }
/*<       TOP = TOP-2 >*/
    top += -2;
/*<       LEFT = STACK(TOP) >*/
    left = stack[top - 1];
/*<       RIGHT = STACK(TOP+1) >*/
    right = stack[top];
/*<       GO TO 10 >*/
    goto L10;

/*<    90 DONE = .TRUE. >*/
L90:
    done = TRUE_;

/*<       GO TO 10 >*/
    goto L10;

/*             ... ELSE ONE OR BOTH SUBFILES ARE LARGE */

/*<   100 IF (MIN0(LLEN,RLEN).GT.TINY) GO TO 120 >*/
L100:
    if (min(llen,rlen) > tiny) {
	goto L120;
    }

/*             ... ONE SUBFILE IS SMALL, ONE LARGE.  IGNORE THE SMALL ONE */

/*<       IF (LLEN.GT.RLEN) GO TO 110 >*/
    if (llen > rlen) {
	goto L110;
    }
/*<       LEFT = I >*/
    left = i__;
/*<       GO TO 10 >*/
    goto L10;

/*<   110 RIGHT = J-1 >*/
L110:
    right = j - 1;

/*<       GO TO 10 >*/
    goto L10;

/*         ... ELSE BOTH ARE LARGER THAN TINY.  ONE MUST BE STACKED. */

/*<   120 IF (TOP.GE.STKLEN) GO TO 240 >*/
L120:
    if (top >= stklen) {
	goto L240;
    }
/*<       IF (LLEN.GT.RLEN) GO TO 130 >*/
    if (llen > rlen) {
	goto L130;
    }
/*<       STACK(TOP) = I >*/
    stack[top - 1] = i__;
/*<       STACK(TOP+1) = RIGHT >*/
    stack[top] = right;
/*<       RIGHT = J-1 >*/
    right = j - 1;
/*<       GO TO 140 >*/
    goto L140;

/*<   130 STACK(TOP) = LEFT >*/
L130:
    stack[top - 1] = left;
/*<       STACK(TOP+1) = J-1 >*/
    stack[top] = j - 1;
/*<       LEFT = I >*/
    left = i__;

/*<   140 TOP = TOP+2 >*/
L140:
    top += 2;

/*<       GO TO 10 >*/
    goto L10;

/*     ------------------------------------------------------------ */
/*     INSERTION SORT THE ENTIRE FILE, WHICH CONSISTS OF A LIST */
/*     OF 'TINY' SUBFILES, LOCALLY OUT OF ORDER, GLOBALLY IN ORDER. */
/*     ------------------------------------------------------------ */

/*     ... FIRST, FIND LARGEST ELEMENT IN 'KEY' */

/*<   150 I = N-1 >*/
L150:
    i__ = n - 1;
/*<       LEFT = MAX0(0,N-TINY) >*/
/* Computing MAX */
    i__1 = 0, i__2 = n - tiny;
    left = max(i__1,i__2);
/*<       K = KEY(N) >*/
    k = key[n];
/*<       J = N >*/
    j = n;

/*<   160 IF (I.LE.LEFT) GO TO 180 >*/
L160:
    if (i__ <= left) {
	goto L180;
    }
/*<       IF (KEY(I).LE.K) GO TO 170 >*/
    if (key[i__] <= k) {
	goto L170;
    }
/*<       K = KEY(I) >*/
    k = key[i__];
/*<       J = I >*/
    j = i__;

/*<   170 I = I-1 >*/
L170:
    --i__;
/*<       GO TO 160 >*/
    goto L160;

/*<   180 IF (J.EQ.N) GO TO 190 >*/
L180:
    if (j == n) {
	goto L190;
    }

/*     ... LARGEST ELEMENT WILL BE IN  KEY(N) */

/*<       KEY(J) = KEY(N) >*/
    key[j] = key[n];
/*<       KEY(N) = K >*/
    key[n] = k;
/*<       D = DATA(N) >*/
    d__ = data[n];
/*<       DATA(N) = DATA(J) >*/
    data[n] = data[j];
/*<       DATA(J) = D >*/
    data[j] = d__;

/*     ... INSERTION SORT ... FOR I := N-1 STEP -1 TO 1 DO ... */

/*<   190 I = N-1 >*/
L190:
    i__ = n - 1;
/*<       IP1 = N >*/
    ip1 = n;

/*<   200 IF (KEY(I).LE.KEY(IP1)) GO TO 220 >*/
L200:
    if (key[i__] <= key[ip1]) {
	goto L220;
    }

/*             ... OUT OF ORDER ... MOVE UP TO CORRECT PLACE */

/*<       K = KEY(I) >*/
    k = key[i__];
/*<       D = DATA(I) >*/
    d__ = data[i__];
/*<       J = IP1 >*/
    j = ip1;
/*<       JM1 = I >*/
    jm1 = i__;

/*             ... REPEAT ... UNTIL 'CORRECT PLACE FOR K FOUND' */

/*<   210 KEY(JM1) = KEY(J) >*/
L210:
    key[jm1] = key[j];
/*<       DATA(JM1) = DATA(J) >*/
    data[jm1] = data[j];
/*<       JM1 = J >*/
    jm1 = j;
/*<       J = J+1 >*/
    ++j;
/*<       IF (KEY(J).LT.K) GO TO 210 >*/
    if (key[j] < k) {
	goto L210;
    }

/*<       KEY(JM1) = K >*/
    key[jm1] = k;
/*<       DATA(JM1) = D >*/
    data[jm1] = d__;

/*<   220 IP1 = I >*/
L220:
    ip1 = i__;
/*<       I = I-1 >*/
    --i__;
/*<       IF (I.GT.0) GO TO 200 >*/
    if (i__ > 0) {
	goto L200;
    }

/*<   230 RETURN >*/
L230:
    return 0;

/*<   240 ERROR = 1 >*/
L240:
    *error = 1;
/*<       GO TO 230 >*/
    goto L230;

/*<       END >*/
} /* qsort_ */

/*<       SUBROUTINE SBAGN (N,NZ,IA,JA,A,IWORK,LEVELL,NOUTT,IERR) >*/
/* Subroutine */ int sbagn_(integer *n, integer *nz, integer *ia, integer *ja,
	 doublereal *a, integer *iwork, integer *levell, integer *noutt, 
	integer *ierr)
{
    /* Format strings */
    static char fmt_10[] = "(\0020\002,\002*** F A T A L     E R R O R *****\
*******\002/\0020\002,\002    IN ITPACK ROUTINE SBAGN   \002/\002 \002,\002 \
   IER = \002,i10/\002 \002,\002    NZ TOO SMALL - NO ROOM FOR NEW ENTRY\002)"
	    ;

    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    integer i__, j, np1, ier, ntn, nto, now, nadd, nout, level, naddp1;

    /* Fortran I/O blocks */
    static cilist io___649 = { 0, 0, 0, fmt_10, 0 };



/* ... THE ROUTINES SBINI, SBSIJ, AND SBEND CREATE A SPARSE */
/*     MATRIX STRUCTURE BY MEANS OF A LINKED LIST WHICH IS */
/*     DESTROYED BY SBEND. SBAGN CREATES A NEW LINKED LIST */
/*     SO THAT ELEMENTS MAY BE ADDED TO THE MATRIX AFTER SBEND */
/*     HAS BEEN CALLED. SBAGN SHOULD BE CALLED WITH THE APPRO- */
/*     PRIATE PARAMETERS, AND THEN SBSIJ AND SBEND CAN BE CALLED */
/*     TO ADD THE ELEMENTS AND COMPLETE THE SPARSE MATRIX STRUC- */
/*     TURE. */

/* ... PARAMETER LIST: */

/*           N       ORDER OF THE SYSTEM */
/*           NZ      MAXIMUM NUMBER OF NON-ZERO ELEMENTS */
/*                   IN THE SYSTEM */
/*           IA, JA  INTEGER ARRAYS OF THE SPARSE */
/*                   MATRIX STRUCTURE */
/*           A       D.P. ARRAY OF THE SPARSE MATRIX */
/*                   STRUCTURE */
/*           IWORK   WORK ARRAY OF DIMENSION NZ */
/*           LEVEL   OUTPUT LEVEL CONTROL (= LEVELL) */
/*           NOUT  OUTPUT FILE NUMBER (= NOUTT) */
/*           IER     ERROR FLAG (= IERR). POSSIBLE RETURNS ARE */
/*                      IER = 0, SUCCESSFUL COMPLETION */
/*                          = 703, NZ TOO SMALL - NO MORE */
/*                                 ELEMENTS CAN BE ADDED */

/* ... SPECIFICTIONS FOR ARGUMENTS */

/*<       INTEGER NZ,IA(1),JA(1),IWORK(NZ),N,LEVELL,NOUTT,IERR >*/
/*<       DOUBLE PRECISION A(NZ) >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER I,IER,J,LEVEL,NOUT,NADD,NADDP1,NOW,NP1,NTO,NTN >*/

/* ... INITIALIZE LOCAL VARIABLES AND MAKE ERROR CHECK */

/*<       NOW = IA(N+1)-1 >*/
    /* Parameter adjustments */
    --iwork;
    --a;
    --ia;
    --ja;

    /* Function Body */
    now = ia[*n + 1] - 1;
/*<       NADD = NZ-NOW >*/
    nadd = *nz - now;
/*<       IER = 0 >*/
    ier = 0;
/*<       LEVEL = LEVELL >*/
    level = *levell;
/*<       NOUT = NOUTT >*/
    nout = *noutt;
/*<       IF (NADD.LE.0) IER = 703 >*/
    if (nadd <= 0) {
	ier = 703;
    }
/*<       IF (IER.EQ.0) GO TO 20 >*/
    if (ier == 0) {
	goto L20;
    }
/*<       IF (LEVEL.GE.0) WRITE (NOUT,10) IER >*/
    if (level >= 0) {
	io___649.ciunit = nout;
	s_wsfe(&io___649);
	do_fio(&c__1, (char *)&ier, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 90 >*/
    goto L90;

/* ... SHIFT ELEMENTS OF A AND JA DOWN AND ADD ZERO FILL */

/*<    20 NTO = NOW >*/
L20:
    nto = now;
/*<       NTN = NZ >*/
    ntn = *nz;
/*<       DO 30 I = 1,NOW >*/
    i__1 = now;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<          JA(NTN) = JA(NTO) >*/
	ja[ntn] = ja[nto];
/*<          A(NTN) = A(NTO) >*/
	a[ntn] = a[nto];
/*<          NTO = NTO-1 >*/
	--nto;
/*<          NTN = NTN-1 >*/
	--ntn;
/*<    30 CONTINUE >*/
/* L30: */
    }
/*<       DO 40 I = 1,NADD >*/
    i__1 = nadd;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<          JA(I) = 0 >*/
	ja[i__] = 0;
/*<          A(I) = 0.D0 >*/
	a[i__] = 0.;
/*<    40 CONTINUE >*/
/* L40: */
    }

/* ... UPDATE IA TO REFLECT DOWNWARD SHIFT IN A AND JA */

/*<       NP1 = N+1 >*/
    np1 = *n + 1;
/*<       DO 50 I = 1,NP1 >*/
    i__1 = np1;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<          IA(I) = IA(I)+NADD >*/
	ia[i__] += nadd;
/*<    50 CONTINUE >*/
/* L50: */
    }

/* ... CREATE LINKED LIST */

/*<       NADDP1 = NADD+1 >*/
    naddp1 = nadd + 1;
/*<       DO 60 I = NADDP1,NZ >*/
    i__1 = *nz;
    for (i__ = naddp1; i__ <= i__1; ++i__) {
/*<          IWORK(I) = I+1 >*/
	iwork[i__] = i__ + 1;
/*<    60 CONTINUE >*/
/* L60: */
    }
/*<       DO 70 I = 1,NADD >*/
    i__1 = nadd;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<          IWORK(I) = 0 >*/
	iwork[i__] = 0;
/*<    70 CONTINUE >*/
/* L70: */
    }
/*<       DO 80 I = 1,N >*/
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<          J = IA(I+1)-1 >*/
	j = ia[i__ + 1] - 1;
/*<          IWORK(J) = -I >*/
	iwork[j] = -i__;
/*<    80 CONTINUE >*/
/* L80: */
    }

/* ... INDICATE IN LAST POSITION OF IA HOW MANY SPACES */
/*     ARE LEFT IN A AND JA FOR ADDITION OF ELEMENTS */

/*<       IA(N+1) = NADD >*/
    ia[*n + 1] = nadd;
/*<       RETURN >*/
    return 0;

/* ... ERROR RETURN */

/*<    90 IERR = IER >*/
L90:
    *ierr = ier;
/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* sbagn_ */

/*<       SUBROUTINE SBELM (NN,IA,JA,A,RHS,IW,RW,TOL,ISYM,LEVEL,NOUT,IER) >*/
/* Subroutine */ int sbelm_(integer *nn, integer *ia, integer *ja, doublereal 
	*a, doublereal *rhs, integer *iw, doublereal *rw, doublereal *tol, 
	integer *isym, integer *level, integer *nout, integer *ier)
{
    /* Format strings */
    static char fmt_30[] = "(\0020\002,\002*** F A T A L     E R R O R *****\
*******\002/\0020\002,\002    IN ITPACK ROUTINE SBELM   \002/\002 \002,\002 \
   DIAGONAL ELEMENT\002,i10,\002 NOT POSITIVE  \002/\002 \002,\002    CURREN\
T VALUE = \002,d15.8)";
    static char fmt_150[] = "(\0020\002,\002*** F A T A L     E R R O R ****\
********\002/\0020\002,\002    IN ITPACK ROUTINE SBELM   \002/\002 \002,\002\
    NO DIAGONAL ENTRY IN ROW  \002,i10)";

    /* System generated locals */
    integer i__1, i__2;
    doublereal d__1, d__2, d__3;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    integer n;
    doublereal di;
    integer ii, jj, kk, ibgn, iend, jjdi, icnt;

    /* Fortran I/O blocks */
    static cilist io___665 = { 0, 0, 0, fmt_30, 0 };
    static cilist io___666 = { 0, 0, 0, fmt_150, 0 };



/* ... SBELM IS DESIGNED TO REMOVE ROWS AND COLUMNS OF THE MATRIX */
/* ... WHERE DABS(A(I,J))/A(I,I) .LE. TOL FOR J = 1 TO N AND A(I,I) */
/* ... .GT. 0. THIS IS TO TAKE CARE OF MATRICES ARISING */
/* ... FROM FINITE ELEMENT DISCRETIZATIONS OF PDE^S WITH DIRICHLET */
/* ... BOUNDARY CONDITIONS.  ANY SUCH ROWS AND CORRESPONDING COLUMNS */
/* ... ARE THEN SET TO THE IDENTITY AFTER CORRECTING RHS. */

/* ... PARAMETER LIST: */

/*          N      DIMENSION OF MATRIX (= NN) */
/*          IA,JA  INTEGER ARRAYS OF SPARSE MATRIX REPRESENTATION */
/*          A      D.P. ARRAY OF SPARSE MATRIX REPRESENTATION */
/*          RHS    RIGHT HAND SIDE OF MATRIX PROBLEM */
/*          IW,RW  WORK ARRAYS OF LENGTH N */
/*          TOL    TOLERANCE FACTOR */
/*          ISYM   FLAG FOR TYPE OF STORAGE FOR SYSTEM */
/*                 (0: SYMMETRIC, 1:NONSYMMETRIC) */
/*          LEVEL  PRINTING SWITCH FOR ERROR CONDITION */
/*          NOUT OUTPUT TAPE NUMBER */
/*          IER    ERROR FLAG: NONZERO VALUE ON RETURN MEANS */
/*                    101 : DIAGONAL ENTRY NOT POSITIVE */
/*                    102 : THERE IS NO DIAGONAL ENTRY IN ROW */

/* ********************************************************************** */

/*     UPDATE.  SBELM HAS BEEN REWRITTEN TO SPEED UP THE LOCATION OF */
/*              OF ROWS WHICH ARE TO BE ELIMINATED.  THIS IS DONE BY */
/*              FIRST STORING THE LARGEST ELEMENT OF EACH ROW IN */
/*              THE ARRAY RW.  THE DIAGONAL ENTRY IS THEN COMPARED */
/*              WITH THE CORRESPONDING ELEMENT IN RW.  IF IT IS */
/*              DECIDED TO ELIMINATE THE ROW THEN IT IS MARKED FOR */
/*              ELIMINATION. */

/*              WHEN A ROW IS TO BE ELIMINATED ITS DIAGONAL ENTRY */
/*              IS STORED IN  RW  AND  IW IS MARKED BY A NONZERO */
/*              (WHICH IS THIS ROW NUMBER) */

/*              ROWS WHICH HAVE ONLY DIAGONAL ENTRIES ARE NOT */
/*              ALTERED. */

/* ********************************************************************* */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER NN,IA(1),JA(1),IW(NN),ISYM,LEVEL,NOUT,IER >*/
/*<       DOUBLE PRECISION A(1),RHS(NN),RW(NN),TOL >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER IBGN,ICNT,IEND,JJ,JJDI,KK,N >*/
/*<       DOUBLE PRECISION DI >*/

/*<       N = NN >*/
    /* Parameter adjustments */
    --rw;
    --iw;
    --rhs;
    --ia;
    --ja;
    --a;

    /* Function Body */
    n = *nn;

/*        IF (N .GE. 1) GO TO 10 */
/*           IER = 100 */
/*           RETURN */
/* 10     CONTINUE */

/* ... STORE THE LARGEST (DABSOLUTE VALUE) OFF DIAGONAL ENTRY FOR */
/* ... ROW II IN RW(II). */

/*<       IER = 0 >*/
    *ier = 0;
/*<       ICNT = 0 >*/
    icnt = 0;
/*<       DO 10 II = 1,N >*/
    i__1 = n;
    for (ii = 1; ii <= i__1; ++ii) {
/*<          RW(II) = 0.0D0 >*/
	rw[ii] = 0.;
/*<          IW(II) = 0 >*/
	iw[ii] = 0;
/*<    10 CONTINUE >*/
/* L10: */
    }
/*<       DO 20 II = 1,N >*/
    i__1 = n;
    for (ii = 1; ii <= i__1; ++ii) {
/*<          IBGN = IA(II) >*/
	ibgn = ia[ii];
/*<          IEND = IA(II+1)-1 >*/
	iend = ia[ii + 1] - 1;
/*<          IF (IBGN.GT.IEND) GO TO 140 >*/
	if (ibgn > iend) {
	    goto L140;
	}
/*<          DO 20 JJ = IBGN,IEND >*/
	i__2 = iend;
	for (jj = ibgn; jj <= i__2; ++jj) {
/*<             KK = JA(JJ) >*/
	    kk = ja[jj];
/*<             IF (KK.EQ.II) GO TO 20 >*/
	    if (kk == ii) {
		goto L20;
	    }
/*<             RW(II) = DMAX1(RW(II),DABS(A(JJ))) >*/
/* Computing MAX */
	    d__2 = rw[ii], d__3 = (d__1 = a[jj], abs(d__1));
	    rw[ii] = max(d__2,d__3);
/*<             IF (ISYM.NE.0) GO TO 20 >*/
	    if (*isym != 0) {
		goto L20;
	    }
/*<             RW(KK) = DMAX1(RW(KK),DABS(A(JJ))) >*/
/* Computing MAX */
	    d__2 = rw[kk], d__3 = (d__1 = a[jj], abs(d__1));
	    rw[kk] = max(d__2,d__3);
/*<    20 CONTINUE >*/
L20:
	    ;
	}
    }

/* ... FOR II = 1 TO N FIND THE DIAGONAL ENTRY IN ROW II */

/*<       DO 80 II = 1,N >*/
    i__2 = n;
    for (ii = 1; ii <= i__2; ++ii) {
/*<          IBGN = IA(II) >*/
	ibgn = ia[ii];
/*<          IEND = IA(II+1)-1 >*/
	iend = ia[ii + 1] - 1;
/*<          DO 40 JJ = IBGN,IEND >*/
	i__1 = iend;
	for (jj = ibgn; jj <= i__1; ++jj) {
/*<             IF (JA(JJ).NE.II) GO TO 40 >*/
	    if (ja[jj] != ii) {
		goto L40;
	    }
/*<             DI = A(JJ) >*/
	    di = a[jj];
/*<             JJDI = JJ >*/
	    jjdi = jj;
/*<             IF (DI.GT.0.D0) GO TO 50 >*/
	    if (di > 0.) {
		goto L50;
	    }
/*<             IER = 101 >*/
	    *ier = 101;
/*<             IF (LEVEL.GE.0) WRITE (NOUT,30) II,DI >*/
	    if (*level >= 0) {
		io___665.ciunit = *nout;
		s_wsfe(&io___665);
		do_fio(&c__1, (char *)&ii, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&di, (ftnlen)sizeof(doublereal));
		e_wsfe();
	    }
/*<    >*/
/*<             RETURN >*/
	    return 0;
/*<    40    CONTINUE >*/
L40:
	    ;
	}
/*<          GO TO 140 >*/
	goto L140;
/*<    50    CONTINUE >*/
L50:

/* ... CHECK THE SIZE OF THE LARGEST OFF DIAGONAL ELEMENT */
/* ... ( STORED IN RW(II) ) AGAINST THE DIAGONAL ELEMENT DII. */

/*<          IF (RW(II).NE.0.0D0) GO TO 60 >*/
	if (rw[ii] != 0.) {
	    goto L60;
	}
/*<          IF (1.0D0/DI.LE.TOL) GO TO 70 >*/
	if (1. / di <= *tol) {
	    goto L70;
	}
/*<          GO TO 80 >*/
	goto L80;
/*<    60    IF (RW(II)/DI.GT.TOL) GO TO 80 >*/
L60:
	if (rw[ii] / di > *tol) {
	    goto L80;
	}

/* ... THE OFF DIAGONAL ELEMENTS ARE SMALL COMPARED TO THE DIAGONAL */
/* ... THEREFORE MARK IT FOR ELIMINATION AND PERFORM INITIAL */
/* ... PROCESSING */

/*<    70    ICNT = ICNT+1 >*/
L70:
	++icnt;
/*<          IW(II) = II >*/
	iw[ii] = ii;
/*<          RW(II) = DI >*/
	rw[ii] = di;
/*<          A(JJDI) = 1.0D0 >*/
	a[jjdi] = 1.;
/*<          RHS(II) = RHS(II)/DI >*/
	rhs[ii] /= di;

/*<    80 CONTINUE >*/
L80:
	;
    }

/* ... ELIMINATE THE ROWS AND COLUMNS INDICATED BY THE NONZERO */
/* ... ENTRIES IN IW.  THERE ARE ICNT OF THEM */

/*<       IF (ICNT.EQ.0) GO TO 130 >*/
    if (icnt == 0) {
	goto L130;
    }

/* ... THE ELIMINATION IS AS FOLLOWS: */

/*     FOR II = 1 TO N DO */
/*        IF ( IW(II) .NE. 0 ) THEN */
/*           SET DIAGONAL VALUE TO 1.0  ( ALREADY DONE ) */
/*           SET RHS(II) = RHS(II) / RW(II)   ( ALREADY DONE ) */
/*           FIND NONZERO OFFDIAGONAL ENTRIES  KK */
/*           IF ( IW(KK) .EQ. 0 ) FIX UP RHS(KK)  WHEN USING SYMMETRIC ST */
/*           SET A(II,KK) = 0.0 */
/*        ELSE ( I.E.  IW(II) .EQ. 0  ) */
/*           FIND NONZERO OFFDIAGONAL ENTRIES   KK */
/*           IF ( IW(KK) .NE. 0 ) FIX UP RHS(II) */
/*                                AND SET A(II,KK) = 0.0 */
/*        END IF */
/*     END DO */

/*<       DO 120 II = 1,N >*/
    i__2 = n;
    for (ii = 1; ii <= i__2; ++ii) {
/*<          IBGN = IA(II) >*/
	ibgn = ia[ii];
/*<          IEND = IA(II+1)-1 >*/
	iend = ia[ii + 1] - 1;
/*<          IF (IW(II).EQ.0) GO TO 100 >*/
	if (iw[ii] == 0) {
	    goto L100;
	}

/* ... THE II-TH ROW IS TO BE ELIMINATED */

/*<          DO 90 JJ = IBGN,IEND >*/
	i__1 = iend;
	for (jj = ibgn; jj <= i__1; ++jj) {
/*<             KK = JA(JJ) >*/
	    kk = ja[jj];
/*<             IF (KK.EQ.II) GO TO 90 >*/
	    if (kk == ii) {
		goto L90;
	    }
/*<    >*/
	    if (iw[kk] == 0 && *isym == 0) {
		rhs[kk] -= a[jj] * rhs[ii];
	    }
/*<             A(JJ) = 0.0D0 >*/
	    a[jj] = 0.;
/*<    90    CONTINUE >*/
L90:
	    ;
	}
/*<          GO TO 120 >*/
	goto L120;

/* ... THE II-TH ROW IS KEPT.  CHECK THE OFF-DIAGONAL ENTRIES */

/*<   100    DO 110 JJ = IBGN,IEND >*/
L100:
	i__1 = iend;
	for (jj = ibgn; jj <= i__1; ++jj) {
/*<             KK = JA(JJ) >*/
	    kk = ja[jj];
/*<             IF (KK.EQ.II.OR.IW(KK).EQ.0) GO TO 110 >*/
	    if (kk == ii || iw[kk] == 0) {
		goto L110;
	    }
/*<             RHS(II) = RHS(II)-A(JJ)*RHS(KK) >*/
	    rhs[ii] -= a[jj] * rhs[kk];
/*<             A(JJ) = 0.0D0 >*/
	    a[jj] = 0.;
/*<   110    CONTINUE >*/
L110:
	    ;
	}

/*<   120 CONTINUE >*/
L120:
	;
    }

/*<   130 RETURN >*/
L130:
    return 0;

/* ... ERROR TRAPS -- NO DIAGONAL ENTRY IN ROW II (ROW MAY BE EMPTY). */

/*<   140 CONTINUE >*/
L140:
/*<       IER = 102 >*/
    *ier = 102;
/*<       IF (LEVEL.GE.0) WRITE (NOUT,150) II >*/
    if (*level >= 0) {
	io___666.ciunit = *nout;
	s_wsfe(&io___666);
	do_fio(&c__1, (char *)&ii, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/

/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* sbelm_ */

/*<       SUBROUTINE SBEND (N,NZ,IA,JA,A,IWORK) >*/
/* Subroutine */ int sbend_(integer *n, integer *nz, integer *ia, integer *ja,
	 doublereal *a, integer *iwork)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    integer i__, l, jaj;
    doublereal val;
    integer top, ideg, link, next, hlink, mhlink, ohlink, nulink, maxtop;


/* *********************************************************************** */

/*     SBEND IS THE THIRD OF A SUITE OF SUBROUTINES TO AID THE */
/*     USER TO CONSTRUCT THE  IA, JA, A DATA STRUCTURE USED IN */
/*     ITPACK. */

/*     SBEND RESTRUCTURES THE LINKED LIST DATA STRUCTURE BUILT BY */
/*     SBINI AND SBSIJ INTO THE FINAL DATA STRUCTURE REQUIRE BY */
/*     ITPACK.  THE RESTRUCTURING CAN TAKE PLACE IN THE MINIMUM */
/*     AMOUNT OF MEMORY REQUIRED TO HOLD THE NONZERO STRUCTURE OF */
/*     THE SPARSE MATRIX BUT WILL RUN QUICKER IF MORE STORAGE */
/*     IS ALLOWED. */

/*     SBEND IS BASED ON SUBROUTINE BUILD OF THE SPARSE MATRIX */
/*     PACKAGE SPARSPAK DEVELOPED BY ALAN GEORGE AND JOSEPH LUI */
/*     OF THE UNIVERSITY OF WATERLOO, WATERLOO, ONTARIO. */

/* ... PARAMETERS */

/* ...... INPUT */

/*     N       THE ORDER OF THE LINEAR SYSTEM */

/*     NZ      THE LENGTH OF THE ARRAYS JA, IWORK, AND A. */

/* ...... INPUT/OUTPUT */

/*     IA      INTEGER ARRAY OF LENGTH N+1.  THE FIRST N ENTRIES */
/*             POINT TO THE BEGINNING OF THE LINKED LIST FOR EACH */
/*             ROW.  IA(N+1)-1 IS THE TOP OF THE LINKED LISTS */
/*             CONTAINED IN JA, IWORK, AND A.  ON OUTPUT IA WILL */
/*             POINT TO THE FIRST ENTRY OF EACH ROW IN THE FINAL */
/*             DATA STRUCTURE. */

/*     JA      INTEGER ARRAY OF LENGTH NZ.  ON INPUT JA STORES THE */
/*             COLUMN NUMBERS OF THE NONZERO ENTRIES AS INDICATED */
/*             BY THE LINKED LISTS.  ON OUTPUT JA STORES THE */
/*             COLUMN NUMBERS IN ROW ORDERED FORM. */

/*     A       D.P. ARRAY OF LENGTH NZ.  ON INPUT A STORES THE */
/*             VALUE OF THE NOZERO ENTRIES AS INDICATED BY THE */
/*             LINKED LISTS.  ON OUTPUT A STORES THE VALUES IN */
/*             ROW ORDERED FORM. */

/*     IWORK    INTEGER ARRAY OF LENGTH NZ.  ON INPUT IWORK STORES THE */
/*             THE LINKS OF THE LINKED LISTS.  ON OUTPUT IT IS */
/*             DESTROYED. */

/* *********************************************************************** */

/*<       INTEGER N,NZ,IA(1),JA(NZ),IWORK(NZ) >*/
/*<       DOUBLE PRECISION A(NZ) >*/

/*<    >*/
/*<       DOUBLE PRECISION VAL >*/

/* *********************************************************************** */

/* ... INITIALIZATION */

/* ...... THE VARIABLES NEXT AND TOP RESPECTIVELY POINT TO THE */
/*        NEXT AVAILABLE ENTRY FOR THE FINAL DATA STRUCTURE AND */
/*        THE TOP OF THE REMAINDER OF THE LINKED LISTS. */

/*<       NEXT = 1 >*/
    /* Parameter adjustments */
    --iwork;
    --a;
    --ja;
    --ia;

    /* Function Body */
    next = 1;
/*<       TOP = IA(N+1)+1 >*/
    top = ia[*n + 1] + 1;
/*<       MAXTOP = NZ-IA(N+1)+1 >*/
    maxtop = *nz - ia[*n + 1] + 1;

/* *********************************************************************** */

/* ... CONVERT EACH ROW INTO FINAL FORM */

/*<       DO 90 I = 1,N >*/
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<          IDEG = 0 >*/
	ideg = 0;
/*<          NULINK = IA(I) >*/
	nulink = ia[i__];

/* ... LOOP OVER EACH NODE IN THE LINKED LIST OF ROW I */

/*<    10    LINK = NULINK >*/
L10:
	link = nulink;
/*<          IF (LINK.LE.0) GO TO 80 >*/
	if (link <= 0) {
	    goto L80;
	}
/*<          NULINK = IWORK(LINK) >*/
	nulink = iwork[link];
/*<          JAJ = JA(LINK) >*/
	jaj = ja[link];
/*<          VAL = A(LINK) >*/
	val = a[link];

/* ... CHECK TO SEE IF A COLLISION BETWEEN THE LINKED LISTS */
/*     AND THE FINAL FORM HAS OCCURRED. */

/*<          IF (NEXT.GE.TOP.AND.LINK.NE.TOP) GO TO 20 >*/
	if (next >= top && link != top) {
	    goto L20;
	}

/* ... COLLISION HAS NOT OCCURRED.  FREE THE SPACE FOR THE TRIPLE */
/*     (JA(LINK), A(LINK), IWORK(LINK)) */

/*<          JA(LINK) = 0 >*/
	ja[link] = 0;
/*<          A(LINK) = 0.0D0 >*/
	a[link] = 0.;
/*<          IWORK(LINK) = 0 >*/
	iwork[link] = 0;

/* ... SPECIAL CASE TO MOVE  TOP  DOWN IF LINK .EQ. TOP */

/*<          IF (LINK.EQ.TOP) GO TO 60 >*/
	if (link == top) {
	    goto L60;
	}
/*<          GO TO 70 >*/
	goto L70;

/* *********************************************************************** */

/* ... COLLISION HAS OCCURRED.  CLEAR OFF SOME SPACE FOR THE CURRENT */
/*     ENTRY BY MOVING THE TRIPLE ( JA(TOP),A(TOP),IWORK(TOP) ) */
/*     DOWNWARDS TO THE FREED TRIPLE ( JA(LINK),A(LINK),IWORK(LINK) ). */
/*     THEN ADJUST THE LINK FIELDS. */

/* ...... PATCH UP THE LINKED LIST FOR THE CURRENT ROW I.  THEN */
/*        TRAVERSE THE LINKED LIST CONTAINING TOP UNTIL THE POINTER */
/*        POINTER BACK TO IA IS FOUND. */

/*<    20    IA(I) = LINK >*/
L20:
	ia[i__] = link;
/*<          HLINK = TOP >*/
	hlink = top;

/*<    30    HLINK = IWORK(HLINK) >*/
L30:
	hlink = iwork[hlink];
/*<          IF (HLINK.GT.0) GO TO 30 >*/
	if (hlink > 0) {
	    goto L30;
	}

/* ...... NOW FOLLOW THE LINKED LIST BACK TO TOP KEEPING TRACK */
/*        OF THE OLD LINK. */

/* ......... SPECIAL CASE IF IA(-HLINK) = TOP */

/*<          MHLINK = -HLINK >*/
	mhlink = -hlink;
/*<          IF (IA(MHLINK).NE.TOP) GO TO 40 >*/
	if (ia[mhlink] != top) {
	    goto L40;
	}

/*<          IWORK(LINK) = IWORK(TOP) >*/
	iwork[link] = iwork[top];
/*<          JA(LINK) = JA(TOP) >*/
	ja[link] = ja[top];
/*<          A(LINK) = A(TOP) >*/
	a[link] = a[top];
/*<          IA(MHLINK) = LINK >*/
	ia[mhlink] = link;
/*<          IF (NULINK.EQ.TOP) NULINK = LINK >*/
	if (nulink == top) {
	    nulink = link;
	}
/*<          GO TO 60 >*/
	goto L60;

/* ......... USUAL CASE. */

/*<    40    HLINK = IA(MHLINK) >*/
L40:
	hlink = ia[mhlink];
/*<    50    OHLINK = HLINK >*/
L50:
	ohlink = hlink;
/*<          HLINK = IWORK(OHLINK) >*/
	hlink = iwork[ohlink];
/*<          IF (HLINK.NE.TOP) GO TO 50 >*/
	if (hlink != top) {
	    goto L50;
	}

/*<          IWORK(LINK) = IWORK(TOP) >*/
	iwork[link] = iwork[top];
/*<          JA(LINK) = JA(TOP) >*/
	ja[link] = ja[top];
/*<          A(LINK) = A(TOP) >*/
	a[link] = a[top];
/*<          IF (OHLINK.NE.LINK) IWORK(OHLINK) = LINK >*/
	if (ohlink != link) {
	    iwork[ohlink] = link;
	}
/*<          IF (NULINK.EQ.TOP) NULINK = LINK >*/
	if (nulink == top) {
	    nulink = link;
	}

/* ... COLLAPSE TOP OF LINK LIST BY AS MUCH AS POSSIBLE */

/*<    60    TOP = TOP+1 >*/
L60:
	++top;
/*<          IF (TOP.GE.MAXTOP) GO TO 70 >*/
	if (top >= maxtop) {
	    goto L70;
	}
/*<          IF (IWORK(TOP).NE.0) GO TO 70 >*/
	if (iwork[top] != 0) {
	    goto L70;
	}
/*<          GO TO 60 >*/
	goto L60;

/* *********************************************************************** */

/* ... PUT THE CURRENT TRIPLE INTO THE FINAL DATA STRUCTURE */

/*<    70    JA(NEXT) = JAJ >*/
L70:
	ja[next] = jaj;
/*<          A(NEXT) = VAL >*/
	a[next] = val;
/*<          NEXT = NEXT+1 >*/
	++next;
/*<          IDEG = IDEG+1 >*/
	++ideg;
/*<          GO TO 10 >*/
	goto L10;

/* ... FINAL STRUCTURE FOR ROW I IS COMPLETE.  LINKED LIST IS */
/*     DESTROYED AND WILL BE RECAPTURED AS NECESSARY BY THE */
/*     LOOP ON LABEL 60 */

/*<    80    IA(I) = IDEG >*/
L80:
	ia[i__] = ideg;

/*<    90 CONTINUE >*/
/* L90: */
    }

/* *********************************************************************** */

/* ... FINALIZE THE DATA STRUCTURE BY BUILDING THE FINAL VERSION OF */
/*     IA. */

/*<       L = IA(1)+1 >*/
    l = ia[1] + 1;
/*<       IA(1) = 1 >*/
    ia[1] = 1;
/*<       DO 100 I = 1,N >*/
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<          IDEG = IA(I+1) >*/
	ideg = ia[i__ + 1];
/*<          IA(I+1) = L >*/
	ia[i__ + 1] = l;
/*<          L = L+IDEG >*/
	l += ideg;
/*<   100 CONTINUE >*/
/* L100: */
    }

/* ... FINAL IA, JA, A DATA STRUCTURE BUILT. */

/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* sbend_ */

/*<       SUBROUTINE SBINI (N,NZ,IA,JA,A,IWORK) >*/
/* Subroutine */ int sbini_(integer *n, integer *nz, integer *ia, integer *ja,
	 doublereal *a, integer *iwork)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    integer i__;
    extern /* Subroutine */ int vfill_(integer *, doublereal *, doublereal *),
	     ivfill_(integer *, integer *, integer *);


/* *********************************************************************** */

/*     SBINI IS THE FIRST OF A SUITE OF THREE SUBROUTINES TO AID */
/*     THE USER TO CONSTRUCT THE IA, JA, A DATA STRUCTURE USED */
/*     IN ITPACK. */

/*     SBINI INITIALIZES THE ARRAYS IA, JA, IWORK, AND A.  THE OTHER */
/*     SUBROUTINES IN THE SUITE ARE SBSIJ ( WHICH BUILDS A LINKED */
/*     LIST REPRESENTATION OF THE MATRIX STRUCTURE ) AND SBEND ( WHICH */
/*     RESTRUCTURE THE LINKED LIST FORM INTO THE FINAL FORM ). */

/* ... PARAMETERS */

/* ...... INPUT */

/*     N          THE ORDER OF THE LINEAR SYSTEM */

/*     NZ         THE MAXIMUM NUMBER OF NONZEROES ALLOWED IN THE */
/*                LINEAR SYSTEM. */

/* ...... OUTPUT */

/*     IA         INTEGER ARRAY OF LENGTH N+1.  SBINI SETS THIS ARRAY */
/*                TO -I FOR I = 1 THRU N.  IA(N+1) IS SET TO NZ. */

/*     JA         INTEGER ARRAY OF LENGTH NZ.  INITIALIZED TO ZERO HERE. */

/*     A          D.P. ARRAY OF LENGTH NZ.  INITIALIZED TO ZERO HERE. */

/*     IWORK       INTEGER ARRAY OF LENGTH NZ.  INITIALIZED TO ZERO HERE. */

/* *********************************************************************** */

/*<       INTEGER N,NZ,IA(1),JA(NZ),IWORK(NZ),I >*/
/*<       DOUBLE PRECISION A(NZ) >*/

/* *********************************************************************** */

/*<       DO 10 I = 1,N >*/
    /* Parameter adjustments */
    --iwork;
    --a;
    --ja;
    --ia;

    /* Function Body */
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<          IA(I) = -I >*/
	ia[i__] = -i__;
/*<    10 CONTINUE >*/
/* L10: */
    }
/*<       IA(N+1) = NZ >*/
    ia[*n + 1] = *nz;

/*<       CALL IVFILL (NZ,JA,0) >*/
    ivfill_(nz, &ja[1], &c__0);
/*<       CALL IVFILL (NZ,IWORK,0) >*/
    ivfill_(nz, &iwork[1], &c__0);
/*<       CALL VFILL (NZ,A,0.D0) >*/
    vfill_(nz, &a[1], &c_b21);

/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* sbini_ */

/*<    >*/
/* Subroutine */ int sbsij_(integer *n, integer *nz, integer *ia, integer *ja,
	 doublereal *a, integer *iwork, integer *ii, integer *jj, doublereal *
	vall, integer *mode, integer *levell, integer *noutt, integer *ierr)
{
    /* Format strings */
    static char fmt_10[] = "(\0020\002,\002*** F A T A L     E R R O R *****\
*******\002/\0020\002,\002    IN ITPACK ROUTINE SBSIJ   \002/\002 \002,\002 \
   IER = \002,i10/\002 \002,\002    ( \002,i10,\002 , \002,i10,\002 )\002\
/\002 \002,\002    IMPROPER VALUE FOR I OR J \002)";
    static char fmt_50[] = "(\0020\002,\002*** W A R N I N G ************\
\002/\0020\002,\002    IN ITPACK ROUTINE SBSIJ   \002/\002 \002,\002    IER \
= \002,i10/\002 \002,\002    ( \002,i10,\002 , \002,i10,\002 )\002/\002 \002,\
\002    ENTRY ALREADY SET AND IS LEFT AS \002,d15.8)";
    static char fmt_70[] = "(\0020\002,\002*** W A R N I N G ************\
\002/\0020\002,\002    IN ITPACK ROUTINE SBSIJ   \002/\002 \002,\002    IER \
= \002,i10/\002 \002,\002    ( \002,i10,\002 , \002,i10,\002 )\002/\002 \002,\
\002    ENTRY ALREADY SET - CURRENT VALUE OF\002,d15.8/\002 \002,\002       \
                         RESET TO\002,d15.8)";
    static char fmt_90[] = "(\0020\002,\002*** W A R N I N G ************\
\002/\0020\002,\002    IN ITPACK ROUTINE SBSIJ   \002/\002 \002,\002    IER \
= \002,i10/\002 \002,\002    ( \002,i10,\002 , \002,i10,\002 )\002/\002 \002,\
\002    ENTRY ALREADY SET - CURRENT VALUE OF\002,d15.8/\002 \002,\002       \
                         RESET TO\002,d15.8)";
    static char fmt_120[] = "(\0020\002,\002*** F A T A L     E R R O R ****\
********\002/\0020\002,\002    IN ITPACK ROUTINE SBSIJ   \002/\002 \002,\002\
    IER = \002,i10/\002 \002,\002    NZ TOO SMALL - NO ROOM FOR NEW ENTRY\
\002)";

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    integer i__, j, ier;
    doublereal val;
    integer npl1, link;
    doublereal temp;
    integer next, nout, level;

    /* Fortran I/O blocks */
    static cilist io___687 = { 0, 0, 0, fmt_10, 0 };
    static cilist io___691 = { 0, 0, 0, fmt_50, 0 };
    static cilist io___692 = { 0, 0, 0, fmt_70, 0 };
    static cilist io___694 = { 0, 0, 0, fmt_90, 0 };
    static cilist io___695 = { 0, 0, 0, fmt_120, 0 };



/* *********************************************************************** */

/*     SBSIJ IS THE SECOND OF A SUITE OF THREE SUBROUTINES TO AID IN */
/*     THE CONSTRUCTION OF THE IA, JA, A DATA STRUCTURE USED IN */
/*     ITPACK. */

/*     SBSIJ TAKES THE INDIVIDUAL ENTRIES OF THE SPARSE MATRIX AS */
/*     GIVEN TO IT AT EACH CALL VIA  (I,J,VAL) AND INSERTS IT INTO */
/*     A LINKED LIST REPRESENTATION OF THE SPARSE MATRIX. */

/*     EACH ROW OF THE SPARSE MATRIX IS ASSOCIATED WITH A CIRCULAR */
/*     LINKED LIST BEGINNING AT IA(I).  THE LAST ENTERED ELEMENT IN */
/*     EACH LIST POINTS BACK TO IA(I) WITH THE VALUE -I.  THE LINKS */
/*     ARE STORED IN THE ARRAY IWORK, WHILE JA AND A STORE THE COLUMN */
/*     NUMBER AND VALUE IN PARALLEL TO IWORK.  THE LINKED LISTED ARE */
/*     STORED BEGINNING AT ENTRY NZ AND WORKING BACKWARDS TOWARDS 1. */

/* ... PARAMETERS */

/* ...... INPUT */

/*     N       THE ORDER OF THE LINEAR SYSTEM */

/*     NZ      THE LENGTH OF THE ARRAYS  JA, A, AND IWORK */

/*     I, J    THE ROW AND COLUMN NUMBERS OF THE ENTRY OF THE SPARSE */
/*             LINEAR SYSTEM TO BE ENTERED IN THE DATA STRUCTURE(=II,JJ) */

/*     VAL     THE NONZERO VALUE ASSOCIATED WITH (I,J)  (= VALL) */

/*     MODE    IF THE (I,J) ENTRY HAS ALREADY BEEN SET, MODE SPECIFIES */
/*             THE WAY IN WHICH THE ENTRY IS TO BE TREATED. */
/*             IF   MODE .LT. 0  LET THE VALUE REMAIN AS IS */
/*                       .EQ. 0  RESET IT TO THE NEW VALUE */
/*                       .GT. 0  ADD THE NEW VALUE TO THE OLD VALUE */

/*     NOUT  OUTPUT FILE NUMBER (= NOUTT) */

/*     LEVEL   OUTPUT FILE SWITCH (= LEVELL) */
/* ... INPUT/OUTPUT */

/*     IA      INTEGER ARRAY OF LENGTH N+1.  THE FIRST N ENTRIES */
/*             POINT TO THE BEGINNING OF THE LINKED LIST FOR EACH */
/*             ROW.  IA(N+1) POINTS TO THE NEXT ENTRY AVAILABLE FOR */
/*             STORING THE CURRENT ENTRY INTO THE LINKED LIST. */

/*     JA      INTEGER ARRAY OF LENGTH NZ.  JA STORES THE COLUMN */
/*             NUMBERS OF THE NONZERO ENTRIES. */

/*     A       D.P. ARRAY OF LENGTH NZ.  A STORES THE VALUE OF THE */
/*             NONZERO ENTRIES. */

/*     IWORK   INTEGER ARRAY OF LENGTH NZ. IWORK STORES THE LINKS. */

/*     IER     ERROR FLAG.(= IERR)  POSSIBLE RETURNS ARE */
/*             IER =    0   SUCCESSFUL COMPLETION */
/*                 =  700   ENTRY WAS ALREADY SET,  VALUE HANDLED */
/*                          AS SPECIFIED BY MODE. */
/*                 =  701   IMPROPER VALUE OF EITHER I OR J INDEX */
/*                 =  702   NO ROOM REMAINING, NZ TOO SMALL. */

/* *********************************************************************** */

/*<       INTEGER N,NZ,IA(1),JA(NZ),IWORK(NZ),II,JJ,MODE,LEVELL,NOUTT,IERR >*/
/*<       DOUBLE PRECISION A(NZ),VALL >*/

/*<       INTEGER LINK,NEXT,NPL1,I,J,LEVEL,NOUT,IER >*/
/*<       DOUBLE PRECISION VAL,TEMP >*/

/* *********************************************************************** */

/* ... CHECK THE VALIDITY OF THE (I,J) ENTRY */

/*<       I = II >*/
    /* Parameter adjustments */
    --iwork;
    --a;
    --ja;
    --ia;

    /* Function Body */
    i__ = *ii;
/*<       J = JJ >*/
    j = *jj;
/*<       VAL = VALL >*/
    val = *vall;
/*<       LEVEL = LEVELL >*/
    level = *levell;
/*<       NOUT = NOUTT >*/
    nout = *noutt;
/*<       IER = 0 >*/
    ier = 0;
/*<       IF (I.LE.0.OR.I.GT.N) IER = 701 >*/
    if (i__ <= 0 || i__ > *n) {
	ier = 701;
    }
/*<       IF (J.LE.0.OR.J.GT.N) IER = 701 >*/
    if (j <= 0 || j > *n) {
	ier = 701;
    }
/*<       IF (IER.EQ.0) GO TO 20 >*/
    if (ier == 0) {
	goto L20;
    }
/*<       IF (LEVEL.GE.0) WRITE (NOUT,10) IER,I,J >*/
    if (level >= 0) {
	io___687.ciunit = nout;
	s_wsfe(&io___687);
	do_fio(&c__1, (char *)&ier, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&i__, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&j, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 130 >*/
    goto L130;

/* ... TRAVERSE THE LINK LIST POINTED TO BY IA(I) UNTIL EITHER */
/* ... THE J ENTRY OR THE END OF THE LIST HAS BEEN FOUND. */

/*<    20 NPL1 = N+1 >*/
L20:
    npl1 = *n + 1;
/*<       LINK = IA(I) >*/
    link = ia[i__];

/* ...... SPECIAL CASE FOR THE FIRST ENTRY IN THE ROW */

/*<       IF (LINK.GT.0) GO TO 30 >*/
    if (link > 0) {
	goto L30;
    }
/*<       NEXT = IA(NPL1) >*/
    next = ia[npl1];
/*<       IF (NEXT.LT.1) GO TO 110 >*/
    if (next < 1) {
	goto L110;
    }

/*<       IA(I) = NEXT >*/
    ia[i__] = next;
/*<       JA(NEXT) = J >*/
    ja[next] = j;
/*<       A(NEXT) = VAL >*/
    a[next] = val;
/*<       IWORK(NEXT) = -I >*/
    iwork[next] = -i__;
/*<       IA(NPL1) = NEXT-1 >*/
    ia[npl1] = next - 1;
/*<       GO TO 130 >*/
    goto L130;

/* ... FOLLOW THE LINK LIST UNTIL J OR THE END OF THE LIST IS FOUND */

/*<    30 IF (JA(LINK).EQ.J) GO TO 40 >*/
L30:
    if (ja[link] == j) {
	goto L40;
    }
/*<       IF (IWORK(LINK).LE.0) GO TO 100 >*/
    if (iwork[link] <= 0) {
	goto L100;
    }
/*<       LINK = IWORK(LINK) >*/
    link = iwork[link];
/*<       GO TO 30 >*/
    goto L30;

/* : */
/* ... ENTRY (I,J) ALREADY HAS BEEN SET.  RESET VALUE DEPENDING ON MODE */

/*<    40 IER = 700 >*/
L40:
    ier = 700;
/*<       IF (MODE.GE.0) GO TO 60 >*/
    if (*mode >= 0) {
	goto L60;
    }
/*<       IF (LEVEL.GE.1) WRITE (NOUT,50) IER,I,J,A(LINK) >*/
    if (level >= 1) {
	io___691.ciunit = nout;
	s_wsfe(&io___691);
	do_fio(&c__1, (char *)&ier, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&i__, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&j, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&a[link], (ftnlen)sizeof(doublereal));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 130 >*/
    goto L130;
/*<    60 IF (MODE.GE.1) GO TO 80 >*/
L60:
    if (*mode >= 1) {
	goto L80;
    }
/*<       IF (LEVEL.GE.1) WRITE (NOUT,70) IER,I,J,A(LINK),VAL >*/
    if (level >= 1) {
	io___692.ciunit = nout;
	s_wsfe(&io___692);
	do_fio(&c__1, (char *)&ier, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&i__, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&j, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&a[link], (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&val, (ftnlen)sizeof(doublereal));
	e_wsfe();
    }
/*<    >*/
/*<       A(LINK) = VAL >*/
    a[link] = val;
/*<       GO TO 130 >*/
    goto L130;
/*<    80 TEMP = A(LINK)+VAL >*/
L80:
    temp = a[link] + val;
/*<       IF (LEVEL.GE.1) WRITE (NOUT,90) IER,I,J,A(LINK),TEMP >*/
    if (level >= 1) {
	io___694.ciunit = nout;
	s_wsfe(&io___694);
	do_fio(&c__1, (char *)&ier, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&i__, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&j, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&a[link], (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&temp, (ftnlen)sizeof(doublereal));
	e_wsfe();
    }
/*<    >*/
/*<       A(LINK) = TEMP >*/
    a[link] = temp;
/*<       GO TO 130 >*/
    goto L130;

/* ... ENTRY (I,J) HAS NOT BEEN SET.  ENTER IT INTO THE LINKED LIST */

/*<   100 NEXT = IA(NPL1) >*/
L100:
    next = ia[npl1];
/*<       IF (NEXT.LT.1) GO TO 110 >*/
    if (next < 1) {
	goto L110;
    }

/*<       IWORK(LINK) = NEXT >*/
    iwork[link] = next;
/*<       JA(NEXT) = J >*/
    ja[next] = j;
/*<       A(NEXT) = VAL >*/
    a[next] = val;
/*<       IWORK(NEXT) = -I >*/
    iwork[next] = -i__;
/*<       IA(NPL1) = NEXT-1 >*/
    ia[npl1] = next - 1;
/*<       GO TO 130 >*/
    goto L130;

/* *********************************************************************** */

/* ... ERROR TRAP FOR NO ROOM REMAINING */

/*<   110 IER = 702 >*/
L110:
    ier = 702;
/*<       IF (LEVEL.GE.0) WRITE (NOUT,120) IER >*/
    if (level >= 0) {
	io___695.ciunit = nout;
	s_wsfe(&io___695);
	do_fio(&c__1, (char *)&ier, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/

/*<   130 CONTINUE >*/
L130:
/*<       IERR = IER >*/
    *ierr = ier;
/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* sbsij_ */

/*<       SUBROUTINE SCAL (NN,IA,JA,A,RHS,U,D,LEVEL,NOUT,IER) >*/
/* Subroutine */ int scal_(integer *nn, integer *ia, integer *ja, doublereal *
	a, doublereal *rhs, doublereal *u, doublereal *d__, integer *level, 
	integer *nout, integer *ier)
{
    /* Format strings */
    static char fmt_10[] = "(\0020\002,\002*** F A T A L     E R R O R *****\
*******\002/\0020\002,\002    IN ITPACK ROUTINE SCAL    \002/\002 \002,\002 \
   DIAGONAL ENTRY IN ROW \002,i10,\002 NEGATIVE\002)";
    static char fmt_30[] = "(\0020\002,\002*** F A T A L     E R R O R *****\
*******\002/\0020\002,\002    IN ITPACK ROUTINE SCAL    \002/\002 \002,\002 \
   DIAGONAL ENTRY IN ROW \002,i10,\002 IS ZERO\002)";
    static char fmt_60[] = "(\0020\002,\002*** F A T A L     E R R O R *****\
*******\002/\0020\002,\002    IN ITPACK ROUTINE SCAL    \002/\002 \002,\002 \
   NO DIAGONAL ENTRY IN ROW\002,i10)";

    /* System generated locals */
    integer i__1, i__2;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();
    double sqrt(doublereal);

    /* Local variables */
    integer i__, j, n;
    doublereal di;
    integer ii, jj, im1, np1, jadd, jajj, ibgn, iend, jjpi;

    /* Fortran I/O blocks */
    static cilist io___702 = { 0, 0, 0, fmt_10, 0 };
    static cilist io___703 = { 0, 0, 0, fmt_30, 0 };
    static cilist io___704 = { 0, 0, 0, fmt_60, 0 };



/* ... ORIGINAL MATRIX IS SCALED TO A UNIT DIAGONAL MATRIX.  RHS */
/* ... AND U ARE SCALED ACCORDINGLY.  THE MATRIX IS THEN SPLIT AND */
/* ... IA, JA, AND A RESHUFFLED. */

/* ... PARAMETER LIST: */

/*          N      DIMENSION OF MATRIX (= NN) */
/*          IA,JA  INTEGER ARRAYS OF SPARSE MATRIX REPRESENTATION */
/*          A      D.P. ARRAY OF SPARSE MATRIX REPRESENTATION */
/*          RHS    RIGHT HAND SIDE OF MATRIX PROBLEM */
/*          U      LATEST ESTIMATE OF SOLUTION */
/*          D      OUTPUT VECTOR CONTAINING THE SQUARE ROOTS */
/*                    OF THE DIAGONAL ENTRIES */
/*          LEVEL  PRINTING SWITCH FOR ERROR CONDITION */
/*          NOUT OUTPUT TAPE NUMBER */
/*          IER    ERROR FLAG: ON RETURN NONZERO VALUES MEAN */
/*                    401 : THE ITH DIAGONAL ELEMENT IS .LE. 0. */
/*                    402 : NO DIAGONAL ELEMENT IN ROW I */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER IA(1),JA(1),NN,LEVEL,NOUT,IER >*/
/*<       DOUBLE PRECISION A(1),RHS(NN),U(NN),D(NN) >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER I,IBGN,IEND,II,IM1,J,JADD,JAJJ,JJ,JJPI,N,NP1 >*/
/*<       DOUBLE PRECISION DI >*/

/* ... EXTRACT SQUARE ROOT OF THE DIAGONAL OUT OF A AND SCALE U AND RHS */

/*<       N = NN >*/
    /* Parameter adjustments */
    --d__;
    --u;
    --rhs;
    --ia;
    --ja;
    --a;

    /* Function Body */
    n = *nn;
/*<       IER = 0 >*/
    *ier = 0;
/*<       DO 80 II = 1,N >*/
    i__1 = n;
    for (ii = 1; ii <= i__1; ++ii) {
/*<          IBGN = IA(II) >*/
	ibgn = ia[ii];
/*<          IEND = IA(II+1)-1 >*/
	iend = ia[ii + 1] - 1;
/*<          IF (IBGN.GT.IEND) GO TO 50 >*/
	if (ibgn > iend) {
	    goto L50;
	}
/*<          DO 40 JJ = IBGN,IEND >*/
	i__2 = iend;
	for (jj = ibgn; jj <= i__2; ++jj) {
/*<             IF (JA(JJ).NE.II) GO TO 40 >*/
	    if (ja[jj] != ii) {
		goto L40;
	    }
/*<             DI = A(JJ) >*/
	    di = a[jj];
/*<             IF (DI.GT.0.D0) GO TO 70 >*/
	    if (di > 0.) {
		goto L70;
	    }
/*<             IF (DI.EQ.0.D0) GO TO 20 >*/
	    if (di == 0.) {
		goto L20;
	    }
/*<             IER = 401 >*/
	    *ier = 401;
/*<             IF (LEVEL.GE.0) WRITE (NOUT,10) II >*/
	    if (*level >= 0) {
		io___702.ciunit = *nout;
		s_wsfe(&io___702);
		do_fio(&c__1, (char *)&ii, (ftnlen)sizeof(integer));
		e_wsfe();
	    }
/*<    >*/
/*<             RETURN >*/
	    return 0;
/*<    20       IER = 401 >*/
L20:
	    *ier = 401;
/*<             IF (LEVEL.GE.0) WRITE (NOUT,30) >*/
	    if (*level >= 0) {
		io___703.ciunit = *nout;
		s_wsfe(&io___703);
		e_wsfe();
	    }
/*<    >*/
/*<             RETURN >*/
	    return 0;
/*<    40    CONTINUE >*/
L40:
	    ;
	}
/*<    50    IER = 402 >*/
L50:
	*ier = 402;
/*<          IF (LEVEL.GE.0) WRITE (NOUT,60) II >*/
	if (*level >= 0) {
	    io___704.ciunit = *nout;
	    s_wsfe(&io___704);
	    do_fio(&c__1, (char *)&ii, (ftnlen)sizeof(integer));
	    e_wsfe();
	}
/*<    >*/
/*<          RETURN >*/
	return 0;

/*<    70    CONTINUE >*/
L70:
/*<          DI = DSQRT(DABS(DI)) >*/
	di = sqrt((abs(di)));
/*<          RHS(II) = RHS(II)/DI >*/
	rhs[ii] /= di;
/*<          U(II) = U(II)*DI >*/
	u[ii] *= di;
/*<          D(II) = DI >*/
	d__[ii] = di;
/*<    80 CONTINUE >*/
/* L80: */
    }

/* ... SHIFT MATRIX TO ELIMINATE DIAGONAL ENTRIES */

/*<       IF (N.EQ.1) GO TO 110 >*/
    if (n == 1) {
	goto L110;
    }
/*<       NP1 = N+1 >*/
    np1 = n + 1;
/*<       DO 100 I = 1,N >*/
    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<          IM1 = I-1 >*/
	im1 = i__ - 1;
/*<          II = NP1-I >*/
	ii = np1 - i__;
/*<          IBGN = IA(II) >*/
	ibgn = ia[ii];
/*<          IEND = IA(II+1)-1 >*/
	iend = ia[ii + 1] - 1;
/*<          JADD = IBGN+IEND >*/
	jadd = ibgn + iend;
/*<          DO 90 J = IBGN,IEND >*/
	i__2 = iend;
	for (j = ibgn; j <= i__2; ++j) {
/*<             JJ = JADD-J >*/
	    jj = jadd - j;
/*<             JJPI = JJ+IM1 >*/
	    jjpi = jj + im1;
/*<             IF (JA(JJ).EQ.II) IM1 = I >*/
	    if (ja[jj] == ii) {
		im1 = i__;
	    }
/*<             A(JJPI) = A(JJ) >*/
	    a[jjpi] = a[jj];
/*<             JA(JJPI) = JA(JJ) >*/
	    ja[jjpi] = ja[jj];
/*<    90    CONTINUE >*/
/* L90: */
	}
/*<          IA(II+1) = IA(II+1)+I-1 >*/
	ia[ii + 1] = ia[ii + 1] + i__ - 1;
/*<   100 CONTINUE >*/
/* L100: */
    }
/*<   110 IA(1) = IA(1)+N >*/
L110:
    ia[1] += n;

/* ... SCALE SHIFTED MATRIX AND STORE D ARRAY IN FIRST N ENTRIES OF A */

/*<       DO 140 II = 1,N >*/
    i__1 = n;
    for (ii = 1; ii <= i__1; ++ii) {
/*<          IBGN = IA(II) >*/
	ibgn = ia[ii];
/*<          IEND = IA(II+1)-1 >*/
	iend = ia[ii + 1] - 1;
/*<          DI = D(II) >*/
	di = d__[ii];
/*<          IF (IBGN.GT.IEND) GO TO 130 >*/
	if (ibgn > iend) {
	    goto L130;
	}
/*<          DO 120 JJ = IBGN,IEND >*/
	i__2 = iend;
	for (jj = ibgn; jj <= i__2; ++jj) {
/*<             JAJJ = JA(JJ) >*/
	    jajj = ja[jj];
/*<             A(JJ) = A(JJ)/(DI*D(JAJJ)) >*/
	    a[jj] /= di * d__[jajj];
/*<   120    CONTINUE >*/
/* L120: */
	}
/*<   130    CONTINUE >*/
L130:
/*<          A(II) = DI >*/
	a[ii] = di;
/*<   140 CONTINUE >*/
/* L140: */
    }

/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* scal_ */

/*<       SUBROUTINE SUM3 (N,C1,X1,C2,X2,C3,X3) >*/
/* Subroutine */ int sum3_(integer *n, doublereal *c1, doublereal *x1, 
	doublereal *c2, doublereal *x2, doublereal *c3, doublereal *x3)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    integer i__;


/* ... COMPUTES X3 = C1*X1 + C2*X2 + C3*X3 */

/* ... PARAMETER LIST: */

/*          N        INTEGER LENGTH OF VECTORS X1, X2, X3 */
/*          C1,C2,C3 D.P. CONSTANTS */
/*          X1,X2,X3 D.P. VECTORS SUCH THAT */
/*                   X3(I) = C1*X1(I) + C2*X2(I) + C3*X3(I) */
/*                   X3(I) = C1*X1(I) + C2*X2(I)  IF C3 = 0. */

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER I >*/

/* *** BEGIN: ITPACK COMMON */

/*<       INTEGER IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/
/*<       COMMON /ITCOM1/ IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/

/*<       LOGICAL ADAPT,BETADT,CASEII,HALT,PARTAD >*/
/*<       COMMON /ITCOM2/ ADAPT,BETADT,CASEII,HALT,PARTAD >*/

/*<    >*/
/*<    >*/

/* *** END  : ITPACK COMMON */

/*<       DOUBLE PRECISION X1(N),X2(N),X3(N),C1,C2,C3 >*/

/*<       IF (N.LE.0) RETURN >*/
    /* Parameter adjustments */
    --x3;
    --x2;
    --x1;

    /* Function Body */
    if (*n <= 0) {
	return 0;
    }
/*<       IF (DABS(C3).EQ.0.D0) GO TO 20 >*/
    if (abs(*c3) == 0.) {
	goto L20;
    }

/*<       DO 10 I = 1,N >*/
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<          X3(I) = C1*X1(I)+C2*X2(I)+C3*X3(I) >*/
	x3[i__] = *c1 * x1[i__] + *c2 * x2[i__] + *c3 * x3[i__];
/*<    10 CONTINUE >*/
/* L10: */
    }
/*<       RETURN >*/
    return 0;

/* ... COMPUTE X3 = C1*X1 + C2*X2 */

/*<    20 DO 30 I = 1,N >*/
L20:
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<          X3(I) = C1*X1(I)+C2*X2(I) >*/
	x3[i__] = *c1 * x1[i__] + *c2 * x2[i__];
/*<    30 CONTINUE >*/
/* L30: */
    }

/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* sum3_ */

/*<       DOUBLE PRECISION FUNCTION TAU (II) >*/
doublereal tau_(integer *ii)
{
    /* Initialized data */

    static doublereal t[8] = { 1.5,1.8,1.85,1.9,1.94,1.96,1.975,1.985 };

    /* System generated locals */
    doublereal ret_val;


/* ... THIS SUBROUTINE SETS TAU(II) FOR THE SOR METHOD. */

/* ... PARAMETER LIST: */

/*          II     NUMBER OF TIMES PARAMETERS HAVE BEEN CHANGED */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER II >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       DOUBLE PRECISION T(8) >*/

/*<    >*/

/*<       TAU = 1.992D0 >*/
    ret_val = 1.992;
/*<       IF (II.LE.8) TAU = T(II) >*/
    if (*ii <= 8) {
	ret_val = t[*ii - 1];
    }

/*<       RETURN >*/
    return ret_val;
/*<       END >*/
} /* tau_ */

/*<       FUNCTION TIMER (TIMDMY) >*/
doublereal timer_(real *timdmy)
{
    /* System generated locals */
    real ret_val;

    /* Local variables */
    extern doublereal etime_(real *);
    real total, tarray[2];


/* ... TIMER IS A ROUTINE TO RETURN THE EXECUTION TIME IN */
/* ... SECONDS. */

/* ... PARAMETERS -- */

/*          TIMDMY   DUMMY ARGUMENT */


/* ********************************************* */
/* **                                         ** */
/* **   THIS ROUTINE IS NOT PORTABLE.         ** */
/* **                                         ** */
/* ********************************************* */

/*<       REAL TIMDMY >*/

/* ... CRAY Y-MP. */

/*     TIMER = SECOND () */

/* ... UNIX ETIME FACILITY. */

/*<       EXTERNAL ETIME >*/
/*<       DIMENSION TARRAY(2) >*/
/*<       REAL ETIME, TIMER >*/
/*<       TOTAL = ETIME (TARRAY) >*/
    total = etime_(tarray);
/*<       TIMER = TOTAL >*/
    ret_val = total;

/* ... IBM RISC SYSTEM/6000. */

/*     TIMER = FLOAT(MCLOCK())/100.0 */

/*<       RETURN >*/
    return ret_val;
/*<       END >*/
} /* timer_ */

/*<       LOGICAL FUNCTION TSTCHG (IBMTH) >*/
logical tstchg_(integer *ibmth)
{
    /* System generated locals */
    doublereal d__1;
    logical ret_val;

    /* Builtin functions */
    double sqrt(doublereal), pow_di(doublereal *, integer *), pow_dd(
	    doublereal *, doublereal *);

    /* Local variables */
    integer ip;


/*     THIS FUNCTION PERFORMS A TEST TO DETERMINE IF PARAMETERS */
/*     SHOULD BE CHANGED FOR SEMI-ITERATION ACCELERATED METHODS. */

/* ... PARAMETER LIST: */

/*          IBMTH  INDICATOR OF BASIC METHOD BEING ACCELERATED BY SI */
/*                      IBMTH = 1,   JACOBI */
/*                            = 2,   REDUCED SYSTEM */
/*                            = 3,   SSOR */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER IBMTH >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER IP >*/

/* *** BEGIN: ITPACK COMMON */

/*<       INTEGER IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/
/*<       COMMON /ITCOM1/ IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/

/*<       LOGICAL ADAPT,BETADT,CASEII,HALT,PARTAD >*/
/*<       COMMON /ITCOM2/ ADAPT,BETADT,CASEII,HALT,PARTAD >*/

/*<    >*/
/*<    >*/

/* *** END  : ITPACK COMMON */

/*     DESCRIPTION OF VARIABLES IN COMMON BLOCKS IN MAIN SUBROUTINE */

/*<       IP = IN-IS >*/
    ip = itcom1_1.in - itcom1_1.is;
/*<       IF (IBMTH.EQ.2) IP = 2*IP >*/
    if (*ibmth == 2) {
	ip <<= 1;
    }

/*<       IF (IN.EQ.0) GO TO 10 >*/
    if (itcom1_1.in == 0) {
	goto L10;
    }
/*<       IF (IP.LT.3) GO TO 20 >*/
    if (ip < 3) {
	goto L20;
    }

/*<       QA = DSQRT(DABS(DELNNM/DELSNM)) >*/
    itcom3_1.qa = sqrt((d__1 = itcom3_1.delnnm / itcom3_1.delsnm, abs(d__1)));
/*<       QT = 2.D0*DSQRT(DABS(RRR**IP))/(1.D0+RRR**IP) >*/
    itcom3_1.qt = sqrt((d__1 = pow_di(&itcom3_1.rrr, &ip), abs(d__1))) * 2. / 
	    (pow_di(&itcom3_1.rrr, &ip) + 1.);
/*<       IF ((QA.GE.1.D0).OR.(QA.LT.QT**FF)) GO TO 20 >*/
    if (itcom3_1.qa >= 1. || itcom3_1.qa < pow_dd(&itcom3_1.qt, &itcom3_1.ff))
	     {
	goto L20;
    }

/* ... TEST PASSES -- CHANGE PARAMETERS */

/*<    10 TSTCHG = .TRUE. >*/
L10:
    ret_val = TRUE_;
/*<       RETURN >*/
    return ret_val;

/* ... TEST FAILS -- DO NOT CHANGE PARAMETERS */

/*<    20 TSTCHG = .FALSE. >*/
L20:
    ret_val = FALSE_;
/*<       RETURN >*/
    return ret_val;

/*<       END >*/
} /* tstchg_ */

/*<       SUBROUTINE UNSCAL (N,IA,JA,A,RHS,U,D) >*/
/* Subroutine */ int unscal_(integer *n, integer *ia, integer *ja, doublereal 
	*a, doublereal *rhs, doublereal *u, doublereal *d__)
{
    /* System generated locals */
    integer i__1, i__2;
    doublereal d__1;

    /* Local variables */
    doublereal di;
    integer ii, jj, is, jajj, ibgn, iend, jjpi, inew;


/* ... THIS SUBROUTINE REVERSES THE PROCESS OF SCAL. */

/* ... PARAMETER LIST: */

/*          N      DIMENSION OF MATRIX */
/*          IA,JA  INTEGER ARRAYS OF SPARSE MATRIX REPRESENTATION */
/*          A      D.P. ARRAY OF SPARSE MATRIX REPRESENTATION */
/*          RHS    RIGHT HAND SIDE OF MATRIX PROBLEM */
/*          U      LATEST ESTIMATE OF SOLUTION */
/*          D      VECTOR CONTAINING THE SQUARE ROOTS */
/*                    OF THE DIAGONAL ENTRIES */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER IA(1),JA(1),N >*/
/*<       DOUBLE PRECISION A(1),RHS(N),U(N),D(N) >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER IBGN,IEND,II,INEW,IS,JAJJ,JJ,JJPI >*/
/*<       DOUBLE PRECISION DI >*/

/* ... EXTRACT DIAGONAL FROM SCALED A AND UNSCALE U AND RHS */

/*<       DO 10 II = 1,N >*/
    /* Parameter adjustments */
    --d__;
    --u;
    --rhs;
    --ia;
    --ja;
    --a;

    /* Function Body */
    i__1 = *n;
    for (ii = 1; ii <= i__1; ++ii) {
/*<          DI = A(II) >*/
	di = a[ii];
/*<          U(II) = U(II)/DI >*/
	u[ii] /= di;
/*<          RHS(II) = RHS(II)*DI >*/
	rhs[ii] *= di;
/*<          D(II) = DI >*/
	d__[ii] = di;
/*<    10 CONTINUE >*/
/* L10: */
    }

/* ... UNSCALE A */

/*<       DO 30 II = 1,N >*/
    i__1 = *n;
    for (ii = 1; ii <= i__1; ++ii) {
/*<          IBGN = IA(II) >*/
	ibgn = ia[ii];
/*<          IEND = IA(II+1)-1 >*/
	iend = ia[ii + 1] - 1;
/*<          IF (IBGN.GT.IEND) GO TO 30 >*/
	if (ibgn > iend) {
	    goto L30;
	}
/*<          DI = D(II) >*/
	di = d__[ii];
/*<          DO 20 JJ = IBGN,IEND >*/
	i__2 = iend;
	for (jj = ibgn; jj <= i__2; ++jj) {
/*<             JAJJ = JA(JJ) >*/
	    jajj = ja[jj];
/*<             A(JJ) = A(JJ)*DI*D(JAJJ) >*/
	    a[jj] = a[jj] * di * d__[jajj];
/*<    20    CONTINUE >*/
/* L20: */
	}
/*<    30 CONTINUE >*/
L30:
	;
    }

/* ... INSERT DIAGONAL BACK INTO A */

/*<       DO 60 II = 1,N >*/
    i__1 = *n;
    for (ii = 1; ii <= i__1; ++ii) {
/*<          IBGN = IA(II) >*/
	ibgn = ia[ii];
/*<          IEND = IA(II+1)-1 >*/
	iend = ia[ii + 1] - 1;
/*<          IS = N-II >*/
	is = *n - ii;
/*<          INEW = IBGN-IS-1 >*/
	inew = ibgn - is - 1;
/*<          A(INEW) = D(II)**2 >*/
/* Computing 2nd power */
	d__1 = d__[ii];
	a[inew] = d__1 * d__1;
/*<          JA(INEW) = II >*/
	ja[inew] = ii;
/*<          IF (IS.EQ.0.OR.IBGN.GT.IEND) GO TO 50 >*/
	if (is == 0 || ibgn > iend) {
	    goto L50;
	}
/*<          DO 40 JJ = IBGN,IEND >*/
	i__2 = iend;
	for (jj = ibgn; jj <= i__2; ++jj) {
/*<             JJPI = JJ-IS >*/
	    jjpi = jj - is;
/*<             A(JJPI) = A(JJ) >*/
	    a[jjpi] = a[jj];
/*<             JA(JJPI) = JA(JJ) >*/
	    ja[jjpi] = ja[jj];
/*<    40    CONTINUE >*/
/* L40: */
	}
/*<    50    CONTINUE >*/
L50:
/*<          IA(II) = INEW >*/
	ia[ii] = inew;
/*<    60 CONTINUE >*/
/* L60: */
    }

/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* unscal_ */

/*<       SUBROUTINE VEVMW (N,V,W) >*/
/* Subroutine */ int vevmw_(integer *n, doublereal *v, doublereal *w)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    integer i__, m, mp1;


/* ... VEVMW COMPUTES V = V - W */

/* ... PARAMETER LIST: */

/*          N      INTEGER LENGTH OF VECTORS V AND W */
/*          V      D.P. VECTOR */
/*          W      D.P. VECTOR SUCH THAT   V(I) = V(I) - W(I) */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER N >*/
/*<       DOUBLE PRECISION V(N),W(N) >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER I,M,MP1 >*/

/*<       IF (N.LE.0) RETURN >*/
    /* Parameter adjustments */
    --w;
    --v;

    /* Function Body */
    if (*n <= 0) {
	return 0;
    }
/*<       M = MOD(N,4) >*/
    m = *n % 4;

/*<       IF (M.EQ.0) GO TO 20 >*/
    if (m == 0) {
	goto L20;
    }
/*<       DO 10 I = 1,M >*/
    i__1 = m;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<          V(I) = V(I)-W(I) >*/
	v[i__] -= w[i__];
/*<    10 CONTINUE >*/
/* L10: */
    }
/*<       IF (N.LT.4) RETURN >*/
    if (*n < 4) {
	return 0;
    }

/*<    20 MP1 = M+1 >*/
L20:
    mp1 = m + 1;
/*<       DO 30 I = MP1,N,4 >*/
    i__1 = *n;
    for (i__ = mp1; i__ <= i__1; i__ += 4) {
/*<          V(I) = V(I)-W(I) >*/
	v[i__] -= w[i__];
/*<          V(I+1) = V(I+1)-W(I+1) >*/
	v[i__ + 1] -= w[i__ + 1];
/*<          V(I+2) = V(I+2)-W(I+2) >*/
	v[i__ + 2] -= w[i__ + 2];
/*<          V(I+3) = V(I+3)-W(I+3) >*/
	v[i__ + 3] -= w[i__ + 3];
/*<    30 CONTINUE >*/
/* L30: */
    }
/*<       RETURN >*/
    return 0;

/*<       END >*/
} /* vevmw_ */

/*<       SUBROUTINE VEVPW (N,V,W) >*/
/* Subroutine */ int vevpw_(integer *n, doublereal *v, doublereal *w)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    integer i__, m, mp1;


/* ... VPW COMPUTES    V = V + W */

/* ... PARAMETER LIST: */

/*          N      LENGTH OF VECTORS V AND W */
/*          V      D.P. VECTOR */
/*          W      D.P. VECTOR SUCH THAT   V(I) = V(I) + W(I) */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER N >*/
/*<       DOUBLE PRECISION V(N),W(N) >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER I,M,MP1 >*/

/*<       IF (N.LE.0) RETURN >*/
    /* Parameter adjustments */
    --w;
    --v;

    /* Function Body */
    if (*n <= 0) {
	return 0;
    }

/*<       M = MOD(N,4) >*/
    m = *n % 4;
/*<       IF (M.EQ.0) GO TO 20 >*/
    if (m == 0) {
	goto L20;
    }
/*<       DO 10 I = 1,M >*/
    i__1 = m;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<          V(I) = V(I)+W(I) >*/
	v[i__] += w[i__];
/*<    10 CONTINUE >*/
/* L10: */
    }
/*<       IF (N.LT.4) RETURN >*/
    if (*n < 4) {
	return 0;
    }

/*<    20 MP1 = M+1 >*/
L20:
    mp1 = m + 1;
/*<       DO 30 I = MP1,N,4 >*/
    i__1 = *n;
    for (i__ = mp1; i__ <= i__1; i__ += 4) {
/*<          V(I) = V(I)+W(I) >*/
	v[i__] += w[i__];
/*<          V(I+1) = V(I+1)+W(I+1) >*/
	v[i__ + 1] += w[i__ + 1];
/*<          V(I+2) = V(I+2)+W(I+2) >*/
	v[i__ + 2] += w[i__ + 2];
/*<          V(I+3) = V(I+3)+W(I+3) >*/
	v[i__ + 3] += w[i__ + 3];
/*<    30 CONTINUE >*/
/* L30: */
    }

/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* vevpw_ */

/*<       SUBROUTINE VFILL (N,V,VAL) >*/
/* Subroutine */ int vfill_(integer *n, doublereal *v, doublereal *val)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    integer i__, m, mp1;


/*     FILLS A VECTOR, V, WITH A CONSTANT VALUE, VAL. */

/* ... PARAMETER LIST: */

/*          N      INTEGER LENGTH OF VECTOR V */
/*          V      D.P. VECTOR */
/*          VAL    D.P. CONSTANT THAT FILLS FIRST N LOCATIONS OF V */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER N >*/
/*<       DOUBLE PRECISION V(N),VAL >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER I,M,MP1 >*/

/*<       IF (N.LE.0) RETURN >*/
    /* Parameter adjustments */
    --v;

    /* Function Body */
    if (*n <= 0) {
	return 0;
    }

/*     CLEAN UP LOOP SO REMAINING VECTOR LENGTH IS A MULTIPLE OF 10 */

/*<       M = MOD(N,10) >*/
    m = *n % 10;
/*<       IF (M.EQ.0) GO TO 20 >*/
    if (m == 0) {
	goto L20;
    }
/*<       DO 10 I = 1,M >*/
    i__1 = m;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<          V(I) = VAL >*/
	v[i__] = *val;
/*<    10 CONTINUE >*/
/* L10: */
    }
/*<       IF (N.LT.10) RETURN >*/
    if (*n < 10) {
	return 0;
    }

/*<    20 MP1 = M+1 >*/
L20:
    mp1 = m + 1;
/*<       DO 30 I = MP1,N,10 >*/
    i__1 = *n;
    for (i__ = mp1; i__ <= i__1; i__ += 10) {
/*<          V(I) = VAL >*/
	v[i__] = *val;
/*<          V(I+1) = VAL >*/
	v[i__ + 1] = *val;
/*<          V(I+2) = VAL >*/
	v[i__ + 2] = *val;
/*<          V(I+3) = VAL >*/
	v[i__ + 3] = *val;
/*<          V(I+4) = VAL >*/
	v[i__ + 4] = *val;
/*<          V(I+5) = VAL >*/
	v[i__ + 5] = *val;
/*<          V(I+6) = VAL >*/
	v[i__ + 6] = *val;
/*<          V(I+7) = VAL >*/
	v[i__ + 7] = *val;
/*<          V(I+8) = VAL >*/
	v[i__ + 8] = *val;
/*<          V(I+9) = VAL >*/
	v[i__ + 9] = *val;
/*<    30 CONTINUE >*/
/* L30: */
    }

/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* vfill_ */

/*<       SUBROUTINE VOUT (N,V,ISWT,NOUTT) >*/
/* Subroutine */ int vout_(integer *n, doublereal *v, integer *iswt, integer *
	noutt)
{
    /* Format strings */
    static char fmt_10[] = "(//5x,\002RESIDUAL VECTOR\002)";
    static char fmt_20[] = "(//5x,\002SOLUTION VECTOR\002)";
    static char fmt_30[] = "(10x,8i15)";
    static char fmt_40[] = "(10x,120(\002-\002)/)";
    static char fmt_50[] = "(4x,i5,\002+  \002,8d15.5)";

    /* System generated locals */
    integer i__1, i__2;

    /* Builtin functions */
    integer s_wsfe(cilist *), e_wsfe(), do_fio(integer *, char *, ftnlen);

    /* Local variables */
    integer i__, j, k, jm1, nout, kupper;

    /* Fortran I/O blocks */
    static cilist io___737 = { 0, 0, 0, fmt_10, 0 };
    static cilist io___738 = { 0, 0, 0, fmt_20, 0 };
    static cilist io___739 = { 0, 0, 0, fmt_30, 0 };
    static cilist io___741 = { 0, 0, 0, fmt_40, 0 };
    static cilist io___744 = { 0, 0, 0, fmt_50, 0 };



/*     THIS SUBROUTINE EFFECTS PRINTING OF RESIDUAL AND SOLUTION */
/*     VECTORS - CALLED FROM PERROR */

/* ... PARAMETER LIST: */

/*          V      VECTOR OF LENGTH N */
/*          ISWT   LABELLING INFORMATION */
/*          NOUT OUTPUT DEVICE NUMBER (= NOUTT) */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER N,ISWT,NOUTT >*/
/*<       DOUBLE PRECISION V(N) >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER I,J,JM1,K,KUPPER,NOUT >*/

/*<       NOUT = NOUTT >*/
    /* Parameter adjustments */
    --v;

    /* Function Body */
    nout = *noutt;

/*        IF (N .LE. 0) RETURN */

/*<       KUPPER = MIN0(N,8) >*/
    kupper = min(*n,8);
/*<       IF (ISWT.EQ.1) WRITE (NOUT,10) >*/
    if (*iswt == 1) {
	io___737.ciunit = nout;
	s_wsfe(&io___737);
	e_wsfe();
    }
/*<    10 FORMAT (//5X,'RESIDUAL VECTOR') >*/
/*<       IF (ISWT.EQ.2) WRITE (NOUT,20) >*/
    if (*iswt == 2) {
	io___738.ciunit = nout;
	s_wsfe(&io___738);
	e_wsfe();
    }
/*<    20 FORMAT (//5X,'SOLUTION VECTOR') >*/
/*<       WRITE (NOUT,30) (I,I=1,KUPPER) >*/
    io___739.ciunit = nout;
    s_wsfe(&io___739);
    i__1 = kupper;
    for (i__ = 1; i__ <= i__1; ++i__) {
	do_fio(&c__1, (char *)&i__, (ftnlen)sizeof(integer));
    }
    e_wsfe();
/*<    30 FORMAT (10X,8I15) >*/
/*<       WRITE (NOUT,40) >*/
    io___741.ciunit = nout;
    s_wsfe(&io___741);
    e_wsfe();
/*<    40 FORMAT (10X,120('-')/) >*/

/*<       DO 60 J = 1,N,8 >*/
    i__1 = *n;
    for (j = 1; j <= i__1; j += 8) {
/*<          KUPPER = MIN0(J+7,N) >*/
/* Computing MIN */
	i__2 = j + 7;
	kupper = min(i__2,*n);
/*<          JM1 = J-1 >*/
	jm1 = j - 1;
/*<          WRITE (NOUT,50) JM1,(V(K),K=J,KUPPER) >*/
	io___744.ciunit = nout;
	s_wsfe(&io___744);
	do_fio(&c__1, (char *)&jm1, (ftnlen)sizeof(integer));
	i__2 = kupper;
	for (k = j; k <= i__2; ++k) {
	    do_fio(&c__1, (char *)&v[k], (ftnlen)sizeof(doublereal));
	}
	e_wsfe();
/*<    50    FORMAT (4X,I5,'+  ',8D15.5) >*/
/*<    60 CONTINUE >*/
/* L60: */
    }

/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* vout_ */

/*<       SUBROUTINE WEVMW (N,V,W) >*/
/* Subroutine */ int wevmw_(integer *n, doublereal *v, doublereal *w)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    integer i__, m, mp1;


/* ... WEVMW COMPUTES W = V - W */

/* ... PARAMETER LIST: */

/*          N      INTEGER LENGTH OF VECTORS V AND W */
/*          V      D.P. VECTOR */
/*          W      D.P. VECTOR SUCH THAT   W(I) = V(I) - W(I) */

/* ... SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER N >*/
/*<       DOUBLE PRECISION V(N),W(N) >*/

/* ... SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER I,M,MP1 >*/

/*<       IF (N.LE.0) RETURN >*/
    /* Parameter adjustments */
    --w;
    --v;

    /* Function Body */
    if (*n <= 0) {
	return 0;
    }
/*<       M = MOD(N,4) >*/
    m = *n % 4;
/*<       IF (M.EQ.0) GO TO 20 >*/
    if (m == 0) {
	goto L20;
    }
/*<       DO 10 I = 1,M >*/
    i__1 = m;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<          W(I) = V(I)-W(I) >*/
	w[i__] = v[i__] - w[i__];
/*<    10 CONTINUE >*/
/* L10: */
    }
/*<       IF (N.LT.4) RETURN >*/
    if (*n < 4) {
	return 0;
    }

/*<    20 MP1 = M+1 >*/
L20:
    mp1 = m + 1;
/*<       DO 30 I = MP1,N,4 >*/
    i__1 = *n;
    for (i__ = mp1; i__ <= i__1; i__ += 4) {
/*<          W(I) = V(I)-W(I) >*/
	w[i__] = v[i__] - w[i__];
/*<          W(I+1) = V(I+1)-W(I+1) >*/
	w[i__ + 1] = v[i__ + 1] - w[i__ + 1];
/*<          W(I+2) = V(I+2)-W(I+2) >*/
	w[i__ + 2] = v[i__ + 2] - w[i__ + 2];
/*<          W(I+3) = V(I+3)-W(I+3) >*/
	w[i__ + 3] = v[i__ + 3] - w[i__ + 3];
/*<    30 CONTINUE >*/
/* L30: */
    }

/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* wevmw_ */

/*<       SUBROUTINE ZBRENT (N,TRI,EPS,NSIG,AA,BB,MAXFNN,IER) >*/
/* Subroutine */ int zbrent_(integer *n, doublereal *tri, doublereal *eps, 
	integer *nsig, doublereal *aa, doublereal *bb, integer *maxfnn, 
	integer *ier)
{
    /* Initialized data */

    static doublereal zero = 0.;
    static doublereal half = .5;
    static doublereal one = 1.;
    static doublereal three = 3.;
    static doublereal ten = 10.;

    /* Format strings */
    static char fmt_100[] = "(\0020\002,\002*** W A R N I N G ***********\
*\002/\0020\002,\002    IN ITPACK ROUTINE ZBRENT  \002/\002 \002,\002    ALG\
ORITHM FAILED TO CONVERGE   \002/\002 \002,\002    IN\002,i6,\002 ITERATIONS \
\002)";
    static char fmt_120[] = "(\0020\002,\002*** W A R N I N G ***********\
*\002/\0020\002,\002    IN ITPACK ROUTINE ZBRENT  \002/\002 \002,\002    F(A\
) AND F(B) HAVE SAME SIGN   \002)";

    /* System generated locals */
    integer i__1;
    doublereal d__1;

    /* Builtin functions */
    double pow_di(doublereal *, integer *), d_sign(doublereal *, doublereal *)
	    ;
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    doublereal a, b, c__, d__, e, p, q, r__, s, t, fa, fb, fc;
    integer ic;
    doublereal rm, tol, rone, temp;
    integer maxfn;
    extern doublereal determ_(integer *, doublereal *, doublereal *);

    /* Fortran I/O blocks */
    static cilist io___773 = { 0, 0, 0, fmt_100, 0 };
    static cilist io___774 = { 0, 0, 0, fmt_120, 0 };



/*   MODIFIED IMSL ROUTINE NAME   - ZBRENT */

/* ----------------------------------------------------------------------- */

/*   COMPUTER            - CDC/SINGLE */

/*   LATEST REVISION     - JANUARY 1, 1978 */

/*   PURPOSE             - ZERO OF A FUNCTION WHICH CHANGES SIGN IN A */
/*                           GIVEN INTERVAL (BRENT ALGORITHM) */

/*   USAGE               - CALL ZBRENT (F,EPS,NSIG,A,B,MAXFN,IER) */

/*   ARGUMENTS    TRI    - A TRIDIAGONAL MATRIX OF ORDER N */
/*                EPS    - FIRST CONVERGENCE CRITERION (INPUT).  A ROOT, */
/*                           B, IS ACCEPTED IF DABS(F(B)) IS LESS THAN OR */
/*                           EQUAL TO EPS.  EPS MAY BE SET TO ZERO. */
/*                NSIG   - SECOND CONVERGENCE CRITERION (INPUT).  A ROOT, */
/*                           B, IS ACCEPTED IF THE CURRENT APPROXIMATION */
/*                           AGREES WITH THE TRUE SOLUTION TO NSIG */
/*                           SIGNIFICANT DIGITS. */
/*                A,B    - ON INPUT, THE USER MUST SUPPLY TWO POINTS, A */
/*                           AND B, SUCH THAT F(A) AND F(B) ARE OPPOSITE */
/*                           IN SIGN. (= AA, BB) */
/*                           ON OUTPUT, BOTH A AND B ARE ALTERED.  B */
/*                           WILL CONTAIN THE BEST APPROXIMATION TO THE */
/*                           ROOT OF F. SEE REMARK 1. */
/*                MAXFN  - ON INPUT, MAXFN SHOULD CONTAIN AN UPPER BOUND */
/*                           ON THE NUMBER OF FUNCTION EVALUATIONS */
/*                           REQUIRED FOR CONVERGENCE.  ON OUTPUT, MAXFN */
/*                           WILL CONTAIN THE ACTUAL NUMBER OF FUNCTION */
/*                           EVALUATIONS USED. (= MAXFNN) */
/*                IER    - ERROR PARAMETER. (OUTPUT) */
/*                         TERMINAL ERROR */
/*                           IER = 501 INDICATES THE ALGORITHM FAILED TO */
/*                             CONVERGE IN MAXFN EVALUATIONS. */
/*                           IER = 502 INDICATES F(A) AND F(B) HAVE THE */
/*                             SAME SIGN. */

/*   PRECISION/HARDWARE  - SINGLE AND DOUBLE/H32 */
/*                       - SINGLE/H36,H48,H60 */

/*   NOTATION            - INFORMATION ON SPECIAL NOTATION AND */
/*                           CONVENTIONS IS AVAILABLE IN THE MANUAL */
/*                           INTRODUCTION OR THROUGH IMSL ROUTINE UHELP */

/*   REMARKS  1.  LET F(X) BE THE CHARACTERISTIC FUNCTION OF THE MATRIX */
/*                TRI EVALUATED AT X. FUNCTION DETERM EVALUATES F(X). */
/*                ON EXIT FROM ZBRENT, WHEN IER=0, A AND B SATISFY THE */
/*                FOLLOWING, */
/*                F(A)*F(B) .LE.0, */
/*                DABS(F(B)) .LE. DABS(F(A)), AND */
/*                EITHER DABS(F(B)) .LE. EPS OR */
/*                DABS(A-B) .LE. MAX(DABS(B),0.1)*10.0**(-NSIG). */
/*                THE PRESENCE OF 0.1 IN THIS ERROR CRITERION CAUSES */
/*                LEADING ZEROES TO THE RIGHT OF THE DECIMAL POINT TO BE */
/*                COUNTED AS SIGNIFICANT DIGITS. SCALING MAY BE REQUIRED */
/*                IN ORDER TO ACCURATELY DETERMINE A ZERO OF SMALL */
/*                MAGNITUDE. */
/*            2.  ZBRENT IS GUARANTEED TO REACH CONVERGENCE WITHIN */
/*                K = (DLOG((B-A)/D)+1.0)**2 FUNCTION EVALUATIONS WHERE */
/*                  D=MIN(OVER X IN (A,B) OF */
/*                    MAX(DABS(X),0.1)*10.0**(-NSIG)). */
/*                THIS IS AN UPPER BOUND ON THE NUMBER OF EVALUATIONS. */
/*                RARELY DOES THE ACTUAL NUMBER OF EVALUATIONS USED BY */
/*                ZBRENT EXCEED DSQRT(K). D CAN BE COMPUTED AS FOLLOWS, */
/*                  P = DBLE(AMIN1(DABS(A),DABS(B))) */
/*                  P = DMAX1(0.1,P) */
/*                  IF ((A-0.1)*(B-0.1).LT.0.0) P = 0.1 */
/*                  D = P*10.0**(-NSIG) */

/*   COPYRIGHT           - 1977 BY IMSL, INC. ALL RIGHTS RESERVED. */

/*   WARRANTY            - IMSL WARRANTS ONLY THAT IMSL TESTING HAS BEEN */
/*                           APPLIED TO THIS CODE. NO OTHER WARRANTY, */
/*                           EXPRESSED OR IMPLIED, IS APPLICABLE. */

/* ----------------------------------------------------------------------- */

/* *** BEGIN: ITPACK COMMON */

/*<       INTEGER IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/
/*<       COMMON /ITCOM1/ IN,IS,ISYM,ITMAX,LEVEL,NOUT >*/

/*<       LOGICAL ADAPT,BETADT,CASEII,HALT,PARTAD >*/
/*<       COMMON /ITCOM2/ ADAPT,BETADT,CASEII,HALT,PARTAD >*/

/*<    >*/
/*<    >*/

/* *** END  : ITPACK COMMON */

/*     DESCRIPTION OF VARIABLES IN COMMON BLOCK IN MAIN SUBROUTINE */

/*                                  SPECIFICATIONS FOR ARGUMENTS */

/*<       INTEGER NSIG,MAXFNN,IER >*/
/*<       DOUBLE PRECISION TRI(2,1),EPS,AA,BB >*/

/*                                  SPECIFICATIONS FOR LOCAL VARIABLES */

/*<       INTEGER IC,MAXFN >*/
/*<    >*/
/*<    >*/
    /* Parameter adjustments */
    tri -= 3;

    /* Function Body */

/*                                  FIRST EXECUTABLE STATEMENT */

/*<       A = AA >*/
    a = *aa;
/*<       B = BB >*/
    b = *bb;
/*<       MAXFN = MAXFNN >*/
    maxfn = *maxfnn;
/*<       IER = 0 >*/
    *ier = 0;
/*<       T = TEN**(-NSIG) >*/
    i__1 = -(*nsig);
    t = pow_di(&ten, &i__1);
/*<       IC = 2 >*/
    ic = 2;
/*<       FA = DETERM(N,TRI,A) >*/
    fa = determ_(n, &tri[3], &a);
/*<       FB = DETERM(N,TRI,B) >*/
    fb = determ_(n, &tri[3], &b);
/*<       S = B >*/
    s = b;

/*                                  TEST FOR SAME SIGN */

/*<       IF (FA*FB.GT.ZERO) GO TO 110 >*/
    if (fa * fb > zero) {
	goto L110;
    }
/*<    10 C = A >*/
L10:
    c__ = a;
/*<       FC = FA >*/
    fc = fa;
/*<       D = B-C >*/
    d__ = b - c__;
/*<       E = D >*/
    e = d__;
/*<    20 IF (DABS(FC).GE.DABS(FB)) GO TO 30 >*/
L20:
    if (abs(fc) >= abs(fb)) {
	goto L30;
    }
/*<       A = B >*/
    a = b;
/*<       B = C >*/
    b = c__;
/*<       C = A >*/
    c__ = a;
/*<       FA = FB >*/
    fa = fb;
/*<       FB = FC >*/
    fb = fc;
/*<       FC = FA >*/
    fc = fa;
/*<    30 CONTINUE >*/
L30:
/*<       TOL = T*DMAX1(DABS(B),0.1D0) >*/
/* Computing MAX */
    d__1 = abs(b);
    tol = t * max(d__1,.1);
/*<       RM = (C-B)*HALF >*/
    rm = (c__ - b) * half;

/*                                  TEST FOR FIRST CONVERGENCE CRITERIA */

/*<       IF (DABS(FB).LE.EPS) GO TO 80 >*/
    if (abs(fb) <= *eps) {
	goto L80;
    }

/*                                  TEST FOR SECOND CONVERGENCE CRITERIA */

/*<       IF (DABS(C-B).LE.TOL) GO TO 80 >*/
    if ((d__1 = c__ - b, abs(d__1)) <= tol) {
	goto L80;
    }

/*                                  CHECK EVALUATION COUNTER */

/*<       IF (IC.GE.MAXFN) GO TO 90 >*/
    if (ic >= maxfn) {
	goto L90;
    }

/*                                  IS BISECTION FORCED */

/*<       IF (DABS(E).LT.TOL) GO TO 60 >*/
    if (abs(e) < tol) {
	goto L60;
    }
/*<       IF (DABS(FA).LE.DABS(FB)) GO TO 60 >*/
    if (abs(fa) <= abs(fb)) {
	goto L60;
    }
/*<       S = FB/FA >*/
    s = fb / fa;
/*<       IF (A.NE.C) GO TO 40 >*/
    if (a != c__) {
	goto L40;
    }

/*                                  LINEAR INTERPOLATION */

/*<       P = (C-B)*S >*/
    p = (c__ - b) * s;
/*<       Q = ONE-S >*/
    q = one - s;
/*<       GO TO 50 >*/
    goto L50;

/*                                  INVERSE QUADRATIC INTERPOLATION */

/*<    40 Q = FA/FC >*/
L40:
    q = fa / fc;
/*<       R = FB/FC >*/
    r__ = fb / fc;
/*<       RONE = R-ONE >*/
    rone = r__ - one;
/*<       P = S*((C-B)*Q*(Q-R)-(B-A)*RONE) >*/
    p = s * ((c__ - b) * q * (q - r__) - (b - a) * rone);
/*<       Q = (Q-ONE)*RONE*(S-ONE) >*/
    q = (q - one) * rone * (s - one);
/*<    50 IF (P.GT.ZERO) Q = -Q >*/
L50:
    if (p > zero) {
	q = -q;
    }
/*<       IF (P.LT.ZERO) P = -P >*/
    if (p < zero) {
	p = -p;
    }
/*<       S = E >*/
    s = e;
/*<       E = D >*/
    e = d__;

/*                                  IF DABS(P/Q).GE.75*DABS(C-B) THEN */
/*                                     FORCE BISECTION */

/*<       IF (P+P.GE.THREE*RM*Q) GO TO 60 >*/
    if (p + p >= three * rm * q) {
	goto L60;
    }

/*                                  IF DABS(P/Q).GE..5*DABS(S) THEN FORCE */
/*                                     BISECTION. S = THE VALUE OF P/Q */
/*                                     ON THE STEP BEFORE THE LAST ONE */

/*<       IF (P+P.GE.DABS(S*Q)) GO TO 60 >*/
    if (p + p >= (d__1 = s * q, abs(d__1))) {
	goto L60;
    }
/*<       D = P/Q >*/
    d__ = p / q;
/*<       GO TO 70 >*/
    goto L70;

/*                                  BISECTION */

/*<    60 E = RM >*/
L60:
    e = rm;
/*<       D = E >*/
    d__ = e;

/*                                  INCREMENT B */

/*<    70 A = B >*/
L70:
    a = b;
/*<       FA = FB >*/
    fa = fb;
/*<       TEMP = D >*/
    temp = d__;
/*<       IF (DABS(TEMP).LE.HALF*TOL) TEMP = DSIGN(HALF*TOL,RM) >*/
    if (abs(temp) <= half * tol) {
	d__1 = half * tol;
	temp = d_sign(&d__1, &rm);
    }
/*<       B = B+TEMP >*/
    b += temp;
/*<       S = B >*/
    s = b;
/*<       FB = DETERM(N,TRI,S) >*/
    fb = determ_(n, &tri[3], &s);
/*<       IC = IC+1 >*/
    ++ic;
/*<       IF (FB*FC.LE.ZERO) GO TO 20 >*/
    if (fb * fc <= zero) {
	goto L20;
    }
/*<       GO TO 10 >*/
    goto L10;

/*                                  CONVERGENCE OF B */

/*<    80 A = C >*/
L80:
    a = c__;
/*<       MAXFN = IC >*/
    maxfn = ic;
/*<       GO TO 130 >*/
    goto L130;

/*                                  MAXFN EVALUATIONS */

/*<    90 IER = 501 >*/
L90:
    *ier = 501;
/*<       A = C >*/
    a = c__;
/*<       MAXFN = IC >*/
    maxfn = ic;
/*<       IF (LEVEL.GE.1) WRITE (NOUT,100) MAXFN >*/
    if (itcom1_1.level >= 1) {
	io___773.ciunit = itcom1_1.nout;
	s_wsfe(&io___773);
	do_fio(&c__1, (char *)&maxfn, (ftnlen)sizeof(integer));
	e_wsfe();
    }
/*<    >*/
/*<       GO TO 130 >*/
    goto L130;

/*                                  TERMINAL ERROR - F(A) AND F(B) HAVE */
/*                                  THE SAME SIGN */

/*<   110 IER = 502 >*/
L110:
    *ier = 502;
/*<       MAXFN = IC >*/
    maxfn = ic;
/*<       IF (LEVEL.GE.1) WRITE (NOUT,120) >*/
    if (itcom1_1.level >= 1) {
	io___774.ciunit = itcom1_1.nout;
	s_wsfe(&io___774);
	e_wsfe();
    }
/*<    >*/
/*<   130 CONTINUE >*/
L130:
/*<       AA = A >*/
    *aa = a;
/*<       BB = B >*/
    *bb = b;
/*<       MAXFNN = MAXFN >*/
    *maxfnn = maxfn;
/*<       RETURN >*/
    return 0;
/*<       END >*/
} /* zbrent_ */

#ifdef __cplusplus
	}
#endif
