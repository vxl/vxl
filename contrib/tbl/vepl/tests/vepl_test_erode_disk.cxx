//:
// \file
//  Test of the vepl_erode_disk templated IP classes
//  vepl_erode_disk<vil_image,vil_image,T,T>
//  for T in { unsigned char, unsigned short, float, RGB }.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   12 sept. 2000

#include <vepl/vepl_erode_disk.h>
//#include <vil/vil_rgb_byte.h>
#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <vcl_cstdlib.h>
#include <vcl_string.h>
#include "test_driver.h"

#define ALL_TESTS(x,v,m) \
  ONE_TEST(x,byte_img,byte_ori,unsigned char,v,m+"_byte"); \
  ONE_TEST(x,shrt_img,shrt_ori,unsigned short,v,m+"_short"); \
/*ONE_TEST(x,flot_img,flot_ori,float,v,m+"_float"); */ \
/*ONE_TEST(x,colr_img,colr_ori,vil_rgb_byte,v,m+"_colour") */

int vepl_test_erode_disk() {
  vcl_cout << "Starting vepl_erode_disk tests\n";
  vcl_cout << "Creating test and output images ...";
  vil_image byte_img = CreateTest8bitImage(32,32),  byte_ori = CreateTest8bitImage(32,32);
  vil_image shrt_img = CreateTest16bitImage(32,32), shrt_ori = CreateTest16bitImage(32,32);
#if 0
  vil_image flot_img = CreateTestfloatImage(32,32), flot_ori = CreateTestfloatImage(32,32);
  vil_image colr_img = CreateTest24bitImage(32,32), colr_ori = CreateTest24bitImage(32,32);
#endif

  vcl_cout << " done\n";

  vcl_string m = "vepl_erode_disk";
  ALL_TESTS(vepl_erode_disk,0,m);
  return 0;
}

TESTMAIN(vepl_test_erode_disk);
