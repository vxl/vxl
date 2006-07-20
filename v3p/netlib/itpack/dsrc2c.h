extern int v3p_netlib_jcg_(
  v3p_netlib_integer *nn,
  v3p_netlib_integer *ia,
  v3p_netlib_integer *ja,
  v3p_netlib_doublereal *a,
  v3p_netlib_doublereal *rhs,
  v3p_netlib_doublereal *u,
  v3p_netlib_integer *iwksp,
  v3p_netlib_integer *nw,
  v3p_netlib_doublereal *wksp,
  v3p_netlib_integer *iparm,
  v3p_netlib_doublereal *rparm,
  v3p_netlib_integer *ierr
  );
extern int v3p_netlib_jsi_(
  v3p_netlib_integer *nn,
  v3p_netlib_integer *ia,
  v3p_netlib_integer *ja,
  v3p_netlib_doublereal *a,
  v3p_netlib_doublereal *rhs,
  v3p_netlib_doublereal *u,
  v3p_netlib_integer *iwksp,
  v3p_netlib_integer *nw,
  v3p_netlib_doublereal *wksp,
  v3p_netlib_integer *iparm,
  v3p_netlib_doublereal *rparm,
  v3p_netlib_integer *ierr
  );
extern int v3p_netlib_sor_(
  v3p_netlib_integer *nn,
  v3p_netlib_integer *ia,
  v3p_netlib_integer *ja,
  v3p_netlib_doublereal *a,
  v3p_netlib_doublereal *rhs,
  v3p_netlib_doublereal *u,
  v3p_netlib_integer *iwksp,
  v3p_netlib_integer *nw,
  v3p_netlib_doublereal *wksp,
  v3p_netlib_integer *iparm,
  v3p_netlib_doublereal *rparm,
  v3p_netlib_integer *ierr
  );
extern int v3p_netlib_ssorcg_(
  v3p_netlib_integer *nn,
  v3p_netlib_integer *ia,
  v3p_netlib_integer *ja,
  v3p_netlib_doublereal *a,
  v3p_netlib_doublereal *rhs,
  v3p_netlib_doublereal *u,
  v3p_netlib_integer *iwksp,
  v3p_netlib_integer *nw,
  v3p_netlib_doublereal *wksp,
  v3p_netlib_integer *iparm,
  v3p_netlib_doublereal *rparm,
  v3p_netlib_integer *ierr
  );
extern int v3p_netlib_ssorsi_(
  v3p_netlib_integer *nn,
  v3p_netlib_integer *ia,
  v3p_netlib_integer *ja,
  v3p_netlib_doublereal *a,
  v3p_netlib_doublereal *rhs,
  v3p_netlib_doublereal *u,
  v3p_netlib_integer *iwksp,
  v3p_netlib_integer *nw,
  v3p_netlib_doublereal *wksp,
  v3p_netlib_integer *iparm,
  v3p_netlib_doublereal *rparm,
  v3p_netlib_integer *ierr
  );
extern int v3p_netlib_rscg_(
  v3p_netlib_integer *nn,
  v3p_netlib_integer *ia,
  v3p_netlib_integer *ja,
  v3p_netlib_doublereal *a,
  v3p_netlib_doublereal *rhs,
  v3p_netlib_doublereal *u,
  v3p_netlib_integer *iwksp,
  v3p_netlib_integer *nw,
  v3p_netlib_doublereal *wksp,
  v3p_netlib_integer *iparm,
  v3p_netlib_doublereal *rparm,
  v3p_netlib_integer *ierr
  );
extern int v3p_netlib_rssi_(
  v3p_netlib_integer *nn,
  v3p_netlib_integer *ia,
  v3p_netlib_integer *ja,
  v3p_netlib_doublereal *a,
  v3p_netlib_doublereal *rhs,
  v3p_netlib_doublereal *u,
  v3p_netlib_integer *iwksp,
  v3p_netlib_integer *nw,
  v3p_netlib_doublereal *wksp,
  v3p_netlib_integer *iparm,
  v3p_netlib_doublereal *rparm,
  v3p_netlib_integer *ierr
  );
extern int v3p_netlib_itjcg_(
  v3p_netlib_integer *nn,
  v3p_netlib_integer *ia,
  v3p_netlib_integer *ja,
  v3p_netlib_doublereal *a,
  v3p_netlib_doublereal *u,
  v3p_netlib_doublereal *u1,
  v3p_netlib_doublereal *d__,
  v3p_netlib_doublereal *d1,
  v3p_netlib_doublereal *dtwd,
  v3p_netlib_doublereal *tri
  );
