/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "mvl_five_point_camera_pencil.h"

#include <vcl_cmath.h>
#include <vcl_cstdlib.h>

#include <vnl/vnl_vector.h>
#include <vnl/algo/vnl_svd.h>

bool mvl_five_point_camera_pencil(double const xs[5],
                                  double const ys[5],
                                  vnl_matrix<double> *A,
                                  vnl_matrix<double> *B)
{
#if 0
  A->resize(3, 4);
  B->resize(3, 4);
#else
  A->assert_size(3, 4);
  B->assert_size(3, 4);
#endif

  // normalization: translate the last point to the origin
  // and scale each other point by the hypot() of the first
  // two components.
  vnl_matrix<double> design(3, 4);
  for (int i=0; i<4; ++i) {
    double u = xs[i] - xs[4];
    double v = ys[i] - ys[4];
    double one_over_r = 1.0 / vcl_sqrt(u*u + v*v);
    if (! one_over_r)
      return false;
    design[0][i] = u * one_over_r;
    design[1][i] = v * one_over_r;
    design[2][i] = one_over_r;
  }

  // Since we know the coordinates of the first five world points, we
  // know the columns of the camera matrix P, up to scale:
  //   P= [ l1*x1 | l2*x2 | l3*x3 | l4*x4 ]
  //
  // and also:
  //   x5 ~ l1*x1 + l2*x2 + l3*x3 + l4*x4
  //
  // The space of such l = [l1; l2; l3; l4] can be found as the 2D
  // nullspace of linear system
  //   x5 \times [ x1; x2; x3; x4 ] l = 0
  // Since x5 = [ 0; 0; 1 ], this just means we take the nullspace
  // of the leading 2x4 submatrix of [ x1; x2; x3; x4 ].

  //vcl_cerr << "design = " << matlab(design) << vcl_endl;
  vnl_svd<double> svd(design.extract(2, 4));
  //vcl_cerr << "singvals = " << svd.W() << vcl_endl;

  // get basis for nullspace :
  vnl_vector<double> a(svd.V().get_column(2));
  vnl_vector<double> b(svd.V().get_column(3));
  //vcl_cerr << "a = " << matlab(a) << vcl_endl;
  //vcl_cerr << "b = " << matlab(b) << vcl_endl;

  // make the 3x4 cameras :
  *A = design;
  A->scale_column(0, a[0]);
  A->scale_column(1, a[1]);
  A->scale_column(2, a[2]);
  A->scale_column(3, a[3]);
  *B = design;
  B->scale_column(0, b[0]);
  B->scale_column(1, b[1]);
  B->scale_column(2, b[2]);
  B->scale_column(3, b[3]);
  //vcl_cerr << "A = " << matlab(A) << vcl_endl;
  //vcl_cerr << "B = " << matlab(B) << vcl_endl;
  //vcl_exit(0);

  // translate the last point back again.
  for (int i=0; i<4; ++i) {
    (*A)[0][i] += xs[4] * (*A)[2][i];
    (*A)[1][i] += ys[4] * (*A)[2][i];

    (*B)[0][i] += xs[4] * (*B)[2][i];
    (*B)[1][i] += ys[4] * (*B)[2][i];
  }

  return true;
}

bool mvl_five_point_camera_pencil_parameters(vnl_matrix<double> const &A,
                                             vnl_matrix<double> const &B,
                                             vnl_vector<double> const &X,
                                             double u, double v,
                                             double st[2],
                                             double res[2])
{
  // project.
  vnl_vector<double> AX(A * X);
  vnl_vector<double> BX(B * X);

  // translate (u, v) to origin (0, 0)
  AX[0] -= u * AX[2]; AX[1] -= v * AX[2];
  BX[0] -= u * BX[2]; BX[1] -= v * BX[2];

  // this line wants to pass through the origin. alas, it might not.
  vnl_vector<double> l = cross_3d(AX, BX);

  if (st) {
    double s =  (l[1]*BX[0] - l[0]*BX[1]);
    double t = -(l[1]*AX[0] - l[0]*AX[1]);
#if 1
    double n = vcl_sqrt(s*s + t*t);
    if (n == 0)
      return false;
    st[0] = s/n;
    st[1] = t/n;
#else
    st[0] = s;
    st[1] = t;
#endif

#if 0
    vcl_cerr << A << vcl_endl;
    vcl_cerr << B << vcl_endl;
    vcl_cerr << X << vcl_endl;
    vcl_cerr << u << ' ' << v << vcl_endl;
    vcl_cerr << st[0] << ' ' << st[1] << vcl_endl;
    vcl_exit(0);
#endif
  }

  if (res) {
    // squared magnitude of the normal vector :
    double nn = l[0]*l[0] + l[1]*l[1];
    if (nn == 0)
      return false; // if it's the line at infinity, fail

    // the residuals are :
    res[0] = l[0] * l[2] / nn;
    res[1] = l[1] * l[2] / nn;
  }

  return true;
}
