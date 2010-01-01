#include <testlib/testlib_test.h>
#include <vbl/vbl_sparse_array_3d.h>

static void vbl_test_sparse_array_3d()
{
  vbl_sparse_array_3d<double> x;
  double d = 1.23;
  testlib_test_begin("Putting something in (1,2,3)");
  x(1,2,3) = d;
  testlib_test_perform(true);
  testlib_test_begin("Putting something in (100,200,300)");
  x(100,200,300) = 100.2003;
  testlib_test_perform(true);
  TEST("Something in (1,2,3)", x.fullp(1,2,3), true);
  TEST("Content of x(1,2,3) is correct", x(1,2,3), d);
  TEST("Something in (100,200,300)", x.fullp(100,200,300), true);
  TEST("Nothing in (2,3,4) yet", x.fullp(2,3,4), false);
  x.put(2,3,4, 7);
  TEST("Something in (2,3,4) now", x.fullp(2,3,4), true);
  TEST("Content of x(2,3,4) is correct", x(2,3,4), 7);
}

TESTMAIN(vbl_test_sparse_array_3d);
