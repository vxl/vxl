#include "vnl_rotation_matrix.h"
#include <vcl/vcl_cmath.h>

bool vnl_rotation_matrix(double const x[3], double **R) {
  // normalize x to a unit vector u, of norm 'angle'.
  double u[3] = {x[0], x[1], x[2]};
  double angle = sqrt(u[0]*u[0] + u[1]*u[1] + u[2]*u[2]);
  u[0] /= angle;
  u[1] /= angle;
  u[2] /= angle;

  // start with an identity matrix.  
  for (unsigned i=0; i<3; ++i)
    for (unsigned j=0; j<3; ++j)
      R[i][j] = (i==j ? 1 : 0);
  
  // add (cos(angle)-1)*(1 - u u').
  double cos_angle = cos(angle);
  for (unsigned i=0; i<3; ++i)
    for (unsigned j=0; j<3; ++j)
      R[i][j] += (cos_angle-1) * ((i==j ? 1:0) - u[i]*u[j]);
  
  // add sin(angle) * [u]
  double sin_angle = sin(angle);
  /* */                      R[0][1] -= sin_angle*u[2]; R[0][2] += sin_angle*u[1];
  R[1][0] += sin_angle*u[2]; /* */                      R[1][2] -= sin_angle*u[0];
  R[2][0] -= sin_angle*u[1]; R[2][1] += sin_angle*u[0]; /* */

#if 0
  //cerr << "axis = [" << axis[0] << ' ' << axis[1] << ' ' << axis[2] << "];" << endl;
  
  cerr << "R=[" << endl;
  for (unsigned i=0; i<3; ++i) {
    for (unsigned j=0; j<3; ++j)
      cerr << ' ' << R[i][j];
    cerr << endl;
  }
  cerr << "];" << endl;
  exit(1);
#endif
  return true;
}

bool vnl_rotation_matrix(double const axis[3], double R[3][3]) {
  double *R_[3] = {R[0], R[1], R[2]};
  return vnl_rotation_matrix(axis, R_);
}

bool vnl_rotation_matrix(double const axis[3], double *R0, double *R1, double *R2) {
  double *R[3] = {R0, R1, R2};
  return vnl_rotation_matrix(axis, R);
}

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>

bool vnl_rotation_matrix(vnl_vector<double> const &axis, vnl_matrix<double> &R) {
  return vnl_rotation_matrix(&axis[0], R.data_array());
}

vnl_matrix<double> vnl_rotation_matrix(vnl_vector<double> const &axis) {
  vnl_matrix<double> R(3, 3);
  vnl_rotation_matrix(&axis[0], R.data_array());
  return R;
}
