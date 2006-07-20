/* itpack-test.f -- translated by f2c (version 20050501).
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
#include "f2c.h"

/* Common Block Declarations */

struct {
    doublereal ax, ay, bx, by, drelpr, hx, hy;
} tbk11_;

#define tbk11_1 tbk11_

struct {
    integer bctype[4], ilevel, isym, nout, ngridx, ngridy, n;
} tbk12_;

#define tbk12_1 tbk12_

struct {
    doublereal ax, ay, az, bx, by, bz, drelpr, hx, hy, hz;
} tbk21_;

#define tbk21_1 tbk21_

struct {
    integer bctype[6], ilevel, isym, nout, ngridx, ngridy, ngridz, n;
} tbk22_;

#define tbk22_1 tbk22_

struct {
    doublereal drelpr, zeta;
} tbk31_;

#define tbk31_1 tbk31_

struct {
    integer idebug, ieran, ilevel, iord, ipct, iplt, iseed, isym, itmax, 
	    large, level, maxnz, maxn, nout, nblack, nb, nrbn, nred, nw, 
	    nzblk, nzred, n;
} tbk32_;

#define tbk32_1 tbk32_

/* Table of constant values */

static doublereal c_b19 = 0.;
static doublereal c_b27 = 1.;
static real c_b28 = (float)0.;
static integer c__1 = 1;
static integer c__3 = 3;
static integer c__4 = 4;
static integer c__2 = 2;
static integer c__0 = 0;
static integer c__5 = 5;
static integer c__6 = 6;
static integer c_n1 = -1;
static doublereal c_b388 = 2147483647.;
static doublereal c_b445 = .05;

/*<       PROGRAM ITPTST (OUTPUT,TAPE6=OUTPUT)       >*/
/* Main program */ int MAIN__()
{
    /* Builtin functions */
    /* Subroutine */ int s_stop(char *, ftnlen);

    /* Local variables */
    extern /* Subroutine */ int test1d_(doublereal *), test2d_(doublereal *), 
	    test3d_(doublereal *);
    doublereal drelpr;


/*     CHANGES TO BE MADE FOR USE ON DIFFERENT COMPUTERS: */
/*     1. REMOVE OR CHANGE PROGRAM LINE ABOVE OR OPEN LINE BELOW */
/*     2. CHANGE THE VALUE OF DRELPR BELOW AND IN ITPACK ROUTINE DFAULT */
/*     3. CHANGES IN THE ITPACK TIMING ROUTINE TIMER */

/*     OPEN(UNIT=6,DEVICE='DSK',ACCESS='SEQOUT',FILE='OUT.LPT') */

/*     MACHINE PRECISION  DRELPR */

/*     DRELPR = 1.26D-29  FOR CDC CYBER 170/750  (APPROX.) 2**-96 */
/*            = 2.22D-16  FOR DEC 10             (APPROX.) 2**-52 */
/*            = 7.11D-15  FOR VAX 11/780         (APPROX.) 2**-47 */
/*            = 1.14D-13  FOR IBM 370/158        (APPROX.) 2**-43 */

/*<       DOUBLE PRECISION DRELPR  >*/
/*<       DRELPR = 1.26D-29      >*/
    drelpr = 1.26e-29;
/*<       CALL TEST1D (DRELPR)   >*/
    test1d_(&drelpr);
/*<       CALL TEST2D (DRELPR)   >*/
    test2d_(&drelpr);
/*<       CALL TEST3D (DRELPR)   >*/
    test3d_(&drelpr);

/*     CALL TEST4D(DRELPR) */

/*<       STOP >*/
    s_stop("", (ftnlen)0);
/*<       END  >*/
    return 0;
} /* MAIN__ */

/*<       SUBROUTINE TEST1D (EPSI) >*/
/* Subroutine */ int test1d_(doublereal *epsi)
{
    /* Format strings */
    static char fmt_10[] = "(\0021\002//15x,\002ITPACK 2C  TEST PROGRAM -- T\
EST1D\002/15x,\002TESTS ITERATIVE MODULES\002/15x,\002USES FIVE POINT SYMMET\
RIC DISCRETIZATION\002)";
    static char fmt_50[] = "(\0021\002///)";
    static char fmt_70[] = "(//15x,\002SYMMETRIC SPARSE STORAGE USED\002)";
    static char fmt_80[] = "(//15x,\002NONSYMMETRIC SPARSE STORAGE USED\002)";
    static char fmt_90[] = "(15x,\002NATURAL ORDERING USED\002)";
    static char fmt_100[] = "(15x,\002RED-BLACK ORDERING USED\002)";

    /* System generated locals */
    integer i__1, i__2;

    /* Builtin functions */
    integer s_wsfe(cilist *), e_wsfe();

    /* Local variables */
    doublereal a[1729];
    integer i__, j;
    doublereal u[361];
    integer ia[362], ja[1729], nb, nw;
    extern /* Subroutine */ int jcg_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *);
    integer ier;
    extern /* Subroutine */ int jsi_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *);
    doublereal rhs[361];
    extern /* Subroutine */ int sor_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *);
    doublereal tim1[7], tim2[7];
    extern /* Subroutine */ int rscg_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *);
    doublereal zeta;
    integer iter[7], iwrk[7];
    extern /* Subroutine */ int rssi_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *);
    doublereal wksp[2606];
    extern /* Subroutine */ int time1_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *, doublereal *,
	     doublereal *, doublereal *, doublereal *, integer *, integer *);
    integer loop1, loop2, ieran, level, iparm[12];
    extern /* Subroutine */ int vfill_(integer *, doublereal *, doublereal *);
    doublereal gridx[21], gridy[21], rparm[12];
    integer itmax, itest, jtest, mxneq, iwork[1729], iwksp[1083];
    doublereal digit1[7], digit2[7];
    extern /* Subroutine */ int sym5pt_(doublereal *, integer *, doublereal *,
	     integer *, doublereal *, integer *, integer *, integer *, 
	    doublereal *, integer *, integer *), dfault_(integer *, 
	    doublereal *);
    integer nelmax, ngrdxd, ngrdyd;
    extern /* Subroutine */ int ssorcg_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *), ssorsi_(
	    integer *, integer *, integer *, doublereal *, doublereal *, 
	    doublereal *, integer *, integer *, doublereal *, integer *, 
	    doublereal *, integer *);

    /* Fortran I/O blocks */
    static cilist io___13 = { 0, 0, 0, fmt_10, 0 };
    static cilist io___19 = { 0, 0, 0, fmt_50, 0 };
    static cilist io___27 = { 0, 0, 0, fmt_70, 0 };
    static cilist io___28 = { 0, 0, 0, fmt_80, 0 };
    static cilist io___29 = { 0, 0, 0, fmt_90, 0 };
    static cilist io___30 = { 0, 0, 0, fmt_100, 0 };



/* ... TEST1D IS A PROGRAM DESIGNED TO TEST ITPACK 2C METHODS ON */
/* ... MATRICES ARISING FROM THE SYMMETRIC FIVE POINT DISCRETIZATION */
/* ... OF TWO DIMENSIONAL ELLIPTIC PARTIAL DIFFERENTIAL EQUATIONS ON */
/* ... A RECTANGLE WITH A RECTANGULAR MESH.  ALL SEVEN METHODS FROM */
/* ... ITPACK 2C ARE TESTED AND A SUMMARY IS PRINTED AT THE END. */

/*     THE EXACT SIZE OF THE TEST PROBLEMS CAN BE INCREASED OR */
/*     DECREASED BY CHANGING THE ARRAY SIZE IN DIMENSION STATEMENTS */
/*     AND THE VARIABLES LISTED BELOW UNDER SIZE OF TEST PROBLEM. */
/*     ALSO, THE NUMBER OF TIMES THROUGH THE TEST LOOPS CAN BE REDUCED */
/*     BY CHANGING  ITEST AND JTEST  AS FOLLOWS. */

/*          ITEST = 1 FOR SYMMETRIC STORAGE TEST */
/*                = 2 FOR SYMMETRIC AND NONSYMMETRIC STORAGE TEST */
/*          JTEST = 1 FOR NATURAL ORDERING TEST */
/*                = 2 FOR NATURAL AND RED-BLACK ORDERING TEST */

/*     ARRAY DIMENSIONING */

/*<    >*/
/*<       DOUBLE PRECISION ZETA,EPSI,AX,AY,BX,BY,DRELPR,HX,HY  >*/
/*<    >*/

/* ... SYM5PT COMMON BLOCKS */

/*<       COMMON /TBK11/ AX,AY,BX,BY,DRELPR,HX,HY    >*/
/*<       COMMON /TBK12/ BCTYPE,ILEVEL,ISYM,NOUT,NGRIDX,NGRIDY,N >*/

/* ... INITIALIZE INTEGER CONSTANTS WHICH CONTROL OUTPUT AND DEFINE */
/* ... ARRAY DIMENSIONS. THEY ARE */

/*        DRELPR    -  MACHINE PRECISION */
/*        NOUT    -  FORTRAN OUTPUT UNIT */
/*        LEVEL   -  LEVEL OF OUTPUT FROM ITPACK 2C */
/*        IERAN    -  ERROR ANALYSIS SWITCH */
/*        ITMAX   -  MAXIMUM NUMBER OF ITERATIONS ALLOWED */
/*        ZETA    -  STOPPING CRITERION */
/*        NW      -  SIZE OF THE DOUBLE PRECISION ARRAY WKSP. */
/*        ILEVEL  -  0/1 LEVEL OF OUTPUT FROM SYM7PT */
/*        MXNEQ   -  MAXIMUM NUMBER OF EQUATIONS TO BE GENERATED (I.E. */
/*                   THE MAXIMUM NUMBER OF INTERIOR AND NON-DIRICHLET */
/*                   BOUNDARY POINTS) */
/*        NELMAX  -  MAXIMUM NUMBER OF NON-ZERO ENTRIES IN THE UPPER */
/*                   TRIANGULAR PART OF THE RESULTING SYMMETRIC MATRIX */
/*        NGRIDX  -  NUMBER OF HORIZONTAL MESH PLANES. */
/*        NGRIDY  -  NUMBER OF VERTICAL MESH PLANES. */
/*        NGRDXD  -  MAXIMUM NUMBER OF VERTICAL MESH PLANES INCLUDING THE */
/*                   VERTICAL BOUNDARY PLANES. */
/*        NGRDYD  -  MAXIMUM NUMBER OF HORIZONTAL MESH PLANES INCLUDING */
/*                   THE HORIZONTAL BOUNDARY PLANES. */

/*<       DRELPR = EPSI  >*/
    tbk11_1.drelpr = *epsi;

/*     SET ITPACK SWITCHES */

/*<       NOUT = 6     >*/
    tbk12_1.nout = 6;
/*<       LEVEL = 1    >*/
    level = 1;
/*<       IERAN = 0    >*/
    ieran = 0;
/*<       ITMAX = 110  >*/
    itmax = 110;
/*<       ZETA = 0.5D-5  >*/
    zeta = 5e-6;
/*<       NW = 2606    >*/
    nw = 2606;

/*     SET NUMBER OF TIMES THROUGH TEST LOOPS */

/*<       ITEST = 2    >*/
    itest = 2;
/*<       JTEST = 2    >*/
    jtest = 2;

/*     SET SIZE OF TEST PROBLEM */

/*<       ILEVEL = 0   >*/
    tbk12_1.ilevel = 0;
/*<       MXNEQ = 361  >*/
    mxneq = 361;
/*<       NELMAX = 1729  >*/
    nelmax = 1729;
/*<       NGRIDX = 21  >*/
    tbk12_1.ngridx = 21;
/*<       NGRIDY = 21  >*/
    tbk12_1.ngridy = 21;

/*<       NGRDXD = NGRIDX        >*/
    ngrdxd = tbk12_1.ngridx;
/*<       NGRDYD = NGRIDY        >*/
    ngrdyd = tbk12_1.ngridy;
/*<       WRITE (NOUT,10)        >*/
    io___13.ciunit = tbk12_1.nout;
    s_wsfe(&io___13);
    e_wsfe();
/*<    >*/

/* ... SET UP DEFAULT VALUES FOR BCTYPE AND INITIALIZE INFORMATION ARRAYS */

/* ... BCTYPE DEFINES THE BOUNDARY CONDITIONS ON THE EDGES OF THE */
/* ... RECTANGLE.  WHERE */

/*        BCTYPE(I) = 0 IMPLIES THE ITH SIDE IS NEUMANN OR MIXED */
/*                  = 1 IMPLIES THE ITH SIDE IS DIRICHLET */

/*          AND I = 1 IMPLIES THE EAST  SIDE DEFINED BY (BX, Y) */
/*                = 2 IMPLIES THE SOUTH SIDE DEFINED BY ( X,AY) */
/*                = 3 IMPLIES THE WEST  SIDE DEFINED BY (AX, Y) */
/*                = 4 IMPLIES THE NORTH SIDE DEFINED BY ( X,BY) */

/*<       DO 20 I = 1,4  >*/
    for (i__ = 1; i__ <= 4; ++i__) {
/*<          BCTYPE(I) = 1       >*/
	tbk12_1.bctype[i__ - 1] = 1;
/*<    20 CONTINUE     >*/
/* L20: */
    }

/* ... DEFINE THE DISCRETIZATION MESH */

/*       AX      -  MINIMUM X VALUE ON THE RECTANGLE (WEST SIDE) */
/*       BX      -  MAXIMUM X VALUE ON THE RECTANGLE (EAST SIDE) */
/*       GRIDX   -  DOUBLE PRECISION ARRAY CONTAINING THE X-COORDINATE OF */
/*                  HORIZONTAL MESH LINES FROM WEST TO EAST. */
/*                  THESE ARE UNIFORM BUT THAT IS NOT REQUIRED. */
/*       AY      -  MINIMUM Y VALUE ON THE RECTANGLE (SOUTH SIDE) */
/*       BY      -  MAXIMUM Y VALUE ON THE RECTANGLE (NORTH SIDE) */
/*       GRIDY   -  DOUBLE PRECISION ARRAY CONTAINING THE Y-COORDINATE OF */
/*                  VERTICAL MESH LINES FROM SOUTH TO NORTH. */
/*                  THESE ARE  UNIFORM BUT THAT IS NOT REQUIRED. */

/*<       AX = 0.D0    >*/
    tbk11_1.ax = 0.;
/*<       BX = 1.D0    >*/
    tbk11_1.bx = 1.;
/*<       HX = (BX-AX)/DBLE(FLOAT(NGRIDX-1)) >*/
    tbk11_1.hx = (tbk11_1.bx - tbk11_1.ax) / (doublereal) ((real) (
	    tbk12_1.ngridx - 1));
/*<       DO 30 J = 1,NGRIDX     >*/
    i__1 = tbk12_1.ngridx;
    for (j = 1; j <= i__1; ++j) {
/*<          GRIDX(J) = AX+DBLE(FLOAT(J-1))*HX       >*/
	gridx[j - 1] = tbk11_1.ax + (doublereal) ((real) (j - 1)) * 
		tbk11_1.hx;
/*<    30 CONTINUE     >*/
/* L30: */
    }
/*<       GRIDX(NGRIDX) = BX     >*/
    gridx[tbk12_1.ngridx - 1] = tbk11_1.bx;

/*<       AY = 0.D0    >*/
    tbk11_1.ay = 0.;
/*<       BY = 1.D0    >*/
    tbk11_1.by = 1.;
/*<       HY = (BY-AY)/DBLE(FLOAT(NGRIDY-1)) >*/
    tbk11_1.hy = (tbk11_1.by - tbk11_1.ay) / (doublereal) ((real) (
	    tbk12_1.ngridy - 1));
/*<       DO 40 J = 1,NGRIDY     >*/
    i__1 = tbk12_1.ngridy;
    for (j = 1; j <= i__1; ++j) {
/*<          GRIDY(J) = AY+DBLE(FLOAT(J-1))*HY       >*/
	gridy[j - 1] = tbk11_1.ay + (doublereal) ((real) (j - 1)) * 
		tbk11_1.hy;
/*<    40 CONTINUE     >*/
/* L40: */
    }
/*<       GRIDY(NGRIDY) = BY     >*/
    gridy[tbk12_1.ngridy - 1] = tbk11_1.by;

/* ... DISCRETIZE THE ELLIPTIC PDE */

/*<       DO 60 LOOP1 = 1,ITEST  >*/
    i__1 = itest;
    for (loop1 = 1; loop1 <= i__1; ++loop1) {
/*<          ISYM = LOOP1-1      >*/
	tbk12_1.isym = loop1 - 1;
/*<          IF (LOOP1.EQ.2) WRITE (NOUT,50) >*/
	if (loop1 == 2) {
	    io___19.ciunit = tbk12_1.nout;
	    s_wsfe(&io___19);
	    e_wsfe();
	}
/*<    50    FORMAT ('1'///)     >*/
/*<    >*/
	sym5pt_(gridx, &ngrdxd, gridy, &ngrdyd, rhs, &mxneq, ia, ja, a, &
		nelmax, iwork);

/* ... SOLVE THE MATRIX PROBLEM */

/*<          DO 60 LOOP2 = 1,JTEST >*/
	i__2 = jtest;
	for (loop2 = 1; loop2 <= i__2; ++loop2) {
/*<             NB = LOOP2-2     >*/
	    nb = loop2 - 2;
/*<             IF (ISYM.EQ.0) WRITE (NOUT,70)       >*/
	    if (tbk12_1.isym == 0) {
		io___27.ciunit = tbk12_1.nout;
		s_wsfe(&io___27);
		e_wsfe();
	    }
/*<             IF (ISYM.EQ.1) WRITE (NOUT,80)       >*/
	    if (tbk12_1.isym == 1) {
		io___28.ciunit = tbk12_1.nout;
		s_wsfe(&io___28);
		e_wsfe();
	    }
/*<             IF (NB.EQ.(-1)) WRITE (NOUT,90)      >*/
	    if (nb == -1) {
		io___29.ciunit = tbk12_1.nout;
		s_wsfe(&io___29);
		e_wsfe();
	    }
/*<             IF (NB.EQ.0) WRITE (NOUT,100)        >*/
	    if (nb == 0) {
		io___30.ciunit = tbk12_1.nout;
		s_wsfe(&io___30);
		e_wsfe();
	    }

/*        TEST JCG */

/*<             CALL DFAULT (IPARM,RPARM)  >*/
	    dfault_(iparm, rparm);
/*<             IPARM(1) = ITMAX >*/
	    iparm[0] = itmax;
/*<             IPARM(2) = LEVEL >*/
	    iparm[1] = level;
/*<             IPARM(5) = ISYM  >*/
	    iparm[4] = tbk12_1.isym;
/*<             IPARM(9) = NB    >*/
	    iparm[8] = nb;
/*<             IPARM(12) = IERAN  >*/
	    iparm[11] = ieran;
/*<             RPARM(1) = ZETA  >*/
	    rparm[0] = zeta;
/*<             CALL VFILL (N,U,0.D0)      >*/
	    vfill_(&tbk12_1.n, u, &c_b19);
/*<             CALL JCG (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER) >*/
	    jcg_(&tbk12_1.n, ia, ja, a, rhs, u, iwksp, &nw, wksp, iparm, 
		    rparm, &ier);
/*<             TIM1(1) = RPARM(9) >*/
	    tim1[0] = rparm[8];
/*<             TIM2(1) = RPARM(10)        >*/
	    tim2[0] = rparm[9];
/*<             DIGIT1(1) = RPARM(11)      >*/
	    digit1[0] = rparm[10];
/*<             DIGIT2(1) = RPARM(12)      >*/
	    digit2[0] = rparm[11];
/*<             ITER(1) = IPARM(1) >*/
	    iter[0] = iparm[0];
/*<             IWRK(1) = IPARM(8) >*/
	    iwrk[0] = iparm[7];

/*        TEST JSI */

/*<             CALL DFAULT (IPARM,RPARM)  >*/
	    dfault_(iparm, rparm);
/*<             IPARM(1) = ITMAX >*/
	    iparm[0] = itmax;
/*<             IPARM(2) = LEVEL >*/
	    iparm[1] = level;
/*<             IPARM(5) = ISYM  >*/
	    iparm[4] = tbk12_1.isym;
/*<             IPARM(9) = NB    >*/
	    iparm[8] = nb;
/*<             IPARM(12) = IERAN  >*/
	    iparm[11] = ieran;
/*<             RPARM(1) = ZETA  >*/
	    rparm[0] = zeta;
/*<             CALL VFILL (N,U,0.D0)      >*/
	    vfill_(&tbk12_1.n, u, &c_b19);
/*<             CALL JSI (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER) >*/
	    jsi_(&tbk12_1.n, ia, ja, a, rhs, u, iwksp, &nw, wksp, iparm, 
		    rparm, &ier);
/*<             TIM1(2) = RPARM(9) >*/
	    tim1[1] = rparm[8];
/*<             TIM2(2) = RPARM(10)        >*/
	    tim2[1] = rparm[9];
/*<             DIGIT1(2) = RPARM(11)      >*/
	    digit1[1] = rparm[10];
/*<             DIGIT2(2) = RPARM(12)      >*/
	    digit2[1] = rparm[11];
/*<             ITER(2) = IPARM(1) >*/
	    iter[1] = iparm[0];
/*<             IWRK(2) = IPARM(8) >*/
	    iwrk[1] = iparm[7];

/*        TEST SOR */

/*<             CALL DFAULT (IPARM,RPARM)  >*/
	    dfault_(iparm, rparm);
/*<             IPARM(1) = ITMAX >*/
	    iparm[0] = itmax;
/*<             IPARM(2) = LEVEL >*/
	    iparm[1] = level;
/*<             IPARM(5) = ISYM  >*/
	    iparm[4] = tbk12_1.isym;
/*<             IPARM(9) = NB    >*/
	    iparm[8] = nb;
/*<             IPARM(12) = IERAN  >*/
	    iparm[11] = ieran;
/*<             RPARM(1) = ZETA  >*/
	    rparm[0] = zeta;
/*<             CALL VFILL (N,U,0.D0)      >*/
	    vfill_(&tbk12_1.n, u, &c_b19);
/*<             CALL SOR (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER) >*/
	    sor_(&tbk12_1.n, ia, ja, a, rhs, u, iwksp, &nw, wksp, iparm, 
		    rparm, &ier);
/*<             TIM1(3) = RPARM(9) >*/
	    tim1[2] = rparm[8];
/*<             TIM2(3) = RPARM(10)        >*/
	    tim2[2] = rparm[9];
/*<             DIGIT1(3) = RPARM(11)      >*/
	    digit1[2] = rparm[10];
/*<             DIGIT2(3) = RPARM(12)      >*/
	    digit2[2] = rparm[11];
/*<             ITER(3) = IPARM(1) >*/
	    iter[2] = iparm[0];
/*<             IWRK(3) = IPARM(8) >*/
	    iwrk[2] = iparm[7];

/*        TEST SSORCG */

/*<             CALL DFAULT (IPARM,RPARM)  >*/
	    dfault_(iparm, rparm);
/*<             IPARM(1) = ITMAX >*/
	    iparm[0] = itmax;
/*<             IPARM(2) = LEVEL >*/
	    iparm[1] = level;
/*<             IPARM(5) = ISYM  >*/
	    iparm[4] = tbk12_1.isym;
/*<             IPARM(9) = NB    >*/
	    iparm[8] = nb;
/*<             IPARM(12) = IERAN  >*/
	    iparm[11] = ieran;
/*<             RPARM(1) = ZETA  >*/
	    rparm[0] = zeta;
/*<             CALL VFILL (N,U,0.D0)      >*/
	    vfill_(&tbk12_1.n, u, &c_b19);
/*<             CALL SSORCG (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER)  >*/
	    ssorcg_(&tbk12_1.n, ia, ja, a, rhs, u, iwksp, &nw, wksp, iparm, 
		    rparm, &ier);
/*<             ITER(4) = IPARM(1) >*/
	    iter[3] = iparm[0];
/*<             IWRK(4) = IPARM(8) >*/
	    iwrk[3] = iparm[7];
/*<             TIM1(4) = RPARM(9) >*/
	    tim1[3] = rparm[8];
/*<             TIM2(4) = RPARM(10)        >*/
	    tim2[3] = rparm[9];
/*<             DIGIT1(4) = RPARM(11)      >*/
	    digit1[3] = rparm[10];
/*<             DIGIT2(4) = RPARM(12)      >*/
	    digit2[3] = rparm[11];

/*        TEST SSORSI */

/*<             CALL DFAULT (IPARM,RPARM)  >*/
	    dfault_(iparm, rparm);
/*<             IPARM(1) = ITMAX >*/
	    iparm[0] = itmax;
/*<             IPARM(2) = LEVEL >*/
	    iparm[1] = level;
/*<             IPARM(5) = ISYM  >*/
	    iparm[4] = tbk12_1.isym;
/*<             IPARM(9) = NB    >*/
	    iparm[8] = nb;
/*<             IPARM(12) = IERAN  >*/
	    iparm[11] = ieran;
/*<             RPARM(1) = ZETA  >*/
	    rparm[0] = zeta;
/*<             CALL VFILL (N,U,0.D0)      >*/
	    vfill_(&tbk12_1.n, u, &c_b19);
/*<             CALL SSORSI (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER)  >*/
	    ssorsi_(&tbk12_1.n, ia, ja, a, rhs, u, iwksp, &nw, wksp, iparm, 
		    rparm, &ier);
/*<             TIM1(5) = RPARM(9) >*/
	    tim1[4] = rparm[8];
/*<             TIM2(5) = RPARM(10)        >*/
	    tim2[4] = rparm[9];
/*<             DIGIT1(5) = RPARM(11)      >*/
	    digit1[4] = rparm[10];
/*<             DIGIT2(5) = RPARM(12)      >*/
	    digit2[4] = rparm[11];
/*<             ITER(5) = IPARM(1) >*/
	    iter[4] = iparm[0];
/*<             IWRK(5) = IPARM(8) >*/
	    iwrk[4] = iparm[7];

/*        TEST RSCG */

/*<             CALL DFAULT (IPARM,RPARM)  >*/
	    dfault_(iparm, rparm);
/*<             IPARM(1) = ITMAX >*/
	    iparm[0] = itmax;
/*<             IPARM(2) = LEVEL >*/
	    iparm[1] = level;
/*<             IPARM(5) = ISYM  >*/
	    iparm[4] = tbk12_1.isym;
/*<             IPARM(12) = IERAN  >*/
	    iparm[11] = ieran;
/*<             RPARM(1) = ZETA  >*/
	    rparm[0] = zeta;
/*<             CALL VFILL (N,U,0.D0)      >*/
	    vfill_(&tbk12_1.n, u, &c_b19);
/*<             CALL RSCG (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER)  >*/
	    rscg_(&tbk12_1.n, ia, ja, a, rhs, u, iwksp, &nw, wksp, iparm, 
		    rparm, &ier);
/*<             TIM1(6) = RPARM(9) >*/
	    tim1[5] = rparm[8];
/*<             TIM2(6) = RPARM(10)        >*/
	    tim2[5] = rparm[9];
/*<             DIGIT1(6) = RPARM(11)      >*/
	    digit1[5] = rparm[10];
/*<             DIGIT2(6) = RPARM(12)      >*/
	    digit2[5] = rparm[11];
/*<             ITER(6) = IPARM(1) >*/
	    iter[5] = iparm[0];
/*<             IWRK(6) = IPARM(8) >*/
	    iwrk[5] = iparm[7];

/*        TEST RSSI */

/*<             CALL DFAULT (IPARM,RPARM)  >*/
	    dfault_(iparm, rparm);
/*<             IPARM(1) = ITMAX >*/
	    iparm[0] = itmax;
/*<             IPARM(2) = LEVEL >*/
	    iparm[1] = level;
/*<             IPARM(5) = ISYM  >*/
	    iparm[4] = tbk12_1.isym;
/*<             IPARM(12) = IERAN  >*/
	    iparm[11] = ieran;
/*<             RPARM(1) = ZETA  >*/
	    rparm[0] = zeta;
/*<             CALL VFILL (N,U,0.D0)      >*/
	    vfill_(&tbk12_1.n, u, &c_b19);
/*<             CALL RSSI (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER)  >*/
	    rssi_(&tbk12_1.n, ia, ja, a, rhs, u, iwksp, &nw, wksp, iparm, 
		    rparm, &ier);
/*<             TIM1(7) = RPARM(9) >*/
	    tim1[6] = rparm[8];
/*<             TIM2(7) = RPARM(10)        >*/
	    tim2[6] = rparm[9];
/*<             DIGIT1(7) = RPARM(11)      >*/
	    digit1[6] = rparm[10];
/*<             DIGIT2(7) = RPARM(12)      >*/
	    digit2[6] = rparm[11];
/*<             ITER(7) = IPARM(1) >*/
	    iter[6] = iparm[0];
/*<             IWRK(7) = IPARM(8) >*/
	    iwrk[6] = iparm[7];

/*     TIMING ANALYSIS */

/*<    >*/
	    time1_(&tbk12_1.n, ia, ja, a, wksp, &wksp[tbk12_1.n], iter, tim1, 
		    tim2, digit1, digit2, iwrk, &tbk12_1.nout);
/*<    60 CONTINUE     >*/
/* L60: */
	}
    }

/*<    70 FORMAT (//15X,'SYMMETRIC SPARSE STORAGE USED')       >*/
/*<    80 FORMAT (//15X,'NONSYMMETRIC SPARSE STORAGE USED')    >*/
/*<    90 FORMAT (15X,'NATURAL ORDERING USED')       >*/
/*<   100 FORMAT (15X,'RED-BLACK ORDERING USED')     >*/
/*<       RETURN       >*/
    return 0;
/*<       END  >*/
} /* test1d_ */

/*<    >*/
/* Subroutine */ int time1_(integer *n, integer *ia, integer *ja, doublereal *
	a, doublereal *v, doublereal *w, integer *iter, doublereal *tim1, 
	doublereal *tim2, doublereal *digit1, doublereal *digit2, integer *
	iwork, integer *nout)
{
    /* Initialized data */

    static integer maxlp = 20;

    /* Format strings */
    static char fmt_30[] = "(\0020\002,3(/),15x,\002TIMING ANALYSIS\002/15x,\
15(\002-\002)//35x,\002ITERATION\002,3x,\002TOTAL\002,9x,\002NO. OF\002,4x\
,\002AVG ITER/\002,2x,\002DIGITS\002,4x,\002DIGITS\002,5x,\002WKSP\002/15x\
,\002METHOD\002,14x,\002TIME (SEC)\002,2x,\002TIME (SEC)\002,1x,\002ITERATIO\
NS\002,1x,\002MTX-VCTR MLT\002,1x,\002STP TST\002,3x,\002RES/RHS\002,4x,\002\
USED\002/15x,103(\002-\002))";
    static char fmt_40[] = "(15x,\002JACOBI CG        \002,2f10.3,i14,f10.1,\
1x,2f10.1,i10/15x,\002JACOBI SI        \002,2f10.3,i14,f10.1,1x,2f10.1,i10/1\
5x,\002SOR              \002,2f10.3,i14,f10.1,1x,2f10.1,i10/15x,\002SYMMETRI\
C SOR CG \002,2f10.3,i14,f10.1,1x,2f10.1,i10/15x,\002SYMMETRIC SOR SI \002,2\
f10.3,i14,f10.1,1x,2f10.1,i10/15x,\002REDUCED SYSTEM CG\002,2f10.3,i14,f10.1\
,1x,2f10.1,i10/15x,\002REDUCED SYSTEM SI\002,2f10.3,i14,f10.1,1x,2f10.1,i10/)"
	    ;

    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), e_wsfe(), do_fio(integer *, char *, ftnlen);

    /* Local variables */
    integer i__;
    doublereal rel[7], temp;
    real timi1, timi2;
    extern /* Subroutine */ int vfill_(integer *, doublereal *, doublereal *);
    extern doublereal timer_(real *);
    extern /* Subroutine */ int pmult_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *);
    doublereal timmat;

    /* Fortran I/O blocks */
    static cilist io___48 = { 0, 0, 0, fmt_30, 0 };
    static cilist io___51 = { 0, 0, 0, fmt_40, 0 };


/*<       INTEGER ITER(7),IA(1),JA(1),IWORK(7)       >*/
/*<    >*/
/*<       DATA MAXLP / 20 /      >*/
    /* Parameter adjustments */
    --w;
    --v;
    --ia;
    --ja;
    --a;
    --iter;
    --tim1;
    --tim2;
    --digit1;
    --digit2;
    --iwork;

    /* Function Body */

