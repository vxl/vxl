/*
  fsm@robots.ox.ac.uk
*/
#include "vnl_complex_eigensystem.h"
#include <vnl/vnl_complex_ops.h>
#include <assert.h>
#include <stdlib.h>

extern "C" {
  // correct ?
  typedef vnl_double_complex doublecomplex;
  typedef int integer;
  typedef double doublereal;
  
  // see zgeev.c for information about zgeev_().
  int zgeev_(const char *jobvl, 
	     const char *jobvr, 
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
};

void vnl_complex_eigensystem::compute(vnl_matrix<vnl_double_complex> const & A,
				      bool right,
				      bool left)
{
  A.assert_size(N,N);

  //
  // Remember that fortran matrices and C matrices are transposed
  // relative to each other. Moveover, the documentation for zgeev
  // says that left eigenvectors u satisfy u^h A = lambda u^h,
  // where ^h denotes adjoint (conjugate transpose).
  // So we pass our left eigenvector storage as their right 
  // eigenvector storage and vice versa.
  // But then we also have to conjugate our R after calling the routine.
  //
  vnl_matrix<vnl_double_complex> tmp(A);

  int work_space=10*N;
  vnl_vector<vnl_double_complex> work(work_space);

  int rwork_space=2*N;
  vnl_vector<double> rwork(rwork_space);
  
  int info;
  int tmpN = N;
  zgeev_(right ? "V" : "N",   // jobvl
	 left  ? "V" : "N",   // jobvr
	 &tmpN,               // n
	 tmp.data_block(),    // a
	 &tmpN,               // lda
	 W.data_block(),      // w
	 R.data_block(),      // vl
	 &tmpN,               // ldvl
	 L.data_block(),      // vr
	 &tmpN,               // ldvr
	 work.data_block(),   // work
	 &work_space,         // lwork
	 rwork.data_block(),  // rwork
	 &info                // info
	 );
  assert(tmpN == N);

  if (!right)
    R.fill(0);
  else {
    // conjugate all elements of R :
    for (int i=0;i<N;i++)
      for (int j=0;j<N;j++)
	R(i,j) = conj( R(i,j) );
  }

  if (!left)
    L.fill(0);

  if (info == 0) {
    // success
    }
  else if (info < 0) {
    cerr << (-info) << "th argument has illegal value" << endl;
  }
  else if (info > 0) {
    cerr << "vnl_qr algorithm failed." << endl;
  }
  else {
    assert(false); // blah
  }
}

//--------------------------------------------------------------------------------

//
vnl_complex_eigensystem::vnl_complex_eigensystem(vnl_matrix<vnl_double_complex> const &A,
						 bool right,
						 bool left)
  : N(A.rows())
  , L(N,N)
  , R(N,N)
  , W(N)
{
  compute(A,right,left);
}

//
vnl_complex_eigensystem::vnl_complex_eigensystem(vnl_matrix<double> const &A_real,
						 vnl_matrix<double> const &A_imag,
						 bool right,
						 bool left)
  : N(A_real.rows())
  , L(N,N)
  , R(N,N)
  , W(N)
{
  A_real.assert_size(N,N);
  A_imag.assert_size(N,N);
  
  vnl_matrix<vnl_double_complex> A(N,N);
  vnl_complexify(A_real.begin(), A_imag.begin(), A.begin(), A.size());

  compute(A,right,left);
}

//
vnl_complex_eigensystem::~vnl_complex_eigensystem()
{
}
