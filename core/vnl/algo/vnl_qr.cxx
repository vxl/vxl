//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation "vnl_qr.h"
#endif
//
// Class: vnl_qr
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 08 Dec 96
// Modifications:
//   081296 AWF Temporarily abandoned as I realized my problem was symmetric...
//   080697 AWF Recovered, implemented solve().
//   200897 AWF Added determinant().
//   071097 AWF Added Q(), R().

#include "vnl_qr.h"
#include <vcl/vcl_iostream.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_matlab_print.h>
#include <vnl/algo/vnl_netlib.h> // dqrdc_(), dqrsl_()

// -- Extract the vnl_qr decomposition of matrix M.  The decomposition is stored in
// a compact and time-efficient packed form, which is most easily used via the
// "solve" and "determinant" methods.
vnl_qr::vnl_qr(const vnl_matrix<double>& M):
  qrdc_out_(M.columns(), M.rows(), 999),
  qraux_(M.columns(), 0),
  jpvt_(M.rows(), 1234),
  Q_(0),
  R_(0)
{
  // Fill transposed O/P matrix
  int c = M.columns();
  int r = M.rows();
  for(int i = 0; i < r; ++i)
    for(int j = 0; j < c; ++j)
      qrdc_out_(j,i) = M(i,j);
  
  int do_pivot = 0; // Enable/disable pivoting.
  jpvt_.fill(0); // Allow all columns to be pivoted if pivoting is enabled.

  vnl_vector<double> work(M.rows());
  dqrdc_(qrdc_out_.data_block(), // On output, UT is R, below diag is mangled Q
	 r, r, c,
	 qraux_.data_block(), // Further information required to demangle Q
	 jpvt_.data_block(),
	 work.data_block(),
	 do_pivot);
}

vnl_qr::~vnl_qr()
{
  delete Q_;
  delete R_;
}

// -- Solve equation M x = b for x using the computed decomposition.
vnl_vector<double> vnl_qr::solve(const vnl_vector<double>& b) const
{
  int n = qrdc_out_.columns();
  int p = qrdc_out_.rows();
  const double* b_data = b.data_block();
  vnl_vector<double> QtB(n);
  vnl_vector<double> x(p);
  
  // JOB: ABCDE decimal
  // A     B     C     D              E
  // ---   ---   ---   ---            ---
  // Qb    Q'b   x     norm(A*x - b)  A*x

  int JOB = 100;

  int info = 0;
  dqrsl_(qrdc_out_.data_block(),
	 n, n, p,
	 qraux_.data_block(),
	 b_data, 0, QtB.data_block(),
	 x.data_block(),
	 0/*residual*/,
	 0/*Ax*/,
	 JOB,
	 &info);
  
  if (info > 0) {
    cerr << "vnl_qr::solve() -- A is rank-def by " << info << endl;
  }

  return x;
}

// -- Return the determinant of M.  This is computed from M = vnl_qr as follows:
// |M| = |Q| |R|
// |R| is the product of the diagonal elements.
// |Q| is (-1)^n as it is a product of Householder reflections.
// So det = -prod(-r_ii).
double vnl_qr::determinant() const
{
  // |M| = |Q| |R|
  // |R| is the product of the diagonal elements.
  // |Q| is (-1)^n as it is a product of Householder reflections.
  int m = vnl_math_min((int)qrdc_out_.columns(), (int)qrdc_out_.rows());
  double det = qrdc_out_(0,0);

  for(int i = 1; i < m; ++i)
    det *= -qrdc_out_(i,i);

  return det;
}

// -- Unpack and return orthonomal part Q.
vnl_matrix<double>& vnl_qr::Q() 
{
  int m = qrdc_out_.columns(); // column-major storage
  int n = qrdc_out_.rows();

  bool verbose = false;

  if (!Q_) {
    Q_ = new vnl_matrix<double>(m,m);
    // extract Q.
    if (verbose) {
      cerr << "vnl_qr::Q() " << endl;
      cerr << " m,n = " << m << ", " << n << endl;
      cerr << " qr0 = [" << qrdc_out_ << "];\n";
      cerr << " aux = [" << qraux_ << "];\n";
    }
    
    Q_->set_identity();
    vnl_matrix<double>& Q = *Q_;
    
    vnl_vector<double> v(m, 0.0);
    vnl_vector<double> w(m, 0.0);

    // Golub and vanLoan, p199.  backward accumulation of householder matrices
    // Householder vcl_vector k is [zeros(1,k-1) qraux_[k] qrdc_out_[k,:]]
    
    for(int k = n-1; k >= 0; --k) {
      if (k >= m) continue;
      // Make housevec v, and accumulate norm at the same time.
      v[k] = qraux_[k];
      double sq = v[k]*v[k];
      for(int j = k+1; j < m; ++j) {
	v[j] = qrdc_out_(k,j);
	sq += v[j]*v[j];
      }
      if (verbose) MATLABPRINT(v);
      
      // Premultiply emerging Q by house(v)
      // note that v[0..k-1] == 0
      // row house is
      //   Q -= (2/v'*v) Q v v'
      if (sq > 0.0) {
	double scale = 2/sq;
	// w = (2/v'*v) Q v
	for(int i = k; i < m; ++i) {
	  w[i] = 0;
	  for(int j = k; j < m; ++j)
	    w[i] += scale * Q(j,i) * v[j];
	}
	if (verbose) MATLABPRINT(w);
	
	// Q -= w v'
	for(int i = k; i < m; ++i)
	  for(int j = k; j < m; ++j)
	    Q(i,j) -= w[j] * v[i];
      }
    }
  }
  return *Q_;
}

// -- Unpack and return R.
vnl_matrix<double>& vnl_qr::R()
{
  if (!R_) {
    int m = qrdc_out_.columns(); // column-major storage
    int n = qrdc_out_.rows();
    R_ = new vnl_matrix<double>(m,n);
    
    for(int i = 0; i < m; ++i)
      for(int j = 0; j < n; ++j)
	if (i > j)
	  R_->put(i,j,0);
	else
	  R_->put(i,j, qrdc_out_(j,i));
  }

  return *R_;
}


// -- Return residual vcl_vector d of M x = b -> d = Q'b 
vnl_vector<double> vnl_qr::QtB(const vnl_vector<double>& b) const
{
  int n = qrdc_out_.columns();
  int p = qrdc_out_.rows();
  const double* b_data = b.data_block();
  vnl_vector<double> QtB(n);
    
  // JOB: ABCDE decimal
  // A     B     C     D              E
  // ---   ---   ---   ---            ---
  // Qb    Q'b   x     norm(A*x - b)  A*x

  int JOB = 1000;

  int info = 0;
  dqrsl_(qrdc_out_.data_block(),
	 n, n, p,
	 qraux_.data_block(),
	 b_data,
	 0,			// A: Qb
	 QtB.data_block(),	// B: Q'b
	 0,			// C: x
	 0,			// D: residual
	 0,			// E: Ax
	 JOB,
	 &info);
  
  if (info > 0) {
    cerr << "vnl_qr::QtB() -- A is rank-def by " << info << endl;
  }

  return QtB;
}