/*<       CALL VFILL (N,V,1.D0)  >*/
    vfill_(n, &v[1], &c_b27);
/*<       TIMI1 = TIMER(0.0)     >*/
    timi1 = timer_(&c_b28);
/*<       DO 10 I = 1,MAXLP      >*/
    i__1 = maxlp;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<          CALL PMULT (N,IA,JA,A,V,W)    >*/
	pmult_(n, &ia[1], &ja[1], &a[1], &v[1], &w[1]);
/*<    10 CONTINUE     >*/
/* L10: */
    }
/*<       TIMI2 = TIMER(0.0)     >*/
    timi2 = timer_(&c_b28);
/*<       TIMMAT = DBLE(TIMI2-TIMI1)/DBLE(FLOAT(MAXLP))        >*/
    timmat = (doublereal) (timi2 - timi1) / (doublereal) ((real) maxlp);

/*<       WRITE (NOUT,30)        >*/
    io___48.ciunit = *nout;
    s_wsfe(&io___48);
    e_wsfe();
/*<       DO 20 I = 1,7  >*/
    for (i__ = 1; i__ <= 7; ++i__) {
/*<          REL(I) = 0.D0       >*/
	rel[i__ - 1] = 0.;
/*<          TEMP = DBLE(FLOAT(ITER(I)))   >*/
	temp = (doublereal) ((real) iter[i__]);
/*<          IF ((TIMI2.EQ.TIMI1).OR.(ITER(I).EQ.0)) GO TO 20  >*/
	if (timi2 == timi1 || iter[i__] == 0) {
	    goto L20;
	}
/*<          REL(I) = TIM1(I)/(TEMP*TIMMAT)  >*/
	rel[i__ - 1] = tim1[i__] / (temp * timmat);
/*<    20 CONTINUE     >*/
L20:
	;
    }

