/*
  fsm@robots.ox.ac.uk
*/

#include "vnl_cpoly_roots.h"
#include <vnl/algo/vnl_complex_eigensystem.h>
#include <assert.h>

void vnl_cpoly_roots::compute(vnl_vector<vnl_double_complex> const &a) {
  // companion matrix
  vnl_matrix<vnl_double_complex> comp(N,N); 
  comp.fill(0);
  for (unsigned i=0;i<N-1;i++)
    comp(i+1,i) = 1;
  for (unsigned i=0;i<N;i++)
    comp(i,N-1) = -a[N-1-i];
  
  //
  vnl_complex_eigensystem eig(comp,
			      false, // we only want
			      false);// the eigenvalues.
  
  //
  solns = eig.W;
}

//--------------------------------------------------------------------------------

vnl_cpoly_roots::vnl_cpoly_roots(const vnl_vector<vnl_double_complex>& a)
  : solns(a.size())
  , N(a.size()) // degree
{
  compute(a);
}

vnl_cpoly_roots::vnl_cpoly_roots(vnl_vector<double> const & a_real,
				 vnl_vector<double> const & a_imag) 
  : solns(a_real.size())
  , N(a_real.size()) // degree
{
  assert(a_real.size() == a_imag.size());
  vnl_vector<vnl_double_complex> a(N);
  for (unsigned i=0;i<N;i++)
    a[i] = vnl_double_complex(a_real[i],a_imag[i]);

  //cerr << "a = " << a << endl;
  compute(a);
  //cerr << "s = " << solns << endl;
}

vnl_cpoly_roots::~vnl_cpoly_roots()
{
}
