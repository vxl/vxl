#include <vcl_iostream.h>
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

  double a = 0;
  double b = 1;

  vcl_cout << "simpson integral of x/(1+x^2) from "<< a << " to " << b << " is : "<< 
    simpson_integral.integral(&f, a, b, 100) << "\n";

  vnl_adaptsimpson_integral adaptsimpson_integral;

  vcl_cout << "adaptive simpson integral of x/(1+x^2) from "<< a << " to " << b << " is : "<< 
    adaptsimpson_integral.integral(&f, a, b, 1e-13) << "\n";
   
}

TESTMAIN( test_integral );
