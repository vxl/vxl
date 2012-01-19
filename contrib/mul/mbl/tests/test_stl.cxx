// This is mul/mbl/tests/test_stl.cxx
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_iterator.h>
#include <vcl_utility.h>
#include <vcl_algorithm.h>
#include <testlib/testlib_test.h>
#include <mbl/mbl_stl.h>

static void test_stl_sequence()
{
  vcl_cout << "**************************\n"
           << " Testing mbl_stl_sequence\n"
           << "**************************\n";

  const unsigned int n = 20;
  vcl_vector<double> x(n);

  for (unsigned int i=0;i<n;++i) x[i]=double(i);

  vcl_vector<double> y;

  mbl_stl_sequence_n(vcl_back_inserter(y), n,
    vcl_bind1st(vcl_plus<unsigned>(), 1u), 0u);

  TEST("Generate incremental sequence using mbl_stl_sequence_n", y, x);

  mbl_stl_increments(y.begin(), y.end(), 0);
  TEST("Generate incremental sequence using mbl_stl_increments", y, x);
}


static void test_stl_common()
{
  vcl_cout << "********************************\n"
           << " Testing mbl_find_common_values\n"
           << "********************************\n";

  unsigned l1_data[] = { 1, 2, 4, 4, 6 };
  unsigned l2_data[] = { 1, 3, 4, 4, 7 };
  
  vcl_vector<unsigned> l1(l1_data, l1_data + sizeof(l1_data)/sizeof(unsigned));
  vcl_vector<unsigned> l2(l2_data, l2_data + sizeof(l2_data)/sizeof(unsigned));

  
  typedef vcl_pair<vcl_vector<unsigned>::iterator,
    vcl_vector<unsigned>::iterator> it_pair_t;

  it_pair_t it_pair = mbl_stl_find_common_value(l1.begin(), l1.end(), l2.begin(), l2.end());
  TEST("1st find", *it_pair.first == 1 && *it_pair.second == 1, true);

  it_pair = mbl_stl_find_common_value(it_pair.first+1u, l1.end(), it_pair.second, l2.end());
  TEST("2nd find", *it_pair.first == 4 && *it_pair.second == 4, true);

  it_pair = mbl_stl_find_common_value(it_pair.first+1u, l1.end(), it_pair.second, l2.end());
  TEST("3rd find", *it_pair.first == 4 && *it_pair.second == 4, true);

  it_pair = mbl_stl_find_common_value(it_pair.first+1u, l1.end(), it_pair.second, l2.end());
  TEST("4th find", it_pair.first == l1.end() && it_pair.second == l2.end(), true);



  vcl_sort(l1.begin(), l1.end(), vcl_greater<unsigned>());
  vcl_sort(l2.begin(), l2.end(), vcl_greater<unsigned>());

  it_pair = mbl_stl_find_common_value(l1.begin(), l1.end(),
    l2.begin(), l2.end(), vcl_greater<unsigned>());
  TEST("1st find", *it_pair.first == 4 && *it_pair.second == 4, true);

  it_pair = mbl_stl_find_common_value(it_pair.first+1u, l1.end(),
    it_pair.second, l2.end(), vcl_greater<unsigned>());
  TEST("2nd find", *it_pair.first == 4 && *it_pair.second == 4, true);

  it_pair = mbl_stl_find_common_value(it_pair.first+1u, l1.end(),
    it_pair.second, l2.end(), vcl_greater<unsigned>());
  TEST("3rd find", *it_pair.first == 1 && *it_pair.second == 1, true);

  it_pair = mbl_stl_find_common_value(it_pair.first+1u, l1.end(),
    it_pair.second, l2.end(), vcl_greater<unsigned>());
  TEST("4th find", it_pair.first == l1.end() && it_pair.second == l2.end(), true);

}

void test_stl()
{
  test_stl_common();
  test_stl_sequence();
}

TESTMAIN(test_stl);
