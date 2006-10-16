//:
// \file
// \author Tim Cootes
// \brief Generate pure 3 x 3 rotation matrix from 2 x 3 projection matrix

#include <vnl/vnl_cross.h>
#include <m23d/m23d_pure_ortho_projection.h>
#include <m23d/m23d_rotation_from_ortho_projection.h>

//: Generate pure 3 x 3 rotation matrix from 2 x 3 projection matrix
//  Result may also include a reflection.
//  The result, R, should be such that PR.transpose() approx= (sI|0)
vnl_matrix<double> m23d_rotation_from_ortho_projection(const vnl_matrix<double>& M)
{
  vnl_matrix<double> P = m23d_pure_ortho_projection(M);
  vnl_matrix<double> R(3,3);
  R.update(P,0,0);
  R.set_row(2,vnl_cross_3d(P.get_row(0),P.get_row(1)));

  // Test for reflection
  vnl_matrix<double> PRt = M*R.transpose();
  if (PRt(0,0)<0) R*=-1.0;
  return R;
}
