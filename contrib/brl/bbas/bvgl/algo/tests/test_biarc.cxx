// This is bbas/bvgl/algo/tests/test_biarc.cxx

#include <iostream>
#include <cmath>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <testlib/testlib_test.h>
#include <vnl/vnl_math.h>
#include <bvgl/algo/bvgl_biarc.h>


//: Test functions of biarc class
MAIN( test_biarc )
{
  double tolerance = 1e-7;

  START (" Test bvgl_biarc class");
  std::cout << "Test Constructors" << std::endl;
  bvgl_biarc biarc1 = bvgl_biarc();
  // This must always be true TEST("Default Constructor", &biarc1 != 0, true);
  biarc1 = bvgl_biarc(vgl_point_2d<double >(0, 0), 0, vgl_point_2d< double >(5, 5), vnl_math::pi/2);
  bvgl_biarc biarc2 = bvgl_biarc(biarc1);
  bool test_copy = (biarc1.start() == biarc2.start()) && (biarc1.k1() == biarc2.k1())
    && (biarc1.len1() == biarc2.len1()) && (biarc1.k2() == biarc2.k2())
    && (biarc1.len2() == biarc2.len2());
  TEST("Copy Constructor", test_copy, true);

  // data structure: case = [x0, y0, t0 (radian), x1, y1, t1(radian)]
  // result = [k1, len1, k2, len2]
  double case1[] = {0, 0, 0, 5, 5, vnl_math::pi_over_2};
  double result1[] = {0.2, 7.85398163397448, 0, 0};
  double case2[] = {2, 1, vnl_math::pi /6.0, 8, 10, 2.0 * vnl_math::pi/3};
  double result2[] = {0.0227016253375765, 5.85824714851934, 0.224997196145613, 6.39032227745528};
  double case3[] = {-3, -4, -vnl_math::pi/3.0, -10, -10, -5.0*vnl_math::pi/6.0};
  double result3[] = {-0.335782219762339, 5.91496327154951, 0.0826450374282758, 5.02562746975781};
  double case4[] = {0, 0, 0, 0, 5, 0};
  double result4[] = {0.8, 3.92699081698724, -0.8, 3.92699081698724};
  double case5[] = {0, 0, 0, 5, 0, 0};
  double result5[] = {0, 5, 0, 0};

  bool test_passed = true;

  // test case 1 - create biarc from 2 point-tangent on a circle
  bvgl_biarc b = bvgl_biarc(vgl_point_2d< double >(case1[0], case1[1]), case1[2],
    vgl_point_2d<double >(case1[3], case1[4]), case1[5]);
  double r1[] = {b.k1(), b.len1(), b.k2(), b.len2()};
  test_passed = true;
  for (int i = 0; i < 4; i ++){
    if (std::fabs(r1[i] - result1[i]) > tolerance)
      test_passed = false;
  }
  TEST("2 point-tangents on a circle", test_passed, true);

  // test case 2 - create biarc from 2 point-tangent not on a circle
  std::cout << "Test biarc computation algorithms - compared with result from Matlab code" << std::endl;
  b.set_start_params(vgl_point_2d< double >(case2[0], case2[1]), case2[2]);
  b.set_end_params(vgl_point_2d< double >(case2[3], case2[4]), case2[5]);
  b.compute_biarc_params();
  double r2[] = {b.k1(), b.len1(), b.k2(), b.len2()};
  test_passed = true;
  for (int i = 0; i < 4; i ++){
    if (std::fabs(r2[i] - result2[i]) > tolerance)
      test_passed = false;
  }
  TEST("2 point-tangents not on a circle (1)", test_passed, true);

  // test case 3 - create biarc from 2 point-tangent not on a circle
  b.compute_biarc_params(vgl_point_2d< double >(case3[0], case3[1]), case3[2],
    vgl_point_2d< double >(case3[3], case3[4]), case3[5]);
  double r3[] = {b.k1(), b.len1(), b.k2(), b.len2()};
  test_passed = true;
  for (int i = 0; i < 4; i ++){
    if (std::fabs(r3[i] - result3[i]) > tolerance)
      test_passed = false;
  }
  TEST("2 point-tangents not on a circle - (2)", test_passed, true);

  // test case 4 - create biarc from 2 point-tangent that makes an "S"
  b.compute_biarc_params(vgl_point_2d< double >(case4[0], case4[1]), case4[2],
    vgl_point_2d< double >(case4[3], case4[4]), case4[5]);
  double r4[] = {b.k1(), b.len1(), b.k2(), b.len2()};
  test_passed = true;
  for (int i = 0; i < 4; i ++){
    if (std::fabs(r4[i] - result4[i]) > tolerance)
      test_passed = false;
  }
  TEST("2 point-tangents forming an S shape ", test_passed, true);

  // test case 4 - create biarc from 2 point-tangent on a line
  b.compute_biarc_params(vgl_point_2d< double >(case5[0], case5[1]), case5[2],
    vgl_point_2d< double >(case5[3], case5[4]), case5[5]);
  double r5[] = {b.k1(), b.len1(), b.k2(), b.len2()};
  test_passed = true;
  for (int i = 0; i < 4; i ++){
    if (std::fabs(r5[i] - result5[i]) > tolerance)
      test_passed = false;
  }
  TEST("2 point-tangents on a line - (1)", test_passed, true);

  // test geometry functions
  std::cout << "Test geometry functions" << std::endl;
  b.compute_biarc_params(vgl_point_2d< double >(case4[0], case4[1]), case4[2],
    vgl_point_2d< double >(case4[3], case4[4]), case4[5]);
  // correct results
  double radius1 = 1.25;
  double radius2 = 1.25;
  int dir1 = 1;
  int dir2 = -1;
  vgl_point_2d< double > mid_pt(0, 2.5);
  vgl_point_2d< double > center1(0, 1.25);
  vgl_point_2d< double > center2(0, 3.75);
  double len = result4[1] + result4[3];
  double s1 = result4[1]/2;
  vgl_point_2d<double > p1(1.25, 1.25);
  vgl_vector_2d<double > t1(0, 1);
  double k1 = 0.8;

  double s2 = result4[1] + result4[3]/2;
  vgl_point_2d<double > p2(-1.25, 3.75);
  vgl_vector_2d<double > t2(0, 1);
  double k2 = -0.8;

  //// s3 and s4 are outside the biarc
  //double s3 = -1;
  //vgl_point_2d< double > p3(-1, 0);
  //vgl_vector_2d< double > t3(1, 0);
  //double k3 = 0;

  //double s4 = result4[1] + result4[3] + 2;
  //vgl_point_2d< double > p4(2, 5);
  //vgl_vector_2d< double > t4(1, 0);
  //double k4 = 0;

  TEST("r1() ", std::fabs(b.r1() - radius1) < tolerance, true);
  TEST("r2() ", std::fabs(b.r2() - radius2) < tolerance, true);
  TEST("dir1() ", std::fabs((double)( b.dir1() - dir1)) < tolerance, true);
  TEST("dir2() ", std::fabs((double)(b.dir2() - dir2)) < tolerance, true);
  // mid_pt
  test_passed = std::fabs(b.mid_pt().x()-mid_pt.x()) < tolerance &&
    std::fabs(b.mid_pt().y()-mid_pt.y()) < tolerance;
  TEST("mid_pt() ", test_passed, true);

  test_passed = std::fabs(b.center1().x()-center1.x()) < tolerance &&
    std::fabs(b.center1().y()-center1.y()) < tolerance;
  TEST("center1() ", test_passed, true);

  test_passed = std::fabs(b.center2().x()-center2.x()) < tolerance &&
    std::fabs(b.center2().y()-center2.y()) < tolerance;
  TEST("center2() ", test_passed, true);

  TEST("len() ", std::fabs(b.len() - len) < tolerance, true);

  // point at s1
  test_passed = std::fabs(b.point_at(s1).x()-p1.x()) < tolerance &&
    std::fabs(b.point_at(s1).y()-p1.y()) < tolerance;
  TEST("Point at s1 ", test_passed, true);
  // tangent at s1
  test_passed = std::fabs(b.tangent_at(s1).x()-t1.x()) < tolerance &&
    std::fabs(b.tangent_at(s1).y()-t1.y()) < tolerance;
  TEST("Tangent at s1 - inside arc 1 ", test_passed, true);
  // curvature at s1
  TEST("Curvature at s1", std::fabs(b.curvature_at(s1) - k1) < tolerance, true);

  // point at s2
  test_passed = std::fabs(b.point_at(s2).x()-p2.x()) < tolerance &&
    std::fabs(b.point_at(s2).y()-p2.y()) < tolerance;
  TEST("Point at s2 - inside arc 2 ", test_passed, true);
    // tangent at s2
  test_passed = std::fabs(b.tangent_at(s2).x()-t2.x()) < tolerance &&
    std::fabs(b.tangent_at(s2).y()-t2.y()) < tolerance;
  TEST("Tangent at s2 ", test_passed, true);
  // curvature at s2
  TEST("Curvature at s2", std::fabs(b.curvature_at(s2) - k2) < tolerance, true);

  SUMMARY();
}
