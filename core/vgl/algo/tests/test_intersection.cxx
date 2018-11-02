// Some tests for vgl_intersection
// Gamze Tunali, Jan 2007.
#include <iostream>
#include <limits>
#include <list>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <testlib/testlib_test.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_intersection.h>
#include <vgl/vgl_line_3d_2_points.h>
#include <vgl/vgl_line_segment_3d.h>
#include <vgl/vgl_ray_3d.h>
#include <vgl/algo/vgl_intersection.h>

static void test_plane_intersection()
{
  vgl_plane_3d<double> pl1(vgl_vector_3d<double>(10,10,10), vgl_point_3d<double>(10,0,-10));
  vgl_line_3d_2_points<double> l1(vgl_point_3d<double>(1,4,1),vgl_point_3d<double>(-1,-4,-1));
  vgl_point_3d<double> pt1 = vgl_intersection(l1, pl1);
  TEST_NEAR("vgl_intersection(l1,pl1) = O", vgl_distance(pt1, vgl_point_3d<double>(0,0,0)), 0.0, 1e-8);

  vgl_line_3d_2_points<double> l2(vgl_point_3d<double>(0,0,0),vgl_point_3d<double>(10,0,-10));
  vgl_point_3d<double> pt2 = vgl_intersection(l2, pl1);

  const double inf = std::numeric_limits<double>::infinity();
  TEST("vgl_intersection(l2,pl1) = (inf,inf,inf)", pt2,
       vgl_point_3d<double>(inf, inf, inf));

  vgl_line_3d_2_points<double> l3(vgl_point_3d<double>(0,10,0),vgl_point_3d<double>(10,10,-10));
  vgl_point_3d<double> pt3 = vgl_intersection(l3, pl1);
  TEST("vgl_intersection(l2,p1) = (inf,0,0)", pt3, vgl_point_3d<double>(inf, 0, 0));

  vgl_line_segment_3d<double> l4(vgl_point_3d<double>(1,4,1),vgl_point_3d<double>(-1,-4,-1));
  vgl_point_3d<double> pt4;
  bool rv4 = vgl_intersection(l4, pl1, pt4);
  TEST("Found intersection(l4,pl1)", rv4, true);
  TEST_NEAR("vgl_intersection(l4,pl1) = O", vgl_distance(pt4, vgl_point_3d<double>(0,0,0)), 0.0, 1e-8);

  vgl_line_segment_3d<double> l5(vgl_point_3d<double>(0,0,0),vgl_point_3d<double>(10,0,-10));
  vgl_point_3d<double> pt5;
  bool rv5 = vgl_intersection(l5, pl1, pt5);

  TEST("Found intersection(l5,pl1)", rv5, true);
  TEST("vgl_intersection(l5,pl1) = (inf,inf,inf)", pt5,
       vgl_point_3d<double>(inf, inf, inf));

  vgl_line_segment_3d<double> l6(vgl_point_3d<double>(0,10,0),vgl_point_3d<double>(10,10,-10));
  vgl_point_3d<double> pt6;
  bool rv6 = vgl_intersection(l6, pl1, pt6);
  TEST("Found no intersection(l6,pl1)", rv6, false);

  vgl_line_segment_3d<double> l7(vgl_point_3d<double>(1,4,1),vgl_point_3d<double>(0,0,0));
  vgl_point_3d<double> pt7;
  bool rv7 = vgl_intersection(l7, pl1, pt7);
  TEST("Found intersection(l7,pl1)", rv7, true);
  TEST_NEAR("vgl_intersection(l4,pl1) = O", vgl_distance(pt7, vgl_point_3d<double>(0,0,0)), 0.0, 1e-8);


  vgl_line_segment_3d<double> l8(vgl_point_3d<double>(1,4,1),vgl_point_3d<double>(1e-5,1e-5,1e-5));
  vgl_point_3d<double> pt8;
  bool rv8 = vgl_intersection(l8, pl1, pt8);
  TEST("Found no intersection(l8,pl1)", rv8, false);
}

static void test_three_planes()
{
  vgl_plane_3d<double> pl4(1,0,0,-3), pl5(0,1,0,8), pl6(0,0,1,-12);
  {
   vgl_point_3d<double> pi(pl4,pl5,pl6); // intersection
   TEST("intersection", pi, vgl_point_3d<double>(3,-8,12));
   TEST("is_ideal", is_ideal(pi), false);
   vgl_point_3d<double> pj = vgl_intersection(pl4,pl5,pl6);
   TEST("three planes intersecting", pj, pi);
  }
}

