// This is core/vil/algo/tests/test_algo_suppress_non_plateau.cxx
#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/algo/vil_suppress_non_plateau.h>

static void test_algo_suppress_non_plateau_byte()
{
  std::cout << "***************************************\n"
           << " Testing vil_algo_suppress_non_plateau\n"
           << "***************************************\n";

  vil_image_view<vxl_byte> image0,dest_im;
  image0.set_size(10,10);
  image0.fill(5);

  image0(3,7)=18;  // One peak
  image0(7,5)=19;  // Another peak
  image0(4,3)=20;  // A plateau
  image0(4,4)=20;  // A plateau

  vil_suppress_non_plateau_3x3(image0, dest_im, vxl_byte(10)); // Ignore pixels < 10
  TEST("Plateau at (3,7)", dest_im(3,7), 18);
  TEST("Plateau at (7,5)", dest_im(7,5), 19);
  TEST("Plateau at (4,3)", dest_im(4,3), 20);
  TEST("Plateau at (4,4)", dest_im(4,4), 20);

  TEST("No plateau at 2,2", dest_im(2,2), 0);
  TEST("No plateau at 0,1", dest_im(3,3), 0);
}

static void test_algo_suppress_non_plateau()
{
  test_algo_suppress_non_plateau_byte();
}

TESTMAIN(test_algo_suppress_non_plateau);
