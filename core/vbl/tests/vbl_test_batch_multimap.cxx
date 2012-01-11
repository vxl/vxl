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

static void vbl_test_batch_multimap1()
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
  TEST("batch_multimap::operator=", bmmap, bmmap3);
}

static void vbl_test_batch_multimap2()
{
  vcl_vector<vcl_pair<int, vcl_string> > test_data;

  // All these values should sort correctly even via their string values
  // assuming the char type is ascii.
  test_data.push_back(vcl_make_pair(3, "3a"));
  test_data.push_back(vcl_make_pair(1, "1a"));
  test_data.push_back(vcl_make_pair(3, "3b"));
  test_data.push_back(vcl_make_pair(2, "2a"));
  test_data.push_back(vcl_make_pair(-7, "-7a"));
  test_data.push_back(vcl_make_pair(5, "5a"));
  test_data.push_back(vcl_make_pair(3, "3c"));
  // make sure there is enough data to force default sort heuristics into faster unstable sort range.
  for (char c='a'; c<'z'; c++)
  {
    test_data.push_back(vcl_make_pair(6, vcl_string("6")+c));
    test_data.push_back(vcl_make_pair(7, vcl_string("7")+c));
    test_data.push_back(vcl_make_pair(8, vcl_string("8")+c));
  }

  vbl_batch_multimap<int, vcl_string> bmmap(test_data.begin(), test_data.end());

 TEST("Check ordinary default sort used by assign messed up value order",
    issorted(bmmap.lower_bound(-10), bmmap.upper_bound(10)), false);

  TEST("batch_multimap::size", bmmap.size(), 82);
  TEST("batch_multimap::find", bmmap.find(-7)->second, "-7a");
  TEST("batch_multimap::count", bmmap.count(3), 3);
  TEST("batch_multimap::equal_range", bmmap.equal_range(3),
       vcl_make_pair(bmmap.lower_bound(3), bmmap.upper_bound(3)));
  TEST("batch_multimap::lower_bound", (bmmap.lower_bound(3)-1)->second, "2a");
  TEST("batch_multimap::upper_bound", (bmmap.upper_bound(3))->second, "5a");

  vbl_batch_multimap<int, vcl_string> bmmap2;
  bmmap2.assign(test_data.begin(), test_data.end());
  TEST("batch_multimap::assign && operator ==", bmmap == bmmap2, true);

 
  vcl_stable_sort(test_data.begin(), test_data.end(), vbl_batch_multimap<int, vcl_string>::value_compare_t(vcl_less<int>()));
  bmmap2.assign_sorted(test_data.begin(), test_data.end());
  TEST("Check assign_sorted() kept stable sort value order",
    issorted(bmmap2.lower_bound(-10), bmmap2.upper_bound(10)), true);

}

static void vbl_test_batch_multimap()
{
  vbl_test_batch_multimap1();
  vbl_test_batch_multimap2();
}

TESTMAIN(vbl_test_batch_multimap);
