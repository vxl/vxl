// This is brl/bbas/bvgl/tests/bvgl_test_h_matrix_2d_compute_linear.cxx
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vgl/vgl_homg_point_2d.h>
#include <bvgl/bvgl_h_matrix_2d.h>
#include <bvgl/bvgl_h_matrix_2d_compute_linear.h>
#include <testlib/testlib_test.h>

void bvgl_test_h_matrix_2d_compute_linear()
{
  vcl_cout << "Test the recovery of an identity transform\n";
  vcl_vector<vgl_homg_point_2d<double> > points1, points2;

  //setup points in frame 1
  vgl_homg_point_2d<double> p10(0.0, 0.0, 1.0), p11(1.0, 0.0, 1.0);
  vgl_homg_point_2d<double> p12(0.0, 1.0, 1.0), p13(1.0, 1.0, 1.0);
  vgl_homg_point_2d<double> p14(0.5, 0.5, 1.0), p15(0.75, 0.75, 1.0);
  points1.push_back(p10); points1.push_back(p11); points1.push_back(p12);
  points1.push_back(p13); points1.push_back(p14); points1.push_back(p15);

  //setup points in frame 2
  vgl_homg_point_2d<double> p20(0.0, 0.0, 1.0), p21(2.0, 0.0, 1.0);
  vgl_homg_point_2d<double> p22(0.0, 2.0, 1.0), p23(2.0, 2.0, 1.0);
  vgl_homg_point_2d<double> p24(1.0, 1.0, 1.0), p25(1.5, 1.5, 1.0);
  points2.push_back(p20); points2.push_back(p21); points2.push_back(p22);
  points2.push_back(p23); points2.push_back(p24); points2.push_back(p25);

  bvgl_h_matrix_2d_compute_linear hmcl;
  bvgl_h_matrix_2d<double> H = hmcl.compute(points1, points2);
  vcl_cout << "The resulting transform\n" << H << '\n';
}

TESTMAIN(bvgl_test_h_matrix_2d_compute_linear);
