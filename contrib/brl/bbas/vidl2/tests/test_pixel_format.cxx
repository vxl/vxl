// This is contrib/brl/bbas/vidl2/tests/test_pixel_format.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vcl_iomanip.h>
#include <vidl2/vidl2_pixel_format.h>


static void test_pixel_format()
{
  vcl_cout << "****************************\n"
           << " Testing vidl2_pixel_format \n"
           << "****************************\n";

  TEST("vidl2_pixel_format_bpp",
       vidl2_pixel_format_bpp(VIDL2_PIXEL_FORMAT_RGB_24),
       vidl2_pixel_traits<VIDL2_PIXEL_FORMAT_RGB_24>::bits_per_pixel() );

  TEST("vidl2_pixel_format_packed",
       vidl2_pixel_format_packed(VIDL2_PIXEL_FORMAT_YUV_422),
       vidl2_pixel_traits<VIDL2_PIXEL_FORMAT_YUV_422>::packed() );

  TEST("vidl2_pixel_format_planar",
       vidl2_pixel_format_planar(VIDL2_PIXEL_FORMAT_YUV_422P),
       vidl2_pixel_traits<VIDL2_PIXEL_FORMAT_YUV_422P>::planar() );

  TEST("vidl2_pixel_format_to_string",
       vidl2_pixel_format_to_string(VIDL2_PIXEL_FORMAT_MONO_8),
       vidl2_pixel_traits<VIDL2_PIXEL_FORMAT_MONO_8>::name() );

  TEST("vidl2_pixel_format_from_string",
       vidl2_pixel_format_from_string(vidl2_pixel_traits<VIDL2_PIXEL_FORMAT_YUV_420P>::name()),
       VIDL2_PIXEL_FORMAT_YUV_420P );

  vcl_cout << "\nSupported pixel types:\n";
  for(vidl2_pixel_format fmt = VIDL2_PIXEL_FORMAT_UNKNOWN;
      fmt != VIDL2_PIXEL_FORMAT_ENUM_END; fmt = vidl2_pixel_format(fmt+1))
  {
    vcl_cout << "  " <<vcl_setw(15) <<fmt <<" "<<vidl2_pixel_format_bpp(fmt)<<" bpp" << '\n';
  }
  vcl_cout << vcl_endl;

}

TESTMAIN(test_pixel_format);

