// Solve LS problem M x = B, warning if M is nearly singular.
// Peter Vanroose, February 2000

#include <vnl/algo/vnl_svd.h>

template <class D>  // D is often double or float
vnl_matrix<D> solve_with_warning(const vnl_matrix<D>& M,
				 const vnl_matrix<D>& B)
{
  // Take svd of vnl_matrix<D> M, setting singular values
  // smaller than 1e-8 to 0, and hold the result.
  vnl_svd<D> svd(M, 1e-8);
  // Check for rank-deficiency
  if (svd.singularities() > 1)
    cerr << "Warning: Singular matrix, condition = " << svd.well_condition() << endl;
  return svd.solve(B);
}
