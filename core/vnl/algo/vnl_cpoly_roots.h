#ifndef vnl_c_poly_roots_h_
#define vnl_c_poly_roots_h_
/*
  fsm@robots.ox.ac.uk
*/

#include <vnl/vnl_complex.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>

//
// Class to find all the roots of a univariate polynomial f
// with complex coefficients. Currently works by computing the 
// eigenvalues of the companion matrix of f.
//
// The input vector a of coefficients are given to the constructor.
// The polynomial is f = t^N + a[0] t^{N-1} + ... + a[N-1]
// The roots can then be found in the 'solns' member.
//

//: find all the roots of a univariate polynomial with complex coefficients.

class vnl_cpoly_roots {
public:
  vnl_cpoly_roots(vnl_vector<vnl_double_complex> const & a);
  vnl_cpoly_roots(vnl_vector<double> const & a_real,
		  vnl_vector<double> const & a_imag);
  ~vnl_cpoly_roots();
  
  // the roots can be found in here :
  vnl_vector<vnl_double_complex> solns;

private:
  unsigned N; //degree
  void compute(vnl_vector<vnl_double_complex> const & a); // this does the actual work
};

#endif
