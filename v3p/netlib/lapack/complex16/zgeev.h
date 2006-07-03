extern int v3p_netlib_zgeev_(
  char *jobvl,
  char *jobvr,
  v3p_netlib_integer *n,
  v3p_netlib_doublecomplex *a,
  v3p_netlib_integer *lda,
  v3p_netlib_doublecomplex *w,
  v3p_netlib_doublecomplex *vl,
  v3p_netlib_integer *ldvl,
  v3p_netlib_doublecomplex *vr,
  v3p_netlib_integer *ldvr,
  v3p_netlib_doublecomplex *work,
  v3p_netlib_integer *lwork,
  v3p_netlib_doublereal *rwork,
  v3p_netlib_integer *info,
  v3p_netlib_ftnlen jobvl_len,
  v3p_netlib_ftnlen jobvr_len
  );
