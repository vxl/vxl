// This is mul/vil3d/tests/test_algo_make_distance_filter.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vil3d/algo/vil3d_make_distance_filter.h>
#include <vil3d/vil3d_crop.h>

static void test_algo_make_distance_filter()
{
  vcl_cout << "**************************\n"
           << " Testing vil3d_make_distance_filter\n"
           << "**************************\n";

  vil3d_structuring_element se;
  vcl_vector<double> d;
  vil3d_make_distance_filter_r1(1.0,1.0,1.0,se,d);

  TEST("13 elements",d.size(),13);

  vcl_cout<<"Filter points: "<<se<<vcl_endl;
}

TESTMAIN(test_algo_make_distance_filter);
