#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <vnl/vnl_analytic_integrant.h>
#include <vnl/algo/vnl_simpson_integral.h>
#include <vnl/algo/vnl_adaptsimpson_integral.h>
#include <testlib/testlib_test.h>

class my_test_integrant : public vnl_analytic_integrant
{
 public:
  double f_(double x) { return x/(1+x*x); }
};

void test_integral()
{
  my_test_integrant f;
  vnl_simpson_integral simpson_integral;

  float a = 0;
  float b = 1;

  TEST_NEAR("simpson integral of x/(1+x^2) from  0 to 1 is: ",
            simpson_integral.integral(&f, a, b, 100), 0.5*vcl_log(2.0), 1e-6);

  vnl_adaptsimpson_integral adaptsimpson_integral;

  TEST_NEAR("adaptive simpson integral of x/(1+x^2) from 0 to 1 is: ",
            adaptsimpson_integral.integral(&f, a, b, 1e-11f), 0.5*vcl_log(2.0), 1e-6);
}

TESTMAIN( test_integral );
