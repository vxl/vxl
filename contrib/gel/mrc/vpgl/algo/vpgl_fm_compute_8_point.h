// This is gel/mrc/vpgl/algo/vpgl_fm_compute_8_point.h
#ifndef vpgl_fm_compute_8_point_h_
#define vpgl_fm_compute_8_point_h_
//:
// \file
// \brief The 8 point algorithm for computing a fundamental matrix from point correspondences.
// \author Thomas Pollard
// \date 5/10/05
//
//  The point correspondences in relation to F are defined by
//     pl^t[F]pr = 0
//
// \verbatim
//  Modifications
//   9/27/2007  Ricardo Fabbri   Imposed order of 1) rank-enforcement and 2) de-normalization.
// \endverbatim
//
#include <vpgl/vpgl_fundamental_matrix.h>

class vpgl_fm_compute_8_point
{
 public:
  //: If precondition = true, points are conditioned prior to computation.
  vpgl_fm_compute_8_point( bool precondition = true ) :
      precondition_(precondition){};

  //: Compute from two sets of corresponding points.
  // Put the resulting matrix into fm, return true if successful.
  // Points pr are associated with the RHS of the fundamental matrix
  // while the points pl are associated with the LHS.
  bool compute( const vcl_vector< vgl_homg_point_2d<double> >& pr,
                const vcl_vector< vgl_homg_point_2d<double> >& pl,
                vpgl_fundamental_matrix<double>& fm );

 protected:
  bool precondition_;
};

#endif // vpgl_fm_compute_8_point_h_
