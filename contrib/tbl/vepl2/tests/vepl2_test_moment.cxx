// This is tbl/vepl2/tests/vepl2_test_moment.cxx

//:
// \file
//  Test of the vepl2_moment function.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   7 October 2002, from vepl/tests

#include <vepl2/vepl2_moment.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include "test_driver.h"

int vepl2_test_moment() {
  vcl_cout << "Starting vepl2_moment tests\n";
  vcl_cout << "Creating test and output images ...";
  vil2_image_view_base_sptr flot_img = CreateTestfloatImage(32,32), flot_ori = CreateTestfloatImage(32,32);
  vcl_cout << " done\n";

  vcl_string m = "vepl2_moment";
#define args ,2
  ONE_TEST(vepl2_moment,flot_img,flot_ori,float,283,m+"_float",args);

  return 0;
}

TESTMAIN(vepl2_test_moment);
