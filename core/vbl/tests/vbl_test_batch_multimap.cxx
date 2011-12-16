// This is core/vbl/tests/vbl_test_batch_multimap.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vbl/vbl_batch_multimap.h>


namespace
{
  template <typename CI>
  bool issorted(CI begin, CI end)
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

  TEST("batch_multimap sorted data set correctly", issorted(bmmap.begin(), bmmap.end()), true);

  TEST("batch_multimap::size", bmmap.size(), 7);
  TEST("batch_multimap::find", bmmap.find("-7")->second, -7);
  TEST("batch_multimap::count", bmmap.count("3"), 3);
  TEST("batch_multimap::equal_range", bmmap.equal_range("3"),
       vcl_make_pair(bmmap.lower_bound("3"), bmmap.upper_bound("3")));
  TEST("batch_multimap::lower_bound", (bmmap.lower_bound("3")-1)->second, 2);
  TEST("batch_multimap::upper_bound", (bmmap.upper_bound("3"))->second, 5);


  vbl_batch_multimap<vcl_string, int> bmmap2;
  bmmap2.assign(test_data.begin(), test_data.end());
  TEST("batch_multimap::assign && operator ==", bmmap == bmmap2, true);

  vcl_sort(test_data.begin(), test_data.end());
  bmmap2.assign_sorted(test_data.begin(), test_data.end());
  TEST("batch_multimap::assign_sorted && operator ==", bmmap == bmmap2, true);

  vbl_batch_multimap<vcl_string, int> bmmap3 = bmmap2;
  bmmap3.assign_sorted(test_data.begin(), test_data.end());
  TEST("batch_multimap::operator=", bmmap, bmmap3);
}

TESTMAIN(vbl_test_batch_multimap);
