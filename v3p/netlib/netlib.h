//-*- c++ -*-------------------------------------------------------------------
#ifndef netlib_h_
#define netlib_h_

typedef struct { float r, i; } complex;
typedef struct { double r, i; } doublecomplex;

extern "C" {
  int dsvdc_(double *x, const int& ldx, const int& m, const int& n,
             double *sv,
             double *errors,
             double *u, const int& ldu,
             double *v, const int& ldv,
             double *work,
             const int& job, int *info);

  int rsg_(const int& nm, const int& n,
           const double *a_matrix, const double *b_matrix,
           double *eigenvalues,
           const int& want_eigenvectors, const double *eigenvectors,
           const double *workspace_1_size_n, const double *workspace_2_size_n,
           int* output_error_code);

  int rs_(const int& nm, const int& n, const double *a_matrix,
          double *eigenvalues,
          const int& want_eigenvectors, const double *eigenvectors,
          const double *workspace_1_size_n, const double *workspace_2_size_n,
          int* output_error_code);

  int dqrdc_(double *x, const int& ldx, const int& n, const int& p, double* qraux, int *jpvt, double *work, const int& job);

  void cg_(double* starting_guess, double* e, int* it, double* step,
           double* tolerance, int* max_iterations, int* nr_unknowns, int* m,
           double (*cost_function)(), void (*gradient_func)(), void (*both)(),
           void (*preconditioning_func)(), double *work)

  void chscdf_(float* x, int* nu, float* cdf);
  void cqrdc_(complex* x, int* ldx, int* n, int* p, complex* qraux, int* jpvt, complex* work, int* job);
  void cqrsl_(complex* x, int* ldx, int* n, int* k, complex* qraux, complex* y, complex* qy, complex* qty, complex* b, complex* rsd, complex* xb, int* job,* info);
  void csvdc_(complex* x, int* ldx, int* n, int* p, complex* s, complex* e, complex* u, int* ldu, complex* v, int* ldv, complex* work, int* job,* info);
  void dgpfa_(double* a, double* b, double* trigs, int* inc, int* jump, int* n, int* lot, int* isign, int* npqr, int* info);
  void dnlaso_(int (*op)(), int (*iovect)(), int* n, int* nval, int* nfig, int* nperm, int* nmval, double* val, int* nmvec, double* vec, int* nblock, int* maxop, int* maxj, double* work, int* ind,* ierr);
  void dpoco_(double* a, int* lda, int* n, double* rcond, double* z, int* info);
  void dpodi_(double* a, int* lda, int* n, double* det, int* job);
  void dpofa_(double* a, int* lda, int* n,* info);
  void dposl_(double* a, int* lda, int* n, double* b);
  void dqrsl_(double* x, int* ldx, int* n, int* k, double* qraux, double* y, double* qy, double* qty, double* b, double* rsd, double* xb, int* job,* info);
  void dsetgpfa_(double* trigs, int* n, int* ires, int* info);
  void gpfa_(float* a, float* b, float* trigs, int* inc, int* jump, int* n, int* lot, int* isign, int* nj, int* info);
  void lbfgs_(int* n, lbfgs_(int* m, double* x, double* f, double* g, int* /*bool*/ diagco, double* diag, int* iprint, double* eps, double* xtol, double* w, int* iflag);
  void lmder1_(void (*fcn)(), int* m, int* n, double* x, double* fvec, double* fjac, int* ldfjac, double* tol, int* info, int* ipvt, double* wa, int* lwa);
  void lmdif_(void (*fcn)(), int* m, int* n, double* x, double* fvec, double* ftol, double* xtol, double* gtol, int* maxfev, double* epsfcn, double* diag, int* mode, double* factor, int* nprint, int* info, int* nfev, double* fjac, int* ldfjac, int* ipvt, double* qtf, double* wa1, double* wa2, double* wa3, double* wa4, double* errors);
  void lsqr_(int* m, int* n, void (*aprod)(), double* damp, int* leniw, int* lenrw, int* iw, double* rw, double* u, double* v, double* w, double* x, double* se, double* atol, double* btol, double* conlim, int* itnlim, int* nout, int* istop, int* itn, double* anorm, double* acond, double* rnorm, double* arnorm,* xnorm);
  void rg_(int* nm, int* n, double* a, double* wr, double* wi, int* matz, double* z, int* iv1, double* fv1, int* ierr);
  void rpoly_(double* op, int* degree, double* zeror, double* zeroi, int* /*bool*/ fail);
  void setgpfa_(float* trigs, int* n, int* ires, int* info);
  void sqrdc_(float* x, int* ldx, int* n, int* p, float* qraux, int* jpvt, float* work, int* job);
  void sqrsl_(float* x, int* ldx, int* n, int* k, float* qraux, float* y, float* qy, float* qty, float* b, float* rsd, float* xb, int* job,* info);
  void ssvdc_(float* x, int* ldx, int* n, int* p, float* s, float* e, float* u, int* ldu, float* v, int* ldv, float* work, int* job,* info);
  void zgeev_(char* jobvl, char* jobvr, int* n, doublecomplex* a, int* lda, doublecomplex* w, doublecomplex* vl, int* ldvl, doublecomplex* vr, int* ldvr, doublecomplex* work, int* lwork, double* rwork, int* info, long jobvl_len, long jobvr_len);
  void zqrdc_(doublecomplex* x, int* ldx, int* n, int* p, doublecomplex* qraux, int* jpvt, doublecomplex* work, int* job);
  void zqrsl_(doublecomplex* x, int* ldx, int* n, int* k, doublecomplex* qraux, doublecomplex* y, doublecomplex* qy, doublecomplex* qty, doublecomplex* b, doublecomplex* rsd, doublecomplex* xb, int* job,* info);
  void zsvdc_(doublecomplex* x, int* ldx, int* n, int* p, doublecomplex* s, doublecomplex* e, doublecomplex* u, int* ldu, doublecomplex* v, int* ldv, doublecomplex* work, int* job,* info);

}

#endif
