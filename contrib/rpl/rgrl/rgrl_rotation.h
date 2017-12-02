#ifndef rgrl_rotation_h_
#define rgrl_rotation_h_
#pragma once
//:
// \file
// \author Gehua Yang
// \date   Nov 2013

#include <rgrl/rgrl_fwd.h>
#include <vnl/vnl_matrix_fixed.h>

void rgrl_rotation_3d_to_matrix(double const theta, double const alpha, double const phi,
                                       vnl_matrix<double> & R);

inline void rgrl_rotation_3d_to_matrix(double const theta, double const alpha, double const phi,
                                       vnl_matrix_fixed<double, 3, 3> & R)
{
  vnl_matrix<double> RR(R.as_matrix());
  rgrl_rotation_3d_to_matrix(theta, alpha, phi, RR);
}

void rgrl_rotation_3d_from_matrix(vnl_matrix<double> const& R,
                                  double& phi, double& alpha, double& theta);

void rgrl_rotation_2d_to_matrix(double const theta, vnl_matrix<double> & R);

inline void rgrl_rotation_2d_to_matrix(double const theta, vnl_matrix_fixed<double, 2, 2> & R)
{
  vnl_matrix<double> RR(R.as_matrix());
  rgrl_rotation_2d_to_matrix(theta, RR);
}

void rgrl_rotation_2d_from_matrix(vnl_matrix<double> const& R, double &theta);

#endif //rgrl_rotation_h_
