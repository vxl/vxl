//:
// \file
// \author Peter Vanroose, ESAT, KULeuven.
// \date  4 July, 2001

#include <iostream>
#include <sstream>
#include "testlib/testlib_test.h"
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif

#include "vgl/vgl_homg_point_1d.h"
#include "vgl/vgl_homg_point_2d.h"
#include "vgl/vgl_homg_point_3d.h"
#include "vgl/vgl_homg_line_2d.h"
#include "vgl/vgl_homg_line_3d_2_points.h"
#include "vgl/vgl_homg_plane_3d.h"
#include "vgl/vgl_distance.h"
#include "vgl/vgl_1d_basis.h"
#include <vgl/algo/vgl_homg_operators_1d.h>
#include <vgl/algo/vgl_homg_operators_2d.h>
#include <vgl/algo/vgl_homg_operators_3d.h>
#include "vnl/vnl_math.h"
#include "vnl/vnl_vector_fixed.h"
#include "vnl/vnl_float_2x2.h"
#include "vnl/vnl_double_3x3.h"

static void test_homg_point_1d()
{
  float d[] = {5,1};
  vgl_homg_point_1d<float> p1(6,3), p2(d), p3(-1,-8);

  TEST("inequality", (p1 != p3), true);

  p3.set(-12,-6);
  TEST("equality", (p1 == p3), true);

  float d1 = p1 - p2;
  TEST("sum; difference", (p2+d1), p1);

  TEST("+=", (p2+=d1), p1);
  TEST("+=", p2, p1);

  float dd[] = { 1,5, -3,7};
  vnl_float_2x2 mm(dd);
  p2 = mm * p1; // homography
  TEST("matrix * point", p2, vgl_homg_point_1d<float>(7));

  p2.set(4,1);
  p3.set(-13,-2);
  double r = ratio(p1,p2,p3);
  TEST("ratio", r, 2.25);
  vgl_homg_point_1d<float> m = midpoint(p1,p2,2.25f);
  TEST("generalised midpoint", m, p3);
  m = vgl_homg_operators_1d<float>::midpoint(p1,p2);
  TEST("ordinary midpoint", m, vgl_homg_point_1d<float>(3.f));

  m = centre(p1,p2);
  vgl_homg_point_1d<float> cc(3); // constructor with one argument
  TEST("centre", m, cc);
  std::vector<vgl_homg_point_1d<float> > v1;
  v1.push_back(p1); v1.push_back(p2); v1.push_back(m);
  cc = centre(v1); // assignment
  TEST("centre", m, cc);

  cc.set(1,0);
  r = cross_ratio(cc,p1,p2,p3); // must equal ratio(p1,p2,p3)
  TEST("cross_ratio", r, 2.25);

  r = vgl_homg_operators_1d<float>::cross_ratio(cc,p1,p2,p3);
  TEST("cross_ratio", r, 2.25);

  TEST("is_ideal", is_ideal(p2), false);
  p2.set(-6,0);
  TEST("ideal", p2.ideal(), true);

  vgl_homg_operators_1d<float>::unitize(p2);
  TEST("unitize()", p2.x(), -1.f);

  vgl_homg_point_1d<double> u(-3,4);
  vgl_homg_operators_1d<double>::unitize(u);
  TEST_NEAR("unitize: x", u.x(), -0.6, 1e-12);
  TEST_NEAR("unitize: w", u.w(), 0.8, 1e-12);

  vnl_vector_fixed<double,2> v = vgl_homg_operators_1d<double>::get_vector(u);
  TEST_NEAR("get_vector", v.two_norm(), 1.0, 1e-12);

  p2.set(4,1);
  r = vgl_homg_operators_1d<float>::dot(p1,p2);
  TEST("dot", r, 27);

  r = vgl_homg_operators_1d<float>::cross(p1,p2);
  TEST("cross", r, -6);

  vgl_homg_point_1d<float> cj = vgl_homg_operators_1d<float>::conjugate(p1,p2,p3);
  TEST("conjugate", cj, vgl_homg_point_1d<float>(23.0f,7.0f));

  float cf = vgl_homg_operators_1d<float>::conjugate(1,7,4, 2);
  TEST("conjugate", cf, 3.f);

  r = vgl_homg_operators_1d<float>::distance(p1,p2);
  TEST("distance", r, 2);

  r = vgl_distance(p1,p2);
  TEST("distance", r, 2);

  std::stringstream is; is << "4.25 -5 7e1";
  vgl_homg_point_1d<double> p; is >> p;
  TEST("istream", p, vgl_homg_point_1d<double>(4.25,-5));
}

