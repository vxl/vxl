// This is mul/vil3d/tests/test_algo_make_distance_filter.cxx
#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil3d/algo/vil3d_make_distance_filter.h>
#include <vil3d/vil3d_crop.h>

static void test_algo_make_distance_filter()
{
  std::cout << "**************************\n"
           << " Testing vil3d_make_distance_filter\n"
           << "**************************\n";

  vil3d_structuring_element se;
  std::vector<double> d;
  vil3d_make_distance_filter_r1(1.0,1.0,1.0,se,d);

  TEST("13 elements",d.size(),13);

  std::cout<<"Filter points: "<<se<<std::endl;
}

TESTMAIN(test_algo_make_distance_filter);