/*<    >*/
    io___51.ciunit = *nout;
    s_wsfe(&io___51);
    for (i__ = 1; i__ <= 7; ++i__) {
	do_fio(&c__1, (char *)&tim1[i__], (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&tim2[i__], (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&iter[i__], (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&rel[i__ - 1], (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&digit1[i__], (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&digit2[i__], (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&iwork[i__], (ftnlen)sizeof(integer));
    }
    e_wsfe();

/*<       RETURN       >*/
    return 0;

/*<    >*/
/*<    >*/

/*<       END  >*/
} /* time1_ */

/*<       SUBROUTINE PDE1 (X,Y,CVALUS)     >*/
/* Subroutine */ int pde1_(doublereal *x, doublereal *y, doublereal *cvalus)
{

/* ... THIS SUBROUTINE IS A USER SUPPLIED SUBROUTINE TO SPECIFY THE */
/* ... SELF-ADJOINT ELLIPTIC PDE FOR SYM5PT IN THE FOLLOWING FORM */

/*        (CVALUS(1)*UX)X + (CVALUS(3)*UY)Y + CVALUS(6)*U = CVALUS(7) */

/*     NOTE:  CVALUS(I), FOR I = 2, 4, AND 5 ARE NOT USED. */

/*<       DOUBLE PRECISION CVALUS(7),X,Y   >*/

/*<       CVALUS(1) = 1.D0       >*/
    /* Parameter adjustments */
    --cvalus;

    /* Function Body */
    cvalus[1] = 1.;
/*<       CVALUS(2) = 0.D0       >*/
    cvalus[2] = 0.;
/*<       CVALUS(3) = 2.D0       >*/
    cvalus[3] = 2.;
/*<       CVALUS(4) = 0.D0       >*/
    cvalus[4] = 0.;
/*<       CVALUS(5) = 0.D0       >*/
    cvalus[5] = 0.;
/*<       CVALUS(6) = 0.D0       >*/
    cvalus[6] = 0.;
/*<       CVALUS(7) = 0.D0       >*/
    cvalus[7] = 0.;

/*<       RETURN       >*/
    return 0;
/*<       END  >*/
} /* pde1_ */

/*<       DOUBLE PRECISION FUNCTION BCOND1 (ISIDE,X,Y,BVALUS)  >*/
doublereal bcond1_(integer *iside, doublereal *x, doublereal *y, doublereal *
	bvalus)
{
    /* System generated locals */
    doublereal ret_val;


/* ... THIS DOUBLE PRECISION FUNCTION IS A USER SUPPLIED FUNCTION TO SPEC */
/* ... BOUNDARY CONDITIONS OF THE ELLIPTIC PDE DEPENDING ON ISIDE, X, */
/* ... AND Y. */

/*        IF ISIDE = 1, THEN X = BX (EAST SIDE) */
/*                 = 2, THEN Y = AY (SOUTH SIDE) */
/*                 = 3, THEN X = AX (WEST SIDE) */
/*                 = 4, THEN Y = BY (NORTH SIDE) */

/* ... THE BVALUS ARRAY IS DEFINED AS FOLLOWS */

/*        BVALUS(1)*U + BVALUS(2)*UX + BVALUS(3)*UY = BVALUS(4) */

/*        NOTE:  BCOND1 IS SET TO BVALUS(4) BEFORE RETURNING. */

/*<       DOUBLE PRECISION BVALUS(4),X,Y   >*/

/*<       GO TO (10,20,30,40), ISIDE       >*/
    /* Parameter adjustments */
    --bvalus;

    /* Function Body */
    switch (*iside) {
	case 1:  goto L10;
	case 2:  goto L20;
	case 3:  goto L30;
	case 4:  goto L40;
    }

/*<    10 BVALUS(1) = 1.D0       >*/
L10:
    bvalus[1] = 1.;
/*<       BVALUS(4) = 1.D0+X*Y   >*/
    bvalus[4] = *x * *y + 1.;
/*<       BVALUS(2) = 0.D0       >*/
    bvalus[2] = 0.;
/*<       BVALUS(3) = 0.D0       >*/
    bvalus[3] = 0.;
/*<       GO TO 50     >*/
    goto L50;

/*<    20 BVALUS(1) = 1.D0       >*/
L20:
    bvalus[1] = 1.;
/*<       BVALUS(4) = 1.D0+X*Y   >*/
    bvalus[4] = *x * *y + 1.;
/*<       BVALUS(2) = 0.D0       >*/
    bvalus[2] = 0.;
/*<       BVALUS(3) = 0.D0       >*/
    bvalus[3] = 0.;
/*<       GO TO 50     >*/
    goto L50;

/*<    30 BVALUS(1) = 1.D0       >*/
L30:
    bvalus[1] = 1.;
/*<       BVALUS(4) = 1.D0+X*Y   >*/
    bvalus[4] = *x * *y + 1.;
/*<       BVALUS(2) = 0.D0       >*/
    bvalus[2] = 0.;
/*<       BVALUS(3) = 0.D0       >*/
    bvalus[3] = 0.;
/*<       GO TO 50     >*/
    goto L50;

/*<    40 BVALUS(1) = 1.D0       >*/
L40:
    bvalus[1] = 1.;
/*<       BVALUS(4) = 1.D0+X*Y   >*/
    bvalus[4] = *x * *y + 1.;
/*<       BVALUS(2) = 0.D0       >*/
    bvalus[2] = 0.;
/*<       BVALUS(3) = 0.D0       >*/
    bvalus[3] = 0.;

/*<    50 BCOND1 = BVALUS(4)     >*/
L50:
    ret_val = bvalus[4];
/*<       RETURN       >*/
    return ret_val;
/*<       END  >*/
} /* bcond1_ */

/*<    >*/
/* Subroutine */ int sym5pt_(doublereal *gridx, integer *ngrdxd, doublereal *
	gridy, integer *ngrdyd, doublereal *rhs, integer *mxneq, integer *ia, 
	integer *ja, doublereal *a, integer *nels, integer *iwksp)
{
    /* Format strings */
    static char fmt_10[] = "(/10x,\002FINITE DIFFERENCE MODULE\002,\002 --\
-- \002,\002SYMMETRIC FIVE POINT\002/10x,\002DOMAIN = RECTANGLE  (\002,d11.4,\
\002,\002,d11.4,\002) X (\002,d11.4,\002,\002,d11.4,\002)\002)";
    static char fmt_20[] = "(/10x,\002COEFFICIENTS OF VERTICAL MESH LINES\
\002)";
    static char fmt_30[] = "(/8x,8(2x,d11.4))";
    static char fmt_40[] = "(/10x,\002COEFFICIENTS OF HORIZONTAL MESH LINE\
S\002)";
    static char fmt_50[] = "(/10x,\002BOUNDARY CONDITIONS ON PIECES 1,2,3,4 \
ARE \002,3(1x,i1,\002,\002),1x,i1,\002.\002)";
    static char fmt_80[] = "(/10x,\002SYM5PT COMPLETED SUCCESSFULLY.\002)";
    static char fmt_90[] = "(10x,\002SPARSE MATRIX REPRESENTATION FINISHED\
.\002/15x,\002NO. OF EQUATIONS        =\002,i8/15x,\002NO. OF NON-ZEROES    \
   =\002,i8/15x,\002TOTAL MATRIX STORAGE    =\002,i8/)";
    static char fmt_110[] = "(/10x,\002SYM5PT ERROR -- NGRIDX .LT. 3 \002)";
    static char fmt_120[] = "(/10x,\002SYM5PT ERROR -- NGRIDY .LT. 3 \002)";
    static char fmt_140[] = "(/10x,\002N .GT. MXNEQ, N =\002,i10,\002 MXNE\
Q =\002,i10)";

    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();
    /* Subroutine */ int s_stop(char *, ftnlen);

    /* Local variables */
    doublereal b;
    integer i__;
    doublereal he;
    integer ij;
    doublereal hn, hs, hw;
    integer nu;
    doublereal px, py;
    integer ix1, ix2, jy1, jy2, nuu, ixx, jyy, inij;
    extern /* Subroutine */ int pnt2d_(doublereal *, doublereal *, integer *, 
	    integer *, integer *, integer *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *, 
	    integer *, doublereal *, integer *);
    integer ixadd, jyadd;
    extern /* Subroutine */ int sbend_(integer *, integer *, integer *, 
	    integer *, doublereal *, integer *), sbini_(integer *, integer *, 
	    integer *, integer *, doublereal *, integer *);
    integer lngthx;

    /* Fortran I/O blocks */
    static cilist io___57 = { 0, 0, 0, fmt_10, 0 };
    static cilist io___58 = { 0, 0, 0, fmt_20, 0 };
    static cilist io___59 = { 0, 0, 0, fmt_30, 0 };
    static cilist io___61 = { 0, 0, 0, fmt_40, 0 };
    static cilist io___62 = { 0, 0, 0, fmt_30, 0 };
    static cilist io___63 = { 0, 0, 0, fmt_50, 0 };
    static cilist io___77 = { 0, 0, 0, fmt_80, 0 };
    static cilist io___80 = { 0, 0, 0, fmt_90, 0 };
    static cilist io___81 = { 0, 0, 0, fmt_110, 0 };
    static cilist io___82 = { 0, 0, 0, fmt_120, 0 };
    static cilist io___83 = { 0, 0, 0, fmt_140, 0 };



/*<       INTEGER IA(1),JA(NELS),BCTYPE(4),IWKSP(NELS) >*/
/*<    >*/

/* ... SYM5PT / SYM7PT COMMON BLOCKS */

/*<       COMMON /TBK11/ AX,AY,BX,BY,DRELPR,HX,HY    >*/
/*<       COMMON /TBK12/ BCTYPE,ILEVEL,ISYM,NOUT,NGRIDX,NGRIDY,N >*/

/*<       IF (NGRIDX.LT.3.OR.NGRIDY.LT.3) GO TO 100  >*/
    /* Parameter adjustments */
    --gridx;
    --gridy;
    --rhs;
    --ia;
    --iwksp;
    --a;
    --ja;

    /* Function Body */
    if (tbk12_1.ngridx < 3 || tbk12_1.ngridy < 3) {
	goto L100;
    }

/*     DETERMINE RANGE OF UNKNOWN GRID POINTS */

/*<       IX1 = 1      >*/
    ix1 = 1;
/*<       IX2 = NGRIDX >*/
    ix2 = tbk12_1.ngridx;
/*<       JY1 = 1      >*/
    jy1 = 1;
/*<       JY2 = NGRIDY >*/
    jy2 = tbk12_1.ngridy;
/*<       IF (BCTYPE(1).EQ.1) IX2 = NGRIDX-1 >*/
    if (tbk12_1.bctype[0] == 1) {
	ix2 = tbk12_1.ngridx - 1;
    }
/*<       IF (BCTYPE(2).EQ.1) JY1 = 2      >*/
    if (tbk12_1.bctype[1] == 1) {
	jy1 = 2;
    }
/*<       IF (BCTYPE(3).EQ.1) IX1 = 2      >*/
    if (tbk12_1.bctype[2] == 1) {
	ix1 = 2;
    }
/*<       IF (BCTYPE(4).EQ.1) JY2 = NGRIDY-1 >*/
    if (tbk12_1.bctype[3] == 1) {
	jy2 = tbk12_1.ngridy - 1;
    }
/*<       LNGTHX = IX2-IX1+1     >*/
    lngthx = ix2 - ix1 + 1;
/*<       N = (JY2-JY1+1)*LNGTHX >*/
    tbk12_1.n = (jy2 - jy1 + 1) * lngthx;
/*<       IF (N.GT.MXNEQ) GO TO 130        >*/
    if (tbk12_1.n > *mxneq) {
	goto L130;
    }

/*     OUTPUT INITIAL GRID INFORMATION */

/*<       IF (ILEVEL.EQ.0) GO TO 60        >*/
    if (tbk12_1.ilevel == 0) {
	goto L60;
    }
/*<       WRITE (NOUT,10) AX,BX,AY,BY      >*/
    io___57.ciunit = tbk12_1.nout;
    s_wsfe(&io___57);
    do_fio(&c__1, (char *)&tbk11_1.ax, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&tbk11_1.bx, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&tbk11_1.ay, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&tbk11_1.by, (ftnlen)sizeof(doublereal));
    e_wsfe();
/*<    >*/

/*<       WRITE (NOUT,20)        >*/
    io___58.ciunit = tbk12_1.nout;
    s_wsfe(&io___58);
    e_wsfe();
/*<    20 FORMAT (/10X,'COEFFICIENTS OF VERTICAL MESH LINES')  >*/
/*<       WRITE (NOUT,30) (GRIDX(I),I=1,NGRIDX)      >*/
    io___59.ciunit = tbk12_1.nout;
    s_wsfe(&io___59);
    i__1 = tbk12_1.ngridx;
    for (i__ = 1; i__ <= i__1; ++i__) {
	do_fio(&c__1, (char *)&gridx[i__], (ftnlen)sizeof(doublereal));
    }
    e_wsfe();
/*<    30 FORMAT (/8X,8(2X,D11.4)) >*/
/*<       WRITE (NOUT,40)        >*/
    io___61.ciunit = tbk12_1.nout;
    s_wsfe(&io___61);
    e_wsfe();
/*<    40 FORMAT (/10X,'COEFFICIENTS OF HORIZONTAL MESH LINES')  >*/
/*<       WRITE (NOUT,30) (GRIDY(I),I=1,NGRIDY)      >*/
    io___62.ciunit = tbk12_1.nout;
    s_wsfe(&io___62);
    i__1 = tbk12_1.ngridy;
    for (i__ = 1; i__ <= i__1; ++i__) {
	do_fio(&c__1, (char *)&gridy[i__], (ftnlen)sizeof(doublereal));
    }
    e_wsfe();
/*<       WRITE (NOUT,50) (BCTYPE(I),I=1,4)  >*/
    io___63.ciunit = tbk12_1.nout;
    s_wsfe(&io___63);
    for (i__ = 1; i__ <= 4; ++i__) {
	do_fio(&c__1, (char *)&tbk12_1.bctype[i__ - 1], (ftnlen)sizeof(
		integer));
    }
    e_wsfe();
/*<    >*/

/*     GENERATE EQUATIONS ONE MESH POINT AT A TIME */

/*<    60 CALL SBINI (N,NELS,IA,JA,A,IWKSP)  >*/
L60:
    sbini_(&tbk12_1.n, nels, &ia[1], &ja[1], &a[1], &iwksp[1]);
/*<       IXADD = 0    >*/
    ixadd = 0;
/*<       JYADD = 0    >*/
    jyadd = 0;
/*<       IF (BCTYPE(3).EQ.1) IXADD = 1    >*/
    if (tbk12_1.bctype[2] == 1) {
	ixadd = 1;
    }
/*<       IF (BCTYPE(2).EQ.1) JYADD = 1    >*/
    if (tbk12_1.bctype[1] == 1) {
	jyadd = 1;
    }

/*<       DO 70 IJ = 1,N >*/
    i__1 = tbk12_1.n;
    for (ij = 1; ij <= i__1; ++ij) {
/*<          IXX = MOD(IJ-1,LNGTHX)+1      >*/
	ixx = (ij - 1) % lngthx + 1;
/*<          JYY = (IJ-IXX)/LNGTHX+1       >*/
	jyy = (ij - ixx) / lngthx + 1;
/*<          IXX = IXX+IXADD     >*/
	ixx += ixadd;
/*<          JYY = JYY+JYADD     >*/
	jyy += jyadd;
/*<          HN = 0.D0 >*/
	hn = 0.;
/*<          HS = 0.D0 >*/
	hs = 0.;
/*<          HE = 0.D0 >*/
	he = 0.;
/*<          HW = 0.D0 >*/
	hw = 0.;
/*<          PX = GRIDX(IXX)     >*/
	px = gridx[ixx];
/*<          PY = GRIDY(JYY)     >*/
	py = gridy[jyy];
/*<          IF (IXX.NE.1) HW = PX-GRIDX(IXX-1)      >*/
	if (ixx != 1) {
	    hw = px - gridx[ixx - 1];
	}
/*<          IF (IXX.NE.NGRIDX) HE = GRIDX(IXX+1)-PX >*/
	if (ixx != tbk12_1.ngridx) {
	    he = gridx[ixx + 1] - px;
	}
/*<          IF (JYY.NE.1) HS = PY-GRIDY(JYY-1)      >*/
	if (jyy != 1) {
	    hs = py - gridy[jyy - 1];
	}
/*<          IF (JYY.NE.NGRIDY) HN = GRIDY(JYY+1)-PY >*/
	if (jyy != tbk12_1.ngridy) {
	    hn = gridy[jyy + 1] - py;
	}

/*<          INIJ = IJ >*/
	inij = ij;
/*<    >*/
	pnt2d_(&px, &py, &inij, &ixx, &jyy, &lngthx, &hn, &hs, &hw, &he, &b, 
		nels, &ia[1], &ja[1], &a[1], &iwksp[1]);

/*<          RHS(IJ) = B >*/
	rhs[ij] = b;
/*<    70 CONTINUE     >*/
/* L70: */
    }

/*<       CALL SBEND (N,NELS,IA,JA,A,IWKSP)  >*/
    sbend_(&tbk12_1.n, nels, &ia[1], &ja[1], &a[1], &iwksp[1]);

/*     NORMAL EXIT */

/*<       IF (ILEVEL.EQ.0) RETURN  >*/
    if (tbk12_1.ilevel == 0) {
	return 0;
    }

/*<       WRITE (NOUT,80)        >*/
    io___77.ciunit = tbk12_1.nout;
    s_wsfe(&io___77);
    e_wsfe();
/*<    80 FORMAT (/10X,'SYM5PT COMPLETED SUCCESSFULLY.')       >*/
/*<       NU = IA(N+1)-1 >*/
    nu = ia[tbk12_1.n + 1] - 1;
/*<       NUU = N+1+2*NU >*/
    nuu = tbk12_1.n + 1 + (nu << 1);
/*<       WRITE (NOUT,90) N,NU,NUU >*/
    io___80.ciunit = tbk12_1.nout;
    s_wsfe(&io___80);
    do_fio(&c__1, (char *)&tbk12_1.n, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&nu, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&nuu, (ftnlen)sizeof(integer));
    e_wsfe();
/*<    >*/

/*<       RETURN       >*/
    return 0;

/*     ERROR EXITS */

/*<   100 CONTINUE     >*/
L100:
/*<       IF (NGRIDX.LT.3) WRITE (NOUT,110)  >*/
    if (tbk12_1.ngridx < 3) {
	io___81.ciunit = tbk12_1.nout;
	s_wsfe(&io___81);
	e_wsfe();
    }
/*<   110 FORMAT (/10X,'SYM5PT ERROR -- NGRIDX .LT. 3 ')       >*/
/*<       IF (NGRIDY.LT.3) WRITE (NOUT,120)  >*/
    if (tbk12_1.ngridy < 3) {
	io___82.ciunit = tbk12_1.nout;
	s_wsfe(&io___82);
	e_wsfe();
    }
/*<   120 FORMAT (/10X,'SYM5PT ERROR -- NGRIDY .LT. 3 ')       >*/

/*<       STOP >*/
    s_stop("", (ftnlen)0);

/*<   130 WRITE (NOUT,140) N,MXNEQ >*/
L130:
    io___83.ciunit = tbk12_1.nout;
    s_wsfe(&io___83);
    do_fio(&c__1, (char *)&tbk12_1.n, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&(*mxneq), (ftnlen)sizeof(integer));
    e_wsfe();
/*<   140 FORMAT (/10X,'N .GT. MXNEQ, N =',I10,' MXNEQ =',I10) >*/
/*<       STOP >*/
    s_stop("", (ftnlen)0);

/*<       END  >*/
    return 0;
} /* sym5pt_ */

/*<    >*/
/* Subroutine */ int pnt2d_(doublereal *pcx, doublereal *pcy, integer *ij, 
	integer *ix, integer *jy, integer *lngthx, doublereal *hn, doublereal 
	*hs, doublereal *hw, doublereal *he, doublereal *b, integer *nels, 
	integer *ia, integer *ja, doublereal *a, integer *iwksp)
{
    /* Format strings */
    static char fmt_130[] = "(/10x,\002SYM5PT -- EQUATION\002,i8/20x,\002RIG\
HT HAND SIDE =       \002,d15.7)";

    /* System generated locals */
    integer i__1;
    doublereal d__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    extern /* Subroutine */ int pde1_(doublereal *, doublereal *, doublereal *
	    );
    doublereal crhs, temp, ceast, cwest;
    extern doublereal bcond1_(integer *, doublereal *, doublereal *, 
	    doublereal *);
    extern /* Subroutine */ int stval1_(integer *, integer *, integer *, 
	    doublereal *, integer *, integer *, doublereal *, integer *);
    doublereal center, bvalus[4], cvalus[7], cnorth, csouth;

    /* Fortran I/O blocks */
    static cilist io___93 = { 0, 0, 0, fmt_130, 0 };



/*<    >*/
/*<       INTEGER IA(1),JA(NELS),BCTYPE(4),IWKSP(NELS) >*/

/* ... SYM5PT / SYM7PT COMMON BLOCKS */

/*<       COMMON /TBK11/ AX,AY,BX,BY,DRELPR,HX,HY    >*/
/*<       COMMON /TBK12/ BCTYPE,ILEVEL,ISYM,NOUT,NGRIDX,NGRIDY,N >*/

/* ... INITIALIZE COEFFICIENTS */

/*<       CEAST = 0.D0 >*/
    /* Parameter adjustments */
    --iwksp;
    --a;
    --ja;
    --ia;

    /* Function Body */
    ceast = 0.;
/*<       CWEST = 0.D0 >*/
    cwest = 0.;
/*<       CNORTH = 0.D0  >*/
    cnorth = 0.;
/*<       CSOUTH = 0.D0  >*/
    csouth = 0.;
/*<       CALL PDE1 (PCX,PCY,CVALUS)       >*/
    pde1_(pcx, pcy, cvalus);
/*<       CENTER = -CVALUS(6)*(HN+HS)*(HE+HW)/4.D0   >*/
    center = -cvalus[5] * (*hn + *hs) * (*he + *hw) / 4.;
/*<       CRHS = -CVALUS(7)*(HN+HS)*(HE+HW)/4.D0     >*/
    crhs = -cvalus[6] * (*hn + *hs) * (*he + *hw) / 4.;

/*     SET EAST COEFFICIENT */

/*<       CALL PDE1 (PCX+.5D0*HE,PCY,CVALUS) >*/
    d__1 = *pcx + *he * .5;
    pde1_(&d__1, pcy, cvalus);
/*<       IF (IX.GE.NGRIDX-1) TEMP = BCOND1(1,BX,PCY,BVALUS)   >*/
    if (*ix >= tbk12_1.ngridx - 1) {
	temp = bcond1_(&c__1, &tbk11_1.bx, pcy, bvalus);
    }
/*<       IF (IX.EQ.NGRIDX) GO TO 20       >*/
    if (*ix == tbk12_1.ngridx) {
	goto L20;
    }
/*<       IF ((IX+1.EQ.NGRIDX).AND.(BCTYPE(1).EQ.1)) GO TO 10  >*/
    if (*ix + 1 == tbk12_1.ngridx && tbk12_1.bctype[0] == 1) {
	goto L10;
    }

/*     NORMAL EAST POINT */

/*<       CEAST = -CVALUS(1)*(HN+HS)/(2.D0*HE)       >*/
    ceast = -cvalus[0] * (*hn + *hs) / (*he * 2.);
/*<       CENTER = CENTER-CEAST  >*/
    center -= ceast;
/*<       GO TO 30     >*/
    goto L30;

/*     EAST POINT IS A DIRICHLET POINT */

/*<    10 TEMP = CVALUS(1)*(HN+HS)/(2.D0*HE) >*/
L10:
    temp = cvalus[0] * (*hn + *hs) / (*he * 2.);
/*<       CRHS = CRHS+BVALUS(4)*TEMP/BVALUS(1)       >*/
    crhs += bvalus[3] * temp / bvalus[0];
/*<       CENTER = CENTER+TEMP   >*/
    center += temp;
/*<       GO TO 30     >*/
    goto L30;

/*     CENTER POINT LIES ON THE EAST BOUNDARY WHICH IS MIXED */

/*<    20 TEMP = CVALUS(1)*(HN+HS)/(2.D0*BVALUS(2))  >*/
L20:
    temp = cvalus[0] * (*hn + *hs) / (bvalus[1] * 2.);
/*<       CENTER = CENTER+BVALUS(1)*TEMP   >*/
    center += bvalus[0] * temp;
/*<       CRHS = CRHS+BVALUS(4)*TEMP       >*/
    crhs += bvalus[3] * temp;

/*     SET WEST COEFFICIENT */

/*<    30 CALL PDE1 (PCX-.5D0*HW,PCY,CVALUS) >*/
L30:
    d__1 = *pcx - *hw * .5;
    pde1_(&d__1, pcy, cvalus);
/*<       IF (IX.LE.2) TEMP = BCOND1(3,AX,PCY,BVALUS)  >*/
    if (*ix <= 2) {
	temp = bcond1_(&c__3, &tbk11_1.ax, pcy, bvalus);
    }
/*<       IF (IX.EQ.1) GO TO 50  >*/
    if (*ix == 1) {
	goto L50;
    }
/*<       IF (IX.EQ.2.AND.BCTYPE(3).EQ.1) GO TO 40   >*/
    if (*ix == 2 && tbk12_1.bctype[2] == 1) {
	goto L40;
    }

/*     NORMAL WEST POINT */

/*<       CWEST = -CVALUS(1)*(HN+HS)/(2.D0*HW)       >*/
    cwest = -cvalus[0] * (*hn + *hs) / (*hw * 2.);
/*<       CENTER = CENTER-CWEST  >*/
    center -= cwest;
/*<       GO TO 60     >*/
    goto L60;

/*     WEST POINT IS DIRICHLET */

/*<    40 TEMP = CVALUS(1)*(HN+HS)/(2.D0*HW) >*/
L40:
    temp = cvalus[0] * (*hn + *hs) / (*hw * 2.);
/*<       CRHS = CRHS+BVALUS(4)*TEMP/BVALUS(1)       >*/
    crhs += bvalus[3] * temp / bvalus[0];
/*<       CENTER = CENTER+TEMP   >*/
    center += temp;
/*<       GO TO 60     >*/
    goto L60;

/*     CENTER POINT LIES ON WEST BOUNDARY WHICH IS MIXED. */

/*<    50 TEMP = CVALUS(1)*(HN+HS)/(2.D0*BVALUS(2))  >*/
L50:
    temp = cvalus[0] * (*hn + *hs) / (bvalus[1] * 2.);
/*<       CENTER = CENTER+BVALUS(1)*TEMP   >*/
    center += bvalus[0] * temp;
/*<       CRHS = CRHS+BVALUS(4)*TEMP       >*/
    crhs += bvalus[3] * temp;

/*     SET NORTH COEFFICIENTS */

/*<    60 CALL PDE1 (PCX,PCY+.5D0*HN,CVALUS) >*/
L60:
    d__1 = *pcy + *hn * .5;
    pde1_(pcx, &d__1, cvalus);
/*<       IF (JY.GE.NGRIDY-1) TEMP = BCOND1(4,PCX,BY,BVALUS)   >*/
    if (*jy >= tbk12_1.ngridy - 1) {
	temp = bcond1_(&c__4, pcx, &tbk11_1.by, bvalus);
    }
/*<       IF (JY.EQ.NGRIDY) GO TO 80       >*/
    if (*jy == tbk12_1.ngridy) {
	goto L80;
    }
/*<       IF ((JY.EQ.NGRIDY-1).AND.BCTYPE(4).EQ.1) GO TO 70    >*/
    if (*jy == tbk12_1.ngridy - 1 && tbk12_1.bctype[3] == 1) {
	goto L70;
    }

/*     NORMAL NORTH POINT */

/*<       CNORTH = -CVALUS(3)*(HE+HW)/(2.D0*HN)      >*/
    cnorth = -cvalus[2] * (*he + *hw) / (*hn * 2.);
/*<       CENTER = CENTER-CNORTH >*/
    center -= cnorth;
/*<       GO TO 90     >*/
    goto L90;

/*     NORTH POINT IS DIRICHLET */

/*<    70 TEMP = CVALUS(3)*(HE+HW)/(2.D0*HN) >*/
L70:
    temp = cvalus[2] * (*he + *hw) / (*hn * 2.);
/*<       CRHS = CRHS+BVALUS(4)*TEMP/BVALUS(1)       >*/
    crhs += bvalus[3] * temp / bvalus[0];
/*<       CENTER = CENTER+TEMP   >*/
    center += temp;
/*<       GO TO 90     >*/
    goto L90;

/*     CENTER POINT LIES ON NORTHERN MIXED BOUNDARY */

/*<    80 TEMP = CVALUS(3)*(HE+HW)/(2.D0*BVALUS(3))  >*/
L80:
    temp = cvalus[2] * (*he + *hw) / (bvalus[2] * 2.);
/*<       CENTER = CENTER+BVALUS(1)*TEMP   >*/
    center += bvalus[0] * temp;
/*<       CRHS = CRHS+BVALUS(4)*TEMP       >*/
    crhs += bvalus[3] * temp;

/*     SET SOUTH COEFFICIENTS */

/*<    90 CALL PDE1 (PCX,PCY-.5D0*HS,CVALUS) >*/
L90:
    d__1 = *pcy - *hs * .5;
    pde1_(pcx, &d__1, cvalus);
/*<       IF (JY.LE.2) TEMP = BCOND1(2,PCX,AY,BVALUS)  >*/
    if (*jy <= 2) {
	temp = bcond1_(&c__2, pcx, &tbk11_1.ay, bvalus);
    }
/*<       IF (JY.EQ.1) GO TO 110 >*/
    if (*jy == 1) {
	goto L110;
    }
/*<       IF (JY.EQ.2.AND.BCTYPE(2).EQ.1) GO TO 100  >*/
    if (*jy == 2 && tbk12_1.bctype[1] == 1) {
	goto L100;
    }

/*     NORMAL SOUTH POINT */

/*<       CSOUTH = -CVALUS(3)*(HE+HW)/(2.D0*HS)      >*/
    csouth = -cvalus[2] * (*he + *hw) / (*hs * 2.);
/*<       CENTER = CENTER-CSOUTH >*/
    center -= csouth;
/*<       GO TO 120    >*/
    goto L120;

/*     SOUTH POINT IS DIRICHLET */

/*<   100 TEMP = CVALUS(3)*(HE+HW)/(2.D0*HS) >*/
L100:
    temp = cvalus[2] * (*he + *hw) / (*hs * 2.);
/*<       CRHS = CRHS+BVALUS(4)*TEMP/BVALUS(1)       >*/
    crhs += bvalus[3] * temp / bvalus[0];
/*<       CENTER = CENTER+TEMP   >*/
    center += temp;
/*<       GO TO 120    >*/
    goto L120;

/*     CENTER POINT LIES ON SOUTHERN MIXED BOUNDARY */

/*<   110 TEMP = CVALUS(3)*(HE+HW)/(2.D0*BVALUS(3))  >*/
L110:
    temp = cvalus[2] * (*he + *hw) / (bvalus[2] * 2.);
/*<       CENTER = CENTER+BVALUS(1)*TEMP   >*/
    center += bvalus[0] * temp;
/*<       CRHS = CRHS+BVALUS(4)*TEMP       >*/
    crhs += bvalus[3] * temp;

/*     COEFFICIENT GENERATION IS COMPLETED. */
/*     NOW SET B (RHS) AND IA,JA, AND A (MATRIX REPRESENTATION) */

/*<   120 B = CRHS     >*/
L120:
    *b = crhs;

/*<       IF (ILEVEL.EQ.1) WRITE (NOUT,130) IJ,CRHS  >*/
    if (tbk12_1.ilevel == 1) {
	io___93.ciunit = tbk12_1.nout;
	s_wsfe(&io___93);
	do_fio(&c__1, (char *)&(*ij), (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&crhs, (ftnlen)sizeof(doublereal));
	e_wsfe();
    }
/*<    >*/

/*<       CALL STVAL1 (NELS,IA,JA,A,IJ,IJ,CENTER,IWKSP)        >*/
    stval1_(nels, &ia[1], &ja[1], &a[1], ij, ij, &center, &iwksp[1]);
/*<       CALL STVAL1 (NELS,IA,JA,A,IJ,IJ+1,CEAST,IWKSP)       >*/
    i__1 = *ij + 1;
    stval1_(nels, &ia[1], &ja[1], &a[1], ij, &i__1, &ceast, &iwksp[1]);
/*<       CALL STVAL1 (NELS,IA,JA,A,IJ,IJ-LNGTHX,CSOUTH,IWKSP) >*/
    i__1 = *ij - *lngthx;
    stval1_(nels, &ia[1], &ja[1], &a[1], ij, &i__1, &csouth, &iwksp[1]);
/*<       CALL STVAL1 (NELS,IA,JA,A,IJ,IJ-1,CWEST,IWKSP)       >*/
    i__1 = *ij - 1;
    stval1_(nels, &ia[1], &ja[1], &a[1], ij, &i__1, &cwest, &iwksp[1]);
/*<       CALL STVAL1 (NELS,IA,JA,A,IJ,IJ+LNGTHX,CNORTH,IWKSP) >*/
    i__1 = *ij + *lngthx;
    stval1_(nels, &ia[1], &ja[1], &a[1], ij, &i__1, &cnorth, &iwksp[1]);

/*<       RETURN       >*/
    return 0;
/*<       END  >*/
} /* pnt2d_ */

/*<       SUBROUTINE STVAL1 (NELS,IA,JA,A,I,J,VAL,IWKSP)       >*/
/* Subroutine */ int stval1_(integer *nels, integer *ia, integer *ja, 
	doublereal *a, integer *i__, integer *j, doublereal *val, integer *
	iwksp)
{
    /* Format strings */
    static char fmt_20[] = "(20x,\002COLUMN\002,i8,\002   VALUE =\002,d15.7)";

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();
    /* Subroutine */ int s_stop(char *, ftnlen);

    /* Local variables */
    integer ier;
    extern /* Subroutine */ int sbsij_(integer *, integer *, integer *, 
	    integer *, doublereal *, integer *, integer *, integer *, 
	    doublereal *, integer *, integer *, integer *, integer *);

    /* Fortran I/O blocks */
    static cilist io___94 = { 0, 0, 0, fmt_20, 0 };


/*<       DOUBLE PRECISION A(NELS),VAL,AX,AY,BX,BY,DRELPR,HX,HY  >*/
/*<       INTEGER IA(1),JA(NELS),IWKSP(NELS),BCTYPE(4) >*/

/*<       COMMON /TBK11/ AX,AY,BX,BY,DRELPR,HX,HY    >*/
/*<       COMMON /TBK12/ BCTYPE,ILEVEL,ISYM,NOUT,NGRIDX,NGRIDY,N >*/

/*<       IF (J.GT.N.OR.VAL.EQ.0.D0) RETURN  >*/
    /* Parameter adjustments */
    --iwksp;
    --a;
    --ja;
    --ia;

    /* Function Body */
    if (*j > tbk12_1.n || *val == 0.) {
	return 0;
    }
/*<       IF (ISYM.EQ.1) GO TO 10  >*/
    if (tbk12_1.isym == 1) {
	goto L10;
    }
/*<       IF (J.LT.I) RETURN     >*/
    if (*j < *i__) {
	return 0;
    }

/*<    10 IF (ILEVEL.EQ.1) WRITE (NOUT,20) J,VAL     >*/
L10:
    if (tbk12_1.ilevel == 1) {
	io___94.ciunit = tbk12_1.nout;
	s_wsfe(&io___94);
	do_fio(&c__1, (char *)&(*j), (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&(*val), (ftnlen)sizeof(doublereal));
	e_wsfe();
    }
/*<    20 FORMAT (20X,'COLUMN',I8,'   VALUE =',D15.7)  >*/

/*<       IER = 0      >*/
    ier = 0;
/*<       CALL SBSIJ (N,NELS,IA,JA,A,IWKSP,I,J,VAL,0,ILEVEL,NOUT,IER)    >*/
    sbsij_(&tbk12_1.n, nels, &ia[1], &ja[1], &a[1], &iwksp[1], i__, j, val, &
	    c__0, &tbk12_1.ilevel, &tbk12_1.nout, &ier);
/*<       IF (IER.GT.700) STOP   >*/
    if (ier > 700) {
	s_stop("", (ftnlen)0);
    }

/*<       RETURN       >*/
    return 0;
/*<       END  >*/
} /* stval1_ */

/*<       SUBROUTINE TEST2D (EPSI) >*/
/* Subroutine */ int test2d_(doublereal *epsi)
{
    /* Format strings */
    static char fmt_10[] = "(\0021\002//15x,\002ITPACK 2C  TEST PROGRAM -- T\
EST2D\002/15x,\002TESTS ITERATIVE MODULES\002/15x,\002USES SEVEN POINT SYMME\
TRIC DISCRETIZATION\002)";
    static char fmt_60[] = "(\0021\002///)";
    static char fmt_80[] = "(//15x,\002SYMMETRIC SPARSE STORAGE USED\002)";
    static char fmt_90[] = "(//15x,\002NONSYMMETRIC SPARSE STORAGE USED\002)";
    static char fmt_100[] = "(15x,\002NATURAL ORDERING USED\002)";
    static char fmt_110[] = "(15x,\002RED-BLACK ORDERING USED\002)";

    /* System generated locals */
    integer i__1, i__2;

    /* Builtin functions */
    integer s_wsfe(cilist *), e_wsfe();

    /* Local variables */
    doublereal a[1296];
    integer i__, j;
    doublereal u[216];
    integer ia[217], ja[1296], nb, nw;
    extern /* Subroutine */ int jcg_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *);
    integer ier;
    extern /* Subroutine */ int jsi_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *);
    doublereal rhs[216];
    extern /* Subroutine */ int sor_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *);
    doublereal tim1[7], tim2[7];
    extern /* Subroutine */ int rscg_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *);
    doublereal zeta;
    integer iter[7], iwrk[7];
    extern /* Subroutine */ int rssi_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *);
    doublereal wksp[1600];
    extern /* Subroutine */ int time1_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *, doublereal *,
	     doublereal *, doublereal *, doublereal *, integer *, integer *);
    integer loop1, loop2, ieran, level, iparm[12];
    extern /* Subroutine */ int vfill_(integer *, doublereal *, doublereal *);
    doublereal gridx[7], gridy[7], gridz[7], rparm[12];
    integer itmax, itest, jtest, mxneq, iwork[1296], iwksp[648];
    doublereal digit1[7], digit2[7];
    extern /* Subroutine */ int sym7pt_(doublereal *, integer *, doublereal *,
	     integer *, doublereal *, integer *, doublereal *, integer *, 
	    integer *, integer *, doublereal *, integer *, integer *), 
	    dfault_(integer *, doublereal *);
    integer nelmax, ngrdxd, ngrdyd, ngrdzd;
    extern /* Subroutine */ int ssorcg_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *), ssorsi_(
	    integer *, integer *, integer *, doublereal *, doublereal *, 
	    doublereal *, integer *, integer *, doublereal *, integer *, 
	    doublereal *, integer *);

    /* Fortran I/O blocks */
    static cilist io___108 = { 0, 0, 0, fmt_10, 0 };
    static cilist io___115 = { 0, 0, 0, fmt_60, 0 };
    static cilist io___123 = { 0, 0, 0, fmt_80, 0 };
    static cilist io___124 = { 0, 0, 0, fmt_90, 0 };
    static cilist io___125 = { 0, 0, 0, fmt_100, 0 };
    static cilist io___126 = { 0, 0, 0, fmt_110, 0 };



/* ... TEST2D IS A PROGRAM DESIGNED TO TEST ITPACK 2C  METHODS ON */
/* ... MATRICES ARISING FROM THE SYMMETRIC SEVEN POINT DISCRETIZATION */
/* ... OF THREE DIMENSIONAL ELLIPTIC PARTIAL DIFFERENTIAL EQUATIONS ON */
/* ... A 3D RECTANGLE WITH A RECTANGULAR MESH.  ALL SEVEN METHODS FROM */
/* ... ITPACK 2C ARE TESTED AND A SUMMARY IS PRINTED AT THE END. */

/*     THE EXACT SIZE OF THE TEST PROBLEMS CAN BE INCREASED OR */
/*     DECREASED BY CHANGING ARRAY SIZE IN THE DIMENSION STATEMENTS */
/*     AND THE VARIABLES LISTED BELOW UNDER SIZE OF TEST PROBLEM. */
/*     ALSO, THE NUMBER OF TIMES THROUGH THE TEST LOOPS CAN BE REDUCED */
/*     BY CHANGING  ITEST AND JTEST  AS FOLLOWS. */

/*          ITEST = 1 FOR SYMMETRIC STORAGE TEST */
/*                = 2 FOR SYMMETRIC AND NONSYMMETRIC STORAGE TEST */
/*          JTEST = 1 FOR NATURAL ORDERING TEST */
/*                = 2 FOR NATURAL AND RED-BLACK ORDERING TEST */

/* ... SYM7PT COMMON BLOCKS */

/*<       COMMON /TBK21/ AX,AY,AZ,BX,BY,BZ,DRELPR,HX,HY,HZ     >*/
/*<       COMMON /TBK22/ BCTYPE,ILEVEL,ISYM,NOUT,NGRIDX,NGRIDY,NGRIDZ,N  >*/

/*     ARRAY DIMENSIONING */

/*<    >*/
/*<    >*/

/* ... INITIALIZE INTEGER CONSTANTS WHICH CONTROL OUTPUT AND DEFINE */
/* ... ARRAY DIMENSION.  THEY ARE */

/*        DRELPR    -  MACHINE PRECISION */
/*        NOUT    -  FORTRAN OUTPUT UNIT */
/*        LEVEL   -  LEVEL OF OUTPUT FROM ITPACK 2C */
/*        IERAN    -  ERROR ANALYSIS SWITCH */
/*        ITMAX   -  MAXIMUM NUMBER OF ITERATIONS ALLOWED */
/*        ZETA    -  STOPPING CRITERION */
/*        NW      -  SIZE OF THE DOUBLE PRECISION ARRAY WKSP. */

/*        ILEVEL  -  0/1 LEVEL OF OUTPUT FROM SYM7PT */
/*        MXNEQ   -  MAXIMUM NUMBER OF EQUATIONS TO BE GENERATED (I.E. */
/*                   THE MAXIMUM NUMBER OF INTERIOR AND NON-DIRICHLET */
/*                   BOUNDARY POINTS) */
/*        NELMAX  -  MAXIMUM NUMBER OF NON-ZERO ENTRIES IN THE UPPER */
/*                   TRIANGULAR PART OF THE RESULTING SYMMETRIC MATRIX */
/*        NGRIDX  -  NUMBER OF HORIZONTAL MESH PLANES. */
/*        NGRIDY  -  NUMBER OF VERTICAL MESH PLANES. */
/*        NGRIDZ  -  NUMBER OF PERPENDICULAR MESH PLANES. */
/*        NGRDXD  -  MAXIMUM NUMBER OF VERTICAL MESH PLANES INCLUDING THE */
/*                   VERTICAL BOUNDARY PLANES. */
/*        NGRDYD  -  MAXIMUM NUMBER OF HORIZONTAL MESH PLANES INCLUDING */
/*                   THE HORIZONTAL BOUNDARY PLANES. */
/*        NGRDZD  -  MAXIMUM NUMBER OF PERPENDICULAR MESH PLANES INCLUDIN */
/*                   THE PERPENDICULAR BOUNDARY PLANES. */

/*<       DRELPR = EPSI  >*/
    tbk21_1.drelpr = *epsi;

/*     SET ITPACK SWITCHES */

/*<       NOUT = 6     >*/
    tbk22_1.nout = 6;
/*<       LEVEL = 1    >*/
    level = 1;
/*<       IERAN = 0    >*/
    ieran = 0;
/*<       ITMAX = 75   >*/
    itmax = 75;
/*<       ZETA = 0.5D-5  >*/
    zeta = 5e-6;
/*<       NW = 1600    >*/
    nw = 1600;

/*     SET NUMBER OF TIMES THROUGH LOOPS */

/*<       ITEST = 2    >*/
    itest = 2;
/*<       JTEST = 2    >*/
    jtest = 2;

/*     SET SIZE OF TEST PROBLEM */

/*<       ILEVEL = 0   >*/
    tbk22_1.ilevel = 0;
/*<       MXNEQ = 216  >*/
    mxneq = 216;
/*<       NELMAX = 1296  >*/
    nelmax = 1296;
/*<       NGRIDX = 7   >*/
    tbk22_1.ngridx = 7;
/*<       NGRIDY = 7   >*/
    tbk22_1.ngridy = 7;
/*<       NGRIDZ = 7   >*/
    tbk22_1.ngridz = 7;

/*<       NGRDXD = NGRIDX        >*/
    ngrdxd = tbk22_1.ngridx;
/*<       NGRDYD = NGRIDY        >*/
    ngrdyd = tbk22_1.ngridy;
/*<       NGRDZD = NGRIDZ        >*/
    ngrdzd = tbk22_1.ngridz;
/*<       WRITE (NOUT,10)        >*/
    io___108.ciunit = tbk22_1.nout;
    s_wsfe(&io___108);
    e_wsfe();
/*<    >*/

/* ... SET UP DEFAULT VALUES FOR BCTYPE AND INITIALIZE INFORMATION ARRAYS */

/* ... BCTYPE DEFINES THE BOUNDARY CONDITIONS ON THE EDGES OF THE */
/* ... RECTANGLE.  WHERE */
/*        BCTYPE(I) = 0 IMPLIES THE ITH SIDE IS NEUMANN OR MIXED */
/*                  = 1 IMPLIES THE ITH SIDE IS DIRICHLET */

/*          AND I = 1 IMPLIES THE EAST   SIDE DEFINED BY (BX, Y, Z) */
/*                = 2 IMPLIES THE SOUTH  SIDE DEFINED BY ( X,AY, Z) */
/*                = 3 IMPLIES THE WEST   SIDE DEFINED BY (AX, Y, Z) */
/*                = 4 IMPLIES THE NORTH  SIDE DEFINED BY ( X,BY, Z) */
/*                = 5 IMPLIES THE TOP    SIDE DEFINED BY ( X, Y,BZ) */
/*                = 6 IMPLIES THE BOTTOM SIDE DEFINED BY ( X, Y,AZ) */

/*<       DO 20 I = 1,6  >*/
    for (i__ = 1; i__ <= 6; ++i__) {
/*<          BCTYPE(I) = 1       >*/
	tbk22_1.bctype[i__ - 1] = 1;
/*<    20 CONTINUE     >*/
/* L20: */
    }

/* ... DEFINE THE DISCRETIZATION MESH */

/*       AX      -  MINIMUM X VALUE ON THE RECTANGLE (WEST SIDE) */
/*       BX      -  MAXIMUM X VALUE ON THE RECTANGLE (EAST SIDE) */
/*       GRIDX   -  DOUBLE PRECISION ARRAY CONTAINING THE X-COORDINATE OF */
/*                  HORIZONTAL MESH PLANES FROM WEST TO EAST. */
/*                  THESE ARE UNIFORM BUT THAT IS NOT REQUIRED. */
/*       AY      -  MINIMUM Y VALUE ON THE RECTANGLE (SOUTH SIDE) */
/*       BY      -  MAXIMUM Y VALUE ON THE RECTANGLE (NORTH SIDE) */
/*       GRIDY   -  DOUBLE PRECISION ARRAY CONTAINING THE Y-COORDINATE OF */
/*                  VERTICAL MESH PLANES FROM SOUTH TO NORTH. */
/*                  THESE ARE  UNIFORM BUT THAT IS NOT REQUIRED. */
/*       AZ      -  MINIMUM Z VALUE ON THE RECTANGLE (BOTTOM SIDE) */
/*       BZ      -  MAXIMUM Z VALUE ON THE RECTANGLE (TOP SIDE) */
/*       GRIDZ   -  DOUBLE PRECISION ARRAY CONTAINING THE Z-COORDINATE OF */
/*                  PERPENDICULAR MESH PLANES FROM BOTTOM TO TOP. */
/*                  THESE ARE UNIFORM BUT THAT IS NOT REQUIRED. */

/*<       AX = 0.D0    >*/
    tbk21_1.ax = 0.;
/*<       BX = 1.D0    >*/
    tbk21_1.bx = 1.;
/*<       HX = (BX-AX)/DBLE(FLOAT(NGRIDX-1)) >*/
    tbk21_1.hx = (tbk21_1.bx - tbk21_1.ax) / (doublereal) ((real) (
	    tbk22_1.ngridx - 1));
/*<       DO 30 J = 1,NGRIDX     >*/
    i__1 = tbk22_1.ngridx;
    for (j = 1; j <= i__1; ++j) {
/*<          GRIDX(J) = AX+DBLE(FLOAT(J-1))*HX       >*/
	gridx[j - 1] = tbk21_1.ax + (doublereal) ((real) (j - 1)) * 
		tbk21_1.hx;
/*<    30 CONTINUE     >*/
/* L30: */
    }
/*<       GRIDX(NGRIDX) = BX     >*/
    gridx[tbk22_1.ngridx - 1] = tbk21_1.bx;
/*<       BCTYPE(1) = 2  >*/
    tbk22_1.bctype[0] = 2;

/*<       AY = 0.D0    >*/
    tbk21_1.ay = 0.;
/*<       BY = 1.D0    >*/
    tbk21_1.by = 1.;
/*<       HY = (BY-AY)/DBLE(FLOAT(NGRIDY-1)) >*/
    tbk21_1.hy = (tbk21_1.by - tbk21_1.ay) / (doublereal) ((real) (
	    tbk22_1.ngridy - 1));
/*<       DO 40 J = 1,NGRIDY     >*/
    i__1 = tbk22_1.ngridy;
    for (j = 1; j <= i__1; ++j) {
/*<          GRIDY(J) = AY+DBLE(FLOAT(J-1))*HY       >*/
	gridy[j - 1] = tbk21_1.ay + (doublereal) ((real) (j - 1)) * 
		tbk21_1.hy;
/*<    40 CONTINUE     >*/
/* L40: */
    }
/*<       GRIDY(NGRIDY) = BY     >*/
    gridy[tbk22_1.ngridy - 1] = tbk21_1.by;
/*<       BCTYPE(4) = 2  >*/
    tbk22_1.bctype[3] = 2;

/*<       AZ = 0.D0    >*/
    tbk21_1.az = 0.;
/*<       BZ = 1.D0    >*/
    tbk21_1.bz = 1.;
/*<       HZ = (BZ-AZ)/DBLE(FLOAT(NGRIDZ-1)) >*/
    tbk21_1.hz = (tbk21_1.bz - tbk21_1.az) / (doublereal) ((real) (
	    tbk22_1.ngridz - 1));
/*<       DO 50 J = 1,NGRIDZ     >*/
    i__1 = tbk22_1.ngridz;
    for (j = 1; j <= i__1; ++j) {
/*<          GRIDZ(J) = AZ+DBLE(FLOAT(J-1))*HZ       >*/
	gridz[j - 1] = tbk21_1.az + (doublereal) ((real) (j - 1)) * 
		tbk21_1.hz;
/*<    50 CONTINUE     >*/
/* L50: */
    }
/*<       GRIDZ(NGRIDZ) = BZ     >*/
    gridz[tbk22_1.ngridz - 1] = tbk21_1.bz;
/*<       BCTYPE(5) = 2  >*/
    tbk22_1.bctype[4] = 2;

/* ... DISCRETIZE THE ELLIPTIC PDE */

/*<       DO 70 LOOP1 = 1,ITEST  >*/
    i__1 = itest;
    for (loop1 = 1; loop1 <= i__1; ++loop1) {
/*<          ISYM = LOOP1-1      >*/
	tbk22_1.isym = loop1 - 1;
/*<          IF (LOOP1.EQ.2) WRITE (NOUT,60) >*/
	if (loop1 == 2) {
	    io___115.ciunit = tbk22_1.nout;
	    s_wsfe(&io___115);
	    e_wsfe();
	}
/*<    60    FORMAT ('1'///)     >*/
/*<    >*/
	sym7pt_(gridx, &ngrdxd, gridy, &ngrdyd, gridz, &ngrdzd, rhs, &mxneq, 
		ia, ja, a, &nelmax, iwork);

/* ... SOLVE THE MATRIX PROBLEM */

/*<          DO 70 LOOP2 = 1,JTEST >*/
	i__2 = jtest;
	for (loop2 = 1; loop2 <= i__2; ++loop2) {
/*<             NB = LOOP2-2     >*/
	    nb = loop2 - 2;
/*<             IF (ISYM.EQ.0) WRITE (NOUT,80)       >*/
	    if (tbk22_1.isym == 0) {
		io___123.ciunit = tbk22_1.nout;
		s_wsfe(&io___123);
		e_wsfe();
	    }
/*<             IF (ISYM.EQ.1) WRITE (NOUT,90)       >*/
	    if (tbk22_1.isym == 1) {
		io___124.ciunit = tbk22_1.nout;
		s_wsfe(&io___124);
		e_wsfe();
	    }
/*<             IF (NB.EQ.(-1)) WRITE (NOUT,100)     >*/
	    if (nb == -1) {
		io___125.ciunit = tbk22_1.nout;
		s_wsfe(&io___125);
		e_wsfe();
	    }
/*<             IF (NB.EQ.0) WRITE (NOUT,110)        >*/
	    if (nb == 0) {
		io___126.ciunit = tbk22_1.nout;
		s_wsfe(&io___126);
		e_wsfe();
	    }

/*        TEST JCG */

/*<             CALL DFAULT (IPARM,RPARM)  >*/
	    dfault_(iparm, rparm);
/*<             IPARM(1) = ITMAX >*/
	    iparm[0] = itmax;
/*<             IPARM(2) = LEVEL >*/
	    iparm[1] = level;
/*<             IPARM(5) = ISYM  >*/
	    iparm[4] = tbk22_1.isym;
/*<             IPARM(9) = NB    >*/
	    iparm[8] = nb;
/*<             IPARM(12) = IERAN  >*/
	    iparm[11] = ieran;
/*<             RPARM(1) = ZETA  >*/
	    rparm[0] = zeta;
/*<             CALL VFILL (N,U,0.D0)      >*/
	    vfill_(&tbk22_1.n, u, &c_b19);
/*<             CALL JCG (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER) >*/
	    jcg_(&tbk22_1.n, ia, ja, a, rhs, u, iwksp, &nw, wksp, iparm, 
		    rparm, &ier);
/*<             TIM1(1) = RPARM(9) >*/
	    tim1[0] = rparm[8];
/*<             TIM2(1) = RPARM(10)        >*/
	    tim2[0] = rparm[9];
/*<             DIGIT1(1) = RPARM(11)      >*/
	    digit1[0] = rparm[10];
/*<             DIGIT2(1) = RPARM(12)      >*/
	    digit2[0] = rparm[11];
/*<             ITER(1) = IPARM(1) >*/
	    iter[0] = iparm[0];
/*<             IWRK(1) = IPARM(8) >*/
	    iwrk[0] = iparm[7];

/*        TEST JSI */

/*<             CALL DFAULT (IPARM,RPARM)  >*/
	    dfault_(iparm, rparm);
/*<             IPARM(1) = ITMAX >*/
	    iparm[0] = itmax;
/*<             IPARM(2) = LEVEL >*/
	    iparm[1] = level;
/*<             IPARM(5) = ISYM  >*/
	    iparm[4] = tbk22_1.isym;
/*<             IPARM(9) = NB    >*/
	    iparm[8] = nb;
/*<             IPARM(12) = IERAN  >*/
	    iparm[11] = ieran;
/*<             RPARM(1) = ZETA  >*/
	    rparm[0] = zeta;
/*<             CALL VFILL (N,U,0.D0)      >*/
	    vfill_(&tbk22_1.n, u, &c_b19);
/*<             CALL JSI (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER) >*/
	    jsi_(&tbk22_1.n, ia, ja, a, rhs, u, iwksp, &nw, wksp, iparm, 
		    rparm, &ier);
/*<             TIM1(2) = RPARM(9) >*/
	    tim1[1] = rparm[8];
/*<             TIM2(2) = RPARM(10)        >*/
	    tim2[1] = rparm[9];
/*<             DIGIT1(2) = RPARM(11)      >*/
	    digit1[1] = rparm[10];
/*<             DIGIT2(2) = RPARM(12)      >*/
	    digit2[1] = rparm[11];
/*<             ITER(2) = IPARM(1) >*/
	    iter[1] = iparm[0];
/*<             IWRK(2) = IPARM(8) >*/
	    iwrk[1] = iparm[7];

/*        TEST SOR */

/*<             CALL DFAULT (IPARM,RPARM)  >*/
	    dfault_(iparm, rparm);
/*<             IPARM(1) = ITMAX >*/
	    iparm[0] = itmax;
/*<             IPARM(2) = LEVEL >*/
	    iparm[1] = level;
/*<             IPARM(5) = ISYM  >*/
	    iparm[4] = tbk22_1.isym;
/*<             IPARM(9) = NB    >*/
	    iparm[8] = nb;
/*<             IPARM(12) = IERAN  >*/
	    iparm[11] = ieran;
/*<             RPARM(1) = ZETA  >*/
	    rparm[0] = zeta;
/*<             CALL VFILL (N,U,0.D0)      >*/
	    vfill_(&tbk22_1.n, u, &c_b19);
/*<             CALL SOR (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER) >*/
	    sor_(&tbk22_1.n, ia, ja, a, rhs, u, iwksp, &nw, wksp, iparm, 
		    rparm, &ier);
/*<             TIM1(3) = RPARM(9) >*/
	    tim1[2] = rparm[8];
/*<             TIM2(3) = RPARM(10)        >*/
	    tim2[2] = rparm[9];
/*<             DIGIT1(3) = RPARM(11)      >*/
	    digit1[2] = rparm[10];
/*<             DIGIT2(3) = RPARM(12)      >*/
	    digit2[2] = rparm[11];
/*<             ITER(3) = IPARM(1) >*/
	    iter[2] = iparm[0];
/*<             IWRK(3) = IPARM(8) >*/
	    iwrk[2] = iparm[7];

/*        TEST SSORCG */

/*<             CALL DFAULT (IPARM,RPARM)  >*/
	    dfault_(iparm, rparm);
/*<             IPARM(1) = ITMAX >*/
	    iparm[0] = itmax;
/*<             IPARM(2) = LEVEL >*/
	    iparm[1] = level;
/*<             IPARM(5) = ISYM  >*/
	    iparm[4] = tbk22_1.isym;
/*<             IPARM(9) = NB    >*/
	    iparm[8] = nb;
/*<             IPARM(12) = IERAN  >*/
	    iparm[11] = ieran;
/*<             RPARM(1) = ZETA  >*/
	    rparm[0] = zeta;
/*<             CALL VFILL (N,U,0.D0)      >*/
	    vfill_(&tbk22_1.n, u, &c_b19);
/*<             CALL SSORCG (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER)  >*/
	    ssorcg_(&tbk22_1.n, ia, ja, a, rhs, u, iwksp, &nw, wksp, iparm, 
		    rparm, &ier);
/*<             TIM1(4) = RPARM(9) >*/
	    tim1[3] = rparm[8];
/*<             TIM2(4) = RPARM(10)        >*/
	    tim2[3] = rparm[9];
/*<             DIGIT1(4) = RPARM(11)      >*/
	    digit1[3] = rparm[10];
/*<             DIGIT2(4) = RPARM(12)      >*/
	    digit2[3] = rparm[11];
/*<             ITER(4) = IPARM(1) >*/
	    iter[3] = iparm[0];
/*<             IWRK(4) = IPARM(8) >*/
	    iwrk[3] = iparm[7];

/*        TEST SSORSI */

/*<             CALL DFAULT (IPARM,RPARM)  >*/
	    dfault_(iparm, rparm);
/*<             IPARM(1) = ITMAX >*/
	    iparm[0] = itmax;
/*<             IPARM(2) = LEVEL >*/
	    iparm[1] = level;
/*<             IPARM(5) = ISYM  >*/
	    iparm[4] = tbk22_1.isym;
/*<             IPARM(9) = NB    >*/
	    iparm[8] = nb;
/*<             IPARM(12) = IERAN  >*/
	    iparm[11] = ieran;
/*<             RPARM(1) = ZETA  >*/
	    rparm[0] = zeta;
/*<             CALL VFILL (N,U,0.D0)      >*/
	    vfill_(&tbk22_1.n, u, &c_b19);
/*<             CALL SSORSI (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER)  >*/
	    ssorsi_(&tbk22_1.n, ia, ja, a, rhs, u, iwksp, &nw, wksp, iparm, 
		    rparm, &ier);
/*<             TIM1(5) = RPARM(9) >*/
	    tim1[4] = rparm[8];
/*<             TIM2(5) = RPARM(10)        >*/
	    tim2[4] = rparm[9];
/*<             DIGIT1(5) = RPARM(11)      >*/
	    digit1[4] = rparm[10];
/*<             DIGIT2(5) = RPARM(12)      >*/
	    digit2[4] = rparm[11];
/*<             ITER(5) = IPARM(1) >*/
	    iter[4] = iparm[0];
/*<             IWRK(5) = IPARM(8) >*/
	    iwrk[4] = iparm[7];

/*        TEST RSCG */

/*<             CALL DFAULT (IPARM,RPARM)  >*/
	    dfault_(iparm, rparm);
/*<             IPARM(1) = ITMAX >*/
	    iparm[0] = itmax;
/*<             IPARM(2) = LEVEL >*/
	    iparm[1] = level;
/*<             IPARM(5) = ISYM  >*/
	    iparm[4] = tbk22_1.isym;
/*<             IPARM(12) = IERAN  >*/
	    iparm[11] = ieran;
/*<             RPARM(1) = ZETA  >*/
	    rparm[0] = zeta;
/*<             CALL VFILL (N,U,0.D0)      >*/
	    vfill_(&tbk22_1.n, u, &c_b19);
/*<             CALL RSCG (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER)  >*/
	    rscg_(&tbk22_1.n, ia, ja, a, rhs, u, iwksp, &nw, wksp, iparm, 
		    rparm, &ier);
/*<             TIM1(6) = RPARM(9) >*/
	    tim1[5] = rparm[8];
/*<             TIM2(6) = RPARM(10)        >*/
	    tim2[5] = rparm[9];
/*<             DIGIT1(6) = RPARM(11)      >*/
	    digit1[5] = rparm[10];
/*<             DIGIT2(6) = RPARM(12)      >*/
	    digit2[5] = rparm[11];
/*<             ITER(6) = IPARM(1) >*/
	    iter[5] = iparm[0];
/*<             IWRK(6) = IPARM(8) >*/
	    iwrk[5] = iparm[7];

/*        TEST RSSI */

/*<             CALL DFAULT (IPARM,RPARM)  >*/
	    dfault_(iparm, rparm);
/*<             IPARM(1) = ITMAX >*/
	    iparm[0] = itmax;
/*<             IPARM(2) = LEVEL >*/
	    iparm[1] = level;
/*<             IPARM(5) = ISYM  >*/
	    iparm[4] = tbk22_1.isym;
/*<             IPARM(12) = IERAN  >*/
	    iparm[11] = ieran;
/*<             RPARM(1) = ZETA  >*/
	    rparm[0] = zeta;
/*<             CALL VFILL (N,U,0.D0)      >*/
	    vfill_(&tbk22_1.n, u, &c_b19);
/*<             CALL RSSI (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER)  >*/
	    rssi_(&tbk22_1.n, ia, ja, a, rhs, u, iwksp, &nw, wksp, iparm, 
		    rparm, &ier);
/*<             TIM1(7) = RPARM(9) >*/
	    tim1[6] = rparm[8];
/*<             TIM2(7) = RPARM(10)        >*/
	    tim2[6] = rparm[9];
/*<             DIGIT1(7) = RPARM(11)      >*/
	    digit1[6] = rparm[10];
/*<             DIGIT2(7) = RPARM(12)      >*/
	    digit2[6] = rparm[11];
/*<             ITER(7) = IPARM(1) >*/
	    iter[6] = iparm[0];
/*<             IWRK(7) = IPARM(8) >*/
	    iwrk[6] = iparm[7];

/*     TIMING ANALYSIS */

/*<    >*/
	    time1_(&tbk22_1.n, ia, ja, a, wksp, &wksp[tbk22_1.n], iter, tim1, 
		    tim2, digit1, digit2, iwrk, &tbk22_1.nout);
/*<    70 CONTINUE     >*/
/* L70: */
	}
    }

/*<    80 FORMAT (//15X,'SYMMETRIC SPARSE STORAGE USED')       >*/
/*<    90 FORMAT (//15X,'NONSYMMETRIC SPARSE STORAGE USED')    >*/
/*<   100 FORMAT (15X,'NATURAL ORDERING USED')       >*/
/*<   110 FORMAT (15X,'RED-BLACK ORDERING USED')     >*/
/*<       RETURN       >*/
    return 0;
/*<       END  >*/
} /* test2d_ */

