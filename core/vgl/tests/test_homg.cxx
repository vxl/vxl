// \file
// \author Peter.Vanroose@esat.kuleuven.ac.be
// \date  4 July, 2001

#include <testlib/testlib_test.h>
#include <vcl_iostream.h>

#include <vgl/vgl_homg_point_1d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/vgl_homg_line_3d_2_points.h>
#include <vgl/vgl_homg_plane_3d.h>
#include <vgl/vgl_1d_basis.h>
#include <vgl/algo/vgl_homg_operators_1d.h>

static void test_homg_point_1d()
{
  float d[] = {5,1};
  vgl_homg_point_1d<float> p1(6,3), p2(d), p3(-1,-8);
  vcl_cout << p3 << vcl_endl;

  TEST("inequality", (p1 != p3), true);

  p3.set(-12,-6);
  TEST("equality", (p1 == p3), true);

  float d1 = p1 - p2;
  TEST("sum; difference", (p2+d1), p1);

  TEST("+=", (p2+=d1), p1);
  TEST("+=", p2, p1);

  p2.set(4,1);
  p3.set(-13,-2);
  double r = ratio(p1,p2,p3);
  TEST("ratio", r, 2.25);
  vgl_homg_point_1d<float> m = midpoint(p1,p2,2.25f);
  TEST("midpoint", m, p3);

  vgl_homg_point_1d<float> c = centre(p1,p2);
  vgl_homg_point_1d<float> cc(3); // constructor with one argument
  TEST("centre", c, cc);
  vcl_vector<vgl_homg_point_1d<float> > v1;
  v1.push_back(p1); v1.push_back(p2); v1.push_back(c);
  cc = centre(v1); // assignment
  TEST("centre", c, cc);

  cc.set(1,0);
  r = cross_ratio(cc,p1,p2,p3); // must equal ratio(p1,p2,p3)
  TEST("cross_ratio", r, 2.25);

  r = vgl_homg_operators_1d<float>::cross_ratio(cc,p1,p2,p3);
  TEST("cross_ratio", r, 2.25);

  TEST("is_ideal", is_ideal(p2), false);
  p2.set(-6,0);
  TEST("ideal", p2.ideal(), true);
  p2.set(4,1);

  r = vgl_homg_operators_1d<float>::dot(p1,p2);
  vcl_cout << "p1 = " << p1 << ", p2 = " << p2 << '\n';
  vcl_cout << "dot(p1,p2) = " << r << '\n';
  TEST("dot", r, 27);

  r = vgl_homg_operators_1d<float>::cross(p1,p2);
  vcl_cout << "cross(p1,p2) = " << r << '\n';
  TEST("cross", r, -6);

  vgl_homg_1d<float> cj = vgl_homg_operators_1d<float>::conjugate(p1,p2,p3);
  vcl_cout << "conjugate(p1,p2,p3) = " << cj << '\n';
  TEST("conjugate", cj, vgl_homg_1d<float>(23.0f,7.0f));

  r = vgl_homg_operators_1d<float>::distance(p1,p2);
  vcl_cout << "distance(p1,p2) = " << r << '\n';
  TEST("distance", r, 2);
}

static void test_homg_point_2d()
{
  int d[] = {5,5,1};
  vgl_homg_point_2d<int> p1(3,7,1), p2(d), p3(-1,-8,1);
  vcl_cout << p3 << vcl_endl;

  TEST("inequality", (p1 != p3), true);

  p3.set(-6,-14,-2);
  TEST("equality", (p1 == p3), true);

  vgl_vector_2d<int> d1 = p1 - p2;
  TEST("sum; difference", (p2+d1), p1);

  TEST("+=", (p2+=d1), p1);
  TEST("+=", p2, p1);

  p2.set(4,5,1);
  p3.set(7,-1,1);
  bool b = collinear(p1,p2,p3);
  TEST("collinear", b, true);
  double r = ratio(p1,p2,p3);
  TEST("ratio", r, 4.0);
  vgl_homg_point_2d<int> m = midpoint(p1,p2,4);
  TEST("midpoint", m, p3);

  vgl_homg_point_2d<int> c = centre(p1,p3);
  vgl_homg_point_2d<int> cc(5,3,1);
  TEST("centre", c, cc);
  vcl_vector<vgl_homg_point_2d<int> > v1;
  v1.push_back(p1); v1.push_back(p2); v1.push_back(c);
  c = centre(v1); // assignment
  TEST("centre", c, p2);
  vcl_vector<vgl_homg_point_2d<int> > v2;
  v2.push_back(p2); v2.push_back(p3); v2.push_back(cc); v2.push_back(p2);
  c = centre(v2);
  TEST("centre", c, cc);

  r = cross_ratio(p1,p2,c,p3);
  TEST("cross_ratio", r, 1.5);

  vgl_homg_line_2d<double> l1(0,0,1), l2(0,1,0);
  vgl_homg_point_2d<double> pi(l1,l2); // intersection
  vgl_homg_point_2d<double> pp(1,0,0); // point at infinity
  TEST("intersection", pi, pp);
  TEST("ideal", pi.ideal(), true);
  TEST("is_ideal", is_ideal(p2), false);
}

