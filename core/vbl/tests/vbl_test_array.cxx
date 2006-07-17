// This is core/vbl/tests/vbl_test_array.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vbl/vbl_array_1d.h>
#include <vbl/vbl_array_2d.h>
#include <vbl/vbl_array_3d.h>

#include "vbl_test_array_x.h"


static void vbl_test_array_1d()
{
  vcl_cout << "\n Testing vbl_array_1d<vbl_test_array_x>\n+++++++++++++++++++++++++\n\n";

  vbl_array_1d<vbl_test_array_x> v;

  vcl_cout << "size " << v.size() << '\n'
           << "capacity " << v.capacity() << '\n';

  v.push_back(vbl_test_array_x(1, 2));
  v.push_back(vbl_test_array_x(3.0f, 4.0f));
  v.push_back(v.front());
  v.push_back(vbl_test_array_x(-1.0, 2.0));
  v.push_back(v.back());

  vcl_cout << "\nv = " << v
           << "size " << v.size() << '\n'
           << "capacity " << v.capacity() << '\n';
  TEST("size()", v.size(), 5);
  TEST("capacity()", v.capacity() >= v.size(), true);

  for (vbl_array_1d<vbl_test_array_x>::const_iterator i=v.begin(); i!=v.end(); ++i)
    (*i).method();

  TEST("element 0 is 3", v[0], vbl_test_array_x(3,0));
  TEST("element 1 is 7", v[1], vbl_test_array_x(7,0));
  TEST("element 2 is 3", v[2], vbl_test_array_x(3,0));
  TEST("element 3 is 1", v[3], vbl_test_array_x(1,0));
  TEST("element 4 is 1", v[4], vbl_test_array_x(1,0));
}

static void vbl_test_array_2d()
{
  vcl_cout << "\n Testing vbl_array_2d<vbl_test_array_x>\n+++++++++++++++++++++++++\n\n";

  vbl_array_2d<vbl_test_array_x> v(1000,3000);

  vcl_cout << "size " << v.size() << '\n'
           << "rows " << v.rows() << '\n'
           << "columns " << v.columns() << '\n';

  v.resize(2,3);

  v(0,0) = vbl_test_array_x(1, 2);
  v(0,1) = vbl_test_array_x(3.0f, 4.0f);
  v(0,2) = vbl_test_array_x(-2, 1);
  v(1,0) = *(v.begin());
  v(1,1) = vbl_test_array_x(-1.0, 2.0);
  v(1,2) = v.end()[-4];

  vcl_cout << "\nv =\n" << v
           << "size " << v.size() << '\n'
           << "rows " << v.rows() << '\n'
           << "cols " << v.columns() << '\n';

  TEST("size()", v.size(), 6);
  TEST("rows()", v.rows(), 2);
  TEST("cols()", v.cols(), 3);

  for (vbl_array_2d<vbl_test_array_x>::const_iterator i=v.begin(); i!=v.end(); ++i)
    (*i).method();

  TEST("element (0,0) is 3", v(0,0), vbl_test_array_x(3,0));
  TEST("element (0,1) is 7", v(0,1), vbl_test_array_x(7,0));
  TEST("element (0,2) is -1", v(0,2), vbl_test_array_x(-1,0));
  TEST("element (1,0) is 3", v(1,0), vbl_test_array_x(3,0));
  TEST("element (1,1) is 1", v(1,1), vbl_test_array_x(1,0));
  TEST("element (1,2) is -1", v(1,2), vbl_test_array_x(-1,0));

  vbl_array_3d<vbl_test_array_x> z(0,0); // Create Zero sized array
}

static void vbl_test_array_3d()
{
  vcl_cout << "\n Testing vbl_array_3d<vbl_test_array_x>\n+++++++++++++++++++++++++\n\n";

  vbl_array_3d<vbl_test_array_x> v(10,20,30);

  vcl_cout << "size " << v.size() << '\n'
           << "rows " << v.get_row2_count() << '\n'
           << "cols " << v.get_row3_count() << '\n'
           << "depth " << v.get_row1_count() << '\n';

  v.resize(1,2,3);

  v(0,0,0) = vbl_test_array_x(1, 2);
  v(0,0,1) = vbl_test_array_x(3.0f, 4.0f);
  v(0,0,2) = vbl_test_array_x(-2, 1);
  v(0,1,0) = *(v.begin());
  v(0,1,1) = vbl_test_array_x(-1.0, 2.0);
  v(0,1,2) = v.end()[-4];

  vcl_cout << "\nv =\n" << v
           << "size " << v.size() << '\n'
           << "rows " << v.get_row2_count() << '\n'
           << "cols " << v.get_row3_count() << '\n'
           << "depth " << v.get_row1_count() << '\n';

  TEST("size()", v.size(), 6);
  TEST("get_row1_count()", v.get_row1_count(), 1);
  TEST("get_row2_count()", v.get_row2_count(), 2);
  TEST("get_row3_count()", v.get_row3_count(), 3);

  for (vbl_array_3d<vbl_test_array_x>::const_iterator i=v.begin(); i!=v.end(); ++i)
    (*i).method();

  TEST("element (0,0,0) is 3", v(0,0,0), vbl_test_array_x(3,0));
  TEST("element (0,0,1) is 7", v(0,0,1), vbl_test_array_x(7,0));
  TEST("element (0,0,2) is -1", v(0,0,2), vbl_test_array_x(-1,0));
  TEST("element (0,1,0) is 3", v(0,1,0), vbl_test_array_x(3,0));
  TEST("element (0,1,1) is 1", v(0,1,1), vbl_test_array_x(1,0));
  TEST("element (0,1,2) is -1", v(0,1,2), vbl_test_array_x(-1,0));

  vbl_test_array_x buf[6];
  buf[0] = vbl_test_array_x(1, 2);
  buf[1] = vbl_test_array_x(3.0f, 4.0f);
  buf[2] = vbl_test_array_x(-2, 1);
  buf[3] = vbl_test_array_x(1, 2);
  buf[4] = vbl_test_array_x(-1.0, 2.0);
  buf[5] = vbl_test_array_x(-2, 1);
  vbl_array_3d<vbl_test_array_x> w(1,2,3,buf);

  TEST("constructor from buffer", w, v);
  w = v;
  TEST("assignment operator", w, v);
  const vbl_array_3d<vbl_test_array_x> u = v;
  TEST("copy constructor", u, v);

  for (vbl_array_3d<vbl_test_array_x>::const_iterator i=u.begin(); i!=u.end(); ++i)
    (*i).method();

  vbl_array_3d<vbl_test_array_x> z(0,0,0); // Create Zero sized array
  
}

static void vbl_test_array()
{
  vbl_test_array_1d();
  vbl_test_array_2d();
  vbl_test_array_3d();
}

TESTMAIN(vbl_test_array);

