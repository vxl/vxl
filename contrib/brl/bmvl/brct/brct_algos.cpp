// brct_algos.cpp: implementation of the brct_algos class.
//
//////////////////////////////////////////////////////////////////////

#include "brct_algos.h"
#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_4.h>
#include <vnl/vnl_double_4x4.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_interpolator_sptr.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_edgel_chain_sptr.h>
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

vgl_point_3d<double> brct_algos::triangulate_3d_point(const vgl_point_2d<double>& x1, const vnl_double_3x4& P1, \
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

vgl_point_3d<double> brct_algos::bundle_reconstruct_3d_point(vcl_vector<vnl_double_2> &pts, vcl_vector<vnl_double_3x4> &Ps)
{
  int nviews = pts.size();
  assert(pts.size() == Ps.size());

  vnl_matrix<double> A(2*nviews, 4, 0.0);

  for (int v = 0; v<nviews; v++)
    for (int i=0; i<4; i++) {
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

  int size = pts_3d.size();

  for (int i=0; i<size; i++){
    vgl_point_3d<double> &pt = pts_3d[i];
    box->add_point(pt.x(), pt.y(), pt.z());
  }

  return box;
}

void brct_algos::add_box_vrml(double xmin, double ymin, double zmin, double xmax, double ymax, double zmax)
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


vnl_double_2 brct_algos::projection_3d_point(const vnl_double_3x4 &P, const vnl_double_3 &X)
{
    vnl_double_2 z;
    double t1 = 0;
    for (int k=0; k<3; k++)
      t1 += P[2][k]*X[k];
    t1 += P[2][3];

    for (int i=0; i<2; i++)
    {
      double t0 =0;
      for (int k=0; k<3; k++)
        t0 += P[i][k]*X[k];
      t0 += P[i][3];

      z[i] = t0/t1;
    }

    return z;
}

