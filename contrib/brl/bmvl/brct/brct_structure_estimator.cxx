#include "brct_structure_estimator.h"
#include <vnl/vnl_inverse.h>
#include "brct_algos.h"

brct_structure_estimator::brct_structure_estimator(vnl_double_3x4 &P)
{
  A_.set_identity();

  // set small Q_ for tuning process
  Q_.set_identity();

  P_ = P;
}

bugl_gaussian_point_3d<double> brct_structure_estimator::forward(
    bugl_gaussian_point_3d<double>& state,
    bugl_gaussian_point_2d<double>& observe)
{
  vnl_double_3 Y(state.x(), state.y(), state.z());

  Y = A_*Y;

  vnl_double_3x3 Q = A_*state.get_covariant_matrix()*A_.transpose() + Q_;

  vnl_double_2x3 H = get_H_matrix(Y);
  vnl_double_2 z_pred = brct_algos::projection_3d_point(P_, Y);
  vnl_matrix_fixed<double, 3, 2> G = Q*H.transpose()*
    vnl_inverse(H*Q*H.transpose() + observe.get_covariant_matrix());
  Q = (vnl_double_3x3().set_identity() - G*H)*Q;
  state.set_covariant_matrix(Q);

  vnl_double_2 z(observe.x(), observe.y());
  Y = Y + G*(z - z_pred);

  return  bugl_gaussian_point_3d<double> (Y[0], Y[1], Y[2], Q);
}

vnl_double_2x3 brct_structure_estimator::get_H_matrix(vnl_double_3& Y)
{
  vnl_double_2x3 H;
  // sum_{k=1}^{3} {P_{3k}Y_k + P_{34}
  double t = 0;
  for (int k=0; k<3; k++)
    t += P_[2][k]*Y[k];
  t+=P_[2][3];

  // H
  for (int i=0; i<2; i++){
    double t1 = 0;
    for (int k = 0; k<3; k++)
      t1 += P_[i][k]*Y[k];
    t1+= P_[i][3];

    for (int j=0; j<3; j++)
      H[i][j] = P_[i][j] / t - P_[2][j]* t1 / t/t;
  }

  return H;
}