extern int v3p_netlib_itjsi_(
  v3p_netlib_integer *nn,
  v3p_netlib_integer *ia,
  v3p_netlib_integer *ja,
  v3p_netlib_doublereal *a,
  v3p_netlib_doublereal *rhs,
  v3p_netlib_doublereal *u,
  v3p_netlib_doublereal *u1,
  v3p_netlib_doublereal *d__,
  v3p_netlib_integer *icnt
  );
extern int v3p_netlib_itsor_(
  v3p_netlib_integer *nn,
  v3p_netlib_integer *ia,
  v3p_netlib_integer *ja,
  v3p_netlib_doublereal *a,
  v3p_netlib_doublereal *rhs,
  v3p_netlib_doublereal *u,
  v3p_netlib_doublereal *wk
  );
extern int v3p_netlib_itsrcg_(
  v3p_netlib_integer *nn,
  v3p_netlib_integer *ia,
  v3p_netlib_integer *ja,
  v3p_netlib_doublereal *a,
  v3p_netlib_doublereal *rhs,
  v3p_netlib_doublereal *u,
  v3p_netlib_doublereal *u1,
  v3p_netlib_doublereal *c__,
  v3p_netlib_doublereal *c1,
  v3p_netlib_doublereal *d__,
  v3p_netlib_doublereal *dl,
  v3p_netlib_doublereal *wk,
  v3p_netlib_doublereal *tri
  );
extern int v3p_netlib_itsrsi_(
  v3p_netlib_integer *nn,
  v3p_netlib_integer *ia,
  v3p_netlib_integer *ja,
  v3p_netlib_doublereal *a,
  v3p_netlib_doublereal *rhs,
  v3p_netlib_doublereal *u,
  v3p_netlib_doublereal *u1,
  v3p_netlib_doublereal *c__,
  v3p_netlib_doublereal *d__,
  v3p_netlib_doublereal *ctwd,
  v3p_netlib_doublereal *wk
  );
extern int v3p_netlib_itrscg_(
  v3p_netlib_integer *n,
  v3p_netlib_integer *nnb,
  v3p_netlib_integer *ia,
  v3p_netlib_integer *ja,
  v3p_netlib_doublereal *a,
  v3p_netlib_doublereal *ub,
  v3p_netlib_doublereal *ub1,
  v3p_netlib_doublereal *db,
  v3p_netlib_doublereal *db1,
  v3p_netlib_doublereal *wb,
  v3p_netlib_doublereal *tri
  );
extern int v3p_netlib_itrssi_(
  v3p_netlib_integer *n,
  v3p_netlib_integer *nnb,
  v3p_netlib_integer *ia,
  v3p_netlib_integer *ja,
  v3p_netlib_doublereal *a,
  v3p_netlib_doublereal *rhs,
  v3p_netlib_doublereal *ub,
  v3p_netlib_doublereal *ub1,
  v3p_netlib_doublereal *db
  );
extern v3p_netlib_integer v3p_netlib_bisrch_(
  v3p_netlib_integer *n,
  v3p_netlib_integer *k,
  v3p_netlib_integer *l
  );
extern v3p_netlib_doublereal v3p_netlib_cheby_(
  v3p_netlib_doublereal *qa,
  v3p_netlib_doublereal *qt,
  v3p_netlib_doublereal *rrr,
  v3p_netlib_integer *ip,
  v3p_netlib_doublereal *cme,
  v3p_netlib_doublereal *sme
  );
extern int v3p_netlib_chgcon_(
  v3p_netlib_doublereal *tri,
  v3p_netlib_doublereal *gamold,
  v3p_netlib_doublereal *rhoold,
  v3p_netlib_integer *ibmth
  );
extern int v3p_netlib_chgsi_(
  v3p_netlib_doublereal *dtnrm,
  v3p_netlib_integer *ibmth
  );
extern v3p_netlib_logical v3p_netlib_chgsme_(
  v3p_netlib_doublereal *oldnrm,
  v3p_netlib_integer *icnt
  );
extern v3p_netlib_doublereal v3p_netlib_determ_(
  v3p_netlib_integer *n,
  v3p_netlib_doublereal *tri,
  v3p_netlib_doublereal *xlmda
  );
extern int v3p_netlib_dfault_(
  v3p_netlib_integer *iparm,
  v3p_netlib_doublereal *rparm
  );
