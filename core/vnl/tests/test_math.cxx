#include <vcl_iostream.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_complex.h>
#include <vnl/vnl_test.h>

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

  vnl_test_assert("abs(n) == 11", vnl_math_abs(n) == 11);
  vnl_test_assert("abs(f) == 7.5", vnl_math_abs(f) == 7.5);
  vnl_test_assert("abs(d) == pi", vnl_math_abs(d) == vnl_math::pi);
  vnl_test_assert("abs(i) == 1", vnl_math_abs(i) == 1.0);
  vnl_test_assert("abs(-1+2i) == sqrt(5)",
                  vnl_math_abs(vnl_math_abs(z)-vcl_sqrt(5.0)) < 1e-10);
  vnl_test_assert("norm(-1+2i) == 5",
                  vnl_math_abs(vnl_math_squared_magnitude(z)-5) < 1e-10);
  vnl_test_assert("exp(d*i) == -1", vnl_math_abs(e_ipi+1.0) < 1e-10);
  vcl_cout << vcl_endl;

#ifdef VCL_VC
# pragma warning ( push )
# pragma warning ( disable : 4056 4756)
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

#if VCL_USE_LONG_DOUBLE
  long double a5 = (long double)1/(long double)0;
  long double a6 = -(long double)1/(long double)0;
#endif

#if defined(__alpha__)
  float b1 = 0.0f/0.0f; // compiler warning
  double b2 = 0.0/0.0; // compiler warning
  int t = 0x7f800000; a1 = *((float*)(&t)); a2 = -a1;
# if VCL_USE_LONG_DOUBLE
  long double b3 = (long double)0.0/(long double)0.0; // compiler warning
# endif
#else // this gives runtime errors on alpha
  float b1 = vcl_sqrt(-1.0f);
  double b2 = vcl_sqrt(-1.0);
# if VCL_USE_LONG_DOUBLE
  long double b3 = vcl_sqrt((long double)-1.0);
# endif
#endif

  vnl_test_assert(" isfinite(f)    ",  vnl_math_isfinite(f));
  vnl_test_assert(" isfinite(d)    ",  vnl_math_isfinite(d));
  vnl_test_assert(" isfinite(i)    ",  vnl_math_isfinite(i));
  vnl_test_assert(" isfinite(z)    ",  vnl_math_isfinite(z));
  vnl_test_assert("!isfinite(1/0f) ", !vnl_math_isfinite(a1));
  vnl_test_assert(" isinf(1/0f)    ",  vnl_math_isinf(a1));
  vnl_test_assert("!isnan(1/0f)    ", !vnl_math_isnan(a1));
  vnl_test_assert("!isfinite(-1/0f)", !vnl_math_isfinite(a2));
  vnl_test_assert(" isinf(-1/0f)   ",  vnl_math_isinf(a2));
  vnl_test_assert("!isnan(-1/0f)   ", !vnl_math_isnan(a2));
  vnl_test_assert("!isfinite(0/0f) ", !vnl_math_isfinite(b1));
  vnl_test_assert("!isinf(0/0f)    ", !vnl_math_isinf(b1));
  vnl_test_assert(" isnan(0/0f)    ",  vnl_math_isnan(b1));

  vnl_test_assert("!isfinite(1/0d) ", !vnl_math_isfinite(a3));
  vnl_test_assert(" isinf(1/0d)    ",  vnl_math_isinf(a3));
  vnl_test_assert("!isnan(1/0d)    ", !vnl_math_isnan(a3));
  vnl_test_assert("!isfinite(-1/0d)", !vnl_math_isfinite(a4));
  vnl_test_assert(" isinf(-1/0d)   ",  vnl_math_isinf(a4));
  vnl_test_assert("!isnan(-1/0d)   ", !vnl_math_isnan(a4));
  vnl_test_assert("!isfinite(0/0d) ", !vnl_math_isfinite(b2));
  vnl_test_assert("!isinf(0/0d)    ", !vnl_math_isinf(b2));
  vnl_test_assert(" isnan(0/0d)    ",  vnl_math_isnan(b2));
#if VCL_USE_LONG_DOUBLE
  vnl_test_assert("!isfinite(1/0l) ", !vnl_math_isfinite(a5));
  vnl_test_assert(" isinf(1/0l)    ",  vnl_math_isinf(a5));
  vnl_test_assert("!isnan(1/0l)    ", !vnl_math_isnan(a5));
  vnl_test_assert("!isfinite(-1/0l)", !vnl_math_isfinite(a6));
  vnl_test_assert(" isinf(-1/0l)   ",  vnl_math_isinf(a6));
  vnl_test_assert("!isnan(-1/0l)   ", !vnl_math_isnan(a6));
  vnl_test_assert("!isfinite(0/0l) ", !vnl_math_isfinite(b3));
  vnl_test_assert("!isinf(0/0l)    ", !vnl_math_isinf(b3));
  vnl_test_assert(" isnan(0/0l)    ",  vnl_math_isnan(b3));
#endif
}

TESTMAIN(test_math);

