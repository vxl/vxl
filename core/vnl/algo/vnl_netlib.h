#ifndef vnl_netlib_h_
#define vnl_netlib_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
*/

// *** under construction ***

// Purpose: declare in a central place the list of symbols from netlib
// referenced from vnl-algo. This list was auto-generated, so it is
// exhaustive as of 16 March 2000 (10pm)

// Note: the declarations are initially entered as "int f()", which 
// will conflict with the actual prototype. If you get a conflict,
// enter the correct prototype in here.

#include <vnl/vnl_complex.h>
#define vnl_netlib_svd_proto(T) \
T *x, int const &ldx, int const &m, int const &n, \
T *sv, \
T *errors, \
T *u, int const &ldu, \
T *v, int const &ldv, \
T *work, \
int const &job, int *info
#define vnl_netlib_svd_params \
x, ldx, m, n, sv, errors, u, ldu, v, ldv, work, job, info

extern "C" {
  // correct ?
  typedef vnl_double_complex doublecomplex;
  typedef int integer;
  typedef double doublereal;
  
  // complex eigensystem
  int zgeev_(char const *jobvl, 
	     char const *jobvr, 
	     integer *n, 
	     doublecomplex *a,
	     integer *lda, 
	     doublecomplex *w, 
	     doublecomplex *vl, 
	     integer *ldvl, 
	     doublecomplex *vr, 
	     integer *ldvr, 
	     doublecomplex *work, 
	     integer *lwork, 
	     doublereal *rwork, 
	     integer *info);

  // linpack xsvdc() routines
  int zsvdc_(vnl_netlib_svd_proto(vnl_double_complex));
  int csvdc_(vnl_netlib_svd_proto(vnl_float_complex));
  int dsvdc_(vnl_netlib_svd_proto(double));
  int ssvdc_(vnl_netlib_svd_proto(float));

  // double-precision QR
  int dqrdc_(double *x, 
	     int const& ldx, 
	     int const& n, 
	     int const& p, 
	     double* qraux,
	     int *jpvt, 
	     double *work, 
	     int const& job);
  // solve A x = b using QR ?
  int dqrsl_(double const* qrdc, int& ldqrdc, int& n, int& k, double const* qraux,
	     double const* b, double* qb, double* qtb,
	     double* x, double* rsd, double* Ax,
	     int& job, int* info);

  // real eigensystem
  int rg_(int const& nm, 
	  int const& n, 
	  double const* a,
	  double* wr, 
	  double* wi, 
	  int const& matz,
	  double* z,
	  int* iv1, 
	  double* fv1,
	  int* ierr);

  // temperton fft routines
  int gpfa_ (float  *a, float  *b, float  const *triggs, 
	     int const &inc, int const &jump, int const &n, 
	     int const &lot, int const &isign, int const *, int *);
  int setgpfa_ (float  *triggs, const int &, const int *, int *);
  int dgpfa_(double *a, double *b, double const *triggs, 
	     int const &inc, int const &jump, int const &n, 
	     int const &lot, int const &isign, int const *, int *);
  int dsetgpfa_(double *triggs, const int &, const int *, int *);

  // symmetric eigensystem
  int rs_(int const & nm, int const & n,
	  double const *a, double *w,
	  int const & matz, double const *z,
	  double const *fv1, double const *fv2,
	  int * ierr);

  // generalized eigensystem
  int rsg_ (int const & nm, int const & n, double const *a, double const *b,
	    double *w, int const & matz, double *z, double *fv1, double *fv2,
	    int *ierr);
  
  // cholesky
  int dpofa_(double *m, const int& lda, const int& n, int* info);
  int dposl_(const double *a, const int& lda, const int& n, double *b);
  int dpoco_(double *a, const int& lda, const int& n, double* rcond, double *z, int *info);
  int dpodi_(double *a, const int& lda, const int& n, double* det, const int& job);

  // roots of real polynomial
  void rpoly_(const double* op, int* degree, double *zeror, double *zeroi, int *fail);

  //
  int lmder1_();
  int chscdf_();
  int lbfgs_();
  int lmdif_();
  int dnlaso_();
  int cg_();
};


#endif
