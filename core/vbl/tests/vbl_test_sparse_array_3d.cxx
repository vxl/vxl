#include <testlib/testlib_test.h>
#include <vbl/vbl_sparse_array_3d.h>

static void vbl_test_sparse_array_3d()
{
  vbl_sparse_array_3d<double> x;
  double d = 1.23;

  x(1,2,3) = d;
  x(100,200,300) = 100.2003;

  TEST("Something in (1,2,3)", x.fullp(1,2,3), true);
  TEST("get_addr in (1,2,3)", x.get_addr(1,2,3) != 0, true);

  TEST("x(1,2,3) == 1.23", x(1,2,3), d);

  TEST("Something in (100,200,300)", x.fullp(100,200,300), true);
  TEST("get_addr in (100,200,300)", x.get_addr(100,200,300) != 0, true);

  TEST("Nothing in (2,3,4) yet", x.fullp(2,3,4), false);
  TEST("Still nothing in (2,3,4)", x.get_addr(2,3,4), 0);
  x.put(2,3,4, 7);
  TEST("Something in (2,3,4) now", x.fullp(2,3,4), true);

  TEST("Thing in (2,3,4) == 7", x(2,3,4), 7);
}

TESTMAIN(vbl_test_sparse_array_3d);
