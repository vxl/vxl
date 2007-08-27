extern int v3p_netlib_setulb_(
  v3p_netlib_integer *n,
  v3p_netlib_integer *m,
  v3p_netlib_doublereal *x,
  v3p_netlib_doublereal *l,
  v3p_netlib_doublereal *u,
  v3p_netlib_integer *nbd,
  v3p_netlib_doublereal *f,
  v3p_netlib_doublereal *g,
  v3p_netlib_doublereal *factr,
  v3p_netlib_doublereal *pgtol,
  v3p_netlib_doublereal *wa,
  v3p_netlib_integer *iwa,
  char *task,
  v3p_netlib_integer *iprint,
  char *csave,
  v3p_netlib_logical *lsave,
  v3p_netlib_integer *isave,
  v3p_netlib_doublereal *dsave,
  v3p_netlib_ftnlen task_len,
  v3p_netlib_ftnlen csave_len
  );
extern int v3p_netlib_mainlb_(
  v3p_netlib_integer *n,
  v3p_netlib_integer *m,
  v3p_netlib_doublereal *x,
  v3p_netlib_doublereal *l,
  v3p_netlib_doublereal *u,
  v3p_netlib_integer *nbd,
  v3p_netlib_doublereal *f,
  v3p_netlib_doublereal *g,
  v3p_netlib_doublereal *factr,
  v3p_netlib_doublereal *pgtol,
  v3p_netlib_doublereal *ws,
  v3p_netlib_doublereal *wy,
  v3p_netlib_doublereal *sy,
  v3p_netlib_doublereal *ss,
  v3p_netlib_doublereal *yy,
  v3p_netlib_doublereal *wt,
  v3p_netlib_doublereal *wn,
  v3p_netlib_doublereal *snd,
  v3p_netlib_doublereal *z__,
  v3p_netlib_doublereal *r__,
  v3p_netlib_doublereal *d__,
  v3p_netlib_doublereal *t,
  v3p_netlib_doublereal *wa,
  v3p_netlib_doublereal *sg,
  v3p_netlib_doublereal *sgo,
  v3p_netlib_doublereal *yg,
  v3p_netlib_doublereal *ygo,
  v3p_netlib_integer *index,
  v3p_netlib_integer *iwhere,
  v3p_netlib_integer *indx2,
  char *task,
  v3p_netlib_integer *iprint,
  char *csave,
  v3p_netlib_logical *lsave,
  v3p_netlib_integer *isave,
  v3p_netlib_doublereal *dsave,
  v3p_netlib_ftnlen task_len,
  v3p_netlib_ftnlen csave_len
  );
extern int v3p_netlib_active_(
  v3p_netlib_integer *n,
  v3p_netlib_doublereal *l,
  v3p_netlib_doublereal *u,
  v3p_netlib_integer *nbd,
  v3p_netlib_doublereal *x,
  v3p_netlib_integer *iwhere,
  v3p_netlib_integer *iprint,
  v3p_netlib_logical *prjctd,
  v3p_netlib_logical *cnstnd,
  v3p_netlib_logical *boxed
  );
extern int v3p_netlib_bmv_(
  v3p_netlib_integer *m,
  v3p_netlib_doublereal *sy,
  v3p_netlib_doublereal *wt,
  v3p_netlib_integer *col,
  v3p_netlib_doublereal *v,
  v3p_netlib_doublereal *p,
  v3p_netlib_integer *info
  );