static void test_plane_plane()
{
  // Intersect the YZ plane with the plane Y=Z:
  vgl_plane_3d<double> plane1(1,0,0,0), plane2(0,1,-1,0);
  vgl_infinite_line_3d<double> line;
  bool good = vgl_intersection(plane1, plane2, line);
  TEST("plane-plane intersection", good, true);
  if (good) {
    TEST("line points", line.contains(vgl_point_3d<double>(0,0,0))
                     && line.contains(vgl_point_3d<double>(0,1,1)), true);
  }
  // And more intersections: three planes: X=2, Y=3, and Z=1:
  vgl_plane_3d<double> planex(1,0,0,-2),planey(0,1,0,-3), planez(0,0,1,-1);
  vgl_line_segment_3d<double> linexy, linexz, lineyz;
  // intersecting x-y
  good = vgl_intersection(planex, planey, linexy);
  // intersecting x-z
  good = good && vgl_intersection(planez, planex, linexz);
  //intersecting y-z
  good = good && vgl_intersection(planey, planez, lineyz);
  TEST("plane-plane intersections", good, true);
  if (good) {
    TEST("x-y intersection", linexy.point1()==vgl_point_3d<double>(2,3,0)
                          && linexy.point2()==vgl_point_3d<double>(2,3,1), true);
    TEST("x-z intersection", linexz.point1()==vgl_point_3d<double>(2,0,1)
                          && linexz.point2()==vgl_point_3d<double>(2,1,1), true);
    TEST("y-z intersection", lineyz.point1()==vgl_point_3d<double>(0,3,1)
                          && lineyz.point2()==vgl_point_3d<double>(1,3,1), true);
  }
  // And finally two parallel planes (X=0 and X=2):
  good = vgl_intersection(plane1, planex, line);
  TEST("parallel planes do not intersect", good, false);
}

static void test_multiple_planes()
{
  //The line passes through (2,1,1), with direction (0.577, 0.577, 0.577)
  double s3 = 0.577350269;
  vgl_plane_3d<double> pl0(0,s3,-s3,0.0);

  vgl_plane_3d<double> pl1(-s3,0,s3, s3);

  vgl_plane_3d<double> pl2(s3,-s3,0,-s3);

  std::list<vgl_plane_3d<double> > planes;
  planes.push_back(pl0);   planes.push_back(pl1); planes.push_back(pl2);
  vgl_infinite_line_3d<double> line = vgl_intersection(planes);
  vgl_point_3d<double> p0(2, 1, 1);
  bool is_on = line.contains(p0);
  TEST("intersection of multiple planes -> line", is_on, true );

  vgl_plane_3d<double> wpl0(0,1,0,0.0);
  vgl_plane_3d<double> wpl1(1,0,0, 0.0);
  vgl_plane_3d<double> wpl2(0.707,0.707,0.0,1);
  std::list<vgl_plane_3d<double> > wplanes;
  wplanes.push_back(wpl0); wplanes.push_back(wpl1); wplanes.push_back(wpl2);

  std::vector<double> ws;
  ws.push_back(1.0);ws.push_back(1.0);ws.push_back(1.0);
  double residual=0.0;
  vgl_infinite_line_3d<double> weightedline;
  bool intersect = vgl_intersection(wplanes,ws,weightedline,residual);
  vgl_point_3d<double> wp0(weightedline.x0().x(), weightedline.x0().y(),0);
  is_on =is_on && intersect && weightedline.contains(wp0);
  TEST("intersection of multiple planes -> line", is_on, true );
}