static void test_homg_point_2d()
{
  int d[] = {5,5,1};
  vgl_homg_point_2d<int> p1(3,7,1), p2(d), p3(-1,-8,1);

  TEST("inequality", (p1 != p3), true);

  p3.set(-6,-14,-2);
  TEST("equality", (p1 == p3), true);

  vgl_homg_point_2d<double> u(0,-3,4);
  vgl_homg_operators_2d<double>::unitize(u);
  TEST("unitize: x", u.x(), 0);
  TEST_NEAR("unitize: y", u.y(), -0.6, 1e-12);
  TEST_NEAR("unitize: w", u.w(), 0.8, 1e-12);

  vnl_vector_fixed<double,3> v = vgl_homg_operators_2d<double>::get_vector(u);
  TEST_NEAR("get_vector", v.two_norm(), 1.0, 1e-12);
  double dd[] = { 1,0,0, 0,2,0, 1,1,3};
  vnl_double_3x3 mm(dd);
  vgl_homg_point_2d<double> pt = mm*vgl_homg_point_2d<double>(3,7,1); // homography
  TEST("matrix*point", pt, vgl_homg_point_2d<double>(3,14,13));

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

  m = centre(p1,p3); // assignment
  vgl_homg_point_2d<int> cc(5,3,1);
  TEST("centre", m, cc);
  std::vector<vgl_homg_point_2d<int> > v1;
  v1.push_back(p1); v1.push_back(p2); v1.push_back(m);
  m = centre(v1);
  TEST("centre", m, p2);
  std::vector<vgl_homg_point_2d<int> > v2;
  v2.push_back(p2); v2.push_back(p3); v2.push_back(cc); v2.push_back(p2);
  m = centre(v2);
  TEST("centre", m, cc);

  r = cross_ratio(p1,p2,m,p3);
  TEST("cross_ratio", r, 1.5);

  vgl_homg_point_2d<double> q1(3,7,1), q2(5,6,1), q3(7,5,1), q4(-1,9,1);

  TEST("midpoint", vgl_homg_operators_2d<double>::midpoint(q1,q3), q2);

  r = vgl_homg_operators_2d<double>::distance_squared(q1,q2);
  TEST("vgl_homg_operators_2d<double>::distance_squared", r, 5);

  r = vgl_homg_operators_2d<double>::cross_ratio(q1,q2,q3,q4);
  TEST("vgl_homg_operators_2d<double>::cross_ratio", r, 3);

  q4 = vgl_homg_operators_2d<double>::conjugate(q1,q2,q3);
  TEST("vgl_homg_operators_2d<double>::conjugate", q4, vgl_homg_point_2d<double>(13,19,3));

  q4 = vgl_homg_operators_2d<double>::conjugate(q1,q2,q3,3);
  TEST("vgl_homg_operators_2d<double>::conjugate", q4, vgl_homg_point_2d<double>(-1,9,1));

  {
    vgl_homg_line_2d<double> l = vgl_homg_operators_2d<double>::join_oriented(q1,q2);
    TEST("vgl_homg_operators_2d<double>::join_oriented", l, vgl_homg_line_2d<double>(1,2,-17));
  }

  vgl_homg_line_2d<double> l1(0,0,1), l2(0,1,0), l3(1,1,1); // l1 = line at inf
  {
   vgl_homg_point_2d<double> pi(l1,l2); // intersection
   TEST("intersection", pi, vgl_homg_point_2d<double>(1,0,0));
   TEST("ideal", pi.ideal(), true);
   vgl_homg_point_2d<double> pj = vgl_homg_operators_2d<double>::intersection(l1,l2);
   TEST("intersection", pj, pi);
  }
  {
   vgl_homg_point_2d<double> pi(l2,l3); // intersection
   TEST("intersection", pi, vgl_homg_point_2d<double>(-1,0,1));
   TEST("is_ideal", is_ideal(pi), false);
   vgl_homg_point_2d<double> pj = vgl_homg_operators_2d<double>::intersection(l2,l3);
   TEST("intersection", pj, pi);
  }

  l1 = vgl_homg_operators_2d<double>::perp_line_through_point(l3,q1);
  TEST("vgl_homg_operators_2d<double>::perp_line_through_point", l1, vgl_homg_line_2d<double>(1,-1,4));

  q4 = vgl_homg_operators_2d<double>::perp_projection(l3,q1);
  TEST("vgl_homg_operators_2d<double>::perp_projection", q4, vgl_homg_point_2d<double>(-5,3,2));

  r = vgl_homg_operators_2d<double>::perp_dist_squared(l3,q1);
  TEST_NEAR("vgl_homg_operators_2d<double>::perp_dist_squared", r, 60.5, 1e-12);

  r = vgl_homg_operators_2d<double>::perp_dist_squared(q1,l3);
  TEST_NEAR("vgl_homg_operators_2d<double>::perp_dist_squared", r, 60.5, 1e-12);

  double pi_4 = 0.25*vnl_math::pi;
  r = vgl_homg_operators_2d<double>::line_angle(l3);
  TEST_NEAR("vgl_homg_operators_2d<double>::line_angle", r, pi_4, 1e-12);

  r = vgl_homg_operators_2d<double>::angle_between_oriented_lines(l2,l3);
  TEST_NEAR("vgl_homg_operators_2d<double>::angle_between_oriented_lines", r, -pi_4, 1e-12);
  r = vgl_homg_operators_2d<double>::angle_between_oriented_lines(l3,l2);
  TEST_NEAR("vgl_homg_operators_2d<double>::angle_between_oriented_lines", r, pi_4, 1e-12);

  r = vgl_homg_operators_2d<double>::abs_angle(l2,l3);
  TEST_NEAR("vgl_homg_operators_2d<double>::abs_angle", r, pi_4, 1e-12);
  r = vgl_homg_operators_2d<double>::abs_angle(l3,l2);
  TEST_NEAR("vgl_homg_operators_2d<double>::abs_angle", r, pi_4, 1e-12);

  std::stringstream is; is << "4.25 -5 7e1";
  vgl_homg_point_2d<float> p; is >> p;
  TEST("istream", p, vgl_homg_point_2d<float>(4.25f,-5,70));
}