extern int v3p_netlib_cauchy_(
  v3p_netlib_integer *n,
  v3p_netlib_doublereal *x,
  v3p_netlib_doublereal *l,
  v3p_netlib_doublereal *u,
  v3p_netlib_integer *nbd,
  v3p_netlib_doublereal *g,
  v3p_netlib_integer *iorder,
  v3p_netlib_integer *iwhere,
  v3p_netlib_doublereal *t,
  v3p_netlib_doublereal *d__,
  v3p_netlib_doublereal *xcp,
  v3p_netlib_integer *m,
  v3p_netlib_doublereal *wy,
  v3p_netlib_doublereal *ws,
  v3p_netlib_doublereal *sy,
  v3p_netlib_doublereal *wt,
  v3p_netlib_doublereal *theta,
  v3p_netlib_integer *col,
  v3p_netlib_integer *head,
  v3p_netlib_doublereal *p,
  v3p_netlib_doublereal *c__,
  v3p_netlib_doublereal *wbp,
  v3p_netlib_doublereal *v,
  v3p_netlib_integer *nint,
  v3p_netlib_doublereal *sg,
  v3p_netlib_doublereal *yg,
  v3p_netlib_integer *iprint,
  v3p_netlib_doublereal *sbgnrm,
  v3p_netlib_integer *info,
  v3p_netlib_doublereal *epsmch
  );
extern int v3p_netlib_cmprlb_(
  v3p_netlib_integer *n,
  v3p_netlib_integer *m,
  v3p_netlib_doublereal *x,
  v3p_netlib_doublereal *g,
  v3p_netlib_doublereal *ws,
  v3p_netlib_doublereal *wy,
  v3p_netlib_doublereal *sy,
  v3p_netlib_doublereal *wt,
  v3p_netlib_doublereal *z__,
  v3p_netlib_doublereal *r__,
  v3p_netlib_doublereal *wa,
  v3p_netlib_integer *index,
  v3p_netlib_doublereal *theta,
  v3p_netlib_integer *col,
  v3p_netlib_integer *head,
  v3p_netlib_integer *nfree,
  v3p_netlib_logical *cnstnd,
  v3p_netlib_integer *info
  );
extern int v3p_netlib_errclb_(
  v3p_netlib_integer *n,
  v3p_netlib_integer *m,
  v3p_netlib_doublereal *factr,
  v3p_netlib_doublereal *l,
  v3p_netlib_doublereal *u,
  v3p_netlib_integer *nbd,
  char *task,
  v3p_netlib_integer *info,
  v3p_netlib_integer *k,
  v3p_netlib_ftnlen task_len
  );
extern int v3p_netlib_formk_(
  v3p_netlib_integer *n,
  v3p_netlib_integer *nsub,
  v3p_netlib_integer *ind,
  v3p_netlib_integer *nenter,
  v3p_netlib_integer *ileave,
  v3p_netlib_integer *indx2,
  v3p_netlib_integer *iupdat,
  v3p_netlib_logical *updatd,
  v3p_netlib_doublereal *wn,
  v3p_netlib_doublereal *wn1,
  v3p_netlib_integer *m,
  v3p_netlib_doublereal *ws,
  v3p_netlib_doublereal *wy,
  v3p_netlib_doublereal *sy,
  v3p_netlib_doublereal *theta,
  v3p_netlib_integer *col,
  v3p_netlib_integer *head,
  v3p_netlib_integer *info
  );
extern int v3p_netlib_formt_(
  v3p_netlib_integer *m,
  v3p_netlib_doublereal *wt,
  v3p_netlib_doublereal *sy,
  v3p_netlib_doublereal *ss,
  v3p_netlib_integer *col,
  v3p_netlib_doublereal *theta,
  v3p_netlib_integer *info
  );
extern int v3p_netlib_freev_(
  v3p_netlib_integer *n,
  v3p_netlib_integer *nfree,
  v3p_netlib_integer *index,
  v3p_netlib_integer *nenter,
  v3p_netlib_integer *ileave,
  v3p_netlib_integer *indx2,
  v3p_netlib_integer *iwhere,
  v3p_netlib_logical *wrk,
  v3p_netlib_logical *updatd,
  v3p_netlib_logical *cnstnd,
  v3p_netlib_integer *iprint,
  v3p_netlib_integer *iter
  );
extern int v3p_netlib_hpsolb_(
  v3p_netlib_integer *n,
  v3p_netlib_doublereal *t,
  v3p_netlib_integer *iorder,
  v3p_netlib_integer *iheap
  );