static void test_homg_point_3d()
{
  int d[] = {5,5,5,1};
  vgl_homg_point_3d<int> p1(3,7,-1,1), p2(d), p3(-1,-8,7,1);
  vcl_cout << p3 << vcl_endl;

  TEST("inequality", (p1 != p3), true);

  p3.set(-6,-14,2,-2);
  TEST("equality", (p1 == p3), true);

  vgl_vector_3d<int> d1 = p1 - p2;
  TEST("sum; difference", (p2+d1), p1);

  TEST("+=", (p2+=d1), p1);
  TEST("+=", p2, p1);

  p2.set(4,5,2,1);
  p3.set(7,-1,11,1);
  bool b = collinear(p1,p2,p3);
  TEST("collinear", b, true);
  double r = ratio(p1,p2,p3);
  TEST("ratio", r, 4.0);
  vgl_homg_point_3d<int> m = midpoint(p1,p2,4);
  TEST("midpoint", m, p3);

  vgl_homg_point_3d<int> c = centre(p1,p3);
  vgl_homg_point_3d<int> cc(5,3,5,1);
  TEST("centre", c, cc);
  vcl_vector<vgl_homg_point_3d<int> > v1;
  v1.push_back(p1); v1.push_back(p2); v1.push_back(c);
  c = centre(v1); // assignment
  TEST("centre", c, p2);
  vcl_vector<vgl_homg_point_3d<int> > v2;
  v2.push_back(p2); v2.push_back(p3); v2.push_back(cc); v2.push_back(p2);
  c = centre(v2);
  TEST("centre", c, cc);

  r = cross_ratio(p1,p2,c,p3);
  TEST("cross_ratio", r, 1.5);

  vgl_homg_plane_3d<double> pl1(0,0,0,1), pl2(0,0,1,0), pl3(0,1,0,0);
  vgl_homg_point_3d<double> pi(pl1,pl2,pl3); // intersection
  vgl_homg_point_3d<double> pp(1,0,0,0); // point at infinity
  TEST("intersection", pi, pp);
  TEST("ideal", pi.ideal(), true);
  TEST("is_ideal", is_ideal(p2), false);
}

static void test_homg_line_2d()
{
  double d[] = {5,5,-1};
  vgl_homg_line_2d<double> l1(3,7,0), l2(d), l3(0,-1,-8);
  vcl_cout << l3 << vcl_endl;

  TEST("inequality", (l1 != l3), true);

  l3.set(3,7,0);
  TEST("equality", (l1 == l3), true);

  l2.set(4,5,0);
  l3.set(7,-1,0);
  bool b = concurrent(l1,l2,l3); // because they share the point (0,0)
  TEST("concurrent", b, true);

  vgl_homg_point_2d<double> p1(1,0), p2(0,1);
  vgl_homg_line_2d<double> li(p1,p2); // line through these two points
  vcl_cout << li << vcl_endl;
  vgl_homg_line_2d<double> ll(1,1,-1);
  TEST("join", li, ll);

  TEST("ideal", ll.ideal(), false);
  ll.set(0,0,-7);
  TEST("ideal", ll.ideal(), true);
}

static void test_homg_line_3d()
{
  vgl_homg_point_3d<double> p1(1,1,1,2), p2(1,1,1,0), p3(1,2,1,1);
  vgl_homg_line_3d_2_points<double> l1(p1,p2), l2(p2,p1), l3(p1,p3);
  vcl_cout << l1 << vcl_endl;

  TEST("inequality", (l1 != l3), true);
  TEST("equality", (l1 == l2), true);

  TEST("ideal", l1.point_finite().ideal(), false);
  TEST("ideal", l3.point_finite().ideal(), false);
  TEST("ideal", l1.point_infinite().ideal(), true);
  TEST("ideal", l3.point_infinite().ideal(), true);
}

static void test_homg_plane_3d()
{
  double d[] = {0,3,4,1};
  vgl_homg_plane_3d<double> pl1(3,7,-1,1), pl2(d), pl3(-1,-8,7,1);
  vcl_cout << pl3 << vcl_endl;

  TEST("inequality", (pl1 != pl3), true);

  pl3.set(3,7,-1,1);
  TEST("equality", (pl1 == pl3), true);

  vgl_vector_3d<double> d1 = pl2.normal();
  vgl_vector_3d<double> d2 = vgl_vector_3d<double>(0,0.6,0.8);
  vcl_cout << d1 << vcl_endl;
  TEST("normal", d1, d2);

  vgl_homg_point_3d<double> p1(1,0,0), p2(0,1,0), p3(0,0,1);
  vgl_homg_plane_3d<double> pl(p1,p2,p3); // homg_plane through 3 points
  vcl_cout << pl << vcl_endl;
  vgl_homg_plane_3d<double> pp(1,1,1,-1);
  TEST("join", pl, pp);

  TEST("ideal", pp.ideal(), false);
  pp.set(0,0,0,-7);
  TEST("ideal", pp.ideal(), true);
}

MAIN( test_homg )
{
  START( "Test homg" );

  test_homg_point_1d();
  test_homg_point_2d();
  test_homg_point_3d();
  test_homg_line_2d();
  test_homg_line_3d();
  test_homg_plane_3d();

  SUMMARY();
}
