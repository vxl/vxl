// This is brl/bbas/bvgl/tests/bvgl_test_norm_trans_2d.cxx
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_point_2d.h>
#include <bvgl/bvgl_h_matrix_2d.h>
#include <bvgl/bvgl_norm_trans_2d.h>
#include <testlib/testlib_test.h>

void bvgl_test_norm_trans_2d()
{
  int Npts = 100;
  bvgl_norm_trans_2d<double> nt;
  vcl_cout << "testing normalizing transform for points\n";
  vcl_vector<vgl_homg_point_2d<double> > points;
  for (int i = 0; i<Npts; i++)
  {
    double x = i*1.01, y = i*1.2;
    vgl_homg_point_2d<double> p(x, y);
    points.push_back(p);
  }
  nt.compute_from_points(points);
  vcl_cout << "Normalizing Transform\n" << nt << '\n';
  double cx=0, cy=0;
  for (int i=0, j=0; i<Npts; i++,j++)
  {
    if (j>9) j = 0;
    vgl_point_2d<double> p(nt(points[i]));
    if (!j)
    {
      vcl_cout << "p[" << i << "] =  " << vgl_point_2d<double>(points[i])
               << '\n' << "pnorm[" << i << "] =  " << p << '\n';
    }
    cx += p.x();
    cy += p.y();
  }
  vcl_cout << "Residual Center(" << cx/Npts << ' ' << cy/Npts << ")\n";
  TEST_NEAR("...", 0.0, cx/Npts, 1e-4);
  TEST_NEAR("...", 0.0, cy/Npts, 1e-4);
}

TESTMAIN(bvgl_test_norm_trans_2d);
