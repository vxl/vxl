// This is core/vidl/tests/test_convert.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vil/vil_image_view.h>
#include <vidl/vidl_config.h>
#include <vidl/vidl_convert.h>
#include <vul/vul_timer.h>
#include <vcl_cstring.h> // for vcl_memcpy

#if VIDL_HAS_FFMPEG
#include <vidl/vidl_ffmpeg_convert.h>
#endif


static void test_convert()
{
  vcl_cout << "***********************\n"
           << " Testing vidl_convert\n"
           << "***********************\n";

  {
    vxl_byte buffer[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    vil_image_view<vxl_byte> image;
    vidl_frame_sptr frame = new vidl_shared_frame(buffer, 4, 2, VIDL_PIXEL_FORMAT_UYVY_422);
    vidl_convert_to_view(*frame, image);

    vcl_cout << image.ni() << ' '<< image.nj() << ' ' << image.nplanes() << vcl_endl;
    for (unsigned j = 0; j < image.nj(); ++j){
      for (unsigned i = 0; i < image.ni(); ++i){
        vcl_cout << '(';
        for (unsigned p = 0; p < image.nplanes(); ++p)
          vcl_cout << (int)image(i,j,p)<< ' ';
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
        if (!success)
          break;
      }
      if (!success)
        break;
    }
    TEST("vidl_convert_to_view (UYVY_422)", success, true);
  }

  {
    vxl_byte buffer1[16], buffer2[16];
    int num_unsupported = 0;
    for (int i=0; i<VIDL_PIXEL_FORMAT_ENUM_END; ++i){
      vidl_shared_frame frame1(buffer1, 2, 2, vidl_pixel_format(i));
      for (int j=0; j<VIDL_PIXEL_FORMAT_ENUM_END; ++j){
        vidl_shared_frame frame2(buffer2, 2, 2, vidl_pixel_format(j));
        if (!vidl_convert_frame(frame1,frame2))
          ++num_unsupported;
      }
    }
    TEST("vidl_convert_frame - support for all formats", num_unsupported, 0);
    if (num_unsupported > 0)
      vcl_cerr << "Warning:  conversion failed for "<< num_unsupported << " out of "
               << VIDL_PIXEL_FORMAT_ENUM_END*VIDL_PIXEL_FORMAT_ENUM_END
               << " format pairs\n";
  }

  // timing tests
  {
    const int ni = 640, nj = 480;
    vil_image_view<vxl_byte> image(ni,nj,1,3);
    vil_image_view<float> imagef(ni,nj,3);
    vidl_frame_sptr frame_image = new vidl_memory_chunk_frame(image);
    vxl_byte buffer[ni*nj*3];
    vidl_frame_sptr frame = new vidl_shared_frame(buffer, ni, nj, VIDL_PIXEL_FORMAT_UYVY_422);

    vul_timer timer;
    for (unsigned int i=0; i<10; ++i)
      vidl_convert_to_view(*frame, image);
    float time = timer.all()/10000.0f;
    vcl_cout << "copy time = " << time << vcl_endl;

    timer.mark();
    for (unsigned int i=0; i<10; ++i)
      vidl_convert_to_view(*frame, image);
    time = timer.all()/10000.0f;
    vcl_cout << "copy convert time = " << time << vcl_endl;

    timer.mark();
    for (unsigned int i=0; i<10; ++i)
      vidl_convert_frame(*frame, *frame_image);
    time = timer.all()/10000.0f;
    vcl_cout << "frame convert time = " << time << vcl_endl;

    timer.mark();
    for (unsigned int i=0; i<10; ++i)
      vidl_convert_to_view(*frame, imagef);
    time = timer.all()/10000.0f;
    vcl_cout << "copy float convert time = " << time << vcl_endl;

    timer.mark();
    for (unsigned int i=0; i<10; ++i)
      vcl_memcpy(image.top_left_ptr(), buffer, ni*nj*3);
    time = timer.all()/10000.0f;
    vcl_cout << "memcpy time = " << time << vcl_endl;

#if VIDL_HAS_FFMPEG
    vidl_frame_sptr frame2 = new vidl_memory_chunk_frame(image);
    if (!vidl_ffmpeg_convert(frame, frame2))
      vcl_cerr << "FFMPEG unable to make conversion\n";
    timer.mark();
    for (unsigned int i=0; i<10; ++i)
      vidl_ffmpeg_convert(frame, frame2);
    time = timer.all()/10000.0f;
    vcl_cout << "ffmpeg time = " << time << vcl_endl;
#endif
  }
}

TESTMAIN(test_convert);
