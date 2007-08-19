#include <vpgl/algo/vpgl_ortho_procrustes.h>
//:
// \file
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_trace.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_det.h>
#include <vnl/algo/vnl_svd.h>

//: only one constructor. X and Y must have dimensions 3 x N
vpgl_ortho_procrustes::
vpgl_ortho_procrustes(vnl_matrix<double> const& X,
                      vnl_matrix<double> const& Y)
  : cannot_compute_(false), computed_(false), s_(1.0), residual_(0)
{
  if (X.rows()!=3 || Y.rows()!=3||X.columns()!=Y.columns()){
    cannot_compute_ = true;
    vcl_cout << "Fatal error in vpgl_ortho_procrustes - unmatched pointsets\n";
    return;
  }
  X_ = X;
  Y_ = Y;
}

void vpgl_ortho_procrustes::compute()
{
  unsigned N = X_.columns();
  //remove the centroid of both point sets
  vnl_vector_fixed<double, 3> Cx, Cy;
  Cx.fill(0);   Cy.fill(0);
  for (unsigned c = 0; c<N; ++c)
  {
    vnl_vector_fixed<double, 3> vx, vy;
    for (unsigned r = 0; r<3; ++r)
    {
      vx[r] = X_[r][c];
      vy[r] = Y_[r][c];
    }
    Cx += vx;
    Cy += vy;
  }
  Cx/=N;   Cy/=N;
  vnl_matrix<double> Xm(3, N), Ym(3,N);
  double smx=0, smy=0;
  for (unsigned c = 0; c<N; ++c)
  {
    vnl_vector_fixed<double, 3> Sx, Sy;
    for (unsigned r = 0; r<3; ++r)
    {
      Xm[r][c] = X_[r][c]-Cx[r];
      Ym[r][c] = Y_[r][c]-Cy[r];
      Sx[r] = Xm[r][c];
      Sy[r] = Ym[r][c];
    }
    smx += Sx.squared_magnitude();
    smy += Sy.squared_magnitude();
  }

  double neu = vnl_trace<double, 3, 3>(Xm*(Xm.transpose()));
  double den = vnl_trace<double, 3, 3>(Ym*(Ym.transpose()));
  if (den!=0)
    s_ = vcl_sqrt(neu/den);

  //Normalize X, Y by the average radius
  double sigma_x = vcl_sqrt(smx/N), sigma_y = vcl_sqrt(smy/N);
  Xm /= sigma_x;
  Ym /= sigma_y;

  vnl_matrix<double> Xt = Xm.transpose();
  vnl_matrix_fixed<double, 3,3> M = Ym*Xt;
  vnl_svd<double> SVD(M);
  vnl_matrix_fixed<double, 3,3> U = SVD.U();
  vnl_matrix_fixed<double, 3,3> V = SVD.V();
  vnl_matrix_fixed<double, 3,3> Ut = U.transpose();
  vnl_matrix_fixed<double, 3, 3> T, temp;
  temp = V*Ut;
  T.fill(0);
  T[0][0]=1.0;
  T[1][1]=1.0;
  T[2][2]=vnl_det<double>(temp);
  vnl_matrix_fixed<double, 3,3> rr = V*T*Ut;
  R_ = vgl_rotation_3d<double>(rr);
  t_ = (1.0/s_)*Cx - rr*Cy;
  //compute the mean square residual
  residual_ = 0;
  for (unsigned c = 0; c<N; ++c)
  {
    vnl_vector_fixed<double, 3> x, y, diff;
    for (unsigned r = 0; r<3; ++r)
    {
      x[r]=X_[r][c];
      y[r]=Y_[r][c];
    }
    diff = s_*(rr*y + t_) - x;
    residual_ += diff.squared_magnitude();
  }
  residual_ /= (3*N);
  computed_ = true;
}

vgl_rotation_3d<double>  vpgl_ortho_procrustes::R()
{
  if (!computed_&&!cannot_compute_)
    this->compute();
  return R_;
}

double vpgl_ortho_procrustes::s()
{
  if (!computed_&&!cannot_compute_)
    this->compute();
  return s_;
}


double vpgl_ortho_procrustes::residual_mean_sq_error()
{
  if (!computed_&&!cannot_compute_)
    this->compute();
  return residual_;
}

vnl_vector_fixed<double, 3> vpgl_ortho_procrustes::t()
{
  if (!computed_&&!cannot_compute_)
    this->compute();
  return t_;
}
