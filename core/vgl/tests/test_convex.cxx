// Some tests for vgl_convex
// Ian Scott, Feb 2004.
#include <testlib/testlib_test.h>
#include <vgl/vgl_convex.h>
#include <vcl_iostream.h>

static void test_convex()
{
  vcl_cout << "Test 2d convex hull\n";

  vcl_cout << "Inputs\n";
  // Simple triangle
  vcl_vector<vgl_point_2d<double> > pts;
  pts.push_back( vgl_point_2d<double>(0.0, 0.0) );
  pts.push_back( vgl_point_2d<double>(0.0, 0.0) );
  pts.push_back( vgl_point_2d<double>(5.0, 0.0) );
  pts.push_back( vgl_point_2d<double>(3.0, 1.0) );
  pts.push_back( vgl_point_2d<double>(2.0, 1.0) );
  pts.push_back( vgl_point_2d<double>(0.0, 5.0) );

  for (unsigned i=0; i < pts.size(); ++i)
    vcl_cout << '(' << pts[i].x() <<','<<pts[i].y()<<") ";
  vcl_cout << vcl_endl;

  vgl_polygon<double> poly=vgl_convex_hull(pts);
  vcl_cout << "Outputs\n";
  poly.print(vcl_cout);
  vcl_cout << vcl_endl;

  TEST("inside", poly.contains( pts[0] ), true );
  TEST("inside", poly.contains( pts[1] ), true );
  TEST("inside", poly.contains( pts[3] ), true );
  TEST("inside", poly.contains( pts[4] ), true );
}



TESTMAIN(test_convex);
