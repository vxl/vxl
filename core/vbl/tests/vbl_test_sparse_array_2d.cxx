#include <testlib/testlib_test.h>
#include <vbl/vbl_sparse_array_2d.h>

static void vbl_test_sparse_array_2d()
{
  vbl_sparse_array_2d<double> x;
  double d = 1.23;
  testlib_test_begin("Putting something in (1,2)");
  x(1,2) = d;
  testlib_test_perform(true);
  testlib_test_begin("Putting something in (100,200)");
  x(100,200) = 100.2003;
  testlib_test_perform(true);
  TEST("Something in (1,2)", x.fullp(1,2), true);
  TEST("Content of x(1,2) is correct", x(1,2), d);
  TEST("Something in (100,200)", x.fullp(100,200), true);
  TEST("Nothing in (2,3) yet", x.fullp(2,3), false);
  x.put(2,3, 7);
  TEST("Something in (2,3) now", x.fullp(2,3), true);
  TEST("Content of x(2,3) is correct", x(2,3), 7);
}

TESTMAIN(vbl_test_sparse_array_2d);
