#include <vcl_cmath.h>
#include <vcl_vector.h>
#include <vnl/vnl_math.h>
#include <testlib/testlib_test.h>

#include <rsdl/rsdl_point.h>
#include <rsdl/rsdl_bounding_box.h>
#include <rsdl/rsdl_dist.h>

static void test_dist()
{
  const unsigned int Nc = 2;
  const unsigned int Na = 3;

  double p_cart_arr[] = { 3.4, 4.5 };
  double q_cart_arr[] = { -1.4, 7.6 };
  double p_ang_arr[] = { vnl_math::pi/4,  vnl_math::pi/8, -vnl_math::pi };
  double q_ang_arr[] = { vnl_math::pi/8, -vnl_math::pi,   3*vnl_math::pi/4 };

  vcl_vector<double> p_cart;
  vcl_vector<double> q_cart;
  vcl_vector<double> p_ang;
  vcl_vector<double> q_ang;
  for ( unsigned int i=0; i < Nc; ++i )  p_cart.push_back( p_cart_arr[i] );
  for ( unsigned int i=0; i < Nc; ++i )  q_cart.push_back( q_cart_arr[i] );
  for ( unsigned int i=0; i < Na; ++i )  p_ang.push_back( p_ang_arr[i] );
  for ( unsigned int i=0; i < Na; ++i )  q_ang.push_back( q_ang_arr[i] );

  double sq_dist =
    vnl_math_sqr( 3.4 + 1.4 )
    + vnl_math_sqr( 4.5-7.6 )
    + vnl_math_sqr( vnl_math::pi/8 )
    + vnl_math_sqr( 7*vnl_math::pi/8 )
    + vnl_math_sqr( vnl_math::pi/4 );
  rsdl_point p( p_cart.begin(), p_cart.end(), p_ang.begin(), p_ang.end() );
  rsdl_point q( q_cart.begin(), q_cart.end(), q_ang.begin(), q_ang.end() );

  TEST_NEAR("square distance between points", sq_dist, rsdl_dist_sq( p, q ), 1e-6);
  TEST_NEAR("distance between points", vcl_sqrt(sq_dist), rsdl_dist( p, q ), 1e-6);

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
  vcl_vector<double> cart0;
  vcl_vector<double> ang0;
  for ( unsigned int i=0; i < Nc; ++i )  cart0.push_back( cart0_arr[i] );
  for ( unsigned int i=0; i < Na; ++i )  ang0.push_back( ang0_arr[i] );

  rsdl_point test_p( cart0.begin(), cart0.end(), ang0.begin(), ang0.end() );
  TEST_NEAR("point-to-box dist sq - point inside", rsdl_dist_sq( test_p, box), 0, 1e-6);
  TEST_NEAR("point-to-box dist - point inside", rsdl_dist( test_p, box), 0, 1e-6);

  cart0[0] = -2.5; cart0[1] = 7.5;
  test_p.set_cartesian( cart0.begin() );
  TEST_NEAR("point-to-box dist sq - point outside cartesian", rsdl_dist_sq( test_p, box), 3.25, 1e-6);
  TEST_NEAR("point-to-box dist sq - point outside cartesian", rsdl_dist( test_p, box), vcl_sqrt(3.25), 1e-6);

  cart0[0] = 1;  cart0[1] = 4;  // cartesian back inside
  test_p.set_cartesian( cart0.begin() );
  ang0[0] = vnl_math::pi/16;    // non-wrap, below
  ang0[1] = 5*vnl_math::pi/8;   // wrap, closer to "min" (larger angle value)
  ang0[2] = vnl_math::pi/5;     // non-wrap, below, but closer to above value
  test_p.set_angular( ang0.begin() );
  sq_dist = vnl_math_sqr( vnl_math::pi/16 ) + vnl_math_sqr( vnl_math::pi/8 ) +
    vnl_math_sqr( 2*vnl_math::pi/5 );
  TEST_NEAR("point-to-box dist sq - angle outside (a)", rsdl_dist_sq( test_p, box), sq_dist , 1e-6);
  TEST_NEAR("point-to-box dist - angle outside (a) ", rsdl_dist( test_p, box), vcl_sqrt(sq_dist), 1e-6);

  ang0[0] = 15*vnl_math::pi/8;    // non-wrap, above, but closer to min value
  ang0[1] = 3*vnl_math::pi/8;    // wrap, closer to "max" (smaller angle value)
  ang0[2] = 19*vnl_math::pi/10;  // non-wrap, above
  test_p.set_angular( ang0.begin() );
  sq_dist = vnl_math_sqr( vnl_math::pi/4 ) + vnl_math_sqr( vnl_math::pi/8 ) +
    vnl_math_sqr( vnl_math::pi/10 );
  TEST_NEAR("point-to-box dist sq - angle outside (b)", rsdl_dist_sq( test_p, box), sq_dist , 1e-6);
  TEST_NEAR("point-to-box dist - angle outside (b) ", rsdl_dist( test_p, box), vcl_sqrt(sq_dist), 1e-6);


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
  testlib_test_begin( "rsdl_dist_box_relation - cartesian only (a) " );
  testlib_test_perform( !inside && !intersects );

  // below/between (intersects)
  inner_c.min_cartesian(0) = -3.0; inner_c.max_cartesian(0) = -1.0;
  rsdl_dist_box_relation( inner_c, outer_c, inside, intersects );
  testlib_test_begin( "rsdl_dist_box_relation - cartesian only (b) " );
  testlib_test_perform( !inside && intersects );

  // below/above (intersects)
  inner_c.min_cartesian(0) = -20.0; inner_c.max_cartesian(0) = 20.0;
  rsdl_dist_box_relation( inner_c, outer_c, inside, intersects );
  testlib_test_begin( "rsdl_dist_box_relation - cartesian only (c) " );
  testlib_test_perform( !inside && intersects );

  // between/between (inside and intersects)
  inner_c.min_cartesian(0) = -1.0; inner_c.max_cartesian(0) = 1.0;
  rsdl_dist_box_relation( inner_c, outer_c, inside, intersects );
  testlib_test_begin( "rsdl_dist_box_relation - cartesian only (d) " );
  testlib_test_perform( inside && intersects );

  // between/above (intersects)
  inner_c.min_cartesian(0) = -1.0; inner_c.max_cartesian(0) = 8.0;
  rsdl_dist_box_relation( inner_c, outer_c, inside, intersects );
  testlib_test_begin( "rsdl_dist_box_relation - cartesian only (e) " );
  testlib_test_perform( !inside && intersects );

  // above/above (nothing)
  inner_c.min_cartesian(0) = 7.0; inner_c.max_cartesian(0) = 8.0;
  rsdl_dist_box_relation( inner_c, outer_c, inside, intersects );
  testlib_test_begin( "rsdl_dist_box_relation - cartesian only (f) " );
  testlib_test_perform( !inside && !intersects );

  //  Angular next.  Start with neither interval wrapping around (6 tests)
  rsdl_point min_a(0,1), max_a(0,1);
  min_a.angular(0) = -vnl_math::pi/2; max_a.cartesian(0) = vnl_math::pi/2;
  rsdl_bounding_box outer_a( min_a, max_a );
  rsdl_bounding_box inner_a( min_a, max_a );

  //  both below (nothing)
  inner_a.min_cartesian(0) = -vnl_math::pi; inner_a.max_cartesian(0) = -3*vnl_math::pi/4;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  testlib_test_begin( "rsdl_dist_box_relation - angular only (1a) " );
  testlib_test_perform( !inside && !intersects );

  //  below / between (intersects)
  inner_a.min_cartesian(0) = -vnl_math::pi; inner_a.max_cartesian(0) = -vnl_math::pi/4;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  testlib_test_begin( "rsdl_dist_box_relation - angular only (1b) " );
  testlib_test_perform( !inside && intersects );

  //  below / above (intersects)
  inner_a.min_cartesian(0) = -vnl_math::pi; inner_a.max_cartesian(0) = vnl_math::pi;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  testlib_test_begin( "rsdl_dist_box_relation - angular only (1c) " );
  testlib_test_perform( !inside && intersects );

  // between / between (inside and intersects)
  inner_a.min_cartesian(0) = -vnl_math::pi/4; inner_a.max_cartesian(0) = vnl_math::pi/4;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  testlib_test_begin( "rsdl_dist_box_relation - angular only (1d) " );
  testlib_test_perform( inside && intersects );

  // between / above (intersects)
  inner_a.min_cartesian(0) = 0; inner_a.max_cartesian(0) = vnl_math::pi;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  testlib_test_begin( "rsdl_dist_box_relation - angular only (1e) " );
  testlib_test_perform( !inside && intersects );

  // above / above (nothing)
  inner_a.min_cartesian(0) = vnl_math::pi; inner_a.max_cartesian(0) = 1.5*vnl_math::pi;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  testlib_test_begin( "rsdl_dist_box_relation - angular only (1f) " );
  testlib_test_perform( !inside && !intersects );

  //
  //  Now, outer does not wrap around, but inner does
  //

  outer_a.min_cartesian(0) = vnl_math::pi/2; outer_a.max_cartesian(0) = 3*vnl_math::pi/2;

  // below / below   (intersects because of wrap-around)
  inner_a.min_cartesian(0) = vnl_math::pi/4; inner_a.max_cartesian(0) = vnl_math::pi/8;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  testlib_test_begin( "rsdl_dist_box_relation - angular only (2a) " );
  testlib_test_perform( !inside && intersects );

  //  between / below (intersects)
  inner_a.min_cartesian(0) = vnl_math::pi; inner_a.max_cartesian(0) = vnl_math::pi/8;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  testlib_test_begin( "rsdl_dist_box_relation - angular only (2b) " );
  testlib_test_perform( !inside && intersects );

  // above / below (nothing)
  inner_a.min_cartesian(0) = 7*vnl_math::pi/4; inner_a.max_cartesian(0) = vnl_math::pi/8;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  testlib_test_begin( "rsdl_dist_box_relation - angular only (2c) " );
  testlib_test_perform( !inside && !intersects );

  // between / between (intersects)
  inner_a.min_cartesian(0) = vnl_math::pi; inner_a.max_cartesian(0) = 3*vnl_math::pi/4;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  testlib_test_begin( "rsdl_dist_box_relation - angular only (2d) " );
  testlib_test_perform( !inside && intersects );

  // above / between (intersects)
  inner_a.min_cartesian(0) = 7*vnl_math::pi/4; inner_a.max_cartesian(0) = vnl_math::pi;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  testlib_test_begin( "rsdl_dist_box_relation - angular only (2e) " );
  testlib_test_perform( !inside && intersects );

  // above / above (intersects)
  inner_a.min_cartesian(0) = 7*vnl_math::pi/4; inner_a.max_cartesian(0) = 1.6*vnl_math::pi;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  testlib_test_begin( "rsdl_dist_box_relation - angular only (2f) " );
  testlib_test_perform( !inside && intersects );


  //  Now, outer wraps around, but inner does not
  outer_a.min_cartesian(0) = 3*vnl_math::pi/2; outer_a.max_cartesian(0) = vnl_math::pi/2;

  //  below / below (inside and intersects)
  inner_a.min_cartesian(0) = vnl_math::pi/8; inner_a.max_cartesian(0) = vnl_math::pi/4;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  testlib_test_begin( "rsdl_dist_box_relation - angular only (3a) " );
  testlib_test_perform( inside && intersects );

  //  below / between (intersects)
  inner_a.min_cartesian(0) = vnl_math::pi/8; inner_a.max_cartesian(0) = 3*vnl_math::pi/4;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  testlib_test_begin( "rsdl_dist_box_relation - angular only (3b) " );
  testlib_test_perform( !inside && intersects );

  //  below / above (intersects)
  inner_a.min_cartesian(0) = vnl_math::pi/8; inner_a.max_cartesian(0) = 7*vnl_math::pi/4;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  testlib_test_begin( "rsdl_dist_box_relation - angular only (3c) " );
  testlib_test_perform( !inside && intersects );

  //  between / between (nothing)
  inner_a.min_cartesian(0) = 7*vnl_math::pi/8; inner_a.max_cartesian(0) = vnl_math::pi;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  testlib_test_begin( "rsdl_dist_box_relation - angular only (3d) " );
  testlib_test_perform( !inside && !intersects );

  //  between / above (intersects)
  inner_a.min_cartesian(0) = 7*vnl_math::pi/8; inner_a.max_cartesian(0) = 7*vnl_math::pi/4;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  testlib_test_begin( "rsdl_dist_box_relation - angular only (3e) " );
  testlib_test_perform( !inside && intersects );

  //  above / above (inside and intersects)
  inner_a.min_cartesian(0) = 3*vnl_math::pi/2; inner_a.max_cartesian(0) = 7*vnl_math::pi/4;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  testlib_test_begin( "rsdl_dist_box_relation - angular only (3f) " );
  testlib_test_perform( inside && intersects );


 //  Now, both wrap around...
  outer_a.min_cartesian(0) = 3*vnl_math::pi/2; outer_a.max_cartesian(0) = vnl_math::pi/2;

  //  below / below (intersect)
  inner_a.min_cartesian(0) = vnl_math::pi/4; inner_a.max_cartesian(0) = vnl_math::pi/8;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  testlib_test_begin( "rsdl_dist_box_relation - angular only (4a) " );
  testlib_test_perform( !inside && intersects );

  //  between / below (intersect)
  inner_a.min_cartesian(0) = vnl_math::pi; inner_a.max_cartesian(0) = vnl_math::pi/8;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  testlib_test_begin( "rsdl_dist_box_relation - angular only (4b) " );
  testlib_test_perform( !inside && intersects );

  //  above / below (inside and intersect)
  inner_a.min_cartesian(0) = 7*vnl_math::pi/4; inner_a.max_cartesian(0) = vnl_math::pi/8;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  testlib_test_begin( "rsdl_dist_box_relation - angular only (4c) " );
  testlib_test_perform( inside && intersects );

  //  between / between (intersect)
  inner_a.min_cartesian(0) = 9*vnl_math::pi/8; inner_a.max_cartesian(0) = vnl_math::pi;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  testlib_test_begin( "rsdl_dist_box_relation - angular only (4d) " );
  testlib_test_perform( intersects );

  //  above / between (intersect)
  inner_a.min_cartesian(0) = 7*vnl_math::pi/4; inner_a.max_cartesian(0) = vnl_math::pi;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  testlib_test_begin( "rsdl_dist_box_relation - angular only (4e) " );
  testlib_test_perform( intersects );

  //  above / above (intersect)
  inner_a.min_cartesian(0) = 7*vnl_math::pi/4; inner_a.max_cartesian(0) = 13*vnl_math::pi/8;
  rsdl_dist_box_relation( inner_a, outer_a, inside, intersects );
  testlib_test_begin( "rsdl_dist_box_relation - angular only (4f) " );
  testlib_test_perform( intersects );

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
  testlib_test_begin( "rsdl_dist_box_relation - complete (a) " );
  rsdl_dist_box_relation( inner, outer, inside, intersects );
  testlib_test_perform( inside && intersects );

  // inside fails on cartesian
  inner.min_cartesian(0) = 1.25;
  testlib_test_begin( "rsdl_dist_box_relation - complete (b) " );
  rsdl_dist_box_relation( inner, outer, inside, intersects );
  testlib_test_perform( !inside && intersects );

  // inside fails on angular
  inner.min_cartesian(0) = 1.95;
  inner.min_angular(2) = 5*pi/4;
  testlib_test_begin( "rsdl_dist_box_relation - complete (c) " );
  rsdl_dist_box_relation( inner, outer, inside, intersects );
  testlib_test_perform( !inside && intersects );

  // intersects fails on cartesian
  inner.min_cartesian(0) = 4.75; inner.max_cartesian(0) = 5.0;
  testlib_test_begin( "rsdl_dist_box_relation - complete (d) " );
  rsdl_dist_box_relation( inner, outer, inside, intersects );
  testlib_test_perform( !inside && !intersects );

  // intersects fails on angular
  inner.min_cartesian(0) = 1.95;
  inner.min_angular(0) = 0;   inner.max_angular(0) = pi/4;
  testlib_test_begin( "rsdl_dist_box_relation - complete (e) " );
  rsdl_dist_box_relation( inner, outer, inside, intersects );
  testlib_test_perform( !inside && !intersects );


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

  testlib_test_begin( "rsdl_dist_point_in_box (a) " );
  inside = rsdl_dist_point_in_box( pt, outer );
  testlib_test_perform( inside );

  pt.angular(2) = 7*pi/4;
  testlib_test_begin( "rsdl_dist_point_in_box (b) " );
  inside = rsdl_dist_point_in_box( pt, outer );
  testlib_test_perform( inside );

  testlib_test_begin( "rsdl_dist_point_in_box (c) " );
  pt.cartesian(1) = -2;
  inside = rsdl_dist_point_in_box( pt, outer );
  testlib_test_perform( !inside );

  testlib_test_begin( "rsdl_dist_point_in_box (d) " );
  pt.cartesian(1) = 1;
  pt.cartesian(0) = 5;
  inside = rsdl_dist_point_in_box( pt, outer );
  testlib_test_perform( !inside );

  testlib_test_begin( "rsdl_dist_point_in_box (e) " );
  pt.cartesian(0) = 2.5;
  inside = rsdl_dist_point_in_box( pt, outer );
  testlib_test_perform( inside );  // sanity check to make sure things are reset correctly

  testlib_test_begin( "rsdl_dist_point_in_box (f) " );
  double temp = pt.angular(0);
  pt.angular(0) = -3*pi/2;
  inside = rsdl_dist_point_in_box( pt, outer );
  pt.angular(0) = temp;
  bool back_inside = rsdl_dist_point_in_box( pt, outer );
  testlib_test_perform( !inside && back_inside );

  testlib_test_begin( "rsdl_dist_point_in_box (g) " );
  temp = pt.angular(0);
  pt.angular(0) = 3*pi/2;
  inside = rsdl_dist_point_in_box( pt, outer );
  pt.angular(0) = temp;
  back_inside = rsdl_dist_point_in_box( pt, outer );
  testlib_test_perform( !inside && back_inside );

  testlib_test_begin( "rsdl_dist_point_in_box (h) " );
  temp = pt.angular(2);
  pt.angular(2) = pi;
  inside = rsdl_dist_point_in_box( pt, outer );
  pt.angular(2) = temp;
  back_inside = rsdl_dist_point_in_box( pt, outer );
  testlib_test_perform( !inside && back_inside );
}

TESTMAIN(test_dist);