/*<       SUBROUTINE PDE2 (X,Y,Z,CVALUS)   >*/
/* Subroutine */ int pde2_(doublereal *x, doublereal *y, doublereal *z__, 
	doublereal *cvalus)
{

/* ... THIS SUBROUTINE IS A USER SUPPLIED SUBROUTINE TO SPECIFY THE */
/* ... SELF-ADJOINT ELLIPTIC PDE FOR SYM7PT IN THE FOLLOWING FORM */

/*        (CVALUS(1)*UX)X + (CVALUS(3)*UY)Y + (CVALUS(6)*UZ)Z */
/*                                          + CVALUS(10)*U = CVALUS(11) */

/*     NOTE:  CVALUS(I), FOR I = 2, 4, 5, 7, 8, AND 9 ARE NOT USED. */

/*<       DOUBLE PRECISION CVALUS(11),X,Y,Z  >*/

/*<       CVALUS(1) = 1.D0       >*/
    /* Parameter adjustments */
    --cvalus;

    /* Function Body */
    cvalus[1] = 1.;
/*<       CVALUS(2) = 0.D0       >*/
    cvalus[2] = 0.;
/*<       CVALUS(3) = 2.D0       >*/
    cvalus[3] = 2.;
/*<       CVALUS(4) = 0.D0       >*/
    cvalus[4] = 0.;
/*<       CVALUS(5) = 0.D0       >*/
    cvalus[5] = 0.;
/*<       CVALUS(6) = 1.D0       >*/
    cvalus[6] = 1.;
/*<       CVALUS(7) = 0.D0       >*/
    cvalus[7] = 0.;
/*<       CVALUS(8) = 0.D0       >*/
    cvalus[8] = 0.;
/*<       CVALUS(9) = 0.D0       >*/
    cvalus[9] = 0.;
/*<       CVALUS(10) = 0.D0      >*/
    cvalus[10] = 0.;
/*<       CVALUS(11) = 0.D0      >*/
    cvalus[11] = 0.;

/*<       RETURN       >*/
    return 0;
/*<       END  >*/
} /* pde2_ */

/*<       DOUBLE PRECISION FUNCTION BCOND2 (ISIDE,X,Y,Z,BVALUS)  >*/
doublereal bcond2_(integer *iside, doublereal *x, doublereal *y, doublereal *
	z__, doublereal *bvalus)
{
    /* System generated locals */
    doublereal ret_val;


/* ... THIS DOUBLE PRECISION FUNCTION IS A USER SUPPLIED FUNCTION TO SPEC */
/* ... BOUNDARY CONDITIONS OF THE ELLIPTIC PDE DEPENDING ON ISIDE, X, */
/* ... Y, AND Z. */

/*        IF ISIDE = 1, THEN X = BX (EAST SIDE) */
/*                 = 2, THEN Y = AY (SOUTH SIDE) */
/*                 = 3, THEN X = AX (WEST SIDE) */
/*                 = 4, THEN Y = BY (NORTH SIDE) */
/*                 = 5, THEN Z = BZ (TOP SIDE) */
/*                 = 6, THEN Z = AZ (BOTTOM SIDE) */

/* ... THE BVALUS ARRAY IS DEFINED AS FOLLOWS */

/*        BVALUS(1)*U + BVALUS(2)*UX + BVALUS(3)*UY + BVALUS(4)*UZ */
/*                                                  = BVALUS(5) */

/*        NOTE:  BCOND2 IS SET TO BVALUS(5) BEFORE RETURNING. */

/*<       DOUBLE PRECISION BVALUS(5),X,Y,Z >*/

/*<       GO TO (10,20,30,40,50,60), ISIDE >*/
    /* Parameter adjustments */
    --bvalus;

    /* Function Body */
    switch (*iside) {
	case 1:  goto L10;
	case 2:  goto L20;
	case 3:  goto L30;
	case 4:  goto L40;
	case 5:  goto L50;
	case 6:  goto L60;
    }

/*<    10 BVALUS(2) = 1.D0       >*/
L10:
    bvalus[2] = 1.;
/*<       BVALUS(1) = 0.D0       >*/
    bvalus[1] = 0.;
/*<       BVALUS(5) = Y*Z*(1.D0+Y*Z)       >*/
    bvalus[5] = *y * *z__ * (*y * *z__ + 1.);
/*<       BVALUS(3) = 0.D0       >*/
    bvalus[3] = 0.;
/*<       BVALUS(4) = 0.D0       >*/
    bvalus[4] = 0.;
/*<       GO TO 70     >*/
    goto L70;

/*<    20 BVALUS(1) = 1.D0       >*/
L20:
    bvalus[1] = 1.;
/*<       BVALUS(5) = 1.D0       >*/
    bvalus[5] = 1.;
/*<       BVALUS(2) = 0.D0       >*/
    bvalus[2] = 0.;
/*<       BVALUS(3) = 0.D0       >*/
    bvalus[3] = 0.;
/*<       BVALUS(4) = 0.D0       >*/
    bvalus[4] = 0.;
/*<       GO TO 70     >*/
    goto L70;

/*<    30 BVALUS(1) = 1.D0       >*/
L30:
    bvalus[1] = 1.;
/*<       BVALUS(5) = 1.D0       >*/
    bvalus[5] = 1.;
/*<       BVALUS(2) = 0.D0       >*/
    bvalus[2] = 0.;
/*<       BVALUS(3) = 0.D0       >*/
    bvalus[3] = 0.;
/*<       BVALUS(4) = 0.D0       >*/
    bvalus[4] = 0.;
/*<       GO TO 70     >*/
    goto L70;

/*<    40 BVALUS(3) = 1.D0       >*/
L40:
    bvalus[3] = 1.;
/*<       BVALUS(1) = 0.D0       >*/
    bvalus[1] = 0.;
/*<       BVALUS(2) = 0.D0       >*/
    bvalus[2] = 0.;
/*<       BVALUS(5) = X*Z*(1.D0+X*Z)       >*/
    bvalus[5] = *x * *z__ * (*x * *z__ + 1.);
/*<       BVALUS(4) = 0.D0       >*/
    bvalus[4] = 0.;
/*<       GO TO 70     >*/
    goto L70;

/*<    50 BVALUS(4) = 1.D0       >*/
L50:
    bvalus[4] = 1.;
/*<       BVALUS(1) = 0.D0       >*/
    bvalus[1] = 0.;
/*<       BVALUS(2) = 0.D0       >*/
    bvalus[2] = 0.;
/*<       BVALUS(3) = 0.D0       >*/
    bvalus[3] = 0.;
/*<       BVALUS(5) = X*Y*(1.D0+X*Y)       >*/
    bvalus[5] = *x * *y * (*x * *y + 1.);
/*<       GO TO 70     >*/
    goto L70;

/*<    60 BVALUS(1) = 1.D0       >*/
L60:
    bvalus[1] = 1.;
/*<       BVALUS(5) = 1.D0       >*/
    bvalus[5] = 1.;
/*<       BVALUS(2) = 0.D0       >*/
    bvalus[2] = 0.;
/*<       BVALUS(3) = 0.D0       >*/
    bvalus[3] = 0.;
/*<       BVALUS(4) = 0.D0       >*/
    bvalus[4] = 0.;

/*<    70 CONTINUE     >*/
L70:
/*<       BCOND2 = BVALUS(5)     >*/
    ret_val = bvalus[5];
/*<       RETURN       >*/
    return ret_val;
/*<       END  >*/
} /* bcond2_ */

/*<    >*/
/* Subroutine */ int sym7pt_(doublereal *gridx, integer *ngrdxd, doublereal *
	gridy, integer *ngrdyd, doublereal *gridz, integer *ngrdzd, 
	doublereal *rhs, integer *mxneq, integer *ia, integer *ja, doublereal 
	*a, integer *nels, integer *iwksp)
{
    /* Format strings */
    static char fmt_10[] = "(//10x,\002FINITE DIFFERENCE MODULE\002,\002 ---\
- \002,\002SYMMETRIC SEVEN POINT\002//10x,\002DOMAIN = BOX  (\002,d11.4,\002,\
\002,d11.4,\002) X (\002,d11.4,\002,\002,d11.4,\002) X (\002,d11.4,\002,\002\
,d11.4,\002)\002)";
    static char fmt_20[] = "(/10x,\002COEFFICIENTS OF X-MESH LINES\002)";
    static char fmt_30[] = "(/8x,8(2x,d11.4))";
    static char fmt_40[] = "(/10x,\002COEFFICIENTS OF Y-MESH LINES\002)";
    static char fmt_50[] = "(/10x,\002COEFFICIENTS OF Z-MESH LINES\002)";
    static char fmt_60[] = "(/10x,\002BOUNDARY CONDITIONS ON PIECES 1,2,3,4,\
5,6 ARE \002,5(1x,i1,\002,\002),1x,i1,\002.\002//)";
    static char fmt_90[] = "(/10x,\002SYM7PT COMPLETED SUCCESSFULLY.\002)";
    static char fmt_100[] = "(10x,\002SPARSE MATRIX REPRESENTATION FINISHED\
.\002/15x,\002NO. OF EQUATIONS        =\002,i8/15x,\002NO. OF NON-ZEROES    \
   =\002,i8/15x,\002TOTAL MATRIX STORAGE    =\002,i8/)";
    static char fmt_120[] = "(//20x,\002SYM7PT ERROR -- NGRIDX .LT. 3 \002)";
    static char fmt_130[] = "(//20x,\002SYM7PT ERROR -- NGRIDY .LT. 3 \002)";
    static char fmt_140[] = "(//20x,\002SYM7PT ERROR -- NGRIDZ .LT. 3 \002)";
    static char fmt_160[] = "(/10x,\002N .GT. MXNEQ, N =\002,i10,\002 MXNE\
Q =\002,i10)";

    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();
    /* Subroutine */ int s_stop(char *, ftnlen);

    /* Local variables */
    doublereal b;
    integer i__;
    doublereal hd, he;
    integer ij;
    doublereal hn, hu, hs, hw;
    integer nu;
    doublereal px, py, pz;
    integer ix1, ix2, jy1, jy2, kz1, kz2, nuu, ixx, jyy, kzz, ijin;
    extern /* Subroutine */ int pnt3d_(doublereal *, doublereal *, doublereal 
	    *, integer *, integer *, integer *, integer *, integer *, integer 
	    *, doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *, 
	    integer *, doublereal *, integer *), sbend_(integer *, integer *, 
	    integer *, integer *, doublereal *, integer *), sbini_(integer *, 
	    integer *, integer *, integer *, doublereal *, integer *);
    integer lngthx, lngthy;

    /* Fortran I/O blocks */
    static cilist io___147 = { 0, 0, 0, fmt_10, 0 };
    static cilist io___148 = { 0, 0, 0, fmt_20, 0 };
    static cilist io___149 = { 0, 0, 0, fmt_30, 0 };
    static cilist io___151 = { 0, 0, 0, fmt_40, 0 };
    static cilist io___152 = { 0, 0, 0, fmt_30, 0 };
    static cilist io___153 = { 0, 0, 0, fmt_50, 0 };
    static cilist io___154 = { 0, 0, 0, fmt_30, 0 };
    static cilist io___155 = { 0, 0, 0, fmt_60, 0 };
    static cilist io___171 = { 0, 0, 0, fmt_90, 0 };
    static cilist io___174 = { 0, 0, 0, fmt_100, 0 };
    static cilist io___175 = { 0, 0, 0, fmt_120, 0 };
    static cilist io___176 = { 0, 0, 0, fmt_130, 0 };
    static cilist io___177 = { 0, 0, 0, fmt_140, 0 };
    static cilist io___178 = { 0, 0, 0, fmt_160, 0 };



/*<       INTEGER IA(1),JA(NELS),BCTYPE(6),IWKSP(NELS) >*/
/*<    >*/

/* ...  SYM7PT COMMON BLOCKS */

/*<       COMMON /TBK21/ AX,AY,AZ,BX,BY,BZ,DRELPR,HX,HY,HZ     >*/
/*<       COMMON /TBK22/ BCTYPE,ILEVEL,ISYM,NOUT,NGRIDX,NGRIDY,NGRIDZ,N  >*/

/*<       IF (NGRIDX.LT.3.OR.NGRIDY.LT.3.OR.NGRIDZ.LT.3) GO TO 110       >*/
    /* Parameter adjustments */
    --gridx;
    --gridy;
    --gridz;
    --rhs;
    --ia;
    --iwksp;
    --a;
    --ja;

    /* Function Body */
    if (tbk22_1.ngridx < 3 || tbk22_1.ngridy < 3 || tbk22_1.ngridz < 3) {
	goto L110;
    }

/*     DETERMINE RANGE OF UNKNOWN GRID POINTS */

/*<       IX1 = 1      >*/
    ix1 = 1;
/*<       IX2 = NGRIDX >*/
    ix2 = tbk22_1.ngridx;
/*<       JY1 = 1      >*/
    jy1 = 1;
/*<       JY2 = NGRIDY >*/
    jy2 = tbk22_1.ngridy;
/*<       KZ1 = 1      >*/
    kz1 = 1;
/*<       KZ2 = NGRIDZ >*/
    kz2 = tbk22_1.ngridz;
/*<       IF (BCTYPE(1).EQ.1) IX2 = NGRIDX-1 >*/
    if (tbk22_1.bctype[0] == 1) {
	ix2 = tbk22_1.ngridx - 1;
    }
/*<       IF (BCTYPE(2).EQ.1) JY1 = 2      >*/
    if (tbk22_1.bctype[1] == 1) {
	jy1 = 2;
    }
/*<       IF (BCTYPE(3).EQ.1) IX1 = 2      >*/
    if (tbk22_1.bctype[2] == 1) {
	ix1 = 2;
    }
/*<       IF (BCTYPE(4).EQ.1) JY2 = NGRIDY-1 >*/
    if (tbk22_1.bctype[3] == 1) {
	jy2 = tbk22_1.ngridy - 1;
    }
/*<       IF (BCTYPE(6).EQ.1) KZ1 = 2      >*/
    if (tbk22_1.bctype[5] == 1) {
	kz1 = 2;
    }
/*<       IF (BCTYPE(5).EQ.1) KZ2 = NGRIDZ-1 >*/
    if (tbk22_1.bctype[4] == 1) {
	kz2 = tbk22_1.ngridz - 1;
    }
/*<       LNGTHX = IX2-IX1+1     >*/
    lngthx = ix2 - ix1 + 1;
/*<       LNGTHY = JY2-JY1+1     >*/
    lngthy = jy2 - jy1 + 1;
/*<       N = LNGTHX*LNGTHY*(KZ2-KZ1+1)    >*/
    tbk22_1.n = lngthx * lngthy * (kz2 - kz1 + 1);
/*<       IF (N.GT.MXNEQ) GO TO 150        >*/
    if (tbk22_1.n > *mxneq) {
	goto L150;
    }

/*     OUTPUT INITIAL GRID INFORMATION */

/*<       IF (ILEVEL.EQ.0) GO TO 70        >*/
    if (tbk22_1.ilevel == 0) {
	goto L70;
    }
/*<       WRITE (NOUT,10) AX,BX,AY,BY,AZ,BZ  >*/
    io___147.ciunit = tbk22_1.nout;
    s_wsfe(&io___147);
    do_fio(&c__1, (char *)&tbk21_1.ax, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&tbk21_1.bx, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&tbk21_1.ay, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&tbk21_1.by, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&tbk21_1.az, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&tbk21_1.bz, (ftnlen)sizeof(doublereal));
    e_wsfe();
/*<    >*/

/*<       WRITE (NOUT,20)        >*/
    io___148.ciunit = tbk22_1.nout;
    s_wsfe(&io___148);
    e_wsfe();
/*<    20 FORMAT (/10X,'COEFFICIENTS OF X-MESH LINES') >*/
/*<       WRITE (NOUT,30) (GRIDX(I),I=1,NGRIDX)      >*/
    io___149.ciunit = tbk22_1.nout;
    s_wsfe(&io___149);
    i__1 = tbk22_1.ngridx;
    for (i__ = 1; i__ <= i__1; ++i__) {
	do_fio(&c__1, (char *)&gridx[i__], (ftnlen)sizeof(doublereal));
    }
    e_wsfe();
/*<    30 FORMAT (/8X,8(2X,D11.4)) >*/
/*<       WRITE (NOUT,40)        >*/
    io___151.ciunit = tbk22_1.nout;
    s_wsfe(&io___151);
    e_wsfe();
/*<    40 FORMAT (/10X,'COEFFICIENTS OF Y-MESH LINES') >*/
/*<       WRITE (NOUT,30) (GRIDY(I),I=1,NGRIDY)      >*/
    io___152.ciunit = tbk22_1.nout;
    s_wsfe(&io___152);
    i__1 = tbk22_1.ngridy;
    for (i__ = 1; i__ <= i__1; ++i__) {
	do_fio(&c__1, (char *)&gridy[i__], (ftnlen)sizeof(doublereal));
    }
    e_wsfe();
/*<       WRITE (NOUT,50)        >*/
    io___153.ciunit = tbk22_1.nout;
    s_wsfe(&io___153);
    e_wsfe();
/*<    50 FORMAT (/10X,'COEFFICIENTS OF Z-MESH LINES') >*/
/*<       WRITE (NOUT,30) (GRIDZ(I),I=1,NGRIDZ)      >*/
    io___154.ciunit = tbk22_1.nout;
    s_wsfe(&io___154);
    i__1 = tbk22_1.ngridz;
    for (i__ = 1; i__ <= i__1; ++i__) {
	do_fio(&c__1, (char *)&gridz[i__], (ftnlen)sizeof(doublereal));
    }
    e_wsfe();
/*<       WRITE (NOUT,60) (BCTYPE(I),I=1,6)  >*/
    io___155.ciunit = tbk22_1.nout;
    s_wsfe(&io___155);
    for (i__ = 1; i__ <= 6; ++i__) {
	do_fio(&c__1, (char *)&tbk22_1.bctype[i__ - 1], (ftnlen)sizeof(
		integer));
    }
    e_wsfe();
/*<    >*/

/*     GENERATE EQUATIONS ONE MESH POINT AT A TIME */

/*<    70 CONTINUE     >*/
L70:
/*<       CALL SBINI (N,NELS,IA,JA,A,IWKSP)  >*/
    sbini_(&tbk22_1.n, nels, &ia[1], &ja[1], &a[1], &iwksp[1]);

/*<       DO 80 IJ = 1,N >*/
    i__1 = tbk22_1.n;
    for (ij = 1; ij <= i__1; ++ij) {
/*<          IXX = MOD(IJ-1,LNGTHX)+1      >*/
	ixx = (ij - 1) % lngthx + 1;
/*<          JYY = MOD((IJ-IXX)/LNGTHX,LNGTHY)+1     >*/
	jyy = (ij - ixx) / lngthx % lngthy + 1;
/*<          KZZ = (IJ-IXX-(JYY-1)*LNGTHX)/(LNGTHX*LNGTHY)+1   >*/
	kzz = (ij - ixx - (jyy - 1) * lngthx) / (lngthx * lngthy) + 1;
/*<          IXX = IXX+IX1-1     >*/
	ixx = ixx + ix1 - 1;
/*<          JYY = JYY+JY1-1     >*/
	jyy = jyy + jy1 - 1;
/*<          KZZ = KZZ+KZ1-1     >*/
	kzz = kzz + kz1 - 1;

/*<          HU = 0.D0 >*/
	hu = 0.;
/*<          HD = 0.D0 >*/
	hd = 0.;
/*<          HW = 0.D0 >*/
	hw = 0.;
/*<          HE = 0.D0 >*/
	he = 0.;
/*<          HS = 0.D0 >*/
	hs = 0.;
/*<          HN = 0.D0 >*/
	hn = 0.;
/*<          PZ = GRIDZ(KZZ)     >*/
	pz = gridz[kzz];
/*<          IF (KZZ.NE.1) HD = PZ-GRIDZ(KZZ-1)      >*/
	if (kzz != 1) {
	    hd = pz - gridz[kzz - 1];
	}
/*<          IF (KZZ.NE.NGRIDZ) HU = GRIDZ(KZZ+1)-PZ >*/
	if (kzz != tbk22_1.ngridz) {
	    hu = gridz[kzz + 1] - pz;
	}
/*<          PY = GRIDY(JYY)     >*/
	py = gridy[jyy];
/*<          IF (JYY.NE.1) HS = PY-GRIDY(JYY-1)      >*/
	if (jyy != 1) {
	    hs = py - gridy[jyy - 1];
	}
/*<          IF (JYY.NE.NGRIDY) HN = GRIDY(JYY+1)-PY >*/
	if (jyy != tbk22_1.ngridy) {
	    hn = gridy[jyy + 1] - py;
	}
/*<          PX = GRIDX(IXX)     >*/
	px = gridx[ixx];
/*<          IF (IXX.NE.1) HW = PX-GRIDX(IXX-1)      >*/
	if (ixx != 1) {
	    hw = px - gridx[ixx - 1];
	}
/*<          IF (IXX.NE.NGRIDX) HE = GRIDX(IXX+1)-PX >*/
	if (ixx != tbk22_1.ngridx) {
	    he = gridx[ixx + 1] - px;
	}

/*<          IJIN = IJ >*/
	ijin = ij;
/*<    >*/
	pnt3d_(&px, &py, &pz, &ijin, &ixx, &jyy, &kzz, &lngthx, &lngthy, &hn, 
		&hs, &hw, &he, &hu, &hd, &b, nels, &ia[1], &ja[1], &a[1], &
		iwksp[1]);

/*<          RHS(IJ) = B >*/
	rhs[ij] = b;
/*<    80 CONTINUE     >*/
/* L80: */
    }
/*<       CALL SBEND (N,NELS,IA,JA,A,IWKSP)  >*/
    sbend_(&tbk22_1.n, nels, &ia[1], &ja[1], &a[1], &iwksp[1]);

/*     NORMAL EXIT */

/*<       IF (ILEVEL.EQ.0) RETURN  >*/
    if (tbk22_1.ilevel == 0) {
	return 0;
    }

/*<       WRITE (NOUT,90)        >*/
    io___171.ciunit = tbk22_1.nout;
    s_wsfe(&io___171);
    e_wsfe();
/*<    90 FORMAT (/10X,'SYM7PT COMPLETED SUCCESSFULLY.')       >*/
/*<       NU = IA(N+1)-1 >*/
    nu = ia[tbk22_1.n + 1] - 1;
/*<       NUU = N+1+2*NU >*/
    nuu = tbk22_1.n + 1 + (nu << 1);
/*<       WRITE (NOUT,100) N,NU,NUU        >*/
    io___174.ciunit = tbk22_1.nout;
    s_wsfe(&io___174);
    do_fio(&c__1, (char *)&tbk22_1.n, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&nu, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&nuu, (ftnlen)sizeof(integer));
    e_wsfe();
/*<    >*/

/*<       RETURN       >*/
    return 0;

/*     ERROR EXITS */

/*<   110 CONTINUE     >*/
L110:
/*<       IF (NGRIDX.LT.3) WRITE (NOUT,120)  >*/
    if (tbk22_1.ngridx < 3) {
	io___175.ciunit = tbk22_1.nout;
	s_wsfe(&io___175);
	e_wsfe();
    }
/*<   120 FORMAT (//20X,'SYM7PT ERROR -- NGRIDX .LT. 3 ')      >*/
/*<       IF (NGRIDY.LT.3) WRITE (NOUT,130)  >*/
    if (tbk22_1.ngridy < 3) {
	io___176.ciunit = tbk22_1.nout;
	s_wsfe(&io___176);
	e_wsfe();
    }
/*<   130 FORMAT (//20X,'SYM7PT ERROR -- NGRIDY .LT. 3 ')      >*/
/*<       IF (NGRIDZ.LT.3) WRITE (NOUT,140)  >*/
    if (tbk22_1.ngridz < 3) {
	io___177.ciunit = tbk22_1.nout;
	s_wsfe(&io___177);
	e_wsfe();
    }
/*<   140 FORMAT (//20X,'SYM7PT ERROR -- NGRIDZ .LT. 3 ')      >*/

/*<       STOP >*/
    s_stop("", (ftnlen)0);

/*<   150 WRITE (NOUT,160) N,MXNEQ >*/
L150:
    io___178.ciunit = tbk22_1.nout;
    s_wsfe(&io___178);
    do_fio(&c__1, (char *)&tbk22_1.n, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&(*mxneq), (ftnlen)sizeof(integer));
    e_wsfe();
/*<   160 FORMAT (/10X,'N .GT. MXNEQ, N =',I10,' MXNEQ =',I10) >*/
/*<       STOP >*/
    s_stop("", (ftnlen)0);

/*<       END  >*/
    return 0;
} /* sym7pt_ */

