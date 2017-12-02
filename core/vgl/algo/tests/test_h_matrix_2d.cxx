//:
// \file
// \author Joseph Mundy
// \date  March 27, 2003
//
// \verbatim
//  Modifications
//   24 June 2003 - Peter Vanroose - added test_projective_basis_from_lines()
// \endverbatim

#include <iostream>
#include <testlib/testlib_test.h>
#include <vcl_compiler.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vgl/algo/vgl_norm_trans_2d.h>
#include <vgl/algo/vgl_h_matrix_2d_compute_linear.h>
#include <vgl/algo/vgl_h_matrix_2d_compute_4point.h>
#include <vgl/algo/vgl_h_matrix_2d_compute_rigid_body.h>
#include <vgl/algo/vgl_h_matrix_2d_optimize_lmq.h>

static void test_identity_transform()
{
  std::cout << "\n=== Testing identity transform on point ===\n";
  vgl_h_matrix_2d<double> Id(vnl_matrix_fixed<double,3,3>().set_identity());
  vgl_homg_point_2d<double> p(3,2,1), pp = Id(p);
  std::cout << "Id\n" << Id << '\n'
           << "p =" << p << " , Id(p) = pp =" << pp << '\n';
  TEST_NEAR("identity", length(p-pp), 0.0, 1e-06);
}

static void test_perspective_transform()
{
  std::cout << "\n=== Testing perspective transform on point ===\n";
  double M[] = { 1.0,  2.0, 1.0,
                 0.5, -2.0, 1.5,
                 0.25, 3.0, 1.75 };
  vgl_h_matrix_2d<double> Tproj(M);
  vgl_homg_point_2d<double> p(3,2,1);
  vgl_homg_point_2d<double> pp = Tproj(p);
  vgl_homg_point_2d<double> ppp = Tproj.preimage(pp);
  std::cout << "Tproj\n" << Tproj << '\n'
           << "p =" << p << " , Tproj(p) = pp =" << pp << '\n'
           << " , Tproj.preimage(pp) = ppp =" << ppp << '\n';
  TEST_NEAR("perspective", length(p-ppp) , 0.0, 1e-06);
}

static void test_transform_types()
{
  vgl_h_matrix_2d<double> h; h.set_identity();
  vgl_homg_point_2d<double> p(3.0,2.0,1.5), pp;
  pp = h(p);
  TEST_NEAR("identity", length(p-pp), 0.0, 1e-06);
  h.set_translation(1.1, 1.2);
  pp = h(p);
  vgl_homg_point_2d<double> tt(4.65, 3.8, 1.5);
  TEST_NEAR("translation", length(pp-tt), 0.0, 1e-06);
  h.set_rotation(0.7853982);
  pp = h(p);
  vgl_homg_point_2d<double> tr(2.35711, 5.33553, 1.5);
  TEST_NEAR("rotation", length(pp-tr), 0.0, 1e-05);
  h.set_scale(1.4);
  pp = h(p);
  vgl_homg_point_2d<double> ts(3.29995, 7.46975, 1.5);
  TEST_NEAR("scale", length(pp-ts), 0.0, 1e-05);
  h.set_similarity(1.2, 0.7853982, 1.1, 1.2);
  pp = h(p);
  vgl_homg_point_2d<double> tsim(2.49853,6.04264, 1.5);
  TEST_NEAR("similarity", length(pp-tsim), 0.0, 1e-05);
  h.set_similarity(1.0, 0.7853982, 1.1, 1.2);
  h.set_aspect_ratio(1.4);
  pp = h(p);
  vgl_homg_point_2d<double> tasp(2.35711,7.46975, 1.5);
  TEST_NEAR("aspect ratio", length(pp-tasp), 0.0, 1e-05);
  vnl_matrix<double> M(2,3);
  M[0][0]=0.707107;   M[0][1]=-0.707107; M[0][2]=1.1;
  M[1][0]=0.98995;   M[1][1]=0.989949; M[1][2]=1.68;
  h.set_affine(M);
  pp = h(p);
  TEST_NEAR("affine", length(pp-tasp), 0.0, 1e-05);
}