static void test_homg_point_3d()
{
  int d[] = {5,5,5,1};
  vgl_homg_point_3d<int> p1(3,7,-1,1), p2(d), p3(-1,-8,7,1);

  TEST("inequality", (p1 != p3), true);

  p3.set(-6,-14,2,-2);
  TEST("equality", (p1 == p3), true);

  vgl_homg_point_3d<double> u(2,-1,-2,4);
  vgl_homg_operators_3d<double>::unitize(u);
  TEST_NEAR("unitize: x", u.x(),  0.4, 1e-12);
  TEST_NEAR("unitize: y", u.y(), -0.2, 1e-12);
  TEST_NEAR("unitize: z", u.z(), -0.4, 1e-12);
  TEST_NEAR("unitize: w", u.w(),  0.8, 1e-12);

  vnl_vector_fixed<double,4> v = vgl_homg_operators_3d<double>::get_vector(u);
  TEST_NEAR("get_vector", v.two_norm(), 1.0, 1e-12);
  double dd[] = { 1,0,0,0, 0,2,0,0, 0,0,3,0, 1,1,1,3};
  vnl_double_4x4 mm(dd);
  vgl_homg_point_3d<double> pt = mm*vgl_homg_point_3d<double>(3,-2,7,1); // homography
  TEST("matrix*point", pt, vgl_homg_point_3d<double>(3,-4,21,11));

  vgl_vector_3d<int> d1 = p1 - p2;
  TEST("sum; difference", (p2+d1), p1);

  TEST("+=", (p2+=d1), p1);
  TEST("+=", p2, p1);

  p2.set(4,5,2,1);
  bool b = collinear(p1,p2,p2);
  TEST("any two points are collinear", b, true);
  p3.set(7,-1,11,1);
  b = collinear(p1,p2,p3);
  TEST("three collinear points", b, true);
  double r = ratio(p1,p2,p3);
  TEST("ratio", r, 4.0);
  vgl_homg_point_3d<int> m = midpoint(p1,p2,4);
  TEST("midpoint", m, p3);
  p3.set(2,3,4,1); // "random" point
  b = collinear(p1,p2,p3);
  TEST("three arbitrary points are not collinear", b, false);

  b = coplanar(p1,p2,p3,p3); TEST("any three points are coplanar", b, true);
  vgl_homg_point_3d<int> p4;
  p4.set(7,12,1,2); // mid point of p1 and p2
  b = coplanar(p1,p2,p3,p4); TEST("a line and a point are coplanar", b, true);
  p4.set(1,-2,3,0); // point at infinity of the line p1-p2
  b = coplanar(p1,p2,p3,p4); TEST("a line and a point are coplanar", b, true);
  p4.set(9,3,4,1); // "random" point
  b = coplanar(p1,p2,p3,p4); TEST("four arbitrary points are not coplanar", b, false);
  p1.set(3,7,-1,0);
  p2.set(4,5,2,0);
  p3.set(2,3,4,0);
  p4.set(9,3,4,0);
  b = coplanar(p1,p2,p3,p4); TEST("four arbitrary points at infinity are coplanar", b, true);
  b = collinear(p1,p2,p3); TEST("three arbitrary points at infinity are not collinear", b, false);

  p1.set(3,7,-1,1);
  p2.set(4,5,2,1);
  p3.set(7,-1,11,1);
  m = centre(p1,p3); // assignment
  vgl_homg_point_3d<int> cc(5,3,5,1);
  TEST("centre", m, cc);
  std::vector<vgl_homg_point_3d<int> > v1;
  v1.push_back(p1); v1.push_back(p2); v1.push_back(m);
  m = centre(v1);
  TEST("centre", m, p2);
  std::vector<vgl_homg_point_3d<int> > v2;
  v2.push_back(p2); v2.push_back(p3); v2.push_back(cc); v2.push_back(p2);
  m = centre(v2);
  TEST("centre", m, cc);

  r = cross_ratio(p1,p2,m,p3);
  TEST("cross_ratio", r, 1.5);

  vgl_homg_point_3d<double> q1(1,3,7,1), q2(2,5,6,1), q3(3,7,5,1), q4(4,-1,9,1);

  TEST("midpoint", vgl_homg_operators_3d<double>::midpoint(q1,q3), q2);

  r = vgl_homg_operators_3d<double>::distance_squared(q1,q2);
  TEST("vgl_homg_operators_3d<double>::distance_squared", r, 6);

  r = vgl_homg_operators_3d<double>::cross_ratio(q1,q2,q3,q4);
  TEST("vgl_homg_operators_3d<double>::cross_ratio", r, 3);

  q4 = vgl_homg_operators_3d<double>::conjugate(q1,q2,q3);
  TEST("vgl_homg_operators_3d<double>::conjugate", q4, vgl_homg_point_3d<double>(90,234,342,54));

  q4 = vgl_homg_operators_3d<double>::conjugate(q1,q2,q3,3);
  TEST("vgl_homg_operators_3d<double>::conjugate", q4, vgl_homg_point_3d<double>(2,2,-18,-2));

  vgl_homg_plane_3d<double> pl1(0,0,0,1), pl2(0,1,0,0), pl3(0,0,1,0), pl4(1,1,1,1); // pl1 = plane at inf
  {
   vgl_homg_point_3d<double> pi(pl1,pl2,pl3); // intersection
   TEST("intersection", pi, vgl_homg_point_3d<double>(1,0,0,0));
   TEST("ideal", pi.ideal(), true);
   vgl_homg_point_3d<double> pj = vgl_homg_operators_3d<double>::intersection(pl1,pl2,pl3);
   TEST("intersection", pj, pi);
  }
  {
   vgl_homg_point_3d<double> pi(pl2,pl3,pl4); // intersection
   TEST("intersection", pi, vgl_homg_point_3d<double>(-1,0,0,1));
   TEST("is_ideal", is_ideal(pi), false);
   vgl_homg_point_3d<double> pj = vgl_homg_operators_3d<double>::intersection(pl2,pl3,pl4);
   TEST("intersection", pj, pi);
  }

  r = vgl_homg_operators_3d<double>::perp_dist_squared(pl3,q1);
  TEST_NEAR("vgl_homg_operators_3d<double>::perp_dist_squared", r, 49, 1e-12);

  r = vgl_homg_operators_3d<double>::perp_dist_squared(q1,pl3);
  TEST_NEAR("vgl_homg_operators_3d<double>::perp_dist_squared", r, 49, 1e-12);

  std::stringstream is; is << "4 -5 6 0";
  vgl_homg_point_3d<int> p; is >> p;
  TEST("istream", p, vgl_homg_point_3d<int>(4,-5,6,0));
}

