// This is core/vbl/tests/vbl_test_batch_multimap.cxx
#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
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
  std::vector<std::pair<std::string, int> > test_data;

  // All these values should sort correctly even via their string values
  // assuming the char type is ascii.
  test_data.emplace_back(std::string("3"), 3);
  test_data.emplace_back(std::string("1"), 1);
  test_data.emplace_back(std::string("2"), 2);
  test_data.emplace_back(std::string("3"), 3);
  test_data.emplace_back(std::string("-7"), -7);
  test_data.emplace_back(std::string("5"), 5);
  test_data.emplace_back(std::string("3"), 3);


  vbl_batch_multimap<std::string, int> bmmap(test_data.begin(), test_data.end());

  TEST("batch_multimap sorted data set correctly", issorted(bmmap.begin(), bmmap.end()), true);

  TEST("batch_multimap::size", bmmap.size(), 7);
  TEST("batch_multimap::find", bmmap.find("-7")->second, -7);
  TEST("batch_multimap::count", bmmap.count("3"), 3);
  TEST("batch_multimap::equal_range", bmmap.equal_range("3"),
       std::make_pair(bmmap.lower_bound("3"), bmmap.upper_bound("3")));
  TEST("batch_multimap::lower_bound", (bmmap.lower_bound("3")-1)->second, 2);
  TEST("batch_multimap::upper_bound", (bmmap.upper_bound("3"))->second, 5);


  vbl_batch_multimap<std::string, int> bmmap2;
  bmmap2.assign(test_data.begin(), test_data.end());
  TEST("batch_multimap::assign && operator ==", bmmap == bmmap2, true);

  std::sort(test_data.begin(), test_data.end());
  bmmap2.assign_sorted(test_data.begin(), test_data.end());
  TEST("batch_multimap::assign_sorted && operator ==", bmmap == bmmap2, true);

  vbl_batch_multimap<std::string, int> bmmap3 = bmmap2;
  TEST("batch_multimap::operator=", bmmap, bmmap3);
}

static void vbl_test_batch_multimap2()
{
  std::vector<std::pair<int, std::string> > test_data;

  // All these values should sort correctly even via their string values
  // assuming the char type is ascii.
  test_data.emplace_back(3, std::string("3a"));
  test_data.emplace_back(1, std::string("1a"));
  test_data.emplace_back(3, std::string("3b"));
  test_data.emplace_back(2, std::string("2a"));
  test_data.emplace_back(-7, std::string("-7a"));
  test_data.emplace_back(5, std::string("5a"));
  test_data.emplace_back(3, std::string("3c"));
  // make sure there is enough data to force default sort heuristics into faster unstable sort range.
  for (char c='a'; c<'z'; c++)
  {
    test_data.emplace_back(6, std::string("6")+c);
    test_data.emplace_back(7, std::string("7")+c);
    test_data.emplace_back(8, std::string("8")+c);
  }

  vbl_batch_multimap<int, std::string> bmmap(test_data.begin(), test_data.end());

 TEST("Check ordinary default sort used by assign messed up value order",
    issorted(bmmap.lower_bound(-10), bmmap.upper_bound(10)), false);

  TEST("batch_multimap::size", bmmap.size(), 82);
  TEST("batch_multimap::find", bmmap.find(-7)->second, "-7a");
  TEST("batch_multimap::count", bmmap.count(3), 3);
  TEST("batch_multimap::equal_range", bmmap.equal_range(3),
       std::make_pair(bmmap.lower_bound(3), bmmap.upper_bound(3)));
  TEST("batch_multimap::lower_bound", (bmmap.lower_bound(3)-1)->second, "2a");
  TEST("batch_multimap::upper_bound", (bmmap.upper_bound(3))->second, "5a");

  vbl_batch_multimap<int, std::string> bmmap2;
  bmmap2.assign(test_data.begin(), test_data.end());
  TEST("batch_multimap::assign && operator ==", bmmap == bmmap2, true);


  std::stable_sort(test_data.begin(), test_data.end(), vbl_batch_multimap<int, std::string>::value_compare_t(std::less<int>()));
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
