// This is core/vbl/tests/vbl_test_batch_compact_multimap.cxx
#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/vbl_batch_compact_multimap.h>

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


static void vbl_test_batch_compact_multimap1()
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


  vbl_batch_compact_multimap<std::string, int> bcmmap(test_data.begin(), test_data.end());

  TEST("batch_compact_multimap sorted data set correctly", issorted(bcmmap.keys_begin(), bcmmap.keys_end()), true);
  TEST("batch_compact_multimap sorted data set correctly", issorted(bcmmap.values_begin(), bcmmap.values_end()), true);

  TEST("batch_compact_multimap::size", bcmmap.size(), 7);
  TEST("batch_compact_multimap::find", *bcmmap.find("-7"), -7);
  TEST("batch_compact_multimap::find", bcmmap.find("-8"), bcmmap.values_end());
  TEST("batch_compact_multimap::count", bcmmap.count("3"), 3);
  TEST("batch_compact_multimap::equal_range", bcmmap.equal_range("3"),
    std::make_pair(bcmmap.lower_bound("3"), bcmmap.upper_bound("3")));
  TEST("batch_compact_multimap::lower_bound", bcmmap.lower_bound("--"), bcmmap.values_begin());
  TEST("batch_compact_multimap::lower_bound", *(bcmmap.lower_bound("3")-1), 2);
  TEST("batch_compact_multimap::upper_bound", *(bcmmap.upper_bound("3")), 5);

  vbl_batch_compact_multimap<std::string, int> bcmmap2;
  bcmmap2.assign(test_data.begin(), test_data.end());
  TEST("batch_compact_multimap::assign && operator ==", bcmmap == bcmmap2, true);

  std::sort(test_data.begin(), test_data.end());
  bcmmap2.assign_sorted(test_data.begin(), test_data.end());
  TEST("batch_compact_multimap::assign_sorted && operator ==", bcmmap == bcmmap2, true);

  vbl_batch_compact_multimap<std::string, int> bcmmap3 = bcmmap2;
  bcmmap3.assign_sorted(test_data.begin(), test_data.end());
  TEST("batch_compact_multimap::operator=", bcmmap, bcmmap3);
}


static void vbl_test_batch_compact_multimap2()
{
  std::vector<std::pair<int, std::string> > test_data;

  // All these values should sort correctly even via their string values
  // assuming the char type is ascii.
  test_data.emplace_back(3, std::string("3a"));
  test_data.emplace_back(1, std::string("1a"));
  test_data.emplace_back(3, std::string("3b"));
  test_data.emplace_back(2, std::string("2a"));
  test_data.emplace_back(-7,std::string("-7a"));
  test_data.emplace_back(5, std::string("5a"));
  test_data.emplace_back(3, std::string("3c"));
  // make sure there is enough data to force default sort heuristics into faster unstable sort range.
  for (char c='a'; c<'z'; c++)
  {
    test_data.emplace_back(6, std::string("6")+c);
    test_data.emplace_back(7, std::string("7")+c);
    test_data.emplace_back(8, std::string("8")+c);
  }

  vbl_batch_compact_multimap<int, std::string> bcmmap(test_data.begin(), test_data.end());

  TEST("batch_compact_multimap sorted data set correctly", issorted(bcmmap.keys_begin(), bcmmap.keys_end()), true);
  TEST("batch_compact_multimap sorted data values are scrabled by default sort", issorted(bcmmap.values_begin(), bcmmap.values_end()), false);

  TEST("batch_compact_multimap::size", bcmmap.size(), 82);
  TEST("batch_compact_multimap::find", *bcmmap.find(-7), "-7a");
  TEST("batch_compact_multimap::find", bcmmap.find(-8), bcmmap.values_end());
  TEST("batch_compact_multimap::count", bcmmap.count(3), 3);
  TEST("batch_compact_multimap::equal_range", bcmmap.equal_range(3),
    std::make_pair(bcmmap.lower_bound(3), bcmmap.upper_bound(3)));
  TEST("batch_compact_multimap::lower_bound", bcmmap.lower_bound(-10), bcmmap.values_begin());
  TEST("batch_compact_multimap::lower_bound", *(bcmmap.lower_bound(3)-1), "2a");
  TEST("batch_compact_multimap::upper_bound", *(bcmmap.upper_bound(3)), "5a");

  vbl_batch_compact_multimap<int, std::string> bcmmap2;
  bcmmap2.assign(test_data.begin(), test_data.end());
  TEST("batch_compact_multimap::assign && operator ==", bcmmap == bcmmap2, true);

  std::stable_sort(test_data.begin(), test_data.end(),
    vbl_batch_compact_multimap<int, std::string>::input_compare(std::less<int>()));
  bcmmap2.assign_sorted(test_data.begin(), test_data.end());
  TEST("Check assign_sorted() kept stable sort value order",
    issorted(bcmmap2.lower_bound(-10), bcmmap2.upper_bound(10)), true);

}

static void vbl_test_batch_compact_multimap()
{
  vbl_test_batch_compact_multimap1();
  vbl_test_batch_compact_multimap2();
}

TESTMAIN(vbl_test_batch_compact_multimap);
