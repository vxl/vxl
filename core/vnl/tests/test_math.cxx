#include <vcl_iostream.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_complex.h>
#include <testlib/testlib_test.h>
#ifdef VCL_VC
// This only seems to work if placed right near the top
# pragma warning ( disable : 4756)
#endif

void test_math() {
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
  testlib_test_assert("abs(-1+2i) == sqrt(5)",
                  vnl_math_abs(vnl_math_abs(z)-vcl_sqrt(5.0)) < 1e-10);
  testlib_test_assert("norm(-1+2i) == 5",
                  vnl_math_abs(vnl_math_squared_magnitude(z)-5) < 1e-10);
  testlib_test_assert("exp(d*i) == -1", vnl_math_abs(e_ipi+1.0) < 1e-10);
  vcl_cout << vcl_endl;

#ifdef VCL_VC
# pragma warning ( push )
# pragma warning ( disable : 4056)
// can't seem to disable warning 4756
#endif
  // Create Inf and -Inf:
  float a1 = 1.0e33f/1.0e-33f;
  float a2 = -1.0e33f/1.0e-33f;
  double a3 = 1.0e300/1.0e-300;
  double a4 = -1.0e300/1.0e-300;
#ifdef VCL_VC
#pragma warning ( pop )
#endif

#if !defined(VCL_VC50) && !defined(VCL_VC60)
  long double a5 = (long double)1/(long double)0; // compiler warning
  long double a6 = -(long double)1/(long double)0; // compiler warning
#endif

#if defined(__alpha__)
  float b1 = 0.0f/0.0f; // compiler warning
  double b2 = 0.0/0.0; // compiler warning
  long double b3 = (long double)0.0/(long double)0.0; // compiler warning
  int t = 0x7f800000; a1 = *((float*)(&t)); a2 = -a1;
#else // this gives runtime errors on alpha
  float b1 = vcl_sqrt(-1.0f);
  double b2 = vcl_sqrt(-1.0);
  long double b3 = vcl_sqrt((long double)-1.0);
#endif

  testlib_test_assert(" isfinite(f)    ",  vnl_math_isfinite(f));
  testlib_test_assert(" isfinite(d)    ",  vnl_math_isfinite(d));
  testlib_test_assert(" isfinite(i)    ",  vnl_math_isfinite(i));
  testlib_test_assert(" isfinite(z)    ",  vnl_math_isfinite(z));
  testlib_test_assert("!isfinite(1/0f) ", !vnl_math_isfinite(a1));
  testlib_test_assert(" isinf(1/0f)    ",  vnl_math_isinf(a1));
  testlib_test_assert("!isnan(1/0f)    ", !vnl_math_isnan(a1));
  testlib_test_assert("!isfinite(-1/0f)", !vnl_math_isfinite(a2));
  testlib_test_assert(" isinf(-1/0f)   ",  vnl_math_isinf(a2));
  testlib_test_assert("!isnan(-1/0f)   ", !vnl_math_isnan(a2));
  testlib_test_assert("!isfinite(0/0f) ", !vnl_math_isfinite(b1));
  testlib_test_assert("!isinf(0/0f)    ", !vnl_math_isinf(b1));
  testlib_test_assert(" isnan(0/0f)    ",  vnl_math_isnan(b1));

  testlib_test_assert("!isfinite(1/0d) ", !vnl_math_isfinite(a3));
  testlib_test_assert(" isinf(1/0d)    ",  vnl_math_isinf(a3));
  testlib_test_assert("!isnan(1/0d)    ", !vnl_math_isnan(a3));
  testlib_test_assert("!isfinite(-1/0d)", !vnl_math_isfinite(a4));
  testlib_test_assert(" isinf(-1/0d)   ",  vnl_math_isinf(a4));
  testlib_test_assert("!isnan(-1/0d)   ", !vnl_math_isnan(a4));
  testlib_test_assert("!isfinite(0/0d) ", !vnl_math_isfinite(b2));
  testlib_test_assert("!isinf(0/0d)    ", !vnl_math_isinf(b2));
  testlib_test_assert(" isnan(0/0d)    ",  vnl_math_isnan(b2));

#if !defined(VCL_VC50) && !defined(VCL_VC60)
  testlib_test_assert("!isfinite(1/0l) ", !vnl_math_isfinite(a5));
  testlib_test_assert(" isinf(1/0l)    ",  vnl_math_isinf(a5));
  testlib_test_assert("!isnan(1/0l)    ", !vnl_math_isnan(a5));
  testlib_test_assert("!isfinite(-1/0l)", !vnl_math_isfinite(a6));
  testlib_test_assert(" isinf(-1/0l)   ",  vnl_math_isinf(a6));
  testlib_test_assert("!isnan(-1/0l)   ", !vnl_math_isnan(a6));
#endif
  testlib_test_assert("!isfinite(0/0l) ", !vnl_math_isfinite(b3));
  testlib_test_assert("!isinf(0/0l)    ", !vnl_math_isinf(b3));
  testlib_test_assert(" isnan(0/0l)    ",  vnl_math_isnan(b3));
}

TESTMAIN(test_math);

