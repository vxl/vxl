/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation "vnl_generalized_schur"
#endif
#include "vnl_generalized_schur.h"

#include <vcl_vector.h>
#include <vcl_iostream.h>

#include <vnl/vnl_vector.h>

extern "C" int dgges_(char const *JOBVSL,
                      char const *JOBVSR,
                      char const *SORT,
                      bool (*DELCTG)(double, double, double),
                      int const *N,
                      double *A,
                      int const *LDA,
                      double *B,
                      int const *LDB,
                      int const *SDIM,
                      double *ALPHAR,
                      double *ALPHAI,
                      double *BETA,
                      double *VSL,
                      int const *LDVSL,
                      double *VSR,
                      int const *LDVSR,
                      double *WORK,
                      int const *LWORK,
                      bool *BWORK,
                      int *INFO);

VCL_DEFINE_SPECIALIZATION
bool vnl_generalized_schur/*<double>*/(vnl_matrix<double> *A,
                                       vnl_matrix<double> *B,
                                       vnl_vector<double> *alphar,
                                       vnl_vector<double> *alphai,
                                       vnl_vector<double> *beta,
                                       vnl_matrix<double> *L,
                                       vnl_matrix<double> *R)
{
  int n = A->rows();
  
  assert(n == A->rows());
  assert(n == A->cols());
  assert(n == B->rows());
  assert(n == B->cols());
  assert(alphar); alphar->resize(n);    alphar->fill(0);
  assert(alphai); alphai->resize(n);    alphai->fill(0);
  assert(beta);   beta  ->resize(n);    beta  ->fill(0);
  assert(L);      L     ->resize(n, n); L     ->fill(0);
  assert(R);      R     ->resize(n, n); R     ->fill(0);
  
  int sdim = 0;
  int lwork = 1000 + (8*n + 16);
  double *work = new double[lwork]; //vcl_vector<double> work(lwork);
  int info = 0;
  A->inplace_transpose();
  B->inplace_transpose();
  dgges_ ("V", "V",
          "N",
          0,
          &n,
          A->data_block(), &n,
          B->data_block(), &n,
          &sdim,
          alphar->data_block(),
          alphai->data_block(),
          beta->data_block(),
          L->data_block(), &n,
          R->data_block(), &n,
          &work[0], &lwork,
          0,
          &info);
  A->inplace_transpose();
  B->inplace_transpose();
  L->inplace_transpose();
  R->inplace_transpose();
  delete [] work;
  
  if (info != 0) {
    vcl_clog << __FILE__ ": info = " << info << vcl_endl;
    return false;
  }
  else
    return true;
}
