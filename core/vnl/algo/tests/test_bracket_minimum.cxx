#include <vcl_iostream.h>
#include <vnl/vnl_vector.h>
#include <vnl/algo/vnl_bracket_minimum.h>

#include <testlib/testlib_test.h>

struct bm_square1 : public vnl_cost_function {
  bm_square1() : vnl_cost_function(1) {}

  double f(const vnl_vector<double>& x) {
    vcl_cout << ' ' << x[0]<<vcl_endl;
    return (2 - x[0]) * (2 - x[0]) + 10;
  }
};

void test_bracket_minimum()
{
  bm_square1 f;
  double a=5,b=6,c;
  double fa,fb,fc;

  vnl_bracket_minimum(f,a,b,c,fa,fb,fc);

  vcl_cout<<"Bracket: ("<<a<<","<<b<<","<<c<<")"<<vcl_endl;
  vcl_cout<<"fn: ("<<fa<<","<<fb<<","<<fc<<")"<<vcl_endl;

  TEST("a<b",a<b,true);
  TEST("a<c",a<c,true);
  TEST("fa>fb",fa>fb,true);
  TEST("fb<fc",fb<fc,true);

  a = -10; b=-9;
  vnl_bracket_minimum(f,a,b,c,fa,fb,fc);
  vcl_cout<<"Bracket: ("<<a<<","<<b<<","<<c<<")"<<vcl_endl;
  vcl_cout<<"fn: ("<<fa<<","<<fb<<","<<fc<<")"<<vcl_endl;

  TEST("a<b",a<b,true);
  TEST("a<c",a<c,true);
  TEST("fa>fb",fa>fb,true);
  TEST("fb<fc",fb<fc,true);
}

TESTMAIN(test_bracket_minimum);
