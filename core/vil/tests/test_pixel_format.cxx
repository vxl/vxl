// This is mul/vil2/tests/test_pixel_format.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte
#include <vil2/vil2_rgb.h>
#include <vil2/vil2_pixel_format.h>

int test_pixel_format_main( int /*argc*/, char* /*argv*/[] )
{
  vcl_cout << "***************************\n"
           << " Testing vil2_pixel_format\n"
           << "***************************\n";

  TEST("vil2_pixel_format_of(bool)", vil2_pixel_format_of(bool()), VIL2_PIXEL_FORMAT_BOOL);

  TEST("vil2_pixel_format_of(float)", vil2_pixel_format_of(float()), VIL2_PIXEL_FORMAT_FLOAT);

  TEST("vil2_pixel_format_of(vil2_rgb<float>) has 3 components",
       vil2_pixel_format_num_components(vil2_pixel_format_of(vil2_rgb<float>())), 3);

  TEST("vil2_pixel_format_component_format(vxl_byte)",
       vil2_pixel_format_component_format(vil2_pixel_format_of(vxl_byte())),
       VIL2_PIXEL_FORMAT_BYTE);

  TEST("vil2_pixel_format_component_format(vil2_rgba<vxl_byte>)",
       vil2_pixel_format_component_format(vil2_pixel_format_of(vil2_rgba<vxl_byte>())),
       VIL2_PIXEL_FORMAT_BYTE);

  vcl_cout << VIL2_PIXEL_FORMAT_RGBA_DOUBLE << '\n' << 
              VIL2_PIXEL_FORMAT_RGBA_INT_32 << '\n' <<
              VIL2_PIXEL_FORMAT_RGBA_UINT_32 << '\n' <<
              VIL2_PIXEL_FORMAT_RGBA_INT_16 << '\n' <<
              VIL2_PIXEL_FORMAT_RGBA_UINT_16 << '\n' <<
              VIL2_PIXEL_FORMAT_RGBA_BYTE << '\n' <<
              VIL2_PIXEL_FORMAT_RGBA_SBYTE << '\n' <<
              VIL2_PIXEL_FORMAT_RGBA_FLOAT << '\n' <<
              VIL2_PIXEL_FORMAT_RGBA_DOUBLE << vcl_endl;
  return testlib_test_summary();
}
