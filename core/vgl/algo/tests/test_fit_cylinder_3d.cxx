// Some tests for vgl_fit_cylinder_3d
// J.L. Mundy, June 8, 2015.
#include <iostream>
#include <cstdlib>
#include "testlib/testlib_test.h"
#include <vgl/vgl_cylinder_3d.h>
#include <vgl/algo/vgl_fit_cylinder_3d.h>
#include "vgl/vgl_point_3d.h"
#include "vgl/vgl_distance.h"
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include "vnl/vnl_random.h"
// creates points on cylinder with unit radius
static void
test_fit_unit_cylinder()
{
  vgl_vector_3d<double> z(0.0, 0.0, 1.0);
  std::vector<vgl_point_3d<double>> pts;
  double sq2_over2 = sqrt(2.0) / 2.0;
  vgl_point_3d<double> p0(1.0, 0.0, -0.5);
  vgl_point_3d<double> p1(0.0, 1.0, -0.5);
  vgl_point_3d<double> p2(-1.0, 0.0, -0.5);
  vgl_point_3d<double> p3(0.0, -1.0, -0.5);
  vgl_point_3d<double> p4(sq2_over2, sq2_over2, -0.5);
  vgl_point_3d<double> p5(-sq2_over2, sq2_over2, -0.5);
  vgl_point_3d<double> p6(-sq2_over2, -sq2_over2, -0.5);
  vgl_point_3d<double> p7(sq2_over2, -sq2_over2, -0.5);
  vgl_vector_3d<double> vz(0.0, 0.0, 1.0);
  pts.push_back(p0);
  pts.push_back(p0 + vz);
  pts.push_back(p1);
  pts.push_back(p1 + vz);
  pts.push_back(p2);
  pts.push_back(p2 + vz);
  pts.push_back(p3);
  pts.push_back(p3 + vz);
  pts.push_back(p4);
  pts.push_back(p4 + vz);
  pts.push_back(p5);
  pts.push_back(p5 + vz);
  pts.push_back(p6);
  pts.push_back(p6 + vz);
  pts.push_back(p7);
  pts.push_back(p7 + vz);

  vgl_fit_cylinder_3d<double> fit_cyl(pts);

  double error = fit_cyl.fit(z, &std::cout);
  TEST_NEAR("fit perfect unit cylinder", error, 0.0, 1e-6);
}
static void
test_compute_W_fit_unit_cylinder()
{
  vgl_vector_3d<double> z(0.0, 0.0, 1.0);
  std::vector<vgl_point_3d<double>> pts;
  double sq2_over2 = sqrt(2.0) / 2.0;
  vgl_point_3d<double> p0(1.0, 0.0, -5.0);
  vgl_point_3d<double> p1(0.0, 1.0, -5.0);
  vgl_point_3d<double> p2(-1.0, 0.0, -5.0);
  vgl_point_3d<double> p3(0.0, -1.0, -5.0);
  vgl_point_3d<double> p4(sq2_over2, sq2_over2, -5.0);
  vgl_point_3d<double> p5(-sq2_over2, sq2_over2, -5.0);
  vgl_point_3d<double> p6(-sq2_over2, -sq2_over2, -5.0);
  vgl_point_3d<double> p7(sq2_over2, -sq2_over2, -5.0);
  vgl_vector_3d<double> vz(0.0, 0.0, 10.0);
  pts.push_back(p0);
  pts.push_back(p0 + vz);
  pts.push_back(p1);
  pts.push_back(p1 + vz);
  pts.push_back(p2);
  pts.push_back(p2 + vz);
  pts.push_back(p3);
  pts.push_back(p3 + vz);
  pts.push_back(p4);
  pts.push_back(p4 + vz);
  pts.push_back(p5);
  pts.push_back(p5 + vz);
  pts.push_back(p6);
  pts.push_back(p6 + vz);
  pts.push_back(p7);
  pts.push_back(p7 + vz);

  vgl_fit_cylinder_3d<double> fit_cyl(pts);

  double error = fit_cyl.fit(&std::cout);
  TEST_NEAR("compute W and fit perfect unit cylinder", error, 0.0, 1e-6);
}
static void
test_fit_unit_cylinder_rand_error()
{
  vgl_vector_3d<double> z(0.0, 0.0, 1.0);
  std::vector<vgl_point_3d<double>> pts;
  double sq2_over2 = sqrt(2.0) / 2.0;
  vgl_point_3d<double> p0(1.0, 0.0, 0.0);
  vgl_point_3d<double> p1(0.0, 1.0, 0.0);
  vgl_point_3d<double> p2(-1.0, 0.0, 0.0);
  vgl_point_3d<double> p3(0.0, -1.0, 0.0);
  vgl_point_3d<double> p4(sq2_over2, sq2_over2, 0.0);
  vgl_point_3d<double> p5(-sq2_over2, sq2_over2, 0.0);
  vgl_point_3d<double> p6(-sq2_over2, -sq2_over2, 0.0);
  vgl_point_3d<double> p7(sq2_over2, -sq2_over2, 0.0);
  vgl_vector_3d<double> vz(0.0, 0.0, 1.0);
  pts.push_back(p0);
  pts.push_back(p0 + vz);
  pts.push_back(p1);
  pts.push_back(p1 + vz);
  pts.push_back(p2);
  pts.push_back(p2 + vz);
  pts.push_back(p3);
  pts.push_back(p3 + vz);
  pts.push_back(p4);
  pts.push_back(p4 + vz);
  pts.push_back(p5);
  pts.push_back(p5 + vz);
  pts.push_back(p6);
  pts.push_back(p6 + vz);
  pts.push_back(p7);
  pts.push_back(p7 + vz);

  // add random error (+-10%) to the points
  vnl_random ran;
  double e = 0.1;
  for (auto & pt : pts)
  {
    vgl_vector_3d<double> er(ran.drand32(-e, e), ran.drand32(-e, e), ran.drand32(-e, e));
    pt = pt + er;
  }
  vgl_fit_cylinder_3d<double> fit_cyl(pts);
  double error = fit_cyl.fit(z, &std::cout);
  TEST_NEAR("fit unit cylinder with error", error, 0.0, 0.1);
}
static void
test_compute_W_fit_unit_cylinder_rand_error()
{
  vgl_vector_3d<double> z(0.0, 0.0, 1.0);
  std::vector<vgl_point_3d<double>> pts;
  double sq2_over2 = sqrt(2.0) / 2.0;
  vgl_point_3d<double> p0(1.0, 0.0, -5.0);
  vgl_point_3d<double> p1(0.0, 1.0, -5.0);
  vgl_point_3d<double> p2(-1.0, 0.0, -5.0);
  vgl_point_3d<double> p3(0.0, -1.0, -5.0);
  vgl_point_3d<double> p4(sq2_over2, sq2_over2, -5.0);
  vgl_point_3d<double> p5(-sq2_over2, sq2_over2, -5.0);
  vgl_point_3d<double> p6(-sq2_over2, -sq2_over2, -5.0);
  vgl_point_3d<double> p7(sq2_over2, -sq2_over2, -5.0);
  vgl_vector_3d<double> vz(0.0, 0.0, 10.0);
  pts.push_back(p0);
  pts.push_back(p0 + vz);
  pts.push_back(p1);
  pts.push_back(p1 + vz);
  pts.push_back(p2);
  pts.push_back(p2 + vz);
  pts.push_back(p3);
  pts.push_back(p3 + vz);
  pts.push_back(p4);
  pts.push_back(p4 + vz);
  pts.push_back(p5);
  pts.push_back(p5 + vz);
  pts.push_back(p6);
  pts.push_back(p6 + vz);
  pts.push_back(p7);
  pts.push_back(p7 + vz);

  // add random error (+-10%) to the points
  vnl_random ran;
  double e = 0.1;
  for (auto & pt : pts)
  {
    vgl_vector_3d<double> er(ran.drand32(-e, e), ran.drand32(-e, e), ran.drand32(-e, e));
    pt = pt + er;
  }
  vgl_fit_cylinder_3d<double> fit_cyl(pts);
  double error = fit_cyl.fit(&std::cout);
  TEST_NEAR("compute W and fit unit cylinder with error", error, 0.0, 0.1);
}
static void
test_fit_actual_pts()
{
  vgl_vector_3d<double> z(0.0, 0.0, 1.0);
  std::vector<vgl_point_3d<double>> pts;
  vgl_point_3d<double> p0(72.224800109863, 127.833999633789, 71.081001281738);
  vgl_point_3d<double> p1(70.772697448730, 118.206001281738, 70.503997802734);
  vgl_point_3d<double> p2(72.164802551270, 121.077003479004, 70.761596679688);
  vgl_point_3d<double> p3(71.458801269531, 121.928001403809, 72.767303466797);
  vgl_point_3d<double> p4(65.802398681641, 115.774002075195, 71.183898925781);
  vgl_point_3d<double> p5(62.580200195313, 114.206001281738, 70.452003479004);
  vgl_point_3d<double> p6(49.206798553467, 121.575996398926, 71.960098266602);
  vgl_point_3d<double> p7(53.480899810791, 115.714996337891, 73.637802124023);
  vgl_point_3d<double> p8(51.988201141357, 116.570999145508, 71.405899047852);
  vgl_point_3d<double> p9(50.186901092529, 118.406997680664, 75.717697143555);
  vgl_point_3d<double> p10(57.110000610352, 114.061996459961, 73.177696228027);
  vgl_point_3d<double> p11(60.525600433350, 114.305000305176, 70.010498046875);
  vgl_point_3d<double> p12(60.715900421143, 115.761001586914, 73.637802124023);
  vgl_point_3d<double> p13(54.940799713135, 114.845001220703, 75.217102050781);
  vgl_point_3d<double> p14(68.249801635742, 112.859001159668, 72.507003784180);
  vgl_point_3d<double> p15(73.900901794434, 121.288002014160, 77.134201049805);
  vgl_point_3d<double> p16(72.352401733398, 119.390998840332, 75.987503051758);
  vgl_point_3d<double> p17(67.769599914551, 113.900001525879, 76.670898437500);
  vgl_point_3d<double> p18(67.826896667480, 113.430000305176, 74.734596252441);
  vgl_point_3d<double> p19(61.457801818848, 112.666000366211, 75.747100830078);
  vgl_point_3d<double> p20(60.935298919678, 112.667999267578, 74.585296630859);
  vgl_point_3d<double> p21(54.103698730469, 112.794998168945, 77.077697753906);
  vgl_point_3d<double> p22(57.787899017334, 110.442001342773, 76.552398681641);
  vgl_point_3d<double> p23(54.801998138428, 112.566001892090, 70.383003234863);
  vgl_point_3d<double> p24(59.452499389648, 111.444000244141, 71.053703308105);
  vgl_point_3d<double> p25(47.302501678467, 126.745002746582, 75.783500671387);
  vgl_point_3d<double> p26(46.396198272705, 125.186996459961, 72.619697570801);
  vgl_point_3d<double> p27(51.720401763916, 134.764007568359, 75.677497863770);
  vgl_point_3d<double> p28(72.612197875977, 128.964004516602, 77.276603698730);
  vgl_point_3d<double> p29(66.935501098633, 134.675003051758, 76.452903747559);
  vgl_point_3d<double> p30(60.889099121094, 136.768997192383, 77.920799255371);
  vgl_point_3d<double> p31(65.818496704102, 133.451004028320, 76.452903747559);
  vgl_point_3d<double> p32(68.137702941895, 134.052001953125, 78.947303771973);
  vgl_point_3d<double> p33(54.263698577881, 134.671997070313, 77.146003723145);
  vgl_point_3d<double> p34(58.597099304199, 137.714996337891, 79.837699890137);
  vgl_point_3d<double> p35(63.371498107910, 137.065002441406, 79.151000976563);
  vgl_point_3d<double> p36(74.211799621582, 127.301002502441, 74.958999633789);
  vgl_point_3d<double> p37(67.735099792480, 133.117004394531, 72.081703186035);
  vgl_point_3d<double> p38(71.525802612305, 132.054992675781, 73.299896240234);
  pts.push_back(p0);
  pts.push_back(p1);
  pts.push_back(p2);
  pts.push_back(p3);
  pts.push_back(p4);
  pts.push_back(p5);
  pts.push_back(p6);
  pts.push_back(p7);
  pts.push_back(p8);
  pts.push_back(p9);
  pts.push_back(p10);
  pts.push_back(p11);
  pts.push_back(p12);
  pts.push_back(p13);
  pts.push_back(p14);
  pts.push_back(p15);
  pts.push_back(p16);
  pts.push_back(p17);
  pts.push_back(p18);
  pts.push_back(p19);
  pts.push_back(p20);
  pts.push_back(p21);
  pts.push_back(p22);
  pts.push_back(p23);
  pts.push_back(p24);
  pts.push_back(p25);
  pts.push_back(p26);
  pts.push_back(p27);
  pts.push_back(p28);
  pts.push_back(p29);
  pts.push_back(p30);
  pts.push_back(p31);
  pts.push_back(p32);
  pts.push_back(p33);
  pts.push_back(p34);
  pts.push_back(p35);
  pts.push_back(p36);
  pts.push_back(p37);
  pts.push_back(p38);
  vgl_fit_cylinder_3d<double> fit_sph(pts);
  double error = fit_sph.fit(z, &std::cout);
  TEST_NEAR("fit actual cylinder data", error, 0.0, 2.0);
}
static void
test_fit_cylinder_3d()
{
  test_fit_unit_cylinder();
  test_compute_W_fit_unit_cylinder();
  test_fit_unit_cylinder_rand_error();
  test_compute_W_fit_unit_cylinder_rand_error();
  test_fit_actual_pts();
}

TESTMAIN(test_fit_cylinder_3d);
