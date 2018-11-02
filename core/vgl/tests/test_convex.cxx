// Some tests for vgl_convex
// Ian Scott, Feb 2004.
#include <iostream>
#include <testlib/testlib_test.h>
#include <vgl/vgl_convex.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

static void test_convex()
{
  std::cout << "Test 2d convex hull\n\n"
           << "Inputs\n";
  // Simple triangle
  std::vector<vgl_point_2d<double> > pts;
  pts.emplace_back(0.0, 0.0 );
  pts.emplace_back(0.0, 0.0 );
  pts.emplace_back(5.0, 0.0 );
  pts.emplace_back(3.0, 1.0 );
  pts.emplace_back(2.0, 1.0 );
  pts.emplace_back(0.0, 5.0 );

  for (auto & pt : pts)
    std::cout << '(' << pt.x() <<','<<pt.y()<<") ";
  std::cout << std::endl;

  vgl_polygon<double> poly=vgl_convex_hull(pts);
  std::cout << "Outputs\n";
  poly.print(std::cout);
  std::cout << std::endl;

  TEST("inside", poly.contains( pts[0] ), true );
  TEST("inside", poly.contains( pts[1] ), true );
  TEST("inside", poly.contains( pts[3] ), true );
  TEST("inside", poly.contains( pts[4] ), true );
}

TESTMAIN(test_convex);
