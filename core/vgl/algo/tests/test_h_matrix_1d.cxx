//:
// \file
// \author Peter Vanroose
// \date  June 13, 2004

#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_homg_point_1d.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vgl/algo/vgl_h_matrix_1d.h>
#include <vgl/algo/vgl_h_matrix_1d_compute_linear.h>
#include <vgl/algo/vgl_h_matrix_1d_compute_3point.h>
#include <vgl/algo/vgl_h_matrix_1d_compute_optimize.h>

static void test_identity_transform()
{
  std::cout << "\n=== Testing identity transform ===\n";
  vgl_h_matrix_1d<double> Id(vnl_matrix_fixed<double,2,2>().set_identity());
  vgl_homg_point_1d<double> p(3,2), pp = Id(p);
  std::cout << "Id\n" << Id << '\n'
           << "p =" << p << " , Id(p) = pp =" << pp << std::endl;
  TEST("identity", p, pp);
}

static void test_perspective_transform()
{
  std::cout << "\n=== Testing perspective transform ===\n";
  vnl_matrix_fixed<double,2,2> M;
  M.put(0,0, -2.0); M.put(0,1, 1.5);
  M.put(1,0,  3.0); M.put(1,1, 1.75);
  vgl_h_matrix_1d<double> Tproj(M);
  vgl_homg_point_1d<double> p(3,2);
  vgl_homg_point_1d<double> pp = Tproj(p);
  vgl_homg_point_1d<double> ppp = Tproj.preimage(pp);
  std::cout << "Tproj\n" << Tproj << '\n'
           << "p =" << p << " , Tproj(p) = pp =" << pp << '\n'
           << " , Tproj.preimage(pp) = ppp =" << ppp << std::endl;
  TEST("perspective", p, ppp);
}

static void test_projective_basis()
{
  std::cout << "\n=== Testing canonical basis ===\n";
  vgl_homg_point_1d<double> p0(0,1);
  vgl_homg_point_1d<double> p1(1,1);
  vgl_homg_point_1d<double> p2(7,1);
  std::vector<vgl_homg_point_1d<double> > basis_points;
  basis_points.push_back(p0); basis_points.push_back(p1); basis_points.push_back(p2);
  vgl_h_matrix_1d<double> Basis; Basis.projective_basis(basis_points);
  std::cout <<"Transform to Canonical Frame\n" << Basis << '\n'
           <<"canonical p0 = " << Basis(p0) << '\n'
           <<"canonical p1 = " << Basis(p1) << '\n'
           <<"canonical p2 = " << Basis(p2) << std::endl;
  TEST("transformed 1st point", Basis(p0), vgl_homg_point_1d<double>(1,0));
  TEST("transformed 2nd point", Basis(p1), vgl_homg_point_1d<double>(0,1));
  TEST_NEAR("transformed 3rd point", Basis(p2) - vgl_homg_point_1d<double>(1,1), 0.0, 1e-6);
}

static void test_compute_svd()
{
  std::cout << "\n=== Test the recovery of a 2x scale transform using the SVD ===\n";
  std::vector<vgl_homg_point_1d<double> > points1, points2;
  //setup points in frame 1
  vgl_homg_point_1d<double> p10(0,1), p11(1,1), p12(3,2), p13(2,1);
  points1.push_back(p10); points1.push_back(p11); points1.push_back(p12); points1.push_back(p13);

  //setup points in frame 2
  vgl_homg_point_1d<double> p20(0,1), p21(2,1), p22(1,1), p23(3,2);
  points2.push_back(p20); points2.push_back(p21); points2.push_back(p22); points2.push_back(p23);

  vgl_h_matrix_1d<double> H(points1, points2);
  std::cout << "The resulting transform\n" << H << std::endl;
  vnl_matrix_fixed<double,2,2> M=H.get_matrix();
  vgl_homg_point_1d<double> hdiag(M[0][0], M[1][1]);
  std::cout << "The normalized upper diagonal "<< hdiag << std::endl;
  TEST_NEAR("recover 2x scale matrix", hdiag-vgl_homg_point_1d<double>(5,-4), 0.0, 1e-06);
}

