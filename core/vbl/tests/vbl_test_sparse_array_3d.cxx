#include <vcl_iostream.h>
#include <vbl/vbl_sparse_array_3d.h>

extern "C"
void Assert(char const* msg, bool expr)
{
  vcl_cout << msg << " - " << (expr?"passed":" *** failed") << "." << vcl_endl;
}

void test_vbl_sparse_array_3d(void)
{
  vbl_sparse_array_3d<double> x;
  double d = 1.23;

  x(1,2,3) = d;
  x(100,200,300) = 100.2003;

  Assert("Something in (1,2,3)", x.fullp(1,2,3));
  Assert("get_addr in (1,2,3)", x.get_addr(1,2,3) != 0);

  Assert("x(1,2,3) == 1.23", x(1,2,3) == d);

  Assert("Something in (100,200,300)", x.fullp(100,200,300));
  Assert("get_addr in (100,200,300)", x.get_addr(100,200,300) != 0);

  Assert("Nothing in (2,3,4) yet", !x.fullp(2,3,4));
  Assert("Still nothing in (2,3,4)", x.get_addr(2,3,4) == 0);
  x.put(2,3,4, 7);
  Assert("Something in (2,3,4) now", x.fullp(2,3,4));

  Assert("Thing in (2,3,4) == 7", x(2,3,4) == 7);
}

int main(int,char**)
{
  vcl_cout << "Running" << vcl_endl;
  test_vbl_sparse_array_3d();
  return 0;
}