extern int v3p_netlib_lnsrlb_(
  v3p_netlib_integer *n,
  v3p_netlib_doublereal *l,
  v3p_netlib_doublereal *u,
  v3p_netlib_integer *nbd,
  v3p_netlib_doublereal *x,
  v3p_netlib_doublereal *f,
  v3p_netlib_doublereal *fold,
  v3p_netlib_doublereal *gd,
  v3p_netlib_doublereal *gdold,
  v3p_netlib_doublereal *g,
  v3p_netlib_doublereal *d__,
  v3p_netlib_doublereal *r__,
  v3p_netlib_doublereal *t,
  v3p_netlib_doublereal *z__,
  v3p_netlib_doublereal *stp,
  v3p_netlib_doublereal *dnorm,
  v3p_netlib_doublereal *dtd,
  v3p_netlib_doublereal *xstep,
  v3p_netlib_doublereal *stpmx,
  v3p_netlib_integer *iter,
  v3p_netlib_integer *ifun,
  v3p_netlib_integer *iback,
  v3p_netlib_integer *nfgv,
  v3p_netlib_integer *info,
  char *task,
  v3p_netlib_logical *boxed,
  v3p_netlib_logical *cnstnd,
  char *csave,
  v3p_netlib_integer *isave,
  v3p_netlib_doublereal *dsave,
  v3p_netlib_ftnlen task_len,
  v3p_netlib_ftnlen csave_len
  );
extern int v3p_netlib_matupd_(
  v3p_netlib_integer *n,
  v3p_netlib_integer *m,
  v3p_netlib_doublereal *ws,
  v3p_netlib_doublereal *wy,
  v3p_netlib_doublereal *sy,
  v3p_netlib_doublereal *ss,
  v3p_netlib_doublereal *d__,
  v3p_netlib_doublereal *r__,
  v3p_netlib_integer *itail,
  v3p_netlib_integer *iupdat,
  v3p_netlib_integer *col,
  v3p_netlib_integer *head,
  v3p_netlib_doublereal *theta,
  v3p_netlib_doublereal *rr,
  v3p_netlib_doublereal *dr,
  v3p_netlib_doublereal *stp,
  v3p_netlib_doublereal *dtd
  );
extern int v3p_netlib_prn1lb_(
  v3p_netlib_integer *n,
  v3p_netlib_integer *m,
  v3p_netlib_doublereal *l,
  v3p_netlib_doublereal *u,
  v3p_netlib_doublereal *x,
  v3p_netlib_integer *iprint,
  v3p_netlib_integer *itfile,
  v3p_netlib_doublereal *epsmch
  );
extern int v3p_netlib_prn2lb_(
  v3p_netlib_integer *n,
  v3p_netlib_doublereal *x,
  v3p_netlib_doublereal *f,
  v3p_netlib_doublereal *g,
  v3p_netlib_integer *iprint,
  v3p_netlib_integer *itfile,
  v3p_netlib_integer *iter,
  v3p_netlib_integer *nfgv,
  v3p_netlib_integer *nact,
  v3p_netlib_doublereal *sbgnrm,
  v3p_netlib_integer *nint,
  char *word,
  v3p_netlib_integer *iword,
  v3p_netlib_integer *iback,
  v3p_netlib_doublereal *stp,
  v3p_netlib_doublereal *xstep,
  v3p_netlib_ftnlen word_len
  );
extern int v3p_netlib_prn3lb_(
  v3p_netlib_integer *n,
  v3p_netlib_doublereal *x,
  v3p_netlib_doublereal *f,
  char *task,
  v3p_netlib_integer *iprint,
  v3p_netlib_integer *info,
  v3p_netlib_integer *itfile,
  v3p_netlib_integer *iter,
  v3p_netlib_integer *nfgv,
  v3p_netlib_integer *nintol,
  v3p_netlib_integer *nskip,
  v3p_netlib_integer *nact,
  v3p_netlib_doublereal *sbgnrm,
  v3p_netlib_doublereal *time,
  v3p_netlib_integer *nint,
  char *word,
  v3p_netlib_integer *iback,
  v3p_netlib_doublereal *stp,
  v3p_netlib_doublereal *xstep,
  v3p_netlib_integer *k,
  v3p_netlib_doublereal *cachyt,
  v3p_netlib_doublereal *sbtime,
  v3p_netlib_doublereal *lnscht,
  v3p_netlib_ftnlen task_len,
  v3p_netlib_ftnlen word_len
  );
