// This is core/vil/algo/tests/test_algo_threshold.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte
#include <vil/algo/vil_threshold.h>
#include <vil/vil_crop.h>

static void test_threshold_byte(const vil_image_view<vxl_byte>& image)
{
  vcl_cout << "***********************\n"
           << " Testing vil_threshold\n"
           << "***********************\n";

  vil_image_view<bool> bool_im;

  vil_threshold_below(image,bool_im, vxl_byte(7));
  TEST("vil_threshold_below at (0,6)", bool_im(0,6), true);
  TEST("vil_threshold_below at (0,7)", bool_im(0,7), true);
  TEST("vil_threshold_below at (0,8)", bool_im(0,8), false);

  vil_threshold_above(image,bool_im, vxl_byte(7));
  TEST("vil_threshold_above at (0,6)", bool_im(0,6), false);
  TEST("vil_threshold_above at (0,7)", bool_im(0,7), true);
  TEST("vil_threshold_above at (0,8)", bool_im(0,8), true);

  vil_threshold_inside(image,bool_im, vxl_byte(27),vxl_byte(37));
  TEST("vil_threshold_inside at (2,6)", bool_im(2,6), false);
  TEST("vil_threshold_inside at (2,7)", bool_im(2,7), true);
  TEST("vil_threshold_inside at (3,8)", bool_im(3,8), false);

  vil_threshold_outside(image,bool_im, vxl_byte(27),vxl_byte(37));
  TEST("vil_threshold_outside at (2,6)", bool_im(2,6), true);
  TEST("vil_threshold_outside at (2,7)", bool_im(2,7), true);
  TEST("vil_threshold_outside at (2,9)", bool_im(2,9), false);
  TEST("vil_threshold_outside at (3,8)", bool_im(3,8), true);
}

static void test_threshold_byte()
{
  vil_image_view<vxl_byte> image(15,15);
  for (unsigned j=0;j<image.nj();++j)
    for (unsigned i=0;i<image.ni();++i)
      image(j,i) = i+10*j;

  test_threshold_byte(image);

  vcl_cout<<"Test non-contiguous image\n";
  vil_image_view<vxl_byte> crop_image = vil_crop(image,2,10,3,10);
  for (unsigned j=0;j<crop_image.nj();++j)
    for (unsigned i=0;i<crop_image.ni();++i)
      crop_image(j,i) = i+10*j;

  test_threshold_byte(crop_image);
}

static void test_algo_threshold()
{
  test_threshold_byte();
}

TESTMAIN(test_algo_threshold);
