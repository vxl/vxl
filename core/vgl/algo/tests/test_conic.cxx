// This is an extensive test suite for vgl_conic,
// written by Peter Vanroose, ESAT, K.U.Leuven, Belgium, 30 August 2001.
//
// Dec.2010 - PVr - added specific test for a bug in conic intersection,
//                  when two intersection points have equal y value.
//                  (This bug detected by Patrick Pol, 29 Nov. 2010.)
//
#include <cmath>
#include <iostream>
#include <sstream>
#include "testlib/testlib_test.h"
#include "vgl/vgl_conic.h"
#include "vgl/vgl_box_2d.h"
#include "vgl/vgl_homg_point_2d.h"
#include <vgl/algo/vgl_homg_operators_2d.h>
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include "vnl/vnl_double_3x3.h"
#include "vnl/vnl_vector_fixed.h"
#include "vnl/vnl_math.h"

static inline double
sqr(double x)
{
  return x * x;
}

static void
testlib_test_assert_near(const std::string & msg,
                         vgl_homg_point_2d<double> const & p2,
                         vgl_homg_point_2d<double> const & p1,
                         double tol = 1e-6)
{
  double expr = (p1.w() == 0 && p2.w() == 0) ? sqr(p1.x() * p2.y() - p1.y() * p2.x())
                                             : vgl_homg_operators_2d<double>::distance_squared(p1, p2);
  testlib_test_assert_near(msg, expr, 0.0, tol);
}

static void
testlib_test_assert_near(const std::string & msg,
                         vgl_homg_line_2d<double> const & l2,
                         vgl_homg_line_2d<double> const & l1,
                         double tol = 1e-6)
{
  double expr = sqr(l1.a() / l1.b() - l2.a() / l2.b()) + sqr(l1.a() / l1.c() - l2.a() / l2.c()) +
                sqr(l1.b() / l1.c() - l2.b() / l2.c());
  testlib_test_assert_near(msg, expr, 0.0, tol);
}

static void
testlib_test_assert_near(const std::string & msg,
                         vgl_box_2d<double> const & b1,
                         vgl_box_2d<double> const & b2,
                         double tol = 1e-6)
{
  double expr = sqr(b1.min_x() - b2.min_x()) + sqr(b1.min_y() - b2.min_y()) + sqr(b1.max_x() - b2.max_x()) +
                sqr(b1.max_y() - b2.max_y());
  testlib_test_assert_near(msg, expr, 0.0, tol);
}

static double
conic_distance(vgl_conic<double> const & c1, vgl_conic<double> const & c2)
{
  double k; // multiplicative factor for coefficients

  if (c1.a() != 0 && c2.a() != 0)
    k = c1.a() / c2.a();
  else if (c1.b() != 0 && c2.b() != 0)
    k = c1.b() / c2.b();
  else if (c1.c() != 0 && c2.c() != 0)
    k = c1.c() / c2.c();
  else if (c1.d() != 0 && c2.d() != 0)
    k = c1.d() / c2.d();
  else if (c1.e() != 0 && c2.e() != 0)
    k = c1.e() / c2.e();
  else if (c1.f() != 0 && c2.f() != 0)
    k = c1.f() / c2.f();
  else
    k = 1.0;

#define DO_MAX(x, a)                                                                                                   \
  if ((a) > (x))                                                                                                       \
  (x) = a
  double expr = std::abs(c1.a() - c2.a() * k);
  DO_MAX(expr, std::abs(c1.b() - c2.b() * k));
  DO_MAX(expr, std::abs(c1.c() - c2.c() * k));
  DO_MAX(expr, std::abs(c1.d() - c2.d() * k));
  DO_MAX(expr, std::abs(c1.e() - c2.e() * k));
  DO_MAX(expr, std::abs(c1.f() - c2.f() * k));
#undef DO_MAX
  return expr;
}

static void
testlib_test_assert_near(const std::string & msg,
                         vgl_conic<double> const & c2,
                         vgl_conic<double> const & c1,
                         double tol = 1e-6)
{
  testlib_test_assert_near(msg, conic_distance(c1, c2), 0.0, tol);
}

static void
testlib_test_assert_far(const std::string & msg,
                        vgl_conic<double> const & c2,
                        vgl_conic<double> const & c1,
                        double tol = 1e-6)
{
  testlib_test_assert_far(msg, conic_distance(c1, c2), 0.0, tol);
}

static void
check_points_on_conics(vgl_conic<double> const & c1,
                       vgl_conic<double> const & c2,
                       std::list<vgl_homg_point_2d<double>> const & pts)
{
  // verify whether each of the intersection points (in pts) really lie on the conics
  auto it = pts.begin();
  for (; it != pts.end(); ++it)
  {
    vgl_homg_point_2d<double> p = *it;
    double fval1, fval2, x, y;
    if (p.w() == 0)
    { // point at infinity
      x = p.x();
      y = p.y();
      fval1 = x * x * c1.a() + x * y * c1.b() + y * y * c1.c();
      fval2 = x * x * c2.a() + x * y * c2.b() + y * y * c2.c();
    }
    else
    {
      x = p.x() / p.w();
      y = p.y() / p.w();
      fval1 = x * x * c1.a() + x * y * c1.b() + y * y * c1.c() + x * c1.d() + y * c1.e() + c1.f();
      fval2 = x * x * c2.a() + x * y * c2.b() + y * y * c2.c() + x * c2.d() + y * c2.e() + c2.f();
    }
    if (fval1 < -1e-4 || fval1 > 1e-4)
    {
      std::cout << "Intersection " << (p.w() == 0 ? "direction" : "point") << " (" << x << ',' << y << ")\n";
      TEST(" not on conic 1", true, false);
    }
    if (fval2 < -1e-4 || fval2 > 1e-4)
    {
      std::cout << "Intersection " << (p.w() == 0 ? "direction" : "point") << " (" << x << ',' << y << ")\n";
      TEST(" not on conic 2", true, false);
    }
  }
  std::cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n";
}

