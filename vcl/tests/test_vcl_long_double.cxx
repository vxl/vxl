#include <vcl/vcl_compiler.h>

#define vcl_long_double VCL_long_double // FIXIT

vcl_long_double function(vcl_long_double a)
{
  vcl_long_double b = (1 - a)*(1 + a + a*a);
  return b;
}

int main(int, char **)
{
  vcl_long_double a = 2.0;
  vcl_long_double b = function(a);
  a = (1 - a*a*a) / b;
  return 0;
}
