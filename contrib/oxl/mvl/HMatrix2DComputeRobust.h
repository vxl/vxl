#ifndef HMatrix2DComputeRobust_h
#define HMatrix2DComputeRobust_h
//:
// \file
// \brief Robust HMatrix computation
//
// \verbatim
//  Modifications
//   22 Oct 2002 - Peter Vanroose - added vgl_homg_point_2d interface
// \endverbatim

#include <iostream>
#include <vector>
#include <vgl/vgl_fwd.h>
#include <mvl/HMatrix2DCompute4Point.h>
#include <mvl/HMatrix2D.h>
#include <mvl/AffineMetric.h>
#include <mvl/HomgPoint2D.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class HMatrix2DComputeRobust
{
 public:
  HMatrix2DComputeRobust();
  virtual ~HMatrix2DComputeRobust();

  bool compute(PairMatchSetCorner& matches, HMatrix2D* H);

  // Return a HMatrix2D computed as above.
  HMatrix2D compute(PairMatchSetCorner& matches);
  HMatrix2D compute(std::vector<HomgPoint2D>& points1, std::vector<HomgPoint2D>& points2);
  HMatrix2D compute(std::vector<vgl_homg_point_2d<double> >& points1,
                    std::vector<vgl_homg_point_2d<double> >& points2);

  // Data Access
  std::vector<int> get_basis() const { return basis_; }
  std::vector<double> get_residuals() const { return residuals_; }
  std::vector<bool> get_inliers() const { return inliers_; }

  virtual double calculate_term(std::vector<double>& residuals, std::vector<bool>& inlier_list, int& count);
  virtual double calculate_residual(HomgPoint2D& one, HomgPoint2D& two, HMatrix2D* H);
  virtual double calculate_residual(vgl_homg_point_2d<double>& one,
                                    vgl_homg_point_2d<double>& two,
                                    HMatrix2D* H);
  std::vector<double> calculate_residuals(std::vector<HomgPoint2D>& one, std::vector<HomgPoint2D>& two, HMatrix2D* H);
  std::vector<double> calculate_residuals(std::vector<vgl_homg_point_2d<double> >& one,
                                         std::vector<vgl_homg_point_2d<double> >& two,
                                         HMatrix2D* H);
  double stdev(std::vector<double>& residuals);

  double std_;
  std::vector<int> basis_;
  int data_size_;
  std::vector<double> residuals_;
  std::vector<bool> inliers_;
  //  AffineMetric metric_;
};

#endif // HMatrix2DComputeRobust_h
