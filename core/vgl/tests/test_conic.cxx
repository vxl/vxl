// This is an extensive test suite for vgl_conic,
// written by Peter Vanroose, ESAT, K.U.Leuven, Belgium, 30 August 2001.
#include <testlib/testlib_test.h>
#include <vgl/vgl_conic.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/algo/vgl_homg_operators_2d.h>
#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_vector.h>

static inline double sqr(double x) { return x*x; }

static void
testlib_test_assert_near(const vcl_string& msg, vgl_homg_point_2d<double> const& p2,
                         vgl_homg_point_2d<double> const& p1, double tol = 1e-6)
{
  double expr = (p1.w() == 0 && p2.w() == 0) ? sqr(p1.x()*p2.y()-p1.y()*p2.x()) :
                vgl_homg_operators_2d<double>::distance_squared(p1,p2);
  testlib_test_assert_near(msg, expr, 0.0, tol);
}

static void
testlib_test_assert_near(const vcl_string& msg, vgl_homg_line_2d<double> const& l2,
                         vgl_homg_line_2d<double> const& l1, double tol = 1e-6)
{
  double expr = sqr(l1.a()/l1.b()-l2.a()/l2.b())
               +sqr(l1.a()/l1.c()-l2.a()/l2.c())
               +sqr(l1.b()/l1.c()-l2.b()/l2.c());
  testlib_test_assert_near(msg, expr, 0.0, tol);
}

static void
testlib_test_assert_near(const vcl_string& msg, vgl_box_2d<double> const& b1,
                         vgl_box_2d<double> const& b2, double tol = 1e-6)
{
  double expr = sqr(b1.min_x()-b2.min_x())
               +sqr(b1.min_y()-b2.min_y())
               +sqr(b1.max_x()-b2.max_x())
               +sqr(b1.max_y()-b2.max_y());
  testlib_test_assert_near(msg, expr, 0.0, tol);
}

static double
conic_distance(vgl_conic<double> const& c1, vgl_conic<double> const& c2)
{
  double k;  // multiplicative factor for coefficients

  if      ( c1.a() != 0 && c2.a() != 0 )  k = c1.a() / c2.a();
  else if ( c1.b() != 0 && c2.b() != 0 )  k = c1.b() / c2.b();
  else if ( c1.c() != 0 && c2.c() != 0 )  k = c1.c() / c2.c();
  else if ( c1.d() != 0 && c2.d() != 0 )  k = c1.d() / c2.d();
  else if ( c1.e() != 0 && c2.e() != 0 )  k = c1.e() / c2.e();
  else if ( c1.f() != 0 && c2.f() != 0 )  k = c1.f() / c2.f();
  else                                    k = 1.0;

#define DO_MAX(x,a) if ((a)>(x)) x=a
  double expr = vcl_abs( c1.a() - c2.a() * k );
  DO_MAX(expr,  vcl_abs( c1.b() - c2.b() * k ));
  DO_MAX(expr,  vcl_abs( c1.c() - c2.c() * k ));
  DO_MAX(expr,  vcl_abs( c1.d() - c2.d() * k ));
  DO_MAX(expr,  vcl_abs( c1.e() - c2.e() * k ));
  DO_MAX(expr,  vcl_abs( c1.f() - c2.f() * k ));
#undef DO_MAX
  return expr;
}

static void
testlib_test_assert_near(const vcl_string& msg, vgl_conic<double> const& c2,
                         vgl_conic<double> const& c1, double tol = 1e-6)
{
  testlib_test_assert_near(msg, conic_distance(c1,c2), 0.0, tol);
}

static void
testlib_test_assert_far(const vcl_string& msg, vgl_conic<double> const& c2,
                        vgl_conic<double> const& c1, double tol = 1e-6)
{
  testlib_test_assert_far(msg, conic_distance(c1,c2), 0.0, tol);
}

