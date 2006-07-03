extern int v3p_netlib_cg_(
  v3p_netlib_doublereal *x,
  v3p_netlib_doublereal *e,
  v3p_netlib_integer *it,
  v3p_netlib_doublereal *step,
  v3p_netlib_doublereal *t,
  v3p_netlib_integer *limit,
  v3p_netlib_integer *n,
  v3p_netlib_integer *m,
  D_fp value,
  S_fp grad,
  S_fp both,
  S_fp pre,
  v3p_netlib_doublereal *h__
  );
extern v3p_netlib_doublereal v3p_netlib_fv_(
  v3p_netlib_doublereal *a,
  v3p_netlib_doublereal *x,
  v3p_netlib_doublereal *h__,
  v3p_netlib_integer *n,
  D_fp value
  );
extern v3p_netlib_doublereal v3p_netlib_fd_(
  v3p_netlib_doublereal *a,
  v3p_netlib_doublereal *x,
  v3p_netlib_doublereal *h__,
  v3p_netlib_integer *n,
  S_fp grad
  );
extern int v3p_netlib_fvd_(
  v3p_netlib_doublereal *v,
  v3p_netlib_doublereal *d__,
  v3p_netlib_doublereal *a,
  v3p_netlib_doublereal *x,
  v3p_netlib_doublereal *h__,
  v3p_netlib_integer *n,
  S_fp both
  );
extern int v3p_netlib_cub_(
  v3p_netlib_doublereal *x,
  v3p_netlib_doublereal *a,
  v3p_netlib_doublereal *b,
  v3p_netlib_doublereal *c__,
  v3p_netlib_doublereal *d__,
  v3p_netlib_doublereal *e,
  v3p_netlib_doublereal *f
  );
extern int v3p_netlib_ins_(
  v3p_netlib_doublereal *s,
  v3p_netlib_doublereal *f,
  v3p_netlib_doublereal *a,
  v3p_netlib_doublereal *b,
  v3p_netlib_doublereal *c__,
  v3p_netlib_doublereal *fa,
  v3p_netlib_doublereal *fb,
  v3p_netlib_doublereal *fc,
  v3p_netlib_integer *j,
  v3p_netlib_doublereal *y,
  v3p_netlib_doublereal *z__
  );