static void test_projective_basis()
{
  std::cout << "\n=== Testing canonical basis for points ===\n"
           << "Test points on a unit square\n";
  vgl_homg_point_2d<double> p0(0.0,0.0,1.0);
  vgl_homg_point_2d<double> p1(1.0,0.0,1.0);
  vgl_homg_point_2d<double> p2(0.0,1.0,1.0);
  vgl_homg_point_2d<double> p3(1.0,1.0,1.0);
  std::vector<vgl_homg_point_2d<double> > basis_points;
  basis_points.push_back(p0); basis_points.push_back(p1);
  basis_points.push_back(p2); basis_points.push_back(p3);
  vgl_h_matrix_2d<double> Basis; Basis.projective_basis(basis_points);
  std::cout <<"Transform to Canonical Frame\n" << Basis << '\n'
           <<"canonical p0 =" << Basis(p0) << '\n'
           <<"canonical p1 =" << Basis(p1) << '\n'
           <<"canonical p2 =" << Basis(p2) << '\n'
           <<"canonical p3 =" << Basis(p3) << '\n';
  TEST("transformed 1st point", Basis(p0), vgl_homg_point_2d<double>(1,0,0));
  TEST("transformed 2nd point", Basis(p1), vgl_homg_point_2d<double>(0,1,0));
  TEST_NEAR("transformed 3rd point", length(Basis(p2)-p0), 0.0, 1e-06);
  TEST_NEAR("transformed 4th point", length(Basis(p3)-p3), 0.0, 1e-06);
  std::cout << "Test collinear points\n";
  vgl_homg_point_2d<double> pcl0(0.0,0.0,1.0);
  vgl_homg_point_2d<double> pcl1(1.0,1.0,1.0);
  vgl_homg_point_2d<double> pcl2(2.0,2.0,1.0);
  vgl_homg_point_2d<double> pcl3(3.0,3.0,1.0);
  std::vector<vgl_homg_point_2d<double> > collinear_points;
  collinear_points.push_back(pcl0);   collinear_points.push_back(pcl1);
  collinear_points.push_back(pcl2);   collinear_points.push_back(pcl3);
  vgl_h_matrix_2d<double> collinear_basis;
  TEST("collinear degeneracy",
       collinear_basis.projective_basis(collinear_points), false);
}

static void test_projective_basis_from_lines()
{
  std::cout << "\n=== Testing canonical basis for lines ===\n"
           << "Test lines forming a unit square in dual space\n";
  vgl_homg_line_2d<double> l0(1.0,1.0,-1.0); // x+y=w will transform to x=0
  vgl_homg_line_2d<double> l1(1.0,0.0,0.0);  // x=0 will transform to y=0
  vgl_homg_line_2d<double> l2(0.0,1.0,0.0);  // y=0 will transform to w=0
  vgl_homg_line_2d<double> l3(2.0,2.0,-1.0); // 2x+2y=w transforms to x+y+w=0
  std::vector<vgl_homg_line_2d<double> > basis_lines;
  basis_lines.push_back(l0);   basis_lines.push_back(l1);
  basis_lines.push_back(l2);   basis_lines.push_back(l3);
  vgl_h_matrix_2d<double> Basis; Basis.projective_basis(basis_lines);
  std::cout <<"Transform to Canonical Frame\n" << Basis << '\n'
           <<"canonical l0 =" << Basis(l0) << '\n'
           <<"canonical l1 =" << Basis(l1) << '\n'
           <<"canonical l2 =" << Basis(l2) << '\n'
           <<"canonical l3 =" << Basis(l3) << '\n';
  vgl_homg_line_2d<double> lu = Basis(l0);
  TEST_NEAR("transformed 1st line", lu.b()*lu.b() + lu.c()*lu.c(), 0.0, 1e-12);
  lu = Basis(l1);
  TEST_NEAR("transformed 2nd line", lu.a()*lu.a() + lu.c()*lu.c(), 0.0, 1e-12);
  lu = Basis(l2);
  TEST_NEAR("transformed 3rd line", lu.a()*lu.a() + lu.b()*lu.b(), 0.0, 1e-12);
  lu = Basis(l3);
  TEST_NEAR("transformed 4th line", (lu.a()-lu.c())*(lu.a()-lu.c())
                                  + (lu.b()-lu.c())*(lu.b()-lu.c()), 0.0, 1e-12);
  std::cout << "Test concurrent lines\n";
  vgl_homg_line_2d<double> lcl0(0.0,0.0,1.0);
  vgl_homg_line_2d<double> lcl1(1.0,1.0,1.0);
  vgl_homg_line_2d<double> lcl2(2.0,2.0,1.0);
  vgl_homg_line_2d<double> lcl3(3.0,3.0,1.0);
  std::vector<vgl_homg_line_2d<double> > concurrent_lines;
  concurrent_lines.push_back(lcl0);   concurrent_lines.push_back(lcl1);
  concurrent_lines.push_back(lcl2);   concurrent_lines.push_back(lcl3);
  vgl_h_matrix_2d<double> concurrent_basis;
  TEST("concurrent degeneracy",
       concurrent_basis.projective_basis(concurrent_lines), false);
}

