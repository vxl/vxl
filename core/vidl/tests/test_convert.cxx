// This is core/vidl/tests/test_convert.cxx
#include <iostream>
#include <cstring>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_view.h>
#include <vil/vil_crop.h>
#include <vidl/vidl_config.h>
#include <vidl/vidl_convert.h>
#include <vul/vul_timer.h>

#if VIDL_HAS_FFMPEG
#include <vidl/vidl_ffmpeg_convert.h>
#endif


static void test_convert()
{
  std::cout << "***********************\n"
           << " Testing vidl_convert\n"
           << "***********************\n";

  {
    vxl_byte buffer[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    vil_image_view<vxl_byte> image;
    vidl_frame_sptr frame = new vidl_shared_frame(buffer, 4, 2, VIDL_PIXEL_FORMAT_UYVY_422);
    vidl_convert_to_view(*frame, image);

    std::cout << image.ni() << ' '<< image.nj() << ' ' << image.nplanes() << std::endl;
    for (unsigned j = 0; j < image.nj(); ++j){
      for (unsigned i = 0; i < image.ni(); ++i){
        std::cout << '(';
        for (unsigned p = 0; p < image.nplanes(); ++p)
          std::cout << (int)image(i,j,p)<< ' ';
        std::cout << ") ";
      }
      std::cout << std::endl;
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

  // test conversion from image_views to frames
  {
    vil_image_view<vxl_byte> img8(16,16,3);
    img8.fill(100);
    vidl_frame_sptr frame = vidl_convert_to_frame(img8);
    bool valid = !!frame;
    if(valid)
    {
      valid = valid && frame->ni() == img8.ni() && frame->nj() == img8.nj();
      const auto * data8 = static_cast<const vxl_byte*>(frame->data());
      // test that the image is wrapped into a frame
      valid = valid && data8 == img8.top_left_ptr();
      valid = valid && frame->pixel_format() == VIDL_PIXEL_FORMAT_RGB_24P;
    }
    TEST("vidl_convert_to_frame (vxl_byte)", valid, true);

    vil_image_view<vxl_byte> img8nc = vil_crop(img8,3,2,5,3);
    frame = vidl_convert_to_frame(img8nc);
    valid = !!frame;
    if(valid)
    {
      valid = valid && frame->ni() == img8nc.ni() && frame->nj() == img8nc.nj();
      const auto * data8 = static_cast<const vxl_byte*>(frame->data());
      // test that the image is copied (not wrapped) into a frame
      valid = valid && data8 != img8nc.top_left_ptr();
      valid = valid && data8[0] == img8nc(0,0);
      valid = valid && frame->pixel_format() == VIDL_PIXEL_FORMAT_RGB_24P;
    }
    TEST("vidl_convert_to_frame (vxl_byte) non-contiguous", valid, true);

    vil_image_view<vxl_uint_16> img16(10,20,1);
    img16.fill(150);
    frame = vidl_convert_to_frame(img16);
    valid = !!frame;
    if(valid)
    {
      valid = valid && frame->ni() == img16.ni() && frame->nj() == img16.nj();
      const auto * data16 = static_cast<const vxl_uint_16*>(frame->data());
      // test that the image is wrapped into a frame
      valid = valid && data16 == img16.top_left_ptr();
      valid = valid && frame->pixel_format() == VIDL_PIXEL_FORMAT_MONO_16;
    }
    TEST("vidl_convert_to_frame (vxl_uint16)", valid, true);

    vil_image_view<vxl_uint_16> img16nc = vil_crop(img16,0,5,0,10);
    frame = vidl_convert_to_frame(img16nc);
    valid = !!frame;
    if(valid)
    {
      valid = valid && frame->ni() == img16nc.ni() && frame->nj() == img16nc.nj();
      const auto * data16 = static_cast<const vxl_uint_16*>(frame->data());
      // test that the image is copied (not wrapped) into a frame
      valid = valid && data16 != img16nc.top_left_ptr();
      valid = valid && data16[0] == img16nc(0,0);
      valid = valid && frame->pixel_format() == VIDL_PIXEL_FORMAT_MONO_16;
    }
    TEST("vidl_convert_to_frame (vxl_uint16) non-contiguous", valid, true);

    vil_image_view<vil_rgb<vxl_ieee_32> > imgf(25,25);
    imgf.fill(3.14159f);
    frame = vidl_convert_to_frame(imgf);
    valid = !!frame;
    if(valid)
    {
      valid = valid && frame->ni() == imgf.ni() && frame->nj() == imgf.nj();
      const auto * dataf = static_cast<const vil_rgb<vxl_ieee_32>*>(frame->data());
      // test that the image is wrapped into a frame
      valid = valid && dataf == imgf.top_left_ptr();
      valid = valid && frame->pixel_format() == VIDL_PIXEL_FORMAT_RGB_F32;
    }
    TEST("vidl_convert_to_frame (vil_rgb<float>)", valid, true);

    vil_image_view<vil_rgb<vxl_ieee_32> > imgfnc = vil_crop(imgf,5,5,5,5);
    frame = vidl_convert_to_frame(imgfnc);
    valid = !!frame;
    if(valid)
    {
      valid = valid && frame->ni() == imgfnc.ni() && frame->nj() == imgfnc.nj();
      const auto * dataf = static_cast<const vil_rgb<vxl_ieee_32>*>(frame->data());
      // test that the image is copied (not wrapped) into a frame
      valid = valid && dataf != imgfnc.top_left_ptr();
      valid = valid && dataf[0] == imgfnc(0,0);
      valid = valid && frame->pixel_format() == VIDL_PIXEL_FORMAT_RGB_F32P;
    }
    TEST("vidl_convert_to_frame (vil_rgb<float>) non-contiguous", valid, true);
  }

  // test conversions
  {
    vxl_byte buffer1[64], buffer2[64];
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
      std::cerr << "Warning:  conversion failed for "<< num_unsupported << " out of "
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
    std::cout << "copy time = " << time << std::endl;

    timer.mark();
    for (unsigned int i=0; i<10; ++i)
      vidl_convert_to_view(*frame, image);
    time = timer.all()/10000.0f;
    std::cout << "copy convert time = " << time << std::endl;

    timer.mark();
    for (unsigned int i=0; i<10; ++i)
      vidl_convert_frame(*frame, *frame_image);
    time = timer.all()/10000.0f;
    std::cout << "frame convert time = " << time << std::endl;

    timer.mark();
    for (unsigned int i=0; i<10; ++i)
      vidl_convert_to_view(*frame, imagef);
    time = timer.all()/10000.0f;
    std::cout << "copy float convert time = " << time << std::endl;

    timer.mark();
    for (unsigned int i=0; i<10; ++i)
      std::memcpy(image.top_left_ptr(), buffer, ni*nj*3);
    time = timer.all()/10000.0f;
    std::cout << "memcpy time = " << time << std::endl;

#if VIDL_HAS_FFMPEG
    vidl_frame_sptr frame2 = new vidl_memory_chunk_frame(image);
    if (!vidl_ffmpeg_convert(frame, frame2))
      std::cerr << "FFMPEG unable to make conversion\n";
    timer.mark();
    for (unsigned int i=0; i<10; ++i)
      vidl_ffmpeg_convert(frame, frame2);
    time = timer.all()/10000.0f;
    std::cout << "ffmpeg time = " << time << std::endl;
#endif
  }
}

TESTMAIN(test_convert);
