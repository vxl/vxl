#include <vnl/vnl_math.h>
#include <rsdl/rsdl_bounding_box.h>
#include <testlib/testlib_test.h>

static inline bool close( double x, double y ) { return vnl_math_abs(x-y) < 1.0e-6; }

static void test_bounding_box()
{
  const int Nc = 2;
  const int Na = 3;

  rsdl_point min_point(Nc,Na), max_point(Nc,Na);
  min_point.cartesian(0) = 15;               max_point.cartesian(0) = 8;
  min_point.cartesian(1) = 11;               max_point.cartesian(1) = 89.5;
  min_point.angular(0) = -vnl_math::pi/2;    max_point.angular(0) =  vnl_math::pi;
  min_point.angular(1) = -vnl_math::pi/4;    max_point.angular(1) = -vnl_math::pi;
  min_point.angular(2) = 3*vnl_math::pi/2;   max_point.angular(2) = vnl_math::pi;
  testlib_test_begin( "main ctor");
  rsdl_bounding_box box1( min_point, max_point );
  testlib_test_perform( true );
  testlib_test_begin( "cartesian min/max (const access)" );
  testlib_test_perform( close( box1.min_cartesian(0), 8 )
                     && close( box1.max_cartesian(0), 15 )
                     && close( box1.min_cartesian(1), 11 )
                     && close( box1.max_cartesian(1), 89.5 ) );
  testlib_test_begin( "angular min/max (const access)" );
  testlib_test_perform( close( box1.min_angular(0), -vnl_math::pi/2 )
                     && close( box1.max_angular(0), vnl_math::pi )
                     && close( box1.min_angular(1), -vnl_math::pi/4 )
                     && close( box1.max_angular(1), -vnl_math::pi )
                     && close( box1.min_angular(2), 3*vnl_math::pi/2 )
                     && close( box1.max_angular(2), vnl_math::pi ) );

  testlib_test_begin( "cartesian min/max (mutable access)" );
  box1.min_cartesian( 0 ) = -2;
  box1.max_cartesian( 1 ) = 32;
  testlib_test_perform( close( box1.min_cartesian(0), -2 )
                     && close( box1.max_cartesian(0), 15 )
                     && close( box1.min_cartesian(1), 11 )
                     && close( box1.max_cartesian(1), 32 ) );

  testlib_test_begin( "angular min/max (mutable access)" );
  box1.min_angular( 0 ) =  -3*vnl_math::pi/4;
  box1.max_angular( 1 ) =  3*vnl_math::pi/8;
  testlib_test_perform( close( box1.min_angular(0), -3*vnl_math::pi/4 )
                     && close( box1.max_angular(0), vnl_math::pi )
                     && close( box1.min_angular(1), -vnl_math::pi/4 )
                     && close( box1.max_angular(1), 3*vnl_math::pi/8 )
                     && close( box1.min_angular(2), 3*vnl_math::pi/2 )
                     && close( box1.max_angular(2), vnl_math::pi ) );


  testlib_test_begin( "copy constructor" );
  rsdl_bounding_box box2( box1 );
  bool ok = true;
  int i;
  for ( i=0; ok && i<Nc; ++i )
    ok = close( box1.min_cartesian(i), box2.min_cartesian(i) )
      && close( box1.max_cartesian(i), box2.max_cartesian(i) );
  for ( i=0; ok && i<Na; ++i )
    ok = close( box1.min_angular(i), box2.min_angular(i) )
      && close( box1.max_angular(i), box2.max_angular(i) );
  testlib_test_perform( ok );
}

TESTMAIN(test_bounding_box);
