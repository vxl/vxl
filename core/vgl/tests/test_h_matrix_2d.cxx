//:
// \file
// \author Joseph Mundy
// \date  March 27, 2003

#include <testlib/testlib_test.h>
#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_point_2d.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vgl/algo/vgl_h_matrix_2d_compute_linear.h>
#include <vgl/algo/vgl_h_matrix_2d_compute_4point.h>

static void test_identity_transform()
{
  vcl_cout << "Testing identity transform on point\n";
  vnl_matrix_fixed<double, 3, 3> M;
  M.set_identity();
  vgl_h_matrix_2d<double> Id(M);
  vgl_homg_point_2d<double> p(3,2,1), pp, ppp;
  pp = Id(p);
  vcl_cout << "Id \n" << Id << "\n"
           << "p" << p << " , Id(p) = pp " << pp << "\n";
  vgl_point_2d<double> xp(p), xpp(pp);
  double distance = vcl_sqrt((xp.x()-xpp.x())*(xp.x()-xpp.x()) +
                             (xp.y()-xpp.y())*(xp.y()-xpp.y()));
  TEST_NEAR("identity",distance , 0.0, 1e-06);
}
static void test_perspective_transform()
{
  vcl_cout << "Testing identity transform on point\n";
  vnl_matrix_fixed<double, 3, 3> M;
  vgl_homg_point_2d<double> p(3,2,1), pp, ppp;
  M.put(0,0,1);    M.put(0,1,2);   M.put(0,2,1);
  M.put(1,0,0.5);  M.put(1,1,-2);  M.put(1,2,1.5);
  M.put(2,0,0.25); M.put(2,1,3);   M.put(2,2,1.75);
  vgl_h_matrix_2d<double> Tproj(M);
  pp = Tproj(p);
  ppp = Tproj.preimage(pp);
  vcl_cout << "Tproj \n" << Tproj << "\n"
           << "p" << p << " , Tproj(p) = pp " << pp << "\n"
           << " , Tproj.preimage(pp) = ppp " << ppp << "\n";
  vgl_point_2d<double> xp(p), xppp(ppp);
  double distance = vcl_sqrt((xp.x()-xppp.x())*(xp.x()-xppp.x()) +
                             (xp.y()-xppp.y())*(xp.y()-xppp.y()));
  TEST_NEAR("perspective",distance , 0.0, 1e-06);
}
static void test_projective_basis()
{
  vcl_cout << "Testing canonical basis for points\n"
           << "Test points on a unit square\n";
  vgl_homg_point_2d<double> p0(0.0,0.0,1.0);
  vgl_homg_point_2d<double> p1(1.0,0.0,1.0);
  vgl_homg_point_2d<double> p2(0.0,1.0,1.0);
  vgl_homg_point_2d<double> p3(1.0,1.0,1.0);
  vcl_vector<vgl_homg_point_2d<double> > basis_points;
  basis_points.push_back(p0);   basis_points.push_back(p1);
  basis_points.push_back(p2);   basis_points.push_back(p3);
  vgl_h_matrix_2d<double> Basis;
  Basis.projective_basis(basis_points);
  vcl_cout <<"Transform to Canonical Frame \n" << Basis << "\n"
           <<"canonical p0 " << Basis(p0) << "\n"
           <<"canonical p1 " << Basis(p1) << "\n"
           <<"canonical p2 " << Basis(p2) << "\n"
           <<"canonical p3 " << Basis(p3) << "\n";
  vgl_point_2d<double> pu(Basis(p3));
  double distance = vcl_sqrt((pu.x()-1.0)*(pu.x()-1.0) +
                             (pu.y()-1.0)*(pu.y()-1.0));
  TEST_NEAR("identity",distance , 0.0, 1e-06);
  vcl_cout << "Test collinear points\n";
  vgl_homg_point_2d<double> pcl0(0.0,0.0,1.0);
  vgl_homg_point_2d<double> pcl1(1.0,1.0,1.0);
  vgl_homg_point_2d<double> pcl2(2.0,2.0,1.0);
  vgl_homg_point_2d<double> pcl3(3.0,3.0,1.0);
  vcl_vector<vgl_homg_point_2d<double> > collinear_points;
  collinear_points.push_back(pcl0);   collinear_points.push_back(pcl1);
  collinear_points.push_back(pcl2);   collinear_points.push_back(pcl3);
  vgl_h_matrix_2d<double> collinear_basis;
  TEST("collinear degeneracy",
       collinear_basis.projective_basis(collinear_points), false);
}
static void test_compute_linear()
{
  vcl_cout << "Test the recovery of a 2x scale transform using the "
           << "linear algorithm\n";
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

  vgl_h_matrix_2d_compute_linear hmcl;
  vgl_h_matrix_2d<double> H = hmcl.compute(points1, points2);
  vcl_cout << "The resulting transform \n" << H << "\n";
  vnl_matrix_fixed<double, 3, 3> M=H.get_matrix();
  vgl_homg_point_2d<double> hdiag(M[0][0], M[1][1], M[2][2]);
  vgl_point_2d<double> diag(hdiag);
  vcl_cout << "The normalized upper diagonal "<<diag << "\n";
  double distance = vcl_sqrt((diag.x()-2.0)*(diag.x()-2.0) +
                             (diag.y()-2.0)*(diag.y()-2.0));
  TEST_NEAR("recover 2x scale matrix",
            distance, 0.0, 1e-06);
}

static void test_compute_4point()
{
  vcl_cout << "Test the recovery of an 2x scale transform using 4 points\n";
  vcl_vector<vgl_homg_point_2d<double> > points1, points2;

  //setup points in frame 1
  vgl_homg_point_2d<double> p10(0.0, 0.0, 1.0), p11(1.0, 0.0, 1.0);
  vgl_homg_point_2d<double> p12(0.0, 1.0, 1.0), p13(1.0, 1.0, 1.0);

  points1.push_back(p10); points1.push_back(p11); points1.push_back(p12);
  points1.push_back(p13);

  //setup points in frame 2
  vgl_homg_point_2d<double> p20(0.0, 0.0, 1.0), p21(2.0, 0.0, 1.0);
  vgl_homg_point_2d<double> p22(0.0, 2.0, 1.0), p23(2.0, 2.0, 1.0);

  points2.push_back(p20); points2.push_back(p21); points2.push_back(p22);
  points2.push_back(p23);

  vgl_h_matrix_2d_compute_4point hc4p;
  vgl_h_matrix_2d<double> H = hc4p.compute(points1, points2);
  vnl_matrix_fixed<double, 3, 3> M=H.get_matrix();
  vgl_homg_point_2d<double> hdiag(M[0][0], M[1][1], M[2][2]);
  vgl_point_2d<double> diag(hdiag);
  vcl_cout << "The normalized upper diagonal "<<diag << "\n";
  double distance = vcl_sqrt((diag.x()-2.0)*(diag.x()-2.0) +
                             (diag.y()-2.0)*(diag.y()-2.0));
  TEST_NEAR("recover 2x scale matrix", distance, 0.0, 1e-06);
}

MAIN( test_h_matrix_2d )
{
  START( "test_vgl_h_matrix_2d" );
  test_identity_transform();
  test_perspective_transform();
  test_projective_basis();
  test_compute_linear();
  test_compute_4point();
  SUMMARY();
}