static void test_lines_intersection()
{
  bool is_intersection;
  vgl_point_2d<double> point_intersection(0,0);
  is_intersection = vgl_intersection( vgl_line_2d<double>( vgl_point_2d<double>(1,1),
                                                           vgl_vector_2d<double>(0.5, std::sqrt(3.0)/2)),
                                      vgl_line_2d<double>( -std::sqrt(2.0), std::sqrt(2.0), 0 ),
                                      point_intersection );
  double intersection_error;
  intersection_error =  (point_intersection.x() - 1.0) * (point_intersection.x() - 1.0);
  intersection_error += (point_intersection.y() - 1.0) * (point_intersection.y() - 1.0);
  TEST("lines intersection exists", is_intersection, true );
  TEST_NEAR("lines intersection error", intersection_error, 0.0, 1e-12);

  is_intersection = vgl_intersection( vgl_line_2d<double>( 1, 0, -1 ),
                                      vgl_line_2d<double>( 1, 0, -2 ),
                                      point_intersection );
  TEST("lines intersection does not exist", is_intersection, false );
}

static void test_lines_intersect_in_tol()
{
  std::cout<<"Testing intersection of two line segments with tolerance.\n";

  // intersection: should return true
  vgl_point_2d<double> p1(0,0), p2(2,2), q1(2,1), q2(1,2);
  TEST("Line Intersection 1a", vgl_intersection(p1,p2,q1,q2), true);
  TEST("Line Intersection 1b", vgl_intersection(p2,p1,q1,q2), true);
  TEST("Line Intersection 1c", vgl_intersection(p1,p2,q2,q1), true);
  TEST("Line Intersection 1d", vgl_intersection(p2,p1,q2,q1), true);
  // q1 or q2 lies within tolerance of line (p1,p2)
  p1.set(0,0);
  p2.set(2,2);
  q1.set(1,1 + 1e-7);
  q2.set(0,2);
  TEST("Line Intersection 2a", vgl_intersection(p1,p2,q1,q2), true);
  TEST("Line Intersection 2b", vgl_intersection(p2,p1,q1,q2), true);
  TEST("Line Intersection 2c", vgl_intersection(p1,p2,q2,q1), true);
  TEST("Line Intersection 2d", vgl_intersection(p2,p1,q2,q1), true);
  // p1 or p2 lies within tolerance of line (q1,q2)
  p1.set(0,0);
  p2.set(2,2);
  q1.set(-1,-1+1e-7);
  q2.set(2.5,2.5+1e-7);
  TEST("Line Intersection 3a", vgl_intersection(p1,p2,q1,q2), true);
  TEST("Line Intersection 3b", vgl_intersection(p2,p1,q1,q2), true);
  TEST("Line Intersection 3c", vgl_intersection(p1,p2,q2,q1), true);
  TEST("Line Intersection 3d", vgl_intersection(p2,p1,q2,q1), true);
  // lines have 1 common point, should return false : no intersection
  p1.set(0,0);
  p2.set(2,2);
  q1.set(2,2);
  q2.set(4,4);
  TEST("Line Intersection 4a", vgl_intersection(p1,p2,q1,q2), true);
  TEST("Line Intersection 4b", vgl_intersection(p2,p1,q1,q2), true);
  TEST("Line Intersection 4c", vgl_intersection(p1,p2,q2,q1), true);
  TEST("Line Intersection 4d", vgl_intersection(p2,p1,q2,q1), true);
  // should return intersection
  p1.set(0,0);
  p2.set(2,2);
  q1.set(0,0);
  q2.set(4,4);
  TEST("Line Intersection 5a", vgl_intersection(p1,p2,q1,q2), true);
  TEST("Line Intersection 5b", vgl_intersection(p2,p1,q1,q2), true);
  TEST("Line Intersection 5c", vgl_intersection(p1,p2,q2,q1), true);
  TEST("Line Intersection 5d", vgl_intersection(p2,p1,q2,q1), true);
  // should return intersection
  p1.set(0,0);
  p2.set(2.25,2.25);
  q1.set(0,0);
  q2.set(2,2);
  TEST("Line Intersection 6a", vgl_intersection(p1,p2,q1,q2), true);
  TEST("Line Intersection 6b", vgl_intersection(p2,p1,q1,q2), true);
  TEST("Line Intersection 6c", vgl_intersection(p1,p2,q2,q1), true);
  TEST("Line Intersection 6d", vgl_intersection(p2,p1,q2,q1), true);
  //Test 3-d line intersection
  vgl_point_3d<double> pl00(0,0,0), pl01(1,1,1);
  vgl_point_3d<double> pl10(1,1,1), pl11(1,1,2), pint;
  vgl_ray_3d<double> r0(pl00, pl01);
  vgl_ray_3d<double> r1(pl10, pl11);
  TEST("3-d Ray Intersection", vgl_intersection(r0, r1, pint), true);
  TEST("3-d Ray Intersection Point", pint==pl01, true);
}

