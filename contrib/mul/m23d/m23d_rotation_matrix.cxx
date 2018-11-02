#include <iostream>
#include <cmath>
#include "m23d_rotation_matrix.h"
//:
// \file
// \author Tim Cootes
// \brief Generate pure 3x3 rotation matrices

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Generate 3x3 matrix which applies a rotation about the x axis
vnl_matrix<double> m23d_rotation_matrix_x(double A)
{
  vnl_matrix<double> R(3,3,0.0);
  double c = std::cos(A);
  double s = std::sin(A);
  R(0,0)=1.0;
  R(1,1)=c;  R(1,2)=-s;
  R(2,1)=s;  R(2,2)= c;
  return R;
}

//: Generate 3x3 matrix which applies a rotation about the y axis
vnl_matrix<double> m23d_rotation_matrix_y(double A)
{
  vnl_matrix<double> R(3,3,0.0);
  double c = std::cos(A);
  double s = std::sin(A);
  R(1,1)=1.0;
  R(0,0)=c;  R(0,2)=-s;
  R(2,0)=s;  R(2,2)= c;
  return R;
}


//: Generate 3x3 matrix which applies a rotation about the z axis
vnl_matrix<double> m23d_rotation_matrix_z(double A)
{
  vnl_matrix<double> R(3,3,0.0);
  double c = std::cos(A);
  double s = std::sin(A);
  R(2,2)=1.0;
  R(0,0)=c;  R(0,1)=-s;
  R(1,0)=s;  R(1,1)= c;
  return R;
}


//: Generate 3x3 matrix which applies rotations about the x,y and z axes
//  Order of application: x,y, z.
vnl_matrix<double> m23d_rotation_matrix(double Ax, double Ay, double Az)
{
  return m23d_rotation_matrix_z(Az)
         * m23d_rotation_matrix_y(Ay)
         * m23d_rotation_matrix_x(Ax);
}
