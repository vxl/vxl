// This is mul/vil2/tests/test_threshold.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte
#include <vil2/algo/vil2_threshold.h>
#include <vil2/vil2_crop.h>

void test_threshold_byte(const vil2_image_view<vxl_byte>& image)
{
  vcl_cout << "**********************\n"
           << " Testing vil2_threshold\n"
           << "**********************\n";

  vil2_image_view<bool> bool_im;

  vil2_threshold_below(image,bool_im, vxl_byte(7));
  TEST("vil2_threshold_below at (0,6)",bool_im(0,6),true);
  TEST("vil2_threshold_below at (0,7)",bool_im(0,7),true);
  TEST("vil2_threshold_below at (0,8)",bool_im(0,8),false);

  vil2_threshold_above(image,bool_im, vxl_byte(7));
  TEST("vil2_threshold_above at (0,6)",bool_im(0,6),false);
  TEST("vil2_threshold_above at (0,7)",bool_im(0,7),true);
  TEST("vil2_threshold_above at (0,8)",bool_im(0,8),true);

  vil2_threshold_inside(image,bool_im, vxl_byte(27),vxl_byte(37));
  TEST("vil2_threshold_inside at (2,6)",bool_im(2,6),false);
  TEST("vil2_threshold_inside at (2,7)",bool_im(2,7),true);
  TEST("vil2_threshold_inside at (3,8)",bool_im(3,8),false);

  vil2_threshold_outside(image,bool_im, vxl_byte(27),vxl_byte(37));
  TEST("vil2_threshold_outside at (2,6)",bool_im(2,6),true);
  TEST("vil2_threshold_outside at (2,7)",bool_im(2,7),true);
  TEST("vil2_threshold_outside at (2,9)",bool_im(2,9),false);
  TEST("vil2_threshold_outside at (3,8)",bool_im(3,8),true);
}

void test_threshold_byte()
{
  vil2_image_view<vxl_byte> image(15,15);
  for (unsigned j=0;j<image.nj();++j)
    for (unsigned i=0;i<image.ni();++i) image(j,i) = i+10*j;

  test_threshold_byte(image);

  vcl_cout<<"Test non-contiguous image"<<vcl_endl;
  vil2_image_view<vxl_byte> crop_image = vil2_crop(image,2,10,3,10);
  for (unsigned j=0;j<crop_image.nj();++j)
    for (unsigned i=0;i<crop_image.ni();++i) crop_image(j,i) = i+10*j;

  test_threshold_byte(crop_image);
}

MAIN( test_threshold )
{
  START( "vil2_threshold_*" );

  test_threshold_byte();

  SUMMARY();
}