static void test_box_2d_intersection()
{
  std::cout<<"Testing intersection of 2d box and point.\n";
  vgl_point_2d<double> p0(0.3, 0.3), p1(0.7, 0.3), p2(0.6, 0.7), p3(0.7, 0.7), p4(0.8, 0.6);
  vgl_point_2d<double> bp0(0.6, 0.6), bp1(0.8, 0.8);
  vgl_box_2d<double> b1; b1.add(bp0); b1.add(bp1);
  //Case I: point inside box
  TEST("point inside box", vgl_intersection(b1, p3), true);
  //Case II: point outside box
  TEST("point outside box", vgl_intersection(b1, p0), false);
  TEST("point outside box", vgl_intersection(b1, p1), false);
  //Case III: point on boundary of box
  TEST("point on boundary of box", vgl_intersection(b1, p2), true);
  TEST("point on corner of box", vgl_intersection(b1, p4), true);

  std::cout<<"Testing intersection of two 2d boxes.\n";
  vgl_box_2d<double> b2;
  //Case I: one box inside other box
  b2.add(vgl_point_2d<double>(0.6,0.7)); b2.add(vgl_point_2d<double>(0.7,0.8));
  TEST("one box inside other box", vgl_intersection(b1, b2), b2);
  TEST("one box inside other box", vgl_intersection(b2, b1), b2);
  //Case II: no intersection
  b2.empty(); b2.add(vgl_point_2d<double>(0.3,0.3)); b2.add(vgl_point_2d<double>(0.5,0.5));
  TEST("no intersection", vgl_intersection(b1, b2), vgl_box_2d<double>());
  //Case III: corner point touching
  b2.empty(); b2.add(vgl_point_2d<double>(0.3,0.3)); b2.add(vgl_point_2d<double>(0.6,0.6));
  TEST("corner point touching", vgl_intersection(b1, b2), vgl_box_2d<double>(0.6,0.6,0.6,0.6));
}

