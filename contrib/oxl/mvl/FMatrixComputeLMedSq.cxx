#include "FMatrixComputeLMedSq.h"
#include <mvl/HomgOperator2D.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_double_2.h>
#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <vcl_algorithm.h>

FMatrixComputeLMedSq::FMatrixComputeLMedSq(bool rank2_truncate, int size)
{
  rank2_truncate_ = rank2_truncate;
  double std = 1.4826*(1.0 + 5.0/(size - 7));
  // This gives the inlier thresh^2 minus the Median term (M)
  inthresh_ = (2.5*std)*(2.5*std);
}

FMatrixComputeLMedSq::~FMatrixComputeLMedSq() {}

double FMatrixComputeLMedSq::calculate_term(vcl_vector<double>& residuals, vcl_vector<bool>& inlier_list, int& count) {
  double M = median(residuals);
  double thresh = inthresh_;
  thresh *= M;
  for(unsigned int i = 0; i < residuals.size(); i++) {
    if(residuals[i] < thresh) {
      inlier_list[i] = true;
      count++;
    } else {
      inlier_list[i] = false;
    }
  }
  return M;
}

double FMatrixComputeLMedSq::calculate_residual(HomgPoint2D& one, HomgPoint2D& two, FMatrix* F) {

  double ret = 0.0;

  HomgLine2D l1 = F->image2_epipolar_line(one);
  HomgLine2D l2 = F->image1_epipolar_line(two);
  ret += HomgOperator2D::perp_dist_squared(two, l1);
  ret += HomgOperator2D::perp_dist_squared(one, l2);

  return ret;
}

double FMatrixComputeLMedSq::median(vcl_vector<double> residuals) {
  double ret = 0.0;
  vcl_sort(residuals.begin(), residuals.end());
  int size = residuals.size();
  double s2 = (double)size / 2.0;
  if(s2 == 0.0) {
    ret = (residuals[(int)s2] + residuals[(int)s2-1]) / 2.0;
  } else {
    ret = residuals[(int)vcl_floor(s2)];
  }
  return ret;
}
