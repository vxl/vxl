// This is tbl/vipl/tests/vipl_test_threshold.cxx

//:
// \file
//  Test of the vipl_threshold templated IP classes
//  vipl_threshold<vil_image_view<T>,vil_image_view<T>,T,T>
//  for T in { vxl_byte, short, float }.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   12 Sept. 2000
//
// \verbatim
// Modifications:
//   Peter Vanroose, Feb.2004 - replaced vil1_image by vil2_image_view<T>
// \endverbatim

#include <vipl/accessors/vipl_accessors_vil_image_view.h>
#include <vipl/vipl_threshold.h>
#include "test_driver.h"
#include <vxl_config.h> // for vxl_byte

#define ALL_TESTS(x,m,p) \
  ONE_TEST(x,p,byte_img,byte_ori,vil_image_view<vxl_byte>,vxl_byte,1298,m+"_byte"); \
  ONE_TEST(x,p,shrt_img,shrt_ori,vil_image_view<short>,short,3012,m+"_short"); \
/*ONE_TEST(x,p,flot_img,flot_ori,vil_image_view<float>,float,0,m+"_float") */

int vipl_test_threshold()
{
  vil_image_view<vxl_byte> byte_img = CreateTest8bitImage(32,32),  byte_ori = CreateTest8bitImage(32,32);
  vil_image_view<short> shrt_img = CreateTest16bitImage(32,32), shrt_ori = CreateTest16bitImage(32,32);
//vil_image_view<float> flot_img = CreateTestfloatImage(32,32), flot_ori = CreateTestfloatImage(32,32);

  vcl_string m = "vipl_threshold";
  ALL_TESTS(vipl_threshold,m,128);
  return 0;
}

TESTMAIN(vipl_test_threshold);
