#include <vcl_iostream.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_complex.h>
#include <vnl/vnl_numeric_limits.h> // for infinity()
#include <testlib/testlib_test.h>

static
void check_pointer( const void * )
{
}

static
void test_static_const_definition()
{
  check_pointer( &vnl_math::e );
  check_pointer( &vnl_math::log2e );
  check_pointer( &vnl_math::log10e );
  check_pointer( &vnl_math::ln2 );
  check_pointer( &vnl_math::ln10 );
  check_pointer( &vnl_math::pi );
  check_pointer( &vnl_math::pi_over_2 );
  check_pointer( &vnl_math::pi_over_4 );
  check_pointer( &vnl_math::one_over_pi );
  check_pointer( &vnl_math::two_over_pi );
  check_pointer( &vnl_math::two_over_sqrtpi );
  check_pointer( &vnl_math::sqrt2 );
  check_pointer( &vnl_math::sqrt1_2 );
  check_pointer( &vnl_math::eps );
  check_pointer( &vnl_math::sqrteps );
}


void test_math()
{
  // Call it to avoid compiler warnings
  test_static_const_definition();

  int n = -11;
  float f = -7.5;
  double d = -vnl_math::pi;
  vcl_complex<double> i(0,1);
  vcl_complex<double> z(-1,2);
  vcl_complex<double> e_ipi = vcl_exp(d*i);

  vcl_cout << "n=" << n << vcl_endl
           << "f=" << f << vcl_endl
           << "d=" << d << vcl_endl
           << "i=" << i << vcl_endl
           << "z=" << z << vcl_endl
           << "exp(d*i)=" << e_ipi << vcl_endl
           << vcl_endl

           << "abs(n)=" << vnl_math_abs(n) << vcl_endl
           << "abs(f)=" << vnl_math_abs(f) << vcl_endl
           << "abs(d)=" << vnl_math_abs(d) << vcl_endl
           << "abs(i)=" << vnl_math_abs(i) << vcl_endl
           << "abs(z)=" << vnl_math_abs(z) << vcl_endl
           <<"norm(z)=" << vnl_math_squared_magnitude(z) << vcl_endl
           << vcl_endl;

  testlib_test_assert("abs(n) == 11", vnl_math_abs(n) == 11);
  testlib_test_assert("abs(f) == 7.5", vnl_math_abs(f) == 7.5);
  testlib_test_assert("abs(d) == pi", vnl_math_abs(d) == vnl_math::pi);
  testlib_test_assert("abs(i) == 1", vnl_math_abs(i) == 1.0);
  testlib_test_assert_near("abs(-1+2i)~=sqrt(5)",vnl_math_abs(z),vcl_sqrt(5.0));
  testlib_test_assert_near("norm(-1+2i) ~= 5", vnl_math_squared_magnitude(z),5);
  testlib_test_assert_near("exp(d*i) ~= -1", vnl_math_abs(e_ipi+1.0), 0);
  vcl_cout << vcl_endl;

  // Create Inf and -Inf:
  float a1 = vnl_numeric_limits<float>::infinity();
  float a2 = -vnl_numeric_limits<float>::infinity();
  double a3 = vnl_numeric_limits<double>::infinity();
  double a4 = -vnl_numeric_limits<double>::infinity();
  long double a5 = vnl_numeric_limits<long double>::infinity();
  long double a6 = -vnl_numeric_limits<long double>::infinity();

  // Create NaN
  float b1 = vnl_numeric_limits<float>::quiet_NaN();
  double b2 = vnl_numeric_limits<double>::quiet_NaN();
  long double b3 = vnl_numeric_limits<long double>::quiet_NaN();

  testlib_test_assert(" isfinite(f)    ",  vnl_math_isfinite(f));
  testlib_test_assert(" isfinite(d)    ",  vnl_math_isfinite(d));
  testlib_test_assert(" isfinite(i)    ",  vnl_math_isfinite(i));
  testlib_test_assert(" isfinite(z)    ",  vnl_math_isfinite(z));
#ifndef __alpha__ // on alpha, infinity() == max()
  testlib_test_assert("!isfinite(1/0f) ", !vnl_math_isfinite(a1));
  testlib_test_assert(" isinf(1/0f)    ",  vnl_math_isinf(a1));
  testlib_test_assert("!isfinite(-1/0f)", !vnl_math_isfinite(a2));
  testlib_test_assert(" isinf(-1/0f)   ",  vnl_math_isinf(a2));
#endif
  testlib_test_assert("!isnan(1/0f)    ", !vnl_math_isnan(a1));
  testlib_test_assert("!isnan(-1/0f)   ", !vnl_math_isnan(a2));
  testlib_test_assert("!isfinite(0/0f) ", !vnl_math_isfinite(b1));
  testlib_test_assert("!isinf(0/0f)    ", !vnl_math_isinf(b1));
  testlib_test_assert(" isnan(0/0f)    ",  vnl_math_isnan(b1));

#ifndef __alpha__ // on alpha, infinity() == max()
  testlib_test_assert("!isfinite(1/0d) ", !vnl_math_isfinite(a3));
  testlib_test_assert(" isinf(1/0d)    ",  vnl_math_isinf(a3));
  testlib_test_assert("!isfinite(-1/0d)", !vnl_math_isfinite(a4));
  testlib_test_assert(" isinf(-1/0d)   ",  vnl_math_isinf(a4));
#endif
  testlib_test_assert("!isnan(1/0d)    ", !vnl_math_isnan(a3));
  testlib_test_assert("!isnan(-1/0d)   ", !vnl_math_isnan(a4));
  testlib_test_assert("!isfinite(0/0d) ", !vnl_math_isfinite(b2));
  testlib_test_assert("!isinf(0/0d)    ", !vnl_math_isinf(b2));
  testlib_test_assert(" isnan(0/0d)    ",  vnl_math_isnan(b2));

#ifndef __alpha__ // on alpha, infinity() == max()
  testlib_test_assert("!isfinite(1/0l) ", !vnl_math_isfinite(a5));
  testlib_test_assert(" isinf(1/0l)    ",  vnl_math_isinf(a5));
  testlib_test_assert("!isfinite(-1/0l)", !vnl_math_isfinite(a6));
  testlib_test_assert(" isinf(-1/0l)   ",  vnl_math_isinf(a6));
#endif
  testlib_test_assert("!isnan(1/0l)    ", !vnl_math_isnan(a5));
  testlib_test_assert("!isnan(-1/0l)   ", !vnl_math_isnan(a6));
  testlib_test_assert("!isfinite(0/0l) ", !vnl_math_isfinite(b3));
  testlib_test_assert("!isinf(0/0l)    ", !vnl_math_isinf(b3));
  testlib_test_assert(" isnan(0/0l)    ",  vnl_math_isnan(b3));
}

TESTMAIN(test_math);

