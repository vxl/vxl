#ifndef FMatrixComputeRobust_h
#define FMatrixComputeRobust_h
//:
// \file
// \brief Robust FMatrix computation
//
//  FMatrixComputeRobust is a base class for robust F Matrix calculations.
//  Relevant methods are abstracted to allow for sub-classing
//  of different robust variants.
//
// \author
//     David N. McKinnon, UQ I.R.I.S., 17.05.2001
//
// \verbatim
// Modifications
//    22 Oct 2002 - Peter Vanroose - added vgl_homg_point_2d interface
// \endverbatim
//
//-----------------------------------------------------------------------------

#include <mvl/FMatrixCompute.h>
#include <mvl/HomgPoint2D.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vcl_vector.h>

class FMatrixComputeRobust : public FMatrixCompute
{
 public:
  FMatrixComputeRobust();
  virtual ~FMatrixComputeRobust();

  // FMatrixCompute virtuals---------------------------------------------------

  //: Compute a fundamental matrix for a set of point matches.
  // This is the implemented method, other computes wrap this.
  bool compute(PairMatchSetCorner& matches, FMatrix* F);
  bool compute(vcl_vector<HomgPoint2D>& p1, vcl_vector<HomgPoint2D>& p2, FMatrix* F)
    { return FMatrixCompute::compute(p1,p2,F); }
  bool compute(vcl_vector<vgl_homg_point_2d<double> >& p1,
               vcl_vector<vgl_homg_point_2d<double> >& p2,
               FMatrix* F)
    { return FMatrixCompute::compute(p1,p2,F); }

  inline FMatrix compute(PairMatchSetCorner& p) { return FMatrixCompute::compute(p); }
  inline FMatrix compute(vcl_vector<HomgPoint2D>& p1, vcl_vector<HomgPoint2D>& p2)
    { return FMatrixCompute::compute(p1,p2); }
  inline FMatrix compute(vcl_vector<vgl_homg_point_2d<double> >& p1,
                         vcl_vector<vgl_homg_point_2d<double> >& p2)
    { return FMatrixCompute::compute(p1,p2); }

  // Data Access---------------------------------------------------------------

  vcl_vector<int> get_basis() const { return basis_; }
  HomgPoint2D get_epipole1() const { return epipole1_; }
  HomgPoint2D get_epipole2() const { return epipole2_; }
  vcl_vector<double> get_residuals() const { return residuals_; }
  vcl_vector<bool> get_inliers() const { return inliers_; }

  // These virtual functions must be implemented
  virtual double calculate_term(vcl_vector<double>& residuals, vcl_vector<bool>& inlier_list, int& count);
  virtual double calculate_residual(HomgPoint2D& one, HomgPoint2D& two, FMatrix* F);
  virtual double calculate_residual(vgl_homg_point_2d<double>& one,
                                    vgl_homg_point_2d<double>& two, FMatrix* F);
  vcl_vector<double> calculate_residuals(vcl_vector<HomgPoint2D>& one, vcl_vector<HomgPoint2D>& two, FMatrix* F);
  vcl_vector<double> calculate_residuals(vcl_vector<vgl_homg_point_2d<double> >& one,
                                         vcl_vector<vgl_homg_point_2d<double> >& two,
                                         FMatrix* F);
  double stdev(vcl_vector<double>& residuals);

  bool rank2_truncate_;
  int row_;
  int col_;
  double std_;
  vcl_vector<int> basis_;
  HomgPoint2D epipole1_;
  HomgPoint2D epipole2_;
  int data_size_;
  vcl_vector<double> residuals_;
  vcl_vector<bool> inliers_;
  double inthresh_;
};

#endif // FMatrixComputeRobust_h
