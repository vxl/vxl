#include "brct_structure_estimator.h"

brct_structure_estimator::brct_structure_estimator(vnl_double_3x4 &P)
{
  A_.set_identity();
  P_ = P;
}

bugl_gaussian_point_3d<double> brct_structure_estimator::forward(\
    bugl_gaussian_point_3d<double>& state, \
    bugl_gaussian_point_2d<double>& observe)
{
  return state;
}