/*<    >*/
/* Subroutine */ int pnt3d_(doublereal *pcx, doublereal *pcy, doublereal *pcz,
	 integer *ij, integer *ix, integer *jy, integer *kz, integer *lngthx, 
	integer *lngthy, doublereal *hn, doublereal *hs, doublereal *hw, 
	doublereal *he, doublereal *hu, doublereal *hd, doublereal *b, 
	integer *nels, integer *ia, integer *ja, doublereal *a, integer *
	iwksp)
{
    /* Format strings */
    static char fmt_190[] = "(/10x,\002SYM7PT -- EQUATION\002,i8/20x,\002RIG\
HT HAND SIDE =       \002,d15.7)";

    /* System generated locals */
    integer i__1;
    doublereal d__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    doublereal cup;
    integer lxy;
    extern /* Subroutine */ int pde2_(doublereal *, doublereal *, doublereal *
	    , doublereal *);
    doublereal crhs, temp, ceast, cwest;
    extern doublereal bcond2_(integer *, doublereal *, doublereal *, 
	    doublereal *, doublereal *);
    extern /* Subroutine */ int stval2_(integer *, integer *, integer *, 
	    doublereal *, integer *, integer *, doublereal *, integer *);
    doublereal center, clower, bvalus[5], cvalus[11], cnorth, csouth;

    /* Fortran I/O blocks */
    static cilist io___190 = { 0, 0, 0, fmt_190, 0 };



/*<       INTEGER IA(1),JA(NELS),BCTYPE(6),IWKSP(NELS) >*/
/*<    >*/

/* ...  SYM7PT COMMON BLOCKS */

/*<       COMMON /TBK21/ AX,AY,AZ,BX,BY,BZ,DRELPR,HX,HY,HZ     >*/
/*<       COMMON /TBK22/ BCTYPE,ILEVEL,ISYM,NOUT,NGRIDX,NGRIDY,NGRIDZ,N  >*/

/*     INITIALIZE COEFFICIENTS */

/*<       CUP = 0.D0   >*/
    /* Parameter adjustments */
    --iwksp;
    --a;
    --ja;
    --ia;

    /* Function Body */
    cup = 0.;
/*<       CLOWER = 0.D0  >*/
    clower = 0.;
/*<       CEAST = 0.D0 >*/
    ceast = 0.;
/*<       CWEST = 0.D0 >*/
    cwest = 0.;
/*<       CNORTH = 0.D0  >*/
    cnorth = 0.;
/*<       CSOUTH = 0.D0  >*/
    csouth = 0.;
/*<       CALL PDE2 (PCX,PCY,PCZ,CVALUS)   >*/
    pde2_(pcx, pcy, pcz, cvalus);
/*<       CENTER = -CVALUS(10)*(HN+HS)*(HE+HW)*(HU+HD)/8.D0    >*/
    center = -cvalus[9] * (*hn + *hs) * (*he + *hw) * (*hu + *hd) / 8.;
/*<       CRHS = -CVALUS(11)*(HN+HS)*(HE+HW)*(HU+HD)/8.D0      >*/
    crhs = -cvalus[10] * (*hn + *hs) * (*he + *hw) * (*hu + *hd) / 8.;

/*     SET EAST COEFFICIENT */

/*<       CALL PDE2 (PCX+.5D0*HE,PCY,PCZ,CVALUS)     >*/
    d__1 = *pcx + *he * .5;
    pde2_(&d__1, pcy, pcz, cvalus);
/*<       IF (IX.GE.NGRIDX-1) TEMP = BCOND2(1,BX,PCY,PCZ,BVALUS) >*/
    if (*ix >= tbk22_1.ngridx - 1) {
	temp = bcond2_(&c__1, &tbk21_1.bx, pcy, pcz, bvalus);
    }
/*<       IF (IX.EQ.NGRIDX) GO TO 20       >*/
    if (*ix == tbk22_1.ngridx) {
	goto L20;
    }
/*<       IF ((IX+1.EQ.NGRIDX).AND.(BCTYPE(1).EQ.1)) GO TO 10  >*/
    if (*ix + 1 == tbk22_1.ngridx && tbk22_1.bctype[0] == 1) {
	goto L10;
    }

/*     NORMAL EAST POINT */

/*<       CEAST = -CVALUS(1)*(HN+HS)*(HU+HD)/(4.D0*HE) >*/
    ceast = -cvalus[0] * (*hn + *hs) * (*hu + *hd) / (*he * 4.);
/*<       CENTER = CENTER-CEAST  >*/
    center -= ceast;
/*<       GO TO 30     >*/
    goto L30;

/*     EAST POINT IS A DIRICHLET POINT */

/*<    10 TEMP = CVALUS(1)*(HN+HS)*(HU+HD)/(4.D0*HE) >*/
L10:
    temp = cvalus[0] * (*hn + *hs) * (*hu + *hd) / (*he * 4.);
/*<       CRHS = CRHS+BVALUS(5)*TEMP/BVALUS(1)       >*/
    crhs += bvalus[4] * temp / bvalus[0];
/*<       CENTER = CENTER+TEMP   >*/
    center += temp;
/*<       GO TO 30     >*/
    goto L30;

/*     CENTER POINT LIES ON THE EAST BOUNDARY WHICH IS MIXED */

/*<    20 TEMP = CVALUS(1)*(HN+HS)*(HU+HD)/(4.D0*BVALUS(2))    >*/
L20:
    temp = cvalus[0] * (*hn + *hs) * (*hu + *hd) / (bvalus[1] * 4.);
/*<       CENTER = CENTER+BVALUS(1)*TEMP   >*/
    center += bvalus[0] * temp;
/*<       CRHS = CRHS+BVALUS(5)*TEMP       >*/
    crhs += bvalus[4] * temp;

/*     SET WEST COEFFICIENT */

/*<    30 CALL PDE2 (PCX-.5D0*HW,PCY,PCZ,CVALUS)     >*/
L30:
    d__1 = *pcx - *hw * .5;
    pde2_(&d__1, pcy, pcz, cvalus);
/*<       IF (IX.LE.2) TEMP = BCOND2(3,AX,PCY,PCZ,BVALUS)      >*/
    if (*ix <= 2) {
	temp = bcond2_(&c__3, &tbk21_1.ax, pcy, pcz, bvalus);
    }
/*<       IF (IX.EQ.1) GO TO 50  >*/
    if (*ix == 1) {
	goto L50;
    }
/*<       IF (IX.EQ.2.AND.BCTYPE(3).EQ.1) GO TO 40   >*/
    if (*ix == 2 && tbk22_1.bctype[2] == 1) {
	goto L40;
    }

/*     NORMAL WEST POINT */

/*<       CWEST = -CVALUS(1)*(HN+HS)*(HU+HD)/(4.D0*HW) >*/
    cwest = -cvalus[0] * (*hn + *hs) * (*hu + *hd) / (*hw * 4.);
/*<       CENTER = CENTER-CWEST  >*/
    center -= cwest;
/*<       GO TO 60     >*/
    goto L60;

/*     WEST POINT IS DIRICHLET */

/*<    40 TEMP = CVALUS(1)*(HN+HS)*(HU+HD)/(4.D0*HW) >*/
L40:
    temp = cvalus[0] * (*hn + *hs) * (*hu + *hd) / (*hw * 4.);
/*<       CRHS = CRHS+BVALUS(5)*TEMP/BVALUS(1)       >*/
    crhs += bvalus[4] * temp / bvalus[0];
/*<       CENTER = CENTER+TEMP   >*/
    center += temp;
/*<       GO TO 60     >*/
    goto L60;

/*     CENTER POINT LIES ON WEST BOUNDARY WHICH IS MIXED. */

/*<    50 TEMP = -CVALUS(1)*(HN+HS)*(HU+HD)/(4.D0*BVALUS(2))   >*/
L50:
    temp = -cvalus[0] * (*hn + *hs) * (*hu + *hd) / (bvalus[1] * 4.);
/*<       CENTER = CENTER+BVALUS(1)*TEMP   >*/
    center += bvalus[0] * temp;
/*<       CRHS = CRHS+BVALUS(5)*TEMP       >*/
    crhs += bvalus[4] * temp;

/*     SET NORTH COEFFICIENTS */

/*<    60 CALL PDE2 (PCX,PCY+.5D0*HN,PCZ,CVALUS)     >*/
L60:
    d__1 = *pcy + *hn * .5;
    pde2_(pcx, &d__1, pcz, cvalus);
/*<       IF (JY.GE.NGRIDY-1) TEMP = BCOND2(4,PCX,BY,PCZ,BVALUS) >*/
    if (*jy >= tbk22_1.ngridy - 1) {
	temp = bcond2_(&c__4, pcx, &tbk21_1.by, pcz, bvalus);
    }
/*<       IF (JY.EQ.NGRIDY) GO TO 80       >*/
    if (*jy == tbk22_1.ngridy) {
	goto L80;
    }
/*<       IF ((JY.EQ.NGRIDY-1).AND.BCTYPE(4).EQ.1) GO TO 70    >*/
    if (*jy == tbk22_1.ngridy - 1 && tbk22_1.bctype[3] == 1) {
	goto L70;
    }

/*     NORMAL NORTH POINT */

/*<       CNORTH = -CVALUS(3)*(HE+HW)*(HU+HD)/(4.D0*HN)        >*/
    cnorth = -cvalus[2] * (*he + *hw) * (*hu + *hd) / (*hn * 4.);
/*<       CENTER = CENTER-CNORTH >*/
    center -= cnorth;
/*<       GO TO 90     >*/
    goto L90;

/*     NORTH POINT IS DIRICHLET */

/*<    70 TEMP = CVALUS(3)*(HE+HW)*(HU+HD)/(4.D0*HN) >*/
L70:
    temp = cvalus[2] * (*he + *hw) * (*hu + *hd) / (*hn * 4.);
/*<       CRHS = CRHS+BVALUS(5)*TEMP/BVALUS(1)       >*/
    crhs += bvalus[4] * temp / bvalus[0];
/*<       CENTER = CENTER+TEMP   >*/
    center += temp;
/*<       GO TO 90     >*/
    goto L90;

/*     CENTER POINT LIES ON NORTHERN MIXED BOUNDARY */

/*<    80 TEMP = CVALUS(3)*(HE+HW)*(HU+HD)/(4.D0*BVALUS(3))    >*/
L80:
    temp = cvalus[2] * (*he + *hw) * (*hu + *hd) / (bvalus[2] * 4.);
/*<       CENTER = CENTER+BVALUS(1)*TEMP   >*/
    center += bvalus[0] * temp;
/*<       CRHS = CRHS+BVALUS(5)*TEMP       >*/
    crhs += bvalus[4] * temp;

/*     SET SOUTH COEFFICIENTS */

/*<    90 CALL PDE2 (PCX,PCY-.5D0*HS,PCZ,CVALUS)     >*/
L90:
    d__1 = *pcy - *hs * .5;
    pde2_(pcx, &d__1, pcz, cvalus);
/*<       IF (JY.LE.2) TEMP = BCOND2(2,PCX,AY,PCZ,BVALUS)      >*/
    if (*jy <= 2) {
	temp = bcond2_(&c__2, pcx, &tbk21_1.ay, pcz, bvalus);
    }
/*<       IF (JY.EQ.1) GO TO 110 >*/
    if (*jy == 1) {
	goto L110;
    }
/*<       IF (JY.EQ.2.AND.BCTYPE(2).EQ.1) GO TO 100  >*/
    if (*jy == 2 && tbk22_1.bctype[1] == 1) {
	goto L100;
    }

/*     NORMAL SOUTH POINT */

/*<       CSOUTH = -CVALUS(3)*(HE+HW)*(HU+HD)/(4.D0*HS)        >*/
    csouth = -cvalus[2] * (*he + *hw) * (*hu + *hd) / (*hs * 4.);
/*<       CENTER = CENTER-CSOUTH >*/
    center -= csouth;
/*<       GO TO 120    >*/
    goto L120;

/*     SOUTH POINT IS DIRICHLET */

/*<   100 TEMP = CVALUS(3)*(HE+HW)*(HU+HD)/(4.D0*HS) >*/
L100:
    temp = cvalus[2] * (*he + *hw) * (*hu + *hd) / (*hs * 4.);
/*<       CRHS = CRHS+BVALUS(5)*TEMP/BVALUS(1)       >*/
    crhs += bvalus[4] * temp / bvalus[0];
/*<       CENTER = CENTER+TEMP   >*/
    center += temp;
/*<       GO TO 120    >*/
    goto L120;

/*     CENTER POINT LIES ON SOUTHERN MIXED BOUNDARY */

/*<   110 TEMP = -CVALUS(3)*(HE+HW)*(HU+HD)/(4.D0*BVALUS(3))   >*/
L110:
    temp = -cvalus[2] * (*he + *hw) * (*hu + *hd) / (bvalus[2] * 4.);
/*<       CENTER = CENTER+BVALUS(1)*TEMP   >*/
    center += bvalus[0] * temp;
/*<       CRHS = CRHS+BVALUS(5)*TEMP       >*/
    crhs += bvalus[4] * temp;

/*     SET UPPER COEFFICIENTS */

/*<   120 CALL PDE2 (PCX,PCY,PCZ+.5D0*HU,CVALUS)     >*/
L120:
    d__1 = *pcz + *hu * .5;
    pde2_(pcx, pcy, &d__1, cvalus);
/*<       IF (KZ.GE.NGRIDZ-1) TEMP = BCOND2(5,PCX,PCY,BZ,BVALUS) >*/
    if (*kz >= tbk22_1.ngridz - 1) {
	temp = bcond2_(&c__5, pcx, pcy, &tbk21_1.bz, bvalus);
    }
/*<       IF (KZ.EQ.NGRIDZ) GO TO 140      >*/
    if (*kz == tbk22_1.ngridz) {
	goto L140;
    }
/*<       IF ((KZ.EQ.NGRIDZ-1).AND.BCTYPE(5).EQ.1) GO TO 130   >*/
    if (*kz == tbk22_1.ngridz - 1 && tbk22_1.bctype[4] == 1) {
	goto L130;
    }

/*     NORMAL UPPER POINT */

/*<       CUP = -CVALUS(6)*(HE+HW)*(HN+HS)/(4.D0*HU) >*/
    cup = -cvalus[5] * (*he + *hw) * (*hn + *hs) / (*hu * 4.);
/*<       CENTER = CENTER-CUP    >*/
    center -= cup;
/*<       GO TO 150    >*/
    goto L150;

/*     UPPER POINT IS DIRICHLET */

/*<   130 TEMP = CVALUS(6)*(HE+HW)*(HN+HS)/(4.D0*HU) >*/
L130:
    temp = cvalus[5] * (*he + *hw) * (*hn + *hs) / (*hu * 4.);
/*<       CRHS = CRHS+BVALUS(5)*TEMP/BVALUS(1)       >*/
    crhs += bvalus[4] * temp / bvalus[0];
/*<       CENTER = CENTER+TEMP   >*/
    center += temp;
/*<       GO TO 150    >*/
    goto L150;

/*     CENTER POINT LIES ON UPPER  MIXED BOUNDARY */

/*<   140 TEMP = CVALUS(6)*(HE+HW)*(HN+HS)/(4.D0*BVALUS(4))    >*/
L140:
    temp = cvalus[5] * (*he + *hw) * (*hn + *hs) / (bvalus[3] * 4.);
/*<       CENTER = CENTER+BVALUS(1)*TEMP   >*/
    center += bvalus[0] * temp;
/*<       CRHS = CRHS+BVALUS(5)*TEMP       >*/
    crhs += bvalus[4] * temp;

/*     SET LOWER COEFFICIENTS */

/*<   150 CALL PDE2 (PCX,PCY,PCZ-.5D0*HD,CVALUS)     >*/
L150:
    d__1 = *pcz - *hd * .5;
    pde2_(pcx, pcy, &d__1, cvalus);
/*<       IF (KZ.LE.2) TEMP = BCOND2(6,PCX,PCY,AZ,BVALUS)      >*/
    if (*kz <= 2) {
	temp = bcond2_(&c__6, pcx, pcy, &tbk21_1.az, bvalus);
    }
/*<       IF (KZ.EQ.1) GO TO 170 >*/
    if (*kz == 1) {
	goto L170;
    }
/*<       IF (KZ.EQ.2.AND.BCTYPE(6).EQ.1) GO TO 160  >*/
    if (*kz == 2 && tbk22_1.bctype[5] == 1) {
	goto L160;
    }

/*     NORMAL LOWER POINT */

/*<       CLOWER = -CVALUS(6)*(HE+HW)*(HN+HS)/(4.D0*HD)        >*/
    clower = -cvalus[5] * (*he + *hw) * (*hn + *hs) / (*hd * 4.);
/*<       CENTER = CENTER-CLOWER >*/
    center -= clower;
/*<       GO TO 180    >*/
    goto L180;

/*     LOWER POINT IS DIRICHLET */

/*<   160 TEMP = CVALUS(6)*(HE+HW)*(HN+HS)/(4.D0*HD) >*/
L160:
    temp = cvalus[5] * (*he + *hw) * (*hn + *hs) / (*hd * 4.);
/*<       CRHS = CRHS+BVALUS(5)*TEMP/BVALUS(1)       >*/
    crhs += bvalus[4] * temp / bvalus[0];
/*<       CENTER = CENTER+TEMP   >*/
    center += temp;
/*<       GO TO 180    >*/
    goto L180;

/*     CENTER POINT LIES ON LOWER MIXED BOUNDARY */

/*<   170 TEMP = -CVALUS(6)*(HE+HW)*(HN+HS)/(4.D0*BVALUS(4))   >*/
L170:
    temp = -cvalus[5] * (*he + *hw) * (*hn + *hs) / (bvalus[3] * 4.);
/*<       CENTER = CENTER+BVALUS(1)*TEMP   >*/
    center += bvalus[0] * temp;
/*<       CRHS = CRHS+BVALUS(5)*TEMP       >*/
    crhs += bvalus[4] * temp;

/*     COEFFICIENT GENERATION IS COMPLETED. */
/*     NOW SET B (RHS), IA, JA, AND A (MATRIX REPRESENTATION) */

/*<   180 B = CRHS     >*/
L180:
    *b = crhs;

/*<       IF (ILEVEL.EQ.1) WRITE (NOUT,190) IJ,CRHS  >*/
    if (tbk22_1.ilevel == 1) {
	io___190.ciunit = tbk22_1.nout;
	s_wsfe(&io___190);
	do_fio(&c__1, (char *)&(*ij), (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&crhs, (ftnlen)sizeof(doublereal));
	e_wsfe();
    }
/*<    >*/

/*<       LXY = LNGTHX*LNGTHY    >*/
    lxy = *lngthx * *lngthy;
/*<       CALL STVAL2 (NELS,IA,JA,A,IJ,IJ,CENTER,IWKSP)        >*/
    stval2_(nels, &ia[1], &ja[1], &a[1], ij, ij, &center, &iwksp[1]);
/*<       CALL STVAL2 (NELS,IA,JA,A,IJ,IJ+1,CEAST,IWKSP)       >*/
    i__1 = *ij + 1;
    stval2_(nels, &ia[1], &ja[1], &a[1], ij, &i__1, &ceast, &iwksp[1]);
/*<       CALL STVAL2 (NELS,IA,JA,A,IJ,IJ-1,CWEST,IWKSP)       >*/
    i__1 = *ij - 1;
    stval2_(nels, &ia[1], &ja[1], &a[1], ij, &i__1, &cwest, &iwksp[1]);
/*<       CALL STVAL2 (NELS,IA,JA,A,IJ,IJ-LNGTHX,CSOUTH,IWKSP) >*/
    i__1 = *ij - *lngthx;
    stval2_(nels, &ia[1], &ja[1], &a[1], ij, &i__1, &csouth, &iwksp[1]);
/*<       CALL STVAL2 (NELS,IA,JA,A,IJ,IJ+LNGTHX,CNORTH,IWKSP) >*/
    i__1 = *ij + *lngthx;
    stval2_(nels, &ia[1], &ja[1], &a[1], ij, &i__1, &cnorth, &iwksp[1]);
/*<       CALL STVAL2 (NELS,IA,JA,A,IJ,IJ-LXY,CLOWER,IWKSP)    >*/
    i__1 = *ij - lxy;
    stval2_(nels, &ia[1], &ja[1], &a[1], ij, &i__1, &clower, &iwksp[1]);
/*<       CALL STVAL2 (NELS,IA,JA,A,IJ,IJ+LXY,CUP,IWKSP)       >*/
    i__1 = *ij + lxy;
    stval2_(nels, &ia[1], &ja[1], &a[1], ij, &i__1, &cup, &iwksp[1]);

/*<       RETURN       >*/
    return 0;
/*<       END  >*/
} /* pnt3d_ */

/*<       SUBROUTINE STVAL2 (NELS,IA,JA,A,I,J,VAL,IWKSP)       >*/
/* Subroutine */ int stval2_(integer *nels, integer *ia, integer *ja, 
	doublereal *a, integer *i__, integer *j, doublereal *val, integer *
	iwksp)
{
    /* Format strings */
    static char fmt_20[] = "(20x,\002COLUMN\002,i8,\002   VALUE =\002,d15.7)";

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();
    /* Subroutine */ int s_stop(char *, ftnlen);

    /* Local variables */
    integer ier;
    extern /* Subroutine */ int sbsij_(integer *, integer *, integer *, 
	    integer *, doublereal *, integer *, integer *, integer *, 
	    doublereal *, integer *, integer *, integer *, integer *);

    /* Fortran I/O blocks */
    static cilist io___192 = { 0, 0, 0, fmt_20, 0 };


/*<       DOUBLE PRECISION A(NELS),AX,AY,AZ,BX,BY,BZ,HX,HY,HZ,VAL,DRELPR >*/
/*<       INTEGER IA(1),JA(NELS),IWKSP(NELS),BCTYPE(6) >*/
/*<       COMMON /TBK21/ AX,AY,AZ,BX,BY,BZ,DRELPR,HX,HY,HZ     >*/
/*<       COMMON /TBK22/ BCTYPE,ILEVEL,ISYM,NOUT,NGRIDX,NGRIDY,NGRIDZ,N  >*/

/*<       IF (J.GT.N.OR.VAL.EQ.0.D0) RETURN  >*/
    /* Parameter adjustments */
    --iwksp;
    --a;
    --ja;
    --ia;

    /* Function Body */
    if (*j > tbk22_1.n || *val == 0.) {
	return 0;
    }
/*<       IF (ISYM.EQ.1) GO TO 10  >*/
    if (tbk22_1.isym == 1) {
	goto L10;
    }
/*<       IF (J.LT.I) RETURN     >*/
    if (*j < *i__) {
	return 0;
    }

/*<    10 IF (ILEVEL.EQ.1) WRITE (NOUT,20) J,VAL     >*/
L10:
    if (tbk22_1.ilevel == 1) {
	io___192.ciunit = tbk22_1.nout;
	s_wsfe(&io___192);
	do_fio(&c__1, (char *)&(*j), (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&(*val), (ftnlen)sizeof(doublereal));
	e_wsfe();
    }
/*<    20 FORMAT (20X,'COLUMN',I8,'   VALUE =',D15.7)  >*/

/*<       IER = 0      >*/
    ier = 0;
/*<       CALL SBSIJ (N,NELS,IA,JA,A,IWKSP,I,J,VAL,0,ILEVEL,NOUT,IER)    >*/
    sbsij_(&tbk22_1.n, nels, &ia[1], &ja[1], &a[1], &iwksp[1], i__, j, val, &
	    c__0, &tbk22_1.ilevel, &tbk22_1.nout, &ier);

/*<       IF (IER.GT.700) STOP   >*/
    if (ier > 700) {
	s_stop("", (ftnlen)0);
    }

/*<       RETURN       >*/
    return 0;
/*<       END  >*/
} /* stval2_ */

/*<       SUBROUTINE TEST3D (EPSI) >*/
/* Subroutine */ int test3d_(doublereal *epsi)
{
    /* Format strings */
    static char fmt_10[] = "(\0021\002//15x,\002ITPACK 2C  TEST PROGRAM -- T\
EST3 \002//)";
    static char fmt_20[] = "(//2x,70(\002*\002)/5x,\002ORDER OF SYSTEM      \
           =\002,i5/5x,\002PERCENTAGE OFFDIAGONAL NONZEROS =\002,i5/5x,\002I\
NITIAL NUMBER OF RED POINTS    =\002,i5/5x,\002NUMBER OF RED NONZERO ENTRIES\
   =\002,i5/)";
    static char fmt_30[] = "(\0020\002,14x,\002SYMMETRIC SYSTEM USING\002)";
    static char fmt_40[] = "(\0020\002,14x,\002NONSYMMETRIC SYSTEM USING\002)"
	    ;
    static char fmt_50[] = "(15x,\002NATURAL ORDERING\002)";
    static char fmt_60[] = "(15x,\002RED-BLACK ORDERING\002)";

    /* Builtin functions */
    integer s_wsfe(cilist *), e_wsfe(), do_fio(integer *, char *, ftnlen);

    /* Local variables */
    doublereal a[1600];
    integer p[40], ia[41], ja[1600], ip[40];
    doublereal rhs[40], row[40];
    integer loop, irow[40];
    doublereal wksp[500];
    integer iwksp[1600];
    extern /* Subroutine */ int setper_(integer *, integer *), tstprb_(
	    integer *, integer *, doublereal *, doublereal *, integer *, 
	    integer *, integer *, integer *), tstitp_(integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *, doublereal *)
	    , setsys_(integer *, integer *, doublereal *, doublereal *, 
	    integer *, integer *, doublereal *, integer *, integer *);

    /* Fortran I/O blocks */
    static cilist io___194 = { 0, 0, 0, fmt_10, 0 };
    static cilist io___197 = { 0, 0, 0, fmt_20, 0 };
    static cilist io___206 = { 0, 0, 0, fmt_30, 0 };
    static cilist io___207 = { 0, 0, 0, fmt_40, 0 };
    static cilist io___208 = { 0, 0, 0, fmt_50, 0 };
    static cilist io___210 = { 0, 0, 0, fmt_30, 0 };
    static cilist io___211 = { 0, 0, 0, fmt_40, 0 };
    static cilist io___212 = { 0, 0, 0, fmt_60, 0 };



/*     TEST3D EXERCISES ITPACK OVER SPARSE LINEAR SYSTEMS */
/*     OF ORDER AT MOST 40 WITH RANDOMLY GENERATED STRUCTURE. */
/*     (FOR DETAILS SEE SECTION 2 OF CENTER FOR NUMERICAL ANALYSIS */
/*     REPORT CNA-171 BY DAVID R. KINCAID: */
/*     ACCELERATION PARAMETERS FOR A SYMMETRIC SUCCESSIVE OVERRELAXATION */
/*     CONJUGATE GRADIENT METHOD FOR NONSYMMETRIC SYSTEMS, */
/*     CENTER FOR NUMERICAL ANALYSIS, UNIVERSITY OF TEXAS, AUSTIN, TX, */
/*     78712 - ALSO IN ADVANCES IN COMPUTER METHODS FOR PARTIAL */
/*     DIFFERENTIAL EQUATIONS, IV, IMACS, NEW BRUNSWICK, NJ, 1981.) */

/* ***************************************************************** */

/*     THE EXACT SIZE OF THE TEST PROBLEMS CAN BE INCREASED OR */
/*     DECREASED BY CHANGING THE ARRAY SIZE IN DIMENSION STATEMENTS */
/*     AND THE VARIABLES LISTED BELOW UNDER SIZE OF TEST PROBLEM. */

/*     ARRAY DIMENSIONING */

/*<       INTEGER IA(41),JA(1600),IROW(40),IWKSP(1600),P(40),IP(40)      >*/
/*<    >*/

/*     TEST3D COMMON BLOCK */

/*<       COMMON /TBK31/ DRELPR,ZETA       >*/
/*<    >*/

/*<       DRELPR = EPSI  >*/
    tbk31_1.drelpr = *epsi;

/*     SET ITPACK 2C SWITCHS */

/*     ZETA   -  STOPPING TEST CRITERION */
/*     LEVEL  -  LEVEL OF OUTPUT FROM ITPACK 2C */
/*     ITMAX  -  MAXIMUM NUMBER OF ITERATIONS ALLOWED */
/*     IERAN  -  ERROR ANALYSIS SWITCH */
/*     NOUT   - OUTPUT UNIT NUMBER */

/*<       ZETA = 1.D-5 >*/
    tbk31_1.zeta = 1e-5;
/*<       LEVEL = 1    >*/
    tbk32_1.level = 1;
/*<       ITMAX = 50   >*/
    tbk32_1.itmax = 50;
/*<       IERAN = 0    >*/
    tbk32_1.ieran = 0;
/*<       NOUT = 6     >*/
    tbk32_1.nout = 6;

/*     PROBLEM SIZE */

/*     MAXNZ  -  MAXIMUM NUMBER OF NONZEROS */
/*     MAXN   -  MAXIMUM SIZE OF SYSTEM */
/*     NW     -  SIZE OF DOUBLE PRECISION ARRAY WKSP */
/*     N      -  SIZE OF LINEAR SYSTEM */
/*     IPCT   -  PERCENTAGE OF OFFDIAGONAL NONZEROS */

/*<       MAXNZ = 1600 >*/
    tbk32_1.maxnz = 1600;
/*<       MAXN = 40    >*/
    tbk32_1.maxn = 40;
/*<       NW = 500     >*/
    tbk32_1.nw = 500;
/*<       N = 40       >*/
    tbk32_1.n = 40;
/*<       IPCT = 20    >*/
    tbk32_1.ipct = 20;

/*     SWITCHES FOR TESTING PROGRAM */

/*     ILEVEL  -  0/1 SWITCH FOR PRINT FROM TESTING PROGRAM */
/*     IDEBUG  -  0/1 SWITCH FOR DEBUG PRINTING FROM TESTING PROGRAM */
/*     IPLT    -  0/1 SWITCH FOR PRINTER PLOTTING OF NONZERO STRUCTURE */
/*     LARGE   -  0/1 SWITCH FOR TESTING ITPACK ROUTINE SBELM */
/*     ISEED   -  RANDUM NUMBER GENERATOR SEED */

/*<       ILEVEL = 0   >*/
    tbk32_1.ilevel = 0;
/*<       IDEBUG = 0   >*/
    tbk32_1.idebug = 0;
/*<       IPLT = 1     >*/
    tbk32_1.iplt = 1;
/*<       LARGE = 1    >*/
    tbk32_1.large = 1;
/*<       ISEED = 256  >*/
    tbk32_1.iseed = 256;

/*     OTHER VARIABLES THAT CONTROL NATURE OF TEST */

/*     NRED  -  NUMBER OF RED EQUATIONS */
/*     NBLACK - NUMBER OF BLACK EQUATIONS */
/*     NZRED  - NUMBER OF RED OFFDIAGONAL NONZERO ENTRIES */
/*     NZBLK  - NUMBER OF BLACK OFFDIAGONAL NONZERO ENTRIES */
/*     NRNB   - SIZE OF RED/BLACK BLOCK (NRED*NBLACK) */
/*     ISYM   - 0/1 SWITCH FOR SYMMETRIC/NONSYMMETRIC STORAGE */
/*     NB     - ORDER OF BLACK SUBSYSTEM */
/*     IORD   - 1/2 SWITCH FOR NATURAL/RED-BLACK ORDERING */

/*<       WRITE (NOUT,10)        >*/
    io___194.ciunit = tbk32_1.nout;
    s_wsfe(&io___194);
    e_wsfe();
/*<    10 FORMAT ('1'//15X,'ITPACK 2C  TEST PROGRAM -- TEST3 '//)        >*/

/*     GENERATE RANDOM NUMBERS FOR N, NRED, NZRED, P */

/*<       CALL SETPER (P,IP)     >*/
    setper_(p, ip);

/*<       WRITE (NOUT,20) N,IPCT,NRED,NZRED  >*/
    io___197.ciunit = tbk32_1.nout;
    s_wsfe(&io___197);
    do_fio(&c__1, (char *)&tbk32_1.n, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&tbk32_1.ipct, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&tbk32_1.nred, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&tbk32_1.nzred, (ftnlen)sizeof(integer));
    e_wsfe();
/*<    >*/

/* ...... LOOP FOR SYMMETRIC AND NONSYMMETRIC SYSTEMS OF SAME SIZE */

/*<       DO 70 LOOP = 1,2       >*/
    for (loop = 1; loop <= 2; ++loop) {
/*<          ISYM = LOOP-1       >*/
	tbk32_1.isym = loop - 1;

/* ...... GENERATE RANDOM SPARSE SYSTEM */

/*<          CALL SETSYS (IA,JA,A,RHS,P,IP,ROW,IROW,IWKSP)     >*/
	setsys_(ia, ja, a, rhs, p, ip, row, irow, iwksp);

/*<          IF (ISYM.EQ.0) WRITE (NOUT,30)  >*/
	if (tbk32_1.isym == 0) {
	    io___206.ciunit = tbk32_1.nout;
	    s_wsfe(&io___206);
	    e_wsfe();
	}
/*<          IF (ISYM.NE.0) WRITE (NOUT,40)  >*/
	if (tbk32_1.isym != 0) {
	    io___207.ciunit = tbk32_1.nout;
	    s_wsfe(&io___207);
	    e_wsfe();
	}
/*<          WRITE (NOUT,50)     >*/
	io___208.ciunit = tbk32_1.nout;
	s_wsfe(&io___208);
	e_wsfe();
/*<    30    FORMAT ('0',14X,'SYMMETRIC SYSTEM USING') >*/
/*<    40    FORMAT ('0',14X,'NONSYMMETRIC SYSTEM USING')      >*/
/*<    50    FORMAT (15X,'NATURAL ORDERING') >*/
/*<    60    FORMAT (15X,'RED-BLACK ORDERING')       >*/
/* L60: */

/* ...... TEST ITPACK 2C ROUTINES: NATURAL ORDERING */

/*<          IORD = 1  >*/
	tbk32_1.iord = 1;
/*<          CALL TSTITP (IA,JA,A,RHS,ROW,IWKSP,WKSP)  >*/
	tstitp_(ia, ja, a, rhs, row, iwksp, wksp);

/* ...... TEST RED-BLACK SYSTEM */

/*<          CALL TSTPRB (IA,JA,A,RHS,P,IP,IROW,IWKSP) >*/
	tstprb_(ia, ja, a, rhs, p, ip, irow, iwksp);

/*<          IF (ISYM.EQ.0) WRITE (NOUT,30)  >*/
	if (tbk32_1.isym == 0) {
	    io___210.ciunit = tbk32_1.nout;
	    s_wsfe(&io___210);
	    e_wsfe();
	}
/*<          IF (ISYM.NE.0) WRITE (NOUT,40)  >*/
	if (tbk32_1.isym != 0) {
	    io___211.ciunit = tbk32_1.nout;
	    s_wsfe(&io___211);
	    e_wsfe();
	}
/*<          WRITE (NOUT,60)     >*/
	io___212.ciunit = tbk32_1.nout;
	s_wsfe(&io___212);
	e_wsfe();

/* ...... TEST ITPACK 2C ROUTINES: RED-BLACK ORDERING */

/*<          IORD = 2  >*/
	tbk32_1.iord = 2;
/*<          CALL TSTITP (IA,JA,A,RHS,ROW,IWKSP,WKSP)  >*/
	tstitp_(ia, ja, a, rhs, row, iwksp, wksp);

/*<    70 CONTINUE     >*/
/* L70: */
    }

/*<       RETURN       >*/
    return 0;
/*<       END  >*/
} /* test3d_ */

/*<       SUBROUTINE SETPER (P,IP) >*/
/* Subroutine */ int setper_(integer *p, integer *ip)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    integer i__, nrnb;
    extern integer irand_(integer *, integer *, integer *);


/*       GENERATE RANDOM NUMBERS FOR */
/*           ORDER OF SYSTEM, RED POINTS, NONZERO POINTS, PERMUTATION */

/*<       INTEGER P(1),IP(1)     >*/
/*<       DOUBLE PRECISION DRELPR,ZETA     >*/

/*<       COMMON /TBK31/ DRELPR,ZETA       >*/
/*<    >*/

/*<       NRED = N/2   >*/
    /* Parameter adjustments */
    --ip;
    --p;

    /* Function Body */
    tbk32_1.nred = tbk32_1.n / 2;
/*<       NBLACK = N-NRED        >*/
    tbk32_1.nblack = tbk32_1.n - tbk32_1.nred;
/*<       NRNB = NRED*NBLACK     >*/
    nrnb = tbk32_1.nred * tbk32_1.nblack;

/*       COMPUTE NUMBER OF NONZEROS RED ENTRIES */

/*<       NZRED = IFIX(SNGL(DBLE(FLOAT(NRNB))*DBLE(FLOAT(IPCT))*1.0D-2)) >*/
    tbk32_1.nzred = (integer) ((real) ((doublereal) ((real) nrnb) * (
	    doublereal) ((real) tbk32_1.ipct) * .01));
/*<       NZBLK = NZRED  >*/
    tbk32_1.nzblk = tbk32_1.nzred;

/*       GENERATE RANDOM PERMUTATION */

/*<       DO 10 I = 1,N  >*/
    i__1 = tbk32_1.n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<          P(I) = I  >*/
	p[i__] = i__;
/*<          IP(I) = IRAND(1,N,ISEED)      >*/
	ip[i__] = irand_(&c__1, &tbk32_1.n, &tbk32_1.iseed);
/*<    10 CONTINUE     >*/
/* L10: */
    }

/*<       RETURN       >*/
    return 0;
/*<       END  >*/
} /* setper_ */

