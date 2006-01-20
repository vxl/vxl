// This is brl/bbas/vidl2/tests/test_pixel_iterator.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vidl2/vidl2_frame.h>
#include <vidl2/vidl2_pixel_iterator.h>


static void test_pixel_iterator()
{
  vcl_cout << "******************************\n"
           << " Testing vidl2_pixel_iterator\n"
           << "******************************\n";

  {
    // The test buffer below contains 3 pixels encoded in RGB 24
    //                  | R  G  B| R  G  B| R  G  B |
    vxl_byte buffer[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    vidl2_frame_sptr frame = new vidl2_shared_frame(buffer,3,1,VIDL2_PIXEL_FORMAT_RGB_24);
    vidl2_pixel_iterator<VIDL2_PIXEL_FORMAT_RGB_24> itr(frame);

    bool success = true;
    for(unsigned int i=0; i<3; ++i, ++itr){
      vcl_cout << "RGB = ("<< (int)itr(0) << "," << (int)itr(1) << "," << (int)itr(2) << ")\n";
      success = success && itr(0) == buffer[3*i]
                        && itr(1) == buffer[3*i+1]
                        && itr(2) == buffer[3*i+2];
    }
    TEST("vidl2_pixel_iterator (RGB 24)",success,true);
  }

  {
    // The test buffer below contains 3 pixels encoded in BGR 24
    //                  | B  G  R| B  G  R| B  G  R |
    vxl_byte buffer[] = { 3, 2, 1, 6, 5, 4, 9, 8, 7 };
    vidl2_frame_sptr frame = new vidl2_shared_frame(buffer,3,1,VIDL2_PIXEL_FORMAT_BGR_24);
    vidl2_pixel_iterator<VIDL2_PIXEL_FORMAT_BGR_24> itr(frame);

    bool success = true;
    for(unsigned int i=0; i<3; ++i, ++itr){
      vcl_cout << "RGB = ("<< (int)itr(0) << "," << (int)itr(1) << "," << (int)itr(2) << ")\n";
      success = success && itr(0) == buffer[3*i+2]
          && itr(1) == buffer[3*i+1]
          && itr(2) == buffer[3*i];
    }
    TEST("vidl2_pixel_iterator (BGR 24)",success,true);
  }

  {
    // The test buffer below contains 3 pixels encoded in RGBA 32
    //                  | R  G  B  A| R  G  B  A| R  G  B  A|
    vxl_byte buffer[] = { 1, 2, 3, 0, 4, 5, 6, 0, 7, 8, 9, 0 };
    vidl2_frame_sptr frame = new vidl2_shared_frame(buffer,3,1,VIDL2_PIXEL_FORMAT_RGBA_32);
    vidl2_pixel_iterator<VIDL2_PIXEL_FORMAT_RGBA_32> itr(frame);

    bool success = true;
    for(unsigned int i=0; i<3; ++i, ++itr){
      vcl_cout << "RGBA = ("<< (int)itr(0) << "," << (int)itr(1)
          << "," << (int)itr(2) << ","<< (int)itr(3) << ")\n";
      success = success && itr(0) == buffer[4*i]
          && itr(1) == buffer[4*i+1]
          && itr(2) == buffer[4*i+2]
          && itr(3) == buffer[4*i+3];
    }
    TEST("vidl2_pixel_iterator (RGBA 32)",success,true);
  }

  {
    // The test buffer below contains 2 pixels encoded in RGB 555
    //                      |RGB_555|RGB_555|
    vxl_uint_16 buffer[] = { 0x7FAF, 0x0103 };
    vidl2_frame_sptr frame = new vidl2_shared_frame(buffer,2,1,VIDL2_PIXEL_FORMAT_RGB_555);
    vidl2_pixel_iterator<VIDL2_PIXEL_FORMAT_RGB_555> itr(frame);

    bool success = true;
    for(unsigned int i=0; i<2; ++i, ++itr){
      vcl_cout << "RGB = ("<< (int)itr(0) << "," << (int)itr(1) << "," << (int)itr(2) << ")\n";
      success = success && itr(0) == vxl_byte((buffer[i] >> 10)&31)<<3
          && itr(1) == vxl_byte((buffer[i] >> 5)&31)<<3
          && itr(2) == vxl_byte(buffer[i]&31)<<3;
    }
    TEST("vidl2_pixel_iterator (RGB 555)",success,true);
  }

  {
    // The test buffer below contains 2 pixels encoded in RGB 565
    //                      |RGB_565|RGB_565|
    vxl_uint_16 buffer[] = { 0x7FAF, 0x0103 };
    vidl2_frame_sptr frame = new vidl2_shared_frame(buffer,2,1,VIDL2_PIXEL_FORMAT_RGB_565);
    vidl2_pixel_iterator<VIDL2_PIXEL_FORMAT_RGB_565> itr(frame);

    bool success = true;
    for(unsigned int i=0; i<2; ++i, ++itr){
      vcl_cout << "RGB = ("<< (int)itr(0) << "," << (int)itr(1) << "," << (int)itr(2) << ")\n";
      success = success && itr(0) == vxl_byte((buffer[i] >> 11)&31)<<3
          && itr(1) == vxl_byte((buffer[i] >> 5)&63)<<2
          && itr(2) == vxl_byte(buffer[i]&31)<<3;
    }
    TEST("vidl2_pixel_iterator (RGB 565)",success,true);
  }

  {
    // The test buffer below contains 4 pixels encoded in YUV 4:4:4
    //                  | U  Y  V| U  Y  V| U  Y  V|  U   Y   V|
    vxl_byte buffer[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
    vidl2_frame_sptr frame = new vidl2_shared_frame(buffer,2,2,VIDL2_PIXEL_FORMAT_UYV_444);
    vidl2_pixel_iterator<VIDL2_PIXEL_FORMAT_UYV_444> itr(frame);

    bool success = true;
    for(unsigned int i=0; i<4; ++i, ++itr){
      vcl_cout << "YUV = ("<< (int)itr(0) << "," << (int)itr(1) << "," << (int)itr(2) << ")\n";
      success = success && itr(0) == buffer[i*3+1]
          && itr(1) == buffer[i*3]
          && itr(2) == buffer[i*3+2];
    }
    TEST("vidl2_pixel_iterator (YUV 444)",success,true);
  }

  {
    // The test buffer below contains 4 pixels encoded in YUV 4:2:2
    //                  | U  Y0 V Y1| U  Y0 V  Y1 |
    vxl_byte buffer[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    vidl2_frame_sptr frame = new vidl2_shared_frame(buffer,2,2,VIDL2_PIXEL_FORMAT_UYVY_422);
    vidl2_pixel_iterator<VIDL2_PIXEL_FORMAT_UYVY_422> itr(frame);

    bool success = true;
    for(unsigned int i=0; i<4; ++i, ++itr){
      vcl_cout << "YUV = ("<< (int)itr(0) << "," << (int)itr(1) << "," << (int)itr(2) << ")\n";
      success = success && itr(0) == buffer[2*i+1]
          && itr(1) == buffer[i*2-(i%2)*2]
          && itr(2) == buffer[i*2+2-(i%2)*2];
    }
    TEST("vidl2_pixel_iterator (YUV 422)",success,true);
  }

  {
    // The test buffer below contains 8 pixels encoded in YUV 4:1:1
    //                  | U  Y0 Y1 V  Y2 Y3|U  Y0 Y1  V  Y2  Y3|
    vxl_byte buffer[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
    vidl2_frame_sptr frame = new vidl2_shared_frame(buffer,2,2,VIDL2_PIXEL_FORMAT_UYVY_411);
    vidl2_pixel_iterator<VIDL2_PIXEL_FORMAT_UYVY_411> itr(frame);

    bool success = true;
    for(unsigned int i=0; i<8; ++i, ++itr){
      vcl_cout << "YUV = ("<< (int)itr(0) << "," << (int)itr(1) << "," << (int)itr(2) << ")\n";
      success = success && itr(0) == buffer[6*(i/4)+(i&3)+1+((i>>1)&1)]
          && itr(1) == buffer[6*(i/4)]
          && itr(2) == buffer[6*(i/4)+3];
    }
    TEST("vidl2_pixel_iterator (YUV 411)",success,true);
  }

  {
    // The test buffer below contains 4 pixels encoded in Mono 16
    vxl_uint_16 buffer[] = { 58791, 23010, 5, 100 };
    vidl2_frame_sptr frame = new vidl2_shared_frame(buffer,2,2,VIDL2_PIXEL_FORMAT_MONO_16);
    vidl2_pixel_iterator<VIDL2_PIXEL_FORMAT_MONO_16> itr(frame);
    bool success = true;
    for(unsigned int i=0; i<4; ++i, ++itr){
      vcl_cout << (int)itr(0) << " ";
      success = success && (itr(0) == buffer[i]);
    }
    vcl_cout << '\n';
    TEST("vidl2_pixel_iterator (Mono 16)",success,true);
  }

  {
    // The test buffer below contains 4 pixels encoded in Mono 8
    vxl_byte buffer[] = { 255, 133, 122, 10 };
    vidl2_frame_sptr frame = new vidl2_shared_frame(buffer,2,2,VIDL2_PIXEL_FORMAT_MONO_8);
    vidl2_pixel_iterator<VIDL2_PIXEL_FORMAT_MONO_8> itr(frame);
    bool success = true;
    for(unsigned int i=0; i<4; ++i, ++itr){
      vcl_cout << (int)itr(0) << " ";
      success = success && (itr(0) == buffer[i]);
    }
    vcl_cout << '\n';
    TEST("vidl2_pixel_iterator (Mono 8)",success,true);
  }

  {
    // The test buffer below contains 16 pixels encoded in Mono 1 (boolean image)
    vxl_byte buffer[] = { 0xA3, 0x7B }; // 1010 0011 0111 1011
    vidl2_frame_sptr frame = new vidl2_shared_frame(buffer,4,4,VIDL2_PIXEL_FORMAT_MONO_1);
    vidl2_pixel_iterator<VIDL2_PIXEL_FORMAT_MONO_1> itr(frame);
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
