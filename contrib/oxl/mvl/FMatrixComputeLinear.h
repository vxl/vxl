// This is oxl/mvl/FMatrixComputeLinear.h
#ifndef FMatrixComputeLinear_h_
#define FMatrixComputeLinear_h_
//:
// \file
// \brief Hartley 8-point fundamental matrix fit
//
//    FMatrixComputeLinear is a subclass of FMatrixCompute, and implements
//    the ``8-point'' or linear least squares algorithm for estimation of
//    the fundamental matrix.
//
//    Points are preconditioned as described in [Hartley, ``In defense of
//    the 8-point algorithm'', ICCV95], and the resulting F matrix is rank-2
//    truncated.  The conditioning and truncation are optional and may be
//    omitted.
//
//    Note: As with any nonrobust algorithm, mismatches in the input data
//    may severely effect the result.
//
// \example examples/exampleFMatrixCompute.cxx
//
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 21 Aug 96
//
// \verbatim
// Modifications
//    22 Oct 2002 - Peter Vanroose - added vgl_homg_point_2d interface
// \endverbatim
//
//-----------------------------------------------------------------------------

#include <vgl/vgl_fwd.h>
#include <mvl/FMatrixCompute.h>
#include <mvl/FMatrix.h>

class FMatrixComputeLinear : public FMatrixCompute
{
  bool precondition_;
  bool rank2_truncate_;
 public:
  //: Initialize FMatrixComputeLinear object.
  //  If precondition = false, points are not conditioned prior to computation.
  // If rank2_truncate = false, the resulting solution is not forced to rank 2
  // using the vnl_svd<double>.
  FMatrixComputeLinear(bool precondition = true, bool rank2_truncate = true);

  // Computations--------------------------------------------------------------

  //: Compute a fundamental matrix for a set of point matches.
  //
  // Return false if the calculation fails or there are fewer than eight point
  // matches in the list.
  //
  bool compute(PairMatchSetCorner&, FMatrix* F) override;

  //: Interface to above using arrays of HomgPoint2D.
  //  Makes a PairMatchSetCorner, and then calls the compute method above.
  bool compute(std::vector<HomgPoint2D>&, std::vector<HomgPoint2D>&, FMatrix* F) override;

  //: Interface to above using arrays of vgl_homg_point_2d.
  //  Makes a PairMatchSetCorner, and then calls the compute method above.
  bool compute(std::vector<vgl_homg_point_2d<double> >&,
               std::vector<vgl_homg_point_2d<double> >&,
               FMatrix& F) override;

  //: Interface to above using preconditioned points
  bool compute_preconditioned(std::vector<HomgPoint2D>&, std::vector<HomgPoint2D>&, FMatrix* F);

  //: Interface to above using preconditioned points
  bool compute_preconditioned(std::vector<vgl_homg_point_2d<double> >&,
                              std::vector<vgl_homg_point_2d<double> >&,
                              FMatrix& F);

  inline FMatrix compute(PairMatchSetCorner& p) { return FMatrixCompute::compute(p); }
  inline FMatrix compute(std::vector<HomgPoint2D>& p1, std::vector<HomgPoint2D>& p2)
  { return FMatrixCompute::compute(p1,p2); }
  inline FMatrix compute(std::vector<vgl_homg_point_2d<double> >& p1,
                         std::vector<vgl_homg_point_2d<double> >& p2)
  { return FMatrixCompute::compute(p1,p2); }
};

#endif // FMatrixComputeLinear_h_