/*<       SUBROUTINE SETSYS (IA,JA,A,RHS,P,IP,ROW,IROW,IWKSP)  >*/
/* Subroutine */ int setsys_(integer *ia, integer *ja, doublereal *a, 
	doublereal *rhs, integer *p, integer *ip, doublereal *row, integer *
	irow, integer *iwksp)
{
    /* Format strings */
    static char fmt_30[] = "(/2x,\002QSORT ERROR, IER =\002,i5)";
    static char fmt_60[] = "(/2x,20(\002+\002),\002SYMMETRIC STORAGE FORMA\
T\002,20(\002+\002))";
    static char fmt_80[] = "(/2x,\002 NONZERO ENTRY ( \002,i10,\002 , \002,i\
10,\002 ) SET = \002,d15.8)";
    static char fmt_110[] = "(/2x,20(\002+\002),\002NONSYMMETRIC STORAGE FOR\
MAT\002,20(\002+\002))";
    static char fmt_220[] = "(\0021\002/2x,\002PATTERN OF NONZEROS WITH NATU\
RAL ORDERING    \002/)";
    static char fmt_240[] = "(//2x,\002DEBUG PRINTING\002,2x,\002P ARRAY  BE\
FORE RED-BLACK INDEXING\002)";
    static char fmt_250[] = "(2x,10i8)";

    /* System generated locals */
    integer i__1, i__2, i__3;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();
    /* Subroutine */ int s_stop(char *, ftnlen);

    /* Local variables */
    integer i__, j, k, pi, pj, ier;
    doublereal val, tol;
    integer nbig;
    doublereal eval;
    extern /* Subroutine */ int sbend_(integer *, integer *, integer *, 
	    integer *, doublereal *, integer *), sbagn_(integer *, integer *, 
	    integer *, integer *, doublereal *, integer *, integer *, integer 
	    *, integer *);
    extern integer irand_(integer *, integer *, integer *);
    extern /* Subroutine */ int sbelm_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, integer *, doublereal *, doublereal *,
	     integer *, integer *, integer *, integer *), sbini_(integer *, 
	    integer *, integer *, integer *, doublereal *, integer *), sbsij_(
	    integer *, integer *, integer *, integer *, doublereal *, integer 
	    *, integer *, integer *, doublereal *, integer *, integer *, 
	    integer *, integer *), qsort_(integer *, integer *, doublereal *, 
	    integer *), pltadj_(integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *);
    extern doublereal random_(integer *);
    extern /* Subroutine */ int sysout_(integer *, integer *, integer *, 
	    integer *, integer *, doublereal *, doublereal *, integer *, 
	    integer *);

    /* Fortran I/O blocks */
    static cilist io___217 = { 0, 0, 0, fmt_30, 0 };
    static cilist io___219 = { 0, 0, 0, fmt_60, 0 };
    static cilist io___224 = { 0, 0, 0, fmt_80, 0 };
    static cilist io___225 = { 0, 0, 0, fmt_110, 0 };
    static cilist io___226 = { 0, 0, 0, fmt_80, 0 };
    static cilist io___227 = { 0, 0, 0, fmt_80, 0 };
    static cilist io___228 = { 0, 0, 0, fmt_80, 0 };
    static cilist io___231 = { 0, 0, 0, fmt_80, 0 };
    static cilist io___233 = { 0, 0, 0, fmt_220, 0 };
    static cilist io___234 = { 0, 0, 0, fmt_240, 0 };
    static cilist io___235 = { 0, 0, 0, fmt_250, 0 };



/*     GENERATE SPARSE SYSTEM WITH RANDOM STRUCTURE */

/*<       INTEGER IA(1),JA(1),P(1),IP(1),IWKSP(1),IROW(1),PI,PJ  >*/
/*<       DOUBLE PRECISION A(1),RHS(1),ROW(1),TOL,ZETA,EVAL,DRELPR,VAL   >*/

/*<       COMMON /TBK31/ DRELPR,ZETA       >*/
/*<    >*/

/*<       DO 10 I = 1,N  >*/
    /* Parameter adjustments */
    --iwksp;
    --irow;
    --row;
    --ip;
    --p;
    --rhs;
    --a;
    --ja;
    --ia;

    /* Function Body */
    i__1 = tbk32_1.n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<          ROW(I) = DBLE(FLOAT(P(I)))    >*/
	row[i__] = (doublereal) ((real) p[i__]);
/*<    10 CONTINUE     >*/
/* L10: */
    }
/*<       CALL QSORT (N,IP,ROW,IER)        >*/
    qsort_(&tbk32_1.n, &ip[1], &row[1], &ier);
/*<       DO 20 I = 1,N  >*/
    i__1 = tbk32_1.n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<          P(I) = IFIX(SNGL(ROW(I)))     >*/
	p[i__] = (integer) ((real) row[i__]);
/*<    20 CONTINUE     >*/
/* L20: */
    }
/*<       IF (IER.EQ.0) GO TO 40 >*/
    if (ier == 0) {
	goto L40;
    }

/*<       WRITE (NOUT,30) IER    >*/
    io___217.ciunit = tbk32_1.nout;
    s_wsfe(&io___217);
    do_fio(&c__1, (char *)&ier, (ftnlen)sizeof(integer));
    e_wsfe();
/*<    30 FORMAT (/2X,'QSORT ERROR, IER =',I5)       >*/
/*<       STOP >*/
    s_stop("", (ftnlen)0);

/*<    40 CALL SBINI (N,MAXNZ,IA,JA,A,IWKSP) >*/
L40:
    sbini_(&tbk32_1.n, &tbk32_1.maxnz, &ia[1], &ja[1], &a[1], &iwksp[1]);

/*       ZERO OUT THINGS , SET RHS TO 1.0 */

/*<       DO 50 K = 1,N  >*/
    i__1 = tbk32_1.n;
    for (k = 1; k <= i__1; ++k) {
/*<          RHS(K) = 1.D0       >*/
	rhs[k] = 1.;
/*<          ROW(K) = 0.D0       >*/
	row[k] = 0.;
/*<    50 CONTINUE     >*/
/* L50: */
    }

/*<       IF (ISYM.NE.0) GO TO 100 >*/
    if (tbk32_1.isym != 0) {
	goto L100;
    }

/* ******************* SYMMETRIC STORAGE CASE ********************** */

/*<       WRITE (NOUT,60)        >*/
    io___219.ciunit = tbk32_1.nout;
    s_wsfe(&io___219);
    e_wsfe();
/*<    60 FORMAT (/2X,20('+'),'SYMMETRIC STORAGE FORMAT',20('+'))        >*/

/*       STORE SYMMETRIC SYSTEM  OFF-DIAGONAL ELEMENTS */

/*<       DO 90 K = 1,NZRED      >*/
    i__1 = tbk32_1.nzred;
    for (k = 1; k <= i__1; ++k) {
/*<    70    I = IRAND(1,NRED,ISEED)       >*/
L70:
	i__ = irand_(&c__1, &tbk32_1.nred, &tbk32_1.iseed);
/*<          J = IRAND(NRED+1,N,ISEED)     >*/
	i__2 = tbk32_1.nred + 1;
	j = irand_(&i__2, &tbk32_1.n, &tbk32_1.iseed);
/*<          PI = MIN0(P(I),P(J))  >*/
/* Computing MIN */
	i__2 = p[i__], i__3 = p[j];
	pi = min(i__2,i__3);
/*<          PJ = MAX0(P(I),P(J))  >*/
/* Computing MAX */
	i__2 = p[i__], i__3 = p[j];
	pj = max(i__2,i__3);
/*<          VAL = -DBLE(RANDOM(ISEED))    >*/
	val = -((doublereal) random_(&tbk32_1.iseed));
/*<          IF (IDEBUG.NE.0) WRITE (NOUT,80) PI,PJ,VAL        >*/
	if (tbk32_1.idebug != 0) {
	    io___224.ciunit = tbk32_1.nout;
	    s_wsfe(&io___224);
	    do_fio(&c__1, (char *)&pi, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&pj, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&val, (ftnlen)sizeof(doublereal));
	    e_wsfe();
	}
/*<    >*/
/*<          CALL SBSIJ (N,MAXNZ,IA,JA,A,IWKSP,PI,PJ,VAL,-1,ILEVEL,NOUT,IER) >*/
	sbsij_(&tbk32_1.n, &tbk32_1.maxnz, &ia[1], &ja[1], &a[1], &iwksp[1], &
		pi, &pj, &val, &c_n1, &tbk32_1.ilevel, &tbk32_1.nout, &ier);
/*<          IF (IER.EQ.700) GO TO 70      >*/
	if (ier == 700) {
	    goto L70;
	}
/*<          IF (IER.GT.700) STOP  >*/
	if (ier > 700) {
	    s_stop("", (ftnlen)0);
	}
/*<          ROW(PI) = ROW(PI)-VAL >*/
	row[pi] -= val;
/*<          ROW(PJ) = ROW(PJ)-VAL >*/
	row[pj] -= val;
/*<    90 CONTINUE     >*/
/* L90: */
    }
/*<       GO TO 160    >*/
    goto L160;

/* ******************* NONSYMMETRIC STORAGE CASE ****************** */

/*<   100 WRITE (NOUT,110)       >*/
L100:
    io___225.ciunit = tbk32_1.nout;
    s_wsfe(&io___225);
    e_wsfe();
/*<   110 FORMAT (/2X,20('+'),'NONSYMMETRIC STORAGE FORMAT',20('+'))     >*/

/*       STORE NONSYMMETRIC SYSTEM  OFF-DIAGONAL ELEMENTS */

/*<       DO 130 K = 1,NZRED     >*/
    i__1 = tbk32_1.nzred;
    for (k = 1; k <= i__1; ++k) {
/*<   120    I = IRAND(1,NRED,ISEED)       >*/
L120:
	i__ = irand_(&c__1, &tbk32_1.nred, &tbk32_1.iseed);
/*<          J = IRAND(NRED+1,N,ISEED)     >*/
	i__2 = tbk32_1.nred + 1;
	j = irand_(&i__2, &tbk32_1.n, &tbk32_1.iseed);
/*<          PI = P(I) >*/
	pi = p[i__];
/*<          PJ = P(J) >*/
	pj = p[j];
/*<          VAL = -DBLE(RANDOM(ISEED))    >*/
	val = -((doublereal) random_(&tbk32_1.iseed));
/*<          IF (IDEBUG.NE.0) WRITE (NOUT,80) PI,PJ,VAL        >*/
	if (tbk32_1.idebug != 0) {
	    io___226.ciunit = tbk32_1.nout;
	    s_wsfe(&io___226);
	    do_fio(&c__1, (char *)&pi, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&pj, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&val, (ftnlen)sizeof(doublereal));
	    e_wsfe();
	}
/*<          CALL SBSIJ (N,MAXNZ,IA,JA,A,IWKSP,PI,PJ,VAL,-1,ILEVEL,NOUT,IER) >*/
	sbsij_(&tbk32_1.n, &tbk32_1.maxnz, &ia[1], &ja[1], &a[1], &iwksp[1], &
		pi, &pj, &val, &c_n1, &tbk32_1.ilevel, &tbk32_1.nout, &ier);
/*<          IF (IER.EQ.700) GO TO 120     >*/
	if (ier == 700) {
	    goto L120;
	}
/*<          IF (IER.GT.700) STOP  >*/
	if (ier > 700) {
	    s_stop("", (ftnlen)0);
	}
/*<          ROW(PI) = ROW(PI)-VAL >*/
	row[pi] -= val;
/*<   130 CONTINUE     >*/
/* L130: */
    }
/*<       DO 150 K = 1,NZBLK     >*/
    i__1 = tbk32_1.nzblk;
    for (k = 1; k <= i__1; ++k) {
/*<   140    I = IRAND(NRED+1,N,ISEED)     >*/
L140:
	i__2 = tbk32_1.nred + 1;
	i__ = irand_(&i__2, &tbk32_1.n, &tbk32_1.iseed);
/*<          J = IRAND(1,NRED,ISEED)       >*/
	j = irand_(&c__1, &tbk32_1.nred, &tbk32_1.iseed);
/*<          PI = P(I) >*/
	pi = p[i__];
/*<          PJ = P(J) >*/
	pj = p[j];
/*<          VAL = -DBLE(RANDOM(ISEED))    >*/
	val = -((doublereal) random_(&tbk32_1.iseed));
/*<          IF (IDEBUG.NE.0) WRITE (NOUT,80) PI,PJ,VAL        >*/
	if (tbk32_1.idebug != 0) {
	    io___227.ciunit = tbk32_1.nout;
	    s_wsfe(&io___227);
	    do_fio(&c__1, (char *)&pi, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&pj, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&val, (ftnlen)sizeof(doublereal));
	    e_wsfe();
	}
/*<          CALL SBSIJ (N,MAXNZ,IA,JA,A,IWKSP,PI,PJ,VAL,-1,ILEVEL,NOUT,IER) >*/
	sbsij_(&tbk32_1.n, &tbk32_1.maxnz, &ia[1], &ja[1], &a[1], &iwksp[1], &
		pi, &pj, &val, &c_n1, &tbk32_1.ilevel, &tbk32_1.nout, &ier);
/*<          IF (IER.EQ.700) GO TO 140     >*/
	if (ier == 700) {
	    goto L140;
	}
/*<          IF (IER.GT.700) STOP  >*/
	if (ier > 700) {
	    s_stop("", (ftnlen)0);
	}
/*<          ROW(PI) = ROW(PI)-VAL >*/
	row[pi] -= val;
/*<   150 CONTINUE     >*/
/* L150: */
    }

/* **************************************************************** */

/*       SET DIAGONAL */

/*<   160 CONTINUE     >*/
L160:
/*<       DO 170 K = 1,N >*/
    i__1 = tbk32_1.n;
    for (k = 1; k <= i__1; ++k) {
/*<          VAL = ROW(K)+1.D0   >*/
	val = row[k] + 1.;
/*<          PI = K    >*/
	pi = k;
/*<          PJ = K    >*/
	pj = k;
/*<          IF (IDEBUG.NE.0) WRITE (NOUT,80) PI,PJ,VAL        >*/
	if (tbk32_1.idebug != 0) {
	    io___228.ciunit = tbk32_1.nout;
	    s_wsfe(&io___228);
	    do_fio(&c__1, (char *)&pi, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&pj, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&val, (ftnlen)sizeof(doublereal));
	    e_wsfe();
	}
/*<          CALL SBSIJ (N,MAXNZ,IA,JA,A,IWKSP,PI,PJ,VAL,0,ILEVEL,NOUT,IER)  >*/
	sbsij_(&tbk32_1.n, &tbk32_1.maxnz, &ia[1], &ja[1], &a[1], &iwksp[1], &
		pi, &pj, &val, &c__0, &tbk32_1.ilevel, &tbk32_1.nout, &ier);
/*<          IF (IER.GT.700) STOP  >*/
	if (ier > 700) {
	    s_stop("", (ftnlen)0);
	}
/*<   170 CONTINUE     >*/
/* L170: */
    }
/*<       CALL SBEND (N,MAXNZ,IA,JA,A,IWKSP) >*/
    sbend_(&tbk32_1.n, &tbk32_1.maxnz, &ia[1], &ja[1], &a[1], &iwksp[1]);
/*<       IF (IDEBUG.EQ.0) GO TO 180       >*/
    if (tbk32_1.idebug == 0) {
	goto L180;
    }
/*<       CALL SYSOUT (N,NZRED,NZBLK,IA,JA,A,RHS,NOUT,ISYM)    >*/
    sysout_(&tbk32_1.n, &tbk32_1.nzred, &tbk32_1.nzblk, &ia[1], &ja[1], &a[1],
	     &rhs[1], &tbk32_1.nout, &tbk32_1.isym);
/*<   180 CONTINUE     >*/
L180:

/*     SET SOME DIAGONAL ELEMENTS VERY LARGE */

/*<       IF (LARGE.EQ.0) GO TO 200        >*/
    if (tbk32_1.large == 0) {
	goto L200;
    }
/*<       EVAL = 1.D8  >*/
    eval = 1e8;
/*<       CALL SBAGN (N,MAXNZ,IA,JA,A,IWKSP,ILEVEL,NOUT,IER)   >*/
    sbagn_(&tbk32_1.n, &tbk32_1.maxnz, &ia[1], &ja[1], &a[1], &iwksp[1], &
	    tbk32_1.ilevel, &tbk32_1.nout, &ier);
/*<       IF (IER.NE.0) STOP     >*/
    if (ier != 0) {
	s_stop("", (ftnlen)0);
    }

/*<       DO 190 K = 1,N,5       >*/
    i__1 = tbk32_1.n;
    for (k = 1; k <= i__1; k += 5) {
/*<          NBIG = IRAND(1,N,ISEED)       >*/
	nbig = irand_(&c__1, &tbk32_1.n, &tbk32_1.iseed);
/*<          RHS(NBIG) = EVAL+RHS(NBIG)    >*/
	rhs[nbig] = eval + rhs[nbig];
/*<          PI = NBIG >*/
	pi = nbig;
/*<          PJ = NBIG >*/
	pj = nbig;
/*<          IF (IDEBUG.NE.0) WRITE (NOUT,80) PI,PJ,VAL        >*/
	if (tbk32_1.idebug != 0) {
	    io___231.ciunit = tbk32_1.nout;
	    s_wsfe(&io___231);
	    do_fio(&c__1, (char *)&pi, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&pj, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&val, (ftnlen)sizeof(doublereal));
	    e_wsfe();
	}
/*<          CALL SBSIJ (N,MAXNZ,IA,JA,A,IWKSP,PI,PJ,EVAL,1,ILEVEL,NOUT,IER) >*/
	sbsij_(&tbk32_1.n, &tbk32_1.maxnz, &ia[1], &ja[1], &a[1], &iwksp[1], &
		pi, &pj, &eval, &c__1, &tbk32_1.ilevel, &tbk32_1.nout, &ier);
/*<          IF (IER.GT.700) STOP  >*/
	if (ier > 700) {
	    s_stop("", (ftnlen)0);
	}
/*<   190 CONTINUE     >*/
/* L190: */
    }
/*<       CALL SBEND (N,MAXNZ,IA,JA,A,IWKSP) >*/
    sbend_(&tbk32_1.n, &tbk32_1.maxnz, &ia[1], &ja[1], &a[1], &iwksp[1]);

/*<       IF (IDEBUG.EQ.0) GO TO 200       >*/
    if (tbk32_1.idebug == 0) {
	goto L200;
    }
/*<       CALL SYSOUT (N,NZRED,NZBLK,IA,JA,A,RHS,NOUT,ISYM)    >*/
    sysout_(&tbk32_1.n, &tbk32_1.nzred, &tbk32_1.nzblk, &ia[1], &ja[1], &a[1],
	     &rhs[1], &tbk32_1.nout, &tbk32_1.isym);

/* ...... TEST ROUTINE TO REMOVE ROWS AND COLUMNS */

/*<   200 CONTINUE     >*/
L200:
/*<       TOL = 1.D-8  >*/
    tol = 1e-8;
/*<       CALL SBELM (N,IA,JA,A,RHS,IROW,ROW,TOL,ISYM,ILEVEL,NOUT,IER)   >*/
    sbelm_(&tbk32_1.n, &ia[1], &ja[1], &a[1], &rhs[1], &irow[1], &row[1], &
	    tol, &tbk32_1.isym, &tbk32_1.ilevel, &tbk32_1.nout, &ier);
/*<       IF (IER.NE.0) STOP     >*/
    if (ier != 0) {
	s_stop("", (ftnlen)0);
    }
/*<       IF (IDEBUG.EQ.0) GO TO 210       >*/
    if (tbk32_1.idebug == 0) {
	goto L210;
    }
/*<       CALL SYSOUT (N,NZRED,NZBLK,IA,JA,A,RHS,NOUT,ISYM)    >*/
    sysout_(&tbk32_1.n, &tbk32_1.nzred, &tbk32_1.nzblk, &ia[1], &ja[1], &a[1],
	     &rhs[1], &tbk32_1.nout, &tbk32_1.isym);
/*<   210 CONTINUE     >*/
L210:

/* ...... SYSTEM SET-UP */

/*<       IF (IPLT.EQ.0) GO TO 230 >*/
    if (tbk32_1.iplt == 0) {
	goto L230;
    }
/*<       WRITE (NOUT,220)       >*/
    io___233.ciunit = tbk32_1.nout;
    s_wsfe(&io___233);
    e_wsfe();
/*<   220 FORMAT ('1'/2X,'PATTERN OF NONZEROS WITH NATURAL ORDERING    '/) >*/
/*<       CALL PLTADJ (N,IA,JA,ISYM,IROW,IWKSP,NOUT) >*/
    pltadj_(&tbk32_1.n, &ia[1], &ja[1], &tbk32_1.isym, &irow[1], &iwksp[1], &
	    tbk32_1.nout);
/*<   230 IF (IDEBUG.EQ.0) RETURN  >*/
L230:
    if (tbk32_1.idebug == 0) {
	return 0;
    }

/*       DEBUGGING PRINT OUT */

/*<       WRITE (NOUT,240)       >*/
    io___234.ciunit = tbk32_1.nout;
    s_wsfe(&io___234);
    e_wsfe();
/*<    >*/
/*<       WRITE (NOUT,250) (P(I),I=1,N)    >*/
    io___235.ciunit = tbk32_1.nout;
    s_wsfe(&io___235);
    i__1 = tbk32_1.n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	do_fio(&c__1, (char *)&p[i__], (ftnlen)sizeof(integer));
    }
    e_wsfe();
/*<   250 FORMAT (2X,10I8)       >*/
/*<       CALL SYSOUT (N,NZRED,NZBLK,IA,JA,A,RHS,NOUT,ISYM)    >*/
    sysout_(&tbk32_1.n, &tbk32_1.nzred, &tbk32_1.nzblk, &ia[1], &ja[1], &a[1],
	     &rhs[1], &tbk32_1.nout, &tbk32_1.isym);
/*<       RETURN       >*/
    return 0;
/*<       END  >*/
} /* setsys_ */

/*<       SUBROUTINE TSTPRB (IA,JA,A,RHS,P,IP,IROW,ICOL)       >*/
/* Subroutine */ int tstprb_(integer *ia, integer *ja, doublereal *a, 
	doublereal *rhs, integer *p, integer *ip, integer *irow, integer *
	icol)
{
    /* Format strings */
    static char fmt_10[] = "(//5x,\002COMPUTED NUMBER OF RED POINTS   =\002,\
i5/5x,\002IER                             =\002,i5/5x,\002ELAPSED TIME FOR I\
NDEXING       =\002,f5.3/)";
    static char fmt_20[] = "(\0021\002/2x,\002PATTERN OF NONZEROS WITH RED-B\
LACK ORDERING  \002/)";
    static char fmt_40[] = "(//2x,\002DEBUG PRINTING\002,2x,\002P ARRAY  AFT\
ER RED-BLACK INDEXING\002)";
    static char fmt_50[] = "(2x,10i8)";

    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();
    /* Subroutine */ int s_stop(char *, ftnlen);

    /* Local variables */
    integer i__, nr, ier;
    doublereal time;
    real timi1, timi2;
    extern doublereal timer_(real *);
    extern /* Subroutine */ int pltadj_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *), pervec_(integer *, 
	    doublereal *, integer *), permat_(integer *, integer *, integer *,
	     doublereal *, integer *, integer *, integer *, integer *, 
	    integer *, integer *), prbndx_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *),
	     sysout_(integer *, integer *, integer *, integer *, integer *, 
	    doublereal *, doublereal *, integer *, integer *);

    /* Fortran I/O blocks */
    static cilist io___241 = { 0, 0, 0, fmt_10, 0 };
    static cilist io___242 = { 0, 0, 0, fmt_20, 0 };
    static cilist io___243 = { 0, 0, 0, fmt_40, 0 };
    static cilist io___244 = { 0, 0, 0, fmt_50, 0 };



/*     TEST RED-BLACK INDEXING FOR SYSTEMS */

/*<       INTEGER IA(1),JA(1),P(1),IP(1),IROW(1),ICOL(1)       >*/
/*<       DOUBLE PRECISION A(1),RHS(1),DRELPR,ZETA,TIME        >*/

/*<       COMMON /TBK31/ DRELPR,ZETA       >*/
/*<    >*/

/*<       TIMI1 = TIMER(0.0)     >*/
    /* Parameter adjustments */
    --icol;
    --irow;
    --ip;
    --p;
    --rhs;
    --a;
    --ja;
    --ia;

    /* Function Body */
    timi1 = timer_(&c_b28);

/*<       CALL PRBNDX (N,NB,IA,JA,P,IP,ILEVEL,NOUT,IER)        >*/
    prbndx_(&tbk32_1.n, &tbk32_1.nb, &ia[1], &ja[1], &p[1], &ip[1], &
	    tbk32_1.ilevel, &tbk32_1.nout, &ier);

/*<       TIMI2 = TIMER(0.0)     >*/
    timi2 = timer_(&c_b28);
/*<       TIME = DBLE(TIMI2-TIMI1) >*/
    time = (doublereal) (timi2 - timi1);

/*       PRINT RESULTS */

/*<       NR = N-NB    >*/
    nr = tbk32_1.n - tbk32_1.nb;
/*<       WRITE (NOUT,10) NR,IER,TIME      >*/
    io___241.ciunit = tbk32_1.nout;
    s_wsfe(&io___241);
    do_fio(&c__1, (char *)&nr, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&ier, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&time, (ftnlen)sizeof(doublereal));
    e_wsfe();
/*<    >*/

/*       VERIFY RED-BLACK ORDERING */

/*<       CALL PERMAT (N,IA,JA,A,P,ICOL,ISYM,ILEVEL,NOUT,IER)  >*/
    permat_(&tbk32_1.n, &ia[1], &ja[1], &a[1], &p[1], &icol[1], &tbk32_1.isym,
	     &tbk32_1.ilevel, &tbk32_1.nout, &ier);
/*<       CALL PERVEC (N,RHS,P)  >*/
    pervec_(&tbk32_1.n, &rhs[1], &p[1]);
/*<       IF (IER.NE.0) STOP     >*/
    if (ier != 0) {
	s_stop("", (ftnlen)0);
    }

/*<       IF (IPLT.EQ.0) GO TO 30  >*/
    if (tbk32_1.iplt == 0) {
	goto L30;
    }
/*<       WRITE (NOUT,20)        >*/
    io___242.ciunit = tbk32_1.nout;
    s_wsfe(&io___242);
    e_wsfe();
/*<    20 FORMAT ('1'/2X,'PATTERN OF NONZEROS WITH RED-BLACK ORDERING  '/) >*/
/*<       CALL PLTADJ (N,IA,JA,ISYM,IROW,ICOL,NOUT)  >*/
    pltadj_(&tbk32_1.n, &ia[1], &ja[1], &tbk32_1.isym, &irow[1], &icol[1], &
	    tbk32_1.nout);
/*<    30 IF (IDEBUG.EQ.0) GO TO 60        >*/
L30:
    if (tbk32_1.idebug == 0) {
	goto L60;
    }
/*<       WRITE (NOUT,40)        >*/
    io___243.ciunit = tbk32_1.nout;
    s_wsfe(&io___243);
    e_wsfe();
/*<    >*/
/*<       WRITE (NOUT,50) (P(I),I=1,N)     >*/
    io___244.ciunit = tbk32_1.nout;
    s_wsfe(&io___244);
    i__1 = tbk32_1.n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	do_fio(&c__1, (char *)&p[i__], (ftnlen)sizeof(integer));
    }
    e_wsfe();
/*<    50 FORMAT (2X,10I8)       >*/
/*<       CALL SYSOUT (N,NZRED,NZBLK,IA,JA,A,RHS,NOUT,ISYM)    >*/
    sysout_(&tbk32_1.n, &tbk32_1.nzred, &tbk32_1.nzblk, &ia[1], &ja[1], &a[1],
	     &rhs[1], &tbk32_1.nout, &tbk32_1.isym);
/*<    60 CONTINUE     >*/
L60:
/*<       RETURN       >*/
    return 0;
/*<       END  >*/
} /* tstprb_ */

/*<       SUBROUTINE TSTITP (IA,JA,A,RHS,U,IWKSP,WKSP) >*/
/* Subroutine */ int tstitp_(integer *ia, integer *ja, doublereal *a, 
	doublereal *rhs, doublereal *u, integer *iwksp, doublereal *wksp)
{
    integer np1;
    extern /* Subroutine */ int jcg_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *);
    integer ier;
    extern /* Subroutine */ int jsi_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *), sor_(integer *,
	     integer *, integer *, doublereal *, doublereal *, doublereal *, 
	    integer *, integer *, doublereal *, integer *, doublereal *, 
	    integer *);
    doublereal tim1[7], tim2[7];
    extern /* Subroutine */ int rscg_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *);
    integer iter[7];
    extern /* Subroutine */ int rssi_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *), time2_(integer 
	    *, integer *, integer *, doublereal *, doublereal *, doublereal *,
	     integer *, doublereal *, doublereal *, doublereal *, doublereal *
	    , doublereal *, integer *, integer *);
    integer iparm[12];
    extern /* Subroutine */ int vfill_(integer *, doublereal *, doublereal *);
    doublereal rparm[12];
    integer iwork[7];
    doublereal digit1[7], digit2[7], digit3[7];
    extern /* Subroutine */ int dfault_(integer *, doublereal *), chknrm_(
	    doublereal *, doublereal *, doublereal *), ssorcg_(integer *, 
	    integer *, integer *, doublereal *, doublereal *, doublereal *, 
	    integer *, integer *, doublereal *, integer *, doublereal *, 
	    integer *), ssorsi_(integer *, integer *, integer *, doublereal *,
	     doublereal *, doublereal *, integer *, integer *, doublereal *, 
	    integer *, doublereal *, integer *);


/*     PROGRAM TO EXERCISE ITPACK 2C */

/*<       INTEGER IA(1),JA(1),IWKSP(1),IPARM(12),ITER(7),IWORK(7)        >*/
/*<    >*/

/*<       COMMON /TBK31/ DRELPR,ZETA       >*/
/*<    >*/

/*        TEST JCG */

/*<       CALL DFAULT (IPARM,RPARM)        >*/
    /* Parameter adjustments */
    --wksp;
    --iwksp;
    --u;
    --rhs;
    --a;
    --ja;
    --ia;

    /* Function Body */
    dfault_(iparm, rparm);
/*<       IPARM(1) = ITMAX       >*/
    iparm[0] = tbk32_1.itmax;
/*<       IPARM(2) = LEVEL       >*/
    iparm[1] = tbk32_1.level;
/*<       IPARM(5) = ISYM        >*/
    iparm[4] = tbk32_1.isym;
/*<       IPARM(12) = IERAN      >*/
    iparm[11] = tbk32_1.ieran;
/*<       RPARM(1) = ZETA        >*/
    rparm[0] = tbk31_1.zeta;
/*<       CALL VFILL (N,U,0.D0)  >*/
    vfill_(&tbk32_1.n, &u[1], &c_b19);
/*<       CALL JCG (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER)       >*/
    jcg_(&tbk32_1.n, &ia[1], &ja[1], &a[1], &rhs[1], &u[1], &iwksp[1], &
	    tbk32_1.nw, &wksp[1], iparm, rparm, &ier);
/*<       TIM1(1) = RPARM(9)     >*/
    tim1[0] = rparm[8];
/*<       TIM2(1) = RPARM(10)    >*/
    tim2[0] = rparm[9];
/*<       DIGIT1(1) = RPARM(11)  >*/
    digit1[0] = rparm[10];
/*<       DIGIT2(1) = RPARM(12)  >*/
    digit2[0] = rparm[11];
/*<       CALL CHKNRM (U,WKSP,DIGIT3(1))   >*/
    chknrm_(&u[1], &wksp[1], digit3);
/*<       ITER(1) = IPARM(1)     >*/
    iter[0] = iparm[0];
/*<       IWORK(1) = IPARM(8)    >*/
    iwork[0] = iparm[7];

/*        TEST JSI */

/*<       CALL DFAULT (IPARM,RPARM)        >*/
    dfault_(iparm, rparm);
/*<       IPARM(1) = ITMAX       >*/
    iparm[0] = tbk32_1.itmax;
/*<       IPARM(2) = LEVEL       >*/
    iparm[1] = tbk32_1.level;
/*<       IPARM(5) = ISYM        >*/
    iparm[4] = tbk32_1.isym;
/*<       IPARM(12) = IERAN      >*/
    iparm[11] = tbk32_1.ieran;
/*<       RPARM(1) = ZETA        >*/
    rparm[0] = tbk31_1.zeta;