static void test_norm_trans()
{
  std::cout << "\n=== Test the computation of normalizing transform\n";

  std::vector<vgl_homg_point_2d<double> > points1;
  //setup points in frame 1
  vgl_homg_point_2d<double> p10(1.414, 1.414, 1.0), p11(-0.707, 0.707, 1.0);
  vgl_homg_point_2d<double> p12(-1.414, -1.414, 1.0), p13(0.707, -0.707, 1.0);
  vgl_homg_point_2d<double> p14(0, 1.263, 1.0), p15(0, -1.263, 1.0);
  points1.push_back(p10); points1.push_back(p11); points1.push_back(p12);
  points1.push_back(p13); points1.push_back(p14); points1.push_back(p15);

  vgl_norm_trans_2d<double> nt;
  bool good = nt.compute_from_points(points1, false);
  double dg = 0.0;
  if (!good)
    dg = 1.0;
  std::cout << "The resulting transform\n";
  vnl_matrix_fixed<double,3,3> M=nt.get_matrix();
  std::cout << M
           << "Normalized Points\n";
  unsigned n = 0;
  double Sxx = 0, Sxy = 0, Syy=0;
  for (std::vector<vgl_homg_point_2d<double> >::iterator pit = points1.begin();
       pit != points1.end(); ++pit,++n)
  {
    vgl_homg_point_2d<double> p = nt(*pit);
    double x = p.x()/p.w(), y = p.y()/p.w();
    std::cout << "p[" << n << "]=(" << x << ' ' << y << ")\n";
    Sxx += x*x; Sxy+= x*y; Syy += y*y;
  }
  std::cout << "Sxx = " << std::sqrt(Sxx/5.0) << " Sxy = "
           << std::sqrt(Sxy/5.0) << " Syy = " << std::sqrt(Syy/5.0) << '\n';

  double sigma_x = std::sqrt(Sxx/5.0), sigma_y = std::sqrt(Syy/5.0);
  double error = (sigma_x-1)*(sigma_y-1);
  TEST_NEAR("Transformed Variances", error + dg, 0.0, 1e-02);
}

static void test_compute_linear_points()
{
  std::cout << "\n=== Test the recovery of a 2x scale transform using the "
           << "linear algorithm ===\n";
  std::vector<vgl_homg_point_2d<double> > points1, points2;
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
  std::cout << "The resulting transform\n" << H << '\n';
  vnl_matrix_fixed<double,3,3> M=H.get_matrix();
  vgl_homg_point_2d<double> hdiag(M[0][0], M[1][1], M[2][2]);
  std::cout << "The normalized upper diagonal "<< hdiag << '\n';
  TEST_NEAR("recover 2x scale matrix", length(hdiag-p23), 0.0, 1e-06);
  // Test if optimization converges
  vgl_h_matrix_2d<double> h_init, h_opt;
  M[0][0] += 0.1;//perturb the initial guess
  h_init.set(M);
  vgl_h_matrix_2d_optimize_lmq lmq(h_init);
  lmq.optimize(points1, points2, h_opt);
  std::cout << "The optimized transform\n" << h_opt << '\n';
  vnl_matrix_fixed<double,3,3> Mop = h_opt.get_matrix();
  TEST_NEAR("Optimized Scale Factor", Mop[0][0]/Mop[2][2], 2.0, 5e-03);
}

