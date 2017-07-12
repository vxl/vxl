// Some tests for vgl_fit_sphere_3d
// J.L. Mundy, July 7, 2017.
#include <iostream>
#include <cstdlib>
#include <testlib/testlib_test.h>
#include <vgl/algo/vgl_fit_quadric_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_distance.h>
#include <vcl_compiler.h>
#include <vnl/vnl_random.h>
#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_generalized_eigensystem.h>
// creates points on the unit sphere
static void test_linear_fit_sphere_taubin()
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
  vgl_point_3d<double> p8(-r, -r, r);
  vgl_point_3d<double> p9(-r, r, -r);
  vgl_point_3d<double> p10(r, -r, -r);
  vgl_point_3d<double> p11(r, r, -r);
  vgl_point_3d<double> p12(r, -r, r);
  vgl_point_3d<double> p13(-r, r, r);
  pts.push_back(p0);  pts.push_back(p1);  pts.push_back(p2);
  pts.push_back(p3);  pts.push_back(p4);  pts.push_back(p5);
  pts.push_back(p6);  pts.push_back(p7);  pts.push_back(p8);
  pts.push_back(p9);  pts.push_back(p10);  pts.push_back(p11);
  pts.push_back(p12); pts.push_back(p13);
  vgl_fit_quadric_3d<double> fit_sph(pts);
  double error = fit_sph.fit_linear_Taubin(&std::cout);
  TEST_NEAR("linear fit Taubin perfect unit sphere", error, 0.0, 1e-6);
}
static void test_linear_fit_taubin_sphere_rand_error()
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
  vgl_point_3d<double> p8(-r, -r, r);
  vgl_point_3d<double> p9(-r, r, -r);
  vgl_point_3d<double> p10(r, -r, -r);
  vgl_point_3d<double> p11(r, r, -r);
  vgl_point_3d<double> p12(r, -r, r);
  vgl_point_3d<double> p13(-r, r, r);
  pts.push_back(p0);  pts.push_back(p1);  pts.push_back(p2);
  pts.push_back(p3);  pts.push_back(p4);  pts.push_back(p5);
  pts.push_back(p6);  pts.push_back(p7);  pts.push_back(p8);
  pts.push_back(p9);  pts.push_back(p10);  pts.push_back(p11);
  pts.push_back(p12); pts.push_back(p13);
  // add random error (+-10%) to the points
  vnl_random ran;
  double e = 0.1;
  std::cout << "error " << e << std::endl;
  for(std::vector<vgl_point_3d<double> >::iterator pit = pts.begin();
      pit != pts.end(); ++pit){
    vgl_vector_3d<double> er(ran.drand32(-e, e), ran.drand32(-e, e), ran.drand32(-e, e));
    (*pit) = (*pit) + er;
  }
  vgl_fit_quadric_3d<double> fit_sph(pts);
  double error = fit_sph.fit_linear_Taubin(&std::cout);
  TEST_NEAR("linear fit sphere with error", error, 0.0, 0.1);
}
static void test_linear_fit_Taubin_parabolid()
{
  std::vector<vgl_point_3d<double> > pts;
  vgl_point_3d<double> p0(1.0, 1.0, 2.0);
  vgl_point_3d<double> p1(2.0, 2.0, 8.0);
  vgl_point_3d<double> p2(3.0, 3.0, 18.0);
  vgl_point_3d<double> p3(-1.0, -1.0, 2.0);
  vgl_point_3d<double> p4(-2.0, -2.0, 8.0);
  vgl_point_3d<double> p5(-3.0, -3.0, 18.0);
  vgl_point_3d<double> p6(1.0, -1.0, 2.0);
  vgl_point_3d<double> p7(2.0, -2.0, 8.0);
  vgl_point_3d<double> p8(3.0, -3.0, 18.0);
  vgl_point_3d<double> p9(-1.0, 1.0, 2.0);
  vgl_point_3d<double> p10(-2.0, 2.0, 8.0);
  vgl_point_3d<double> p11(-3.0, 3.0, 18.0);
  pts.push_back(p0); pts.push_back(p1); pts.push_back(p2); pts.push_back(p3);
  pts.push_back(p4); pts.push_back(p5); pts.push_back(p6); pts.push_back(p7);
  pts.push_back(p8); pts.push_back(p9); pts.push_back(p10); pts.push_back(p11);
  vgl_fit_quadric_3d<double> fit_parab(pts);
  double lin_error = fit_parab.fit_linear_Taubin(&std::cout);
  TEST_NEAR("linear fit parabolid", lin_error, 0.0, 0.25);
}
static void test_linear_fit_Taubin_parabolid_rand_error()
{
  std::vector<vgl_point_3d<double> > pts;
  vgl_point_3d<double> p0(1.0, 1.0, 2.0);
  vgl_point_3d<double> p1(2.0, 2.0, 8.0);
  vgl_point_3d<double> p2(3.0, 3.0, 18.0);
  vgl_point_3d<double> p3(-1.0, -1.0, 2.0);
  vgl_point_3d<double> p4(-2.0, -2.0, 8.0);
  vgl_point_3d<double> p5(-3.0, -3.0, 18.0);
  vgl_point_3d<double> p6(1.0, -1.0, 2.0);
  vgl_point_3d<double> p7(2.0, -2.0, 8.0);
  vgl_point_3d<double> p8(3.0, -3.0, 18.0);
  vgl_point_3d<double> p9(-1.0, 1.0, 2.0);
  vgl_point_3d<double> p10(-2.0, 2.0, 8.0);
  vgl_point_3d<double> p11(-3.0, 3.0, 18.0);
  pts.push_back(p0); pts.push_back(p1); pts.push_back(p2); pts.push_back(p3);
  pts.push_back(p4); pts.push_back(p5); pts.push_back(p6); pts.push_back(p7);
  pts.push_back(p8); pts.push_back(p9); pts.push_back(p10); pts.push_back(p11);
  // add random error to the points
  vnl_random ran;
  double e = 0.5;
  std::cout << "error " << e << std::endl;
  for(std::vector<vgl_point_3d<double> >::iterator pit = pts.begin();
      pit != pts.end(); ++pit){
    vgl_vector_3d<double> er(ran.drand32(-e, e), ran.drand32(-e, e), ran.drand32(-e, e));
    (*pit) = (*pit) + er;
  }
  vgl_fit_quadric_3d<double> fit_parab(pts);
  double lin_error = fit_parab.fit_linear_Taubin(&std::cout);
  TEST_NEAR("linear fit parabolid with noise", lin_error, 0.0, 0.25);
}
static void test_linear_fit_sphere_Taubin_actual_pts()
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
  vgl_fit_quadric_3d<double> fit_sph_act(pts);
  double lin_error = fit_sph_act.fit_linear_Taubin(&std::cout);
  TEST_NEAR("linear fit sphere sclara pts", lin_error, 0.0, 0.25);
}
static void test_linear_fit_sphere_Alliare(){
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
  vgl_point_3d<double> p8(-r, -r, r);
  vgl_point_3d<double> p9(-r, r, -r);
  vgl_point_3d<double> p10(r, -r, -r);
  vgl_point_3d<double> p11(r, r, -r);
  vgl_point_3d<double> p12(r, -r, r);
  vgl_point_3d<double> p13(-r, r, r);
  pts.push_back(p0);  pts.push_back(p1);  pts.push_back(p2);
  pts.push_back(p3);  pts.push_back(p4);  pts.push_back(p5);
  pts.push_back(p6);  pts.push_back(p7);  pts.push_back(p8);
  pts.push_back(p9);  pts.push_back(p10);  pts.push_back(p11);
  pts.push_back(p12); pts.push_back(p13);
  vgl_fit_quadric_3d<double> fit_sph(pts);
  double error = fit_sph.fit_ellipsoid_linear_Allaire(&std::cout);
  TEST_NEAR("linear fit Allaire perfect unit sphere", error, 0.0, 1e-6);
}
static void test_linear_fit_Allaire_sphere_rand_error()
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
  vgl_point_3d<double> p8(-r, -r, r);
  vgl_point_3d<double> p9(-r, r, -r);
  vgl_point_3d<double> p10(r, -r, -r);
  vgl_point_3d<double> p11(r, r, -r);
  vgl_point_3d<double> p12(r, -r, r);
  vgl_point_3d<double> p13(-r, r, r);
  pts.push_back(p0);  pts.push_back(p1);  pts.push_back(p2);
  pts.push_back(p3);  pts.push_back(p4);  pts.push_back(p5);
  pts.push_back(p6);  pts.push_back(p7);  pts.push_back(p8);
  pts.push_back(p9);  pts.push_back(p10);  pts.push_back(p11);
  pts.push_back(p12); pts.push_back(p13);
  vnl_random ran;
  double e = 0.5;
  std::cout << "error " << e << std::endl;
  for(std::vector<vgl_point_3d<double> >::iterator pit = pts.begin();
      pit != pts.end(); ++pit){
    vgl_vector_3d<double> er(ran.drand32(-e, e), ran.drand32(-e, e), ran.drand32(-e, e));
    (*pit) = (*pit) + er;
  }
  vgl_fit_quadric_3d<double> fit_sph(pts);
  double error = fit_sph.fit_ellipsoid_linear_Allaire(&std::cout);
  TEST_NEAR("linear fit Allaire sphere with error", error, 0.0, 0.25);
}
static void test_linear_fit_Allaire_thin_ellipsoid_rand_error()
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
  vgl_point_3d<double> p8(-r, -r, r);
  vgl_point_3d<double> p9(-r, r, -r);
  vgl_point_3d<double> p10(r, -r, -r);
  vgl_point_3d<double> p11(r, r, -r);
  vgl_point_3d<double> p12(r, -r, r);
  vgl_point_3d<double> p13(-r, r, r);
  pts.push_back(p0);  pts.push_back(p1);  pts.push_back(p2);
  pts.push_back(p3);  pts.push_back(p4);  pts.push_back(p5);
  pts.push_back(p6);  pts.push_back(p7);  pts.push_back(p8);
  pts.push_back(p9);  pts.push_back(p10);  pts.push_back(p11);
  pts.push_back(p12); pts.push_back(p13);

  vnl_random ran;
  double e = 0.0;
  std::cout << "error " << e << std::endl;
  for(std::vector<vgl_point_3d<double> >::iterator pit = pts.begin();
      pit != pts.end(); ++pit){
    vgl_vector_3d<double> er(ran.drand32(-e, e), ran.drand32(-e, e), ran.drand32(-e, e));
    (*pit) = (*pit) + er;
  }
  vgl_fit_quadric_3d<double> fit_sph(pts);
  double error = fit_sph.fit_ellipsoid_linear_Allaire(&std::cout);
  TEST_NEAR("linear fit Allaire thin ellipsoid with error", error, 0.0, 0.5);
}
static void test_linear_fit_Allaire_saddle_shape()
{
   std::vector<vgl_point_3d<double> > pts;
   vgl_point_3d<double> p0(1.0, 0.0, 0.0);
  vgl_point_3d<double> p1(0.0, 1.0, 0.0);
  vgl_point_3d<double> p2(-1.0, 0.0, 0.0);
  vgl_point_3d<double> p3(0.0, -1.0, 0.0);
  vgl_point_3d<double> p4(2, 0.0, 1.0);
  vgl_point_3d<double> p5(0.0, 2.0,1.0);
  vgl_point_3d<double> p6(-2.0, 0.0,1.0);
  vgl_point_3d<double> p7(0.0, -2.0,1.0);
  vgl_point_3d<double> p8(2, 0.0, -1.0);
  vgl_point_3d<double> p9(0.0, 2.0,-1.0);
  vgl_point_3d<double> p10(-2.0, 0.0,-1.0);
  vgl_point_3d<double> p11(0.0, -2.0,-1.0);


  pts.push_back(p0);  pts.push_back(p1);  pts.push_back(p2);
  pts.push_back(p3);  pts.push_back(p4);  pts.push_back(p5);
  pts.push_back(p6);  pts.push_back(p7);  pts.push_back(p8);
  pts.push_back(p9);  pts.push_back(p10);  pts.push_back(p11);


  vnl_random ran;
  double e = 0.5;
  std::cout << "error " << e << std::endl;
  for(std::vector<vgl_point_3d<double> >::iterator pit = pts.begin();
      pit != pts.end(); ++pit){
    vgl_vector_3d<double> er(ran.drand32(-e, e), ran.drand32(-e, e), ran.drand32(-e, e));
    (*pit) = (*pit) + er;
  }
  vgl_fit_quadric_3d<double> fit_sad(pts);
  double error = fit_sad.fit_saddle_shaped_quadric_linear_Allaire(&std::cout);
  TEST_NEAR("linear fit Allaire saddle", error, 0.0, 0.5);
}
static void test_fit_quadric_3d()
{
#if 0
  test_linear_fit_sphere_taubin();
  test_linear_fit_taubin_sphere_rand_error();
  test_linear_fit_Taubin_parabolid();
  test_linear_fit_Taubin_parabolid_rand_error();
  test_linear_fit_sphere_Taubin_actual_pts();
  test_linear_fit_sphere_Alliare();
  test_linear_fit_Allaire_sphere_rand_error();
  test_linear_fit_Allaire_thin_ellipsoid_rand_error();
#endif
  test_linear_fit_Allaire_saddle_shape();
}

TESTMAIN(test_fit_quadric_3d);
