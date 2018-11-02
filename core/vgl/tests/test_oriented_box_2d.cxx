// Some tests for vgl_oriented_box
// J.L. Mundy Dec. 2016
#include <iostream>
#include <sstream>
#include <cmath>
#include <testlib/testlib_test.h>
#include <vgl/vgl_oriented_box_2d.h>
#include <vgl/vgl_area.h>
#include <vgl/vgl_box_2d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_fit_oriented_box_2d.h>

static void test_oriented_box_2d()
{
  std::cout << "***************************\n"
           << " Testing vgl_oriented_box_2d\n"
           << "***************************\n\n";
  //test constructors and corner accessor
  vgl_point_2d<double> pmaj1(0.0, 100.0), pmaj2(200.0, 100.0);
  vgl_point_2d<double> pmin1(50.0, 150.0), pmin2(50.0, 0.0);
  vgl_oriented_box_2d<double> ob(pmaj1,pmaj2, pmin1, pmin2);
  std::vector<vgl_point_2d<double> > corns0 = ob.corners();
  vgl_point_2d<double> pmaj11(50.0, 50.0), pmaj21(150.0, 50.0);
  vgl_point_2d<double> pmin11(100.0, 75.0), pmin21(100.0, 25.0);
  vgl_oriented_box_2d<double> ob1(pmaj11,pmaj21, pmin11, pmin21);
  std::vector<vgl_point_2d<double> > corns1 = ob1.corners();

  vgl_oriented_box_2d<double> tbox1 = ob1.translate(200.0, 50.0);
  std::vector<vgl_point_2d<double> > trans_corns1 = tbox1.corners();
  TEST("translate obox", trans_corns1[2].x()+trans_corns1[2].y(), 475.0);

  vgl_point_2d<double> cent = tbox1.centroid();
  TEST("centroid", cent.x()+cent.y(), 400.0);

  vgl_oriented_box_2d<double> rtbox1 = tbox1.rotate(cent, 3.1415926535897/2.0);
  std::vector<vgl_point_2d<double> > rot_trans_corns1 = rtbox1.corners();
  TEST_NEAR("rotate box", rot_trans_corns1[2].x()+rot_trans_corns1[2].y(), 425.0, 0.000001);

  vgl_box_2d<double> bb = rtbox1.enclosing_box();
  double area = vgl_area(bb);
  TEST_NEAR("bounding box", area, 5000.0, 0.000001);
  std::stringstream ss;
  ss << ob;
  vgl_oriented_box_2d<double> ibox;
  ss >> ibox;
  bool eq = ibox == ob;
  TEST("read/write equal", eq, true);
  vgl_point_2d<float> pmaj1a(0.0f, 100.0f), pmaj2a(200.0f, 100.0f);

  vgl_oriented_box_2d<float> ob0(pmaj1a,pmaj2a, 150.0f);
  std::vector<vgl_point_2d<float> > corns = ob0.corners();
  std::cout << "Corners" << std::endl;
  for(auto & corn : corns)
    std::cout << corn << std::endl;
  float width = ob.width();
  float height = ob.height();
  vgl_point_2d<float> p(25.0, 25.0);
  vgl_point_2d<float> uv = ob0.transform_to_obox(p);
  std::cout << "width " << width << " height " << height << " uv " << uv << std::endl;
  vgl_box_2d<float> box = ob0.enclosing_box();
  std::cout << "axis_aligned box " << box << std::endl;
  TEST("box size", width == 200.0f && height == 150.0f, true);
  TEST("corners ", corns[2]==vgl_point_2d<float>(200.0f, 175.0f), true);
  TEST("uv ",  uv == vgl_point_2d<float>(-75.0f/200.0f, -0.5f), true);
  // test contains
  vgl_point_2d<float> pin(199.0, 140.0);
  vgl_point_2d<float> pout(175.0, 180.0);
  bool good = ob0.contains(pin) && !ob0.contains(pout);
  TEST("contains " , good , true);
  vgl_point_2d<float> p0(0.0f, 25.0f), p1(200.0f, 25.0f), p2(0.0f, 175.0);
  vgl_oriented_box_2d<float> ob1a(p0, p1, p2);
  TEST("construct from 3 pts", ob1a == ob0, true);
  vgl_box_2d<float> b, br;
  b.add(vgl_point_2d<float>(0.0f, 0.0f));
  b.add(vgl_point_2d<float>(100.0f, 150.0f));
  br.add(vgl_point_2d<float>(-25.0f, 25.0f));
  br.add(vgl_point_2d<float>(125.0f, 125.0f));
  vgl_oriented_box_2d<float> ob2a(b, -3.1415926535897932384626433832795f/2.0f);
  vgl_oriented_box_2d<float> ob3(br, 0.0f);
  TEST("test rotation constructors", ob2a==ob3, true);
  vgl_point_2d<float> p0a(0.0f, 0.0f), p1a(100.0f, 57.73503f),p2a(-25.0f, 43.30127f);
  vgl_oriented_box_2d<float> obang(p0a, p1a, p2a);
  float ang = obang.angle_in_rad();
  TEST_NEAR("orientation angle", ang, 0.523599f, 0.001);

  // test fiting an oriented box to points
  vgl_point_2d<double> p00(0.0, 0.0), p01(2.0, 0.0), p02(2.0, 1.0), p03(0.0, 1.0);
  std::vector<vgl_point_2d<double> > verts;
  verts.push_back(p00);  verts.push_back(p01);   verts.push_back(p02);   verts.push_back(p03);
  vgl_polygon<double> poly(verts);
  vgl_fit_oriented_box_2d<double> fob(poly);
  vgl_oriented_box_2d<double> obf = fob.fitted_box();
  double angf = obf.angle_in_rad(), wth = obf.width(), hht = obf.height();
  double er = fabs(sin(angf)) + fabs(wth-2)+ fabs(hht-1);
  TEST_NEAR("fit oriented box to pts", er, 0.0, 0.001);

  // test a real data case
  vgl_point_2d<double> p0r( 0.444941, -0.780737);
  vgl_point_2d<double> p1r(-0.823115, -2.21685);
  vgl_point_2d<double> p2r(-4.02411, -2.63267);
  vgl_point_2d<double> p3(-4.06477, -4.40946);
  vgl_point_2d<double> p4(-4.50972, -3.5838);
  vgl_point_2d<double> p5(-4.58641, -4.70911);
  vgl_point_2d<double> p6(-7.41408, -4.55561);
  vgl_point_2d<double> p7(-7.75148, -5.77787);
  vgl_point_2d<double> p8(-7.75148, -5.77787);
  vgl_point_2d<double> p9(-7.41408, -4.55561);
  vgl_point_2d<double> p10(-5.58869, -3.50253);
  vgl_point_2d<double> p11(-5.66042, -2.23449);
  vgl_point_2d<double> p12(-2.60774, -1.48014);
  vgl_point_2d<double> p13(-1.6005, -0.199361);
  vgl_point_2d<double> p14(3.20099, 0.40761);
  vgl_point_2d<double> p15(3.91161, 2.23784);
  vgl_point_2d<double> p16(6.18194, 2.47685);
  vgl_point_2d<double> p17(6.40684, 3.30224);
  vgl_point_2d<double> p18(12.0314, 6.14794);
  vgl_point_2d<double> p19(12.8904, 7.66868);
  vgl_point_2d<double> p20(12.8904, 7.66868);
  vgl_point_2d<double> p21(9.05535, 6.53773);
  vgl_point_2d<double> p22(9.23958, 5.59477);
  vgl_point_2d<double> p23(8.12469, 6.34489);
  vgl_point_2d<double> p24(6.33522, 4.64566);
  vgl_point_2d<double> p25(4.43809, 4.91831);
  vgl_point_2d<double> p26(3.17004, 3.52672);
  vgl_point_2d<double> p27(3.46666, 2.9751);
  vgl_point_2d<double> p28(2.12698, 2.94762);
  vgl_point_2d<double> p29(2.42361, 2.42879);
  vgl_point_2d<double> p30(0.59811, 1.39932);
 std::vector<vgl_point_2d<double> > pts;
  pts.push_back(p0r);  pts.push_back(p1r); pts.push_back(p2r);
  pts.push_back(p3);   pts.push_back(p4);  pts.push_back(p5);
  pts.push_back(p6);   pts.push_back(p7);  pts.push_back(p8);
  pts.push_back(p9);   pts.push_back(p10);  pts.push_back(p11);
  pts.push_back(p12);   pts.push_back(p13);  pts.push_back(p14);
  pts.push_back(p15);   pts.push_back(p16);  pts.push_back(p17);
  pts.push_back(p18);   pts.push_back(p19);  pts.push_back(p20);
  pts.push_back(p21);   pts.push_back(p22);  pts.push_back(p23);
  pts.push_back(p24);   pts.push_back(p25);  pts.push_back(p26);
  pts.push_back(p27);   pts.push_back(p28);  pts.push_back(p29); pts.push_back(p30);
  vgl_fit_oriented_box_2d<double> fob30(pts);
  vgl_oriented_box_2d<double> obf30 = fob30.fitted_box();
  std::vector<vgl_point_2d<double> > cns = obf30.corners();
  for(auto & cn : cns)
          std::cout << cn.x() << ' ' << cn.y() << std::endl;
  double arear = vgl_area(obf30);
  TEST_NEAR("real obox fit" , arear, 78.59471308, 1e-04);
  // second real fit
  vgl_point_2d<double> q0(-6.70591,-2.28258);
  vgl_point_2d<double> q1(-4.14291,-4.83427);
vgl_point_2d<double> q2(-0.631689,-4.85021);
vgl_point_2d<double> q3(0.176725,-6.1546);
vgl_point_2d<double> q4(-2.24703,-7.55325);
vgl_point_2d<double> q5(-0.0691315,-7.62193);
vgl_point_2d<double> q6(1.51102,-6.03368);
vgl_point_2d<double> q7(4.84672,-5.64279);
vgl_point_2d<double> q8(4.60103,-4.56791);
vgl_point_2d<double> q9(5.79494,-3.12394);
vgl_point_2d<double> q10(5.9711,4.3438);
vgl_point_2d<double> q11(3.47803,3.64558);
vgl_point_2d<double> q12(1.61627,4.71676);
vgl_point_2d<double> q13(1.65141,5.66017);
vgl_point_2d<double> q14(-0.59568,6.50897);
vgl_point_2d<double> q15(-2.24605,5.52854);
vgl_point_2d<double> q16(-3.1589,6.50555);
vgl_point_2d<double> q17(-4.70391,5.78521);
vgl_point_2d<double> q18(-4.77425,3.85846);
vgl_point_2d<double> q19(-5.54678,3.45917);
vgl_point_2d<double> q20(-4.77448,1.23897);
 std::vector<vgl_point_2d<double> > qpts;
 qpts.push_back(q0); qpts.push_back(q1); qpts.push_back(q2); qpts.push_back(q3);
 qpts.push_back(q4); qpts.push_back(q5); qpts.push_back(q6); qpts.push_back(q7);
 qpts.push_back(q8); qpts.push_back(q9); qpts.push_back(q10); qpts.push_back(q11);
 qpts.push_back(q12); qpts.push_back(q13); qpts.push_back(q14); qpts.push_back(q15);
 qpts.push_back(q16); qpts.push_back(q17); qpts.push_back(q18); qpts.push_back(q19);
 qpts.push_back(q20);
 vgl_fit_oriented_box_2d<double> q44(qpts);
 double ang_rad = 1.378819939;
 vgl_oriented_box_2d<double> ob44 = q44.fitted_box(ang_rad);
 double angq = ob44.angle_in_rad();
 double w44 = ob44.width();
 double gtw = 14.11896874;
 double er44 =  fabs(w44-gtw)+ fabs(angq-ang_rad);
 TEST_NEAR("specify_angle", er44, 0.0, 0.0001);
}

TESTMAIN(test_oriented_box_2d);