static void test_compute_linear_lines()
{
  std::cout << "\n=== Test the recovery of a 2x scale transform using the "
           << "linear algorithm using lines ===\n";
  std::vector<vgl_homg_line_2d<double> > lines1, lines2;
  //setup lines in frame 1
  double const pt7 = vnl_math::sqrt1_2; // sqrt(0.5);
  double const pt8 = 2.5/3; // 0.866666...
  vgl_homg_line_2d<double> l10(0.0, 1.0, 100.0), l11(1.0, 0.0, 100.0);
  vgl_homg_line_2d<double> l12(pt7, pt7, 100.0), l13(pt7,-pt7, 100.0);
  vgl_homg_line_2d<double> l14(0.5, pt8, 100.0), l15(-0.5, pt8, 100.0);
  lines1.push_back(l10); lines1.push_back(l11); lines1.push_back(l12);
  lines1.push_back(l13); lines1.push_back(l14); lines1.push_back(l15);

  //setup lines in frame 2
  vgl_homg_line_2d<double> l20(0.0, 1.0, 200.0), l21(1.0, 0.0, 200.0);
  vgl_homg_line_2d<double> l22(pt7, pt7, 200.0), l23(pt7, -pt7, 200.0);
  vgl_homg_line_2d<double> l24(0.5, pt8, 200.0), l25(-0.5, pt8, 200.0);
  lines2.push_back(l20); lines2.push_back(l21); lines2.push_back(l22);
  lines2.push_back(l23); lines2.push_back(l24); lines2.push_back(l25);

  //Solve as a least squares problem
  vgl_h_matrix_2d_compute_linear hmcl;
  vgl_h_matrix_2d<double> H = hmcl.compute(lines1, lines2);
  std::cout << "The resulting transform\n" << H << '\n';
  vnl_matrix_fixed<double,3,3> M=H.get_matrix();
  vgl_homg_point_2d<double> hdiag(M[0][0], M[1][1], M[2][2]);
  std::cout << "The normalized upper diagonal "<< hdiag << '\n';
  vgl_homg_point_2d<double> p23(2.0,2.0,1.0);
  TEST_NEAR("recover 2x scale matrix", length(hdiag-p23), 0.0, 1e-06);
  //solve the same problem with weighted least squares
  std::vector<double> w(6,1.0);
  vgl_h_matrix_2d<double> Hwls = hmcl.compute(lines1, lines2, w);
  std::cout << "The resulting transform from weighted least squares\n"
           << Hwls << '\n';
  vnl_matrix_fixed<double,3,3> Mwls=H.get_matrix();
  vgl_homg_point_2d<double> hdiag_wls(Mwls[0][0], Mwls[1][1], Mwls[2][2]);
  std::cout << "The normalized upper diagonal (least squares) "<< hdiag_wls << '\n';
  TEST_NEAR("recover 2x scale matrix from weighted least squares",
            length(hdiag_wls-p23), 0.0, 1e-12);
  // Test if optimization converges
  vgl_h_matrix_2d<double> h_init, h_opt;
  M[0][0] += 0.5;//perturb the initial guess
  h_init.set(M);
  vgl_h_matrix_2d_optimize_lmq lmq(h_init);
  lmq.set_htol(1e-12);
  lmq.optimize(lines1, lines2, h_opt);
  std::cout << "The optimized transform\n" << h_opt << '\n';
  vnl_matrix_fixed<double,3,3> Mop = h_opt.get_matrix();
  //Seems to work but convergence is slow and not particularly accurate
  //Use a large error tolerance just to detect a real defect in coding.
  TEST_NEAR("Optimized Scale Factor", Mop[0][0]/Mop[2][2], 2.0, 0.5);
}

