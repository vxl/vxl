// This is oxl/mvl/mvl_three_view_six_point_structure.cxx
//:
// \file
// \author fsm

#include <iostream>
#include <cmath>
#include "mvl_three_view_six_point_structure.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vnl/algo/vnl_svd.h>
#include <vnl/algo/vnl_rpoly_roots.h>

#include <mvl/mvl_five_point_camera_pencil.h>
#include <mvl/mvl_six_point_design_matrix_row.h>
#include <mvl/mvl_psi.h>

mvl_three_view_six_point_structure::mvl_three_view_six_point_structure()
  : u(3, 6)
  , v(3, 6)
{
}


bool mvl_three_view_six_point_structure::compute()
{
  vnl_double_3x4 A[3];
  vnl_double_3x4 B[3];
  vnl_matrix<double> design(3, 5);

  for (int i=0; i<3; ++i) {
    // compute camera pencils.
    if (! mvl_five_point_camera_pencil(u[i], v[i], &A[i], &B[i]))
      return false;

    // fill out design matrix.
    mvl_six_point_design_matrix_row(A[i].as_matrix(), B[i].as_matrix(), u[i][5], v[i][5], design[i]);
  }

  // compute pencil of solutions.
  vnl_svd<double> svd(design);
  vnl_vector<double> p = svd.V().get_column(3);
  vnl_vector<double> q = svd.V().get_column(4);

  // restrict the cubic to the pencil :
  vnl_vector<double> coeffs(4);
  mvl_psi_constraint_restrict(&p[0], &q[0], &coeffs[0]);

  // solve the cubic on the pencil.
  if (std::abs(coeffs[0]) > std::abs(coeffs[3])) {
    coeffs.flip();
    swap(p, q);
  }
  vnl_rpoly_roots roots(coeffs);

  for (int k=0; k<3; ++k) {
    // extract solution, if (approximately) real.
    double re = roots.real(k);
    double im = roots.imag(k);
    if (/* im != 0 */std::abs(im) > 0.03 * std::abs(re)) {
      solution[k].valid = false;
      //solution[k].clear();
      continue;
    }
    else {
      solution[k].valid = true;
      mvl_psi_invert((re * p + q).data_block(), solution[k].Q.data_block());
    }

    for (int i=0; i<3; ++i) {
      double st[2];
      if (! mvl_five_point_camera_pencil_parameters(A[i], B[i],
                                                    solution[k].Q,
                                                    u[i][5], v[i][5],
                                                    st, nullptr))
        solution[k].valid = false;
      else
        solution[k].P[i] = st[0] * A[i] + st[1] * B[i];
    }
  }

  return true;
}