extern int v3p_netlib_projgr_(
  v3p_netlib_integer *n,
  v3p_netlib_doublereal *l,
  v3p_netlib_doublereal *u,
  v3p_netlib_integer *nbd,
  v3p_netlib_doublereal *x,
  v3p_netlib_doublereal *g,
  v3p_netlib_doublereal *sbgnrm
  );
extern int v3p_netlib_subsm_(
  v3p_netlib_integer *n,
  v3p_netlib_integer *m,
  v3p_netlib_integer *nsub,
  v3p_netlib_integer *ind,
  v3p_netlib_doublereal *l,
  v3p_netlib_doublereal *u,
  v3p_netlib_integer *nbd,
  v3p_netlib_doublereal *x,
  v3p_netlib_doublereal *d__,
  v3p_netlib_doublereal *ws,
  v3p_netlib_doublereal *wy,
  v3p_netlib_doublereal *theta,
  v3p_netlib_integer *col,
  v3p_netlib_integer *head,
  v3p_netlib_integer *iword,
  v3p_netlib_doublereal *wv,
  v3p_netlib_doublereal *wn,
  v3p_netlib_integer *iprint,
  v3p_netlib_integer *info
  );
extern int v3p_netlib_dcsrch_(
  v3p_netlib_doublereal *f,
  v3p_netlib_doublereal *g,
  v3p_netlib_doublereal *stp,
  v3p_netlib_doublereal *ftol,
  v3p_netlib_doublereal *gtol,
  v3p_netlib_doublereal *xtol,
  v3p_netlib_doublereal *stpmin,
  v3p_netlib_doublereal *stpmax,
  char *task,
  v3p_netlib_integer *isave,
  v3p_netlib_doublereal *dsave,
  v3p_netlib_ftnlen task_len
  );
extern int v3p_netlib_dcstep_(
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
  v3p_netlib_doublereal *stpmax
  );
extern int v3p_netlib_timer_(
  v3p_netlib_doublereal *ttime
  );
extern v3p_netlib_doublereal v3p_netlib_dnrm2_(
  v3p_netlib_integer *n,
  v3p_netlib_doublereal *x,
  v3p_netlib_integer *incx
  );
extern v3p_netlib_doublereal v3p_netlib_dpmeps_(
  
  );
extern int v3p_netlib_daxpy_(
  v3p_netlib_integer *n,
  v3p_netlib_doublereal *da,
  v3p_netlib_doublereal *dx,
  v3p_netlib_integer *incx,
  v3p_netlib_doublereal *dy,
  v3p_netlib_integer *incy
  );
extern int v3p_netlib_dcopy_(
  v3p_netlib_integer *n,
  v3p_netlib_doublereal *dx,
  v3p_netlib_integer *incx,
  v3p_netlib_doublereal *dy,
  v3p_netlib_integer *incy
  );
extern v3p_netlib_doublereal v3p_netlib_ddot_(
  v3p_netlib_integer *n,
  v3p_netlib_doublereal *dx,
  v3p_netlib_integer *incx,
  v3p_netlib_doublereal *dy,
  v3p_netlib_integer *incy
  );
extern int v3p_netlib_dpofa_(
  v3p_netlib_doublereal *a,
  v3p_netlib_integer *lda,
  v3p_netlib_integer *n,
  v3p_netlib_integer *info
  );
extern int v3p_netlib_dscal_(
  v3p_netlib_integer *n,
  v3p_netlib_doublereal *da,
  v3p_netlib_doublereal *dx,
  v3p_netlib_integer *incx
  );
extern int v3p_netlib_dtrsl_(
  v3p_netlib_doublereal *t,
  v3p_netlib_integer *ldt,
  v3p_netlib_integer *n,
  v3p_netlib_doublereal *b,
  v3p_netlib_integer *job,
  v3p_netlib_integer *info
  );
