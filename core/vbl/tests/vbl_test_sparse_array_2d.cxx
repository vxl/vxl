#include <vcl_iostream.h>
#include <vbl/vbl_sparse_array_2d.h>

extern "C"
void Assert(char const* msg, bool expr)
{
  vcl_cout << msg << " - " << (expr?"passed":" *** failed") << "." << vcl_endl;
}

void test_vbl_sparse_array_2d(void)
{
  vbl_sparse_array_2d<double> x;
  double d = 1.23;

  x(1,2) = d;
  x(100,200) = 100.2003;

  Assert("Something in (1,2)", x.fullp(1,2));
  Assert("get_addr in (1,2)", x.get_addr(1,2) != 0);

  Assert("x(1,2) == 1.23", x(1,2) == d);

  Assert("Something in (100,200)", x.fullp(100,200));
  Assert("get_addr in (100,200)", x.get_addr(100,200) != 0);

  Assert("Nothing in (2,3) yet", !x.fullp(2,3));
  Assert("Still nothing in (2,3)", x.get_addr(2,3) == 0);
  x.put(2,3, 7);
  Assert("Something in (2,3) now", x.fullp(2,3));

  Assert("Thing in (2,3) == 7", x(2,3) == 7);
}

int main(int,char**)
{
  vcl_cout << "Running" << vcl_endl;
  test_vbl_sparse_array_2d();
  return 0;
}
