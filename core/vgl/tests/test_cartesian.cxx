// \file
// \author Peter.Vanroose@esat.kuleuven.ac.be
// \date  4 July, 2001

#include <testlib/testlib_test.h>
#define APPROX(a,b,c) TEST(a,((b)-(c)<1e-6 && (c)-(b)<1e-6),true)
#include <vcl_iostream.h>

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
#include <vgl/vgl_1d_basis.h>

static void test_vector_2d()
{
  // standard constructor
  vgl_vector_2d<float> const v(1.5f, 0.625f);
  vcl_cout << v << vcl_endl;

  // default constructor
  vgl_vector_2d<float> v0; // == (0,0)

  // comparison
  TEST("inequality", (v != v0), true);
  // length
  TEST("length", v0.length(), 0.0);

  v0.set(1.5, 0.625);
  TEST("equality", (v == v0), true);
  v0 = v; // assignment
  TEST("equality", (v == v0), true);

  TEST("length", v.length(), 1.625);
  // should be "exact" ! (all these numbers are exactly representable in base 2)

  v0.set(1.5, 0);
  TEST("inequality", (v != v0), true);
  vgl_vector_2d<float> v1 (3,0.625);
  TEST("sum", (v+v0), v1);
  TEST("difference", (v1-v0), v);
  TEST("scale", (-v1+2*v0).x(), 0.0f);
  TEST("scale", (-v1+2*v0).y(), -0.625f);

  TEST("dot_product", dot_product(v,v1), 4.890625);
  TEST("cross_product", cross_product(v1,v), 0.625*1.5);
  APPROX("angle", angle(v1,v), 0.18939573); // 10^51'06"

  TEST("parallel", (parallel(v,v)), true);
  TEST("parallel", (parallel(v,vgl_vector_2d<float>())), true); // parallel to (0,0)
  TEST("parallel", (parallel(v,v1,0.1)), false); // not parallel, even with tol=0.1

  TEST("ratio", v/v, 1);
  TEST("ratio", (-v*3.5)/v, -3.5);

  APPROX("ratio", v1/v, 1.852071);

  APPROX("normalized", length(length(v1)*normalized(v1) -v1), 0.0);
  v0=v1;
  normalize(v1);
  APPROX("normalize", length(length(v0)*v1 - v0), 0.0);

  TEST("orthogonal", orthogonal(v,vgl_vector_2d<float>()), true); // orthogonal to (0,0)
  TEST("!orthogonal", orthogonal(v,v1,0.1), false); // even not with tolorance
  TEST("orthogonal", orthogonal(v,vgl_vector_2d<float>(0.625,-1.5)), true);
}

static void test_vector_3d()
{
  // standard constructor
  vgl_vector_3d<float> v(1.5f, 0.625f, 0.0f);
  vcl_cout << v << vcl_endl;

  // default constructor
  vgl_vector_3d<float> v0; // == (0,0,0)

  // comparison
  TEST("inequality", (v != v0), true);
  // length
  TEST("length", v0.length(), 0.0);

  v0.set(1.5, 0.625, 0);
  TEST("equality", (v == v0), true);
  v0 = v; // assignment
  TEST("equality", (v == v0), true);

  TEST("length", v.length(), 1.625);
  // should be "exact" ! (all these numbers are exactly representable in base 2)

  v0.set(1.5, 0, 0);
  TEST("inequality", (v != v0), true);
  vgl_vector_3d<float> v1 (3,0.625, 0);
  TEST("sum", (v+v0), v1);
  TEST("difference", (v1-v0), v);
  TEST("scale", (-v1+2*v0).x(), 0.0f);
  TEST("scale", (-v1+2*v0).y(), -0.625f);
  TEST("scale", (-v1+2*v0).z(), 0);

  TEST("dot_product", dot_product(v,v1), 4.890625);
  TEST("cross_product", cross_product(v1,v).z(), 0.625*1.5);
  APPROX("angle", angle(v1,v), 0.18939573); // 10^51'06"

  TEST("parallel", (parallel(v,v)), true);
  TEST("parallel", (parallel(v,vgl_vector_3d<float>())), true); // parallel to (0,0)
  TEST("parallel", (parallel(v,v1,0.1)), false); // not parallel, even with tol=0.1

  TEST("ratio", v/v, 1);
  TEST("ratio", (-v*3.5)/v, -3.5);
  APPROX("ratio", v1/v, 1.852071);

  TEST("normalized", length(v1)*normalized(v1), v1);
  v0=v1;
  normalize(v1);
  TEST("normalize", length(v0)*v1, v0);

  TEST("orthogonal", orthogonal(v,vgl_vector_3d<float>()), true); // orthogonal to (0,0)
  TEST("!orthogonal", orthogonal(v,v1,0.1), false); // even not with tolorance
  TEST("orthogonal", orthogonal(v,cross_product(v1,v)), true);
  TEST("orthogonal", orthogonal(v1,cross_product(v1,v)), true);
}

