// This is core/vbl/tests/vbl_test_bit_array.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vbl/vbl_bit_array_2d.h>
#include <vbl/vbl_bit_array_3d.h>

static void vbl_test_bit_array_2d()
{
  vcl_cout << "\n Testing vbl_bit_array_2d\n"
           <<   " ++++++++++++++++++++++++\n\n";

  vbl_bit_array_2d v(1000,3000);

  vcl_cout << "size " << v.size() << '\n'
           << "rows " << v.rows() << '\n'
           << "columns " << v.columns() << '\n';

  v.resize(2,3);

  v.put(0,0, 0);
  v.put(1,0, 1);
  v.put(0,1, 1);
  v.put(1,1, 1);
  v.put(0,2, 0);
  v.put(1,2, 0);

  vcl_cout << "\nv =\n" << v
           << "size " << v.size() << '\n'
           << "rows " << v.rows() << '\n'
           << "cols " << v.columns() << '\n';

  TEST("size()", v.size(), 1); // need just 1 byte to store 6 bits
  TEST("rows()", v.rows(), 2);
  TEST("cols()", v.cols(), 3);

  TEST("element (0,0) is 0", v(0,0), 0);
  TEST("element (1,0) is 1", v(1,0), 1);
  TEST("element (0,1) is 1", v(0,1), 1);
  TEST("element (1,1) is 1", v(1,1), 1);
  TEST("element (0,2) is 0", v(0,2), 0);
  TEST("element (1,2) is 0", v(1,2), 0);

  bool buf[6];
  buf[0] = 0;
  buf[1] = 1;
  buf[2] = 1;
  buf[3] = 1;
  buf[4] = 0;
  buf[5] = 0;
  vbl_bit_array_2d w(2,3,buf);

  TEST("constructor from buffer", w, v);
  w = v;
  TEST("assignment operator", w, v);
  const vbl_bit_array_2d u = v;
  TEST("copy constructor", u, v);
}

static void vbl_test_bit_array_3d()
{
  vcl_cout << "\n Testing vbl_bit_array_3d\n"
           <<   " ++++++++++++++++++++++++\n\n";

  vbl_bit_array_3d v(10,20,30);

  vcl_cout << "size " << v.size() << '\n'
           << "rows " << v.row2_count() << '\n'
           << "cols " << v.row3_count() << '\n'
           << "depth " << v.row1_count() << '\n';

  v.resize(1,2,3);

  v.set(0,0,0, 0);
  v.set(0,1,0, 1);
  v.set(0,0,1, 1);
  v.set(0,1,1, 1);
  v.set(0,0,2, 0);
  v.set(0,1,2, 0);

  vcl_cout << "\nv =\n" << v
           << "size " << v.size() << '\n'
           << "rows " << v.row2_count() << '\n'
           << "cols " << v.row3_count() << '\n'
           << "depth " << v.row1_count() << '\n';

  TEST("size()", v.size(), 1); // need just 1 byte to store 6 bits
  TEST("row1_count()", v.row1_count(), 1);
  TEST("row2_count()", v.row2_count(), 2);
  TEST("row3_count()", v.row3_count(), 3);

  TEST("element (0,0,0) is 0", v(0,0,0), 0);
  TEST("element (0,1,0) is 1", v(0,1,0), 1);
  TEST("element (0,0,1) is 1", v(0,0,1), 1);
  TEST("element (0,1,1) is 1", v(0,1,1), 1);
  TEST("element (0,0,2) is 0", v(0,0,2), 0);
  TEST("element (0,1,2) is 0", v(0,1,2), 0);

  bool buf[6];
  buf[0] = 0;
  buf[1] = 1;
  buf[2] = 1;
  buf[3] = 1;
  buf[4] = 0;
  buf[5] = 0;
  vbl_bit_array_3d w(1,2,3,buf);

  TEST("constructor from buffer", w, v);
  w = v;
  TEST("assignment operator", w, v);
  const vbl_bit_array_3d u = v;
  TEST("copy constructor", u, v);
}

static void vbl_test_bit_array()
{
  vbl_test_bit_array_2d();
  vbl_test_bit_array_3d();
}

TESTMAIN(vbl_test_bit_array);
