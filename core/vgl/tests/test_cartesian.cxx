// \file
// \author Peter.Vanroose@esat.kuleuven.ac.be
// \date  4 July, 2001

#include <vgl/vgl_test.h>

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_line_segment_2d.h>
#include <vgl/vgl_line_segment_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_vector_2d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_clip.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#include <vgl/vgl_region_scan_iterator.h>
#include <vgl/vgl_triangle_scan_iterator.h>
#include <vgl/vgl_window_scan_iterator.h>
#include <vgl/vgl_lineseg_test.h>
#include <vgl/vgl_polygon_test.h>
#include <vgl/vgl_triangle_test.h>

static void test_point_2d()
{
  int d[] = {5,5};
  vgl_point_2d<int> p1(3,7), p2(d), p3(-1,-8);
  vcl_cout << p3;

  TEST("inequality", p1 != p3, true);

  p3.set(3,7);
  TEST("equality", p1 == p3, true);

  vgl_vector_2d<int> d1 = p1 - p2;
  TEST("sum, difference", p2+d1, p1);

  p2 += d1;
  TEST("+=", p2, p1);
  
  p2.set(4,5);
  p3.set(7,-1);
  TEST("collinear", collinear(p1,p2,p3), true);
  TEST("ratio", collinear(p1,p2,p3), 4.0);
  TEST("midpoint", midpoint(p1,p2,4), p3;

  TEST("centre", centre(p1,p3), vgl_point_2d<int>(5,3));

  vgl_line_2d<double> l1(1,0,0), l2(0,1,0);
  vgl_point_2d<double> pi(l1,l2); // intersection
  TEST("intersection", pi, vgl_point_2d<double>(0,0));
}

static void test_point_3d()
{
}

static void test_homg_point_2d()
{
}

static void test_homg_point_3d()
{
}

static void test_vector_2d()
{
}

static void test_vector_3d()
{
}

void test_cartesian() {
  test_point_2d();
  test_point_3d();
  test_homg_point_2d();
  test_homg_point_3d();
  test_vector_2d();
  test_vector_3d();
}

TESTMAIN(test_cartesian);
