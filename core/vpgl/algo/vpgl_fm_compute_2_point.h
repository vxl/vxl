// This is core/vpgl/algo/vpgl_fm_compute_2_point.h
#ifndef vpgl_fm_compute_2_point_h_
#define vpgl_fm_compute_2_point_h_
//:
// \file
// \brief A 2 point algorithm for computing the fundamental matrix for translation from point correspondences
// \author J. L. Mundy
// \date March 21, 2007
//
// The point correspondences in relation to F are defined by
// \verbatim
//         [0    tz   -ty]
//     pl^t[-tz   0    tx]pr = 0
//         [ty   -tx    0]
// \endverbatim
// This computation assumes that the cameras differ by only a translation
// Normalization of point coordinates is performed if precondition is true
// In this case, the computation is modified to account for the normalization
// so that F is expressed in the original coordinate system.

#include <vpgl/vpgl_fundamental_matrix.h>
#include <vgl/vgl_homg_point_2d.h>

class vpgl_fm_compute_2_point
{
 public:
  //: If precondition = true, points are conditioned prior to computation.
  vpgl_fm_compute_2_point( bool precondition = true )
  : precondition_(precondition) {}

  //: Compute from two sets of corresponding points.
  // Put the resulting matrix into fm, return true if successful.
  // Points pr are associated with the RHS of the fundamental matrix
  // while the points pl are associated with the LHS.
  bool compute( const std::vector< vgl_homg_point_2d<double> >& pr,
                const std::vector< vgl_homg_point_2d<double> >& pl,
                vpgl_fundamental_matrix<double>& fm );

 protected:
  bool precondition_;
};

#endif // vpgl_fm_compute_2_point_h_
