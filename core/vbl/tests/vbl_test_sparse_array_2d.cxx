#include <testlib/testlib_test.h>
#include <vbl/vbl_sparse_array_2d.h>

static void vbl_test_sparse_array_2d()
{
  vbl_sparse_array_2d<double> x;
  double d = 1.23;

  x(1,2) = d;
  x(100,200) = 100.2003;

  TEST("Something in (1,2)", x.fullp(1,2), true);
  TEST("get_addr in (1,2)", x.get_addr(1,2) != 0, true);

  TEST("x(1,2) == 1.23", x(1,2), d);

  TEST("Something in (100,200)", x.fullp(100,200), true);
  TEST("get_addr in (100,200)", x.get_addr(100,200) != 0, true);

  TEST("Nothing in (2,3) yet", x.fullp(2,3), false);
  TEST("Still nothing in (2,3)", x.get_addr(2,3), 0);
  x.put(2,3, 7);
  TEST("Something in (2,3) now", x.fullp(2,3), true);

  TEST("Thing in (2,3) == 7", x(2,3), 7);
}

TESTMAIN(vbl_test_sparse_array_2d);
