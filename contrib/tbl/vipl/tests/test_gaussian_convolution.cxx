//:
// \file
//  Test of the vipl_gaussian_convolution templated IP classes
//  vipl_gaussian_convolution<mil_image_2d_of<T>,mil_image_2d_of<T>,T,T>
//  for T in { unsigned char, short, float }.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   26 april 2002
//
#include <mil/mil_image_2d_of.h>
#include <vipl/vipl_with_mil/accessors/vipl_accessors_mil_image.h>
#include <vipl/vipl_gaussian_convolution.h>
#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <vcl_cstdlib.h>
#include "test_driver.h"
typedef unsigned char ubyte;

#define ALL_TESTS(x,m,p) \
  ONE_TEST(x,p,byte_img,byte_ori,mil_image_2d_of<ubyte>,ubyte,58939,m+"_byte"); \
  ONE_TEST(x,p,shrt_img,shrt_ori,mil_image_2d_of<short>,short,1595,m+"_short"); \
  ONE_TEST(x,p,flot_img,flot_ori,mil_image_2d_of<float>,float,13.4489,m+"_float")

int test_gaussian_convolution() {
  mil_image_2d_of<ubyte> byte_img = Create_mil8bitImage(32,32),  byte_ori = Create_mil8bitImage(32,32);
  mil_image_2d_of<short> shrt_img = Create_mil16bitImage(32,32), shrt_ori = Create_mil16bitImage(32,32);
  mil_image_2d_of<float> flot_img = Create_milfloatImage(32,32), flot_ori = Create_milfloatImage(32,32);
  mil_image_2d_of<ubyte> byte_out(32, 32);
  mil_image_2d_of<short> shrt_out(32, 32);
  mil_image_2d_of<float> flot_out(32, 32);

  vcl_string m = "vipl_gaussian_convolution";
  ALL_TESTS(vipl_gaussian_convolution,m,5);
  return 0;
}


TESTMAIN(test_gaussian_convolution);
