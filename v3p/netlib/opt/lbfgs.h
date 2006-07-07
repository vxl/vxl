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
  v3p_netlib_integer *iflag
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
extern int v3p_netlib_lb2_(
  
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
  v3p_netlib_doublereal *wa
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