static void test_compute_4point()
{
  std::cout << "\n=== Test the recovery of a 2x scale transform using 4 points ===\n";
  std::vector<vgl_homg_point_2d<double> > points1, points2;

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
  vnl_matrix_fixed<double,3,3> M=H.get_matrix();
  vgl_homg_point_2d<double> hdiag(M[0][0], M[1][1], M[2][2]);
  std::cout << "The normalized upper diagonal "<< hdiag << '\n';
  TEST_NEAR("recover 2x scale matrix", length(hdiag-p23), 0.0, 1e-06);
}

static void test_set_transform()
{
  std::cout << "\n=== Test setting basic transforms =======\n";
  vgl_h_matrix_2d<double> H;
  vgl_homg_point_2d<double> p(1.0, 0.0);//point on the x axis
  H.set_identity().set_rotation(vnl_math::pi/2.0);
  vgl_homg_point_2d<double> rp, sp , tp;
  rp = H(p);
  std::cout << "rotated point " << rp << '\n';
  H.set_scale(2.0);
  sp = H(p);
  std::cout << "rotated and scaled point " << sp << '\n';
  H.set_translation(1.0, 3.0);
  tp = H(p);
  std::cout << "rotated, scaled and translated point " << tp << '\n';
  TEST_NEAR("rotation", rp.y(), 1.0, 1e-06);
  TEST_NEAR("scale and rotation", sp.y(), 2.0, 1e-06);
  TEST_NEAR("scale, rotation and translation", tp.x()+tp.y(), 6.0, 1e-06);
}

static void test_compute_rigid_body()
{
  vgl_homg_point_2d<double> ps00(0,0);
  vgl_homg_point_2d<double> ps01(1,0);
  vgl_homg_point_2d<double> ps10(0,0);
  vgl_homg_point_2d<double> ps11(0.707107,0.707107);
  std::vector<vgl_homg_point_2d<double> > points0, points1;
  points0.push_back(ps00);     points0.push_back(ps01);
  points1.push_back(ps10);     points1.push_back(ps11);
  vgl_h_matrix_2d_compute_rigid_body crb;
  vgl_h_matrix_2d<double> RT = crb.compute(points0, points1);
  std::cout << "Rotation, translation\n" << RT << '\n';
  vnl_matrix_fixed<double, 3, 3> M = RT.get_matrix();
  TEST_NEAR("rigid body pure rotation", M[0][0], 0.707107, 1.0e-6);
  points0.clear();     points1.clear();
  vgl_homg_point_2d<double> p00(-1,1);
  vgl_homg_point_2d<double> p01(1,1);
  vgl_homg_point_2d<double> p02(1,-1);
  vgl_homg_point_2d<double> p03(-1,-1);
  vgl_homg_point_2d<double> p04(0,0);

  vgl_homg_point_2d<double> p10(998.862,500.839);
  vgl_homg_point_2d<double> p11(1000.84,501.138);
  vgl_homg_point_2d<double> p12(1001.14,499.161);
  vgl_homg_point_2d<double> p13(999.161,498.862);
  vgl_homg_point_2d<double> p14(1000, 500);


  points0.push_back(p00);  points0.push_back(p01);
  points0.push_back(p02);  points0.push_back(p03);
  points0.push_back(p04);

  points1.push_back(p10);  points1.push_back(p11);
  points1.push_back(p12);  points1.push_back(p13);
  points1.push_back(p14);
  RT = crb.compute(points0, points1);
  std::cout << "Rotation, translation\n" << RT << '\n';
  M = RT.get_matrix();
  //0.149438
  TEST_NEAR("rigid body rotation and translation", M[0][0], 0.988771, 1.0e-3);
}

static void test_h_matrix_2d()
{
  test_identity_transform();
  test_transform_types();
  test_perspective_transform();
  test_projective_basis();
  test_projective_basis_from_lines();
  test_norm_trans();
  test_compute_linear_points();
  test_compute_linear_lines();
  test_compute_4point();
  test_set_transform();
  test_compute_rigid_body();
}

TESTMAIN(test_h_matrix_2d);
