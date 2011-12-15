// This is core/vbl/tests/vbl_test_batch_multimap.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vbl/vbl_batch_multimap.h>

/*
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

  vbl_array_2d<vbl_test_array_x> z(0,0); // Create Zero sized array
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
*/
namespace
{
  template <typename CI>
  bool is_sorted(CI begin, CI end)
  {
    if (begin==end) return false; // Since it is for a test, empty is unexpected.
    
    end--;
    for (; begin != end; ++begin)
    {
      if ( *begin > *(begin+1)) return false;
    }
    return true;
  }
}

static void vbl_test_batch_multimap()
{
  vcl_vector<vcl_pair<vcl_string, int> > test_data;
  
  // All these values should sort correctly even via their string values
  // assuming the char type is ascii.
  test_data.push_back(vcl_make_pair("3", 3));
  test_data.push_back(vcl_make_pair("1", 1));
  test_data.push_back(vcl_make_pair("2", 2));
  test_data.push_back(vcl_make_pair("3", 3));
  test_data.push_back(vcl_make_pair("-7", -7));
  test_data.push_back(vcl_make_pair("5", 5));
  test_data.push_back(vcl_make_pair("3", 3));
  
  
  vbl_batch_multimap<vcl_string, int> bmmap(test_data.begin(), test_data.end());
  
  TEST("batch_multimap sorted data set correctly", is_sorted(bmmap.begin(), bmmap.end()), true);
  
  TEST("batch_multimap::size", bmmap.size(), 7);
  TEST("batch_multimap::find", bmmap.find("-7")->second, -7);
  TEST("batch_multimap::count", bmmap.count("3"), 3);
  TEST("batch_multimap::equal_range", bmmap.equal_range("3"),
    vcl_make_pair(bmmap.lower_bound("3"), bmmap.upper_bound("3")));
  TEST("batch_multimap::lower_bound", (bmmap.lower_bound("3")-1)->second, 2);
  TEST("batch_multimap::upper_bound", (bmmap.upper_bound("3"))->second, 5);
}

TESTMAIN(vbl_test_batch_multimap);

