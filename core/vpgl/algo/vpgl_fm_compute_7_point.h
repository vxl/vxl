// This is core/vpgl/algo/vpgl_fm_compute_7_point.h
#ifndef vpgl_fm_compute_7_point_h_
#define vpgl_fm_compute_7_point_h_
//:
// \file
// \brief The 7 point algorithm for computing a fundamental matrix from point correspondences.
// \author Thomas Pollard
// \date May 10, 2005
//
// Should template this class. The code here is lifted from oxl/mvl/FMatrixCompute7Point.
// Still need to add preconditioning to points.

#include <vgl/vgl_homg_point_2d.h>
#include <vpgl/vpgl_fundamental_matrix.h>

class vpgl_fm_compute_7_point
{
 public:
  //: If precondition = true, points are conditioned prior to computation.
  vpgl_fm_compute_7_point( bool precondition = true )
   : precondition_(precondition) {}

  //: Compute from two sets of corresponding points.
  // Put the resulting matrix into fm, return true if successful.
  // Points pr are associated with the RHS of the fundamental matrix
  // while the points pl are associated with the LHS.
  bool compute( const std::vector< vgl_homg_point_2d<double> >& pr,
                const std::vector< vgl_homg_point_2d<double> >& pl,
                std::vector< vpgl_fundamental_matrix<double>* >& fm );

 protected:
  static std::vector<double> get_coeffs( vnl_double_3x3 const& F1,
                                        vnl_double_3x3 const& F2 );
  static std::vector<double> solve_quadratic( std::vector<double> v );
  static std::vector<double> solve_cubic( std::vector<double> v );
  bool precondition_;
};

#endif // vpgl_fm_compute_7_point_h_
