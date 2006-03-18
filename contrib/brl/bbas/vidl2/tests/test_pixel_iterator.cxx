// This is brl/bbas/vidl2/tests/test_pixel_iterator.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vcl_memory.h>
#include <vidl2/vidl2_frame.h>
#include <vidl2/vidl2_pixel_iterator.h>
#include <vidl2/vidl2_pixel_iterator.txx>


static void test_pixel_iterator()
{
  vcl_cout << "******************************\n"
           << " Testing vidl2_pixel_iterator\n"
           << "******************************\n";


  bool all_found = true;
  for (unsigned int i=0; i<VIDL2_PIXEL_FORMAT_ENUM_END; ++i) {
    if (!vidl2_has_pixel_iterator(vidl2_pixel_format(i))) {
      all_found = false;
      vcl_cout << "Warning: no pixel iterator found for format "
               << vidl2_pixel_format(i)<<vcl_endl;
    }
  }
  TEST("All iterators found",all_found,true);

  if (vidl2_has_pixel_iterator(VIDL2_PIXEL_FORMAT_RGB_24))
  {
    // The test buffer below contains 3 pixels encoded in RGB 24
    //                  | R  G  B| R  G  B| R  G  B |
    vxl_byte buffer[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    vidl2_frame_sptr frame = new vidl2_shared_frame(buffer,3,1,VIDL2_PIXEL_FORMAT_RGB_24);
    vidl2_pixel_iterator_of<VIDL2_PIXEL_FORMAT_RGB_24> itr(*frame);
    // polymorphic pixel iterator
    vcl_auto_ptr<vidl2_pixel_iterator> pitr(vidl2_make_pixel_iterator(*frame));

    bool success = true, psuccess = true;
    for (unsigned int i=0; i<3; ++i, ++itr, ++(*pitr)) {
      vxl_byte data[3];
      pitr->get_data(data);
      vcl_cout << "RGB = ("<< (int)itr(0) << ',' << (int)itr(1) << ',' << (int)itr(2) << ")\t "
               << "RGB polymorphic = ("<< (int)data[0] << ',' << (int)data[1] << ',' << (int)data[2] << ")\n";
      success = success && itr(0) == buffer[3*i]
                        && itr(1) == buffer[3*i+1]
                        && itr(2) == buffer[3*i+2];
      psuccess = psuccess && data[0] == buffer[3*i]
                          && data[1] == buffer[3*i+1]
                          && data[2] == buffer[3*i+2];
    }
    TEST("vidl2_pixel_iterator (RGB 24)",success,true);
    TEST("vidl2_pixel_iterator (RGB 24) polymorphic",psuccess,true);
  }

  if (vidl2_has_pixel_iterator(VIDL2_PIXEL_FORMAT_RGB_24P))
  {
    // The test buffer below contains 3 pixels encoded in RGB 24P
    //                  | R  R  R| G  G  G| B  B  B |
    vxl_byte buffer[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    vidl2_frame_sptr frame = new vidl2_shared_frame(buffer,3,1,VIDL2_PIXEL_FORMAT_RGB_24P);
    vidl2_pixel_iterator_of<VIDL2_PIXEL_FORMAT_RGB_24P> itr(*frame);
    // polymorphic pixel iterator
    vcl_auto_ptr<vidl2_pixel_iterator> pitr(vidl2_make_pixel_iterator(*frame));

    bool success = true, psuccess = true;
    for (unsigned int i=0; i<3; ++i, ++itr, ++(*pitr)) {
      vxl_byte data[3];
      pitr->get_data(data);
      vcl_cout << "RGB = ("<< (int)itr(0) << ',' << (int)itr(1) << ',' << (int)itr(2) << ")\t "
               << "RGB polymorphic = ("<< (int)data[0] << ',' << (int)data[1] << ',' << (int)data[2] << ")\n";
      success = success && itr(0) == buffer[i]
                        && itr(1) == buffer[i+3]
                        && itr(2) == buffer[i+6];
      psuccess = psuccess && data[0] == buffer[i]
                          && data[1] == buffer[i+3]
                          && data[2] == buffer[i+6];
    }
    TEST("vidl2_pixel_iterator (RGB 24P)",success,true);
    TEST("vidl2_pixel_iterator (RGB 24P) polymorphic",psuccess,true);
  }

  if (vidl2_has_pixel_iterator(VIDL2_PIXEL_FORMAT_BGR_24))
  {
    // The test buffer below contains 3 pixels encoded in BGR 24
    //                  | B  G  R| B  G  R| B  G  R |
    vxl_byte buffer[] = { 3, 2, 1, 6, 5, 4, 9, 8, 7 };
    vidl2_frame_sptr frame = new vidl2_shared_frame(buffer,3,1,VIDL2_PIXEL_FORMAT_BGR_24);
    vidl2_pixel_iterator_of<VIDL2_PIXEL_FORMAT_BGR_24> itr(*frame);
    // polymorphic pixel iterator
    vcl_auto_ptr<vidl2_pixel_iterator> pitr(vidl2_make_pixel_iterator(*frame));

    bool success = true, psuccess = true;
    for (unsigned int i=0; i<3; ++i, ++itr, ++(*pitr)) {
      vxl_byte data[3];
      pitr->get_data(data);
      vcl_cout << "RGB = ("<< (int)itr(0) << ',' << (int)itr(1) << ',' << (int)itr(2) << ")\t "
               << "RGB polymorphic = ("<< (int)data[0] << ',' << (int)data[1] << ',' << (int)data[2] << ")\n";
      success = success && itr(0) == buffer[3*i+2]
                        && itr(1) == buffer[3*i+1]
                        && itr(2) == buffer[3*i];
      psuccess = psuccess && data[0] == buffer[3*i+2]
                          && data[1] == buffer[3*i+1]
                          && data[2] == buffer[3*i];
    }
    TEST("vidl2_pixel_iterator (BGR 24)",success,true);
    TEST("vidl2_pixel_iterator (BGR 24) polymorphic",psuccess,true);
  }

  if (vidl2_has_pixel_iterator(VIDL2_PIXEL_FORMAT_RGBA_32))
  {
    // The test buffer below contains 3 pixels encoded in RGBA 32
    //                  | R  G  B  A| R  G  B  A| R  G  B  A|
    vxl_byte buffer[] = { 1, 2, 3, 0, 4, 5, 6, 0, 7, 8, 9, 0 };
    vidl2_frame_sptr frame = new vidl2_shared_frame(buffer,3,1,VIDL2_PIXEL_FORMAT_RGBA_32);
    vidl2_pixel_iterator_of<VIDL2_PIXEL_FORMAT_RGBA_32> itr(*frame);
    // polymorphic pixel iterator
    vcl_auto_ptr<vidl2_pixel_iterator> pitr(vidl2_make_pixel_iterator(*frame));

    bool success = true, psuccess = true;
    for (unsigned int i=0; i<3; ++i, ++itr, ++(*pitr)) {
      vxl_byte data[4];
      pitr->get_data(data);
      vcl_cout << "RGBA = ("<< (int)itr(0) << ',' << (int)itr(1)
               << ',' << (int)itr(2) << ','<< (int)itr(3) << ")\t "
               << "RGBA polymorphic = ("<< (int)data[0] << ',' << (int)data[1]
               << ',' << (int)data[2] << ',' << (int)data[3] << ")\n";
      success = success && itr(0) == buffer[4*i]
                        && itr(1) == buffer[4*i+1]
                        && itr(2) == buffer[4*i+2]
                        && itr(3) == buffer[4*i+3];
      psuccess = psuccess && data[0] == buffer[4*i]
                          && data[1] == buffer[4*i+1]
                          && data[2] == buffer[4*i+2]
                          && data[3] == buffer[4*i+3];
    }
    TEST("vidl2_pixel_iterator (RGBA 32)",success,true);
    TEST("vidl2_pixel_iterator (RGBA 32) polymorphic",psuccess,true);
  }

  if (vidl2_has_pixel_iterator(VIDL2_PIXEL_FORMAT_RGBA_32P))
  {
    // The test buffer below contains 3 pixels encoded in RGBA 32P
    //                  | R  R  R| G  G  G| B  B  B| A  A  A|
    vxl_byte buffer[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0 };
    vidl2_frame_sptr frame = new vidl2_shared_frame(buffer,3,1,VIDL2_PIXEL_FORMAT_RGBA_32P);
    vidl2_pixel_iterator_of<VIDL2_PIXEL_FORMAT_RGBA_32P> itr(*frame);
    // polymorphic pixel iterator
    vcl_auto_ptr<vidl2_pixel_iterator> pitr(vidl2_make_pixel_iterator(*frame));

    bool success = true, psuccess = true;
    for (unsigned int i=0; i<3; ++i, ++itr, ++(*pitr)) {
      vxl_byte data[4];
      pitr->get_data(data);
      vcl_cout << "RGBA = ("<< (int)itr(0) << ',' << (int)itr(1)
               << ',' << (int)itr(2) << ','<< (int)itr(3) << ")\t "
               << "RGBA polymorphic = ("<< (int)data[0] << ',' << (int)data[1]
               << ',' << (int)data[2] << ',' << (int)data[3] << ")\n";
      success = success && itr(0) == buffer[i]
                        && itr(1) == buffer[i+3]
                        && itr(2) == buffer[i+6]
                        && itr(3) == buffer[i+9];
      psuccess = psuccess && data[0] == buffer[i]
                          && data[1] == buffer[i+3]
                          && data[2] == buffer[i+6]
                          && data[3] == buffer[i+9];
    }
    TEST("vidl2_pixel_iterator (RGBA 32P)",success,true);
    TEST("vidl2_pixel_iterator (RGBA 32P) polymorphic",psuccess,true);
  }

  if (vidl2_has_pixel_iterator(VIDL2_PIXEL_FORMAT_RGB_555))
  {
    // The test buffer below contains 2 pixels encoded in RGB 555
    //                      |RGB_555|RGB_555|
    vxl_uint_16 buffer[] = { 0x7FAF, 0x0103 };
    vidl2_frame_sptr frame = new vidl2_shared_frame(buffer,2,1,VIDL2_PIXEL_FORMAT_RGB_555);
    vidl2_pixel_iterator_of<VIDL2_PIXEL_FORMAT_RGB_555> itr(*frame);
    // polymorphic pixel iterator
    vcl_auto_ptr<vidl2_pixel_iterator> pitr(vidl2_make_pixel_iterator(*frame));

    bool success = true, psuccess = true;
    for (unsigned int i=0; i<2; ++i, ++itr, ++(*pitr)) {
      vxl_byte data[3];
      pitr->get_data(data);
      vcl_cout << "RGB = ("<< (int)itr(0) << ',' << (int)itr(1) << ',' << (int)itr(2) << ")\t "
               << "RGB polymorphic = ("<< (int)data[0] << ',' << (int)data[1] << ',' << (int)data[2] << ")\n";
      success = success && itr(0) == vxl_byte((buffer[i] >> 10)&31)<<3
                        && itr(1) == vxl_byte((buffer[i] >> 5)&31)<<3
                        && itr(2) == vxl_byte(buffer[i]&31)<<3;
      psuccess = psuccess && data[0] == vxl_byte((buffer[i] >> 10)&31)<<3
                          && data[1] == vxl_byte((buffer[i] >> 5)&31)<<3
                          && data[2] == vxl_byte(buffer[i]&31)<<3;
    }
    TEST("vidl2_pixel_iterator (RGB 555)",success,true);
    TEST("vidl2_pixel_iterator (RGB 555) polymorphic",psuccess,true);
  }

  if (vidl2_has_pixel_iterator(VIDL2_PIXEL_FORMAT_RGB_565))
  {
    // The test buffer below contains 2 pixels encoded in RGB 565
    //                      |RGB_565|RGB_565|
    vxl_uint_16 buffer[] = { 0x7FAF, 0x0103 };
    vidl2_frame_sptr frame = new vidl2_shared_frame(buffer,2,1,VIDL2_PIXEL_FORMAT_RGB_565);
    vidl2_pixel_iterator_of<VIDL2_PIXEL_FORMAT_RGB_565> itr(*frame);
    // polymorphic pixel iterator
    vcl_auto_ptr<vidl2_pixel_iterator> pitr(vidl2_make_pixel_iterator(*frame));

    bool success = true, psuccess = true;
    for (unsigned int i=0; i<2; ++i, ++itr, ++(*pitr)) {
      vxl_byte data[3];
      pitr->get_data(data);
      vcl_cout << "RGB = ("<< (int)itr(0) << ',' << (int)itr(1) << ',' << (int)itr(2) << ")\t "
               << "RGB polymorphic = ("<< (int)data[0] << ',' << (int)data[1] << ',' << (int)data[2] << ")\n";
      success = success && itr(0) == vxl_byte((buffer[i] >> 11)&31)<<3
                        && itr(1) == vxl_byte((buffer[i] >> 5)&63)<<2
                        && itr(2) == vxl_byte(buffer[i]&31)<<3;
      psuccess = psuccess && data[0] == vxl_byte((buffer[i] >> 11)&31)<<3
                          && data[1] == vxl_byte((buffer[i] >> 5)&63)<<2
                          && data[2] == vxl_byte(buffer[i]&31)<<3;
    }
    TEST("vidl2_pixel_iterator (RGB 565)",success,true);
    TEST("vidl2_pixel_iterator (RGB 565) polymorphic",psuccess,true);
  }

  if (vidl2_has_pixel_iterator(VIDL2_PIXEL_FORMAT_YUV_444P))
  {
    // The test buffer below contains 4 pixels encoded in YUV 4:4:4 P
    //                  | Y  Y  Y  Y| U  U  U  U| V  V   V   V|
    vxl_byte buffer[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
    vidl2_frame_sptr frame = new vidl2_shared_frame(buffer,2,2,VIDL2_PIXEL_FORMAT_YUV_444P);
    vidl2_pixel_iterator_of<VIDL2_PIXEL_FORMAT_YUV_444P> itr(*frame);
    // polymorphic pixel iterator
    vcl_auto_ptr<vidl2_pixel_iterator> pitr(vidl2_make_pixel_iterator(*frame));

    bool success = true, psuccess = true;
    for (unsigned int i=0; i<4; ++i, ++itr, ++(*pitr)) {
      vxl_byte data[3];
      pitr->get_data(data);
      vcl_cout << "YUV = ("<< (int)itr(0) << ',' << (int)itr(1) << ',' << (int)itr(2) << ")\t "
               << "YUV polymorphic = ("<< (int)data[0] << ',' << (int)data[1] << ',' << (int)data[2] << ")\n";
      success = success && itr(0) == buffer[i]
                        && itr(1) == buffer[i+4]
                        && itr(2) == buffer[i+8];
      psuccess = psuccess && data[0] == buffer[i]
                          && data[1] == buffer[i+4]
                          && data[2] == buffer[i+8];
    }
    TEST("vidl2_pixel_iterator (YUV 444P)",success,true);
    TEST("vidl2_pixel_iterator (YUV 444P) polymorphic",psuccess,true);
  }

  if (vidl2_has_pixel_iterator(VIDL2_PIXEL_FORMAT_UYV_444))
  {
    // The test buffer below contains 4 pixels encoded in UYV 4:4:4
    //                  | U  Y  V| U  Y  V| U  Y  V|  U   Y   V|
    vxl_byte buffer[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
    vidl2_frame_sptr frame = new vidl2_shared_frame(buffer,2,2,VIDL2_PIXEL_FORMAT_UYV_444);
    vidl2_pixel_iterator_of<VIDL2_PIXEL_FORMAT_UYV_444> itr(*frame);
    // polymorphic pixel iterator
    vcl_auto_ptr<vidl2_pixel_iterator> pitr(vidl2_make_pixel_iterator(*frame));

    bool success = true, psuccess = true;
    for (unsigned int i=0; i<4; ++i, ++itr, ++(*pitr)) {
      vxl_byte data[3];
      pitr->get_data(data);
      vcl_cout << "YUV = ("<< (int)itr(0) << ',' << (int)itr(1) << ',' << (int)itr(2) << ")\t "
               << "YUV polymorphic = ("<< (int)data[0] << ',' << (int)data[1] << ',' << (int)data[2] << ")\n";
      success = success && itr(0) == buffer[i*3+1]
                        && itr(1) == buffer[i*3]
                        && itr(2) == buffer[i*3+2];
      psuccess = psuccess && data[0] == buffer[i*3+1]
                          && data[1] == buffer[i*3]
                          && data[2] == buffer[i*3+2];
    }
    TEST("vidl2_pixel_iterator (UYV 444)",success,true);
    TEST("vidl2_pixel_iterator (UYV 444) polymorphic",psuccess,true);
  }


  if (vidl2_has_pixel_iterator(VIDL2_PIXEL_FORMAT_YUV_422P))
  {
    // The test buffer below contains 16 pixels encoded in YUV 422P
    vxl_byte buffer[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, // Y
                          17, 18, 19, 20, 21, 22, 23, 24,  // U
                          25, 26, 27, 28, 29, 30, 31, 32}; // V
      vidl2_frame_sptr frame = new vidl2_shared_frame(buffer,4,4,VIDL2_PIXEL_FORMAT_YUV_422P);
      vidl2_pixel_iterator_of<VIDL2_PIXEL_FORMAT_YUV_422P> itr(*frame);
    // polymorphic pixel iterator
      vcl_auto_ptr<vidl2_pixel_iterator> pitr(vidl2_make_pixel_iterator(*frame));

      bool success = true, psuccess = true;
      for (unsigned int i=0; i<16; ++i, ++itr, ++(*pitr)) {
        vxl_byte data[3];
        pitr->get_data(data);
        vcl_cout << "YUV = ("<< (int)itr(0) << ',' << (int)itr(1) << ',' << (int)itr(2) << ")\t "
                 << "YUV polymorphic = ("<< (int)data[0] << ',' << (int)data[1] << ',' << (int)data[2] << ")\n";
        success = success && itr(0) == buffer[i]
                          && itr(1) == buffer[16+i/2]
                          && itr(2) == buffer[24+i/2];
        psuccess = psuccess && data[0] == buffer[i]
                            && data[1] == buffer[16+i/2]
                            && data[2] == buffer[24+i/2];
      }
      TEST("vidl2_pixel_iterator (YUV 422P)",success,true);
      TEST("vidl2_pixel_iterator (YUV 422P) polymorphic",psuccess,true);
  }

  if (vidl2_has_pixel_iterator(VIDL2_PIXEL_FORMAT_UYVY_422))
  {
    // The test buffer below contains 4 pixels encoded in UYVU 4:2:2
    //                  | U  Y0 V Y1| U  Y0 V  Y1 |
    vxl_byte buffer[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    vidl2_frame_sptr frame = new vidl2_shared_frame(buffer,2,2,VIDL2_PIXEL_FORMAT_UYVY_422);
    vidl2_pixel_iterator_of<VIDL2_PIXEL_FORMAT_UYVY_422> itr(*frame);
    // polymorphic pixel iterator
    vcl_auto_ptr<vidl2_pixel_iterator> pitr(vidl2_make_pixel_iterator(*frame));

    bool success = true, psuccess = true;
    for (unsigned int i=0; i<4; ++i, ++itr, ++(*pitr)) {
      vxl_byte data[3];
      pitr->get_data(data);
      vcl_cout << "YUV = ("<< (int)itr(0) << ',' << (int)itr(1) << ',' << (int)itr(2) << ")\t "
               << "YUV polymorphic = ("<< (int)data[0] << ',' << (int)data[1] << ',' << (int)data[2] << ")\n";
      success = success && itr(0) == buffer[2*i+1]
                        && itr(1) == buffer[i*2-(i%2)*2]
                        && itr(2) == buffer[i*2+2-(i%2)*2];
      psuccess = psuccess && data[0] == buffer[2*i+1]
                          && data[1] == buffer[i*2-(i%2)*2]
                          && data[2] == buffer[i*2+2-(i%2)*2];
    }
    TEST("vidl2_pixel_iterator (UYVY 422)",success,true);
    TEST("vidl2_pixel_iterator (UYVY 422) polymorphic",psuccess,true);
  }

  if (vidl2_has_pixel_iterator(VIDL2_PIXEL_FORMAT_YUYV_422))
  {
    // The test buffer below contains 4 pixels encoded in YUYV 4:2:2
    //                  | Y0 U  Y1 V| Y0 U  Y1 V |
    vxl_byte buffer[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    vidl2_frame_sptr frame = new vidl2_shared_frame(buffer,2,2,VIDL2_PIXEL_FORMAT_YUYV_422);
    vidl2_pixel_iterator_of<VIDL2_PIXEL_FORMAT_YUYV_422> itr(*frame);
    // polymorphic pixel iterator
    vcl_auto_ptr<vidl2_pixel_iterator> pitr(vidl2_make_pixel_iterator(*frame));

    bool success = true, psuccess = true;
    for (unsigned int i=0; i<4; ++i, ++itr, ++(*pitr)) {
      vxl_byte data[3];
      pitr->get_data(data);
      vcl_cout << "YUV = ("<< (int)itr(0) << ',' << (int)itr(1) << ',' << (int)itr(2) << ")\t "
               << "YUV polymorphic = ("<< (int)data[0] << ',' << (int)data[1] << ',' << (int)data[2] << ")\n";
      success = success && itr(0) == buffer[2*i]
                        && itr(1) == buffer[i*2+1-(i%2)*2]
                        && itr(2) == buffer[i*2+3-(i%2)*2];
      psuccess = psuccess && data[0] == buffer[2*i]
                          && data[1] == buffer[i*2+1-(i%2)*2]
                          && data[2] == buffer[i*2+3-(i%2)*2];
    }
    TEST("vidl2_pixel_iterator (YUYV 422)",success,true);
    TEST("vidl2_pixel_iterator (YUYV 422) polymorphic",psuccess,true);
  }

  if (vidl2_has_pixel_iterator(VIDL2_PIXEL_FORMAT_YUV_411P))
  {
    // The test buffer below contains 8 pixels encoded in YUV 4:1:1 P
    //                  | Y  Y  Y  Y  Y  Y  Y  Y| U  U | V   V |
    vxl_byte buffer[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
    vidl2_frame_sptr frame = new vidl2_shared_frame(buffer,4,2,VIDL2_PIXEL_FORMAT_YUV_411P);
    vidl2_pixel_iterator_of<VIDL2_PIXEL_FORMAT_YUV_411P> itr(*frame);
    // polymorphic pixel iterator
    vcl_auto_ptr<vidl2_pixel_iterator> pitr(vidl2_make_pixel_iterator(*frame));

    bool success = true, psuccess = true;
    for (unsigned int i=0; i<8; ++i, ++itr, ++(*pitr)) {
      vxl_byte data[3];
      pitr->get_data(data);
      vcl_cout << "YUV = ("<< (int)itr(0) << ',' << (int)itr(1) << ',' << (int)itr(2) << ")\t "
               << "YUV polymorphic = ("<< (int)data[0] << ',' << (int)data[1] << ',' << (int)data[2] << ")\n";
      success = success && itr(0) == buffer[i]
                        && itr(1) == buffer[(i/4)+8]
                        && itr(2) == buffer[(i/4)+10];
      psuccess = psuccess && data[0] == buffer[i]
                          && data[1] == buffer[(i/4)+8]
                          && data[2] == buffer[(i/4)+10];
    }
    TEST("vidl2_pixel_iterator (YUV 411P)",success,true);
    TEST("vidl2_pixel_iterator (YUV 411P) polymorphic",psuccess,true);
  }

  if (vidl2_has_pixel_iterator(VIDL2_PIXEL_FORMAT_UYVY_411))
  {
    // The test buffer below contains 8 pixels encoded in UYVY 4:1:1
    //                  | U  Y0 Y1 V  Y2 Y3|U  Y0 Y1  V  Y2  Y3|
    vxl_byte buffer[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
    vidl2_frame_sptr frame = new vidl2_shared_frame(buffer,4,2,VIDL2_PIXEL_FORMAT_UYVY_411);
    vidl2_pixel_iterator_of<VIDL2_PIXEL_FORMAT_UYVY_411> itr(*frame);
    // polymorphic pixel iterator
    vcl_auto_ptr<vidl2_pixel_iterator> pitr(vidl2_make_pixel_iterator(*frame));

    bool success = true, psuccess = true;
    for (unsigned int i=0; i<8; ++i, ++itr, ++(*pitr)) {
      vxl_byte data[3];
      pitr->get_data(data);
      vcl_cout << "YUV = ("<< (int)itr(0) << ',' << (int)itr(1) << ',' << (int)itr(2) << ")\t "
               << "YUV polymorphic = ("<< (int)data[0] << ',' << (int)data[1] << ',' << (int)data[2] << ")\n";
      success = success && itr(0) == buffer[6*(i/4)+(i&3)+1+((i>>1)&1)]
                        && itr(1) == buffer[6*(i/4)]
                        && itr(2) == buffer[6*(i/4)+3];
      psuccess = psuccess && data[0] == buffer[6*(i/4)+(i&3)+1+((i>>1)&1)]
                          && data[1] == buffer[6*(i/4)]
                          && data[2] == buffer[6*(i/4)+3];
    }
    TEST("vidl2_pixel_iterator (UYVY 411)",success,true);
    TEST("vidl2_pixel_iterator (UYVY 411) polymorphic",psuccess,true);
  }

  if (vidl2_has_pixel_iterator(VIDL2_PIXEL_FORMAT_YUV_420P))
  {
    // The test buffer below contains 16 pixels encoded in YUV 420P
    vxl_byte buffer[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, // Y
                          17, 18, 19, 20,   // U
                          21, 22, 23, 24 }; // V
    vidl2_frame_sptr frame = new vidl2_shared_frame(buffer,4,4,VIDL2_PIXEL_FORMAT_YUV_420P);
    vidl2_pixel_iterator_of<VIDL2_PIXEL_FORMAT_YUV_420P> itr(*frame);
    // polymorphic pixel iterator
    vcl_auto_ptr<vidl2_pixel_iterator> pitr(vidl2_make_pixel_iterator(*frame));

    bool success = true, psuccess = true;
    for (unsigned int i=0; i<16; ++i, ++itr, ++(*pitr)) {
      vxl_byte data[3];
      pitr->get_data(data);
      vcl_cout << "YUV = ("<< (int)itr(0) << ',' << (int)itr(1) << ',' << (int)itr(2) << ") ";
      if (i%4 == 3) vcl_cout << '\n';
      success = success && itr(0) == buffer[i]
                        && itr(1) == buffer[16+(i/2)%2 + ((i/8)<<1)]
                        && itr(2) == buffer[20+(i/2)%2 + ((i/8)<<1)];
      psuccess = psuccess && data[0] == buffer[i]
                          && data[1] == buffer[16+(i/2)%2 + ((i/8)<<1)]
                          && data[2] == buffer[20+(i/2)%2 + ((i/8)<<1)];
    }
    TEST("vidl2_pixel_iterator (YUV 420P)",success,true);
    TEST("vidl2_pixel_iterator (YUV 420P) polymorphic",psuccess,true);
  }

  if (vidl2_has_pixel_iterator(VIDL2_PIXEL_FORMAT_YUV_410P))
  {
    // The test buffer below contains 16 pixels encoded in YUV 410P
    vxl_byte buffer[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, // Y
                          17,   // U
                          18 }; // V
      vidl2_frame_sptr frame = new vidl2_shared_frame(buffer,4,4,VIDL2_PIXEL_FORMAT_YUV_410P);
      vidl2_pixel_iterator_of<VIDL2_PIXEL_FORMAT_YUV_410P> itr(*frame);
      // polymorphic pixel iterator
      vcl_auto_ptr<vidl2_pixel_iterator> pitr(vidl2_make_pixel_iterator(*frame));

      bool success = true, psuccess = true;
      for (unsigned int i=0; i<16; ++i, ++itr, ++(*pitr)) {
        vxl_byte data[3];
        pitr->get_data(data);
        vcl_cout << "YUV = ("<< (int)itr(0) << ',' << (int)itr(1) << ',' << (int)itr(2) << ") ";
        if (i%4 == 3) vcl_cout << '\n';
        success = success && itr(0) == buffer[i]
                          && itr(1) == buffer[16]
                          && itr(2) == buffer[17];
        psuccess = psuccess && data[0] == buffer[i]
                            && data[1] == buffer[16]
                            && data[2] == buffer[17];
      }
      TEST("vidl2_pixel_iterator (YUV 410P)",success,true);
      TEST("vidl2_pixel_iterator (YUV 410P) polymorphic",psuccess,true);
  }

  if (vidl2_has_pixel_iterator(VIDL2_PIXEL_FORMAT_MONO_16))
  {
    // The test buffer below contains 4 pixels encoded in Mono 16
    vxl_uint_16 buffer[] = { 58791, 23010, 5, 100 };
    vidl2_frame_sptr frame = new vidl2_shared_frame(buffer,2,2,VIDL2_PIXEL_FORMAT_MONO_16);
    vidl2_pixel_iterator_of<VIDL2_PIXEL_FORMAT_MONO_16> itr(*frame);
    // polymorphic pixel iterator
    vcl_auto_ptr<vidl2_pixel_iterator> pitr(vidl2_make_pixel_iterator(*frame));

    bool success = true, psuccess = true;
    for (unsigned int i=0; i<4; ++i, ++itr, ++(*pitr)) {
      vxl_uint_16 data[1];
      pitr->get_data(reinterpret_cast<vxl_byte*>(data));
      vcl_cout << (int)itr(0) << ' ';
      success = success && (itr(0) == buffer[i]);
      psuccess = psuccess && (data[0] == buffer[i]);
    }
    vcl_cout << '\n';
    TEST("vidl2_pixel_iterator (Mono 16)",success,true);
    TEST("vidl2_pixel_iterator (Mono 16) polymorphic",psuccess,true);
  }

  if (vidl2_has_pixel_iterator(VIDL2_PIXEL_FORMAT_MONO_8))
  {
    // The test buffer below contains 4 pixels encoded in Mono 8
    vxl_byte buffer[] = { 255, 133, 122, 10 };
    vidl2_frame_sptr frame = new vidl2_shared_frame(buffer,2,2,VIDL2_PIXEL_FORMAT_MONO_8);
    vidl2_pixel_iterator_of<VIDL2_PIXEL_FORMAT_MONO_8> itr(*frame);
    // polymorphic pixel iterator
    vcl_auto_ptr<vidl2_pixel_iterator> pitr(vidl2_make_pixel_iterator(*frame));

    bool success = true, psuccess = true;
    for (unsigned int i=0; i<4; ++i, ++itr, ++(*pitr)) {
      vxl_byte data[3];
      pitr->get_data(data);
      vcl_cout << (int)itr(0) << ' ';
      success = success && (itr(0) == buffer[i]);
      psuccess = psuccess && (data[0] == buffer[i]);
    }
    vcl_cout << '\n';
    TEST("vidl2_pixel_iterator (Mono 8)",success,true);
    TEST("vidl2_pixel_iterator (Mono 8) polymorphic",psuccess,true);
  }

  if (vidl2_has_pixel_iterator(VIDL2_PIXEL_FORMAT_MONO_1))
  {
    // The test buffer below contains 16 pixels encoded in Mono 1 (boolean image)
    vxl_byte buffer[] = { 0xA3, 0x7B }; // 1010 0011 0111 1011
    vidl2_frame_sptr frame = new vidl2_shared_frame(buffer,4,4,VIDL2_PIXEL_FORMAT_MONO_1);
    vidl2_pixel_iterator_of<VIDL2_PIXEL_FORMAT_MONO_1> itr(*frame);
    // polymorphic pixel iterator
    vcl_auto_ptr<vidl2_pixel_iterator> pitr(vidl2_make_pixel_iterator(*frame));

    bool success = true, psuccess = true;
    for (unsigned int i=0; i<16; ++i, ++itr, ++(*pitr)) {
      bool data[1];
      pitr->get_data(reinterpret_cast<vxl_byte*>(data));
      vcl_cout << itr(0) << ' ';
      success = success && (itr(0) == bool(buffer[i/8] & 128>>i%8));
      psuccess = psuccess && (data[0] == bool(buffer[i/8] & 128>>i%8));
    }
    vcl_cout << '\n';
    TEST("vidl2_pixel_iterator (Mono 1)",success,true);
    TEST("vidl2_pixel_iterator (Mono 1) polymorphic",psuccess,true);
  }
}

TESTMAIN(test_pixel_iterator);