static void test_homg_line_2d()
{
  double d[] = {5,5,-1};
  vgl_homg_line_2d<double> l1(3,7,0), l2(d), l3(0,-1,-8);

  TEST("vgl_distance_origin", vgl_distance_origin(l1), 0);
  TEST_NEAR("vgl_distance_origin", vgl_distance_origin(l2), .141421358, 1e-8);
  TEST("vgl_distance_origin", vgl_distance_origin(l3), 8);

  TEST("inequality", (l1 != l3), true);

  l3.set(3,7,0);
  TEST("equality", (l1 == l3), true);

  vnl_vector_fixed<double,3> v = vgl_homg_operators_2d<double>::get_vector(l2);
  vnl_vector_fixed<double,3> vtrue = vnl_vector_fixed<double,3>(d);
  TEST("get_vector", v, vtrue);
  double dd[] = { 1,0,0, 0,2,0, 1,1,3};
  vnl_double_3x3 mm(dd);
  vgl_homg_line_2d<double> l = mm*vgl_homg_line_2d<double>(3,7,1); // homography
  TEST("matrix*line", l, vgl_homg_line_2d<double>(3,14,13));

  vgl_homg_point_2d<double> p, q;
  l2.get_two_points(p,q);
  TEST("get_two_points()", p, vgl_homg_point_2d<double>(0,1,5));
  TEST("get_two_points()", q, vgl_homg_point_2d<double>(1,0,5));

  l2.set(3,4,0);
  l3.set(7,-1,0);
  bool b = concurrent(l1,l2,l3); // because they share the point (0,0)
  TEST("concurrent", b, true);

  l2.set(5,-12,-7);
  l2.normalize();
  TEST("normalize", l2.a() == -5.0/13 && l2.b() == 12.0/13 && l2.c() == 7.0/13, true);

  l2.set(3,4,0);
  l2.normalize();
  TEST("normalize", l2.a() == 0.6 && l2.b() == 0.8 && l2.c() == 0.0, true);

  std::vector<vgl_homg_line_2d<double> > lst;
  lst.push_back(l1); lst.push_back(l2); lst.push_back(l3);
  p = vgl_homg_operators_2d<double>::lines_to_point(lst);
  TEST("lines_to_point", p, vgl_homg_point_2d<double>(0,0,1));
  v = vgl_homg_operators_2d<double>::most_orthogonal_vector(lst);
  TEST("most_orthogonal_vector", v[0]==0&&v[1]==0&&v[2]==1, true);
  v = vgl_homg_operators_2d<double>::most_orthogonal_vector_svd(lst);
  TEST("most_orthogonal_vector_svd", v[0]==0&&v[1]==0&&v[2]==1, true);

  vgl_homg_point_2d<double> p1(1,0), p2(0,1);
  vgl_homg_line_2d<double> li(p1,p2); // line through these two points
  vgl_homg_line_2d<double> ll(1,1,-1);
  TEST("join", li, ll);

  vgl_homg_point_2d<double> p3(0,3);
  l3.set(0,-1,-8);
  TEST("vgl_distance(line,point)", vgl_distance(l3,p3), 11);
  TEST_NEAR("vgl_distance(line,point)", vgl_distance(l2,p3), 2.4, 1e-9);
  TEST_NEAR("vgl_distance(point,line)", vgl_distance(p3,l2), 2.4, 1e-9);

  TEST("ideal", ll.ideal(), false);
  ll.set(0,0,-7);
  TEST("ideal", ll.ideal(), true);

  vgl_homg_point_2d<double> pp = vgl_homg_operators_2d<double>::closest_point(l3,p1);
  TEST("closest_point to horizontal line", pp, vgl_homg_point_2d<double>(1,-8));
  p = vgl_homg_point_2d<double>(1,2,0);
  pp = vgl_homg_operators_2d<double>::closest_point(ll,p);
  TEST("closest_point to line at infinity", pp, p);
  pp = vgl_homg_operators_2d<double>::closest_point(li,p3);
  TEST("closest_point to finite line", pp, vgl_homg_point_2d<double>(-1,2));
  p = pp;
  pp = vgl_homg_operators_2d<double>::closest_point(li,p);
  TEST("closest_point to point on line", pp, p);

  std::stringstream is; is << "4.25 -5 7e1";
  vgl_homg_line_2d<float> h_l; is >> h_l;
  TEST("istream", h_l, vgl_homg_line_2d<float>(4.25f,-5,70));
}

