// This is core/vgl/algo/vgl_h_matrix_2d_compute_4point.cxx
#include "vgl_h_matrix_2d_compute_4point.h"
//:
// \file
#include "vnl/vnl_inverse.h"
#include <cassert>
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif

//-----------------------------------------------------------------------------
//
//: Compute a plane-plane projectivity using 4 point correspondences.
// Returns false if the calculation fails or there are fewer than four point
// matches in the list.
//
// The algorithm determines the transformation $H_i$ from each pointset to the
// canonical projective basis (see h_matrix_2d::projective_basis), and
// returns the combined transform $H = H_2^{-1} H_1$.
bool
vgl_h_matrix_2d_compute_4point::compute_p(const std::vector<vgl_homg_point_2d<double>> & points1,
                                          const std::vector<vgl_homg_point_2d<double>> & points2,
                                          vgl_h_matrix_2d<double> & H)
{
  vgl_h_matrix_2d<double> H1, H2;
  if (!H1.projective_basis(points1))
    return false;
  if (!H2.projective_basis(points2))
    return false;
  H.set(vnl_inverse(H2.get_matrix()) * H1.get_matrix());
  return true;
}

//-----------------------------------------------------------------------------
//
//: Compute a plane-plane projectivity using 4 line correspondences.
// Returns false if the calculation fails or there are fewer than four line
// matches in the list.
//
// Implementation is the dual of the implementation of compute_p()
bool
vgl_h_matrix_2d_compute_4point::compute_l(const std::vector<vgl_homg_line_2d<double>> & lines1,
                                          const std::vector<vgl_homg_line_2d<double>> & lines2,
                                          vgl_h_matrix_2d<double> & H)
{
  vgl_h_matrix_2d<double> H1, H2;
  if (!H1.projective_basis(lines1))
    return false;
  if (!H2.projective_basis(lines2))
    return false;
  H.set(vnl_inverse(H2.get_matrix()) * H1.get_matrix());
  return true;
}

bool
vgl_h_matrix_2d_compute_4point::compute_l(const std::vector<vgl_homg_line_2d<double>> & lines1,
                                          const std::vector<vgl_homg_line_2d<double>> & lines2,
                                          const std::vector<double> &,
                                          vgl_h_matrix_2d<double> & H)
{
  vgl_h_matrix_2d<double> H1, H2;
  if (!H1.projective_basis(lines1))
    return false;
  if (!H2.projective_basis(lines2))
    return false;
  H.set(vnl_inverse(H2.get_matrix()) * H1.get_matrix());
  return true;
}

bool
vgl_h_matrix_2d_compute_4point::compute_pl(const std::vector<vgl_homg_point_2d<double>> & /*points1*/,
                                           const std::vector<vgl_homg_point_2d<double>> & /*points2*/,
                                           const std::vector<vgl_homg_line_2d<double>> & /*lines1*/,
                                           const std::vector<vgl_homg_line_2d<double>> & /*lines2*/,
                                           vgl_h_matrix_2d<double> &)
{
  assert(!"vgl_h_matrix_2d_compute_4point::compute_pl() NYI");
  return false;
}
