#ifndef FMatrixComputeRobust_h_
#define FMatrixComputeRobust_h_
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
//  Modifications
//   22 Oct 2002 - Peter Vanroose - added vgl_homg_point_2d interface
// \endverbatim
//
//-----------------------------------------------------------------------------

#include <iostream>
#include <vector>
#include <mvl/FMatrixCompute.h>
#include <mvl/HomgPoint2D.h>
#include <vgl/vgl_homg_point_2d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class FMatrixComputeRobust : public FMatrixCompute
{
 public:
  FMatrixComputeRobust();
  ~FMatrixComputeRobust() override;

  // FMatrixCompute virtuals---------------------------------------------------

  //: Compute a fundamental matrix for a set of point matches.
  // This is the implemented method, other computes wrap this.
  bool compute(PairMatchSetCorner& matches, FMatrix* F) override;
  bool compute(std::vector<HomgPoint2D>& p1, std::vector<HomgPoint2D>& p2, FMatrix* F) override
    { return FMatrixCompute::compute(p1,p2,F); }
  bool compute(std::vector<vgl_homg_point_2d<double> >& p1,
               std::vector<vgl_homg_point_2d<double> >& p2,
               FMatrix& F) override
    { return FMatrixCompute::compute(p1,p2,F); }

  inline FMatrix compute(PairMatchSetCorner& p) { return FMatrixCompute::compute(p); }
  inline FMatrix compute(std::vector<HomgPoint2D>& p1, std::vector<HomgPoint2D>& p2)
    { return FMatrixCompute::compute(p1,p2); }
  inline FMatrix compute(std::vector<vgl_homg_point_2d<double> >& p1,
                         std::vector<vgl_homg_point_2d<double> >& p2)
    { return FMatrixCompute::compute(p1,p2); }

  // Data Access---------------------------------------------------------------

  std::vector<int> get_basis() const { return basis_; }
  HomgPoint2D get_epipole1() const { return epipole1_; }
  HomgPoint2D get_epipole2() const { return epipole2_; }
  std::vector<double> get_residuals() const { return residuals_; }
  std::vector<bool> get_inliers() const { return inliers_; }

  // These virtual functions must be implemented
  virtual double calculate_term(std::vector<double>& residuals, std::vector<bool>& inlier_list, int& count);
  virtual double calculate_residual(HomgPoint2D& one, HomgPoint2D& two, FMatrix* F);
  virtual double calculate_residual(vgl_homg_point_2d<double>& one,
                                    vgl_homg_point_2d<double>& two, FMatrix* F);
  std::vector<double> calculate_residuals(std::vector<HomgPoint2D>& one, std::vector<HomgPoint2D>& two, FMatrix* F);
  std::vector<double> calculate_residuals(std::vector<vgl_homg_point_2d<double> >& one,
                                         std::vector<vgl_homg_point_2d<double> >& two,
                                         FMatrix* F);
  double stdev(std::vector<double>& residuals);
 protected:
  bool rank2_truncate_;
  double inthresh_;
  double std_;
 private:
  std::vector<int> basis_;
  HomgPoint2D epipole1_;
  HomgPoint2D epipole2_;
  int data_size_;
  std::vector<double> residuals_;
  std::vector<bool> inliers_;
};

#endif // FMatrixComputeRobust_h_
