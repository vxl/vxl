/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "vnl_generalized_schur.h"

#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vcl_cassert.h>

#include <vnl/vnl_vector.h>

#include "vnl_netlib.h" // dgges_()

VCL_DEFINE_SPECIALIZATION
bool vnl_generalized_schur/*<double>*/(vnl_matrix<double> *A,
                                       vnl_matrix<double> *B,
                                       vnl_vector<double> *alphar,
                                       vnl_vector<double> *alphai,
                                       vnl_vector<double> *beta,
                                       vnl_matrix<double> *L,
                                       vnl_matrix<double> *R)
{
  assert(A->cols() == A->cols());
  assert(A->cols() == B->rows());
  assert(A->cols() == B->cols());

  int n = A->rows();
  assert(alphar!=0); alphar->resize(n);    alphar->fill(0);
  assert(alphai!=0); alphai->resize(n);    alphai->fill(0);
  assert(beta!=0);   beta  ->resize(n);    beta  ->fill(0);
  assert(L!=0);      L     ->resize(n, n); L     ->fill(0);
  assert(R!=0);      R     ->resize(n, n); R     ->fill(0);

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
