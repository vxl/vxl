#include <vcl_iostream.h>
#include <bnl/bnl_analytic_integrant.h>
#include <bnl/algo/bnl_simpson_integral.h>

class my_test_integrant : public bnl_analytic_integrant
{
  public:
    double f_(double x) { return x/(1+x*x); }
};

int main()
{

  my_test_integrant f;
  bnl_simpson_integral simpson_integral;

  double a = 0;
  double b = 1;

  vcl_cout << "integral of x/(1+x^2) from "<< a << " to " << b << " is : "<< 
    simpson_integral.integral(&f, a, b, 100) << "\n";
  
}
