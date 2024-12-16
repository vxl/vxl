// This is core/vpgl/algo/vpgl_affine_fm_compute_5_point.h
#ifndef vpgl_affine_fm_compute_5_point_h_
#define vpgl_affine_fm_compute_5_point_h_
//:
// \file
// \brief The 5 point algorithm for affine fundamental matrix from point correspondences using robust ransac
// \author J.L. Mundy
// \date December 30 2022
//
//  The point correspondences in relation to Fa are defined by
//     $pl^t[Fa]pr = 0$
//
// \verbatim
//  Modifications
//  None
// \endverbatim
//
// the algorithm uses the MUSE* robust estimator to filter out invalid correspondences
// *J. Miller and C. Stewart, MUSE: Robust Surface Fitting using Unbiased Scale Estimates, Proc. CVPR  1996
// a minimum of 5 points are required but designed to handle 1000s of noisy correspondences
//
#include <vgl/vgl_point_2d.h>
#include <vpgl/vpgl_affine_fundamental_matrix.h>

class vpgl_affine_fm_compute_5_point
{
public:
  //: If precondition = true, points are conditioned prior to computation.
  vpgl_affine_fm_compute_5_point(bool precondition = true, bool verbose = false)
    : precondition_(precondition)
    , verbose_(verbose)
  {}

  //: Compute from two sets of corresponding points.
  // Put the resulting matrix into fm, return true if successful.
  // Points pr are associated with the RHS of the fundamental matrix
  // while the points pl are associated with the LHS.
  // Note, Euclidan vgl points for affine geometry
  bool
  compute(const std::vector<vgl_point_2d<double>> & pr,
          const std::vector<vgl_point_2d<double>> & pl,
          vpgl_fundamental_matrix<double> & fm) const;

protected:
  bool verbose_;
  bool precondition_;
};

#endif // vpgl_affine_fm_compute_5_point_h_
