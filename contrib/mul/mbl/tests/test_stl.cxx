// This is mul/mbl/tests/test_stl.cxx
#include <iostream>
#include <vector>
#include <iterator>
#include <utility>
#include <algorithm>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <testlib/testlib_test.h>
#include <mbl/mbl_stl.h>

static void test_stl_sequence()
{
  std::cout << "**************************\n"
           << " Testing mbl_stl_sequence\n"
           << "**************************\n";

  constexpr unsigned int n = 20;
  std::vector<double> x(n);

  for (unsigned int i=0;i<n;++i) x[i]=double(i);

  std::vector<double> y;

  mbl_stl_sequence_n(std::back_inserter(y), n,
    std::bind1st(std::plus<unsigned>(), 1u), 0u);

  TEST("Generate incremental sequence using mbl_stl_sequence_n", y, x);

  mbl_stl_increments(y.begin(), y.end(), 0);
  TEST("Generate incremental sequence using mbl_stl_increments", y, x);
}


static void test_stl_common()
{
  std::cout << "********************************\n"
           << " Testing mbl_find_common_values\n"
           << "********************************\n";

  unsigned l1_data[] = { 1, 2, 4, 4, 6 };
  unsigned l2_data[] = { 1, 3, 4, 4, 7 };

  std::vector<unsigned> l1(l1_data, l1_data + sizeof(l1_data)/sizeof(unsigned));
  std::vector<unsigned> l2(l2_data, l2_data + sizeof(l2_data)/sizeof(unsigned));


  typedef std::pair<std::vector<unsigned>::iterator,
    std::vector<unsigned>::iterator> it_pair_t;

  it_pair_t it_pair = mbl_stl_find_common_value(l1.begin(), l1.end(), l2.begin(), l2.end());
  TEST("1st find", *it_pair.first == 1 && *it_pair.second == 1, true);

  it_pair = mbl_stl_find_common_value(it_pair.first+1u, l1.end(), it_pair.second, l2.end());
  TEST("2nd find", *it_pair.first == 4 && *it_pair.second == 4, true);

  it_pair = mbl_stl_find_common_value(it_pair.first+1u, l1.end(), it_pair.second, l2.end());
  TEST("3rd find", *it_pair.first == 4 && *it_pair.second == 4, true);

  it_pair = mbl_stl_find_common_value(it_pair.first+1u, l1.end(), it_pair.second, l2.end());
  TEST("4th find", it_pair.first == l1.end() && it_pair.second == l2.end(), true);



  std::sort(l1.begin(), l1.end(), std::greater<unsigned>());
  std::sort(l2.begin(), l2.end(), std::greater<unsigned>());

  it_pair = mbl_stl_find_common_value(l1.begin(), l1.end(),
    l2.begin(), l2.end(), std::greater<unsigned>());
  TEST("1st find", *it_pair.first == 4 && *it_pair.second == 4, true);

  it_pair = mbl_stl_find_common_value(it_pair.first+1u, l1.end(),
    it_pair.second, l2.end(), std::greater<unsigned>());
  TEST("2nd find", *it_pair.first == 4 && *it_pair.second == 4, true);

  it_pair = mbl_stl_find_common_value(it_pair.first+1u, l1.end(),
    it_pair.second, l2.end(), std::greater<unsigned>());
  TEST("3rd find", *it_pair.first == 1 && *it_pair.second == 1, true);

  it_pair = mbl_stl_find_common_value(it_pair.first+1u, l1.end(),
    it_pair.second, l2.end(), std::greater<unsigned>());
  TEST("4th find", it_pair.first == l1.end() && it_pair.second == l2.end(), true);

}

void test_stl()
{
  test_stl_common();
  test_stl_sequence();
}

TESTMAIN(test_stl);