static void test_box_3d_intersection()
{
  std::cout<<"Testing intersection of 3d box and point.\n";
  vgl_point_3d<int> p0(3,3,3), p1(7,3,5), p2(6,7,7), p3(7,7,7), p4(6,7,8), p5(6,8,8);
  vgl_point_3d<int> bp0(6,6,6), bp1(8,8,8);
  vgl_box_3d<int> b1; b1.add(bp0); b1.add(bp1);
  //Case I: point inside box
  TEST("point inside box", vgl_intersection(b1, p3), true);
  //Case II: point outside box
  TEST("point outside box", vgl_intersection(b1, p0), false);
  TEST("point outside box", vgl_intersection(b1, p1), false);
  //Case III: point on boundary of box
  TEST("point on face of box", vgl_intersection(b1, p2), true);
  TEST("point on edge of box", vgl_intersection(b1, p4), true);
  TEST("point on corner of box", vgl_intersection(b1, p5), true);

  std::cout<<"Testing intersection of two 3d boxes.\n";
  vgl_box_3d<int> b2;
  //Case I: one box inside other box
  b2.add(vgl_point_3d<int>(6,7,7)); b2.add(vgl_point_3d<int>(7,7,8));
  TEST("one box inside other box", vgl_intersection(b1, b2), b2);
  TEST("one box inside other box", vgl_intersection(b2, b1), b2);
  //Case II: no intersection
  b2.empty(); b2.add(vgl_point_3d<int>(3,3,3)); b2.add(vgl_point_3d<int>(5,5,5));
  TEST("no intersection", vgl_intersection(b1, b2), vgl_box_3d<int>());
  //Case III: corner point touching
  b2.empty(); b2.add(vgl_point_3d<int>(3,3,3)); b2.add(vgl_point_3d<int>(6,6,6));
  TEST("corner point touching", vgl_intersection(b1, b2), vgl_box_3d<int>(6,6,6,6,6,6));
  //unit cube in 3-d
  vgl_point_3d<double> pd0(0.0, 0.0, 0.0), pd1(1.0, 1.0, 1.0);
  vgl_box_3d<double> bd0;
  bd0.add(pd0); bd0.add(pd1);
  //x-y plane at z=2
  vgl_plane_3d<double> plane2(0.0, 0.0, 1.0, -2.0);
  TEST("x-y plane intersect box z=2", vgl_intersection<double>(bd0, plane2), false);
  //x-y plane at z=0.5
  vgl_plane_3d<double> plane_5(0.0, 0.0, 1.0, -0.5);
  TEST("x-y plane intersect box z=0.5", vgl_intersection<double>(bd0, plane_5), true);
  //Test intersection of infinite line with box_3d.
  vgl_infinite_line_3d<double> l3da(vgl_point_3d<double>(-1, 0.5, 0.5),
                                    vgl_vector_3d<double>(1, 0, 0));
  vgl_point_3d<double> ip0, ip1;
  bool good = vgl_intersection<double>(bd0, l3da, ip0, ip1);
  TEST("box_inf_line_xdir", good, true);
  if (good) {
    TEST_NEAR("box_inf_line_xdir points", ip0.x()+ip1.x(), 1.0, 1.0e-7);
  }
  vgl_infinite_line_3d<double> l3db(vgl_point_3d<double>(0.5, -1.0, 0.5),
                                    vgl_vector_3d<double>(0, 1.0, 0));
  good = vgl_intersection<double>(bd0, l3db, ip0, ip1);
  TEST("box_inf_line_ydir", good, true);
  if (good) {
    TEST_NEAR("box_inf_line_ydir points", ip0.y()+ip1.y(), 1.0, 1.0e-7);
  }
  vgl_infinite_line_3d<double> l3dc(vgl_point_3d<double>(0.5, 0.5, -1.0),
                                    vgl_vector_3d<double>(0, 0, 1.0));
  good = vgl_intersection<double>(bd0, l3dc, ip0, ip1);
  TEST("box_inf_line_zdir", good, true);
  if (good) {
    TEST_NEAR("box_inf_line_zdir points", ip0.z()+ip1.z(), 1.0, 1.0e-7);
  }

  //A more generic test case
  vgl_point_3d<double> pg0(-1, -1, -1), pg1(1, 1, 1);
  vgl_box_3d<double> bg0;
  bg0.add(pg0); bg0.add(pg1);
  vgl_infinite_line_3d<double> l3g(vgl_point_3d<double>(0.0, 0.0, 0.0),
                                   vgl_vector_3d<double>(0.5773502691896257,
                                                         0.5773502691896257,
                                                         0.5773502691896257));
  good = vgl_intersection<double>(bg0, l3g, ip0, ip1);
  TEST("generic line intersect box", good, true);
  if (good) {
    TEST_NEAR("generic line intersection points",length(ip0-ip1),3.464101615137,1.0e-6);
  }
  //Test intersection of ray with box_3d.
  vgl_point_3d<double> pr0(-1,-1,-1), pr1(2,2,2);
  vgl_ray_3d<double> ry(pr0, pr1);
  good = vgl_intersection<double>(bd0, ry, ip0, ip1);
  TEST("ray intersect box from outside", good, true);
  vgl_point_3d<double> pr2(0.5, 0.5, 0.5);
  vgl_ray_3d<double> ry1(pr2, pr1);
  good = vgl_intersection<double>(bd0, ry1, ip0, ip1)
       && ip0==ip1;
  TEST("ray intersect box from inside", good, true);
}