static void test_compute_linear()
{
  std::cout << "\n=== Test the recovery of a 2x scale transform using the linear algorithm ===\n";
  std::vector<vgl_homg_point_1d<double> > points1, points2;
  //setup points in frame 1
  vgl_homg_point_1d<double> p10(0,1), p11(1,1), p12(3,2), p13(2,1);
  points1.push_back(p10); points1.push_back(p11); points1.push_back(p12); points1.push_back(p13);

  //setup points in frame 2
  vgl_homg_point_1d<double> p20(0,1), p21(2,1), p22(1,1), p23(3,2);
  points2.push_back(p20); points2.push_back(p21); points2.push_back(p22); points2.push_back(p23);

  vgl_h_matrix_1d_compute_linear hmcl;
  vgl_h_matrix_1d<double> H = hmcl.compute(points1, points2);
  std::cout << "The resulting transform\n" << H << std::endl;
  vnl_matrix_fixed<double,2,2> M=H.get_matrix();
  vgl_homg_point_1d<double> hdiag(M[0][0], M[1][1]);
  std::cout << "The normalized upper diagonal "<< hdiag << std::endl;
  TEST_NEAR("recover 2x scale matrix", hdiag-vgl_homg_point_1d<double>(5,-4), 0.0, 1e-06);
}

static void test_compute_3point()
{
  std::cout << "\n=== Test the recovery of a 2x scale transform using 3 points ===\n";
  std::vector<vgl_homg_point_1d<double> > points1, points2;

  //setup points in frame 1
  vgl_homg_point_1d<double> p10(0,1), p11(1,0), p12(1,1);

  points1.push_back(p10); points1.push_back(p11); points1.push_back(p12);

  //setup points in frame 2
  vgl_homg_point_1d<double> p20(0,1), p21(2,1), p22(1,2);

  points2.push_back(p20); points2.push_back(p21); points2.push_back(p22);

  vgl_h_matrix_1d_compute_3point hc4p;
  vgl_h_matrix_1d<double> H = hc4p.compute(points1, points2);
  vnl_matrix_fixed<double,2,2> M=H.get_matrix();
  vgl_homg_point_1d<double> hdiag(M[0][0], M[1][1]);
  std::cout << "The normalized upper diagonal "<< hdiag << std::endl;
  TEST_NEAR("recover 2x scale matrix", hdiag-vgl_homg_point_1d<double>(2,3), 0.0, 1e-06);
}

static void test_compute_optimize()
{
  std::cout << "\n=== Test the recovery of a 2x scale transform using the optimize algorithm ===\n";
  std::vector<vgl_homg_point_1d<double> > points1, points2;
  //setup points in frame 1
  vgl_homg_point_1d<double> p10(0,1), p11(1,1), p12(3,2), p13(2,1);
  points1.push_back(p10); points1.push_back(p11); points1.push_back(p12); points1.push_back(p13);

  //setup points in frame 2
  vgl_homg_point_1d<double> p20(0,1), p21(2,1), p22(1,1), p23(3,2);
  points2.push_back(p20); points2.push_back(p21); points2.push_back(p22); points2.push_back(p23);

  vgl_h_matrix_1d_compute_optimize hmcl;
  vgl_h_matrix_1d<double> H = hmcl.compute(points1, points2);
  std::cout << "The resulting transform\n" << H << std::endl;
  vnl_matrix_fixed<double,2,2> M=H.get_matrix();
  vgl_homg_point_1d<double> hdiag(M[0][0], M[1][1]);
  std::cout << "The normalized upper diagonal "<< hdiag << std::endl;
  TEST_NEAR("recover 2x scale matrix", hdiag-vgl_homg_point_1d<double>(5,-4), 0.0, 1e-06);
}

static void test_inverse()
{
  vnl_matrix_fixed<float,2,2> M;
  M.put(0,0, -2.0f); M.put(0,1, 1.5f);
  M.put(1,0,  3.0f); M.put(1,1, 1.75f);
  vgl_h_matrix_1d<float> T(M);
  TEST("not Euclidean", T.is_euclidean(), false);
  TEST("not a rotation", T.is_rotation(), false);
  vgl_h_matrix_1d<float> Tinv = T.get_inverse();
  std::cout << "The inverse is\n" << Tinv;
  TEST_NEAR("Lower right value", Tinv.get(1,1), 0.25f, 1e-06);
  vgl_h_matrix_1d<float> P = T * Tinv;
  TEST("product is identity", P.is_identity(), true);
}

static void test_h_matrix_1d()
{
  test_identity_transform();
  test_perspective_transform();
  test_projective_basis();
  test_compute_svd();
  test_compute_linear();
  test_compute_3point();
  test_compute_optimize();
  test_inverse();
}

TESTMAIN(test_h_matrix_1d);
