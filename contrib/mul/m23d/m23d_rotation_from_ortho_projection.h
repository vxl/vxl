//:
// \file
// \author Tim Cootes
// \brief Generate pure 3 x 3 rotation matrix from 2 x 3 projection matrix

#include <vnl/vnl_cross.h>
#include <m23d/m23d_pure_ortho_projection.h>

//: Generate pure 3 x 3 rotation matrix from 2 x 3 projection matrix
//  Result may also include a reflection.
//  The result, R, should be such that PR.transpose() approx= (sI|0)
//
//  Note that the sign of the 3rd row is undefined
vnl_matrix<double> m23d_rotation_from_ortho_projection(const vnl_matrix<double>& M);


