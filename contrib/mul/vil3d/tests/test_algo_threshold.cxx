// This is mul/vil3d/tests/test_algo_threshold.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte
#include <vil3d/algo/vil3d_threshold.h>
#include <vil3d/vil3d_crop.h>

static void test_threshold_byte(const vil3d_image_view<vxl_byte>& image)
{
  vcl_cout << "*************************\n"
           << " Testing vil3d_threshold\n"
           << "*************************\n";

  vil3d_image_view<bool> bool_im;

  vil3d_threshold_below(image,bool_im, vxl_byte(7));
  TEST("vil3d_threshold_below at (0,0,6)",bool_im(0,0,6),true);
  TEST("vil3d_threshold_below at (0,0,7)",bool_im(0,0,7),true);
  TEST("vil3d_threshold_below at (0,0,8)",bool_im(0,0,8),false);

  vil3d_threshold_above(image,bool_im, vxl_byte(7));
  TEST("vil3d_threshold_above at (0,0,6)",bool_im(0,0,6),false);
  TEST("vil3d_threshold_above at (0,0,7)",bool_im(0,0,7),true);
  TEST("vil3d_threshold_above at (0,0,8)",bool_im(0,0,8),true);

  vil3d_threshold_inside(image,bool_im, vxl_byte(27),vxl_byte(37));
  TEST("vil3d_threshold_inside at (0,2,6)",bool_im(0,2,6),false);
  TEST("vil3d_threshold_inside at (0,2,7)",bool_im(0,2,7),true);
  TEST("vil3d_threshold_inside at (0,3,8)",bool_im(0,3,8),false);

  vil3d_threshold_outside(image,bool_im, vxl_byte(27),vxl_byte(37));
  TEST("vil3d_threshold_outside at (0,2,6)",bool_im(0,2,6),true);
  TEST("vil3d_threshold_outside at (0,2,7)",bool_im(0,2,7),true);
  TEST("vil3d_threshold_outside at (0,2,9)",bool_im(0,2,9),false);
  TEST("vil3d_threshold_outside at (0,3,8)",bool_im(0,3,8),true);
}

static void test_threshold_byte()
{
  vil3d_image_view<vxl_byte> image(15,15,15);
  for (unsigned k=0;k<image.nk();++k)
    for (unsigned j=0;j<image.nj();++j)
      for (unsigned i=0;i<image.ni();++i) image(k,j,i) = i+10*j+k*100;

  test_threshold_byte(image);

  vcl_cout<<"Test non-contiguous image\n";
  vil3d_image_view<vxl_byte> crop_image = vil3d_crop(image,2,10,3,10,4,10);
  for (unsigned k=0;k<crop_image.nk();++k)
    for (unsigned j=0;j<crop_image.nj();++j)
      for (unsigned i=0;i<crop_image.ni();++i) crop_image(k,j,i) = i+10*j+k*100;

  test_threshold_byte(crop_image);
}

static void test_algo_threshold()
{
  test_threshold_byte();
}

TESTMAIN(test_algo_threshold);
