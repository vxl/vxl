// This is brl/bbas/bvgl/tests/bvgl_test_h_matrix_2d_compute_4point.cxx
#include <vcl_iostream.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_point_2d.h>
#include <vnl/vnl_matrix_fixed.h>
#include <bvgl/bvgl_h_matrix_2d.h>
#include <bvgl/bvgl_h_matrix_2d_compute_4point.h>
#include <testlib/testlib_test.h>

void bvgl_test_h_matrix_2d_compute_4point()
{
  vcl_cout << "Test the recovery of an 2x scale transform using 4 points\n";
  vcl_vector<vgl_homg_point_2d<double> > points1, points2;

  //setup points in frame 1
  vgl_homg_point_2d<double> p10(0.0, 0.0, 1.0), p11(1.0, 0.0, 1.0);
  vgl_homg_point_2d<double> p12(0.0, 1.0, 1.0), p13(1.0, 1.0, 1.0);
  points1.push_back(p10); points1.push_back(p11);
  points1.push_back(p12); points1.push_back(p13);

  //setup points in frame 2
  vgl_homg_point_2d<double> p20(0.0, 0.0, 1.0), p21(2.0, 0.0, 1.0);
  vgl_homg_point_2d<double> p22(0.0, 2.0, 1.0), p23(2.0, 2.0, 1.0);
  points2.push_back(p20); points2.push_back(p21);
  points2.push_back(p22); points2.push_back(p23);

  bvgl_h_matrix_2d_compute_4point hc4p;
  bvgl_h_matrix_2d<double> H = hc4p.compute(points1, points2);
  vnl_matrix_fixed<double, 3, 3> M = H.get_matrix();
  vgl_homg_point_2d<double> hdiag(M[0][0], M[1][1], M[2][2]);
  vgl_point_2d<double> diag(hdiag);
  double distance = (diag.x()-2.0)*(diag.x()-2.0) +
                    (diag.y()-2.0)*(diag.y()-2.0);
  TEST_NEAR("The normalized upper diagonal = (2,2)", distance, 0.0, 1e-12);
  vcl_cout << "recover 2x scale matrix\n";
}

TESTMAIN(bvgl_test_h_matrix_2d_compute_4point);
