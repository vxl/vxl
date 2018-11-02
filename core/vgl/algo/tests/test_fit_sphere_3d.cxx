// Some tests for vgl_fit_sphere_3d
// J.L. Mundy, June 8, 2015.
#include <iostream>
#include <cstdlib>
#include <testlib/testlib_test.h>
#include <vgl/algo/vgl_fit_sphere_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_distance.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_random.h>
// creates points on the unit sphere
static void test_linear_fit_unit_sphere()
{
  std::vector<vgl_point_3d<double> > pts;
  vgl_point_3d<double> p0(1.0, 0.0, 0.0);
  vgl_point_3d<double> p1(0.0, 1.0, 0.0);
  vgl_point_3d<double> p2(0.0, 0.0, 1.0);
  vgl_point_3d<double> p3(-1.0, 0.0, 0.0);
  vgl_point_3d<double> p4(0.0, -1.0, 0.0);
  vgl_point_3d<double> p5(0.0, 0.0, -1.0);
  double r = 0.5773503;
  vgl_point_3d<double> p6(r, r, r);
  vgl_point_3d<double> p7(-r, -r, -r);
  pts.push_back(p0);  pts.push_back(p1);  pts.push_back(p2);
  pts.push_back(p3);  pts.push_back(p4);  pts.push_back(p5);
  pts.push_back(p6);  pts.push_back(p7);
  vgl_fit_sphere_3d<double> fit_sph(pts);
  double error = fit_sph.fit_linear(&std::cout);
  TEST_NEAR("linear fit perfect unit sphere", error, 0.0, 1e-6);
  double non_lin_error = fit_sph.fit(&std::cout, true);
  TEST_NEAR("nonlinear fit perfect unit sphere", non_lin_error, 0.0, 1e-6);
}
static void test_linear_fit_unit_sphere_rand_error()
{
  std::vector<vgl_point_3d<double> > pts;
  vgl_point_3d<double> p0(1.0, 0.0, 0.0);
  vgl_point_3d<double> p1(0.0, 1.0, 0.0);
  vgl_point_3d<double> p2(0.0, 0.0, 1.0);
  vgl_point_3d<double> p3(-1.0, 0.0, 0.0);
  vgl_point_3d<double> p4(0.0, -1.0, 0.0);
  vgl_point_3d<double> p5(0.0, 0.0, -1.0);
  double r = 0.5773503;
  vgl_point_3d<double> p6(r, r, r);
  vgl_point_3d<double> p7(-r, -r, -r);
  pts.push_back(p0);  pts.push_back(p1);  pts.push_back(p2);
  pts.push_back(p3);  pts.push_back(p4);  pts.push_back(p5);
  pts.push_back(p6);  pts.push_back(p7);
  // add random error (+-10%) to the points
  vnl_random ran;
  double e = 0.1;
  for(auto & pt : pts){
    vgl_vector_3d<double> er(ran.drand32(-e, e), ran.drand32(-e, e), ran.drand32(-e, e));
    pt = pt + er;
  }
  vgl_fit_sphere_3d<double> fit_sph(pts);
  double error = fit_sph.fit_linear(&std::cout);
  TEST_NEAR("linear fit unit sphere with error", error, 0.0, 0.1);
  double non_lin_error = fit_sph.fit(&std::cout, true);
  TEST_NEAR("nonlinear fit unit sphere with error", non_lin_error, 0.0, 0.1);
}
static void test_linear_fit_actual_pts()
{
  std::vector<vgl_point_3d<double> > pts;
  vgl_point_3d<double> p0(-10.356499671936,68.164497375488,48.227199554443);
  vgl_point_3d<double> p1(-12.030300140381,68.351600646973,46.570499420166);
  vgl_point_3d<double> p2(-12.924599647522,70.128097534180,46.037700653076);
  vgl_point_3d<double> p3(-12.602700233459,72.161201477051,47.159500122070);
  vgl_point_3d<double> p4(-11.404299736023,75.049201965332,49.112400054932);
  vgl_point_3d<double> p5(-9.647660255432,71.332901000977,49.860500335693);
  vgl_point_3d<double> p6(-13.605299949646,72.556297302246,46.091999053955);
  vgl_point_3d<double> p7(-15.126000404358,71.693496704102,43.016601562500);
  vgl_point_3d<double> p8(-16.063600540161,70.053901672363,41.029098510742);
  vgl_point_3d<double> p9(-13.483099937439,68.981697082520,44.901100158691);
  vgl_point_3d<double> p10(-9.131649971008,69.801300048828,49.823600769043);
  vgl_point_3d<double> p11(-8.480529785156,67.906501770020,50.200801849365);
  vgl_point_3d<double> p12(4.875539779663,71.071899414063,50.690101623535);
  vgl_point_3d<double> p13(-3.617100000381,67.346603393555,51.243801116943);
  vgl_point_3d<double> p14(-4.381750106812,68.257698059082,51.418098449707);
  vgl_point_3d<double> p15(-5.963240146637,68.928100585938,51.265399932861);
  vgl_point_3d<double> p16(3.054660081863,68.043899536133,51.342098236084);
  vgl_point_3d<double> p17(-11.062999725342,72.394500732422,48.930099487305);
  vgl_point_3d<double> p18(-11.003100395203,70.369201660156,48.363498687744);
  vgl_point_3d<double> p19(5.466730117798,69.970596313477,50.311599731445);
  vgl_point_3d<double> p20(-11.655500411987,71.570999145508,48.103099822998);
  vgl_point_3d<double> p21(-10.344300270081,72.406997680664,49.586498260498);
  vgl_point_3d<double> p22(-11.970600128174,72.936203002930,48.114101409912);
  vgl_point_3d<double> p23(-13.019499778748,73.698402404785,47.480598449707);
  vgl_point_3d<double> p24(4.039299964905,69.958900451660,50.850700378418);
  pts.push_back(p0); pts.push_back(p1); pts.push_back(p2); pts.push_back(p3);
  pts.push_back(p4); pts.push_back(p5); pts.push_back(p6); pts.push_back(p7);
  pts.push_back(p8); pts.push_back(p9); pts.push_back(p10); pts.push_back(p11);
  pts.push_back(p12); pts.push_back(p13); pts.push_back(p14); pts.push_back(p15);
  pts.push_back(p16); pts.push_back(p17); pts.push_back(p18); pts.push_back(p19);
  pts.push_back(p20); pts.push_back(p21); pts.push_back(p22); pts.push_back(p23);
  pts.push_back(p24);
  vgl_fit_sphere_3d<double> fit_sph(pts);
  double lin_error = fit_sph.fit_linear(&std::cout);
  TEST_NEAR("linear fit actual sphere data", lin_error, 0.0, 0.25);
  double non_lin_error = fit_sph.fit(&std::cout, true);
  TEST_NEAR("nonlinear fit actual sphere data", non_lin_error, 0.0, 0.25);
}
static void test_fit_sphere_3d()
{
  test_linear_fit_unit_sphere();
  test_linear_fit_unit_sphere_rand_error();
  test_linear_fit_actual_pts();
}

TESTMAIN(test_fit_sphere_3d);
