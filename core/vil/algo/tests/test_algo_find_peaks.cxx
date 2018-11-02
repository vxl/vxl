// This is core/vil/algo/tests/test_algo_find_peaks.cxx
#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vxl_config.h> // for vxl_byte
#include <vil/algo/vil_find_peaks.h>

static void test_find_peaks_byte()
{
  std::cout << "************************\n"
           << " Testing vil_find_peaks\n"
           << "************************\n";

  vil_image_view<vxl_byte> image0;
  image0.set_size(10,10);
  image0.fill(10);
  image0(3,7)=18;  // One peak
  image0(7,5)=19;  // Another peak

  std::vector<unsigned> pi,pj;
  vil_find_peaks_3x3(pi,pj,image0,vxl_byte(0));

  TEST("Number of peaks", pi.size(), 2);

  TEST("Peak at (3,7)", pi[1]==3 && pj[1]==7, true);
  TEST("Peak at (7,5)", pi[0]==7 && pj[0]==5, true);
}

static void test_algo_find_peaks()
{
  test_find_peaks_byte();
}

TESTMAIN(test_algo_find_peaks);
