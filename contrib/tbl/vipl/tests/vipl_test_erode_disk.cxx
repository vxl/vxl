// This is tbl/vipl/tests/vipl_test_erode_disk.cxx

//:
// \file
//  Test of the vipl_erode_disk templated IP classes
//  vipl_erode_disk<vil_image_view<T>,vil_image_view<T>,T,T>
//  for T in { vxl_byte, short, float, RGB }.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   12 sept. 2000
//
// \verbatim
// Modifications:
//   Peter Vanroose, Feb.2004 - replaced vil1_image by vil2_image_view<T>
// \endverbatim

#include <vipl/accessors/vipl_accessors_vil_image_view.h>
#include <vipl/vipl_erode_disk.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include "test_driver.h"
#include <vxl_config.h> // for vxl_byte

#define ALL_TESTS(x,m,p) \
  ONE_TEST(x,p,byte_img,byte_ori,vil_image_view<vxl_byte>,vxl_byte,76567,m+"_byte"); \
  ONE_TEST(x,p,shrt_img,shrt_ori,vil_image_view<short>,short,3383,m+"_short"); \
/*ONE_TEST(x,p,flot_img,flot_ori,vil_image_view<float>,float,0,m+"_float"); */

int vipl_test_erode_disk()
{
  vcl_cout << "Starting vipl_erode_disk tests\n"
           << "Creating test and output images ...";
  vil_image_view<vxl_byte> byte_img = CreateTest8bitImage(32,32),  byte_ori = CreateTest8bitImage(32,32);
  vil_image_view<short> shrt_img = CreateTest16bitImage(32,32), shrt_ori = CreateTest16bitImage(32,32);
#if 0
  vil_image_view<float> flot_img = CreateTestfloatImage(32,32), flot_ori = CreateTestfloatImage(32,32);
  vil_image_view<vxl_byte> colr_img = CreateTest24bitImage(32,32), colr_ori = CreateTest24bitImage(32,32);
  vil_image_view<vxl_byte> colr_img = CreateTest3planeImage(32,32),colr_ori = CreateTest3planeImage(32,32);
#endif
  vcl_cout << " done\n";

  vcl_string m = "vipl_erode_disk";
  ALL_TESTS(vipl_erode_disk,m,5);
  return 0;
}

TESTMAIN(vipl_test_erode_disk);
