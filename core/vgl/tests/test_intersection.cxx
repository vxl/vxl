// Some tests for vgl_intersection
// Gamze Tunali, Jan 2007.
#include <vcl_iostream.h>
#include <vcl_limits.h>
#include <testlib/testlib_test.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_intersection.h>
#include <vgl/vgl_line_3d_2_points.h>


static void test_plane_intersection()
{
  vgl_plane_3d<double> pl1(vgl_vector_3d<double>(10,10,10), vgl_point_3d<double>(10,0,-10));
  vgl_line_3d_2_points<double> l1(vgl_point_3d<double>(1,4,1),vgl_point_3d<double>(-1,-4,-1));
  vgl_point_3d<double> pt1 = vgl_intersection(l1,pl1);
  TEST_NEAR("vgl_intersection(l1,pl1) = O", vgl_distance(pt1, vgl_point_3d<double>(0,0,0)), 0.0, 1e-8);

  vgl_line_3d_2_points<double> l2(vgl_point_3d<double>(0,0,0),vgl_point_3d<double>(10,0,-10));
  vgl_point_3d<double> pt2 = vgl_intersection(l2,pl1);

  const double inf = vcl_numeric_limits<double>::infinity();
  TEST("vgl_intersection(l2,pl1) = (inf,inf,inf)", pt2,
       vgl_point_3d<double>(inf, inf, inf));

  vgl_line_3d_2_points<double> l3(vgl_point_3d<double>(0,10,0),vgl_point_3d<double>(10,10,-10));
  vgl_point_3d<double> pt3 = vgl_intersection(l3,pl1);
  TEST("vgl_intersection(l2,p1) = (inf,0,0)", pt3, vgl_point_3d<double>(inf, 0, 0));
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

static void test_lines_intersection()
{
  bool is_intersection;
  vgl_point_2d<double> point_intersection(0,0);
  is_intersection = vgl_intersection( vgl_line_2d<double>( vgl_point_2d<double>(1,1),
                                                           vgl_vector_2d<double>(0.5, vcl_sqrt(3.0)/2)),
                                      vgl_line_2d<double>( -vcl_sqrt(2.0), vcl_sqrt(2.0), 0 ),
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
  vcl_cout<<"Testing intersection of two line segments with tolerance."<<vcl_endl;

  // intersection: should return true, 
  vgl_point_2d<double> p1(0,0), p2(2,2), q1(2,1), q2(1,2);
  TEST("Line Intersection 1a", vgl_intersection(p1,p2,q1,q2),true);
  TEST("Line Intersection 1b", vgl_intersection(p2,p1,q1,q2),true);
  TEST("Line Intersection 1c", vgl_intersection(p1,p2,q2,q1),true);
  TEST("Line Intersection 1d", vgl_intersection(p2,p1,q2,q1),true);
  // q1 or q2 lies within tolerance of line (p1,p2)
  p1.set(0,0);
  p2.set(2,2);
  q1.set(1,1 + 1e-7);
  q2.set(0,2);
  TEST("Line Intersection 2a", vgl_intersection(p1,p2,q1,q2),true);
  TEST("Line Intersection 2b", vgl_intersection(p2,p1,q1,q2),true);
  TEST("Line Intersection 2c", vgl_intersection(p1,p2,q2,q1),true);
  TEST("Line Intersection 2d", vgl_intersection(p2,p1,q2,q1),true);
  // p1 or p2 lies within tolerance of line (q1,q2)
  p1.set(0,0);
  p2.set(2,2);
  q1.set(-1,-1+1e-7);
  q2.set(2.5,2.5+1e-7);
  TEST("Line Intersection 3a", vgl_intersection(p1,p2,q1,q2),true);
  TEST("Line Intersection 3b", vgl_intersection(p2,p1,q1,q2),true);
  TEST("Line Intersection 3c", vgl_intersection(p1,p2,q2,q1),true);
  TEST("Line Intersection 3d", vgl_intersection(p2,p1,q2,q1),true);
  // lines have 1 common point, should return false : no intersection
  p1.set(0,0);
  p2.set(2,2);
  q1.set(2,2);
  q2.set(4,4);
  TEST("Line Intersection 4a", vgl_intersection(p1,p2,q1,q2),true);
  TEST("Line Intersection 4b", vgl_intersection(p2,p1,q1,q2),true);
  TEST("Line Intersection 4c", vgl_intersection(p1,p2,q2,q1),true);
  TEST("Line Intersection 4d", vgl_intersection(p2,p1,q2,q1),true);
  // should return intersection
  p1.set(0,0);
  p2.set(2,2);
  q1.set(0,0);
  q2.set(4,4);
  TEST("Line Intersection 5a", vgl_intersection(p1,p2,q1,q2),true);
  TEST("Line Intersection 5b", vgl_intersection(p2,p1,q1,q2),true);
  TEST("Line Intersection 5c", vgl_intersection(p1,p2,q2,q1),true);
  TEST("Line Intersection 5d", vgl_intersection(p2,p1,q2,q1),true);
  // should return intersection
  p1.set(0,0);
  p2.set(2.25,2.25);
  q1.set(0,0);
  q2.set(2,2);
  TEST("Line Intersection 6a", vgl_intersection(p1,p2,q1,q2),true);
  TEST("Line Intersection 6b", vgl_intersection(p2,p1,q1,q2),true);
  TEST("Line Intersection 6c", vgl_intersection(p1,p2,q2,q1),true);
  TEST("Line Intersection 6d", vgl_intersection(p2,p1,q2,q1),true);
  
}

static void test_box_poly_intersection()
{ 
  vcl_cout<<"Testing intersection of box and polygon."<<vcl_endl;
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
  bool good = vgl_intersection<float>(b1, poly);
  //Case II: poly entirely inside box
  vgl_point_2d<float> bp2(0.0f, 0.0f), bp3(1.0f, 1.0f);
  vgl_box_2d<float> b2;
  b2.add(bp2); b2.add(bp3);
  good = good && vgl_intersection<float>(b2, poly);
  //Case III: poly and box touch at a vertex
  vgl_point_2d<float> bp4(0.5f, 0.5f);
  vgl_box_2d<float> b3;
  b3.add(bp2); b3.add(bp4);
  good = good && vgl_intersection<float>(b3, poly);
  //Case IV: poly vertex and box touch on a box edge
  vgl_point_2d<float> bp5(0.5f, 0.1f), bp6(0.9f, 0.3f);
  vgl_box_2d<float> b4;
  b4.add(bp5); b4.add(bp6);
  good = good && vgl_intersection<float>(b4, poly);
  //Case V: only poly edges intersect box
  vgl_box_2d<float> b5;
  b5.add(bp4); b5.add(bp3);
  vgl_point_2d<float> pr4(0.6f, 0.1f), pr5(0.7f, 0.1f), pr6(0.65f, 0.9f);
  vgl_polygon<float> poly2(1);
  poly2.push_back(pr4);   poly2.push_back(pr5);   poly2.push_back(pr6);
  good = good && vgl_intersection<float>(b5, poly2);
  //Case VI: no intersection
  vgl_box_2d<float> b6;
  b6.add(pr4); b6.add(bp2);
  good = good && !vgl_intersection<float>(b6, poly);
  //Case VII: box with a single point
  vgl_point_2d<float> ps(1.0f,0.0f);
  vgl_box_2d<float> b7;
  b7.add(ps);
  good = good && !vgl_intersection<float>(b7, poly);
}
void test_intersection()
{
  vcl_cout << "**************************\n"
           << " Testing vgl_intersection\n"
           << "**************************\n\n";
  test_plane_intersection();
  test_three_planes();
  test_lines_intersection();
  test_lines_intersect_in_tol();
  test_box_poly_intersection();
}

TESTMAIN(test_intersection);