/*<       CALL VFILL (N,U,0.D0)  >*/
    vfill_(&tbk32_1.n, &u[1], &c_b19);
/*<       CALL JSI (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER)       >*/
    jsi_(&tbk32_1.n, &ia[1], &ja[1], &a[1], &rhs[1], &u[1], &iwksp[1], &
	    tbk32_1.nw, &wksp[1], iparm, rparm, &ier);
/*<       TIM1(2) = RPARM(9)     >*/
    tim1[1] = rparm[8];
/*<       TIM2(2) = RPARM(10)    >*/
    tim2[1] = rparm[9];
/*<       DIGIT1(2) = RPARM(11)  >*/
    digit1[1] = rparm[10];
/*<       CALL CHKNRM (U,WKSP,DIGIT3(2))   >*/
    chknrm_(&u[1], &wksp[1], &digit3[1]);
/*<       DIGIT2(2) = RPARM(12)  >*/
    digit2[1] = rparm[11];
/*<       ITER(2) = IPARM(1)     >*/
    iter[1] = iparm[0];
/*<       IWORK(2) = IPARM(8)    >*/
    iwork[1] = iparm[7];

/*        TEST SOR */

/*<       CALL DFAULT (IPARM,RPARM)        >*/
    dfault_(iparm, rparm);
/*<       IPARM(1) = ITMAX       >*/
    iparm[0] = tbk32_1.itmax;
/*<       IPARM(2) = LEVEL       >*/
    iparm[1] = tbk32_1.level;
/*<       IPARM(5) = ISYM        >*/
    iparm[4] = tbk32_1.isym;
/*<       IPARM(12) = IERAN      >*/
    iparm[11] = tbk32_1.ieran;
/*<       RPARM(1) = ZETA        >*/
    rparm[0] = tbk31_1.zeta;
/*<       CALL VFILL (N,U,0.D0)  >*/
    vfill_(&tbk32_1.n, &u[1], &c_b19);
/*<       CALL SOR (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER)       >*/
    sor_(&tbk32_1.n, &ia[1], &ja[1], &a[1], &rhs[1], &u[1], &iwksp[1], &
	    tbk32_1.nw, &wksp[1], iparm, rparm, &ier);
/*<       TIM1(3) = RPARM(9)     >*/
    tim1[2] = rparm[8];
/*<       TIM2(3) = RPARM(10)    >*/
    tim2[2] = rparm[9];
/*<       DIGIT1(3) = RPARM(11)  >*/
    digit1[2] = rparm[10];
/*<       CALL CHKNRM (U,WKSP,DIGIT3(3))   >*/
    chknrm_(&u[1], &wksp[1], &digit3[2]);
/*<       DIGIT2(3) = RPARM(12)  >*/
    digit2[2] = rparm[11];
/*<       ITER(3) = IPARM(1)     >*/
    iter[2] = iparm[0];
/*<       IWORK(3) = IPARM(8)    >*/
    iwork[2] = iparm[7];

/*        TEST SSORCG */

/*<       CALL DFAULT (IPARM,RPARM)        >*/
    dfault_(iparm, rparm);
/*<       IPARM(1) = ITMAX       >*/
    iparm[0] = tbk32_1.itmax;
/*<       IPARM(2) = LEVEL       >*/
    iparm[1] = tbk32_1.level;
/*<       IPARM(5) = ISYM        >*/
    iparm[4] = tbk32_1.isym;
/*<       IPARM(12) = IERAN      >*/
    iparm[11] = tbk32_1.ieran;
/*<       RPARM(1) = ZETA        >*/
    rparm[0] = tbk31_1.zeta;
/*<       RPARM(7) = .26D0       >*/
    rparm[6] = .26;
/*<       CALL VFILL (N,U,0.D0)  >*/
    vfill_(&tbk32_1.n, &u[1], &c_b19);
/*<       CALL SSORCG (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER)    >*/
    ssorcg_(&tbk32_1.n, &ia[1], &ja[1], &a[1], &rhs[1], &u[1], &iwksp[1], &
	    tbk32_1.nw, &wksp[1], iparm, rparm, &ier);
/*<       TIM1(4) = RPARM(9)     >*/
    tim1[3] = rparm[8];
/*<       TIM2(4) = RPARM(10)    >*/
    tim2[3] = rparm[9];
/*<       DIGIT1(4) = RPARM(11)  >*/
    digit1[3] = rparm[10];
/*<       DIGIT2(4) = RPARM(12)  >*/
    digit2[3] = rparm[11];
/*<       CALL CHKNRM (U,WKSP,DIGIT3(4))   >*/
    chknrm_(&u[1], &wksp[1], &digit3[3]);
/*<       ITER(4) = IPARM(1)     >*/
    iter[3] = iparm[0];
/*<       IWORK(4) = IPARM(8)    >*/
    iwork[3] = iparm[7];

/*        TEST SSORSI */

/*<       CALL DFAULT (IPARM,RPARM)        >*/
    dfault_(iparm, rparm);
/*<       IPARM(1) = ITMAX       >*/
    iparm[0] = tbk32_1.itmax;
/*<       IPARM(2) = LEVEL       >*/
    iparm[1] = tbk32_1.level;
/*<       IPARM(5) = ISYM        >*/
    iparm[4] = tbk32_1.isym;
/*<       IPARM(12) = IERAN      >*/
    iparm[11] = tbk32_1.ieran;
/*<       RPARM(1) = ZETA        >*/
    rparm[0] = tbk31_1.zeta;
/*<       CALL VFILL (N,U,0.D0)  >*/
    vfill_(&tbk32_1.n, &u[1], &c_b19);
/*<       CALL SSORSI (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER)    >*/
    ssorsi_(&tbk32_1.n, &ia[1], &ja[1], &a[1], &rhs[1], &u[1], &iwksp[1], &
	    tbk32_1.nw, &wksp[1], iparm, rparm, &ier);
/*<       TIM1(5) = RPARM(9)     >*/
    tim1[4] = rparm[8];
/*<       TIM2(5) = RPARM(10)    >*/
    tim2[4] = rparm[9];
/*<       DIGIT1(5) = RPARM(11)  >*/
    digit1[4] = rparm[10];
/*<       DIGIT2(5) = RPARM(12)  >*/
    digit2[4] = rparm[11];
/*<       CALL CHKNRM (U,WKSP,DIGIT3(5))   >*/
    chknrm_(&u[1], &wksp[1], &digit3[4]);
/*<       ITER(5) = IPARM(1)     >*/
    iter[4] = iparm[0];
/*<       IWORK(5) = IPARM(8)    >*/
    iwork[4] = iparm[7];

/*        TEST RSCG */

/*<       CALL DFAULT (IPARM,RPARM)        >*/
    dfault_(iparm, rparm);
/*<       IPARM(1) = ITMAX       >*/
    iparm[0] = tbk32_1.itmax;
/*<       IPARM(2) = LEVEL       >*/
    iparm[1] = tbk32_1.level;
/*<       IPARM(5) = ISYM        >*/
    iparm[4] = tbk32_1.isym;
/*<       IF (IORD.EQ.2) IPARM(9) = NB     >*/
    if (tbk32_1.iord == 2) {
	iparm[8] = tbk32_1.nb;
    }
/*<       IPARM(12) = IERAN      >*/
    iparm[11] = tbk32_1.ieran;
/*<       RPARM(1) = ZETA        >*/
    rparm[0] = tbk31_1.zeta;
/*<       CALL VFILL (N,U,0.D0)  >*/
    vfill_(&tbk32_1.n, &u[1], &c_b19);
/*<       CALL RSCG (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER)      >*/
    rscg_(&tbk32_1.n, &ia[1], &ja[1], &a[1], &rhs[1], &u[1], &iwksp[1], &
	    tbk32_1.nw, &wksp[1], iparm, rparm, &ier);
/*<       TIM1(6) = RPARM(9)     >*/
    tim1[5] = rparm[8];
/*<       TIM2(6) = RPARM(10)    >*/
    tim2[5] = rparm[9];
/*<       DIGIT1(6) = RPARM(11)  >*/
    digit1[5] = rparm[10];
/*<       DIGIT2(6) = RPARM(12)  >*/
    digit2[5] = rparm[11];
/*<       CALL CHKNRM (U,WKSP,DIGIT3(6))   >*/
    chknrm_(&u[1], &wksp[1], &digit3[5]);
/*<       ITER(6) = IPARM(1)     >*/
    iter[5] = iparm[0];
/*<       IWORK(6) = IPARM(8)    >*/
    iwork[5] = iparm[7];

/*        TEST RSSI */

/*<       CALL DFAULT (IPARM,RPARM)        >*/
    dfault_(iparm, rparm);
/*<       IPARM(1) = ITMAX       >*/
    iparm[0] = tbk32_1.itmax;
/*<       IPARM(2) = LEVEL       >*/
    iparm[1] = tbk32_1.level;
/*<       IPARM(5) = ISYM        >*/
    iparm[4] = tbk32_1.isym;
/*<       IF (IORD.EQ.2) IPARM(9) = NB     >*/
    if (tbk32_1.iord == 2) {
	iparm[8] = tbk32_1.nb;
    }
/*<       IPARM(12) = IERAN      >*/
    iparm[11] = tbk32_1.ieran;
/*<       RPARM(1) = ZETA        >*/
    rparm[0] = tbk31_1.zeta;
/*<       CALL VFILL (N,U,0.D0)  >*/
    vfill_(&tbk32_1.n, &u[1], &c_b19);
/*<       CALL RSSI (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER)      >*/
    rssi_(&tbk32_1.n, &ia[1], &ja[1], &a[1], &rhs[1], &u[1], &iwksp[1], &
	    tbk32_1.nw, &wksp[1], iparm, rparm, &ier);
/*<       TIM1(7) = RPARM(9)     >*/
    tim1[6] = rparm[8];
/*<       TIM2(7) = RPARM(10)    >*/
    tim2[6] = rparm[9];
/*<       DIGIT1(7) = RPARM(11)  >*/
    digit1[6] = rparm[10];
/*<       DIGIT2(7) = RPARM(12)  >*/
    digit2[6] = rparm[11];
/*<       CALL CHKNRM (U,WKSP,DIGIT3(7))   >*/
    chknrm_(&u[1], &wksp[1], &digit3[6]);
/*<       ITER(7) = IPARM(1)     >*/
    iter[6] = iparm[0];
/*<       IWORK(7) = IPARM(8)    >*/
    iwork[6] = iparm[7];

/*     TIMING ANALYSIS */

/*<       NP1 = N+1    >*/
    np1 = tbk32_1.n + 1;
/*<    >*/
    time2_(&tbk32_1.n, &ia[1], &ja[1], &a[1], &wksp[1], &wksp[np1], iter, 
	    tim1, tim2, digit1, digit2, digit3, iwork, &tbk32_1.nout);

/*<       RETURN       >*/
    return 0;
/*<       END  >*/
} /* tstitp_ */

/*<    >*/
/* Subroutine */ int time2_(integer *n, integer *ia, integer *ja, doublereal *
	a, doublereal *v, doublereal *w, integer *iter, doublereal *tim1, 
	doublereal *tim2, doublereal *digit1, doublereal *digit2, doublereal *
	digit3, integer *iwork, integer *nout)
{
    /* Initialized data */

    static integer maxlp = 50;

    /* Format strings */
    static char fmt_30[] = "(\0020\002,3(/),15x,\002TIMING ANALYSIS\002/15x,\
15(\002-\002)//35x,\002ITERATION\002,3x,\002TOTAL\002,9x,\002NO. OF\002,4x\
,\002AVG ITER/\002,2x,\002DIGITS\002,4x,\002DIGITS\002,5x,\002DIGITS\002,5x\
,\002WKSP\002/15x,\002METHOD\002,14x,\002TIME (SEC)\002,2x,\002TIME (SEC)\
\002,1x,\002ITERATIONS\002,1x,\002MTX-VCTR MLT\002,1x,\002STP TST\002,3x,\
\002RES/RHS\002,4x,\002TRUE\002,7x,\002USED\002/15x,103(\002-\002))";
    static char fmt_40[] = "(15x,\002JACOBI CG        \002,2f10.3,i14,f10.1,\
1x,3f10.1,i10/15x,\002JACOBI SI        \002,2f10.3,i14,f10.1,1x,3f10.1,i10/1\
5x,\002SOR              \002,2f10.3,i14,f10.1,1x,3f10.1,i10/15x,\002SYMMETRI\
C SOR CG \002,2f10.3,i14,f10.1,1x,3f10.1,i10/15x,\002SYMMETRIC SOR SI \002,2\
f10.3,i14,f10.1,1x,3f10.1,i10/15x,\002REDUCED SYSTEM CG\002,2f10.3,i14,f10.1\
,1x,3f10.1,i10/15x,\002REDUCED SYSTEM SI\002,2f10.3,i14,f10.1,1x,3f10.1,i10/)"
	    ;

    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), e_wsfe(), do_fio(integer *, char *, ftnlen);

    /* Local variables */
    integer i__;
    doublereal rel[7], temp;
    real timi1, timi2;
    extern /* Subroutine */ int vfill_(integer *, doublereal *, doublereal *);
    extern doublereal timer_(real *);
    extern /* Subroutine */ int pmult_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *);
    doublereal timmat;

    /* Fortran I/O blocks */
    static cilist io___262 = { 0, 0, 0, fmt_30, 0 };
    static cilist io___265 = { 0, 0, 0, fmt_40, 0 };


/*<       INTEGER ITER(7),IA(1),JA(1),IWORK(7)       >*/
/*<    >*/
/*<       DATA MAXLP / 50 /      >*/
    /* Parameter adjustments */
    --w;
    --v;
    --ia;
    --ja;
    --a;
    --iter;
    --tim1;
    --tim2;
    --digit1;
    --digit2;
    --digit3;
    --iwork;

    /* Function Body */

/*<       CALL VFILL (N,V,1.D0)  >*/
    vfill_(n, &v[1], &c_b27);
/*<       TIMI1 = TIMER(0.0)     >*/
    timi1 = timer_(&c_b28);
/*<       DO 10 I = 1,MAXLP      >*/
    i__1 = maxlp;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<          CALL PMULT (N,IA,JA,A,V,W)    >*/
	pmult_(n, &ia[1], &ja[1], &a[1], &v[1], &w[1]);
/*<    10 CONTINUE     >*/
/* L10: */
    }
/*<       TIMI2 = TIMER(0.0)     >*/
    timi2 = timer_(&c_b28);
/*<       TIMMAT = DBLE(TIMI2-TIMI1)/DBLE(FLOAT(MAXLP))        >*/
    timmat = (doublereal) (timi2 - timi1) / (doublereal) ((real) maxlp);

/*<       WRITE (NOUT,30)        >*/
    io___262.ciunit = *nout;
    s_wsfe(&io___262);
    e_wsfe();
/*<       DO 20 I = 1,7  >*/
    for (i__ = 1; i__ <= 7; ++i__) {
/*<          REL(I) = 0.D0       >*/
	rel[i__ - 1] = 0.;
/*<          TEMP = DBLE(FLOAT(ITER(I)))   >*/
	temp = (doublereal) ((real) iter[i__]);
/*<          IF ((TIMI2.EQ.TIMI1).OR.(ITER(I).EQ.0)) GO TO 20  >*/
	if (timi2 == timi1 || iter[i__] == 0) {
	    goto L20;
	}
/*<          REL(I) = TIM1(I)/(TEMP*TIMMAT)  >*/
	rel[i__ - 1] = tim1[i__] / (temp * timmat);
/*<    20 CONTINUE     >*/
L20:
	;
    }

/*<    >*/
    io___265.ciunit = *nout;
    s_wsfe(&io___265);
    for (i__ = 1; i__ <= 7; ++i__) {
	do_fio(&c__1, (char *)&tim1[i__], (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&tim2[i__], (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&iter[i__], (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&rel[i__ - 1], (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&digit1[i__], (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&digit2[i__], (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&digit3[i__], (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&iwork[i__], (ftnlen)sizeof(integer));
    }
    e_wsfe();

/*<       RETURN       >*/
    return 0;

/*<    >*/
/*<    >*/

/*<       END  >*/
} /* time2_ */

/*<       SUBROUTINE CHKNRM (U,WKSP,DIGIT) >*/
/* Subroutine */ int chknrm_(doublereal *u, doublereal *wksp, doublereal *
	digit)
{
    /* Format strings */
    static char fmt_30[] = "(/13x,\002NO. OF DIGITS IN TRUE RATIO =\002,f5.1)"
	    ;

    /* System generated locals */
    integer i__1;
    doublereal d__1;

    /* Builtin functions */
    double d_lg10(doublereal *);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    integer i__;
    extern doublereal ddot_(integer *, doublereal *, integer *, doublereal *, 
	    integer *);
    doublereal wknrm;

    /* Fortran I/O blocks */
    static cilist io___268 = { 0, 0, 0, fmt_30, 0 };



/*     COMPUTE TRUE RATIO */

/*<       DOUBLE PRECISION U(1),WKSP(1),DIGIT,DRELPR,WKNRM,ZETA,DDOT     >*/

/*<       COMMON /TBK31/ DRELPR,ZETA       >*/
/*<    >*/

/*<       DO 10 I = 1,N  >*/
    /* Parameter adjustments */
    --wksp;
    --u;

    /* Function Body */
    i__1 = tbk32_1.n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<          WKSP(I) = U(I)-1.D0 >*/
	wksp[i__] = u[i__] - 1.;
/*<    10 CONTINUE     >*/
/* L10: */
    }

/*<       DIGIT = -DLOG10(DRELPR)  >*/
    *digit = -d_lg10(&tbk31_1.drelpr);
/*<       WKNRM = DDOT(N,WKSP,1,WKSP,1)    >*/
    wknrm = ddot_(&tbk32_1.n, &wksp[1], &c__1, &wksp[1], &c__1);
/*<       IF (WKNRM.EQ.0.D0) GO TO 20      >*/
    if (wknrm == 0.) {
	goto L20;
    }
/*<       DIGIT = -(DLOG10(WKNRM)-DLOG10(DBLE(FLOAT(N))))/2.0D0  >*/
    d__1 = (doublereal) ((real) tbk32_1.n);
    *digit = -(d_lg10(&wknrm) - d_lg10(&d__1)) / 2.;

/*<    20 IF (ILEVEL.EQ.1) WRITE (NOUT,30) DIGIT     >*/
L20:
    if (tbk32_1.ilevel == 1) {
	io___268.ciunit = tbk32_1.nout;
	s_wsfe(&io___268);
	do_fio(&c__1, (char *)&(*digit), (ftnlen)sizeof(doublereal));
	e_wsfe();
    }
/*<    30 FORMAT (/13X,'NO. OF DIGITS IN TRUE RATIO =',F5.1)   >*/

/*<       RETURN       >*/
    return 0;
/*<       END  >*/
} /* chknrm_ */

/*<       INTEGER FUNCTION IRAND (I,J,ISEED) >*/
integer irand_(integer *i__, integer *j, integer *iseed)
{
    /* System generated locals */
    integer ret_val;

    /* Local variables */
    extern doublereal random_(integer *);


/* ***************************************************************** */

/*      THIS SUBPROGRAM GENERATES UNIFORMLY DISTRIBUTED RANDOM */
/*      INTEGERS BETWEEN  I  AND  J  (INCLUSIVE) */

/* ****************************************************************** */

/*<       INTEGER I,J,ISEED      >*/

/* ==================================================================== */

/*<       IRAND = IFIX(FLOAT(J-I+1)*RANDOM(ISEED))+I >*/
    ret_val = (integer) ((real) (*j - *i__ + 1) * random_(iseed)) + *i__;

/*<       RETURN       >*/
    return ret_val;
/*<       END  >*/
} /* irand_ */

/*<       FUNCTION RANDOM (ISEED)  >*/
doublereal random_(integer *iseed)
{
    /* System generated locals */
    real ret_val;
    doublereal d__1;

    /* Builtin functions */
    double d_mod(doublereal *, doublereal *);

    /* Local variables */
    doublereal dl;


/*     RANDOM NUMBER GENERATOR - UNIFORMLY DISTRIBUTED IN (0,1) */
/*                               ISEED IN (1,2147483647) */

/*     FOLLOWING CODE USED BECAUSE OF POSSIBLE USE WITH */
/*     SHORT WORD LENGTH COMPUTERS */

/*<       DOUBLE PRECISION DL    >*/
/*<       DL = DMOD(16807.0D0*DBLE(FLOAT(ISEED)),2147483647.0D0) >*/
    d__1 = (doublereal) ((real) (*iseed)) * 16807.;
    dl = d_mod(&d__1, &c_b388);
/*<       ISEED = IDINT(DL)      >*/
    *iseed = (integer) dl;
/*<       RANDOM = SNGL(DL*4.6566128752458D-10)      >*/
    ret_val = (real) (dl * 4.6566128752458e-10);

/*     ON LONG WORD LENGTH COMPUTERS THE FOLLOWING CODE */
/*     MAY BE USED OR LOCAL RANDOM NUMBER GENERATOR */

/*     ISEED = MOD(16807*ISEED,2147483647) */
/*     RANDOM = FLOAT(ISEED)*4.6566128752458E-10 */

/*<       RETURN       >*/
    return ret_val;
/*<       END  >*/
} /* random_ */

/*<       SUBROUTINE PLTADJ (N,IA,JA,ISYM,IROW,ICOL,NOUT)      >*/
/* Subroutine */ int pltadj_(integer *n, integer *ia, integer *ja, integer *
	isym, integer *irow, integer *icol, integer *nout)
{
    /* Initialized data */

    static struct {
	char e_1[4];
	integer e_2;
	} equiv_283 = { {' ', ' ', ' ', ' '}, 0 };

#define blank (*(integer *)&equiv_283)

    static struct {
	char e_1[4];
	integer e_2;
	} equiv_284 = { {'*', ' ', ' ', ' '}, 0 };

#define star (*(integer *)&equiv_284)


    /* Format strings */
    static char fmt_80[] = "(2x,i5,3x,120a1)";

    /* System generated locals */
    integer i__1, i__2;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    integer i__, j, k, m, jj, im1, kadd, ibgn, iend, itemp;

    /* Fortran I/O blocks */
    static cilist io___282 = { 0, 0, 0, fmt_80, 0 };



/* ******************************************************************** */

/*     THIS SUBROUTINE PLOTS THE ADJACENCY STRUCTURE OF A SPARE */
/*     MATRIX STORED IN SYMMETRIC/NONSYMMETRIC FORMAT */

/*     WRITTEN BY ROGER G GRIMES    AUGUST 1980 */

/* ********************************************************************* */

/*<       INTEGER N,IA(1),JA(1),ICOL(1),IROW(N)      >*/

/*<       INTEGER BLANK,STAR,I,M,IM1,KADD,IBGN,IEND,J,JJ       >*/

/*<       DATA BLANK / 1H / ,STAR / 1H+/   >*/
    /* Parameter adjustments */
    --irow;
    --ia;
    --ja;
    --icol;

    /* Function Body */

/* ==================================================================== */

/* ... SET WORKSPACE ARRAYS TO ALL BLANKS.  IROW WILL BE USED TO INDICATE */
/*     THE ADJACENCY STRUCTURE FOR THE CURRENT ROW.  ICOL WILL STORE THE */
/*     ADJACENCY STRUCTURE FOR THE LOWER TRIANGLE FOR COLUMNS 1 THRU */
/*     THE CURRENT COLUMN ( SAME AS CURRENT ROW ). */

/*<       DO 10 I = 1,N  >*/
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<          IROW(I) = BLANK     >*/
	irow[i__] = blank;
/*<    10 CONTINUE     >*/
/* L10: */
    }
/*<       IF (ISYM.NE.0) GO TO 30  >*/
    if (*isym != 0) {
	goto L30;
    }
/*<       M = N*(N+1)/2  >*/
    m = *n * (*n + 1) / 2;
/*<       DO 20 I = 1,M  >*/
    i__1 = m;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<          ICOL(I) = BLANK     >*/
	icol[i__] = blank;
/*<    20 CONTINUE     >*/
/* L20: */
    }

/* ... LOOP OVER EACH ROW ( AND COLUMN ) */

/*<    30 KADD = 0     >*/
L30:
    kadd = 0;
/*<       DO 100 I = 1,N >*/
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<          IM1 = I-1 >*/
	im1 = i__ - 1;
/*<          KADD = KADD+IM1     >*/
	kadd += im1;
/*<          IBGN = IA(I)        >*/
	ibgn = ia[i__];
/*<          IEND = IA(I+1)-1    >*/
	iend = ia[i__ + 1] - 1;
/*<          IF (IBGN.GT.IEND) GO TO 50    >*/
	if (ibgn > iend) {
	    goto L50;
	}
/*<          DO 40 J = IBGN,IEND >*/
	i__2 = iend;
	for (j = ibgn; j <= i__2; ++j) {
/*<             JJ = JA(J)       >*/
	    jj = ja[j];
/*<             IROW(JJ) = STAR  >*/
	    irow[jj] = star;
/*<             IF (ISYM.NE.0) GO TO 40    >*/
	    if (*isym != 0) {
		goto L40;
	    }
/*<             K = (JJ-1)*JJ/2+I  >*/
	    k = (jj - 1) * jj / 2 + i__;
/*<             ICOL(K) = STAR   >*/
	    icol[k] = star;
/*<    40    CONTINUE  >*/
L40:
	    ;
	}
/*<          IF (I.EQ.1) GO TO 70  >*/
	if (i__ == 1) {
	    goto L70;
	}
/*<          IF (ISYM.NE.0) GO TO 70       >*/
	if (*isym != 0) {
	    goto L70;
	}
/*<    50    DO 60 J = 1,IM1     >*/
L50:
	i__2 = im1;
	for (j = 1; j <= i__2; ++j) {
/*<             ITEMP = KADD+J   >*/
	    itemp = kadd + j;
/*<             IROW(J) = ICOL(ITEMP)      >*/
	    irow[j] = icol[itemp];
/*<    60    CONTINUE  >*/
/* L60: */
	}
/*<    70    WRITE (NOUT,80) I,(IROW(K),K=1,N)       >*/
L70:
	io___282.ciunit = *nout;
	s_wsfe(&io___282);
	do_fio(&c__1, (char *)&i__, (ftnlen)sizeof(integer));
	i__2 = *n;
	for (k = 1; k <= i__2; ++k) {
	    do_fio(&c__1, (char *)&irow[k], (ftnlen)sizeof(integer));
	}
	e_wsfe();
/*<    80    FORMAT (2X,I5,3X,120A1)       >*/
/*<          DO 90 J = 1,N       >*/
	i__2 = *n;
	for (j = 1; j <= i__2; ++j) {
/*<             IROW(J) = BLANK  >*/
	    irow[j] = blank;
/*<    90    CONTINUE  >*/
/* L90: */
	}
/*<   100 CONTINUE     >*/
/* L100: */
    }

/* ... RETURN */

/*<       RETURN       >*/
    return 0;
/*<       END  >*/
} /* pltadj_ */

#undef star
#undef blank


/*<       SUBROUTINE SYSOUT (N,NZRED,NZBLK,IA,JA,A,RHS,NOUT,ISYM)        >*/
/* Subroutine */ int sysout_(integer *n, integer *nzred, integer *nzblk, 
	integer *ia, integer *ja, doublereal *a, doublereal *rhs, integer *
	nout, integer *isym)
{
    /* Format strings */
    static char fmt_10[] = "(\0020  SYSTEM (A,B) \002)";
    static char fmt_120[] = "(/2x,11(1x,g11.4))";
    static char fmt_70[] = "(//2x,\002IA ARRAY\002)";
    static char fmt_80[] = "(2x,10i8)";
    static char fmt_90[] = "(//2x,\002JA ARRAY\002)";
    static char fmt_100[] = "(//2x,\002A ARRAY \002)";
    static char fmt_110[] = "(//2x,\002RHS ARRAY \002)";

    /* System generated locals */
    integer i__1, i__2;

    /* Builtin functions */
    integer s_wsfe(cilist *), e_wsfe(), do_fio(integer *, char *, ftnlen);

    /* Local variables */
    integer i__, j, jaj;
    doublereal tmp[100]	/* was [10][10] */;
    integer ibgn, iend, nzend;

    /* Fortran I/O blocks */
    static cilist io___285 = { 0, 0, 0, fmt_10, 0 };
    static cilist io___292 = { 0, 0, 0, fmt_120, 0 };
    static cilist io___293 = { 0, 0, 0, fmt_70, 0 };
    static cilist io___294 = { 0, 0, 0, fmt_80, 0 };
    static cilist io___295 = { 0, 0, 0, fmt_90, 0 };
    static cilist io___296 = { 0, 0, 0, fmt_80, 0 };
    static cilist io___298 = { 0, 0, 0, fmt_100, 0 };
    static cilist io___299 = { 0, 0, 0, fmt_120, 0 };
    static cilist io___300 = { 0, 0, 0, fmt_110, 0 };
    static cilist io___301 = { 0, 0, 0, fmt_120, 0 };



/*     PRINT SYSTEM OUT IN 2X2 FORMAT IF  N .LE. 10 */

/*<       INTEGER IA(1),JA(1)    >*/
/*<       DOUBLE PRECISION A(1),RHS(1),TMP(10,10)    >*/

/*<       WRITE (NOUT,10)        >*/
    /* Parameter adjustments */
    --rhs;
    --a;
    --ja;
    --ia;

    /* Function Body */
    io___285.ciunit = *nout;
    s_wsfe(&io___285);
    e_wsfe();
/*<    10 FORMAT ('0  SYSTEM (A,B) ')      >*/

/*<       IF (N.GT.10) GO TO 60  >*/
    if (*n > 10) {
	goto L60;
    }

/*<       DO 20 I = 1,N  >*/
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<          DO 20 J = 1,N       >*/
	i__2 = *n;
	for (j = 1; j <= i__2; ++j) {
/*<             TMP(I,J) = 0.D0  >*/
	    tmp[i__ + j * 10 - 11] = 0.;
/*<    20 CONTINUE     >*/
/* L20: */
	}
    }

/*<       DO 40 I = 1,N  >*/
    i__2 = *n;
    for (i__ = 1; i__ <= i__2; ++i__) {
/*<          IBGN = IA(I)        >*/
	ibgn = ia[i__];
/*<          IEND = IA(I+1)-1    >*/
	iend = ia[i__ + 1] - 1;
/*<          IF (IBGN.GT.IEND) GO TO 40    >*/
	if (ibgn > iend) {
	    goto L40;
	}
/*<          DO 30 J = IBGN,IEND >*/
	i__1 = iend;
	for (j = ibgn; j <= i__1; ++j) {
/*<             JAJ = JA(J)      >*/
	    jaj = ja[j];
/*<             TMP(I,JAJ) = A(J)  >*/
	    tmp[i__ + jaj * 10 - 11] = a[j];
/*<             IF (ISYM.EQ.0) TMP(JAJ,I) = A(J)     >*/
	    if (*isym == 0) {
		tmp[jaj + i__ * 10 - 11] = a[j];
	    }
/*<    30    CONTINUE  >*/
/* L30: */
	}
/*<    40 CONTINUE     >*/
L40:
	;
    }

/*<       DO 50 I = 1,N  >*/
    i__2 = *n;
    for (i__ = 1; i__ <= i__2; ++i__) {
/*<          WRITE (NOUT,120) (TMP(I,J),J=1,N),RHS(I)  >*/
	io___292.ciunit = *nout;
	s_wsfe(&io___292);
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    do_fio(&c__1, (char *)&tmp[i__ + j * 10 - 11], (ftnlen)sizeof(
		    doublereal));
	}
	do_fio(&c__1, (char *)&rhs[i__], (ftnlen)sizeof(doublereal));
	e_wsfe();
/*<    50 CONTINUE     >*/
/* L50: */
    }
/*<       RETURN       >*/
    return 0;

/*<    60 CONTINUE     >*/
L60:
/*<       WRITE (NOUT,70)        >*/
    io___293.ciunit = *nout;
    s_wsfe(&io___293);
    e_wsfe();
/*<    70 FORMAT (//2X,'IA ARRAY') >*/
/*<       IEND = N+1   >*/
    iend = *n + 1;
/*<       WRITE (NOUT,80) (IA(I),I=1,IEND) >*/
    io___294.ciunit = *nout;
    s_wsfe(&io___294);
    i__2 = iend;
    for (i__ = 1; i__ <= i__2; ++i__) {
	do_fio(&c__1, (char *)&ia[i__], (ftnlen)sizeof(integer));
    }
    e_wsfe();
/*<    80 FORMAT (2X,10I8)       >*/
/*<       IEND = IA(IEND)-1      >*/
    iend = ia[iend] - 1;
/*<       WRITE (NOUT,90)        >*/
    io___295.ciunit = *nout;
    s_wsfe(&io___295);
    e_wsfe();
/*<    90 FORMAT (//2X,'JA ARRAY') >*/
/*<       WRITE (NOUT,80) (JA(I),I=1,IEND) >*/
    io___296.ciunit = *nout;
    s_wsfe(&io___296);
    i__2 = iend;
    for (i__ = 1; i__ <= i__2; ++i__) {
	do_fio(&c__1, (char *)&ja[i__], (ftnlen)sizeof(integer));
    }
    e_wsfe();
/*<       NZEND = NZRED+N        >*/
    nzend = *nzred + *n;
/*<       IF (ISYM.NE.0) NZEND = NZEND+NZBLK >*/
    if (*isym != 0) {
	nzend += *nzblk;
    }
/*<       WRITE (NOUT,100)       >*/
    io___298.ciunit = *nout;
    s_wsfe(&io___298);
    e_wsfe();
/*<   100 FORMAT (//2X,'A ARRAY ') >*/
/*<       WRITE (NOUT,120) (A(I),I=1,NZEND)  >*/
    io___299.ciunit = *nout;
    s_wsfe(&io___299);
    i__2 = nzend;
    for (i__ = 1; i__ <= i__2; ++i__) {
	do_fio(&c__1, (char *)&a[i__], (ftnlen)sizeof(doublereal));
    }
    e_wsfe();
/*<       WRITE (NOUT,110)       >*/
    io___300.ciunit = *nout;
    s_wsfe(&io___300);
    e_wsfe();
/*<   110 FORMAT (//2X,'RHS ARRAY ')       >*/
/*<       WRITE (NOUT,120) (RHS(I),I=1,N)  >*/
    io___301.ciunit = *nout;
    s_wsfe(&io___301);
    i__2 = *n;
    for (i__ = 1; i__ <= i__2; ++i__) {
	do_fio(&c__1, (char *)&rhs[i__], (ftnlen)sizeof(doublereal));
    }
    e_wsfe();
/*<   120 FORMAT (/2X,11(1X,G11.4))        >*/

/*<       RETURN       >*/
    return 0;
/*<       END  >*/
} /* sysout_ */

