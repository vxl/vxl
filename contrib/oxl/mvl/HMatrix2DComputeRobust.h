#ifndef HMatrix2DComputeRobust_h
#define HMatrix2DComputeRobust_h

//:
// \file
// \brief Robust HMatrix computation
//
// \verbatim
// Modifications
//    22 Oct 2002 - Peter Vanroose - added vgl_homg_point_2d interface
// \endverbatim
//
#include <mvl/HMatrix2DCompute4Point.h>
#include <mvl/HMatrix2D.h>
#include <mvl/AffineMetric.h>
#include <mvl/HomgPoint2D.h>
#include <vcl_vector.h>

class HMatrix2DComputeRobust {
public:
  HMatrix2DComputeRobust();
  virtual ~HMatrix2DComputeRobust();

  bool compute(PairMatchSetCorner& matches, HMatrix2D* H);

  // Return a HMatrix2D computed as above.
  HMatrix2D compute(PairMatchSetCorner& matches) { return HMatrix2DComputeRobust::compute(matches); }
  HMatrix2D compute(vcl_vector<HomgPoint2D>& points1, vcl_vector<HomgPoint2D>& points2) {
    return HMatrix2DComputeRobust::compute(points1, points2); }
  HMatrix2D compute(vcl_vector<vgl_homg_point_2d<double> >& points1,
                    vcl_vector<vgl_homg_point_2d<double> >& points2) {
    return HMatrix2DComputeRobust::compute(points1, points2); }

  // Data Access
  vcl_vector<int> get_basis() const { return basis_; }
  vcl_vector<double> get_residuals() const { return residuals_; }
  vcl_vector<bool> get_inliers() const { return inliers_; }

  virtual double calculate_term(vcl_vector<double>& residuals, vcl_vector<bool>& inlier_list, int& count);
  virtual double calculate_residual(HomgPoint2D& one, HomgPoint2D& two, HMatrix2D* H);
  virtual double calculate_residual(vgl_homg_point_2d<double>& one,
                                    vgl_homg_point_2d<double>& two,
                                    HMatrix2D* H);
  vcl_vector<double> calculate_residuals(vcl_vector<HomgPoint2D>& one, vcl_vector<HomgPoint2D>& two, HMatrix2D* H);
  vcl_vector<double> calculate_residuals(vcl_vector<vgl_homg_point_2d<double> >& one,
                                         vcl_vector<vgl_homg_point_2d<double> >& two,
                                         HMatrix2D* H);
  double stdev(vcl_vector<double>& residuals);

  double std_;
  vcl_vector<int> basis_;
  int data_size_;
  vcl_vector<double> residuals_;
  vcl_vector<bool> inliers_;
//  AffineMetric metric_;
};

#endif // HMatrix2DComputeRobust_h
