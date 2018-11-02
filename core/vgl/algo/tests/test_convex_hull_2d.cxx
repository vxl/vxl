//:
// \file
// \author Joseph Mundy
// \date  July 14, 2004

#include <vector>
#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_area.h>
#include <vgl/vgl_oriented_box_2d.h>
#include <vgl/algo/vgl_convex_hull_2d.h>

static void test_4_point_hull()
{
  vgl_point_2d<double> p0(0.0,0.0), p1(2.0,0.0);
  vgl_point_2d<double> p2(1.0,2.0), p3(1.0,1.0);
  vgl_point_2d<double> p4(0.5,0.25), p5(0.4,0.7);
  std::vector<vgl_point_2d<double> > points;
  points.push_back(p0);   points.push_back(p1);
  points.push_back(p2);   points.push_back(p3);
  points.push_back(p4);   points.push_back(p5);
  vgl_convex_hull_2d<double> ch(points);
  vgl_polygon<double> poly = ch.hull();
  std::cout << poly << '\n';
  // A convex hull should be convex, and consequently only have a single sheet:
  TEST("Hull is convex", poly.num_sheets(), 1);
  // Points p3, p4 and p5 should lie inside the other 3:
  TEST("Hull shape is triangular", poly.num_vertices(), 3);
  TEST("Hull point 1", poly[0][0], p0);
  TEST("Hull point 2", poly[0][1], p1);
  TEST("Hull point 3", poly[0][2], p2);
  // Verify that the 3 other points are inside the convex hull:
  TEST("All points are inside the convex hull",
       poly.contains(p3) && poly.contains(p4) && poly.contains(p5), true);

  // test the minimum area bounding rectangle
  vgl_oriented_box_2d<double> obox = ch.min_area_enclosing_rectangle();
  std::cout << obox << std::endl;
  vgl_point_2d<double> c =  obox.centroid();
  double a = vgl_area(obox);
  bool good = a == 4.0;
  good = good && c.x()==1.0 && c.y() == 1.0;
  TEST("enclosing rectangle", good, true);
}

static void test_5_point_hull()
{
  vgl_point_2d<double> p0(0.0,0.0), p1(2.0,0.0);
  vgl_point_2d<double> p2(1.0,2.0), p3(2.0,4.0);
  vgl_point_2d<double> p4(0.0,4.0);
  std::vector<vgl_point_2d<double> > points;
  points.push_back(p0);   points.push_back(p1);
  points.push_back(p2);   points.push_back(p3);
  points.push_back(p4);
  vgl_convex_hull_2d<double> ch(points);
  vgl_polygon<double> poly = ch.hull();
  std::cout << poly << '\n';
  // test the minimum area bounding rectangle
  vgl_oriented_box_2d<double> obox = ch.min_area_enclosing_rectangle();
  vgl_point_2d<double> c = obox.centroid();
  double h = obox.height();
  bool good = c.x()== 1.0 && c.y()==2.0;
  good = good && h == 2.0;
  std::cout << obox << std::endl;
  TEST("enclosing rect 5 pts", good, true);
}
static void test_obox_large_hull()
{
  vgl_point_2d<double> p0(-16.8281136,5.36292028);
  vgl_point_2d<double> p1(-16.1361694,-0.0697314814);
  vgl_point_2d<double> p2(-14.9275990,-4.09468365);
  vgl_point_2d<double> p3(-14.8412733,-4.38188696);
  vgl_point_2d<double> p4(-14.5822935,-5.24350071);
  vgl_point_2d<double> p5(-14.4959669,-5.53070450);
  vgl_point_2d<double> p6(-14.4096403,-5.81790781);
  vgl_point_2d<double> p7(-10.8729181,-11.3334799);
  vgl_point_2d<double> p8(-10.4129543,-11.8236399);
  vgl_point_2d<double> p9(-9.57935429,-12.5126657);
  vgl_point_2d<double> p10(-2.22391939,-10.9297152);
  vgl_point_2d<double> p11(-0.500052691,-10.4201393);
  vgl_point_2d<double> p12(0.361880541,-10.1653509);
  vgl_point_2d<double> p13(0.936503410,-9.99276257);
  vgl_point_2d<double> p14(2.66037178,-9.47499657);
  vgl_point_2d<double> p15(2.94768238,-9.38870239);
  vgl_point_2d<double> p16(3.23499274,-9.30240822);
  vgl_point_2d<double> p17(22.1975346,-3.59060740);
  vgl_point_2d<double> p18(22.1112080,-3.29930997);
  vgl_point_2d<double> p19(21.8522282,-2.43769574);
  vgl_point_2d<double> p20(21.7659016,-2.15049243);
  vgl_point_2d<double> p21(18.5718231,8.47196579);
  vgl_point_2d<double> p22(17.6802254,10.3961029);
  vgl_point_2d<double> p23(8.76825523,20.2528687);
  vgl_point_2d<double> p24(-15.3632269,6.74229956);
  vgl_point_2d<double> p25(-16.1388321,6.19621229);
  vgl_point_2d<double> p26(-16.6271286,5.73641920);
  std::vector<vgl_point_2d<double> > pts;
  pts.push_back(p0);  pts.push_back(p1);   pts.push_back(p2);
  pts.push_back(p3);  pts.push_back(p4);   pts.push_back(p5);
  pts.push_back(p6);  pts.push_back(p7);   pts.push_back(p8);
  pts.push_back(p9);  pts.push_back(p10);   pts.push_back(p11);
  pts.push_back(p12);  pts.push_back(p13);   pts.push_back(p14);
  pts.push_back(p15);  pts.push_back(p16);   pts.push_back(p17);
  pts.push_back(p18);  pts.push_back(p19);   pts.push_back(p20);
  pts.push_back(p21);  pts.push_back(p22);   pts.push_back(p23);
  pts.push_back(p24);  pts.push_back(p25);   pts.push_back(p26);
  vgl_convex_hull_2d<double> ch(pts);
  vgl_polygon<double> poly = ch.hull();
  std::vector<vgl_point_2d<double> > hverts = poly[0];
  size_t n = hverts.size();
  bool good = (n == pts.size());
  if(good)
    for(size_t i = 0; i<n; ++i)
      good = good && hverts[i]==pts[i];
  TEST("cv_hull forms a cv_hull", good , true);
  good =true;
  vgl_oriented_box_2d<double> obox = ch.min_area_enclosing_rectangle();
  for(size_t i = 0; i<n; ++i)
    good = good && obox.contains(pts[i]);
  TEST("cv_hull is contained in obox", good , true);
}
static void test_convex_hull_2d()
{
  test_4_point_hull();
  test_5_point_hull();
  test_obox_large_hull();
}

TESTMAIN(test_convex_hull_2d);
