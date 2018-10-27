#include <testlib/testlib_root_dir.h>
#include <testlib/testlib_test.h>

#include <bstm_multi/basic/array_4d.h>

void test_array_4d() {


  index_4d a(0, 1, 2, 3);
  index_4d b(0, 1, 1, 30);
  index_4d c(0, 1, 0, 3);
  index_4d c2(0, 1, 0, 3);
  index_4d d(0, 1, 0, 4);
  TEST("index_4d equality", c, c2);
  TEST("index_4d inequality", (c != d), true);
  TEST("index_4d inequality", (c != a), true);

  auto *buffer = new double[2 * 3 * 4 * 5];
  array_4d<double> arr(buffer, 2, 3, 4, 5);
  TEST("test coords to index", arr.index_from_coords(0, 0, 0, 3), 3);
  TEST("test coords to index", arr.index_from_coords(0, 0, 2, 3), 3 + 2 * 5);
  TEST("test coords to index", arr.index_from_coords(1, 0, 2, 3), 73);
  TEST("test coords to index", arr.index_from_coords(0, 1, 2, 3), 33);
  TEST("test index to coords", arr.coords_from_index(3), (index_4d(0, 0, 0, 3)));
  TEST("test index to coords",
       arr.coords_from_index(3 + 2 * 5),
       (index_4d(0, 0, 2, 3)));
  TEST("test index to coords", arr.coords_from_index(73), (index_4d(1, 0, 2, 3)));
  TEST("test index to coords", arr.coords_from_index(33), (index_4d(0, 1, 2, 3)));

  arr[10] = 33.0;
  TEST("test array access", arr(arr.coords_from_index(10)), 33.0);
  TEST("test array access", arr[arr.index_from_coords(0, 0, 2, 0)], 33.0);
  delete[] buffer;
}

TESTMAIN(test_array_4d);
