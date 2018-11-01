// This is oxl/mvl/FMPlanarNonLinFun.h
#ifndef FMPlanarNonLinFun_h_
#define FMPlanarNonLinFun_h_
//:
// \file
// \brief a class that contains the functions required for FMPlanarComputeLinear.
//
// \author
//   Martin Armstrong, Oxford 21/11/96
//
// \verbatim
//  Modifications
//   22 Oct 2002 - Peter Vanroose - added vgl_homg_point_2d interface
// \endverbatim
//
//-----------------------------------------------------------------------------

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_least_squares_function.h>
#include <vnl/vnl_double_3x3.h>
#include <vgl/vgl_homg_point_2d.h>
#include <mvl/HomgPoint2D.h>
#include <mvl/HomgMetric.h>
#include <mvl/HomgNorm2D.h>

class FMatrixPlanar;
class ImageMetric;

class FMPlanarNonLinFun : public vnl_least_squares_function
{
  int data_size_;

  std::vector<vgl_homg_point_2d<double> > points1_;
  std::vector<vgl_homg_point_2d<double> > points2_;

  HomgNorm2D normalized_;

  vnl_double_3x3 denorm_matrix_;
  vnl_double_3x3 denorm_matrix_inv_;

#if 0 // unused ?!
  double outlier_distance_squared_;
  int terminate_count_;
#endif

  HomgMetric image_metric1_;
  HomgMetric image_metric2_;

 public:
  //: Initialize object, will fit F to points1,2 using imagemetrics.
  //  Rejecting points > outlier_distance_squared from epipolar lines
  FMPlanarNonLinFun(const ImageMetric*, const ImageMetric*,
                    double outlier_distance_squared,
                    std::vector<vgl_homg_point_2d<double> >& points1,
                    std::vector<vgl_homg_point_2d<double> >& points2);
  FMPlanarNonLinFun(const ImageMetric*, const ImageMetric*,
                    double outlier_distance_squared,
                    std::vector<HomgPoint2D>& points1,
                    std::vector<HomgPoint2D>& points2);

  bool compute(FMatrixPlanar* F);

  //: The virtual function from vnl_levenberg_marquardt
  void f(vnl_vector<double> const& x, vnl_vector<double>& fx) override;

  // Helpers-------------------------------------------------------------------
 private:
  void fmatrix_to_params(const FMatrixPlanar& F, vnl_vector<double>& params);
  FMatrixPlanar params_to_fmatrix(const vnl_vector<double>& params);

  void fmatrix_to_params_awf(const FMatrixPlanar& F, vnl_vector<double>& params);
  FMatrixPlanar params_to_fmatrix_awf(const vnl_vector<double>& params);

  void fmatrix_to_params_mna(const FMatrixPlanar& F, vnl_vector<double>& params);
  FMatrixPlanar params_to_fmatrix_mna(const vnl_vector<double>& params);
};

#endif // FMPlanarNonLinFun_h_