static void test_homg_line_3d()
{
  vgl_homg_point_3d<double> p1(1,1,1,2), p2(1,1,1,0), p3(1,2,1,1);
  vgl_homg_line_3d_2_points<double> l1(p1,p2), l2(p2,p1), l3(p1,p3);

  TEST("inequality", (l1 != l3), true);
  TEST("equality", (l1 == l2), true);

  double dd[] = { 1,0,0,0, 0,2,0,0, 0,0,3,0, 1,1,1,3};
  vnl_double_4x4 mm(dd);
  vgl_homg_plane_3d<double> l = mm*vgl_homg_plane_3d<double>(1,3,7,1); // homography
  TEST("matrix*plane", l, vgl_homg_plane_3d<double>(1,6,21,14));

  vgl_homg_point_3d<double> p4(1,0,3);
  TEST_NEAR("vgl_distance(line,point)", vgl_distance(l3,p4), std::sqrt(72.0/11), 1e-9);
  TEST_NEAR("vgl_distance(line,point)", vgl_distance(l2,p4), std::sqrt(14.0/3), 1e-9);
  TEST_NEAR("vgl_distance(point,line)", vgl_distance(p4,l2), std::sqrt(14.0/3), 1e-9);

  TEST("ideal", l1.point_finite().ideal(), false);
  TEST("ideal", l3.point_finite().ideal(), false);
  TEST("ideal", l1.point_infinite().ideal(), true);
  TEST("ideal", l3.point_infinite().ideal(), true);

  l3 = vgl_homg_operators_3d<double>::perp_line_through_point(l1,p3);
  l2 = vgl_homg_line_3d_2_points<double>(p3,vgl_homg_point_3d<double>(1,-2,1,0));
  TEST("vgl_homg_operators_3d<double>::perp_line_through_point", l3, l2);

  p4 = vgl_homg_operators_3d<double>::perp_projection(l1,p1);
  TEST("vgl_homg_operators_3d<double>::perp_projection", p4, vgl_homg_point_3d<double>(1,1,1,2));

  double r = vgl_homg_operators_3d<double>::angle_between_oriented_lines(l1,l2);
  double pi_2 = 0.5*vnl_math::pi;
  TEST_NEAR("vgl_homg_operators_3d<double>::angle_between_oriented_lines", r, pi_2, 1e-9);

  r = vgl_homg_operators_3d<double>::perp_dist_squared(l3,p1);
  TEST_NEAR("vgl_homg_operators_3d<double>::perp_dist_squared", r, 6.25/3, 1e-12);

  r = vgl_homg_operators_3d<double>::perp_dist_squared(p1,l3);
  TEST_NEAR("vgl_homg_operators_3d<double>::perp_dist_squared", r, 6.25/3, 1e-12);
}

