// This is core/vidl/tests/test_pixel_format.cxx
#include <iostream>
#include <iomanip>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vidl/vidl_pixel_format.h>


static void test_pixel_format()
{
  std::cout << "****************************\n"
           << " Testing vidl_pixel_format\n"
           << "****************************\n";

  TEST("vidl_pixel_format_bpp",
       vidl_pixel_format_bpp(VIDL_PIXEL_FORMAT_RGB_24),
       vidl_pixel_traits_of<VIDL_PIXEL_FORMAT_RGB_24>::bits_per_pixel );

  TEST("vidl_pixel_format_num_channels",
       vidl_pixel_format_num_channels(VIDL_PIXEL_FORMAT_RGB_24),
       vidl_pixel_traits_of<VIDL_PIXEL_FORMAT_RGB_24>::num_channels );

  TEST("vidl_pixel_format_color",
       vidl_pixel_format_color(VIDL_PIXEL_FORMAT_YUYV_422),
       vidl_pixel_traits_of<VIDL_PIXEL_FORMAT_YUYV_422>::color() );

  TEST("vidl_pixel_format_arrangement",
       vidl_pixel_format_arrangement(VIDL_PIXEL_FORMAT_YUV_422P),
       vidl_pixel_traits_of<VIDL_PIXEL_FORMAT_YUV_422P>::arrangement() );

  TEST("vidl_pixel_format_chroma_shift_x",
       vidl_pixel_format_chroma_shift_x(VIDL_PIXEL_FORMAT_YUV_422P),
       vidl_pixel_traits_of<VIDL_PIXEL_FORMAT_YUV_422P>::chroma_shift_x );

  TEST("vidl_pixel_format_chroma_shift_y",
       vidl_pixel_format_chroma_shift_y(VIDL_PIXEL_FORMAT_YUV_410P),
       vidl_pixel_traits_of<VIDL_PIXEL_FORMAT_YUV_410P>::chroma_shift_y );

  TEST("vidl_pixel_format_to_string",
       vidl_pixel_format_to_string(VIDL_PIXEL_FORMAT_MONO_8),
       vidl_pixel_traits_of<VIDL_PIXEL_FORMAT_MONO_8>::name() );

  TEST("vidl_pixel_format_from_string",
       vidl_pixel_format_from_string(vidl_pixel_traits_of<VIDL_PIXEL_FORMAT_YUV_420P>::name()),
       VIDL_PIXEL_FORMAT_YUV_420P );

  std::cout << "\nSupported pixel types:\n";
  for (vidl_pixel_format fmt = VIDL_PIXEL_FORMAT_UNKNOWN;
       fmt != VIDL_PIXEL_FORMAT_ENUM_END; fmt = vidl_pixel_format(fmt+1))
  {
    std::cout << "  " <<std::setw(15) <<fmt <<' '<<vidl_pixel_format_bpp(fmt)<<" bpp\n";
  }
  std::cout << std::endl;
}

TESTMAIN(test_pixel_format);
