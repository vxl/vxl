#include <vcl_iostream.h>
#include <bnl/bnl_analytic_integrant.h>

class my_test_integrant : public bnl_integrant_fnct
{
  public:
    double f_(double x) { return x/(1+x*x); }
};

int main()
{
  my_test_integrant f;
  
  vcl_cout << f.f_(1) << "\n";
}
