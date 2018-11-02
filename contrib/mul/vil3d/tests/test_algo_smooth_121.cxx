// This is mul/vil3d/tests/test_algo_smooth_121.cxx
#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vxl_config.h> // for vxl_byte
#include <vil3d/algo/vil3d_smooth_121.h>
#include <vil3d/vil3d_crop.h>
#include <vil3d/vil3d_print.h>

static void test_smooth_121_i_byte()
{
  std::cout << "************************\n"
           << " Testing vil3d_smooth_121_i\n"
           << "************************\n";

  vil3d_image_view<vxl_byte> image(10,10,10);
  image.fill(vxl_byte(0));
  image(5,5,5)=vxl_byte(100);

  vil3d_image_view<float> smooth_im;
  vil3d_smooth_121_i(image,smooth_im);

  TEST_NEAR("smooth_im(4,5,5)", smooth_im(4,5,5),  25.0f, 0.0001f);
  TEST_NEAR("smooth_im(5,5,5)", smooth_im(5,5,5),  50.0f, 0.0001f);
  TEST_NEAR("smooth_im(6,5,5)", smooth_im(6,5,5),  25.0f, 0.0001f);
  TEST_NEAR("smooth_im(5,6,5)", smooth_im(5,6,5),   0.0f, 0.0001f);
  TEST_NEAR("smooth_im(5,5,6)", smooth_im(5,5,6),   0.0f, 0.0001f);
}
static void test_smooth_121_j_byte()
{
  std::cout << "************************\n"
           << " Testing vil3d_smooth_121_j\n"
           << "************************\n";

  vil3d_image_view<vxl_byte> image(10,10,10);
  image.fill(vxl_byte(0));
  image(5,5,5)=vxl_byte(100);

  vil3d_image_view<float> smooth_im;
  vil3d_smooth_121_j(image,smooth_im);

  TEST_NEAR("smooth_im(4,5,5)", smooth_im(4,5,5),   0.0f, 0.0001f);
  TEST_NEAR("smooth_im(5,5,5)", smooth_im(5,5,5),  50.0f, 0.0001f);
  TEST_NEAR("smooth_im(6,5,5)", smooth_im(6,5,5),   0.0f, 0.0001f);
  TEST_NEAR("smooth_im(5,4,5)", smooth_im(5,4,5),  25.0f, 0.0001f);
  TEST_NEAR("smooth_im(5,6,5)", smooth_im(5,6,5),  25.0f, 0.0001f);
  TEST_NEAR("smooth_im(5,5,4)", smooth_im(5,5,4),   0.0f, 0.0001f);
  TEST_NEAR("smooth_im(5,5,6)", smooth_im(5,5,6),   0.0f, 0.0001f);
}
static void test_smooth_121_k_byte()
{
  std::cout << "************************\n"
           << " Testing vil3d_smooth_121_j\n"
           << "************************\n";

  vil3d_image_view<vxl_byte> image(10,10,10);
  image.fill(vxl_byte(0));
  image(5,5,5)=vxl_byte(100);

  vil3d_image_view<float> smooth_im;
  vil3d_smooth_121_k(image,smooth_im);

  TEST_NEAR("smooth_im(5,5,5)", smooth_im(5,5,5),  50.0f, 0.0001f);

  TEST_NEAR("smooth_im(4,5,5)", smooth_im(4,5,5),   0.0f, 0.0001f);
  TEST_NEAR("smooth_im(6,5,5)", smooth_im(6,5,5),   0.0f, 0.0001f);
  TEST_NEAR("smooth_im(5,4,5)", smooth_im(5,4,5),   0.0f, 0.0001f);
  TEST_NEAR("smooth_im(5,6,5)", smooth_im(5,6,5),   0.0f, 0.0001f);
  TEST_NEAR("smooth_im(5,5,4)", smooth_im(5,5,4),  25.0f, 0.0001f);
  TEST_NEAR("smooth_im(5,5,6)", smooth_im(5,5,6),  25.0f, 0.0001f);
}

static void test_smooth_121_byte()
{
  std::cout << "************************\n"
           << " Testing vil3d_smooth_121\n"
           << "************************\n";

  vil3d_image_view<vxl_byte> image(10,10,10);
  image.fill(vxl_byte(0));
  image(5,5,5)=vxl_byte(64);

  vil3d_image_view<float> smooth_im;
  vil3d_smooth_121(image,smooth_im);

  TEST_NEAR("smooth_im(5,5,5)", smooth_im(5,5,5),   8.0f, 0.0001f);

  TEST_NEAR("smooth_im(4,5,5)", smooth_im(4,5,5),   4.0f, 0.0001f);
  TEST_NEAR("smooth_im(6,5,5)", smooth_im(6,5,5),   4.0f, 0.0001f);
  TEST_NEAR("smooth_im(5,4,5)", smooth_im(5,4,5),   4.0f, 0.0001f);
  TEST_NEAR("smooth_im(5,6,5)", smooth_im(5,6,5),   4.0f, 0.0001f);
  TEST_NEAR("smooth_im(5,5,4)", smooth_im(5,5,4),   4.0f, 0.0001f);
  TEST_NEAR("smooth_im(5,5,6)", smooth_im(5,5,6),   4.0f, 0.0001f);

  TEST_NEAR("smooth_im(5,6,6)", smooth_im(5,6,6),   2.0f, 0.0001f);
  TEST_NEAR("smooth_im(6,6,6)", smooth_im(6,6,6),   1.0f, 0.0001f);
  TEST_NEAR("smooth_im(6,6,7)", smooth_im(6,6,7),   0.0f, 0.0001f);
}

static void test_algo_smooth_121()
{
  test_smooth_121_i_byte();
  test_smooth_121_j_byte();
  test_smooth_121_k_byte();
  test_smooth_121_byte();
}

TESTMAIN(test_algo_smooth_121);
