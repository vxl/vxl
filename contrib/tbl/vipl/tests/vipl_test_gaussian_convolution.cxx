// This is tbl/vipl/tests/vipl_test_gaussian_convolution.cxx

//:
// \file
//  Test of the vipl_gaussian_convolution templated IP classes
//  vipl_gaussian_convolution<mil_image_2d_of<T>,mil_image_2d_of<T>,T,T>
//  for T in { unsigned char, short, float }.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   26 april 2002

#include <vil1/vil1_image.h>
#include <vipl/accessors/vipl_accessors_vil1_image.h>
#include <vipl/vipl_gaussian_convolution.h>
#include "test_driver.h"
typedef unsigned char ubyte;

#define ALL_TESTS(x,m,p) \
  ONE_TEST(x,p,byte_img,byte_ori,vil1_image,ubyte,10705,m+"_byte"); \
  ONE_TEST(x,p,shrt_img,shrt_ori,vil1_image,short,385,m+"_short"); \
  ONE_TEST(x,p,flot_img,flot_ori,vil1_image,float,2.19553,m+"_float")

int vipl_test_gaussian_convolution() {
  vil1_image byte_img = CreateTest8bitImage(32,32),  byte_ori = CreateTest8bitImage(32,32);
  vil1_image shrt_img = CreateTest16bitImage(32,32), shrt_ori = CreateTest16bitImage(32,32);
  vil1_image flot_img = CreateTestfloatImage(32,32), flot_ori = CreateTestfloatImage(32,32);

  vcl_string m = "vipl_gaussian_convolution";
  ALL_TESTS(vipl_gaussian_convolution,m,1.0);

  return 0;
}


TESTMAIN(vipl_test_gaussian_convolution);
