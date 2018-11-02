// This is core/vil/algo/tests/test_algo_find_plateaus.cxx
#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vxl_config.h> // for vxl_byte
#include <vil/algo/vil_find_plateaus.h>

static void test_find_plateaus_byte()
{
  std::cout << "***************************\n"
           << " Testing vil_find_plateaus\n"
           << "***************************\n";

  vil_image_view<vxl_byte> image0;
  image0.set_size(10,10);
  image0.fill(5);
  image0(3,7)=18;  // A peak
  image0(7,5)=19;  // Another peak
  image0(4,3)=20;  // A plateau point
  image0(4,4)=20;  // A plateau point

  std::vector<unsigned> pi, pj;
  vil_find_plateaus_3x3(pi, pj, image0, vxl_byte(10)); // Ignore pixels below 10

  const size_t nplat = pi.size();
  TEST("Number of plateaus==4", nplat, 4);

  TEST("Plateau at (4,3)", pi[0]==4 && pj[0]==3, true);
  TEST("Plateau at (4,4)", pi[1]==4 && pj[1]==4, true);
  TEST("Plateau at (7,5)", pi[2]==7 && pj[2]==5, true);
  TEST("Plateau at (3,7)", pi[3]==3 && pj[3]==7, true);
}

static void test_algo_find_plateaus()
{
  test_find_plateaus_byte();
}

TESTMAIN(test_algo_find_plateaus);
