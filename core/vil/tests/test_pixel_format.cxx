// This is mul/vil2/tests/test_pixel_format.cxx
#include <vcl_iostream.h>
#include <testlib/testlib_test.h>
#include <vil/vil_byte.h>
#include <vil2/vil2_pixel_format.h>

int test_pixel_format_main( int argc, char* argv[] )
{
  vcl_cout << "***************************\n"
           << " Testing vil2_pixel_format\n"
           << "***************************\n";

  TEST("vil2_pixel_format_of(byte)", vil2_pixel_format_of(bool(0)), VIL2_PIXEL_FORMAT_BOOL);

  TEST("vil2_pixel_format_of(vil_rgb<vil_byte>) has 3 components", 
    vil2_pixel_format_num_components(vil2_pixel_format_of(vil_rgb<float>(0))), 3);
 
  TEST("vil2_pixel_format_component_format(vil_rgb<vil2_byte>)",
    vil2_pixel_format_component_format(vil2_pixel_format_of(vil_rgba<vil2_byte>(0))), VIL2_PIXEL_FORMAT_BYTE);

  return testlib_test_summary();
}
