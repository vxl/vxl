#include <testlib/testlib_root_dir.h>
#include <testlib/testlib_test.h>

#include <bstm_multi/basic/array_4d.h>

void test_array_4d() {

  TEST("test cmp function", cmp(4, 5), -1);
  TEST("test cmp function", cmp(5, 5), 0);
  TEST("test cmp function", cmp(6, 5), 1);

  vec4 a(0, 1, 2, 3);
  vec4 b(0, 1, 1, 30);
  vec4 c(0, 1, 0, 3);
  vec4 c2(0, 1, 0, 3);
  vec4 d(0, 1, 0, 4);
  TEST("vec4 equality", c, c2);
  TEST("vec4 inequality", (c != d), true);
  TEST("vec4 inequality", (c != a), true);
  TEST("vec4 less than", (b < a), true);
  TEST("vec4 greater than", (a > c), true);

  double *buffer = new double[2 * 3 * 4 * 5];
  array_4d<double> arr(buffer, 2, 3, 4, 5);
  TEST("test coords to index", arr.index_from_coords(0, 0, 0, 3), 3);
  TEST("test coords to index", arr.index_from_coords(0, 0, 2, 3), 3 + 2 * 5);
  TEST("test coords to index", arr.index_from_coords(1, 0, 2, 3), 73);
  TEST("test coords to index", arr.index_from_coords(0, 1, 2, 3), 33);
  TEST("test index to coords", arr.coords_from_index(3), (vec4(0, 0, 0, 3)));
  TEST("test index to coords",
       arr.coords_from_index(3 + 2 * 5),
       (vec4(0, 0, 2, 3)));
  TEST("test index to coords", arr.coords_from_index(73), (vec4(1, 0, 2, 3)));
  TEST("test index to coords", arr.coords_from_index(33), (vec4(0, 1, 2, 3)));

  arr[10] = 33.0;
  TEST("test array access", arr(arr.coords_from_index(10)), 33.0);
  TEST("test array access", arr[arr.index_from_coords(0, 0, 2, 0)], 33.0);
  delete[] buffer;
}

TESTMAIN(test_array_4d);
