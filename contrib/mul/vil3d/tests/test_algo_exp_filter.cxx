// This is mul/vil3d/tests/test_algo_exp_filter.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vil3d/algo/vil3d_exp_filter.h>
#include <vil3d/vil3d_crop.h>

static void test_exp_filter_float(vil3d_image_view<float>& image)
{
  vcl_cout << "**************************\n"
           << " Testing vil3d_exp_filter\n"
           << "**************************\n";

  vil3d_image_view<float> dest;

  image.fill(0.0f);
  for (unsigned int j=0;j<image.nj();++j) image(5,j,5)=90;

  vil3d_exp_filter_i(image,dest,0.5f);
  TEST_NEAR("exp_filter_i(0)",dest(5,5,5),30.0f,0.1f);
  TEST_NEAR("exp_filter_i(-1)",dest(4,5,5),15.0f,0.1f);
  TEST_NEAR("exp_filter_i(1)",dest(6,5,5),15.0f,0.1f);

  image.fill(0.0f);
  image(5,5,5)=81.0f;
  vil3d_exp_filter(image,dest,0.5f);
  TEST_NEAR("exp_filter(5,5,5)",dest(5,5,5),3.0f,0.01f);
  TEST_NEAR("exp_filter(4,5,5)",dest(4,5,5),1.5f,0.01f);
  TEST_NEAR("exp_filter(6,5,5)",dest(6,5,5),1.5f,0.01f);
  TEST_NEAR("exp_filter(5,6,5)",dest(5,6,5),1.5f,0.01f);
  TEST_NEAR("exp_filter(5,4,5)",dest(5,4,5),1.5f,0.01f);
  TEST_NEAR("exp_filter(5,5,4)",dest(5,5,4),1.5f,0.01f);
  TEST_NEAR("exp_filter(5,5,6)",dest(5,5,6),1.5f,0.01f);
}

static void test_exp_filter_float()
{
  vil3d_image_view<float> image(15,16,17);
  test_exp_filter_float(image);

  vcl_cout<<"Test non-contiguous image\n";
  vil3d_image_view<float> crop_image = vil3d_crop(image,2,10,3,11,4,12);
  test_exp_filter_float(crop_image);
}

static void test_algo_exp_filter()
{
  test_exp_filter_float();
}

TESTMAIN(test_algo_exp_filter);