extern int v3p_netlib_echall_(
  v3p_netlib_integer *nn,
  v3p_netlib_integer *ia,
  v3p_netlib_integer *ja,
  v3p_netlib_doublereal *a,
  v3p_netlib_doublereal *rhs,
  v3p_netlib_integer *iparm,
  v3p_netlib_doublereal *rparm,
  v3p_netlib_integer *icall
  );
extern int v3p_netlib_echout_(
  v3p_netlib_integer *iparm,
  v3p_netlib_doublereal *rparm,
  v3p_netlib_integer *imthd
  );
extern v3p_netlib_doublereal v3p_netlib_eigvns_(
  v3p_netlib_integer *n,
  v3p_netlib_doublereal *tri,
  v3p_netlib_doublereal *d__,
  v3p_netlib_doublereal *e2,
  v3p_netlib_integer *ier
  );
extern v3p_netlib_doublereal v3p_netlib_eigvss_(
  v3p_netlib_integer *n,
  v3p_netlib_doublereal *tri,
  v3p_netlib_doublereal *start,
  v3p_netlib_doublereal *zeta,
  v3p_netlib_integer *itmax,
  v3p_netlib_integer *ier
  );
extern int v3p_netlib_eqrt1s_(
  v3p_netlib_doublereal *d__,
  v3p_netlib_doublereal *e2,
  v3p_netlib_integer *nn,
  v3p_netlib_integer *m,
  v3p_netlib_integer *isw,
  v3p_netlib_integer *ierr
  );
extern v3p_netlib_integer v3p_netlib_ipstr_(
  v3p_netlib_doublereal *omega
  );
extern int v3p_netlib_iterm_(
  v3p_netlib_integer *nn,
  v3p_netlib_doublereal *a,
  v3p_netlib_doublereal *u,
  v3p_netlib_doublereal *wk,
  v3p_netlib_integer *imthdd
  );
extern int v3p_netlib_ivfill_(
  v3p_netlib_integer *n,
  v3p_netlib_integer *iv,
  v3p_netlib_integer *ival
  );
extern int v3p_netlib_omeg_(
  v3p_netlib_doublereal *dnrm,
  v3p_netlib_integer *iflag
  );
extern v3p_netlib_logical v3p_netlib_omgchg_(
  v3p_netlib_integer *ndummy
  );
extern v3p_netlib_logical v3p_netlib_omgstr_(
  v3p_netlib_integer *ndummy
  );
extern int v3p_netlib_parcon_(
  v3p_netlib_doublereal *dtnrm,
  v3p_netlib_doublereal *c1,
  v3p_netlib_doublereal *c2,
  v3p_netlib_doublereal *c3,
  v3p_netlib_doublereal *c4,
  v3p_netlib_doublereal *gamold,
  v3p_netlib_doublereal *rhotmp,
  v3p_netlib_integer *ibmth
  );
extern int v3p_netlib_parsi_(
  v3p_netlib_doublereal *c1,
  v3p_netlib_doublereal *c2,
  v3p_netlib_doublereal *c3,
  v3p_netlib_integer *ibmth
  );
extern v3p_netlib_doublereal v3p_netlib_pbeta_(
  v3p_netlib_integer *nn,
  v3p_netlib_integer *ia,
  v3p_netlib_integer *ja,
  v3p_netlib_doublereal *a,
  v3p_netlib_doublereal *v,
  v3p_netlib_doublereal *w1,
  v3p_netlib_doublereal *w2
  );
extern int v3p_netlib_pbsor_(
  v3p_netlib_integer *nn,
  v3p_netlib_integer *ia,
  v3p_netlib_integer *ja,
  v3p_netlib_doublereal *a,
  v3p_netlib_doublereal *u,
  v3p_netlib_doublereal *rhs
  );
extern int v3p_netlib_permat_(
  v3p_netlib_integer *nn,
  v3p_netlib_integer *ia,
  v3p_netlib_integer *ja,
  v3p_netlib_doublereal *a,
  v3p_netlib_integer *p,
  v3p_netlib_integer *newia,
  v3p_netlib_integer *isym,
  v3p_netlib_integer *level,
  v3p_netlib_integer *nout,
  v3p_netlib_integer *ierr
  );
