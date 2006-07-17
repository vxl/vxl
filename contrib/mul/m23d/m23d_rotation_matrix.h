//:
// \file
// \author Tim Cootes
// \brief Generate pure 3x3 rotation matrices

#ifndef m23d_rotation_matrix_h_
#define m23d_rotation_matrix_h_

#include <vnl/vnl_matrix.h>

//: Generate 3x3 matrix which applies a rotation about the x axis
vnl_matrix<double> m23d_rotation_matrix_x(double A);

//: Generate 3x3 matrix which applies a rotation about the y axis
vnl_matrix<double> m23d_rotation_matrix_y(double A);

//: Generate 3x3 matrix which applies a rotation about the z axis
vnl_matrix<double> m23d_rotation_matrix_z(double A);

//: Generate 3x3 matrix which applies rotations about the x,y and z axes
//  Order of application: x,y, z.
vnl_matrix<double> m23d_rotation_matrix(double Ax, double Ay, double Az);


#endif // m23d_rotation_matrix_h_