static void test_box_poly_intersection()
{
  std::cout<<"Testing intersection of box and polygon.\n";
  //test polygon probe
  //a rectangle at 45 degrees (oriented box)
  vgl_point_2d<float> pr0(0.3f, 0.7f), pr1(0.7f, 0.3f), pr2(0.5f, 0.9f),
    pr3(0.9f, 0.5f);
  vgl_polygon<float> poly(1);
  poly.push_back(pr0); poly.push_back(pr1);
  poly.push_back(pr2); poly.push_back(pr3);
  //Case I: box entirely inside polygon
  vgl_point_2d<float> bp0(0.6f, 0.6f), bp1(0.65f, 0.65f);
  vgl_box_2d<float> b1;
  b1.add(bp0); b1.add(bp1);
  TEST("box entirely inside polygon", vgl_intersection(b1, poly), true);
  //Case II: poly entirely inside box
  vgl_point_2d<float> bp2(0.0f, 0.0f), bp3(1.0f, 1.0f);
  vgl_box_2d<float> b2;
  b2.add(bp2); b2.add(bp3);
  TEST("poly entirely inside box", vgl_intersection(b2, poly), true);
  //Case III: poly and box touch at a vertex
  vgl_point_2d<float> bp4(0.5f, 0.5f);
  vgl_box_2d<float> b3;
  b3.add(bp2); b3.add(bp4);
  TEST("poly and box touch at a vertex", vgl_intersection(b3, poly), true);
  //Case IV: poly vertex and box touch on a box edge
  vgl_point_2d<float> bp5(0.5f, 0.1f), bp6(0.9f, 0.3f);
  vgl_box_2d<float> b4;
  b4.add(bp5); b4.add(bp6);
  TEST("poly vertex and box touch on a box edge", vgl_intersection(b4, poly), true);
  //Case V: only poly edges intersect box
  vgl_box_2d<float> b5;
  b5.add(bp4); b5.add(bp3);
  vgl_point_2d<float> pr4(0.6f, 0.1f), pr5(0.7f, 0.1f), pr6(0.65f, 0.9f);
  vgl_polygon<float> poly2(1);
  poly2.push_back(pr4);   poly2.push_back(pr5);   poly2.push_back(pr6);
  TEST("only poly edges intersect box", vgl_intersection(b5, poly2), true);
  //Case VI: no intersection
  vgl_box_2d<float> b6;
  b6.add(pr4); b6.add(bp2);
  TEST("no intersection", vgl_intersection(b6, poly), false);
  //Case VII: box with a single point
  vgl_point_2d<float> ps(1.0f,0.0f);
  vgl_box_2d<float> b7;
  b7.add(ps);
  TEST("box with a single point", vgl_intersection(b7, poly), false);
}

static void test_poly_line_intersection()
{
  std::cout << "Testing polygon - line intersection\n";
  std::vector<vgl_point_2d<double> > sh0, sh1;
  vgl_point_2d<double> p00(-10.0, -10.0), p01(10.0,-10.0);
  vgl_point_2d<double> p02(10.0, 10.0), p03(-10.0, 10.0);
  sh0.push_back(p00);   sh0.push_back(p01);
  sh0.push_back(p02);   sh0.push_back(p03);
  vgl_point_2d<double> p10(-1.0, -1.0), p11(-1.0,1.0);
  vgl_point_2d<double> p12(1.0, 1.0), p13(1.0, -1.0);
  sh1.push_back(p10);   sh1.push_back(p11);
  sh1.push_back(p12);   sh1.push_back(p13);
  vgl_polygon<double> poly;
  poly.push_back(sh0);   poly.push_back(sh1);
  // test line with all line-edge intersections
  vgl_point_2d<double> p0(-20.0, 0.0), p1(20.0, 0.0);
  vgl_line_2d<double> line_a(p0, p1);
  std::vector<vgl_point_2d<double> > inters =
    vgl_intersection<double>(poly, line_a);
  TEST("number of intersections, interior case", inters.size(), 4);
  double corrs=0.0;
  for (auto & inter : inters) {
    corrs+= inter.x();
    corrs+= inter.y();
  }
  TEST_NEAR("intersection locations", corrs, 0.0, 0.001);
  //check grazing intersection
  vgl_point_2d<double> pg0(-20.0, 1.0), pg1(20.0, 1.0);
  vgl_line_2d<double> line_g(pg0, pg1);
  std::vector<vgl_point_2d<double> > ginter =
    vgl_intersection<double>(line_g, poly);
  TEST("number of intersections, vertex case", ginter.size(), 4);
  double t = -ginter[2].x() + ginter[3].x()+ginter[2].y()+ginter[3].y();
  TEST_NEAR("vertex intersection", t, 4.0, 0.001);
}

void test_intersection()
{
  std::cout << "**************************\n"
           << " Testing vgl_intersection\n"
           << "**************************\n\n";
  test_plane_intersection();
  test_three_planes();
  test_plane_plane();
  test_multiple_planes();
  test_lines_intersection();
  test_lines_intersect_in_tol();
  test_box_2d_intersection();
  test_box_3d_intersection();
  test_box_poly_intersection();
  test_poly_line_intersection();
}

TESTMAIN(test_intersection);
