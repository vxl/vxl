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

  vcl_cout << "n=" << n << vcl_endl;
  vcl_cout << "f=" << f << vcl_endl;
  vcl_cout << "d=" << d << vcl_endl;
  vcl_cout << "i=" << i << vcl_endl;
  vcl_cout << "z=" << z << vcl_endl;
  vcl_cout << "exp(d*i)=" << e_ipi << vcl_endl;
  vcl_cout << vcl_endl;

  vcl_cout << "abs(n)=" << vnl_math_abs(n) << vcl_endl;
  vcl_cout << "abs(f)=" << vnl_math_abs(f) << vcl_endl;
  vcl_cout << "abs(d)=" << vnl_math_abs(d) << vcl_endl;
  vcl_cout << "abs(i)=" << vnl_math_abs(i) << vcl_endl;
  vcl_cout << "abs(z)=" << vnl_math_abs(z) << vcl_endl;
  vcl_cout <<"norm(z)=" << vnl_math_squared_magnitude(z) << vcl_endl;
  vcl_cout << vcl_endl;

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

#ifndef VCL_WIN32
  float a1 = 1.0f/0.0f; // compiler warning
  float a2 = 0.0f/0.0f; // compiler warning
  double a3 = 1.0/0.0; // compiler warning
  double a4 = 0.0/0.0; // compiler warning
#else // construct IEEE floats NaN and Inf
  unsigned int i1 = 0x7f800000;
  float a1 = *((float*)(&i1));
  unsigned int i2 = 0x7fc00000;
  float a2 = *((float*)(&i2));
  unsigned int i3[] = {0, 0x7ff00000 }; // little endian
  double a3 = *((double*)i3);
  unsigned int i4[] = {0, 0x7ff80000 }; // little endian
  double a4 = *((double*)i4);
#endif

  vnl_test_assert(" isfinite(f)  ",  vnl_math_isfinite(f));
  vnl_test_assert(" isfinite(d)  ",  vnl_math_isfinite(d));
  vnl_test_assert(" isfinite(i)  ",  vnl_math_isfinite(i));
  vnl_test_assert(" isfinite(z)  ",  vnl_math_isfinite(z));
  vnl_test_assert("!isfinite(1/0)", !vnl_math_isfinite(a1));
  vnl_test_assert("!isfinite(0/0)", !vnl_math_isfinite(a2));
  vnl_test_assert("!isfinite(1/0)", !vnl_math_isfinite(a3));
  vnl_test_assert("!isfinite(0/0)", !vnl_math_isfinite(a4));
  vnl_test_assert(" isinf(1/0)   ",  vnl_math_isinf(a1));
  vnl_test_assert("!isinf(0/0)   ", !vnl_math_isinf(a2));
  vnl_test_assert(" isinf(1/0)   ",  vnl_math_isinf(a3));
  vnl_test_assert("!isinf(0/0)   ", !vnl_math_isinf(a4));
  vnl_test_assert("!isnan(1/0)   ", !vnl_math_isnan(a1));
  vnl_test_assert(" isnan(0/0)   ",  vnl_math_isnan(a2));
  vnl_test_assert("!isnan(1/0)   ", !vnl_math_isnan(a3));
  vnl_test_assert(" isnan(0/0)   ",  vnl_math_isnan(a4));
}

TESTMAIN(test_math);
