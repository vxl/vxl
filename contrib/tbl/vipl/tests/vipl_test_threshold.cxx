//:
// \file
//  Test of the vipl_threshold templated IP classes
//  vipl_threshold<vil_image,vil_image,T,T>
//  for T in { unsigned char, unsigned short, float }.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   12 Sept. 2000

#include <vil/vil_memory_image_of.h>
#include <vil/vil_rgb_byte.h>
#include <vipl/accessors/vipl_accessors_vil_image.h>
#include <vipl/vipl_threshold.h>
#include <vcl_cmath.h>
#include <vcl_cstdlib.h>
#include "test_driver.h"

#define ALL_TESTS(x,m,p) \
  ONE_TEST(x,p,byte_img,byte_ori,vil_image,unsigned char,1298,m+"_byte"); \
  ONE_TEST(x,p,shrt_img,shrt_ori,vil_image,unsigned short,1298,m+"_short"); \
/*ONE_TEST(x,p,flot_img,flot_ori,vil_image,float,0,m+"_float") */

int vipl_test_threshold() {
  vil_image byte_img = CreateTest8bitImage(32,32),  byte_ori = CreateTest8bitImage(32,32);
  vil_image shrt_img = CreateTest16bitImage(32,32), shrt_ori = CreateTest16bitImage(32,32);
  vil_image colr_img = CreateTest24bitImage(32,32), colr_ori = CreateTest24bitImage(32,32);
  vil_image flot_img = CreateTestfloatImage(32,32), flot_ori = CreateTestfloatImage(32,32);
  vil_memory_image_of<float> flot_out(32, 32);
  vil_memory_image_of<unsigned char> byte_out(32, 32);
  vil_memory_image_of<unsigned short> shrt_out(32, 32);
  vil_memory_image_of<vil_rgb_byte> colr_out(32, 32);

  vcl_string m = "vipl_threshold";
  ALL_TESTS(vipl_threshold,m,128);
  return 0;
}

TESTMAIN(vipl_test_threshold);
