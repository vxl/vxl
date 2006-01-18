// This is brl/bbas/vidl2/tests/test_convert.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vidl2/vidl2_convert.h>
#include <vul/vul_timer.h>
#include <vcl_cstring.h> // for vcl_memcpy


static void test_convert()
{
  vcl_cout << "***********************\n"
           << " Testing vidl2_convert\n"
           << "***********************\n";

  {
    vxl_byte buffer[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    vil_image_view<vxl_byte> image;
    vidl2_frame_sptr frame = new vidl2_shared_frame(buffer, 4, 2, VIDL2_PIXEL_FORMAT_YUV_422);
    vidl2_convert_to_view(frame, image);

    vcl_cout << image.ni() << " "<< image.nj() << " " << image.nplanes() << vcl_endl;
    for (unsigned j = 0; j < image.nj(); ++j){
      for (unsigned i = 0; i < image.ni(); ++i){
        vcl_cout << "(";
        for (unsigned p = 0; p < image.nplanes(); ++p)
          vcl_cout << (int)image(i,j,p)<< " ";
        vcl_cout << ") ";
      }
      vcl_cout << vcl_endl;
    }

    bool success = true;
    for (unsigned j = 0; j < image.nj(); ++j){
      for (unsigned i = 0; i < image.ni(); i+=2){
        unsigned int offset =2*(image.ni()*j + i);
        success =  image(i,j,0) == buffer[offset+1]
                && image(i+1,j,0) == buffer[offset+3]
                && image(i,j,1) == buffer[offset]
                && image(i+1,j,1) == buffer[offset]
                && image(i,j,2) == buffer[offset+2]
                && image(i+1,j,2) == buffer[offset+2];
        if(!success)
          break;
      }
      if(!success)
        break;
    }
    TEST("vidl2_convert_to_view (YUV_422)", success, true);
  }

  // timing tests
  {
    const int ni = 640, nj = 480;
    vil_image_view<vxl_byte> image(ni,nj,3);
    vil_image_view<float> imagef(ni,nj,3);
    vxl_byte buffer[ni*nj*3];
    vidl2_frame_sptr frame = new vidl2_shared_frame(buffer, ni, nj, VIDL2_PIXEL_FORMAT_YUV_422);

    vul_timer timer;
    for(unsigned int i=0; i<100; ++i)
      vidl2_convert_to_view(frame, image);
    float time = timer.user()/100000.0f;
    vcl_cout << "copy time = " << time << vcl_endl;

    timer.mark();
    for(unsigned int i=0; i<100; ++i)
      vidl2_convert_to_view_rgb(frame, image);
    time = timer.user()/100000.0f;
    vcl_cout << "copy convert time = " << time << vcl_endl;

    timer.mark();
    for(unsigned int i=0; i<100; ++i)
      vidl2_convert_to_view_rgb(frame, imagef);
    time = timer.user()/100000.0f;
    vcl_cout << "copy float convert time = " << time << vcl_endl;

    timer.mark();
    for(unsigned int i=0; i<100; ++i)
      vcl_memcpy(image.top_left_ptr(), buffer, ni*nj*3);
    time = timer.user()/100000.0f;
    vcl_cout << "memcpy time = " << time << vcl_endl;
  }

}

TESTMAIN(test_convert);
