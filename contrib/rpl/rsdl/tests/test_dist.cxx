#include <iostream>
#include <cmath>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h>
#include <testlib/testlib_test.h>

#include <rsdl/rsdl_point.h>
#include <rsdl/rsdl_bounding_box.h>
#include <rsdl/rsdl_dist.h>

static void test_dist()
{
  double p_cart_arr[] = { 3.4, 4.5 };
  double q_cart_arr[] = { -1.4, 7.6 };
  double p_ang_arr[] = { vnl_math::pi/4,  vnl_math::pi/8, -vnl_math::pi };
  double q_ang_arr[] = { vnl_math::pi/8, -vnl_math::pi,   3*vnl_math::pi/4 };

  std::vector<double> p_cart;
  std::vector<double> q_cart;
  std::vector<double> p_ang;
  std::vector<double> q_ang;
  for (double i : p_cart_arr)  p_cart.push_back( i );
  for (double i : q_cart_arr)  q_cart.push_back( i );
  for (double i : p_ang_arr)  p_ang.push_back( i );
  for (double i : q_ang_arr)  q_ang.push_back( i );

  double sq_dist =
    vnl_math::sqr( 3.4 + 1.4 )
    + vnl_math::sqr( 4.5-7.6 )
    + vnl_math::sqr( vnl_math::pi/8 )
    + vnl_math::sqr( 7*vnl_math::pi/8 )
    + vnl_math::sqr( vnl_math::pi/4 );
  rsdl_point p( p_cart.begin(), p_cart.end(), p_ang.begin(), p_ang.end() );
  rsdl_point q( q_cart.begin(), q_cart.end(), q_ang.begin(), q_ang.end() );

  TEST_NEAR("square distance between points", sq_dist, rsdl_dist_sq( p, q ), 1e-6);
  TEST_NEAR("distance between points", std::sqrt(sq_dist), rsdl_dist( p, q ), 1e-6);

  p_cart[0] = -1.5;  q_cart[0] = 2.5;
  p_cart[1] =  6.0;  q_cart[1] = 3.2;
  p_ang[0] = vnl_math::pi/8;   q_ang[0] = vnl_math::pi/4;   // non-wrap
  p_ang[1] = 3*vnl_math::pi/4; q_ang[1] = vnl_math::pi/4;   // wraps around 0
  p_ang[2] = 7*vnl_math::pi/5; q_ang[2] = 9*vnl_math::pi/5; // non-wrap
  p.set_cartesian( p_cart.begin() ); p.set_angular( p_ang.begin() );
  q.set_cartesian( q_cart.begin() ); q.set_angular( q_ang.begin() );
  rsdl_bounding_box box( p, q );

  double cart0_arr[] = { 1, 4.0 };
  double ang0_arr[] = { 3*vnl_math::pi/16, vnl_math::pi, 8*vnl_math::pi/5 };
  std::vector<double> cart0;
  std::vector<double> ang0;
  for (double i : cart0_arr)  cart0.push_back( i );
  for (double i : ang0_arr)  ang0.push_back( i );

  rsdl_point test_p( cart0.begin(), cart0.end(), ang0.begin(), ang0.end() );
  TEST_NEAR("point-to-box dist sq - point inside", rsdl_dist_sq( test_p, box), 0, 1e-6);
  TEST_NEAR("point-to-box dist - point inside", rsdl_dist( test_p, box), 0, 1e-6);

  cart0[0] = -2.5; cart0[1] = 7.5;
  test_p.set_cartesian( cart0.begin() );
  TEST_NEAR("point-to-box dist sq - point outside cartesian", rsdl_dist_sq( test_p, box), 3.25, 1e-6);
  TEST_NEAR("point-to-box dist sq - point outside cartesian", rsdl_dist( test_p, box), std::sqrt(3.25), 1e-6);

  cart0[0] = 1;  cart0[1] = 4;  // cartesian back inside
  test_p.set_cartesian( cart0.begin() );
  ang0[0] = vnl_math::pi/16;    // non-wrap, below
  ang0[1] = 5*vnl_math::pi/8;   // wrap, closer to "min" (larger angle value)
  ang0[2] = vnl_math::pi/5;     // non-wrap, below, but closer to above value
  test_p.set_angular( ang0.begin() );
  sq_dist = vnl_math::sqr( vnl_math::pi/16 ) + vnl_math::sqr( vnl_math::pi/8 ) +
            vnl_math::sqr( vnl_math::twopi/5 );
  TEST_NEAR("point-to-box dist sq - angle outside (a)", rsdl_dist_sq( test_p, box), sq_dist , 1e-6);
  TEST_NEAR("point-to-box dist - angle outside (a) ", rsdl_dist( test_p, box), std::sqrt(sq_dist), 1e-6);

  ang0[0] = 15*vnl_math::pi/8;    // non-wrap, above, but closer to min value
  ang0[1] = 3*vnl_math::pi/8;    // wrap, closer to "max" (smaller angle value)
  ang0[2] = 19*vnl_math::pi/10;  // non-wrap, above
  test_p.set_angular( ang0.begin() );
  sq_dist = vnl_math::sqr( vnl_math::pi/4 ) + vnl_math::sqr( vnl_math::pi/8 ) +
    vnl_math::sqr( vnl_math::pi/10 );
  TEST_NEAR("point-to-box dist sq - angle outside (b)", rsdl_dist_sq( test_p, box), sq_dist , 1e-6);
  TEST_NEAR("point-to-box dist - angle outside (b) ", rsdl_dist( test_p, box), std::sqrt(sq_dist), 1e-6);


  //  Now testing box intersection code.  This will take a bit because
  //  of the number of conditions in the rsdl_dist_box_relation.
  //  We'll start with 1D boxes (intervals) to check the basic logic.

  //  Cartesian first
  rsdl_point min_c(1,0), max_c(1,0);
  min_c.cartesian(0) = -1.5; max_c.cartesian(0) = 4.5;
  rsdl_bounding_box outer_c( min_c, max_c );
  rsdl_bounding_box inner_c( min_c, max_c );
  bool inside, intersects;

  // below/below (nothing)
  inner_c.min_cartesian(0) = -3.0; inner_c.max_cartesian(0) = -1.9;
  rsdl_dist_box_relation( inner_c, outer_c, inside, intersects );
  TEST("rsdl_dist_box_relation - cartesian only (a)", !inside && !intersects, true);

  // below/between (intersects)
  inner_c.min_cartesian(0) = -3.0; inner_c.max_cartesian(0) = -1.0;
  rsdl_dist_box_relation( inner_c, outer_c, inside, intersects );
  TEST("rsdl_dist_box_relation - cartesian only (b)", !inside && intersects, true);

  // below/above (intersects)
  inner_c.min_cartesian(0) = -20.0; inner_c.max_cartesian(0) = 20.0;
  rsdl_dist_box_relation( inner_c, outer_c, inside, intersects );
  TEST("rsdl_dist_box_relation - cartesian only (c)", !inside && intersects, true);

  // between/between (inside and intersects)
  inner_c.min_cartesian(0) = -1.0; inner_c.max_cartesian(0) = 1.0;
  rsdl_dist_box_relation( inner_c, outer_c, inside, intersects );
  TEST("rsdl_dist_box_relation - cartesian only (d)", inside && intersects, true);

  // between/above (intersects)
  inner_c.min_cartesian(0) = -1.0; inner_c.max_cartesian(0) = 8.0;
  rsdl_dist_box_relation( inner_c, outer_c, inside, intersects );
  TEST("rsdl_dist_box_relation - cartesian only (e)", !inside && intersects, true);

  // above/above (nothing)
  inner_c.min_cartesian(0) = 7.0; inner_c.max_cartesian(0) = 8.0;
  rsdl_dist_box_relation( inner_c, outer_c, inside, intersects );
  TEST("rsdl_dist_box_relation - cartesian only (f)", !inside && !intersects, true);

  //  Angular next.  Start with neither interval wrapping around (6 tests)
  rsdl_point min_a(0,1), max_a(0,1);
  min_a.angular(0) = -vnl_math::pi/2; max_a.cartesian(0) = vnl_math::pi/2;
  rsdl_bounding_box outer_a( min_a, max_a );
  rsdl_bounding_box inner_a( min_a, max_a );

  //  both below (nothing)
  inner_a.min_cartesian(0) = -vnl_math::pi; inner_a.max_cartesian(0) = -3*vnl_math::pi/4;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  TEST("rsdl_dist_box_relation - angular only (1a)", !inside && !intersects, true);

  //  below / between (intersects)
  inner_a.min_cartesian(0) = -vnl_math::pi; inner_a.max_cartesian(0) = -vnl_math::pi/4;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  TEST("rsdl_dist_box_relation - angular only (1b)", !inside && intersects, true);

  //  below / above (intersects)
  inner_a.min_cartesian(0) = -vnl_math::pi; inner_a.max_cartesian(0) = vnl_math::pi;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  TEST("rsdl_dist_box_relation - angular only (1c)", !inside && intersects, true);

  // between / between (inside and intersects)
  inner_a.min_cartesian(0) = -vnl_math::pi/4; inner_a.max_cartesian(0) = vnl_math::pi/4;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  TEST("rsdl_dist_box_relation - angular only (1d)", inside && intersects, true);

  // between / above (intersects)
  inner_a.min_cartesian(0) = 0; inner_a.max_cartesian(0) = vnl_math::pi;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  TEST("rsdl_dist_box_relation - angular only (1e)", !inside && intersects, true);

  // above / above (nothing)
  inner_a.min_cartesian(0) = vnl_math::pi; inner_a.max_cartesian(0) = 1.5*vnl_math::pi;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  TEST("rsdl_dist_box_relation - angular only (1f)", !inside && !intersects, true);

  //
  //  Now, outer does not wrap around, but inner does
  //

  outer_a.min_cartesian(0) = vnl_math::pi/2; outer_a.max_cartesian(0) = 3*vnl_math::pi/2;

  // below / below   (intersects because of wrap-around)
  inner_a.min_cartesian(0) = vnl_math::pi/4; inner_a.max_cartesian(0) = vnl_math::pi/8;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  TEST("rsdl_dist_box_relation - angular only (2a)", !inside && intersects, true);

  //  between / below (intersects)
  inner_a.min_cartesian(0) = vnl_math::pi; inner_a.max_cartesian(0) = vnl_math::pi/8;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  TEST("rsdl_dist_box_relation - angular only (2b)", !inside && intersects, true);

  // above / below (nothing)
  inner_a.min_cartesian(0) = 7*vnl_math::pi/4; inner_a.max_cartesian(0) = vnl_math::pi/8;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  TEST("rsdl_dist_box_relation - angular only (2c)", !inside && !intersects, true);

  // between / between (intersects)
  inner_a.min_cartesian(0) = vnl_math::pi; inner_a.max_cartesian(0) = 3*vnl_math::pi/4;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  TEST("rsdl_dist_box_relation - angular only (2d)", !inside && intersects, true);

  // above / between (intersects)
  inner_a.min_cartesian(0) = 7*vnl_math::pi/4; inner_a.max_cartesian(0) = vnl_math::pi;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  TEST("rsdl_dist_box_relation - angular only (2e)", !inside && intersects, true);

  // above / above (intersects)
  inner_a.min_cartesian(0) = 7*vnl_math::pi/4; inner_a.max_cartesian(0) = 1.6*vnl_math::pi;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  TEST("rsdl_dist_box_relation - angular only (2f)", !inside && intersects, true);


  //  Now, outer wraps around, but inner does not
  outer_a.min_cartesian(0) = 3*vnl_math::pi/2; outer_a.max_cartesian(0) = vnl_math::pi/2;

  //  below / below (inside and intersects)
  inner_a.min_cartesian(0) = vnl_math::pi/8; inner_a.max_cartesian(0) = vnl_math::pi/4;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  TEST("rsdl_dist_box_relation - angular only (3a)", inside && intersects, true);

  //  below / between (intersects)
  inner_a.min_cartesian(0) = vnl_math::pi/8; inner_a.max_cartesian(0) = 3*vnl_math::pi/4;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  TEST("rsdl_dist_box_relation - angular only (3b)", !inside && intersects, true);

  //  below / above (intersects)
  inner_a.min_cartesian(0) = vnl_math::pi/8; inner_a.max_cartesian(0) = 7*vnl_math::pi/4;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  TEST("rsdl_dist_box_relation - angular only (3c)", !inside && intersects, true);

  //  between / between (nothing)
  inner_a.min_cartesian(0) = 7*vnl_math::pi/8; inner_a.max_cartesian(0) = vnl_math::pi;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  TEST("rsdl_dist_box_relation - angular only (3d)", !inside && !intersects, true);

  //  between / above (intersects)
  inner_a.min_cartesian(0) = 7*vnl_math::pi/8; inner_a.max_cartesian(0) = 7*vnl_math::pi/4;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  TEST("rsdl_dist_box_relation - angular only (3e)", !inside && intersects, true);

  //  above / above (inside and intersects)
  inner_a.min_cartesian(0) = 3*vnl_math::pi/2; inner_a.max_cartesian(0) = 7*vnl_math::pi/4;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  TEST("rsdl_dist_box_relation - angular only (3f)", inside && intersects, true);


 //  Now, both wrap around...
  outer_a.min_cartesian(0) = 3*vnl_math::pi/2; outer_a.max_cartesian(0) = vnl_math::pi/2;

  //  below / below (intersect)
  inner_a.min_cartesian(0) = vnl_math::pi/4; inner_a.max_cartesian(0) = vnl_math::pi/8;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  TEST("rsdl_dist_box_relation - angular only (4a)", !inside && intersects, true);

  //  between / below (intersect)
  inner_a.min_cartesian(0) = vnl_math::pi; inner_a.max_cartesian(0) = vnl_math::pi/8;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  TEST("rsdl_dist_box_relation - angular only (4b)", !inside && intersects, true);

  //  above / below (inside and intersect)
  inner_a.min_cartesian(0) = 7*vnl_math::pi/4; inner_a.max_cartesian(0) = vnl_math::pi/8;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  TEST("rsdl_dist_box_relation - angular only (4c)", inside && intersects, true);

  //  between / between (intersect)
  inner_a.min_cartesian(0) = 9*vnl_math::pi/8; inner_a.max_cartesian(0) = vnl_math::pi;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  TEST("rsdl_dist_box_relation - angular only (4d)", intersects, true);

  //  above / between (intersect)
  inner_a.min_cartesian(0) = 7*vnl_math::pi/4; inner_a.max_cartesian(0) = vnl_math::pi;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  TEST("rsdl_dist_box_relation - angular only (4e)", intersects, true);

  //  above / above (intersect)
  inner_a.min_cartesian(0) = 7*vnl_math::pi/4; inner_a.max_cartesian(0) = 13*vnl_math::pi/8;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  TEST("rsdl_dist_box_relation - angular only (4f)", intersects, true);

  //  Test for control logic inside  rsdl_dist_box_relation
  const double pi = vnl_math::pi;
  rsdl_point o_min( 2, 3 ), o_max( 2, 3 );
  o_min.cartesian(0) = 1.5;  o_max.cartesian(0) = 3.5;
  o_min.cartesian(1) = 1.5;  o_max.cartesian(1) = 4.5;
  o_min.angular(0) = pi/2;   o_max.angular(0) = 3*pi/2;
  o_min.angular(1) = 7*pi/4; o_max.angular(1) = pi;
  o_min.angular(2) = 7*pi/4; o_max.angular(2) = pi;
  rsdl_bounding_box outer( o_min, o_max );

  rsdl_point i_min( 2, 3 ), i_max( 2, 3 );
  i_min.cartesian(0) = 1.75; i_max.cartesian(0) = 3.0;
  i_min.cartesian(1) = 2.0;  i_max.cartesian(1) = 3.5;
  i_min.angular(0) = 3*pi/4; i_max.angular(0) = pi;
  i_min.angular(1) = 7.1*pi/4; i_max.angular(1) = pi/2;
  i_min.angular(2) = 7.1*pi/4; i_max.angular(2) = pi/2;
  rsdl_bounding_box inner( i_min, i_max );

  //  succeeds on inside and intersects
  rsdl_dist_box_relation( inner, outer, inside, intersects );
  TEST("rsdl_dist_box_relation - complete (a)", inside && intersects, true);

  // inside fails on cartesian
  inner.min_cartesian(0) = 1.25;
  rsdl_dist_box_relation( inner, outer, inside, intersects );
  TEST("rsdl_dist_box_relation - complete (b)", !inside && intersects, true);

  // inside fails on angular
  inner.min_cartesian(0) = 1.95;
  inner.min_angular(2) = 5*pi/4;
  rsdl_dist_box_relation( inner, outer, inside, intersects );
  TEST("rsdl_dist_box_relation - complete (c)", !inside && intersects, true);

  // intersects fails on cartesian
  inner.min_cartesian(0) = 4.75; inner.max_cartesian(0) = 5.0;
  rsdl_dist_box_relation( inner, outer, inside, intersects );
  TEST("rsdl_dist_box_relation - complete (d)", !inside && !intersects, true);

  // intersects fails on angular
  inner.min_cartesian(0) = 1.95;
  inner.min_angular(0) = 0;   inner.max_angular(0) = pi/4;
  rsdl_dist_box_relation( inner, outer, inside, intersects );
  TEST("rsdl_dist_box_relation - complete (e)", !inside && !intersects, true);

  //  Now rsdl_dist_point_in_box.  Start by resetting the box boundaries
  outer.min_cartesian(0) =  0.5;  outer.max_cartesian(0) = 4.0;
  outer.min_cartesian(1) = -1.5;  outer.max_cartesian(1) = 2.0;
  outer.min_angular(0) = -pi;     outer.max_angular(0) = pi/2;
  outer.min_angular(1) = pi/2;    outer.max_angular(1) = 3*pi/2;
  outer.min_angular(2) = 3*pi/2;  outer.max_angular(2) = pi/2;

  rsdl_point pt(2,3);
  pt.cartesian(0) = 1.0;
  pt.cartesian(1) = 1.0;
  pt.angular(0) = 0;
  pt.angular(1) = pi;
  pt.angular(2) = pi/8;

  inside = rsdl_dist_point_in_box( pt, outer );
  TEST("rsdl_dist_point_in_box (a)", inside, true);

  pt.angular(2) = 7*pi/4;
  inside = rsdl_dist_point_in_box( pt, outer );
  TEST("rsdl_dist_point_in_box (b)", inside, true);

  pt.cartesian(1) = -2;
  inside = rsdl_dist_point_in_box( pt, outer );
  TEST("rsdl_dist_point_in_box (c)", !inside, true);

  pt.cartesian(1) = 1;
  pt.cartesian(0) = 5;
  inside = rsdl_dist_point_in_box( pt, outer );
  TEST("rsdl_dist_point_in_box (d)", !inside, true);

  pt.cartesian(0) = 2.5;
  inside = rsdl_dist_point_in_box( pt, outer );
  TEST("rsdl_dist_point_in_box (e)", inside, true);  // sanity check to make sure things are reset correctly

  double temp = pt.angular(0);
  pt.angular(0) = -3*pi/2;
  inside = rsdl_dist_point_in_box( pt, outer );
  pt.angular(0) = temp;
  bool back_inside = rsdl_dist_point_in_box( pt, outer );
  TEST("rsdl_dist_point_in_box (f)", !inside && back_inside, true);

  temp = pt.angular(0);
  pt.angular(0) = 3*pi/2;
  inside = rsdl_dist_point_in_box( pt, outer );
  pt.angular(0) = temp;
  back_inside = rsdl_dist_point_in_box( pt, outer );
  TEST("rsdl_dist_point_in_box (g)", !inside && back_inside, true);

  temp = pt.angular(2);
  pt.angular(2) = pi;
  inside = rsdl_dist_point_in_box( pt, outer );
  pt.angular(2) = temp;
  back_inside = rsdl_dist_point_in_box( pt, outer );
  TEST("rsdl_dist_point_in_box (h)", !inside && back_inside, true);
}

TESTMAIN(test_dist);
