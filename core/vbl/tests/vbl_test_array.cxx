// This is core/vbl/tests/vbl_test_array.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vbl/vbl_array_1d.h>
#include <vbl/vbl_array_2d.h>
#include <vbl/vbl_array_3d.h>

class X
{
  double x_;
 public:
  double x() const { return x_; }
  X() : x_(0.0) { } // NB default constructor needed for vbl_array_2d<X>
  X(int a, int b) : x_(a + b) { }
  X(float a, float b) : x_(a + b) { }
  X(double a, double b) : x_(a + b) { }
  void method() const { vcl_cout << '[' << x_ << ']' << '\n' << vcl_flush; }
  bool operator==(X const& y) const { return x_ == y.x(); }
};

vcl_ostream& operator<<(vcl_ostream& is, X const& a) { return is << a.x(); }

static void vbl_test_array_1d()
{
  vcl_cout << "\n Testing vbl_array_1d<X>\n+++++++++++++++++++++++++\n\n";

  vbl_array_1d<X> v;

  vcl_cout << "size " << v.size() << '\n'
           << "capacity " << v.capacity() << '\n';

  v.push_back(X(1, 2));
  v.push_back(X(3.0f, 4.0f));
  v.push_back(v.front());
  v.push_back(X(-1.0, 2.0));
  v.push_back(v.back());

  vcl_cout << "\nv = " << v
           << "size " << v.size() << '\n'
           << "capacity " << v.capacity() << '\n';
  TEST("size()", v.size(), 5);
  TEST("capacity()", v.capacity() >= v.size(), true);

  for (vbl_array_1d<X>::const_iterator i=v.begin(); i!=v.end(); ++i)
    (*i).method();

  TEST("element 0 is 3", v[0], X(3,0));
  TEST("element 1 is 7", v[1], X(7,0));
  TEST("element 2 is 3", v[2], X(3,0));
  TEST("element 3 is 1", v[3], X(1,0));
  TEST("element 4 is 1", v[4], X(1,0));
}

static void vbl_test_array_2d()
{
  vcl_cout << "\n Testing vbl_array_2d<X>\n+++++++++++++++++++++++++\n\n";

  vbl_array_2d<X> v(1000,3000);

  vcl_cout << "size " << v.size() << '\n'
           << "rows " << v.rows() << '\n'
           << "columns " << v.columns() << '\n';

  v.resize(2,3);

  v(0,0) = X(1, 2);
  v(0,1) = X(3.0f, 4.0f);
  v(0,2) = X(-2, 1);
  v(1,0) = *(v.begin());
  v(1,1) = X(-1.0, 2.0);
  v(1,2) = v.end()[-4];

  vcl_cout << "\nv =\n" << v
           << "size " << v.size() << '\n'
           << "rows " << v.rows() << '\n'
           << "cols " << v.columns() << '\n';

  TEST("size()", v.size(), 6);
  TEST("rows()", v.rows(), 2);
  TEST("cols()", v.cols(), 3);

  for (vbl_array_2d<X>::const_iterator i=v.begin(); i!=v.end(); ++i)
    (*i).method();

  TEST("element (0,0) is 3", v(0,0), X(3,0));
  TEST("element (0,1) is 7", v(0,1), X(7,0));
  TEST("element (0,2) is -1", v(0,2), X(-1,0));
  TEST("element (1,0) is 3", v(1,0), X(3,0));
  TEST("element (1,1) is 1", v(1,1), X(1,0));
  TEST("element (1,2) is -1", v(1,2), X(-1,0));
}

static void vbl_test_array_3d()
{
  vcl_cout << "\n Testing vbl_array_3d<X>\n+++++++++++++++++++++++++\n\n";

  vbl_array_3d<X> v(10,20,30);

  vcl_cout << "size " << v.size() << '\n'
           << "rows " << v.get_row2_count() << '\n'
           << "cols " << v.get_row3_count() << '\n'
           << "depth " << v.get_row1_count() << '\n';

  v.resize(1,2,3);

  v(0,0,0) = X(1, 2);
  v(0,0,1) = X(3.0f, 4.0f);
  v(0,0,2) = X(-2, 1);
  v(0,1,0) = *(v.begin());
  v(0,1,1) = X(-1.0, 2.0);
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

  for (vbl_array_3d<X>::const_iterator i=v.begin(); i!=v.end(); ++i)
    (*i).method();

  TEST("element (0,0,0) is 3", v(0,0,0), X(3,0));
  TEST("element (0,0,1) is 7", v(0,0,1), X(7,0));
  TEST("element (0,0,2) is -1", v(0,0,2), X(-1,0));
  TEST("element (0,1,0) is 3", v(0,1,0), X(3,0));
  TEST("element (0,1,1) is 1", v(0,1,1), X(1,0));
  TEST("element (0,1,2) is -1", v(0,1,2), X(-1,0));

  X buf[6];
  buf[0] = X(1, 2);
  buf[1] = X(3.0f, 4.0f);
  buf[2] = X(-2, 1);
  buf[3] = X(1, 2);
  buf[4] = X(-1.0, 2.0);
  buf[5] = X(-2, 1);
  vbl_array_3d<X> w(1,2,3,buf);

  TEST("constructor from buffer", w, v);
  w = v;
  TEST("assignment operator", w, v);
  const vbl_array_3d<X> u = v;
  TEST("copy constructor", u, v);

  for (vbl_array_3d<X>::const_iterator i=u.begin(); i!=u.end(); ++i)
    (*i).method();
}

static void vbl_test_array()
{
  vbl_test_array_1d();
  vbl_test_array_2d();
  vbl_test_array_3d();
}

TESTMAIN(vbl_test_array);

#include <vbl/vbl_array_1d.txx>
VBL_ARRAY_1D_INSTANTIATE(X);
#include <vbl/vbl_array_2d.txx>
VBL_ARRAY_2D_INSTANTIATE(X);
#include <vbl/vbl_array_3d.txx>
VBL_ARRAY_3D_INSTANTIATE(X);
