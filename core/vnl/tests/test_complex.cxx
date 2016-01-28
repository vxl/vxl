// This is core/vnl/tests/test_complex.cxx
#include <testlib/testlib_test.h>
// :
// \file

#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <vcl_complex.h>

#include <vnl/vnl_vector.h>
#include <vnl/vnl_random.h>
#include <vnl/vnl_math.h>

// : inverse cosine for complex numbers.
// The implementation is at the bottom of this file.
// \author Peter Vanroose, ESAT, KULeuven.
vcl_complex<double> tc_acos(vcl_complex<double> x);

// make a vector with random, complex entries :
static void fill_rand(vcl_complex<double> * b, vcl_complex<double> * e, vnl_random & rng)
{
  for( vcl_complex<double> * p = b; p < e; ++p )
    {
    (*p) = vcl_complex<double>( rng.drand64(-1.0, +1.0), rng.drand64(-1.0, +1.0) );
    }
}

static void test_operators()
{
  vcl_complex<double> a(-5), b(7, -1), c;
  c = a + b; TEST("sum", c, vcl_complex<double>(2, -1) );
  c = a - b; TEST("difference", c, vcl_complex<double>(-12, 1) );
  c = a * b; TEST("product", c, vcl_complex<double>(-35, 5) );
  c = a / b; TEST_NEAR("quotient", c, vcl_complex<double>(-0.7, -0.1), 1e-12);
  a += b; TEST("+=", a, vcl_complex<double>(2, -1) );
  a -= b; TEST("-=", a, vcl_complex<double>(-5) );
  a *= b; TEST("*=", a, vcl_complex<double>(-35, 5) );
  a /= b; TEST("/=", a, vcl_complex<double>(-5) );
}

static void test_vector()
{
  vnl_random rng(9667566);

  vnl_vector<vcl_complex<double> > a(5); fill_rand(a.begin(), a.end(), rng);
  vnl_vector<vcl_complex<double> > b(5); fill_rand(b.begin(), b.end(), rng);

  vcl_cout << "a=" << a << '\n'
           << "b=" << b << '\n'
           << "dot_product(a,b)=" << dot_product(a, b) << '\n'
           << "inner_product(a,b)=" << inner_product(a, b) << '\n';

  vcl_complex<double> i(0, 1);

  TEST("inner_product() conjugates correctly", inner_product(i * a, b), i * inner_product(a, b) );
  TEST("inner_product() conjugates correctly", inner_product(a, i * b), -i * inner_product(a, b) );

  TEST("dot_product() does not conjugate", dot_product(i * a, b), i * dot_product(a, b) );
  TEST("dot_product() does not conjugate", dot_product(a, i * b), i * dot_product(a, b) );

  double norma = 0;
  for( unsigned n = 0; n < a.size(); ++n )
    {
    norma += vcl_real(a[n]) * vcl_real(a[n]) + vcl_imag(a[n]) * vcl_imag(a[n]);
    }
  norma = vcl_sqrt(norma);
  TEST_NEAR("correct magnitude", norma, a.magnitude(), 1e-12);
}

static void test_cosine()
{
  vnl_random rng(1234567);

  for( int i = 0; i < 20; ++i )
    {
    double              u = rng.drand32(vnl_math::pi_over_2);
    double              v = rng.drand32(2.0);
    vcl_complex<double> c(u, v);
    vcl_complex<double> d = vcl_cos(c);
    vcl_complex<double> e = tc_acos(d);
    vcl_cout << c << ' ' << d << ' ' << e << '\n';
    TEST_NEAR("acos", c, e, 1e-12);
    }
}

void test_complex()
{
  test_operators();
  test_vector();
  test_cosine();
}

TESTMAIN(test_complex);

// : inverse cosine for complex numbers.
// implementation by Peter Vanroose, ESAT, KULeuven.
vcl_complex<double> tc_acos(vcl_complex<double> x)
{
  double a = vcl_real(x), b = vcl_imag(x);

  // special cases:
  if( b == 0 && a > 1 )
    {
    return vcl_complex<double>(0.0, vcl_log(a + vcl_sqrt(a * a - 1) ) ); // == acosh(a)
    }
  else if( b == 0 && a >= -1.0 )
    {
    return vcl_acos(a);
    }

  // the general case:
  // the result c + d*i satisfies a = cos(c)*cosh(d), b = -sin(c)*sinh(d)
  // hence $\frac{a^2}{\cos^2(c)} - \frac{b^2}{\sin^2(c)} = 1$.
  double q = (a * a - 1) * (a * a - 1) + b * b * (b * b + 2 * a * a + 2);
  double t = 0.5 * (1 + a * a + b * b - vcl_sqrt(q) );
  // this $t = \cos^2(c)$ solves the latter biquadratic equation and lies in [0,1].
  double aa = a / vcl_sqrt(t), bb = b / vcl_sqrt(1 - t);
  double r_real = vcl_acos(vcl_sqrt(t) );
  double r_imag = vcl_log(vcl_fabs(aa - bb) );
  return vcl_complex<double>(r_real, r_imag);
}
