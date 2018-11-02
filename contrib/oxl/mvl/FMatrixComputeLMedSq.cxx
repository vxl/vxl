#include <iostream>
#include <algorithm>
#include "FMatrixComputeLMedSq.h"
#include <mvl/HomgOperator2D.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/algo/vgl_homg_operators_2d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

FMatrixComputeLMedSq::FMatrixComputeLMedSq(bool rank2_truncate, int size)
{
  rank2_truncate_ = rank2_truncate;
  double std = 1.4826*(1.0 + 5.0/(size - 7));
  // This gives the inlier thresh^2 minus the Median term (M)
  inthresh_ = (2.5*std)*(2.5*std);
}

FMatrixComputeLMedSq::~FMatrixComputeLMedSq() = default;

double FMatrixComputeLMedSq::calculate_term(std::vector<double>& residuals, std::vector<bool>& inlier_list, int& count)
{
  double M = median(residuals);
  double thresh = inthresh_;
  thresh *= M;
  for (unsigned int i = 0; i < residuals.size(); i++) {
    if (residuals[i] < thresh) {
      inlier_list[i] = true;
      count++;
    } else {
      inlier_list[i] = false;
    }
  }
  return M;
}

double FMatrixComputeLMedSq::calculate_residual(vgl_homg_point_2d<double>& one,
                                                vgl_homg_point_2d<double>& two,
                                                FMatrix* F)
{
  vgl_homg_line_2d<double> l1 = F->image2_epipolar_line(one);
  vgl_homg_line_2d<double> l2 = F->image1_epipolar_line(two);
  return vgl_homg_operators_2d<double>::perp_dist_squared(two, l1)
      +  vgl_homg_operators_2d<double>::perp_dist_squared(one, l2);
}

double FMatrixComputeLMedSq::calculate_residual(HomgPoint2D& one, HomgPoint2D& two, FMatrix* F)
{
  HomgLine2D l1 = F->image2_epipolar_line(one);
  HomgLine2D l2 = F->image1_epipolar_line(two);
  return HomgOperator2D::perp_dist_squared(two, l1)
       + HomgOperator2D::perp_dist_squared(one, l2);
}

double FMatrixComputeLMedSq::median(std::vector<double> residuals)
{
  std::sort(residuals.begin(), residuals.end());
  int size = residuals.size();
  int s2 = size / 2;
  return size == 0 ? 0.0 : size%2 ? residuals[s2] :
         (residuals[s2] + residuals[s2-1]) * 0.5;
}