static void test_point_2d()
{
  int d[] = {5,5};
  vgl_point_2d<int> p1(3,7), p2(d), p3(-1,-8);
  vcl_cout << p3 << vcl_endl;

  TEST("constructor", p1.x() == 3 && p1.y()==7, true);

  TEST("inequality", (p1 != p3), true);

  p3.set(3,7);
  TEST("set", p3.x() == 3 && p3.y()==7, true);

  TEST("equality", (p1 == p3), true);

  vgl_vector_2d<int> d1 = p1 - p2;
  TEST("sum; difference", (p2+d1), p1);

  TEST("+=", (p2+=d1), p1);
  TEST("+=", p2, p1);

  p2.set(4,5);
  p3.set(7,-1);
  bool b = collinear(p1,p2,p3);
  TEST("collinear", b, true);
  double r = ratio(p1,p2,p3);
  TEST("ratio", r, 4.0);
  vgl_point_2d<int> m = midpoint(p1,p2,4);
  TEST("midpoint", m, p3);

  vgl_point_2d<int> c = centre(p1,p3);
  vgl_point_2d<int> cc(5,3);
  TEST("centre", c, cc);
  c = centre(p1,p2,c); // assignment
  TEST("centre", c, p2);
  c = centre(p2,p3,cc,p2);
  TEST("centre", c, cc);

  r = cross_ratio(p1,p2,c,p3);
  TEST("cross_ratio", r, 1.5);

  vgl_line_2d<double> l1(1,0,0), l2(0,1,0);
  vgl_point_2d<double> pi(l1,l2); // intersection
  vgl_point_2d<double> pp(0,0);
  TEST("intersection", pi, pp);
}

static void test_point_3d()
{
  int d[] = {5,5,5};
  vgl_point_3d<int> p1(3,7,-1), p2(d), p3(-1,-8,7);
  vcl_cout << p3 << vcl_endl;

  TEST("inequality", (p1 != p3), true);

  p3.set(3,7,-1);
  TEST("equality", (p1 == p3), true);

  vgl_vector_3d<int> d1 = p1 - p2;
  TEST("sum; difference", (p2+d1), p1);

  TEST("+=", (p2+=d1), p1);
  TEST("+=", p2, p1);

  p2.set(4,5,2);
  p3.set(7,-1,11);
  bool b = collinear(p1,p2,p3);
  TEST("collinear", b, true);
  double r = ratio(p1,p2,p3);
  TEST("ratio", r, 4.0);
  vgl_point_3d<int> m = midpoint(p1,p2,4);
  TEST("midpoint", m, p3);

  vgl_point_3d<int> c = centre(p1,p3);
  vgl_point_3d<int> cc(5,3,5);
  TEST("centre", c, cc);
  c = centre(p1,p2,c); // assignment
  TEST("centre", c, p2);
  c = centre(p2,p3,cc,p2);
  TEST("centre", c, cc);

  r = cross_ratio(p1,p2,c,p3);
  TEST("cross_ratio", r, 1.5);

  vgl_plane_3d<double> pl1(1,0,0,0), pl2(0,1,0,0), pl3(0,0,1,0);
  vgl_point_3d<double> pi(pl1,pl2,pl3); // intersection
  vgl_point_3d<double> pp(0,0,0);
  TEST("intersection", pi, pp);
}

static void test_line_2d()
{
  double d[] = {5,5,-1};
  vgl_line_2d<double> l1(3,7,0), l2(d), l3(0,-1,-8);
  vcl_cout << l3 << vcl_endl;

  TEST("inequality", (l1 != l3), true);

  l3.set(3,7,0);
  TEST("equality", (l1 == l3), true);

  l2.set(4,5,0);
  l3.set(7,-1,0);
  bool b = concurrent(l1,l2,l3); // because they share the point (0,0)
  TEST("concurrent", b, true);

  vgl_point_2d<double> p1(1,0), p2(0,1);
  vgl_line_2d<double> li(p1,p2); // line through these two points
  vcl_cout << li << vcl_endl;
  vgl_line_2d<double> ll(1,1,-1);
  TEST("join", li, ll);
}

static void test_plane_3d()
{
  double d[] = {0,3,4,1};
  vgl_plane_3d<double> pl1(3,7,-1,1), pl2(d), pl3(-1,-8,7,1);
  vcl_cout << pl3 << vcl_endl;

  TEST("inequality", (pl1 != pl3), true);

  pl3.set(3,7,-1,1);
  TEST("equality", (pl1 == pl3), true);

  vgl_vector_3d<double> d1 = pl2.normal();
  vgl_vector_3d<double> d2 = vgl_vector_3d<double>(0,0.6,0.8);
  vcl_cout << d1 << vcl_endl;
  TEST("normal", d1, d2);

  vgl_point_3d<double> p1(1,0,0), p2(0,1,0), p3(0,0,1);
  vgl_plane_3d<double> pl(p1,p2,p3); // plane through 3 points
  vcl_cout << pl << vcl_endl;
  vgl_plane_3d<double> pp(1,1,1,-1);
  TEST("join", pl, pp);
}