static void test_conic()
{
  // "global" variables, actually constants
  vgl_homg_point_2d<double> const centre(1,2,1);
  vgl_homg_point_2d<double> const direction(4,-3,0);
  double const halfpi = vcl_atan2(1.0,0.0);
  double const pythagoras = vcl_atan2(4.0,3.0); // = 0.9273 radians (53.13 degrees)

  // 1. Test constructors
  vcl_cout << "\n\t=== test constructors ===\n";
  // ellipse, centre (1,2), axes lengths 2,1, rotated by 0, pi/2, pi, -pi/2.
  vgl_conic<double> cc(centre, 2,1, 0.0);
  vgl_conic<double> c (centre, 1,2, halfpi);
  vgl_conic<double> c2(centre, 2,1, 2*halfpi);
  vgl_conic<double> c3(centre, 1,2, -halfpi);
  TEST_NEAR("ellipse equality", cc,c, 1e-6);
  TEST_NEAR("ellipse equality", c2,c, 1e-6);
  TEST_NEAR("ellipse equality", c3,c, 1e-6);
  // hyperbola, centre (1,2), axes lengths 2,1, rotated by pi/2.
  cc = vgl_conic<double>(centre, 2,-1, 0.0); // assignment
  c  = vgl_conic<double>(centre, -1,2, halfpi);
  c2 = vgl_conic<double>(centre, 2,-1, 2*halfpi);
  c3 = vgl_conic<double>(centre, -1,2, -halfpi);
  TEST_NEAR("hyperbola equality", cc,c, 1e-6);
  TEST_NEAR("hyperbola equality", c2,c, 1e-6);
  TEST_NEAR("hyperbola equality", c3,c, 1e-6);
  // parabola, "centre" (1,2,0), top (2,1), width parameter 3.
  cc = vgl_conic<double>(direction, 2,1, 3);
  c  = vgl_conic<double>(direction, 2,1,-3);
  TEST_FAR("parabola inequality", cc,c, 1e-6);

  // 2. Test circle
  vcl_cout << "\n\t=== test circle ===\n";
  c = vgl_conic<double>(centre, 1,1, 0); // circle, centre (1,2), radius 1, orientation irrelevant.
  vcl_cout << c << '\n';
  TEST("conic is circle", c.real_type(), "real circle");
  vnl_vector<double> v = vgl_homg_operators_2d<double>::get_vector(c);
  TEST("get_vector", v.size()==6&&v[0]==1&&v[1]==0&&v[2]==1&&v[3]==-2&&v[4]==-4&&v[5]==4, true);
  cc = vgl_conic<double>(1,0,1,-2,-4,4); // idem, by equation
  TEST("circle equality", c, cc);
  vgl_homg_point_2d<double> npt(0,2,1);
  TEST("contains", c.contains(npt), true);
  cc = c.dual_conic();
  TEST("dual conic", cc, vgl_conic<double>(0,4,3,2,4,1));
  npt = vgl_homg_point_2d<double>(0,1,1);
  double dst = vgl_homg_operators_2d<double>::distance_squared(c,npt);
  TEST_NEAR("distance point to circle (outside)", dst, 3-2*vcl_sqrt(2.0), 1e-6);
  vcl_list<vgl_homg_line_2d<double> > lines = vgl_homg_operators_2d<double>::tangent_from(c, npt);
  TEST("tangent lines count = 2", lines.size(), 2);
  TEST("first tangent line", lines.front(), vgl_homg_line_2d<double>(1,0,0));
  TEST("second tangent line", lines.back(), vgl_homg_line_2d<double>(0,1,-1));
  npt = vgl_homg_point_2d<double>(0.5,2,1);
  dst = vgl_homg_operators_2d<double>::distance_squared(c,npt);
  TEST("distance point to circle (inside)", dst, 0.25);
  lines = vgl_homg_operators_2d<double>::tangent_from(c, npt);
  TEST("tangent lines count = 0", lines.size(), 0);

  cc = vgl_conic<double>(1,0,1,-2,-4,4); // idem, by equation
  double mm[] = { 1,0,-1, 0,1,-2, -1,-2,4};
  vnl_double_3x3 m(mm);
  cc = vgl_homg_operators_2d<double>::vgl_conic_from_matrix(m);
  TEST("vgl_conic_from_matrix", cc, c);
  vnl_double_3x3 m2 = vgl_homg_operators_2d<double>::matrix_from_conic(c);
  TEST("matrix_from_conic", m2, m);
  m2 = vgl_homg_operators_2d<double>::matrix_from_dual_conic(c.dual_conic());
  if (m2(0,0) < 0) m2 = -m2;
  TEST("matrix_from_dual_conic", m2, m);

  TEST("bounding box", vgl_homg_operators_2d<double>::compute_bounding_box(c),
                       vgl_box_2d<double>(0,2,1,3));

  npt = vgl_homg_operators_2d<double>::closest_point(c, vgl_homg_point_2d<double>(1.0,20.0,1.0));
  TEST("closest point (outside)", npt, vgl_homg_point_2d<double>(1.0,3.0,1.0));
  npt = vgl_homg_operators_2d<double>::closest_point(c, vgl_homg_point_2d<double>(0.5,2.0,1.0));
  TEST("closest point (inside)", npt, vgl_homg_point_2d<double>(0.0,2.0,1.0));
  npt = vgl_homg_operators_2d<double>::closest_point(c, vgl_homg_point_2d<double>(0.0,2.0,1.0));
  TEST("closest point (on)", npt, vgl_homg_point_2d<double>(0.0,2.0,1.0));

  // 3. Test ellipse
  vcl_cout << "\n\t=== test ellipse ===\n";
  // ellipse, centre (1,2), axes lengths 10,5, orientation(3,4).
  cc = vgl_conic<double>(centre, 10,5, pythagoras);
  c = vgl_conic<double>(73, -72, 52, -2, -136, -2363);
  vcl_cout << cc << '\n' << c << '\n';
  TEST("conic is ellipse", c.real_type(), "real ellipse");
  TEST("centre", c.centre(), centre);
  TEST_NEAR("ellipse equality", cc,c, 1e-6);

  vgl_homg_point_2d<double> startp(7, 10, 1); // rightmost top on the long axis
  vgl_homg_point_2d<double> endp  (5, -1, 1); // downmost top on the short axis
  vgl_homg_point_2d<double> top   (-3, 5, 1); // upmost top on the short axis
  TEST("contains right top", c.contains(startp), true);
  TEST("contains down top", c.contains(endp), true);
  TEST("contains up top", c.contains(top), true);

  npt = vgl_homg_point_2d<double>(-7,8,1);
  dst = vgl_homg_operators_2d<double>::distance_squared(c,npt);
  TEST_NEAR("distance point to ellipse (outside)", dst, 25, 1e-6);
  lines = vgl_homg_operators_2d<double>::tangent_from(c, npt);
  TEST("tangent lines count = 2", lines.size(), 2);
  TEST_NEAR("first tangent line", lines.front(), vgl_homg_line_2d<double>(0.1553449137,0.010926795,1), 1e-6);
  TEST_NEAR("second tangent line", lines.back(), vgl_homg_line_2d<double>(0.022785773,-0.1050624521,1), 1e-6);
  npt = vgl_homg_point_2d<double>(-2,7,2);
  dst = vgl_homg_operators_2d<double>::distance_squared(c,npt);
  TEST_NEAR("distance point to ellipse (inside)", dst, 6.25, 1e-6);
  lines = vgl_homg_operators_2d<double>::tangent_from(c, npt);
  TEST("tangent lines count = 0", lines.size(), 0);

  vcl_cout << vgl_homg_operators_2d<double>::compute_bounding_box(c) << vcl_endl;
  TEST_NEAR("bounding box", vgl_homg_operators_2d<double>::compute_bounding_box(c),
                            vgl_box_2d<double>(-6.2111,8.2111,-6.544,10.544), 1e-6);

  npt = vgl_homg_operators_2d<double>::closest_point(c,startp);
  TEST("closest_point to top", npt, startp);
  npt = vgl_homg_operators_2d<double>::closest_point(c,centre);
  if (vgl_homg_operators_2d<double>::distance_squared(npt,top) > 1)
  { TEST_NEAR("closest_point to centre (1,2)", npt, endp, 1e-6); }
  else
  { TEST_NEAR("closest_point to centre (1,2)", npt, top, 1e-6); }
  npt = vgl_homg_operators_2d<double>::closest_point(c, vgl_homg_point_2d<double>(-23,20,1));
  TEST_NEAR("closest point (outside)", npt, top, 1e-6);
  npt = vgl_homg_operators_2d<double>::closest_point(c, vgl_homg_point_2d<double>(-2,7,2));
  TEST_NEAR("closest point (inside)", npt, top, 1e-6);

  vgl_homg_line_2d<double> l (1,-1,0); // line x=y
  vcl_list<vgl_homg_point_2d<double> > pts = vgl_homg_operators_2d<double>::intersection(c,l);
  double x1 = (69+100*vcl_sqrt(13.0))/53, x2 = (69-100*vcl_sqrt(13.0))/53; // or interchanged
  TEST("intersection count = 2", pts.size(), 2);
  TEST_NEAR("first point", pts.front(), vgl_homg_point_2d<double>(x1,x1,1), 1e-6);
  TEST_NEAR("second point", pts.back(), vgl_homg_point_2d<double>(x2,x2,1), 1e-6);

  cc = vgl_conic<double>(centre, 10,5, 0); // centre (1,2), radii 10,5, orientation (1,0).
  vcl_cout << cc << '\n';
  pts = vgl_homg_operators_2d<double>::intersection(c,cc);
  TEST("intersection count = 4", pts.size(), 4);
  vcl_list<vgl_homg_point_2d<double> >::iterator it = pts.begin();
  vcl_cout << (*it) << '\n';
  TEST_NEAR("first point", (*it), vgl_homg_point_2d<double>(-1.42536,6.85071,1), 1e-6);
  ++it; vcl_cout << (*it) << '\n';
  TEST_NEAR("second point", (*it), vgl_homg_point_2d<double>(8.07107,5.53553,1), 1e-6);
  ++it; vcl_cout << (*it) << '\n';
  TEST_NEAR("third point", (*it), vgl_homg_point_2d<double>(3.42536,-2.85071,1), 1e-6);
  ++it; vcl_cout << (*it) << '\n';
  TEST_NEAR("fourth point", (*it), vgl_homg_point_2d<double>(6.07107,1.53553,-1), 1e-6);

  lines = vgl_homg_operators_2d<double>::common_tangents(c,cc);
  TEST("common tangent count = 4", lines.size(), 4);
  vcl_list<vgl_homg_line_2d<double> >::iterator il = lines.begin();
  vcl_cout << c << '\n' << cc << '\n';
  for (; il != lines.end(); ++il) vcl_cout << (*il) << '\n';

  // 4. Test hyperbola
  vcl_cout << "\n\t=== test hyperbola ===\n";
  // hyperbola, centre (1,2), axes lengths -10,5, orientation(3,4).
  cc = vgl_conic<double>(centre, -10,5, pythagoras);
  c = vgl_conic<double>(11, -24, 4, 26, 8, -521);
  vcl_cout << c << '\n';
  TEST("conic is hyperbola", c.real_type(), "hyperbola");
  TEST("centre is (1,2)", c.centre(), centre);
  TEST_NEAR("hyperbola equality", cc,c, 1e-6);

  // Main axis is the line 3x+4y-11w=0, secondary axis is 4x-3y+2w=0
  vgl_homg_point_2d<double> top1(5, -1, 1); // right top on the long axis
  vgl_homg_point_2d<double> top2(-3, 5, 1); // left top on the long axis
  TEST("contains right top (5,-1)", c.contains(top1), true);
  TEST("contains left top (-3,5)", c.contains(top2), true);

  npt = vgl_homg_point_2d<double>(-7,8,1);
  dst = vgl_homg_operators_2d<double>::distance_squared(c,npt);
  TEST_NEAR("distance point to hyperbola (inside)", dst, 25, 1e-6);
  lines = vgl_homg_operators_2d<double>::tangent_from(c, npt);
  TEST("tangent lines count = 0", lines.size(), 0);
  npt = vgl_homg_point_2d<double>(-2,7,2);
  dst = vgl_homg_operators_2d<double>::distance_squared(c,npt);
  TEST_NEAR("distance point to hyperbola (outside)", dst, 6.25, 1e-6);
  lines = vgl_homg_operators_2d<double>::tangent_from(c, npt);
  TEST("tangent lines count = 2", lines.size(), 2);
  TEST_NEAR("first tangent line", lines.front(), vgl_homg_line_2d<double>(0.5442245293,-0.1302215668,1), 1e-6);
  TEST_NEAR("second tangent line", lines.back(), vgl_homg_line_2d<double>(0.1402140042,-0.2456531417,1), 1e-6);

  npt = vgl_homg_operators_2d<double>::closest_point(c,top1);
  TEST("closest_point to top", npt, top1);
  npt = vgl_homg_operators_2d<double>::closest_point(c,centre);
  if (vgl_homg_operators_2d<double>::distance_squared(npt,top1) > 1)
  { TEST_NEAR("closest_point to centre", npt, top2, 1e-6); }
  else
  { TEST_NEAR("closest_point to centre", npt, top1, 1e-6); }
  npt = vgl_homg_operators_2d<double>::closest_point(c, vgl_homg_point_2d<double>(-2,7,2));
  TEST_NEAR("closest point (outside)", npt, top2, 1e-6);

  l = vgl_homg_line_2d<double>(1,1,0); // line x+y=0
  pts = vgl_homg_operators_2d<double>::intersection(c,l);
  x1 = (-9-20*vcl_sqrt(51.0))/39; x2 = (-9+20*vcl_sqrt(51.0))/39; // or interchanged
  TEST("intersection count = 2", pts.size(), 2);
  TEST_NEAR("first point", pts.front(), vgl_homg_point_2d<double>(x1,-x1,1), 1e-6);
  TEST_NEAR("second point", pts.back(), vgl_homg_point_2d<double>(x2,-x2,1), 1e-6);

  // 5. Test parabola
  vcl_cout << "\n\t=== test parabola ===\n";
  // parabola, top (1,2), orientation(4,-3), "excentricity" -30.
  cc = vgl_conic<double>(direction, 1,2, -30);
  c = vgl_conic<double>(9, 24, 16, -114, -52, 97);
  vcl_cout << c << '\n';
  TEST("conic is parabola", c.real_type(), "parabola");
  TEST("centre is (4,-3,0) (at infinity)", c.centre(), direction);
  TEST("parabola equality", cc, c);
  // Dual conic of any parabola is a conic though (0,0,1), i.e., with f()==0:
  cc = c.dual_conic();
  TEST("dual conic", cc, vgl_conic<double>(73,53,-198,100,-75,0));
  // Symmetry axis is the line 3x+4y-11w=0, top is (1,2)
  top = vgl_homg_point_2d<double>(1, 2, 1);
  TEST("contains top", c.contains(top), true);
  // Tangent in top is 4x-3y+2w=0, i.e., orthogonal to axis:
  TEST("tangent in top is 4x-3y+2w=0", c.tangent_at(top), vgl_homg_line_2d<double>(4,-3,2));

  npt = vgl_homg_point_2d<double>(-3,5,1);
  dst = vgl_homg_operators_2d<double>::distance_squared(c,npt);
  TEST_NEAR("distance point to parabola (outside)", dst, 25, 1e-6);
  lines = vgl_homg_operators_2d<double>::tangent_from(c, npt);
  TEST("tangent lines count = 2", lines.size(), 2);
  TEST_NEAR("first tangent line", lines.front(), vgl_homg_line_2d<double>(2.662925678,1.797755319,-1), 1e-6);
  TEST_NEAR("second tangent line", lines.back(), vgl_homg_line_2d<double>(0.0793177136,0.2475906227,-1), 1e-6);
  npt = vgl_homg_point_2d<double>(6,1,2);
  dst = vgl_homg_operators_2d<double>::distance_squared(c,npt);
  TEST("distance point to parabola (inside)", dst <= 6.25, true);
  lines = vgl_homg_operators_2d<double>::tangent_from(c, npt);
  TEST("tangent lines count = 0", lines.size(), 0);

  npt = vgl_homg_operators_2d<double>::closest_point(c,top);
  TEST("closest_point to top", npt, top);
  npt = vgl_homg_operators_2d<double>::closest_point(c,c.centre());
  TEST("closest_point to centre", npt, c.centre());
  npt = vgl_homg_operators_2d<double>::closest_point(c, vgl_homg_point_2d<double>(-3,5,1));
  TEST_NEAR("closest point (outside)", npt, top, 1e-6);

  l = vgl_homg_line_2d<double>(1,-1,0); // line x=y
  pts = vgl_homg_operators_2d<double>::intersection(c,l);
  x1 = (83+2*vcl_sqrt(534.0))/49; x2 = (83-2*vcl_sqrt(534.0))/49; // or interchanged
  TEST("intersection count = 2", pts.size(), 2);
  TEST_NEAR("first point", pts.front(), vgl_homg_point_2d<double>(x1,x1,1), 1e-6);
  TEST_NEAR("second point", pts.back(), vgl_homg_point_2d<double>(x2,x2,1), 1e-6);

  l = vgl_homg_line_2d<double>(3,4,0); // line parallel to the symmetry axis
  pts = vgl_homg_operators_2d<double>::intersection(c,l);
  TEST("intersection count = 2", pts.size(), 2);
  TEST_NEAR("first point is centre", pts.front(), vgl_homg_point_2d<double>(4,-3,0), 1e-6);
  TEST_NEAR("second point", pts.back(), vgl_homg_point_2d<double>(388,-291,300), 1e-6); // or interchanged

  // 6. Test imaginary circle
  vcl_cout << "\n\t=== test imaginary circle ===\n";
  // imaginary circle, centre (1,2), radius 5i.
  c = vgl_conic<double>(1, 0, 1, -2, -4, 30);
  vcl_cout << c << '\n';
  TEST("conic is imaginary circle", c.real_type(), "imaginary circle");
  TEST("centre is (1,2)", c.centre(), vgl_homg_point_2d<double>(centre));

  // 7. Test imaginary ellipse
  vcl_cout << "\n\t=== test imaginary ellipse ===\n";
  // imaginary ellipse, centre (1,2), axes lengths 5i, 10i, orientation (1,0).
  c = vgl_conic<double>(4, 0, 1, -8, -4, 108);
  vcl_cout << c << '\n';
  TEST("conic is imaginary ellipse", c.real_type(), "imaginary ellipse");
  TEST("centre is (1,2)", c.centre(), vgl_homg_point_2d<double>(centre));

  // 8. Degenerate conics

  // a. real parallel lines:
  vcl_cout << "\n\t=== test 2 real parallel lines ===\n";
  vgl_homg_line_2d<double> l1(1,2,3), l2(1,2,5);
  c = vgl_conic<double>(1,4,4,8,16,15);
  vcl_cout << c << '\n';
  TEST("conic is 2 real parallel lines", c.real_type(), "real parallel lines");
  TEST("centre is common point at infinity", c.centre(), vgl_homg_point_2d<double>(-2,1,0));
  lines = c.components();
  TEST("2 components", lines.size(), 2);
  TEST("first component is x+2y+3w=0", lines.front(), l1);
  TEST("second component is x+2y+5w=0", lines.back(), l2); // or vice versa
  lines = vgl_homg_operators_2d<double>::tangent_from(c, c.centre());
  TEST("tangent lines count from centre = 2", lines.size(), 2);
  npt = vgl_homg_point_2d<double>(3,1,-1);
  TEST("contains (-3,-1)", c.contains(npt), true);
  lines = vgl_homg_operators_2d<double>::tangent_from(c, npt);
  TEST("tangent lines count from point on degenerate conic = 1", lines.size(), 1);
  TEST("tangent line", lines.front(), l2);
  npt = vgl_homg_point_2d<double>(0,1,1);
  TEST("!contains (-3,-1)", c.contains(npt), false);
  lines = vgl_homg_operators_2d<double>::tangent_from(c, npt);
  TEST("tangent lines count from other point = 0", lines.size(), 0);

  // b. imaginary parallel lines:
  vcl_cout << "\n\t=== test 2 imaginary parallel lines ===\n";
  c = vgl_conic<double>(4,4,1,0,0,9); // lines 2x+y+/-3iw=0
  vcl_cout << c << '\n';
  TEST("conic is 2 complex conjugate parallel lines", c.real_type(), "complex parallel lines");
  TEST("centre is common point at infinity", c.centre(), vgl_homg_point_2d<double>(1,-2,0));
  lines = c.components();
  TEST("0 components", lines.size(), 0);
  lines = vgl_homg_operators_2d<double>::tangent_from(c, c.centre());
  TEST("tangent lines count from centre = 0", lines.size(), 0); // tangents are imaginary
  npt = vgl_homg_point_2d<double>(3,1,1);
  TEST("!contains (3,1)", c.contains(npt), false);

  // c. finite coincident lines
  vcl_cout << "\n\t=== test 2 finite coincident lines ===\n";
  l1 = vgl_homg_line_2d<double>(1,2,3);
  c = vgl_conic<double>(1,4,4,6,12,9);
  vcl_cout << c << '\n';
  TEST("conic is 2 coincident lines", c.real_type(), "coincident lines");
  lines = c.components();
  TEST("2 components", lines.size(), 2);
  TEST("first component is x+2y+3w=0", lines.front(), l1);
  TEST("second component is x+2y+3w=0", lines.back(), l1);
  npt = vgl_homg_point_2d<double>(1,1,-1);
  TEST("contains (-1,-1)", c.contains(npt), true);
  lines = vgl_homg_operators_2d<double>::tangent_from(c, npt);
  TEST("tangent lines count from point on degenerate conic = 2", lines.size(), 2);
  TEST("tangent line", lines.front(), l1);
  TEST("tangent line", lines.back(), l1);
  npt = vgl_homg_point_2d<double>(0,1,1);
  TEST("!contains (0,1)", c.contains(npt), false);
  lines = vgl_homg_operators_2d<double>::tangent_from(c, npt);
  TEST("tangent lines count from other point = 0", lines.size(), 0);

  // d. infinite coincident lines
  vcl_cout << "\n\t=== test 2 infinite coincident lines ===\n";
  l1 = vgl_homg_line_2d<double>(0,0,1);
  c = vgl_conic<double>(0,0,0,0,0,1);
  vcl_cout << c << '\n';
  TEST("conic is 2 coincident lines lines", c.real_type(), "coincident lines");
  lines = c.components();
  TEST("2 components", lines.size(), 2);
  TEST("first component is w=0", lines.front(), l1);
  TEST("second component is w=0", lines.back(), l1);
  npt = vgl_homg_point_2d<double>(3,1,0);
  TEST("contains (3,1,0)", c.contains(npt), true);
  lines = vgl_homg_operators_2d<double>::tangent_from(c, npt);
  TEST("tangent lines count from point on degenerate conic = 2", lines.size(), 2);
  TEST("tangent line", lines.front(), l1);
  TEST("tangent line", lines.back(), l1);
  npt = vgl_homg_point_2d<double>(0,1,1);
  TEST("!contains (0,1)", c.contains(npt), false);
  lines = vgl_homg_operators_2d<double>::tangent_from(c, npt);
  TEST("tangent lines count from other point = 0", lines.size(), 0);

  // e. finite, real intersecting lines:
  vcl_cout << "\n\t=== test 2 finite, real intersecting lines ===\n";
  l1 = vgl_homg_line_2d<double>(1,2,3);
  l2 = vgl_homg_line_2d<double>(2,-1,1);
  c = vgl_conic<double>(2,3,-2,7,-1,3);
  vcl_cout << c << '\n';
  TEST("conic is 2 intersecting lines", c.real_type(), "real intersecting lines");
  TEST("centre is intersection point", c.centre(), vgl_homg_point_2d<double>(1,1,-1));
  lines = c.components();
  TEST("2 components", lines.size(), 2);
  TEST("first component is 2x-y+w=0", lines.front(), l2);
  TEST("second component is x+2y+3w=0", lines.back(), l1); // or vice versa
  lines = vgl_homg_operators_2d<double>::tangent_from(c, c.centre());
  TEST("tangent lines count from centre = 2", lines.size(), 2);
  npt = vgl_homg_point_2d<double>(0,1,1);
  TEST("contains (0,1)", c.contains(npt), true);
  lines = vgl_homg_operators_2d<double>::tangent_from(c, npt);
  TEST("tangent lines count from point on degenerate conic = 1", lines.size(), 1);
  TEST("tangent line", lines.front(), l2);
  npt = vgl_homg_point_2d<double>(1,1,0);
  TEST("!contains (1,1,0)", c.contains(npt), false);
  lines = vgl_homg_operators_2d<double>::tangent_from(c, npt);
  TEST("tangent lines count from other point = 0", lines.size(), 0);

  // f. imaginary intersecting lines:
  vcl_cout << "\n\t=== test 2 imaginary intersecting lines ===\n";
  c = vgl_conic<double>(4,0,1,12,0,9); // lines 2x+/-iy+3w=0
  vcl_cout << c << '\n';
  TEST("conic is 2 complex conjugate intersecting lines", c.real_type(), "complex intersecting lines");
  TEST("centre is intersection point", c.centre(), vgl_homg_point_2d<double>(3,0,-2));
  lines = c.components();
  TEST("0 components", lines.size(), 0);
  npt = vgl_homg_point_2d<double>(0,1,1);
  TEST("!contains (0,1)", c.contains(npt), false);
  lines = vgl_homg_operators_2d<double>::tangent_from(c, c.centre());
  TEST("tangent lines count from centre = 0", lines.size(), 0); // tangents are imaginary
  vcl_cout << vgl_homg_operators_2d<double>::compute_bounding_box(c) << vcl_endl;
  TEST("bounding box", vgl_homg_operators_2d<double>::compute_bounding_box(c),
                       vgl_box_2d<double>(-1.5,-1.5,0,0));

  // g. one finite and one infinite intersecting line:
  vcl_cout << "\n\t=== test intersecting lines, 1 infinite ===\n";
  l1 = vgl_homg_line_2d<double>(1,2,3);
  l2 = vgl_homg_line_2d<double>(0,0,1);
  c = vgl_conic<double>(0,0,0,1,2,3);
  vcl_cout << c << '\n';
  TEST("conic is 2 intersecting lines", c.real_type(), "real intersecting lines");
  TEST("centre is point at infinity", c.centre(), vgl_homg_point_2d<double>(-2,1,0));
  lines = c.components();
  TEST("2 components", lines.size(), 2);
  TEST("first component is w=0", lines.front(), l2);
  TEST("second component is x+2y+3w=0", lines.back(), l1); // or vice versa
  lines = vgl_homg_operators_2d<double>::tangent_from(c, c.centre());
  TEST("tangent lines count from centre = 2", lines.size(), 2);
  npt = vgl_homg_point_2d<double>(3,0,-1);
  TEST("contains (-3,0)", c.contains(npt), true);
  lines = vgl_homg_operators_2d<double>::tangent_from(c, npt);
  TEST("tangent lines count from point on degenerate conic = 1", lines.size(), 1);
  TEST("tangent line", lines.front(), l1);
  npt = vgl_homg_point_2d<double>(0,1,1);
  TEST("!contains (0,1)", c.contains(npt), false);
  lines = vgl_homg_operators_2d<double>::tangent_from(c, npt);
  TEST("tangent lines count from other point = 0", lines.size(), 0);

  // Intersections
  c = vgl_conic<double>(centre, 3,3, 0.0); // circle
  cc = vgl_conic<double>(centre, 1,1, 0.0); // concentric circle
  pts = vgl_homg_operators_2d<double>::intersection(c,cc);
  TEST("intersection count of concentric circles = 0", pts.size(), 0);
  for (it = pts.begin(); it != pts.end(); ++it)
    vcl_cout << (*it) << '\n';

  cc = vgl_conic<double>(vgl_homg_point_2d<double>(1,1,1), 1,1, 0.0); // non-concentric circle
  pts = vgl_homg_operators_2d<double>::intersection(c,cc);
  TEST("intersection count of non-concentric circles = 0", pts.size(), 0);
  for (it = pts.begin(); it != pts.end(); ++it)
    vcl_cout << (*it) << '\n';

  cc = vgl_conic<double>(vgl_homg_point_2d<double>(1,5,1), 1,1, 0.0); // intersecting circle
  pts = vgl_homg_operators_2d<double>::intersection(c,cc);
  TEST("intersection count of intersecting circles = 2", pts.size(), 2);
  for (it = pts.begin(); it != pts.end(); ++it)
    vcl_cout << (*it) << '\n';

  cc = vgl_conic<double>(vgl_homg_point_2d<double>(1,5,1), 3,3, 0.0); // intersecting circle
  pts = vgl_homg_operators_2d<double>::intersection(c,cc);
  TEST("intersection count of intersecting circles = 2", pts.size(), 2);
  for (it = pts.begin(); it != pts.end(); ++it)
    vcl_cout << (*it) << '\n';

  cc = vgl_conic<double>(vgl_homg_point_2d<double>(1,1,1), 2,2, 0.0); // tangent circle
  pts = vgl_homg_operators_2d<double>::intersection(c,cc);
  TEST("intersection of touching circles = 2 coincident pts", pts.size(), 2);
  for (it = pts.begin(); it != pts.end(); ++it)
    vcl_cout << (*it) << '\n';
  TEST("1st intersection point = (1,-1)", pts.front(), vgl_homg_point_2d<double>(1,-1,1));
  TEST("2nd intersection point = (1,-1)", pts.back(), vgl_homg_point_2d<double>(1,-1,1));

  cc = vgl_conic<double>(centre, 3,1, 0.0); // concentric touching ellipse
  pts = vgl_homg_operators_2d<double>::intersection(c,cc);
  TEST("intersection of touching ellipses = 2x 2 coincident points", pts.size(), 4);
  for (it = pts.begin(); it != pts.end(); ++it)
    vcl_cout << (*it) << '\n';
  TEST("1st  intersection point = (4,2)", pts.front(), vgl_homg_point_2d<double>(4,2,1));
  TEST("last intersection point = (-2,2)", pts.back(), vgl_homg_point_2d<double>(-2,2,1));

  cc = vgl_conic<double>(centre, 4,2, 0.0); // concentric intersecting ellipse
  pts = vgl_homg_operators_2d<double>::intersection(c,cc);
  TEST("intersection of intersecting ellipses = 4 points", pts.size(), 4);
  for (it = pts.begin(); it != pts.end(); ++it)
    vcl_cout << (*it) << '\n';

  c = vgl_conic<double>(centre, 1,2, 1.5); // arbitrary, concentric ellipses
  cc = vgl_conic<double>(centre, 1,3, 3.0);
  pts = vgl_homg_operators_2d<double>::intersection(c,cc);
  TEST("intersection of intersecting ellipses = 4 points", pts.size(), 4);
  for (it = pts.begin(); it != pts.end(); ++it)
    vcl_cout << (*it) << '\n';

  c = vgl_conic<double>(centre, 3,-3, 0.0); // orthogonal hyperbola
  cc = vgl_conic<double>(centre, 1,-1, 0.0); // concentric hyperbola with same asymptotes
  pts = vgl_homg_operators_2d<double>::intersection(c,cc);
  TEST("intersection of concentric orthogonal hyperbolas = 2x 2 points at infinity", pts.size(), 4);
  for (it = pts.begin(); it != pts.end(); ++it)
    vcl_cout << (*it) << '\n';
  TEST("1st  intersection point is on asymptote x+y=0", pts.front(), vgl_homg_point_2d<double>(1,-1,0));
  TEST("last intersection point is at other asymptote x=y", pts.back(), vgl_homg_point_2d<double>(1,1,0));

  cc = vgl_conic<double>(vgl_homg_point_2d<double>(1,5,1), 1,-1, 0.0); // intersecting hyperbola
  pts = vgl_homg_operators_2d<double>::intersection(c,cc);
  TEST("intersection count of intersecting hyperbolas = 4", pts.size(), 4);
  for (it = pts.begin(); it != pts.end(); ++it)
    vcl_cout << (*it) << '\n';
  TEST("1st intersection point is on asymptote x+y=0", pts.front(), vgl_homg_point_2d<double>(1,-1,0));

  cc = vgl_conic<double>(vgl_homg_point_2d<double>(1,5,1), 3,-1, 0.0); // intersecting hyperbola
  pts = vgl_homg_operators_2d<double>::intersection(c,cc);
  TEST("intersection count of intersecting hyperbolas = 4", pts.size(), 4);
  bool infi = false;
  for (it = pts.begin(); it != pts.end(); ++it)
  { vcl_cout << (*it) << '\n'; if ((*it).w() == 0) infi = true; }
  TEST("intersection points are not at infinity", infi, false);

  cc = vgl_conic<double>(centre, 3,3, 0.0); // concentric touching circle
  pts = vgl_homg_operators_2d<double>::intersection(c,cc);
  TEST("intersection of hyperbola with concentric touching circle = 2x 2 points", pts.size(), 4);
  for (it = pts.begin(); it != pts.end(); ++it)
    vcl_cout << (*it) << '\n';
  TEST("1st  intersection point = (-2,2)", pts.front(), vgl_homg_point_2d<double>(-2,2,1));
  TEST("last intersection point = (4,2)", pts.back(), vgl_homg_point_2d<double>(4,2,1));

  cc = vgl_conic<double>(centre, 2,2, 0.0); // concentric smaller circle
  pts = vgl_homg_operators_2d<double>::intersection(c,cc);
  TEST("intersection of hyperbola with concentric smaller circle = empty", pts.size(), 0);
  for (it = pts.begin(); it != pts.end(); ++it)
    vcl_cout << (*it) << '\n';

  cc = vgl_conic<double>(centre, 4,4, 0.0); // concentric larger circle
  pts = vgl_homg_operators_2d<double>::intersection(c,cc);
  TEST("intersection of hyperbola with concentric larger circle = 4 points", pts.size(), 4);
  for (it = pts.begin(); it != pts.end(); ++it)
    vcl_cout << (*it) << '\n';
}

TESTMAIN(test_conic);
