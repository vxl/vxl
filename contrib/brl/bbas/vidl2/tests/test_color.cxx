// This is brl/bbas/vidl2/tests/test_color.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vidl2/vidl2_color.h>


//: Define the function pointer for pixel format conversion functions
// These are the functions that use types other than vxl_byte
typedef void (*vidl2_color_conv_fptr_8_16)(const vxl_byte* in, vxl_uint_16* out);
typedef void (*vidl2_color_conv_fptr_16_8)(const vxl_uint_16* in, vxl_byte* out);
typedef void (*vidl2_color_conv_fptr_16_16)(const vxl_uint_16* in, vxl_uint_16* out);
typedef void (*vidl2_color_conv_fptr_8_1)(const vxl_byte* in, bool* out);
typedef void (*vidl2_color_conv_fptr_1_8)(const bool* in, vxl_byte* out);
typedef void (*vidl2_color_conv_fptr_1_16)(const bool* in, vxl_uint_16* out);
typedef void (*vidl2_color_conv_fptr_16_1)(const vxl_uint_16* in, bool* out);
typedef void (*vidl2_color_conv_fptr_1_1)(const bool* in, bool* out);



static void test_color()
{
  vcl_cout << "*********************\n"
           << " Testing vidl2_color\n"
           << "*********************\n";

  // Test vidl2_color_converter_func to make sure it finds the
  // correct function pointers
  {
    bool same_func = true;
    vidl2_color_conv_fptr cf = vidl2_color_converter_func( VIDL2_PIXEL_COLOR_RGB, 24,
                                                           VIDL2_PIXEL_COLOR_RGBA, 32);
    same_func = cf == &vidl2_color_converter<VIDL2_PIXEL_COLOR_RGB,
                                             VIDL2_PIXEL_COLOR_RGBA>::convert;
    TEST("vidl2_color_converter_func: RGB to RGBA", same_func, true);

    cf = vidl2_color_converter_func( VIDL2_PIXEL_COLOR_YUV, 24,
                                     VIDL2_PIXEL_COLOR_RGB, 24);
    same_func =
        cf == &vidl2_color_converter<VIDL2_PIXEL_COLOR_YUV,
                                     VIDL2_PIXEL_COLOR_RGB>::convert;
    TEST("vidl2_color_converter_func: YUV to RGB", same_func, true);

    cf = vidl2_color_converter_func( VIDL2_PIXEL_COLOR_MONO, 1,
                                     VIDL2_PIXEL_COLOR_RGB, 24);
    vidl2_color_conv_fptr_1_8 cf_1_8 = &vidl2_color_converter<VIDL2_PIXEL_COLOR_MONO,
                                                              VIDL2_PIXEL_COLOR_RGB>::convert;
    same_func = cf == reinterpret_cast<vidl2_color_conv_fptr>(cf_1_8);
    TEST("vidl2_color_converter_func: MONO_1 to RGB",same_func, true);

    cf = vidl2_color_converter_func( VIDL2_PIXEL_COLOR_MONO, 16,
                                     VIDL2_PIXEL_COLOR_MONO, 1);
    vidl2_color_conv_fptr_16_1 cf_16_1 = &vidl2_color_converter<VIDL2_PIXEL_COLOR_MONO,
                                                                VIDL2_PIXEL_COLOR_MONO>::convert;
    same_func = cf == reinterpret_cast<vidl2_color_conv_fptr>(cf_16_1);
    TEST("vidl2_color_converter_func: MONO_16 to MONO_1",same_func, true);
  }

  {
    vidl2_color_conv_fptr cf = vidl2_color_converter_func( VIDL2_PIXEL_COLOR_RGB, 24,
                                                           VIDL2_PIXEL_COLOR_RGBA, 32);
    assert(cf);

    vxl_byte in[] = { 254, 131, 1 };
    vxl_byte out[4], out2[4];

    cf(in,out);
    vidl2_color_converter<VIDL2_PIXEL_COLOR_RGB,VIDL2_PIXEL_COLOR_RGBA>::convert(in,out2);
    TEST("RGB to RGBA", in[0]==out[0] && in[1]==out[1] && in[2]==out[2] && out[3]==255 &&
                        out[0]==out2[0] && out[1]==out2[1] && out[2]==out2[2] && out[3]==out2[3], true);
  }

  {
    vidl2_color_conv_fptr cf = vidl2_color_converter_func( VIDL2_PIXEL_COLOR_RGBA, 32,
                                                           VIDL2_PIXEL_COLOR_YUV, 24);
    assert(cf);

    vxl_byte rgb[] = { 128, 200, 95, 128 };
    vxl_byte yuv[3], yuv2[3];

    cf(rgb,yuv);
    vidl2_color_converter<VIDL2_PIXEL_COLOR_RGBA,VIDL2_PIXEL_COLOR_YUV>::convert(rgb,yuv2);
    TEST("RGBA to YUV", yuv[0]==yuv2[0] && yuv[1]==yuv2[1] && yuv[2]==yuv2[2], true);
  }

  {
    vidl2_color_conv_fptr cf = vidl2_color_converter_func( VIDL2_PIXEL_COLOR_RGB, 24,
                                                           VIDL2_PIXEL_COLOR_MONO, 16);
    assert(cf);

    vxl_byte in[] = { 128, 200, 95 };
    vxl_uint_16 out[1], out2[1];

    cf(in,reinterpret_cast<vxl_byte*>(out));
    vidl2_color_converter<VIDL2_PIXEL_COLOR_RGB,VIDL2_PIXEL_COLOR_MONO>::convert(in,out2);
    TEST("RGB to MONO_16", out[0]==out2[0], true);
  }
}

TESTMAIN(test_color);