static void test_box_2d()
{
  // Create empty box
  vgl_box_2d<double> b;
  vcl_cout << b << vcl_endl;
  TEST("box is empty", b.is_empty(), true);
  TEST("box has no volume", b.volume(), 0.0);

  vgl_point_2d<double> p1(0,0), p2(0,1), p3(1,0);

  TEST("!contains", b.contains(p1), false);
  b.add(p1);
  vcl_cout << b << vcl_endl;
  TEST("contains", b.contains(p1), true);
  TEST("box is not empty", b.is_empty(), false);
  TEST("box has no volume", b.volume(), 0.0);
  TEST("centroid", b.centroid(), p1);

  TEST("!contains", b.contains(p2), false);
  b.add(p2);
  vcl_cout << b << vcl_endl;
  TEST("contains", b.contains(p1), true);
  TEST("contains", b.contains(p2), true);
  TEST("box is not empty", b.is_empty(), false);
  TEST("box has no volume", b.volume(), 0.0);
  TEST("centroid", b.centroid(), p1 + 0.5*(p2-p1));

  TEST("!contains", b.contains(p3), false);
  b.add(p3);
  vcl_cout << b << vcl_endl;
  TEST("contains", b.contains(p1), true);
  TEST("contains", b.contains(p2), true);
  TEST("contains", b.contains(p3), true);
  TEST("box is not empty", b.is_empty(), false);
  TEST("box has volume 1", b.volume(), 1.0);
  TEST("centroid", b.centroid(), p2 + 0.5*(p3-p2));

  b.empty();
  vcl_cout << b << vcl_endl;
  TEST("!contains", b.contains(p1), false);
  TEST("!contains", b.contains(p2), false);
  TEST("!contains", b.contains(p3), false);
  TEST("box is empty", b.is_empty(), true);
  TEST("box has no volume", b.volume(), 0.0);
}

static void test_box_3d()
{
  // Create empty box
  vgl_box_3d<double> b;
  vcl_cout << b << vcl_endl;
  TEST("box is empty", b.is_empty(), true);
  TEST("box has no volume", b.volume(), 0.0);

  vgl_point_3d<double> p0(0,0,0), p1(1,0,0), p2(0,1,0), p3(0,0,1);

  TEST("!contains", b.contains(p0), false);
  b.add(p0);
  vcl_cout << b << vcl_endl;
  TEST("contains", b.contains(p0), true);
  TEST("box is not empty", b.is_empty(), false);
  TEST("box has no volume", b.volume(), 0.0);
  TEST("centroid", b.centroid(), p0);

  TEST("!contains", b.contains(p1), false);
  b.add(p1);
  vcl_cout << b << vcl_endl;
  TEST("contains", b.contains(p0), true);
  TEST("contains", b.contains(p1), true);
  TEST("box is not empty", b.is_empty(), false);
  TEST("box has no volume", b.volume(), 0.0);
  TEST("centroid", b.centroid(), p0 + 0.5*(p1-p0));

  TEST("!contains", b.contains(p2), false);
  b.add(p2);
  vcl_cout << b << vcl_endl;
  TEST("contains", b.contains(p0), true);
  TEST("contains", b.contains(p1), true);
  TEST("contains", b.contains(p2), true);
  TEST("box is not empty", b.is_empty(), false);
  TEST("box has no volume", b.volume(), 0.0);
  TEST("centroid", b.centroid(), p1 + 0.5*(p2-p1));

  TEST("!contains", b.contains(p3), false);
  b.add(p3);
  vcl_cout << b << vcl_endl;
  TEST("contains", b.contains(p0), true);
  TEST("contains", b.contains(p1), true);
  TEST("contains", b.contains(p2), true);
  TEST("contains", b.contains(p3), true);
  TEST("box is not empty", b.is_empty(), false);
  TEST("box has volume 1", b.volume(), 1.0);
  TEST("centroid", b.centroid(), p1 + 0.5*(p2-p1) + 0.5*(p3-p0));

  b.empty();
  vcl_cout << b << vcl_endl;
  TEST("!contains", b.contains(p0), false);
  TEST("!contains", b.contains(p1), false);
  TEST("!contains", b.contains(p2), false);
  TEST("!contains", b.contains(p3), false);
  TEST("box is empty", b.is_empty(), true);
  TEST("box has no volume", b.volume(), 0.0);
}

MAIN( test_cartesian )
{
  START( "test cartesian" );
  vcl_cout << "--- test_vector_2d ---\n";
  test_vector_2d();
  vcl_cout << "--- test_vector_3d ---\n";
  test_vector_3d();
  vcl_cout << "--- test_point_2d ---\n";
  test_point_2d();
  vcl_cout << "--- test_point_3d ---\n";
  test_point_3d();
  vcl_cout << "--- test_line_2d ---\n";
  test_line_2d();
  vcl_cout << "--- test_plane_3d ---\n";
  test_plane_3d();
  vcl_cout << "--- test_box_2d ---\n";
  test_box_2d();
  vcl_cout << "--- test_box_3d ---\n";
  test_box_3d();
  SUMMARY();
}