/*<       SUBROUTINE TEST4D (EPSI) >*/
/* Subroutine */ int test4d_(doublereal *epsi)
{
    /* Format strings */
    static char fmt_10[] = "(\0021\002//15x,\002ITPACK 2C  TEST PROGRAM -- T\
EST4D\002)";
    static char fmt_20[] = "(\0020 *****************************************\
****** \002)";
    static char fmt_30[] = "(5x,\002 BETA =\002,f10.5)";

    /* Builtin functions */
    integer s_wsfe(cilist *), e_wsfe(), do_fio(integer *, char *, ftnlen);

    /* Local variables */
    doublereal a[33], b, c__, d__, h__;
    integer i__, k, n;
    doublereal u[9];
    integer ia[10], ja[33], kk, nw;
    extern /* Subroutine */ int jcg_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *);
    integer ier;
    extern /* Subroutine */ int jsi_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *);
    doublereal rhs[9];
    extern /* Subroutine */ int sor_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *);
    doublereal beta;
    integer ibgn;
    extern /* Subroutine */ int rscg_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *), rssi_(integer *
	    , integer *, integer *, doublereal *, doublereal *, doublereal *, 
	    integer *, integer *, doublereal *, integer *, doublereal *, 
	    integer *);
    integer isym;
    doublereal wksp[460];
    integer nout, icase, level, iparm[12], idgts;
    extern /* Subroutine */ int vfill_(integer *, doublereal *, doublereal *);
    doublereal rparm[12];
    integer iwksp[27];
    extern /* Subroutine */ int nsexp_(integer *, integer *, doublereal *, 
	    doublereal *, doublereal *, integer *), ssexp_(integer *, integer 
	    *, doublereal *, integer *);
    integer iadapt;
    extern /* Subroutine */ int dfault_(integer *, doublereal *), ssorcg_(
	    integer *, integer *, integer *, doublereal *, doublereal *, 
	    doublereal *, integer *, integer *, doublereal *, integer *, 
	    doublereal *, integer *), ssorsi_(integer *, integer *, integer *,
	     doublereal *, doublereal *, doublereal *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *);

    /* Fortran I/O blocks */
    static cilist io___307 = { 0, 0, 0, fmt_10, 0 };
    static cilist io___317 = { 0, 0, 0, fmt_20, 0 };
    static cilist io___318 = { 0, 0, 0, fmt_30, 0 };



/* ... TEST4D EXERCISES ITPACK 2C WITH VARIOUS SWITCHES SET */
/*     ON THE LINEAR SYSTEM CORRESPONDING TO THE FINITE */
/*     DIFFERENCE SOLUTION TO THE PDE */

/*              U   + U   + BU  = F */
/*               XX    YY     X */

/*     SUBJECT TO U(.,0) = U(.,1) = U(0,.) = U(1,.) = 0 */

/*     WHERE F = 2X(X-1) + Y(Y-1)(2-B(1-2X)). THE TRUE SOLUTION */
/*     IS U = XY(1-X)(1-Y). THE MESH SIZE IS 1/4. */

/*<       INTEGER IA(10),JA(33),IPARM(12),IWKSP(27)  >*/
/*<    >*/
/*<       DOUBLE PRECISION X,Y,BTA,F       >*/

/*<    >*/

/*<       NW = 460     >*/
    nw = 460;
/*<       N = 9        >*/
    n = 9;
/*<       LEVEL = 1    >*/
    level = 1;
/*<       IDGTS = 1    >*/
    idgts = 1;
/*<       NOUT = 6     >*/
    nout = 6;
/*<       WRITE (NOUT,10)        >*/
    io___307.ciunit = nout;
    s_wsfe(&io___307);
    e_wsfe();
/*<    10 FORMAT ('1'//15X,'ITPACK 2C  TEST PROGRAM -- TEST4D')  >*/

/*     ************* START LOOP TO DO BOTH CASES ****************** */

/*<       DO 110 K = 2,11,9      >*/
    for (k = 2; k <= 11; k += 9) {
/*<          BETA = DBLE(FLOAT(K-2))       >*/
	beta = (doublereal) ((real) (k - 2));
/*<          H = 1.D0/4.D0       >*/
	h__ = .25;
/*<          B = 1.D0/4.D0       >*/
	b = .25;
/*<          C = 1.D0/2.D0       >*/
	c__ = .5;
/*<          D = 3.D0/4.D0       >*/
	d__ = .75;
/*<          RHS(1) = -H*H*F(B,B,BETA)     >*/
	rhs[0] = -h__ * h__ * (2. * b * (b - 1.) + b * (b - 1.) * (2. - beta *
		 (1. - 2. * b)));
/*<          RHS(2) = -H*H*F(C,B,BETA)     >*/
	rhs[1] = -h__ * h__ * (2. * c__ * (c__ - 1.) + b * (b - 1.) * (2. - 
		beta * (1. - 2. * c__)));
/*<          RHS(3) = -H*H*F(D,B,BETA)     >*/
	rhs[2] = -h__ * h__ * (2. * d__ * (d__ - 1.) + b * (b - 1.) * (2. - 
		beta * (1. - 2. * d__)));
/*<          RHS(4) = -H*H*F(B,C,BETA)     >*/
	rhs[3] = -h__ * h__ * (2. * b * (b - 1.) + c__ * (c__ - 1.) * (2. - 
		beta * (1. - 2. * b)));
/*<          RHS(5) = -H*H*F(C,C,BETA)     >*/
	rhs[4] = -h__ * h__ * (2. * c__ * (c__ - 1.) + c__ * (c__ - 1.) * (2. 
		- beta * (1. - 2. * c__)));
/*<          RHS(6) = -H*H*F(D,C,BETA)     >*/
	rhs[5] = -h__ * h__ * (2. * d__ * (d__ - 1.) + c__ * (c__ - 1.) * (2. 
		- beta * (1. - 2. * d__)));
/*<          RHS(7) = -H*H*F(B,D,BETA)     >*/
	rhs[6] = -h__ * h__ * (2. * b * (b - 1.) + d__ * (d__ - 1.) * (2. - 
		beta * (1. - 2. * b)));
/*<          RHS(8) = -H*H*F(C,D,BETA)     >*/
	rhs[7] = -h__ * h__ * (2. * c__ * (c__ - 1.) + d__ * (d__ - 1.) * (2. 
		- beta * (1. - 2. * c__)));
/*<          RHS(9) = -H*H*F(D,D,BETA)     >*/
	rhs[8] = -h__ * h__ * (2. * d__ * (d__ - 1.) + d__ * (d__ - 1.) * (2. 
		- beta * (1. - 2. * d__)));
/*<          IBGN = 2  >*/
	ibgn = 2;
/*<          IF (K.EQ.2) IBGN = 1  >*/
	if (k == 2) {
	    ibgn = 1;
	}
/*<          DO 110 I = IBGN,2   >*/
	for (i__ = ibgn; i__ <= 2; ++i__) {
/*<             WRITE (NOUT,20)  >*/
	    io___317.ciunit = nout;
	    s_wsfe(&io___317);
	    e_wsfe();
/*<             WRITE (NOUT,30) BETA       >*/
	    io___318.ciunit = nout;
	    s_wsfe(&io___318);
	    do_fio(&c__1, (char *)&beta, (ftnlen)sizeof(doublereal));
	    e_wsfe();
/*<    >*/
/*<    30       FORMAT (5X,' BETA =',F10.5)  >*/
/*<             ISYM = I-1       >*/
	    isym = i__ - 1;
/*<             IF (ISYM.EQ.0) CALL SSEXP (IA,JA,A,NOUT)       >*/
	    if (isym == 0) {
		ssexp_(ia, ja, a, &nout);
	    }
/*<             IF (ISYM.NE.0) CALL NSEXP (IA,JA,A,BETA,H,NOUT)  >*/
	    if (isym != 0) {
		nsexp_(ia, ja, a, &beta, &h__, &nout);
	    }

/*        TEST JCG */

/*<             DO 40 KK = 1,2   >*/
	    for (kk = 1; kk <= 2; ++kk) {
/*<                IADAPT = KK-1 >*/
		iadapt = kk - 1;
/*<                CALL DFAULT (IPARM,RPARM) >*/
		dfault_(iparm, rparm);
/*<                IPARM(1) = 12 >*/
		iparm[0] = 12;
/*<                IPARM(2) = LEVEL        >*/
		iparm[1] = level;
/*<                IF (IADAPT.EQ.0) IPARM(2) = 3     >*/
		if (iadapt == 0) {
		    iparm[1] = 3;
		}
/*<                IPARM(5) = ISYM >*/
		iparm[4] = isym;
/*<                IPARM(6) = IADAPT       >*/
		iparm[5] = iadapt;
/*<                IPARM(12) = IDGTS       >*/
		iparm[11] = idgts;
/*<                RPARM(1) = 1.D-20       >*/
		rparm[0] = 1e-20;
/*<                RPARM(2) = .75D0        >*/
		rparm[1] = .75;
/*<                RPARM(3) = -.65D0       >*/
		rparm[2] = -.65;
/*<                CALL VFILL (N,U,5.D-2)  >*/
		vfill_(&n, u, &c_b445);
/*<                CALL JCG (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER)  >*/
		jcg_(&n, ia, ja, a, rhs, u, iwksp, &nw, wksp, iparm, rparm, &
			ier);
/*<    40       CONTINUE >*/
/* L40: */
	    }

/*        TEST JSI */

/*<             DO 50 ICASE = 1,2  >*/
	    for (icase = 1; icase <= 2; ++icase) {
/*<                DO 50 KK = 1,2  >*/
		for (kk = 1; kk <= 2; ++kk) {
/*<                   IADAPT = KK-1        >*/
		    iadapt = kk - 1;
/*<                   CALL DFAULT (IPARM,RPARM)      >*/
		    dfault_(iparm, rparm);
/*<                   IPARM(1) = 27        >*/
		    iparm[0] = 27;
/*<                   IPARM(2) = LEVEL     >*/
		    iparm[1] = level;
/*<                   IPARM(5) = ISYM      >*/
		    iparm[4] = isym;
/*<                   IPARM(6) = IADAPT    >*/
		    iparm[5] = iadapt;
/*<                   IPARM(7) = ICASE     >*/
		    iparm[6] = icase;
/*<                   IPARM(12) = IDGTS    >*/
		    iparm[11] = idgts;
/*<                   RPARM(1) = 1.D-1     >*/
		    rparm[0] = .1;
/*<                   RPARM(2) = .75D0     >*/
		    rparm[1] = .75;
/*<                   RPARM(3) = -.65D0    >*/
		    rparm[2] = -.65;
/*<                   RPARM(4) = 1.D0      >*/
		    rparm[3] = 1.;
/*<                   CALL VFILL (N,U,1.D0)  >*/
		    vfill_(&n, u, &c_b27);
/*<    >*/
		    jsi_(&n, ia, ja, a, rhs, u, iwksp, &nw, wksp, iparm, 
			    rparm, &ier);
/*<    50       CONTINUE >*/
/* L50: */
		}
	    }

/*        TEST SOR */

/*<             DO 60 KK = 1,2   >*/
	    for (kk = 1; kk <= 2; ++kk) {
/*<                IADAPT = KK-1 >*/
		iadapt = kk - 1;
/*<                CALL DFAULT (IPARM,RPARM) >*/
		dfault_(iparm, rparm);
/*<                IPARM(1) = 10 >*/
		iparm[0] = 10;
/*<                IPARM(2) = LEVEL        >*/
		iparm[1] = level;
/*<                IPARM(5) = ISYM >*/
		iparm[4] = isym;
/*<                IPARM(6) = IADAPT       >*/
		iparm[5] = iadapt;
/*<                IPARM(7) = 2  >*/
		iparm[6] = 2;
/*<                IPARM(12) = IDGTS       >*/
		iparm[11] = idgts;
/*<                RPARM(1) = 1.D-3        >*/
		rparm[0] = .001;
/*<                RPARM(2) = .71D0        >*/
		rparm[1] = .71;
/*<                RPARM(3) = -.89D0       >*/
		rparm[2] = -.89;
/*<                RPARM(5) = 1.2D0        >*/
		rparm[4] = 1.2;
/*<                CALL VFILL (N,U,0.D0)   >*/
		vfill_(&n, u, &c_b19);
/*<                CALL SOR (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER)  >*/
		sor_(&n, ia, ja, a, rhs, u, iwksp, &nw, wksp, iparm, rparm, &
			ier);
/*<    60       CONTINUE >*/
/* L60: */
	    }

/*        TEST SSORCG */

/*<             DO 70 ICASE = 1,2  >*/
	    for (icase = 1; icase <= 2; ++icase) {
/*<                DO 70 KK = 1,4  >*/
		for (kk = 1; kk <= 4; ++kk) {
/*<                   IADAPT = KK-1        >*/
		    iadapt = kk - 1;
/*<                   CALL DFAULT (IPARM,RPARM)      >*/
		    dfault_(iparm, rparm);
/*<                   IPARM(1) = 4 >*/
		    iparm[0] = 4;
/*<                   IPARM(2) = LEVEL     >*/
		    iparm[1] = level;
/*<                   IPARM(5) = ISYM      >*/
		    iparm[4] = isym;
/*<                   IPARM(6) = IADAPT    >*/
		    iparm[5] = iadapt;
/*<                   IPARM(7) = ICASE     >*/
		    iparm[6] = icase;
/*<                   IPARM(12) = IDGTS    >*/
		    iparm[11] = idgts;
/*<                   RPARM(1) = 1.0D-6    >*/
		    rparm[0] = 1e-6;
/*<                   RPARM(2) = .8D0      >*/
		    rparm[1] = .8;
/*<                   RPARM(3) = -.25D0    >*/
		    rparm[2] = -.25;
/*<                   RPARM(5) = 1.2D0     >*/
		    rparm[4] = 1.2;
/*<                   RPARM(6) = .9D0      >*/
		    rparm[5] = .9;
/*<                   RPARM(7) = .26D0     >*/
		    rparm[6] = .26;
/*<                   CALL VFILL (N,U,0.D0)  >*/
		    vfill_(&n, u, &c_b19);
/*<    >*/
		    ssorcg_(&n, ia, ja, a, rhs, u, iwksp, &nw, wksp, iparm, 
			    rparm, &ier);
/*<    70       CONTINUE >*/
/* L70: */
		}
	    }

/*        TEST SSORSI */

/*<             DO 80 ICASE = 1,2  >*/
	    for (icase = 1; icase <= 2; ++icase) {
/*<                DO 80 KK = 1,4  >*/
		for (kk = 1; kk <= 4; ++kk) {
/*<                   IADAPT = KK-1        >*/
		    iadapt = kk - 1;
/*<                   CALL DFAULT (IPARM,RPARM)      >*/
		    dfault_(iparm, rparm);
/*<                   IPARM(1) = 8 >*/
		    iparm[0] = 8;
/*<                   IPARM(2) = LEVEL     >*/
		    iparm[1] = level;
/*<                   IPARM(5) = ISYM      >*/
		    iparm[4] = isym;
/*<                   IPARM(6) = IADAPT    >*/
		    iparm[5] = iadapt;
/*<                   IPARM(7) = ICASE     >*/
		    iparm[6] = icase;
/*<                   IPARM(12) = IDGTS    >*/
		    iparm[11] = idgts;
/*<                   RPARM(2) = .22D0     >*/
		    rparm[1] = .22;
/*<                   RPARM(5) = 1.25D0    >*/
		    rparm[4] = 1.25;
/*<                   RPARM(6) = .25D0     >*/
		    rparm[5] = .25;
/*<                   RPARM(7) = .24D0     >*/
		    rparm[6] = .24;
/*<                   CALL VFILL (N,U,0.D0)  >*/
		    vfill_(&n, u, &c_b19);
/*<    >*/
		    ssorsi_(&n, ia, ja, a, rhs, u, iwksp, &nw, wksp, iparm, 
			    rparm, &ier);
/*<    80       CONTINUE >*/
/* L80: */
		}
	    }

/*        TEST RSCG */

/*<             DO 90 KK = 1,2   >*/
	    for (kk = 1; kk <= 2; ++kk) {
/*<                IADAPT = KK-1 >*/
		iadapt = kk - 1;
/*<                CALL DFAULT (IPARM,RPARM) >*/
		dfault_(iparm, rparm);
/*<                IPARM(1) = 4  >*/
		iparm[0] = 4;
/*<                IPARM(2) = LEVEL        >*/
		iparm[1] = level;
/*<                IPARM(5) = ISYM >*/
		iparm[4] = isym;
/*<                IPARM(6) = IADAPT       >*/
		iparm[5] = iadapt;
/*<                IPARM(12) = IDGTS       >*/
		iparm[11] = idgts;
/*<                IF (IADAPT.EQ.0) IPARM(12) = 4    >*/
		if (iadapt == 0) {
		    iparm[11] = 4;
		}
/*<                RPARM(1) = 1.0D-20      >*/
		rparm[0] = 1e-20;
/*<                RPARM(2) = .85D0        >*/
		rparm[1] = .85;
/*<                RPARM(3) = -.5D0        >*/
		rparm[2] = -.5;
/*<                RPARM(4) = .5D0 >*/
		rparm[3] = .5;
/*<                CALL VFILL (N,U,5.D-2)  >*/
		vfill_(&n, u, &c_b445);
/*<                CALL RSCG (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER) >*/
		rscg_(&n, ia, ja, a, rhs, u, iwksp, &nw, wksp, iparm, rparm, &
			ier);
/*<    90       CONTINUE >*/
/* L90: */
	    }

/*        TEST RSSI */

/*<             DO 100 KK = 1,2  >*/
	    for (kk = 1; kk <= 2; ++kk) {
/*<                IADAPT = KK-1 >*/
		iadapt = kk - 1;
/*<                CALL DFAULT (IPARM,RPARM) >*/
		dfault_(iparm, rparm);
/*<                IPARM(1) = 20 >*/
		iparm[0] = 20;
/*<                IPARM(2) = LEVEL        >*/
		iparm[1] = level;
/*<                IPARM(5) = ISYM >*/
		iparm[4] = isym;
/*<                IPARM(6) = IADAPT       >*/
		iparm[5] = iadapt;
/*<                IPARM(12) = IDGTS       >*/
		iparm[11] = idgts;
/*<                RPARM(2) = .9D0 >*/
		rparm[1] = .9;
/*<                RPARM(3) = -.75D0       >*/
		rparm[2] = -.75;
/*<                CALL VFILL (N,U,0.D0)   >*/
		vfill_(&n, u, &c_b19);
/*<                CALL RSSI (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER) >*/
		rssi_(&n, ia, ja, a, rhs, u, iwksp, &nw, wksp, iparm, rparm, &
			ier);
/*<   100       CONTINUE >*/
/* L100: */
	    }

/*<   110 CONTINUE     >*/
/* L110: */
	}
    }

/*<       RETURN       >*/
    return 0;
/*<       END  >*/
} /* test4d_ */

/*<       SUBROUTINE SSEXP (IA,JA,A,NOUT)  >*/
/* Subroutine */ int ssexp_(integer *ia, integer *ja, doublereal *a, integer *
	nout)
{
    /* Format strings */
    static char fmt_10[] = "(\0020 SYMMETRIC STORAGE USED\002)";

    /* Builtin functions */
    integer s_wsfe(cilist *), e_wsfe();

    /* Fortran I/O blocks */
    static cilist io___332 = { 0, 0, 0, fmt_10, 0 };


/*<       INTEGER IA(10),JA(33)  >*/
/*<       DOUBLE PRECISION A(33) >*/
/*<       WRITE (NOUT,10)        >*/
    /* Parameter adjustments */
    --a;
    --ja;
    --ia;

    /* Function Body */
    io___332.ciunit = *nout;
    s_wsfe(&io___332);
    e_wsfe();
/*<    10 FORMAT ('0 SYMMETRIC STORAGE USED')        >*/
/*<       A(21) = 4.0D0  >*/
    a[21] = 4.;
/*<       A(19) = A(21)  >*/
    a[19] = a[21];
/*<       A(17) = A(19)  >*/
    a[17] = a[19];
/*<       A(15) = A(17)  >*/
    a[15] = a[17];
/*<       A(12) = A(15)  >*/
    a[12] = a[15];
/*<       A(9) = A(12) >*/
    a[9] = a[12];
/*<       A(7) = A(9)  >*/
    a[7] = a[9];
/*<       A(4) = A(7)  >*/
    a[4] = a[7];
/*<       A(1) = A(4)  >*/
    a[1] = a[4];
/*<       A(20) = -1.0D0 >*/
    a[20] = -1.;
/*<       A(18) = A(20)  >*/
    a[18] = a[20];
/*<       A(16) = A(18)  >*/
    a[16] = a[18];
/*<       A(14) = A(16)  >*/
    a[14] = a[16];
/*<       A(13) = A(14)  >*/
    a[13] = a[14];
/*<       A(11) = A(13)  >*/
    a[11] = a[13];
/*<       A(10) = A(11)  >*/
    a[10] = a[11];
/*<       A(8) = A(10) >*/
    a[8] = a[10];
/*<       A(6) = A(8)  >*/
    a[6] = a[8];
/*<       A(5) = A(6)  >*/
    a[5] = a[6];
/*<       A(3) = A(5)  >*/
    a[3] = a[5];
/*<       A(2) = A(3)  >*/
    a[2] = a[3];
/*<       JA(1) = 1    >*/
    ja[1] = 1;
/*<       JA(2) = 2    >*/
    ja[2] = 2;
/*<       JA(3) = 4    >*/
    ja[3] = 4;
/*<       JA(4) = 2    >*/
    ja[4] = 2;
/*<       JA(5) = 3    >*/
    ja[5] = 3;
/*<       JA(6) = 5    >*/
    ja[6] = 5;
/*<       JA(7) = 3    >*/
    ja[7] = 3;
/*<       JA(8) = 6    >*/
    ja[8] = 6;
/*<       JA(9) = 4    >*/
    ja[9] = 4;
/*<       JA(10) = 5   >*/
    ja[10] = 5;
/*<       JA(11) = 7   >*/
    ja[11] = 7;
/*<       JA(12) = 5   >*/
    ja[12] = 5;
/*<       JA(13) = 6   >*/
    ja[13] = 6;
/*<       JA(14) = 8   >*/
    ja[14] = 8;
/*<       JA(15) = 6   >*/
    ja[15] = 6;
/*<       JA(16) = 9   >*/
    ja[16] = 9;
/*<       JA(17) = 7   >*/
    ja[17] = 7;
/*<       JA(18) = 8   >*/
    ja[18] = 8;
/*<       JA(19) = 8   >*/
    ja[19] = 8;
/*<       JA(20) = 9   >*/
    ja[20] = 9;
/*<       JA(21) = 9   >*/
    ja[21] = 9;
/*<       IA(1) = 1    >*/
    ia[1] = 1;
/*<       IA(2) = 4    >*/
    ia[2] = 4;
/*<       IA(3) = 7    >*/
    ia[3] = 7;
/*<       IA(4) = 9    >*/
    ia[4] = 9;
/*<       IA(5) = 12   >*/
    ia[5] = 12;
/*<       IA(6) = 15   >*/
    ia[6] = 15;
/*<       IA(7) = 17   >*/
    ia[7] = 17;
/*<       IA(8) = 19   >*/
    ia[8] = 19;
/*<       IA(9) = 21   >*/
    ia[9] = 21;
/*<       IA(10) = 22  >*/
    ia[10] = 22;
/*<       RETURN       >*/
    return 0;

/*<       END  >*/
} /* ssexp_ */

/*<       SUBROUTINE NSEXP (IA,JA,A,BETA,H,NOUT)     >*/
/* Subroutine */ int nsexp_(integer *ia, integer *ja, doublereal *a, 
	doublereal *beta, doublereal *h__, integer *nout)
{
    /* Format strings */
    static char fmt_10[] = "(\0020 NONSYMMETRIC STORAGE USED\002)";

    /* Builtin functions */
    integer s_wsfe(cilist *), e_wsfe();

    /* Fortran I/O blocks */
    static cilist io___333 = { 0, 0, 0, fmt_10, 0 };


/*<       INTEGER IA(10),JA(33)  >*/
/*<       DOUBLE PRECISION A(33),BETA,H    >*/
/*<       WRITE (NOUT,10)        >*/
    /* Parameter adjustments */
    --a;
    --ja;
    --ia;

    /* Function Body */
    io___333.ciunit = *nout;
    s_wsfe(&io___333);
    e_wsfe();
/*<    10 FORMAT ('0 NONSYMMETRIC STORAGE USED')     >*/
/*<       JA(1) = 1    >*/
    ja[1] = 1;
/*<       JA(2) = 2    >*/
    ja[2] = 2;
/*<       JA(3) = 4    >*/
    ja[3] = 4;
/*<       JA(4) = 1    >*/
    ja[4] = 1;
/*<       JA(5) = 2    >*/
    ja[5] = 2;
/*<       JA(6) = 3    >*/
    ja[6] = 3;
/*<       JA(7) = 5    >*/
    ja[7] = 5;
/*<       JA(8) = 2    >*/
    ja[8] = 2;
/*<       JA(9) = 3    >*/
    ja[9] = 3;
/*<       JA(10) = 6   >*/
    ja[10] = 6;
/*<       JA(11) = 1   >*/
    ja[11] = 1;
/*<       JA(12) = 4   >*/
    ja[12] = 4;
/*<       JA(13) = 5   >*/
    ja[13] = 5;
/*<       JA(14) = 7   >*/
    ja[14] = 7;
/*<       JA(15) = 2   >*/
    ja[15] = 2;
/*<       JA(16) = 4   >*/
    ja[16] = 4;
/*<       JA(17) = 5   >*/
    ja[17] = 5;
/*<       JA(18) = 6   >*/
    ja[18] = 6;
/*<       JA(19) = 8   >*/
    ja[19] = 8;
/*<       JA(20) = 3   >*/
    ja[20] = 3;
/*<       JA(21) = 5   >*/
    ja[21] = 5;
/*<       JA(22) = 6   >*/
    ja[22] = 6;
/*<       JA(23) = 9   >*/
    ja[23] = 9;
/*<       JA(24) = 4   >*/
    ja[24] = 4;
/*<       JA(25) = 7   >*/
    ja[25] = 7;
/*<       JA(26) = 8   >*/
    ja[26] = 8;
/*<       JA(27) = 5   >*/
    ja[27] = 5;
/*<       JA(28) = 7   >*/
    ja[28] = 7;
/*<       JA(29) = 8   >*/
    ja[29] = 8;
/*<       JA(30) = 9   >*/
    ja[30] = 9;
/*<       JA(31) = 6   >*/
    ja[31] = 6;
/*<       JA(32) = 8   >*/
    ja[32] = 8;
/*<       JA(33) = 9   >*/
    ja[33] = 9;
/*<       IA(1) = 1    >*/
    ia[1] = 1;
/*<       IA(2) = 4    >*/
    ia[2] = 4;
/*<       IA(3) = 8    >*/
    ia[3] = 8;
/*<       IA(4) = 11   >*/
    ia[4] = 11;
/*<       IA(5) = 15   >*/
    ia[5] = 15;
/*<       IA(6) = 20   >*/
    ia[6] = 20;
/*<       IA(7) = 24   >*/
    ia[7] = 24;
/*<       IA(8) = 27   >*/
    ia[8] = 27;
/*<       IA(9) = 31   >*/
    ia[9] = 31;
/*<       IA(10) = 34  >*/
    ia[10] = 34;
/*<       A(33) = 4.0D0  >*/
    a[33] = 4.;
/*<       A(29) = A(33)  >*/
    a[29] = a[33];
/*<       A(25) = A(29)  >*/
    a[25] = a[29];
/*<       A(22) = A(25)  >*/
    a[22] = a[25];
/*<       A(17) = A(22)  >*/
    a[17] = a[22];
/*<       A(12) = A(17)  >*/
    a[12] = a[17];
/*<       A(9) = A(12) >*/
    a[9] = a[12];
/*<       A(5) = A(9)  >*/
    a[5] = a[9];
/*<       A(1) = A(5)  >*/
    a[1] = a[5];
/*<       A(31) = -1.0D0 >*/
    a[31] = -1.;
/*<       A(27) = A(31)  >*/
    a[27] = a[31];
/*<       A(24) = A(27)  >*/
    a[24] = a[27];
/*<       A(23) = A(24)  >*/
    a[23] = a[24];
/*<       A(20) = A(23)  >*/
    a[20] = a[23];
/*<       A(19) = A(20)  >*/
    a[19] = a[20];
/*<       A(15) = A(19)  >*/
    a[15] = a[19];
/*<       A(14) = A(15)  >*/
    a[14] = a[15];
/*<       A(11) = A(14)  >*/
    a[11] = a[14];
/*<       A(10) = A(11)  >*/
    a[10] = a[11];
/*<       A(7) = A(10) >*/
    a[7] = a[10];
/*<       A(3) = A(7)  >*/
    a[3] = a[7];
/*<       A(30) = -(1.0D0+0.5D0*H*BETA)    >*/
    a[30] = -(*h__ * .5 * *beta + 1.);
/*<       A(26) = A(30)  >*/
    a[26] = a[30];
/*<       A(18) = A(26)  >*/
    a[18] = a[26];
/*<       A(13) = A(18)  >*/
    a[13] = a[18];
/*<       A(6) = A(13) >*/
    a[6] = a[13];
/*<       A(2) = A(6)  >*/
    a[2] = a[6];
/*<       A(32) = -(1.0D0-0.5D0*H*BETA)    >*/
    a[32] = -(1. - *h__ * .5 * *beta);
/*<       A(28) = A(32)  >*/
    a[28] = a[32];
/*<       A(21) = A(28)  >*/
    a[21] = a[28];
/*<       A(16) = A(21)  >*/
    a[16] = a[21];
/*<       A(8) = A(16) >*/
    a[8] = a[16];
/*<       A(4) = A(8)  >*/
    a[4] = a[8];
/*<       RETURN       >*/
    return 0;

/*<       END  >*/
} /* nsexp_ */

/* Main program alias */ int itptst_ () { MAIN__ (); return 0; }
#ifdef __cplusplus
	}
#endif