static void
test_conic()
{
  // "global" variables, actually constants
  vgl_homg_point_2d<double> const centre(1, 2, 1);
  vgl_homg_point_2d<double> const direction(4, -3, 0);
  double const halfpi = vnl_math::pi_over_2;
  double const pythagoras = std::atan2(4.0, 3.0); // = 0.9273 radians (53.13 degrees)

  // 1. Test constructors
  std::cout << "\n\t=== test constructors ===\n";
  // ellipse, centre (1,2), axes lengths 2,1, rotated by 0, pi/2, pi, -pi/2.
  vgl_conic<double> cc(centre, 2, 1, 0.0);
  vgl_conic<double> c(centre, 1, 2, halfpi);
  vgl_conic<double> c2(centre, 2, 1, 2 * halfpi);
  vgl_conic<double> c3(centre, 1, 2, -halfpi);
  TEST_NEAR("ellipse equality", cc, c, 1e-6);
  TEST_NEAR("ellipse equality", c2, c, 1e-6);
  TEST_NEAR("ellipse equality", c3, c, 1e-6);
  // hyperbola, centre (1,2), axes lengths 2,1, rotated by pi/2.
  cc = vgl_conic<double>(centre, 2, -1, 0.0); // assignment
  c = vgl_conic<double>(centre, -1, 2, halfpi);
  c2 = vgl_conic<double>(centre, 2, -1, 2 * halfpi);
  c3 = vgl_conic<double>(centre, -1, 2, -halfpi);
  TEST_NEAR("hyperbola equality", cc, c, 1e-6);
  TEST_NEAR("hyperbola equality", c2, c, 1e-6);
  TEST_NEAR("hyperbola equality", c3, c, 1e-6);
  // parabola, "centre" (1,2,0), top (2,1), width parameter 3.
  cc = vgl_conic<double>(direction, 2, 1, 3);
  c = vgl_conic<double>(direction, 2, 1, -3);
  TEST_FAR("parabola inequality", cc, c, 1e-6);
  // istream
  std::stringstream is;
  is << "1.0 -2.0 3.0 -4.0 5.0 0.0";
  vgl_conic<double> c4;
  is >> c4;
  vgl_conic<double> c5(1.0, -2.0, 3.0, -4.0, 5.0, 0.0);
  TEST("istream", c4, c5);

  // 2. Test circle
  std::cout << "\n\t=== test circle ===\n";
  c = vgl_conic<double>(centre, 1, 1, 0); // circle, centre (1,2), radius 1, orientation irrelevant.
  std::cout << c << '\n';
  TEST("conic is circle", c.real_type(), "real circle");
  vnl_vector_fixed<double, 6> v = vgl_homg_operators_2d<double>::get_vector(c);
  TEST("get_vector", v[0] == 1 && v[1] == 0 && v[2] == 1 && v[3] == -2 && v[4] == -4 && v[5] == 4, true);
  TEST_NEAR("curvature at (1,1)", c.curvature_at(vgl_point_2d<double>(1, 1)), 1.0, 1e-6);
  TEST_NEAR("curvature at (2,2)", c.curvature_at(vgl_point_2d<double>(2, 2)), 1.0, 1e-6);

  cc = vgl_conic<double>(1, 0, 1, -2, -4, 4); // idem, by equation
  std::cout << "By equation: " << cc << '\n';
  TEST("circle equality", c, cc);
  double xc, yc, major_axis, minor_axis, angle;
  bool good = cc.ellipse_geometry(xc, yc, major_axis, minor_axis, angle);
  TEST("circle geometry", good, true);
  TEST_NEAR("circle geometry: centre x", xc, 1, 1e-8);
  TEST_NEAR("circle geometry: centre y", yc, 2, 1e-8);
  TEST_NEAR("circle geometry: maj axis", major_axis, 1, 1e-8);
  TEST_NEAR("circle geometry: min axis", minor_axis, 1, 1e-8);

  cc = vgl_conic<double>(-1, 0, -1, 2, 4, -4); // idem, by "opposite" equation
  std::cout << "By 'opposite' equation: " << cc << '\n';
  TEST("circle equality", c, cc);
  good = cc.ellipse_geometry(xc, yc, major_axis, minor_axis, angle);
  TEST("circle geometry", good, true);
  TEST_NEAR("circle geometry: centre x", xc, 1, 1e-8);
  TEST_NEAR("circle geometry: centre y", yc, 2, 1e-8);
  TEST_NEAR("circle geometry: maj axis", major_axis, 1, 1e-8);
  TEST_NEAR("circle geometry: min axis", minor_axis, 1, 1e-8);
  vgl_homg_point_2d<double> npt(0, 2, 1);
  TEST("contains", c.contains(npt), true);
  cc = c.dual_conic();
  TEST("dual conic", cc, vgl_conic<double>(0, 4, 3, 2, 4, 1));
  npt = vgl_homg_point_2d<double>(0, 1, 1);
  double dst = vgl_homg_operators_2d<double>::distance_squared(c, npt);
  TEST_NEAR("distance point to circle (outside)", dst, 3 - 2 * std::sqrt(2.0), 1e-6);
  std::list<vgl_homg_line_2d<double>> lines = vgl_homg_operators_2d<double>::tangent_from(c, npt);
  TEST("tangent lines count = 2", lines.size(), 2);
  TEST("first tangent line", lines.front(), vgl_homg_line_2d<double>(1, 0, 0));
  TEST("second tangent line", lines.back(), vgl_homg_line_2d<double>(0, 1, -1));
  npt = vgl_homg_point_2d<double>(0.5, 2, 1);
  dst = vgl_homg_operators_2d<double>::distance_squared(c, npt);
  TEST("distance point to circle (inside)", dst, 0.25);
  lines = vgl_homg_operators_2d<double>::tangent_from(c, npt);
  TEST("tangent lines count = 0", lines.size(), 0);

  cc = vgl_conic<double>(1, 0, 1, -2, -4, 4); // idem, by equation
  double mm[] = { 1, 0, -1, 0, 1, -2, -1, -2, 4 };
  vnl_double_3x3 m(mm);
  cc = vgl_homg_operators_2d<double>::vgl_conic_from_matrix(m);
  TEST("vgl_conic_from_matrix", cc, c);
  vnl_double_3x3 m2 = vgl_homg_operators_2d<double>::matrix_from_conic(c);
  TEST("matrix_from_conic", m2, m);
  m2 = vgl_homg_operators_2d<double>::matrix_from_dual_conic(c.dual_conic());
  if (m2(0, 0) < 0)
    m2 = -m2;
  TEST("matrix_from_dual_conic", m2, m);

  TEST("bounding box", vgl_homg_operators_2d<double>::compute_bounding_box(c), vgl_box_2d<double>(0, 2, 1, 3));

  npt = vgl_homg_operators_2d<double>::closest_point(c, vgl_homg_point_2d<double>(1.0, 20.0, 1.0));
  TEST("closest point (outside)", npt, vgl_homg_point_2d<double>(1.0, 3.0, 1.0));
  npt = vgl_homg_operators_2d<double>::closest_point(c, vgl_homg_point_2d<double>(0.5, 2.0, 1.0));
  TEST("closest point (inside)", npt, vgl_homg_point_2d<double>(0.0, 2.0, 1.0));
  npt = vgl_homg_operators_2d<double>::closest_point(c, vgl_homg_point_2d<double>(0.0, 2.0, 1.0));
  TEST("closest point (on)", npt, vgl_homg_point_2d<double>(0.0, 2.0, 1.0));

  // 3. Test ellipse
  std::cout << "\n\t=== test ellipse ===\n";
  // ellipse, centre (1,2), axes lengths 10,5, orientation(3,4).
  cc = vgl_conic<double>(centre, 10, 5, pythagoras);
  c = vgl_conic<double>(73, -72, 52, -2, -136, -2363);
  std::cout << cc << '\n' << c << '\n';
  TEST("conic is ellipse", c.real_type(), "real ellipse");
  TEST("centre", c.centre(), centre);
  TEST_NEAR("ellipse equality", cc, c, 1e-6);
  // since the orientation is given counterclockwise,
  // one of the tops should be the point (1,2)+(6,8) (and not (1,2)+(6,-8)):
  TEST("top (6,8)", c.contains(vgl_homg_point_2d<double>(7.0, 10.0, 1.0)), true);
  TEST("top not (6,-8)", c.contains(vgl_homg_point_2d<double>(7.0, -6.0, 1.0)), false);
  TEST("top not (8,6)", c.contains(vgl_homg_point_2d<double>(9.0, 8.0, 1.0)), false);
  TEST("top not (8,-6)", c.contains(vgl_homg_point_2d<double>(9.0, -4.0, 1.0)), false);

  // Test ellipse geometry for an ellipse with a = 2, b = 1 slanted 45 degrees
  // and centered at (1, 2)

  std::cout << "Test ellipse geometry\n";
  cc = vgl_conic<double>(0.492576994, -0.591092393, 0.492576994, 0.197030798, -1.379215583, 0.492576994);
  // identical to cc = vgl_conic<double>(centre, 2,1, vnl_math::pi_over_4);
  good = cc.ellipse_geometry(xc, yc, major_axis, minor_axis, angle);
  std::cout << "ellipse(" << xc << ' ' << yc << ' ' << major_axis << ' ' << minor_axis << ' '
            << angle * vnl_math::deg_per_rad << ")\n";
  TEST("ellipse geometry", good, true);
  TEST_NEAR("ellipse geometry: centre x", xc, 1, 1e-5);
  TEST_NEAR("ellipse geometry: centre y", yc, 2, 1e-5);
  TEST_NEAR("ellipse geometry: maj axis", major_axis, 2, 1e-3);
  TEST_NEAR("ellipse geometry: min axis", minor_axis, 1, 1e-3);
  TEST_NEAR("ellipse geometry: angle", angle, vnl_math::pi_over_4, 1e-6);

  // End ellipse geometry test

  // Test ellipse geometry for an ellipse with a = 2, b = 1 slanted -45 degrees
  // and centered at (-1, 2)

  std::cout << "Test ellipse geometry\n";
  cc = vgl_conic<double>(-0.492576994, -0.591092393, -0.492576994, 0.197030798, 1.379215583, -0.492576994);
  good = cc.ellipse_geometry(xc, yc, major_axis, minor_axis, angle);
  std::cout << "ellipse(" << xc << ' ' << yc << ' ' << major_axis << ' ' << minor_axis << ' '
            << angle * vnl_math::deg_per_rad << ")\n";
  TEST("ellipse geometry", good, true);
  TEST_NEAR("ellipse geometry: centre x", xc, -1, 1e-5);
  TEST_NEAR("ellipse geometry: centre y", yc, 2, 1e-5);
  TEST_NEAR("ellipse geometry: maj axis", major_axis, 2, 1e-3);
  TEST_NEAR("ellipse geometry: min axis", minor_axis, 1, 1e-3);
  TEST_NEAR("ellipse geometry: angle", angle, -vnl_math::pi_over_4, 1e-6);

  // End ellipse geometry test

  vgl_homg_point_2d<double> startp(7, 10, 1); // rightmost top on the long axis
  vgl_homg_point_2d<double> endp(5, -1, 1);   // downmost top on the short axis
  vgl_homg_point_2d<double> top(-3, 5, 1);    // upmost top on the short axis
  TEST("contains right top", c.contains(startp), true);
  TEST("contains down top", c.contains(endp), true);
  TEST("contains up top", c.contains(top), true);

  npt = vgl_homg_point_2d<double>(-7, 8, 1);
  dst = vgl_homg_operators_2d<double>::distance_squared(c, npt);
  TEST_NEAR("distance point to ellipse (outside)", dst, 25, 1e-6);
  lines = vgl_homg_operators_2d<double>::tangent_from(c, npt);
  TEST("tangent lines count = 2", lines.size(), 2);
  TEST_NEAR("first tangent line", lines.front(), vgl_homg_line_2d<double>(0.1553449137, 0.010926795, 1), 1e-6);
  TEST_NEAR("second tangent line", lines.back(), vgl_homg_line_2d<double>(0.022785773, -0.1050624521, 1), 1e-6);
  npt = vgl_homg_point_2d<double>(-2, 7, 2);
  dst = vgl_homg_operators_2d<double>::distance_squared(c, npt);
  TEST_NEAR("distance point to ellipse (inside)", dst, 6.25, 1e-6);
  lines = vgl_homg_operators_2d<double>::tangent_from(c, npt);
  TEST("tangent lines count = 0", lines.size(), 0);

  std::cout << vgl_homg_operators_2d<double>::compute_bounding_box(c) << std::endl;
  TEST_NEAR("bounding box",
            vgl_homg_operators_2d<double>::compute_bounding_box(c),
            vgl_box_2d<double>(-6.2111, 8.2111, -6.544, 10.544),
            1e-6);

  npt = vgl_homg_operators_2d<double>::closest_point(c, startp);
  TEST("closest_point to top", npt, startp);
  npt = vgl_homg_operators_2d<double>::closest_point(c, centre);
  if (vgl_homg_operators_2d<double>::distance_squared(npt, top) > 1)
  {
    TEST_NEAR("closest_point to centre (1,2)", npt, endp, 1e-6);
  }
  else
  {
    TEST_NEAR("closest_point to centre (1,2)", npt, top, 1e-6);
  }
  npt = vgl_homg_operators_2d<double>::closest_point(c, vgl_homg_point_2d<double>(-23, 20, 1));
  TEST_NEAR("closest point (outside)", npt, top, 1e-6);
  npt = vgl_homg_operators_2d<double>::closest_point(c, vgl_homg_point_2d<double>(-2, 7, 2));
  TEST_NEAR("closest point (inside)", npt, top, 1e-6);

  vgl_homg_line_2d<double> l(1, -1, 0); // line x=y
  std::list<vgl_homg_point_2d<double>> pts = vgl_homg_operators_2d<double>::intersection(c, l);
  double x1 = (69 + 100 * std::sqrt(13.0)) / 53, x2 = (69 - 100 * std::sqrt(13.0)) / 53; // or interchanged
  TEST("intersection count = 2", pts.size(), 2);
  TEST_NEAR("first point", pts.front(), vgl_homg_point_2d<double>(x1, x1, 1), 1e-6);
  TEST_NEAR("second point", pts.back(), vgl_homg_point_2d<double>(x2, x2, 1), 1e-6);

  cc = vgl_conic<double>(centre, 10, 5, 0); // centre (1,2), radii 10,5, orientation (1,0).
  std::cout << cc << '\n';
  pts = vgl_homg_operators_2d<double>::intersection(c, cc);
  TEST("intersection count = 4", pts.size(), 4);
  auto it = pts.begin();
  std::cout << (*it) << '\n';
  TEST_NEAR("first point", (*it), vgl_homg_point_2d<double>(-1.42536, 6.85071, 1), 1e-6);
  ++it;
  std::cout << (*it) << '\n';
  TEST_NEAR("second point", (*it), vgl_homg_point_2d<double>(8.07107, 5.53553, 1), 1e-6);
  ++it;
  std::cout << (*it) << '\n';
  TEST_NEAR("third point", (*it), vgl_homg_point_2d<double>(3.42536, -2.85071, 1), 1e-6);
  ++it;
  std::cout << (*it) << '\n';
  TEST_NEAR("fourth point", (*it), vgl_homg_point_2d<double>(6.07107, 1.53553, -1), 1e-6);

  lines = vgl_homg_operators_2d<double>::common_tangents(c, cc);
  TEST("common tangent count = 4", lines.size(), 4);
  auto il = lines.begin();
  std::cout << c << '\n' << cc << '\n';
  for (; il != lines.end(); ++il)
    std::cout << (*il) << '\n';

  // 4. Test hyperbola
  std::cout << "\n\t=== test hyperbola ===\n";
  // hyperbola, centre (1,2), axes lengths -10,5, orientation(3,4).
  cc = vgl_conic<double>(centre, -10, 5, pythagoras);
  c = vgl_conic<double>(11, -24, 4, 26, 8, -521);
  std::cout << c << '\n';
  TEST("conic is hyperbola", c.real_type(), "hyperbola");
  TEST("centre is (1,2)", c.centre(), centre);
  TEST_NEAR("hyperbola equality", cc, c, 1e-6);

  // Main axis is the line 3x+4y-11w=0, secondary axis is 4x-3y+2w=0
  vgl_homg_point_2d<double> top1(5, -1, 1); // right top on the long axis
  vgl_homg_point_2d<double> top2(-3, 5, 1); // left top on the long axis
  TEST("contains right top (5,-1)", c.contains(top1), true);
  TEST("contains left top (-3,5)", c.contains(top2), true);

  npt = vgl_homg_point_2d<double>(-7, 8, 1);
  dst = vgl_homg_operators_2d<double>::distance_squared(c, npt);
  TEST_NEAR("distance point to hyperbola (inside)", dst, 25, 1e-6);
  lines = vgl_homg_operators_2d<double>::tangent_from(c, npt);
  TEST("tangent lines count = 0", lines.size(), 0);
  npt = vgl_homg_point_2d<double>(-2, 7, 2);
  dst = vgl_homg_operators_2d<double>::distance_squared(c, npt);
  TEST_NEAR("distance point to hyperbola (outside)", dst, 6.25, 1e-6);
  lines = vgl_homg_operators_2d<double>::tangent_from(c, npt);
  TEST("tangent lines count = 2", lines.size(), 2);
  TEST_NEAR("first tangent line", lines.front(), vgl_homg_line_2d<double>(0.5442245293, -0.1302215668, 1), 1e-6);
  TEST_NEAR("second tangent line", lines.back(), vgl_homg_line_2d<double>(0.1402140042, -0.2456531417, 1), 1e-6);

  npt = vgl_homg_operators_2d<double>::closest_point(c, top1);
  TEST("closest_point to top", npt, top1);
  npt = vgl_homg_operators_2d<double>::closest_point(c, centre);
  if (vgl_homg_operators_2d<double>::distance_squared(npt, top1) > 1)
  {
    TEST_NEAR("closest_point to centre", npt, top2, 1e-6);
  }
  else
  {
    TEST_NEAR("closest_point to centre", npt, top1, 1e-6);
  }
  npt = vgl_homg_operators_2d<double>::closest_point(c, vgl_homg_point_2d<double>(-2, 7, 2));
  TEST_NEAR("closest point (outside)", npt, top2, 1e-6);

  l = vgl_homg_line_2d<double>(1, 1, 0); // line x+y=0
  pts = vgl_homg_operators_2d<double>::intersection(c, l);
  x1 = (-9 - 20 * std::sqrt(51.0)) / 39;
  x2 = (-9 + 20 * std::sqrt(51.0)) / 39; // or interchanged
  TEST("intersection count = 2", pts.size(), 2);
  TEST_NEAR("first point", pts.front(), vgl_homg_point_2d<double>(x1, -x1, 1), 1e-6);
  TEST_NEAR("second point", pts.back(), vgl_homg_point_2d<double>(x2, -x2, 1), 1e-6);

  // 5. Test parabola
  std::cout << "\n\t=== test parabola ===\n";
  // parabola, top (1,2), orientation(4,-3), "eccentricity" -30.
  cc = vgl_conic<double>(direction, 1, 2, -30);
  c = vgl_conic<double>(9, 24, 16, -114, -52, 97);
  std::cout << c << '\n';
  TEST("conic is parabola", c.real_type(), "parabola");
  TEST("centre is (4,-3,0) (at infinity)", c.centre(), direction);
  TEST("parabola equality", cc, c);
  // Dual conic of any parabola is a conic though (0,0,1), i.e., with f()==0:
  cc = c.dual_conic();
  TEST("dual conic", cc, vgl_conic<double>(73, 53, -198, 100, -75, 0));
  // Symmetry axis is the line 3x+4y-11w=0, top is (1,2)
  top = vgl_homg_point_2d<double>(1, 2, 1);
  TEST("contains top", c.contains(top), true);
  // Tangent in top is 4x-3y+2w=0, i.e., orthogonal to axis:
  TEST("tangent in top is 4x-3y+2w=0", c.tangent_at(top), vgl_homg_line_2d<double>(4, -3, 2));

  npt = vgl_homg_point_2d<double>(-3, 5, 1); // this point lies on the symmetry axis
  dst = vgl_homg_operators_2d<double>::distance_squared(c, npt);
  TEST_NEAR("distance point to parabola (outside)", dst, 25, 1e-6);
  if (dst < 24 || dst > 26) // debugging output
  {
    std::cout << "   closest point = " << vgl_homg_operators_2d<double>::closest_point(c, npt) << '\n';
  }
  lines = vgl_homg_operators_2d<double>::tangent_from(c, npt);
  TEST("tangent lines count = 2", lines.size(), 2);
  TEST_NEAR("first tangent line", lines.front(), vgl_homg_line_2d<double>(2.662925678, 1.797755319, -1), 1e-6);
  TEST_NEAR("second tangent line", lines.back(), vgl_homg_line_2d<double>(0.0793177136, 0.2475906227, -1), 1e-6);
  npt = vgl_homg_point_2d<double>(6, 1, 2);
  dst = vgl_homg_operators_2d<double>::distance_squared(c, npt);
  TEST("distance point to parabola (inside)", dst <= 6.25, true);
  lines = vgl_homg_operators_2d<double>::tangent_from(c, npt);
  TEST("tangent lines count = 0", lines.size(), 0);

  npt = vgl_homg_operators_2d<double>::closest_point(c, top);
  TEST("closest_point to top", npt, top);
  npt = vgl_homg_operators_2d<double>::closest_point(c, c.centre());
  TEST("closest_point to centre", npt, c.centre());
  npt = vgl_homg_operators_2d<double>::closest_point(c, vgl_homg_point_2d<double>(-3, 5, 1));
  TEST_NEAR("closest point (outside)", npt, top, 1e-6);
  if (vgl_homg_operators_2d<double>::distance_squared(npt, top) > 0.1) // debugging output
  {
    std::cout << "   closest point = " << npt << '\n';
  }

  l = vgl_homg_line_2d<double>(1, -1, 0); // line x=y
  pts = vgl_homg_operators_2d<double>::intersection(c, l);
  x1 = (83 + 2 * std::sqrt(534.0)) / 49;
  x2 = (83 - 2 * std::sqrt(534.0)) / 49; // or interchanged
  TEST("intersection count = 2", pts.size(), 2);
  TEST_NEAR("first point", pts.front(), vgl_homg_point_2d<double>(x1, x1, 1), 1e-6);
  TEST_NEAR("second point", pts.back(), vgl_homg_point_2d<double>(x2, x2, 1), 1e-6);

  l = vgl_homg_line_2d<double>(3, 4, 0); // line parallel to the symmetry axis
  pts = vgl_homg_operators_2d<double>::intersection(c, l);
  TEST("intersection count = 2", pts.size(), 2);
  if (pts.front().w() == 0)
  {
    TEST_NEAR("first point is centre", pts.front(), vgl_homg_point_2d<double>(4, -3, 0), 1e-6);
    TEST_NEAR("second point", pts.back(), vgl_homg_point_2d<double>(388, -291, 300), 1e-6);
  }
  else
  { // or interchanged
    TEST_NEAR("second point is centre", pts.back(), vgl_homg_point_2d<double>(4, -3, 0), 1e-6);
    TEST_NEAR("first point", pts.front(), vgl_homg_point_2d<double>(388, -291, 300), 1e-6);
  }

  // 6. Test imaginary circle
  std::cout << "\n\t=== test imaginary circle ===\n";
  // imaginary circle, centre (1,2), radius 5i.
  c = vgl_conic<double>(1, 0, 1, -2, -4, 30);
  std::cout << c << '\n';
  TEST("conic is imaginary circle", c.real_type(), "imaginary circle");
  TEST("centre is (1,2)", c.centre(), vgl_homg_point_2d<double>(centre));

  // 7. Test imaginary ellipse
  std::cout << "\n\t=== test imaginary ellipse ===\n";
  // imaginary ellipse, centre (1,2), axes lengths 5i, 10i, orientation (1,0).
  c = vgl_conic<double>(4, 0, 1, -8, -4, 108);
  std::cout << c << '\n';
  TEST("conic is imaginary ellipse", c.real_type(), "imaginary ellipse");
  TEST("centre is (1,2)", c.centre(), vgl_homg_point_2d<double>(centre));

  // 8. Degenerate conics

  // a. real parallel lines:
  std::cout << "\n\t=== test 2 real parallel lines ===\n";
  vgl_homg_line_2d<double> l1(1, 2, 3), l2(1, 2, 5);
  c = vgl_conic<double>(1, 4, 4, 8, 16, 15);
  std::cout << c << '\n';
  TEST("conic is 2 real parallel lines", c.real_type(), "real parallel lines");
  TEST("centre is common point at infinity", c.centre(), vgl_homg_point_2d<double>(-2, 1, 0));
  lines = c.components();
  TEST("2 components", lines.size(), 2);
  TEST("first component is x+2y+3w=0", lines.front(), l1);
  TEST("second component is x+2y+5w=0", lines.back(), l2); // or vice versa
  lines = vgl_homg_operators_2d<double>::tangent_from(c, c.centre());
  TEST("tangent lines count from centre = 2", lines.size(), 2);
  npt = vgl_homg_point_2d<double>(3, 1, -1);
  TEST("contains (-3,-1)", c.contains(npt), true);
  lines = vgl_homg_operators_2d<double>::tangent_from(c, npt);
  TEST("tangent lines count from point on degenerate conic = 1", lines.size(), 1);
  TEST("tangent line", lines.front(), l2);
  npt = vgl_homg_point_2d<double>(0, 1, 1);
  TEST("!contains (-3,-1)", c.contains(npt), false);
  lines = vgl_homg_operators_2d<double>::tangent_from(c, npt);
  TEST("tangent lines count from other point = 0", lines.size(), 0);

  // b. imaginary parallel lines:
  std::cout << "\n\t=== test 2 imaginary parallel lines ===\n";
  c = vgl_conic<double>(4, 4, 1, 0, 0, 9); // lines 2x+y+/-3iw=0
  std::cout << c << '\n';
  TEST("conic is 2 complex conjugate parallel lines", c.real_type(), "complex parallel lines");
  TEST("centre is common point at infinity", c.centre(), vgl_homg_point_2d<double>(1, -2, 0));
  lines = c.components();
  TEST("0 components", lines.size(), 0);
  lines = vgl_homg_operators_2d<double>::tangent_from(c, c.centre());
  TEST("tangent lines count from centre = 0", lines.size(), 0); // tangents are imaginary
  npt = vgl_homg_point_2d<double>(3, 1, 1);
  TEST("!contains (3,1)", c.contains(npt), false);

  // c. finite coincident lines
  std::cout << "\n\t=== test 2 finite coincident lines ===\n";
  l1 = vgl_homg_line_2d<double>(1, 2, 3);
  c = vgl_conic<double>(1, 4, 4, 6, 12, 9);
  std::cout << c << '\n';
  TEST("conic is 2 coincident lines", c.real_type(), "coincident lines");
  lines = c.components();
  TEST("2 components", lines.size(), 2);
  TEST("first component is x+2y+3w=0", lines.front(), l1);
  TEST("second component is x+2y+3w=0", lines.back(), l1);
  npt = vgl_homg_point_2d<double>(1, 1, -1);
  TEST("contains (-1,-1)", c.contains(npt), true);
  lines = vgl_homg_operators_2d<double>::tangent_from(c, npt);
  TEST("tangent lines count from point on degenerate conic = 2", lines.size(), 2);
  TEST("tangent line", lines.front(), l1);
  TEST("tangent line", lines.back(), l1);
  npt = vgl_homg_point_2d<double>(0, 1, 1);
  TEST("!contains (0,1)", c.contains(npt), false);
  lines = vgl_homg_operators_2d<double>::tangent_from(c, npt);
  TEST("tangent lines count from other point = 0", lines.size(), 0);

  // d. infinite coincident lines
  std::cout << "\n\t=== test 2 infinite coincident lines ===\n";
  l1 = vgl_homg_line_2d<double>(0, 0, 1);
  c = vgl_conic<double>(0, 0, 0, 0, 0, 1);
  std::cout << c << '\n';
  TEST("conic is 2 coincident lines lines", c.real_type(), "coincident lines");
  lines = c.components();
  TEST("2 components", lines.size(), 2);
  TEST("first component is w=0", lines.front(), l1);
  TEST("second component is w=0", lines.back(), l1);
  npt = vgl_homg_point_2d<double>(3, 1, 0);
  TEST("contains (3,1,0)", c.contains(npt), true);
  lines = vgl_homg_operators_2d<double>::tangent_from(c, npt);
  TEST("tangent lines count from point on degenerate conic = 2", lines.size(), 2);
  TEST("tangent line", lines.front(), l1);
  TEST("tangent line", lines.back(), l1);
  npt = vgl_homg_point_2d<double>(0, 1, 1);
  TEST("!contains (0,1)", c.contains(npt), false);
  lines = vgl_homg_operators_2d<double>::tangent_from(c, npt);
  TEST("tangent lines count from other point = 0", lines.size(), 0);

  // e. finite, real intersecting lines:
  std::cout << "\n\t=== test 2 finite, real intersecting lines ===\n";
  l1 = vgl_homg_line_2d<double>(1, 2, 3);
  l2 = vgl_homg_line_2d<double>(2, -1, 1);
  c = vgl_conic<double>(2, 3, -2, 7, -1, 3);
  std::cout << c << '\n';
  TEST("conic is 2 intersecting lines", c.real_type(), "real intersecting lines");
  TEST("centre is intersection point", c.centre(), vgl_homg_point_2d<double>(1, 1, -1));
  lines = c.components();
  TEST("2 components", lines.size(), 2);
  TEST("first component is 2x-y+w=0", lines.front(), l2);
  TEST("second component is x+2y+3w=0", lines.back(), l1); // or vice versa
  lines = vgl_homg_operators_2d<double>::tangent_from(c, c.centre());
  TEST("tangent lines count from centre = 2", lines.size(), 2);
  npt = vgl_homg_point_2d<double>(0, 1, 1);
  TEST("contains (0,1)", c.contains(npt), true);
  lines = vgl_homg_operators_2d<double>::tangent_from(c, npt);
  TEST("tangent lines count from point on degenerate conic = 1", lines.size(), 1);
  TEST("tangent line", lines.front(), l2);
  npt = vgl_homg_point_2d<double>(1, 1, 0);
  TEST("!contains (1,1,0)", c.contains(npt), false);
  lines = vgl_homg_operators_2d<double>::tangent_from(c, npt);
  TEST("tangent lines count from other point = 0", lines.size(), 0);

  // e'. finite, real intersecting lines, parallel to X and Y axis:
  std::cout << "\n\t=== test horizontal & vertical intersecting lines ===\n";
  l1 = vgl_homg_line_2d<double>(1, 0, -1);
  l2 = vgl_homg_line_2d<double>(0, 1, -2);
  c = vgl_conic<double>(0, 1, 0, -2, -1, 2);
  std::cout << c << '\n';
  TEST("conic is 2 intersecting lines", c.real_type(), "real intersecting lines");
  TEST("centre is intersection point", c.centre(), vgl_homg_point_2d<double>(1, 2, 1));
  lines = c.components();
  TEST("2 components", lines.size(), 2);
  TEST("first component is x-w=0", lines.front(), l1);
  TEST("second component is y-2w=0", lines.back(), l2); // or vice versa
  lines = vgl_homg_operators_2d<double>::tangent_from(c, c.centre());
  TEST("tangent lines count from centre = 2", lines.size(), 2);
  npt = vgl_homg_point_2d<double>(0, 2, 1);
  TEST("contains (0,2)", c.contains(npt), true);
  lines = vgl_homg_operators_2d<double>::tangent_from(c, npt);
  TEST("tangent lines count from point on degenerate conic = 1", lines.size(), 1);
  TEST("tangent line", lines.front(), l2);
  npt = vgl_homg_point_2d<double>(2, 1, 0);
  TEST("!contains (2,1,0)", c.contains(npt), false);
  lines = vgl_homg_operators_2d<double>::tangent_from(c, npt);
  TEST("tangent lines count from other point = 0", lines.size(), 0);

  // f. imaginary intersecting lines:
  std::cout << "\n\t=== test 2 imaginary intersecting lines ===\n";
  c = vgl_conic<double>(4, 0, 1, 12, 0, 9); // lines 2x+/-iy+3w=0
  std::cout << c << '\n';
  TEST("conic is 2 complex conjugate intersecting lines", c.real_type(), "complex intersecting lines");
  TEST("centre is intersection point", c.centre(), vgl_homg_point_2d<double>(3, 0, -2));
  lines = c.components();
  TEST("0 components", lines.size(), 0);
  npt = vgl_homg_point_2d<double>(0, 1, 1);
  TEST("!contains (0,1)", c.contains(npt), false);
  lines = vgl_homg_operators_2d<double>::tangent_from(c, c.centre());
  TEST("tangent lines count from centre = 0", lines.size(), 0); // tangents are imaginary
  std::cout << vgl_homg_operators_2d<double>::compute_bounding_box(c) << std::endl;
  TEST("bounding box", vgl_homg_operators_2d<double>::compute_bounding_box(c), vgl_box_2d<double>(-1.5, -1.5, 0, 0));

  // g. one finite and one infinite intersecting line:
  std::cout << "\n\t=== test intersecting lines, 1 infinite ===\n";
  l1 = vgl_homg_line_2d<double>(1, 2, 3);
  l2 = vgl_homg_line_2d<double>(0, 0, 1);
  c = vgl_conic<double>(0, 0, 0, 1, 2, 3);
  std::cout << c << '\n';
  TEST("conic is 2 intersecting lines", c.real_type(), "real intersecting lines");
  TEST("centre is point at infinity", c.centre(), vgl_homg_point_2d<double>(-2, 1, 0));
  lines = c.components();
  TEST("2 components", lines.size(), 2);
  TEST("first component is w=0", lines.front(), l2);
  TEST("second component is x+2y+3w=0", lines.back(), l1); // or vice versa
  lines = vgl_homg_operators_2d<double>::tangent_from(c, c.centre());
  TEST("tangent lines count from centre = 2", lines.size(), 2);
  npt = vgl_homg_point_2d<double>(3, 0, -1);
  TEST("contains (-3,0)", c.contains(npt), true);
  lines = vgl_homg_operators_2d<double>::tangent_from(c, npt);
  TEST("tangent lines count from point on degenerate conic = 1", lines.size(), 1);
  TEST("tangent line", lines.front(), l1);
  npt = vgl_homg_point_2d<double>(0, 1, 1);
  TEST("!contains (0,1)", c.contains(npt), false);
  lines = vgl_homg_operators_2d<double>::tangent_from(c, npt);
  TEST("tangent lines count from other point = 0", lines.size(), 0);
  TEST_NEAR("curvature is zero", c.curvature_at(vgl_point_2d<double>(5.0, -8.0)), 0.0, 1e-6);

  // Intersections
  c = vgl_conic<double>(centre, 3, 3, 0.0);  // circle
  cc = vgl_conic<double>(centre, 1, 1, 0.0); // concentric circle
  pts = vgl_homg_operators_2d<double>::intersection(c, cc);
  TEST("intersection count of concentric circles = 0", pts.size(), 0);
  for (it = pts.begin(); it != pts.end(); ++it)
    std::cout << (*it) << '\n';
  check_points_on_conics(c, cc, pts);

  cc = vgl_conic<double>(vgl_homg_point_2d<double>(1, 1, 1), 1, 1, 0.0); // non-concentric circle
  pts = vgl_homg_operators_2d<double>::intersection(c, cc);
  TEST("intersection count of non-concentric circles = 0", pts.size(), 0);
  for (it = pts.begin(); it != pts.end(); ++it)
    std::cout << (*it) << '\n';
  check_points_on_conics(c, cc, pts);

  cc = vgl_conic<double>(vgl_homg_point_2d<double>(1, 5, 1), 1, 1, 0.0); // intersecting circle
  pts = vgl_homg_operators_2d<double>::intersection(c, cc);
  TEST("intersection count of intersecting circles = 2", pts.size(), 2);
  for (it = pts.begin(); it != pts.end(); ++it)
    std::cout << (*it) << '\n';
  check_points_on_conics(c, cc, pts);

  cc = vgl_conic<double>(vgl_homg_point_2d<double>(1, 5, 1), 3, 3, 0.0); // intersecting circle
  pts = vgl_homg_operators_2d<double>::intersection(c, cc);
  TEST("intersection count of intersecting circles = 2", pts.size(), 2);
  for (it = pts.begin(); it != pts.end(); ++it)
    std::cout << (*it) << '\n';
  check_points_on_conics(c, cc, pts);

  cc = vgl_conic<double>(vgl_homg_point_2d<double>(1, 1, 1), 2, 2, 0.0); // internal tangent circle
  pts = vgl_homg_operators_2d<double>::intersection(c, cc);
  TEST("intersection count of touching circles = 2", pts.size(), 2);
  for (it = pts.begin(); it != pts.end(); ++it)
    std::cout << (*it) << '\n';
  TEST("1st intersection point = (1,-1)", pts.front(), vgl_homg_point_2d<double>(1, -1, 1));
  TEST("2nd intersection point coincides", pts.back(), pts.front());
  check_points_on_conics(c, cc, pts);

  cc = vgl_conic<double>(vgl_homg_point_2d<double>(-3, 2, 1), 1, 1, 0.0); // external tangent circle
  pts = vgl_homg_operators_2d<double>::intersection(c, cc);
  TEST("intersection count of touching circles = 2", pts.size(), 2);
  for (it = pts.begin(); it != pts.end(); ++it)
    std::cout << (*it) << '\n';
  TEST("1st intersection point = (-2,2)", pts.front(), vgl_homg_point_2d<double>(-2, 2, 1));
  TEST("2nd intersection point coincides", pts.back(), pts.front());
  check_points_on_conics(c, cc, pts);

  cc = vgl_conic<double>(centre, 3, 1, 0.0); // concentric touching ellipse
  pts = vgl_homg_operators_2d<double>::intersection(c, cc);
  TEST("intersection of touching ellipses = 2x 2 coincident points", pts.size(), 4);
  for (it = pts.begin(); it != pts.end(); ++it)
    std::cout << (*it) << '\n';
  TEST("1st  intersection point = (4,2)", pts.front(), vgl_homg_point_2d<double>(4, 2, 1));
  TEST("last intersection point = (-2,2)", pts.back(), vgl_homg_point_2d<double>(-2, 2, 1));
  check_points_on_conics(c, cc, pts);

  cc = vgl_conic<double>(centre, 4, 2, 0.0); // concentric intersecting ellipse
  pts = vgl_homg_operators_2d<double>::intersection(c, cc);
  TEST("intersection of intersecting ellipses = 4 points", pts.size(), 4);
  for (it = pts.begin(); it != pts.end(); ++it)
    std::cout << (*it) << '\n';
  check_points_on_conics(c, cc, pts);

  c = vgl_conic<double>(centre, 1, 2, 1.5); // arbitrary, concentric ellipses
  cc = vgl_conic<double>(centre, 1, 3, 3.0);
  pts = vgl_homg_operators_2d<double>::intersection(c, cc);
  TEST("intersection of intersecting ellipses = 4 points", pts.size(), 4);
  for (it = pts.begin(); it != pts.end(); ++it)
    std::cout << (*it) << '\n';
  check_points_on_conics(c, cc, pts);

  // concentric ellipses with intersection points having same y value
  c.set(16, -50, 100, 0, 0, -25);
  c.translate_by(-1, -2);
  cc.set(16, 50, 100, 0, 0, -25);
  cc.translate_by(-1, -2);
  pts = vgl_homg_operators_2d<double>::intersection(c, cc);
  // intersection points should be: (1, 1.5), (1, 2.5), (2.25, 2), and (-0.25, 2).
  TEST("intersection of mirrored ellipses = 4 different, but symmetrically positioned points", pts.size(), 4);
  for (it = pts.begin(); it != pts.end(); ++it)
    std::cout << (*it) << '\n';
  check_points_on_conics(c, cc, pts);

  // circles with intersection points having same y value
  c = vgl_conic<double>(vgl_homg_point_2d<double>(4, 4, 1), 5, 5, 0.0);
  cc = vgl_conic<double>(vgl_homg_point_2d<double>(4, 0, 1), 3, 3, 0.0);
  pts = vgl_homg_operators_2d<double>::intersection(c, cc);
  // intersection points should be: (1, 0) and (7, 0).
  TEST("intersection of circles = 2 points with same y coordinate", pts.size(), 2);
  TEST("intersection points are different", pts.front() == pts.back(), false);
  check_points_on_conics(c, cc, pts);

  c = vgl_conic<double>(centre, 3, -3, 0.0);  // orthogonal hyperbola
  cc = vgl_conic<double>(centre, 1, -1, 0.0); // concentric hyperbola with same asymptotes
  pts = vgl_homg_operators_2d<double>::intersection(c, cc);
  TEST("intersection of concentric orthogonal hyperbolas = 2x 2 points at infinity", pts.size(), 4);
  for (it = pts.begin(); it != pts.end(); ++it)
    std::cout << (*it) << '\n';
  TEST("1st  intersection point is on asymptote x+y=0", pts.front(), vgl_homg_point_2d<double>(1, -1, 0));
  TEST("last intersection point is at other asymptote x=y", pts.back(), vgl_homg_point_2d<double>(1, 1, 0));
  check_points_on_conics(c, cc, pts);

  cc = vgl_conic<double>(vgl_homg_point_2d<double>(1, 5, 1), 1, -1, 0.0); // intersecting hyperbola
  pts = vgl_homg_operators_2d<double>::intersection(c, cc);
  TEST("intersection count of intersecting hyperbolas = 4", pts.size(), 4);
  for (it = pts.begin(); it != pts.end(); ++it)
    std::cout << (*it) << '\n';
  TEST("1st intersection point is on asymptote x+y=0", pts.front(), vgl_homg_point_2d<double>(1, -1, 0));
  check_points_on_conics(c, cc, pts);

  cc = vgl_conic<double>(vgl_homg_point_2d<double>(1, 5, 1), 3, -1, 0.0); // intersecting hyperbola
  pts = vgl_homg_operators_2d<double>::intersection(c, cc);
  TEST("intersection count of intersecting hyperbolas = 4", pts.size(), 4);
  bool infi = false;
  for (it = pts.begin(); it != pts.end(); ++it)
  {
    std::cout << (*it) << '\n';
    if ((*it).w() == 0)
      infi = true;
  }
  TEST("intersection points are not at infinity", infi, false);
  check_points_on_conics(c, cc, pts);

  cc = vgl_conic<double>(centre, 3, 3, 0.0); // concentric touching circle
  pts = vgl_homg_operators_2d<double>::intersection(c, cc);
  TEST("intersection of hyperbola with concentric touching circle = 2x 2 points", pts.size(), 4);
  for (it = pts.begin(); it != pts.end(); ++it)
    std::cout << (*it) << '\n';
  TEST("1st  intersection point = (-2,2)", pts.front(), vgl_homg_point_2d<double>(-2, 2, 1));
  TEST("last intersection point = (4,2)", pts.back(), vgl_homg_point_2d<double>(4, 2, 1));
  check_points_on_conics(c, cc, pts);

  cc = vgl_conic<double>(centre, 2, 2, 0.0); // concentric smaller circle
  pts = vgl_homg_operators_2d<double>::intersection(c, cc);
  TEST("intersection of hyperbola with concentric smaller circle = empty", pts.size(), 0);
  for (it = pts.begin(); it != pts.end(); ++it)
    std::cout << (*it) << '\n';
  check_points_on_conics(c, cc, pts);

  cc = vgl_conic<double>(centre, 4, 4, 0.0); // concentric larger circle
  pts = vgl_homg_operators_2d<double>::intersection(c, cc);
  TEST("intersection of hyperbola with concentric larger circle = 4 points", pts.size(), 4);
  for (it = pts.begin(); it != pts.end(); ++it)
    std::cout << (*it) << '\n';
  check_points_on_conics(c, cc, pts);
}

TESTMAIN(test_conic);