static void test_homg_plane_3d()
{
  double d[] = {0,3,4,1};
  vgl_homg_plane_3d<double> pl1(3,7,-1,1), pl2(d), pl3(-1,-8,7,1);

  TEST("inequality", (pl1 != pl3), true);

  pl3.set(3,7,-1,1);
  TEST("equality", (pl1 == pl3), true);

  vnl_vector_fixed<double,4> v = vgl_homg_operators_3d<double>::get_vector(pl2);
  vnl_vector_fixed<double,4> vtrue = vnl_vector_fixed<double,4>(d);
  TEST("get_vector", v, vtrue);

  vgl_vector_3d<double> d1 = pl2.normal();
  vgl_vector_3d<double> d2 = vgl_vector_3d<double>(0,0.6,0.8);
  TEST_NEAR("normal", (d1-d2).sqr_length(), 0.0, 1e-12);

  pl2.set(1,3,4,0);
  pl3.set(1,7,-1,0);

  vgl_homg_point_3d<double> p1(1,0,0), p2(0,1,0), p3(0,0,1);
  vgl_homg_plane_3d<double> pl(p1,p2,p3); // homg_plane through 3 points
  vgl_homg_plane_3d<double> pp(1,1,1,-1);
  TEST("join", pl, pp);

  vgl_homg_plane_3d<double> pln(pl.normal(),p1);
  TEST("Plane constructed with normal",pln,pl);

  TEST_NEAR("vgl_distance(plane,point)", vgl_distance(pl2,p2), std::sqrt(4.5/13), 1e-9);
  TEST_NEAR("vgl_distance(point,plane)", vgl_distance(p2,pl2), std::sqrt(4.5/13), 1e-9);

  std::vector<vgl_homg_plane_3d<double> > lst;
  lst.push_back(pl1); lst.push_back(pl2); lst.push_back(pl3);
  vgl_homg_point_3d<double> p = vgl_homg_operators_3d<double>::planes_to_point(lst);
  TEST_NEAR("planes_to_point", (p-vgl_homg_point_3d<double>(-31,5,4,62)).sqr_length(), 0.0, 1e-12);
  v = vgl_homg_operators_3d<double>::most_orthogonal_vector_svd(lst);
  TEST_NEAR("most_orthogonal_vector_svd", v.two_norm(), 1.0, 1e-12);
  TEST_NEAR("most_orthogonal_vector_svd", (v-vgl_homg_operators_3d<double>::get_vector(p)).two_norm(), 0.0, 1e-12);

  TEST("ideal", pp.ideal(), false);
  pp.set(0,0,0,-7);
  TEST("ideal", pp.ideal(), true);

  pl2.set(-4,3,0,27);
  pl2.normalize();
  TEST("normalize", pl2.a() == 0.8 && pl2.b() == -0.6 && pl2.c() == 0.0 && pl2.d() == -5.4, true);

  pl2.set(-9,-12,20,15);
  pl2.normalize();
  TEST("normalize", pl2.a() == -0.36 && pl2.b() == -0.48 && pl2.c() == 0.8 && pl2.d() == 0.6, true);

  std::stringstream is; is << "4.25 -5 7e1 5e-1";
  vgl_homg_plane_3d<double> l; is >> l;
  TEST("istream", l, vgl_homg_plane_3d<double>(4.25f,-5,70,0.5));
}

