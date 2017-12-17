// Some tests for vgl_oriented_box
// J.L. Mundy Dec. 2016
#include <iostream>
#include <sstream>
#include <testlib/testlib_test.h>
#include <vgl/vgl_oriented_box_2d.h>
#include <vgl/vgl_area.h>
#include <vgl/vgl_box_2d.h>
#include <vcl_compiler.h>
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
  for(std::vector<vgl_point_2d<float> >::iterator cit = corns.begin();
      cit != corns.end(); ++cit)
    std::cout << *cit << std::endl;
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
  double er = fabs(angf) + fabs(wth-2), fabs(hht-1);
  TEST_NEAR("fit oriented box to pts", er, 0.0, 0.001);
}

TESTMAIN(test_oriented_box_2d);
