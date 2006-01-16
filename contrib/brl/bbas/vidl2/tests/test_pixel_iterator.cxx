// This is brl/bbas/vidl2/tests/test_pixel_iterator.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vidl2/vidl2_pixel_iterator.h>


static void test_pixel_iterator()
{
  vcl_cout << "******************************\n"
           << " Testing vidl2_pixel_iterator\n"
           << "******************************\n";

  {
    // The test buffer below contains 4 pixels encoded in YUV 4:2:2
    //                  | U  Y0 V Y1| U  Y0 V  Y1 |
    vxl_byte buffer[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    vidl2_pixel_iterator<VIDL2_PIXEL_FORMAT_YUV_422> itr(buffer);

    bool success = true;
    for(unsigned int i=0; i<4; ++i, ++itr){
      vcl_cout << "YUV = ("<< (int)itr(0) << "," << (int)itr(1) << "," << (int)itr(2) << ")\n";
      success = success && itr(0) == buffer[2*i+1]
          && itr(1) == buffer[i*2-(i%2)*2]
          && itr(2) == buffer[i*2+2-(i%2)*2];
    }
    TEST("vidl2_pixel_iterator (YUV_422)",success,true);
  }

  {
    // The test buffer below contains 4 pixels encoded in YUV 4:2:2
    vxl_byte buffer[] = { 0xA3, 0x7B }; // 1010 0011 0111 1011
    vidl2_pixel_iterator<VIDL2_PIXEL_FORMAT_MONO_1> itr(buffer);
    bool success = true;
    for(unsigned int i=0; i<16; ++i, ++itr){
      vcl_cout << itr(0) << " ";
      success = success && (itr(0) == bool(buffer[i/8] & 128>>i%8));
    }
    vcl_cout << '\n';
    TEST("vidl2_pixel_iterator (Mono 1)",success,true);
  }

}

TESTMAIN(test_pixel_iterator);