inline bool collinear(vgl_homg_line_2d<int> const& l1,
                      vgl_homg_line_2d<int> const& l2,
                      vgl_homg_line_2d<int> const& l3) {
  return concurrent(l1,l2,l3);
}

#if 0 // currently not used
inline bool collinear(vgl_homg_line_3d_2_points<double> const& l1,
                      vgl_homg_line_3d_2_points<double> const& l2,
                      vgl_homg_line_3d_2_points<double> const& l3) {
  return concurrent(l1,l2,l3) && coplanar(l1,l2,l3);
}
#endif

inline double ratio(vgl_homg_line_2d<int> const& l1,
                    vgl_homg_line_2d<int> const& l2,
                    vgl_homg_line_2d<int> const& l3)
{
  return (l3.a()-l1.a())/(l2.a()-l1.a());
}

inline double cross_ratio(vgl_homg_line_2d<int>const& l1, vgl_homg_line_2d<int>const& l2,
                          vgl_homg_line_2d<int>const& l3, vgl_homg_line_2d<int>const& l4)
{
  vgl_homg_point_2d<int> p1(l1.a(),l1.b(),l1.c());
  vgl_homg_point_2d<int> p2(l2.a(),l2.b(),l2.c());
  vgl_homg_point_2d<int> p3(l3.a(),l3.b(),l3.c());
  vgl_homg_point_2d<int> p4(l4.a(),l4.b(),l4.c());
  return cross_ratio(p1,p2,p3,p4);
}

