#ifndef vnl_affine_approx_h_
#define vnl_affine_approx_h_

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>

//
// Object for computing affine approximations (by truncating
// the local power/taylor series) to a homography.
//
// The constructor takes an (n+1)x(n+1) matrix, regarded
// as a collineation of projective n-space.
//

template <class T>
class vnl_affine_approx  {
public:
  vnl_affine_approx(const vnl_matrix<T> &H);
  ~vnl_affine_approx();

  // compute affine approximation at x, in the form x -> M x + t
  bool compute(const vnl_vector<T> x,
	       vnl_matrix<T> & M,
	       vnl_vector<T> & t) const;

private:
  // decompose as [ A p ]
  //              [ v 1 ]
  int n;
  vnl_matrix<T> A; // n x n
  vnl_vector<T> p; // column n-vector 
  vnl_vector<T> v; // row n-vector
};

#endif
