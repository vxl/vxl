#include <vcl_iostream.h>
#include <vcl_iomanip.h>
#include <vnl/vnl_rational.h>
#include <vnl/vnl_test.h>
#include <vnl/vnl_math.h>

void test_rational() {
  {
    vnl_rational a(-5L), b(7,-1), c, d(3,7), e(2,0);
    c = a + b; TEST("+", c, -12L);
    c = a - b; TEST("-", c, 2L);
    c = a * b; TEST("*", c, 35L);
    c = a / b; TEST("/", c, vnl_rational(5,7));
    c = a % b; TEST("%", c, -5L);
    c = a + 5L; TEST("+", c, 0L);
    c = a - 5L; TEST("+", c, -10L);
    c = a * 5L; TEST("+", c, -25L);
    c = a / 5L; TEST("+", c, -1L);
    c = a % 5L; TEST("+", c, 0L);
    TEST("<", a<d, true);
    TEST("<", a<1L, true);
    TEST(">", -b>d, true);
    TEST("<=", c<=e, true);
    TEST(">=", b>=-7L, true);
    TEST("truncate", truncate(1L+d), 1L);
    TEST("truncate", truncate(-d-1L), -1L);
    TEST("round", round(1L+d), 1L);
    TEST("round", round(-d-1L), -1L);
    TEST("round", round(1L-d), 1L);
    TEST("round", round(d-1L), -1L);
    TEST("floor", floor(1L+d), 1L);
    TEST("floor", floor(-d-1L), -2L);
    TEST("ceil", ceil(1L+d), 2L);
    TEST("ceil", ceil(-d-1L), -1L);
    a += b;
    a -= b;
    a *= b;
    a /= b;
    a %= b;
    vcl_cout << vcl_setprecision(20);
    vcl_cout << "a=" << a << "=" << (double)a << vcl_endl;
    vcl_cout << "b=" << b << "=" << (double)b << vcl_endl;
    vcl_cout << "c=" << c << "=" << (double)c << vcl_endl;
    vcl_cout << "d=" << d << "=" << (double)d << vcl_endl;
    vcl_cout << "e=" << e << vcl_endl; // (double)d ==> floating exception
    d = -7;
    d = -7L;
    vcl_cout << vcl_endl;
  }

  {
    vnl_rational Inf(1,0); ++Inf;
    TEST("Inf+1", Inf.numerator() == 1 && Inf.denominator() == 0, true);
    Inf = -Inf;
    TEST("-Inf", Inf.numerator() == -1 && Inf.denominator() == 0, true);
  }

  {
    vnl_rational r(-15,-20);
    TEST("simplify", r.numerator() == 3 && r.denominator() == 4, true);
  }
  {
    vnl_rational d ( 1.0/3.0 ); // explicit constructor from double
    TEST("construct from double", d, vnl_rational(1,3));
    d = vnl_rational(-5.0/7);
    TEST("construct from double", d, vnl_rational(-5,7));
    d = vnl_rational(0.42857142857);
    TEST("construct from double", d, vnl_rational(3,7));
    d = vnl_rational(-1.23456);
    TEST("construct from double", d, vnl_rational(-123456,100000));
    vnl_rational pi = vnl_rational(vnl_math::pi);
    double pi_a = double(pi);
    TEST("pi", pi_a-vnl_math::pi < 1e-15 && vnl_math::pi-pi_a < 1e-15, true);
    vcl_cout << "Best rational approximation of pi: " << pi << " = "
             << pi_a << vcl_endl;
    vcl_cout << "Compare this with pi in 16 decimals:                   "
             << vnl_math::pi << vcl_endl;
  }
}

TESTMAIN(test_rational);
