#include <vcl_vector.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_math.h>
#include <testlib/testlib_test.h>

#include <rsdl/rsdl_point.h>

static inline bool close( double x, double y ) { return vnl_math_abs(x-y) < 1.0e-6; }

static void test_point()
{
  const unsigned int Nc = 2;
  const unsigned int Na = 3;

  testlib_test_begin( "default ctor, num_cartesian and num_angular" );
  {
    rsdl_point pt0;
    testlib_test_perform( pt0.num_cartesian() == 0 && pt0.num_angular() == 0 );
  }

  testlib_test_begin( "ctor setting size only, num_cartesian and num_angular" );
  {
    rsdl_point pt1( Nc, Na );
    testlib_test_perform( pt1.num_cartesian() == Nc && pt1.num_angular() == Na );
  }

  vcl_vector<double> cart(Nc);
  cart[0] = 2.5; cart[1] = -3.0;
  vcl_vector<double> ang(Na);
  ang[0] = -vnl_math::pi/2; ang[1] = vnl_math::pi/4; ang[2] = vnl_math::pi;
#if 0
  testlib_test_begin( "ctor from two arrays" );
  {
    rsdl_point pt2( cart, cart+Nc, ang, ang+Na );
    bool ok=true;
    for ( unsigned int i=0; ok && i<Nc; ++i ) ok = ok && close(pt2.cartesian(i), cart[i]);
    for ( unsigned int i=0; ok && i<Na; ++i ) ok = ok && close(pt2.angular(i), ang[i]);
    testlib_test_perform( ok );
  }
#endif
  testlib_test_begin( "ctor from two vnl_vectors" );
  {
    vnl_vector<double> cv(2, 1.5,2.1);
    vnl_vector<double> av(3, -3.0,-1.5,3.1);
    rsdl_point pt_from_2v( cv, av );
    bool ok=true;
    for ( unsigned int i=0; ok && i<Nc; ++i ) ok = ok && close(pt_from_2v.cartesian(i), cv[i]);
    for ( unsigned int i=0; ok && i<Na; ++i ) ok = ok && close(pt_from_2v.angular(i), av[i]);
    testlib_test_perform( ok );
  }

  testlib_test_begin( "ctor from two vector ptrs" );
  rsdl_point pt3( cart.begin(), cart.end(), ang.begin(), ang.end() );
  {
    bool ok=true;
    for ( unsigned int i=0; ok && i<Nc; ++i ) ok = ok && close(pt3.cartesian(i), cart[i]);
    for ( unsigned int i=0; ok && i<Na; ++i ) ok = ok && close(pt3.angular(i), ang[i]);
    testlib_test_perform( ok );
  }

  testlib_test_begin( "copy ctor" );
  {
    rsdl_point pt4( pt3 );
    bool ok=true;
    for ( unsigned int i=0; ok && i<Nc; ++i ) ok = ok && close(pt4.cartesian(i), cart[i]);
    for ( unsigned int i=0; ok && i<Na; ++i ) ok = ok && close(pt4.angular(i), ang[i]);
    testlib_test_perform( ok );
  }

  testlib_test_begin( "ctor all angles" );
  vcl_vector<double> a(6);
  a[0] = -0.3;  a[1] = -0.2; a[2] = 0.3; a[3] = 0; a[4] = 0.1; a[5] = 0.3;
  // Note that a.begin() - a.begin() is a null range.
  rsdl_point pt_no_cart( a.begin(), a.begin(), a.begin(), a.end() );
  testlib_test_perform( true );

  testlib_test_begin( "angular" );
  {
    bool ok = true;
    for ( unsigned int i=0; ok && i<6; ++i ) {
      pt_no_cart.angular(i) += i / 10.0;
      ok = ok && close( pt_no_cart.angular(i), a[i] + i / 10.0 );
    }
    testlib_test_perform( ok && pt_no_cart.num_cartesian() == 0 && pt_no_cart.num_angular() == 6 );
  }

  testlib_test_begin( "ctor all cartesian" );
  vcl_vector<double> c(2);
  c[0] = 15; c[1] = 13.1;
  rsdl_point pt_no_ang( c.begin(), c.end(), c.begin(), c.begin() );
  testlib_test_perform( true );

  testlib_test_begin( "cartesian" );
  {
    bool ok = true;
    for ( unsigned int i=0; ok && i<2; ++i ) {
      pt_no_ang.cartesian(i) += i * 5.0;
      ok = ok && close( pt_no_ang.cartesian(i), c[i] + i * 5.0 );
    }
    testlib_test_perform( ok );
  }

  testlib_test_begin( "ctor from single vnl_vector" );
  vnl_vector<double> from_v(5);
  from_v[0] = 1.0;  from_v[1] = 3.0;  from_v[2] = -1.0;  from_v[3] = 0.5;  from_v[4] = 1.1;
  rsdl_point qv( from_v, 3 );
  testlib_test_perform( close( qv.cartesian(0), from_v[0] )
                     && close( qv.cartesian(1), from_v[1] )
                     && close( qv.angular(0), from_v[2] )
                     && close( qv.angular(1), from_v[3] )
                     && close( qv.angular(2), from_v[4] ) );

  vnl_vector<double> new_c(2);
  new_c.x() = -7;  new_c.y() = 3;

  cart[0] = 3.5; cart[1] = -5.5;
  ang[0] = -vnl_math::pi/4; ang[1] = vnl_math::pi/4; ang[2] = vnl_math::pi;
  rsdl_point q( cart.begin(), cart.end(), ang.begin(), ang.end() );

  testlib_test_begin( "set_cartesian from vnl_vector" );
  {
    q.set_cartesian( new_c );
    bool ok = true;
    for ( unsigned int i=0; ok && i<Nc; ++i )
      ok = close( q.cartesian(i), new_c[i] );
    for ( unsigned int i=0; ok && i<Na; ++i )
      ok = close( q.angular(i), ang[i] );
    testlib_test_perform( ok );
  }

  testlib_test_begin( "set_cartesian from array" );
  {
    cart[0] = 80; cart[1]=58.7;
    q.set_cartesian( cart.begin() );
    bool ok = true;
    for ( unsigned int i=0; ok && i<Nc; ++i )
      ok = close( q.cartesian(i), cart[i] );
    for ( unsigned int i=0; ok && i<Na; ++i )
      ok = close( q.angular(i), ang[i] );
    testlib_test_perform( ok );
  }

  testlib_test_begin( "set_angular from vnl_vector" );
  {
    vnl_vector<double> avect(3, -1.0,-1.4,2.0);
    q.set_angular( avect );
    bool ok = true;
    for ( unsigned int i=0; ok && i<Nc; ++i )
      ok = close( q.cartesian(i), cart[i] );
    for ( unsigned int i=0; ok && i<Na; ++i )
      ok = close( q.angular(i), avect[i] );
    testlib_test_perform( ok );
  }

  testlib_test_begin( "set_angular from vcl_vector" );
  {
    ang[0] = -1.5; ang[1] = 2.1; ang[2] = 0.6;
    q.set_angular( ang.begin() );
    bool ok = true;
    for ( unsigned int i=0; ok && i<Nc; ++i )
      ok = close( q.cartesian(i), cart[i] );
    for ( unsigned int i=0; ok && i<Na; ++i )
      ok = close( q.angular(i), ang[i] );
    testlib_test_perform( ok );
  }
}

TESTMAIN(test_point);
