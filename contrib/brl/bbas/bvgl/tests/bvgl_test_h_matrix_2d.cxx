// This is brl/bbas/bvgl/tests/bvgl_test_h_matrix_2d.cxx
#include <vcl_iostream.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vgl/vgl_homg_point_2d.h>
#include <bvgl/bvgl_h_matrix_2d.h>
#include <testlib/testlib_test.h>

static void bvgl_test_h_matrix_2d()
{
  vcl_cout << "Testing identity transform on point\n";
  vnl_matrix_fixed<double, 3, 3> M;
  M.set_identity();
  bvgl_h_matrix_2d<double> Id(M);
  vgl_homg_point_2d<double> p(3,2,1), pp, ppp;
  pp = Id(p);
  vcl_cout << "Id\n" << Id << '\n'
           << 'p' << p << " , Id(p) = pp " << pp << '\n';
  TEST_NEAR("pp.x()", pp.x(), p.x(), 1e-4);
  TEST_NEAR("pp.y()", pp.y(), p.y(), 1e-4);
  TEST_NEAR("pp.w()", pp.w(), p.w(), 1e-4);

  vcl_cout << "Testing perspective transform on point\n";
  M.put(0,0,1);    M.put(0,1,2);   M.put(0,2,1);
  M.put(1,0,0.5);  M.put(1,1,-2);  M.put(1,2,1.5);
  M.put(2,0,0.25); M.put(2,1,3);   M.put(2,2,1.75);
  bvgl_h_matrix_2d<double> Tproj(M);
  pp = Tproj(p);
  ppp = Tproj.preimage(pp);
  vcl_cout << "Tproj\n" << Tproj << '\n'
           << 'p' << p << " , Tproj(p) = pp " << pp << '\n'
           << " , Tproj.preimage(pp) = ppp " << ppp << '\n';
  TEST_NEAR("ppp.x()", ppp.x(), p.x(), 1e-4);
  TEST_NEAR("ppp.y()", ppp.y(), p.y(), 1e-4);
  TEST_NEAR("ppp.w()", ppp.w(), p.w(), 1e-4);
  vcl_cout << "Testing canonical basis for points\n"
           << "Test points on a unit square\n";
  vgl_homg_point_2d<double> p0(0.0,0.0,1.0);
  vgl_homg_point_2d<double> p1(1.0,0.0,1.0);
  vgl_homg_point_2d<double> p2(0.0,1.0,1.0);
  vgl_homg_point_2d<double> p3(1.0,1.0,1.0);
  vcl_vector<vgl_homg_point_2d<double> > basis_points;
  basis_points.push_back(p0);   basis_points.push_back(p1);
  basis_points.push_back(p2);   basis_points.push_back(p3);
  bvgl_h_matrix_2d<double> Basis;
  Basis.projective_basis(basis_points);
  vcl_cout <<"Transform to Canonical Frame\n" << Basis << '\n'
           <<"canonical p0 " << Basis(p0) << '\n'
           <<"canonical p1 " << Basis(p1) << '\n'
           <<"canonical p2 " << Basis(p2) << '\n'
           <<"canonical p3 " << Basis(p3) << '\n';
  TEST_NEAR("p3.x()", p3.x(), 1.0, 1e-4);
  TEST_NEAR("p3.y()", p3.y(), 1.0, 1e-4);
  TEST_NEAR("p3.w()", p3.w(), 1.0, 1e-4);
  vcl_cout << "Test collinear points\n";
  vgl_homg_point_2d<double> pcl0(0.0,0.0,1.0);
  vgl_homg_point_2d<double> pcl1(1.0,1.0,1.0);
  vgl_homg_point_2d<double> pcl2(2.0,2.0,1.0);
  vgl_homg_point_2d<double> pcl3(3.0,3.0,1.0);
  vcl_vector<vgl_homg_point_2d<double> > collinear_points;
  collinear_points.push_back(pcl0);   collinear_points.push_back(pcl1);
  collinear_points.push_back(pcl2);   collinear_points.push_back(pcl3);
  bvgl_h_matrix_2d<double> collinear_basis;
  TEST("bvgl_h_matrix_2d<double>::projective_basis()", !collinear_basis.projective_basis(collinear_points), true);
  vcl_cout <<"Collinear Transform\n" << collinear_basis << '\n';
}

TESTMAIN(bvgl_test_h_matrix_2d);
