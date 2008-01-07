#include "m23d_pure_ortho_projection.h"
//:
// \file
// \author Tim Cootes
// \brief Extracts pure rotation/projection component of 2x3 projection matrix P

#include <vnl/algo/vnl_svd.h>

//: Extracts pure rotation/projection component of 2x3 projection matrix P
// Resulting 2x3 applies only rotation/projection, no scaling.
// However, it may include a reflection.
vnl_matrix<double> m23d_pure_ortho_projection(const vnl_matrix<double>& P)
{
  vnl_svd<double> svd(P.transpose());
  return svd.V()*svd.U().transpose();  // Remove effect of singular values
}
