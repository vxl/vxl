// This is tbl/vipl/tests/vipl_test_erode_disk.cxx

//:
// \file
//  Test of the vipl_erode_disk templated IP classes
//  vipl_erode_disk<vil_image,vil_image,T,T>
//  for T in { unsigned char, unsigned short, float, RGB }.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   12 sept. 2000

#include <vipl/accessors/vipl_accessors_vil_image.h>
#include <vipl/vipl_erode_disk.h>
//#include <vil/vil_rgb_byte.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include "test_driver.h"

#define ALL_TESTS(x,m,p) \
  ONE_TEST(x,p,byte_img,byte_ori,vil_image,unsigned char,76567,m+"_byte"); \
  ONE_TEST(x,p,shrt_img,shrt_ori,vil_image,unsigned short,19530000,m+"_short"); \
/*ONE_TEST(x,p,flot_img,flot_ori,vil_image,float,0,m+"_float"); */ \
/*ONE_TEST(x,p,colr_img,colr_ori,vil_image,vil_rgb_byte,0,m+"_colour") */

int vipl_test_erode_disk() {
  vcl_cout << "Starting vipl_erode_disk tests\n";
  vcl_cout << "Creating test and output images ...";
  vil_image byte_img = CreateTest8bitImage(32,32),  byte_ori = CreateTest8bitImage(32,32);
  vil_image shrt_img = CreateTest16bitImage(32,32), shrt_ori = CreateTest16bitImage(32,32);
#if 0
  vil_image flot_img = CreateTestfloatImage(32,32), flot_ori = CreateTestfloatImage(32,32);
  vil_image colr_img = CreateTest24bitImage(32,32), colr_ori = CreateTest24bitImage(32,32);
  vil_image colr_img = CreateTest3planeImage(32,32),colr_ori = CreateTest3planeImage(32,32);
#endif
  vcl_cout << " done\n";

  vcl_string m = "vipl_erode_disk";
  ALL_TESTS(vipl_erode_disk,m,5);
  return 0;
}

TESTMAIN(vipl_test_erode_disk);
