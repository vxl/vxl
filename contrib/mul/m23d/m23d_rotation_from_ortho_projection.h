//:
// \file
// \author Tim Cootes
// \brief Generate pure 3 x 3 rotation matrix from 2 x 3 projection matrix

#ifndef m23d_rotation_from_ortho_projection_h_
#define m23d_rotation_from_ortho_projection_h_

#include <vnl/vnl_matrix.h>

//: Generate pure 3 x 3 rotation matrix from 2 x 3 projection matrix
//  Result may also include a reflection.
//  The result, R, should be such that PR.transpose() approx= (sI|0)
//
//  Note that the sign of the 3rd row is undefined
vnl_matrix<double> m23d_rotation_from_ortho_projection(const vnl_matrix<double>& M);

#endif // m23d_rotation_from_ortho_projection_h_
