//:
// \file
// \brief implementation of the brct_algos class.
//
//////////////////////////////////////////////////////////////////////

#include "brct_algos.h"
#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_4.h>
#include <vnl/vnl_double_4x4.h>
#include <vnl/vnl_double_2x3.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_interpolator_sptr.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_edgel_chain_sptr.h>
#include <vdgl/vdgl_digital_curve.h>
#include <bbas/bdgl/bdgl_curve_algs.h>
#include <vsol/vsol_box_3d.h>
#include <vcl_cassert.h>

// Construction/Destruction

brct_algos::brct_algos()
{
}

brct_algos::~brct_algos()
{
}

vgl_point_3d<double> brct_algos::triangulate_3d_point(const vgl_point_2d<double>& x1, const vnl_double_3x4& P1,
                                                      const vgl_point_2d<double>& x2, const vnl_double_3x4& P2)
{
  vnl_double_4x4 A;

  for (int i=0; i<4; i++){
    A[0][i] = x1.x()*P1[2][i] - P1[0][i];
    A[1][i] = x1.y()*P1[2][i] - P1[1][i];
    A[2][i] = x2.x()*P2[2][i] - P2[0][i];
    A[3][i] = x2.y()*P2[2][i] - P2[1][i];
  }

  vnl_svd<double> svd_solver(A);
  vnl_double_4 p = svd_solver.nullvector();
  return vgl_homg_point_3d<double>(p[0],p[1],p[2],p[3]);
}

vgl_point_2d<double> brct_algos::projection_3d_point(const vgl_point_3d<double> & x, const vnl_double_3x4& P)
{
  vnl_double_4 X(x.x(),x.y(),x.z(),1.0);
  vnl_double_3 t = P*X;
  return vgl_homg_point_2d<double>(t[0],t[1],t[2]);
}

//: project a 3d gaussian distribution into a 2d gaussian
bugl_gaussian_point_2d<double> brct_algos::project_3d_point(const vnl_double_3x4 &P,
                                                            const bugl_gaussian_point_3d<double> & X)
{
  vnl_double_4 Y(X.x(),X.y(),X.z(),1.0);
  vnl_double_3 u = P*Y;

  // compute weak prospect matrix, i.e., the car is far away from the camera
  vnl_double_2x3 H;

  // sum_{k=1}^{3} {P_{3k}Y_k + P_{34}
  double t = P[2][3];
  for (int k=0; k<3; k++)
    t += P[2][k]*Y[k];

  // 
  for (int i=0; i<2; i++){
    double t1 = P[i][3];
    for (int k = 0; k<3; k++)
      t1 += P[i][k]*Y[k];

    for (int j=0; j<3; j++)
      H[i][j] = P[i][j] / t - P[2][j]* t1 / t/t;
  }

  vnl_matrix_fixed<double, 3, 3> Sigma3d = X.get_covariant_matrix();
  vnl_matrix_fixed<double, 2, 2> Sigma2d = H*Sigma3d*H.transpose();

  vgl_homg_point_2d<double> hp2d(u[0], u[1], u[2]);
  vgl_point_2d<double> p2d(hp2d);
  return bugl_gaussian_point_2d<double>(p2d, Sigma2d);
}

vgl_point_3d<double> brct_algos::bundle_reconstruct_3d_point(vcl_vector<vnl_double_2> &pts, vcl_vector<vnl_double_3x4> &Ps)
{
  assert(pts.size() == Ps.size());
  unsigned int nviews = pts.size();

  vnl_matrix<double> A(2*nviews, 4, 0.0);

  for (unsigned int v = 0; v<nviews; v++)
    for (unsigned int i=0; i<4; i++) {
      A[2*v  ][i] = pts[v][0]*Ps[v][2][i] - Ps[v][0][i];
      A[2*v+1][i] = pts[v][1]*Ps[v][2][i] - Ps[v][1][i];
    }
  vnl_svd<double> svd_solver(A);
  vnl_double_4 p = svd_solver.nullvector();
  return vgl_homg_point_3d<double>(p[0],p[1],p[2],p[3]);
}

vsol_box_3d_sptr brct_algos::get_bounding_box(vcl_vector<vgl_point_3d<double> > &pts_3d)
{
  vsol_box_3d_sptr box = new vsol_box_3d;

  unsigned int size = pts_3d.size();

  for (unsigned int i=0; i<size; i++) {
    vgl_point_3d<double> &pt = pts_3d[i];
    box->add_point(pt.x(), pt.y(), pt.z());
  }

  return box;
}

void brct_algos::add_box_vrml(double /*xmin*/, double /*ymin*/, double /*zmin*/, double /*xmax*/, double /*ymax*/, double /*zmax*/)
{
  vcl_cerr << "brct_algos::add_box_vrml() NYI\n"; // TODO
}

vgl_point_2d<double> brct_algos::closest_point(vdgl_digital_curve_sptr dc, vgl_point_2d<double> pt)
{
  vdgl_interpolator_sptr interp = dc->get_interpolator();
  vdgl_edgel_chain_sptr ec = interp->get_edgel_chain();

  int i = bdgl_curve_algs::closest_point(ec, pt.x(), pt.y());

  return (*ec)[i].get_pt();
}

vgl_point_2d<double> brct_algos::most_possible_point(vdgl_digital_curve_sptr dc, bugl_gaussian_point_2d<double> &pt)
{
  vdgl_interpolator_sptr interp = dc->get_interpolator();
  vdgl_edgel_chain_sptr ec = interp->get_edgel_chain();

  if (!ec)
  { 
    vcl_cout<<"In brct_algos::most_possible_point(...) - warning, null chain\n";
    return vgl_point_2d<double>();
  }

  double maxd = -100;
  int N = ec->size();
  vgl_point_2d<double> pmax;

  for (int i=0; i<N; i++)
  {
    vgl_point_2d<double> p = (*ec)[i].get_pt();
    double d = pt.prob_at(p);
    if (d>maxd)
    {
      maxd = d;
      pmax = p;
    }
  }

  return pmax;
}
 
vnl_double_2 brct_algos::projection_3d_point(const vnl_double_3x4 &P, const vnl_double_3 &X)
{
  double t1 = P[2][3];
  for (int k=0; k<3; k++)
    t1 += P[2][k]*X[k];

  vnl_double_2 z;
  for (int i=0; i<2; i++)
  {
    double t0 = P[i][3];
    for (int k=0; k<3; k++)
      t0 += P[i][k]*X[k];

    z[i] = t0/t1;
  }

  return z;
}