extern int v3p_netlib_perror_(
  v3p_netlib_integer *nn,
  v3p_netlib_integer *ia,
  v3p_netlib_integer *ja,
  v3p_netlib_doublereal *a,
  v3p_netlib_doublereal *rhs,
  v3p_netlib_doublereal *u,
  v3p_netlib_doublereal *w,
  v3p_netlib_doublereal *digtt1,
  v3p_netlib_doublereal *digtt2,
  v3p_netlib_integer *idgtts
  );
extern int v3p_netlib_pervec_(
  v3p_netlib_integer *n,
  v3p_netlib_doublereal *v,
  v3p_netlib_integer *p
  );
extern int v3p_netlib_pfsor_(
  v3p_netlib_integer *nn,
  v3p_netlib_integer *ia,
  v3p_netlib_integer *ja,
  v3p_netlib_doublereal *a,
  v3p_netlib_doublereal *u,
  v3p_netlib_doublereal *rhs
  );
extern int v3p_netlib_pfsor1_(
  v3p_netlib_integer *nn,
  v3p_netlib_integer *ia,
  v3p_netlib_integer *ja,
  v3p_netlib_doublereal *a,
  v3p_netlib_doublereal *u,
  v3p_netlib_doublereal *rhs
  );
extern int v3p_netlib_pjac_(
  v3p_netlib_integer *nn,
  v3p_netlib_integer *ia,
  v3p_netlib_integer *ja,
  v3p_netlib_doublereal *a,
  v3p_netlib_doublereal *u,
  v3p_netlib_doublereal *rhs
  );
extern int v3p_netlib_pmult_(
  v3p_netlib_integer *nn,
  v3p_netlib_integer *ia,
  v3p_netlib_integer *ja,
  v3p_netlib_doublereal *a,
  v3p_netlib_doublereal *u,
  v3p_netlib_doublereal *w
  );
extern int v3p_netlib_prbndx_(
  v3p_netlib_integer *nn,
  v3p_netlib_integer *nblack,
  v3p_netlib_integer *ia,
  v3p_netlib_integer *ja,
  v3p_netlib_integer *p,
  v3p_netlib_integer *ip,
  v3p_netlib_integer *level,
  v3p_netlib_integer *nout,
  v3p_netlib_integer *ier
  );
extern int v3p_netlib_prsblk_(
  v3p_netlib_integer *nnb,
  v3p_netlib_integer *nnr,
  v3p_netlib_integer *ia,
  v3p_netlib_integer *ja,
  v3p_netlib_doublereal *a,
  v3p_netlib_doublereal *ur,
  v3p_netlib_doublereal *vb
  );
extern int v3p_netlib_prsred_(
  v3p_netlib_integer *nnb,
  v3p_netlib_integer *nnr,
  v3p_netlib_integer *ia,
  v3p_netlib_integer *ja,
  v3p_netlib_doublereal *a,
  v3p_netlib_doublereal *ub,
  v3p_netlib_doublereal *vr
  );
extern int v3p_netlib_pssor1_(
  v3p_netlib_integer *nn,
  v3p_netlib_integer *ia,
  v3p_netlib_integer *ja,
  v3p_netlib_doublereal *a,
  v3p_netlib_doublereal *u,
  v3p_netlib_doublereal *rhs,
  v3p_netlib_doublereal *fr,
  v3p_netlib_doublereal *br
  );
extern int v3p_netlib_pstop_(
  v3p_netlib_integer *n,
  v3p_netlib_doublereal *u,
  v3p_netlib_doublereal *dnrm,
  v3p_netlib_doublereal *ccon,
  v3p_netlib_integer *iflag,
  v3p_netlib_logical *q1
  );
extern v3p_netlib_doublereal v3p_netlib_pvtbv_(
  v3p_netlib_integer *n,
  v3p_netlib_integer *ia,
  v3p_netlib_integer *ja,
  v3p_netlib_doublereal *a,
  v3p_netlib_doublereal *v
  );
extern int v3p_netlib_qsort_(
  v3p_netlib_integer *nn,
  v3p_netlib_integer *key,
  v3p_netlib_doublereal *data,
  v3p_netlib_integer *error
  );
extern int v3p_netlib_sbagn_(
  v3p_netlib_integer *n,
  v3p_netlib_integer *nz,
  v3p_netlib_integer *ia,
  v3p_netlib_integer *ja,
  v3p_netlib_doublereal *a,
  v3p_netlib_integer *iwork,
  v3p_netlib_integer *levell,
  v3p_netlib_integer *noutt,
  v3p_netlib_integer *ierr
  );
