// This is tbl/vepl/tests/vepl_test_erode_disk.cxx

//:
// \file
//  Test of the vepl_erode_disk templated IP classes
//  vepl_erode_disk<vil1_image,vil1_image,T,T>
//  for T in { vxl_byte, vxl_uint_16, float, RGB }.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   12 sept. 2000

#include <vepl/vepl_erode_disk.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include "test_driver.h"
#include <vxl_config.h> // for vxl_byte

#define ALL_TESTS(x,v,m) \
  ONE_TEST(x,byte_img,byte_ori,vxl_byte,v,m+"_byte"); \
  ONE_TEST(x,shrt_img,shrt_ori,vxl_uint_16,v,m+"_short"); \
/*ONE_TEST(x,flot_img,flot_ori,float,v,m+"_float"); */ \
/*ONE_TEST(x,colr_img,colr_ori,vil1_rgb_byte,v,m+"_colour") */

int vepl_test_erode_disk()
{
  vcl_cout << "Starting vepl_erode_disk tests\n"
           << "Creating test and output images ...";
  vil1_image byte_img = CreateTest8bitImage(32,32),  byte_ori = CreateTest8bitImage(32,32);
  vil1_image shrt_img = CreateTest16bitImage(32,32), shrt_ori = CreateTest16bitImage(32,32);
#if 0
  vil1_image flot_img = CreateTestfloatImage(32,32), flot_ori = CreateTestfloatImage(32,32);
  vil1_image colr_img = CreateTest24bitImage(32,32), colr_ori = CreateTest24bitImage(32,32);
#endif

  vcl_cout << " done\n";

  vcl_string m = "vepl_erode_disk";
  ALL_TESTS(vepl_erode_disk,0,m);
  return 0;
}

TESTMAIN(vepl_test_erode_disk);
