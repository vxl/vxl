/*
  fsm@robots.ox.ac.uk
*/
#include "vnl_complex_eigensystem.h"
#include <vcl/vcl_cassert.h>
#include <vcl/vcl_cstdlib.h>
#include <vcl/vcl_iostream.h>
#include <vnl/vnl_complex_ops.h>
#include <vnl/algo/vnl_netlib.h> // zgeev_()

void vnl_complex_eigensystem::compute(vnl_matrix<vnl_double_complex> const & A,
				      bool right,
				      bool left)
{
  A.assert_size(N_,N_);

  //
  // Remember that fortran matrices and C matrices are transposed
  // relative to each other. Moveover, the documentation for zgeev
  // says that left eigenvectors u satisfy u^h A = lambda u^h,
  // where ^h denotes adjoint (conjugate transpose).
  // So we pass our left eigenvector storage as their right 
  // eigenvector storage and vice versa.
  // But then we also have to conjugate our R_ after calling the routine.
  //
  vnl_matrix<vnl_double_complex> tmp(A);

  int work_space=10*N_;
  vnl_vector<vnl_double_complex> work(work_space);

  int rwork_space=2*N_;
  vnl_vector<double> rwork(rwork_space);
  
  int info;
  int tmpN = N_;
  zgeev_(right ? "V" : "N",   // jobvl
	 left  ? "V" : "N",   // jobvr
	 &tmpN,               // n
	 tmp.data_block(),    // a
	 &tmpN,               // lda
	 W_.data_block(),     // w
	 R_.data_block(),     // vl
	 &tmpN,               // ldvl
	 L_.data_block(),     // vr
	 &tmpN,               // ldvr
	 work.data_block(),   // work
	 &work_space,         // lwork
	 rwork.data_block(),  // rwork
	 &info                // info
	 );
  assert(tmpN == N_);

  if (!right)
    R_.fill(0);
  else {
    // conjugate all elements of R_ :
    for (int i=0;i<N_;i++)
      for (int j=0;j<N_;j++)
	R_(i,j) = conj( R_(i,j) );
  }

  if (!left)
    L_.fill(0);

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
  : N_(A.rows())
  , L_(N_,N_)
  , R_(N_,N_)
  , W_(N_)
{
  compute(A,right,left);
}

//
vnl_complex_eigensystem::vnl_complex_eigensystem(vnl_matrix<double> const &A_real,
						 vnl_matrix<double> const &A_imag,
						 bool right,
						 bool left)
  : N_(A_real.rows())
  , L_(N_,N_)
  , R_(N_,N_)
  , W_(N_)
{
  A_real.assert_size(N_,N_);
  A_imag.assert_size(N_,N_);
  
  vnl_matrix<vnl_double_complex> A(N_,N_);
  vnl_complexify(A_real.begin(), A_imag.begin(), A.begin(), A.size());

  compute(A,right,left);
}

//
vnl_complex_eigensystem::~vnl_complex_eigensystem() {
}