static void test_1d_basis()
{
  std::cout << "  TEST OF PROJECTIVE BASIS WITH 1D POINTS\n";

  vgl_homg_point_1d<float> p11(0.f), p12(1.f), p13(1.f,0.f);
  vgl_1d_basis<vgl_homg_point_1d<float> >  b_1_p(p11,p12,p13);
  // The following is essentially just a mapping
  // from vgl_homg_point_1d<float> to vgl_homg_point_1d<double> :
  vgl_homg_point_1d<double> p = b_1_p.project(p11);
  TEST("origin", p, vgl_homg_point_1d<double>(0.0));
  p = b_1_p.project(p12);
  TEST("unit point", p, vgl_homg_point_1d<double>(1.0));
  p = b_1_p.project(p13);
  TEST("infinity point", p, vgl_homg_point_1d<double>(1.0,0.0));
  p = b_1_p.project(vgl_homg_point_1d<float>(-3.f));
  TEST("point at -3", p, vgl_homg_point_1d<double>(-3.0));

  std::cout << "  TEST OF PROJECTIVE BASIS ON A 2D LINE\n";

  vgl_homg_point_2d<int> p21(0,1), p22(1,3), p23(2,5); // On the line 2x-y+w=0
  vgl_1d_basis<vgl_homg_point_2d<int> >  b_2_p(p21,p22,p23);
  p = b_2_p.project(p21);
  TEST("origin", p, vgl_homg_point_1d<double>(0.0));
  p = b_2_p.project(p22);
  TEST("unit point", p, vgl_homg_point_1d<double>(1.0));
  p = b_2_p.project(p23);
  TEST("infinity point", p, vgl_homg_point_1d<double>(1.0,0.0));
  p = b_2_p.project(vgl_homg_point_2d<int>(1,2,0));
  TEST("point at -1", p, vgl_homg_point_1d<double>(-1.0));

  std::cout << "  TEST OF PROJECTIVE BASIS ON A 3D LINE\n";

  vgl_homg_point_3d<double> p31(0,1,3), p32(1,3,2), p33(2,5,1); // On the line 2x-y+w=0,x+z=3w
  vgl_1d_basis<vgl_homg_point_3d<double> >  b_3_p(p31,p32,p33);
  p = b_3_p.project(p31);
  TEST("origin", p, vgl_homg_point_1d<double>(0.0));
  p = b_3_p.project(p32);
  TEST("unit point", p, vgl_homg_point_1d<double>(1.0));
  p = b_3_p.project(p33);
  TEST("infinity point", p, vgl_homg_point_1d<double>(1.0,0.0));
  p = b_3_p.project(vgl_homg_point_3d<double>(1,2,-1,0));
  TEST("point at -1", p, vgl_homg_point_1d<double>(-1.0));

  std::cout << "  TEST OF PROJECTIVE BASIS OF CONCURRENT 2D LINES\n";

  vgl_homg_line_2d<int> l21(0,1,1), l22(1,3,1), l23(2,5,1); // Through the point (2,-1,1)
  vgl_1d_basis<vgl_homg_line_2d<int> >  b_2_l(l21,l22,l23);
  p = b_2_l.project(l21);
  TEST("origin", p, vgl_homg_point_1d<double>(0.0));
  p = b_2_l.project(l22);
  TEST("unit point", p, vgl_homg_point_1d<double>(1.0));
  p = b_2_l.project(l23);
  TEST("infinity point", p, vgl_homg_point_1d<double>(1.0,0.0));
  p = b_2_l.project(vgl_homg_line_2d<int>(1,2,0));
  TEST("point at -1", p, vgl_homg_point_1d<double>(-1.0));
}

static void test_homg()
{
  std::cout << "-- Testing vgl_homg_point_1d --\n";
  test_homg_point_1d();
  std::cout << "-- Testing vgl_homg_point_2d --\n";
  test_homg_point_2d();
  std::cout << "-- Testing vgl_homg_point_3d --\n";
  test_homg_point_3d();
  std::cout << "-- Testing vgl_homg_line_2d --\n";
  test_homg_line_2d();
  std::cout << "-- Testing vgl_homg_line_3d --\n";
  test_homg_line_3d();
  std::cout << "-- Testing vgl_homg_plane_3d --\n";
  test_homg_plane_3d();
  std::cout << "-- Testing vgl_1d_basis --\n";
  test_1d_basis();
}

TESTMAIN(test_homg);

#include "vgl/vgl_1d_basis.hxx"
VGL_1D_BASIS_INSTANTIATE(vgl_homg_point_1d<float>);
VGL_1D_BASIS_INSTANTIATE(vgl_homg_point_2d<int>);
VGL_1D_BASIS_INSTANTIATE(vgl_homg_line_2d<int>);
VGL_1D_BASIS_INSTANTIATE(vgl_homg_point_3d<double>);
