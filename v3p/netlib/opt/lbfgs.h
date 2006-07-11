typedef struct v3p_netlib_lbfgs_global_s v3p_netlib_lbfgs_global_t;

/*: Solves the unconstrained minimization problem min F(x1..xN) */
extern int v3p_netlib_lbfgs_(
  v3p_netlib_integer *n,
  v3p_netlib_integer *m,
  v3p_netlib_doublereal *x,
  v3p_netlib_doublereal *f,
  v3p_netlib_doublereal *g,
  v3p_netlib_logical *diagco,
  v3p_netlib_doublereal *diag,
  v3p_netlib_integer *iprint,
  v3p_netlib_doublereal *eps,
  v3p_netlib_doublereal *xtol,
  v3p_netlib_doublereal *w,
  v3p_netlib_integer *iflag,
  v3p_netlib_lbfgs_global_t* v3p_netlib_lbfgs_global_arg
  );
extern int v3p_netlib_lb1_(
  v3p_netlib_integer *iprint,
  v3p_netlib_integer *iter,
  v3p_netlib_integer *nfun,
  v3p_netlib_doublereal *gnorm,
  v3p_netlib_integer *n,
  v3p_netlib_integer *m,
  v3p_netlib_doublereal *x,
  v3p_netlib_doublereal *f,
  v3p_netlib_doublereal *g,
  v3p_netlib_doublereal *stp,
  v3p_netlib_logical *finish
  );
extern int v3p_netlib_mcsrch_(
  v3p_netlib_integer *n,
  v3p_netlib_doublereal *x,
  v3p_netlib_doublereal *f,
  v3p_netlib_doublereal *g,
  v3p_netlib_doublereal *s,
  v3p_netlib_doublereal *stp,
  v3p_netlib_doublereal *ftol,
  v3p_netlib_doublereal *xtol,
  v3p_netlib_integer *maxfev,
  v3p_netlib_integer *info,
  v3p_netlib_integer *nfev,
  v3p_netlib_doublereal *wa,
  v3p_netlib_lbfgs_global_t* v3p_netlib_lbfgs_global_arg
  );
extern int v3p_netlib_mcstep_(
  v3p_netlib_doublereal *stx,
  v3p_netlib_doublereal *fx,
  v3p_netlib_doublereal *dx,
  v3p_netlib_doublereal *sty,
  v3p_netlib_doublereal *fy,
  v3p_netlib_doublereal *dy,
  v3p_netlib_doublereal *stp,
  v3p_netlib_doublereal *fp,
  v3p_netlib_doublereal *dp,
  v3p_netlib_logical *brackt,
  v3p_netlib_doublereal *stpmin,
  v3p_netlib_doublereal *stpmax,
  v3p_netlib_integer *info
  );
extern void v3p_netlib_lbfgs_init(
  v3p_netlib_lbfgs_global_t* v3p_netlib_lbfgs_global_arg
  );

/*
C    GTOL is a DOUBLE PRECISION variable with default value 0.9, which
C        controls the accuracy of the line search routine MCSRCH. If the
C        function and gradient evaluations are inexpensive with respect
C        to the cost of the iteration (which is sometimes the case when
C        solving very large problems) it may be advantageous to set GTOL
C        to a small value. A typical small value is 0.1.  Restriction:
C        GTOL should be greater than 1.D-04.
C
C    STPMIN and STPMAX are non-negative DOUBLE PRECISION variables which
C        specify lower and upper bounds for the step in the line search.
C        Their default values are 1.D-20 and 1.D+20, respectively. These
C        values need not be modified unless the exponents are too large
C        for the machine being used, or unless the problem is extremely
C        badly scaled (in which case the exponents should be increased).
*/
struct v3p_netlib_lbfgs_global_s
{
  v3p_netlib_integer mp, lp;
  v3p_netlib_doublereal gtol, stpmin, stpmax;
  v3p_netlib_doublereal stpinit; /* line search default step length, added by awf */
};
