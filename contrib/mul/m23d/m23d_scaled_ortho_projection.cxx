//:
// \file
// \author Tim Cootes
// \brief Extracts scaled rotation/projection component of 2x3 projection matrix P

#include <m23d/m23d_scaled_ortho_projection.h>
#include <vnl/algo/vnl_svd.h>

//: Extracts scaled rotation/projection component of 2x3 projection matrix P
// Resulting 2x3 is of the form s*(I|0)*R
vnl_matrix<double> m23d_scaled_ortho_projection(const vnl_matrix<double>& P)
{
  vnl_svd<double> svd(P.transpose());
  double k=0.5*(svd.W(0)+svd.W(1));
  return k*svd.V()*svd.U().transpose();
}