extern int v3p_netlib_sbelm_(
  v3p_netlib_integer *nn,
  v3p_netlib_integer *ia,
  v3p_netlib_integer *ja,
  v3p_netlib_doublereal *a,
  v3p_netlib_doublereal *rhs,
  v3p_netlib_integer *iw,
  v3p_netlib_doublereal *rw,
  v3p_netlib_doublereal *tol,
  v3p_netlib_integer *isym,
  v3p_netlib_integer *level,
  v3p_netlib_integer *nout,
  v3p_netlib_integer *ier
  );
extern int v3p_netlib_sbend_(
  v3p_netlib_integer *n,
  v3p_netlib_integer *nz,
  v3p_netlib_integer *ia,
  v3p_netlib_integer *ja,
  v3p_netlib_doublereal *a,
  v3p_netlib_integer *iwork
  );
extern int v3p_netlib_sbini_(
  v3p_netlib_integer *n,
  v3p_netlib_integer *nz,
  v3p_netlib_integer *ia,
  v3p_netlib_integer *ja,
  v3p_netlib_doublereal *a,
  v3p_netlib_integer *iwork
  );
extern int v3p_netlib_sbsij_(
  v3p_netlib_integer *n,
  v3p_netlib_integer *nz,
  v3p_netlib_integer *ia,
  v3p_netlib_integer *ja,
  v3p_netlib_doublereal *a,
  v3p_netlib_integer *iwork,
  v3p_netlib_integer *ii,
  v3p_netlib_integer *jj,
  v3p_netlib_doublereal *vall,
  v3p_netlib_integer *mode,
  v3p_netlib_integer *levell,
  v3p_netlib_integer *noutt,
  v3p_netlib_integer *ierr
  );
extern int v3p_netlib_scal_(
  v3p_netlib_integer *nn,
  v3p_netlib_integer *ia,
  v3p_netlib_integer *ja,
  v3p_netlib_doublereal *a,
  v3p_netlib_doublereal *rhs,
  v3p_netlib_doublereal *u,
  v3p_netlib_doublereal *d__,
  v3p_netlib_integer *level,
  v3p_netlib_integer *nout,
  v3p_netlib_integer *ier
  );
extern int v3p_netlib_sum3_(
  v3p_netlib_integer *n,
  v3p_netlib_doublereal *c1,
  v3p_netlib_doublereal *x1,
  v3p_netlib_doublereal *c2,
  v3p_netlib_doublereal *x2,
  v3p_netlib_doublereal *c3,
  v3p_netlib_doublereal *x3
  );
extern v3p_netlib_doublereal v3p_netlib_tau_(
  v3p_netlib_integer *ii
  );
extern v3p_netlib_E_f v3p_netlib_timer_(
  v3p_netlib_real *timdmy
  );
extern v3p_netlib_logical v3p_netlib_tstchg_(
  v3p_netlib_integer *ibmth
  );
extern int v3p_netlib_unscal_(
  v3p_netlib_integer *n,
  v3p_netlib_integer *ia,
  v3p_netlib_integer *ja,
  v3p_netlib_doublereal *a,
  v3p_netlib_doublereal *rhs,
  v3p_netlib_doublereal *u,
  v3p_netlib_doublereal *d__
  );
extern int v3p_netlib_vevmw_(
  v3p_netlib_integer *n,
  v3p_netlib_doublereal *v,
  v3p_netlib_doublereal *w
  );
extern int v3p_netlib_vevpw_(
  v3p_netlib_integer *n,
  v3p_netlib_doublereal *v,
  v3p_netlib_doublereal *w
  );
extern int v3p_netlib_vfill_(
  v3p_netlib_integer *n,
  v3p_netlib_doublereal *v,
  v3p_netlib_doublereal *val
  );
extern int v3p_netlib_vout_(
  v3p_netlib_integer *n,
  v3p_netlib_doublereal *v,
  v3p_netlib_integer *iswt,
  v3p_netlib_integer *noutt
  );
extern int v3p_netlib_wevmw_(
  v3p_netlib_integer *n,
  v3p_netlib_doublereal *v,
  v3p_netlib_doublereal *w
  );
extern int v3p_netlib_zbrent_(
  v3p_netlib_integer *n,
  v3p_netlib_doublereal *tri,
  v3p_netlib_doublereal *eps,
  v3p_netlib_integer *nsig,
  v3p_netlib_doublereal *aa,
  v3p_netlib_doublereal *bb,
  v3p_netlib_integer *maxfnn,
  v3p_netlib_integer *ier
  );
