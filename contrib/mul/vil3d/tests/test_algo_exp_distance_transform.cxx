// This is mul/vil3d/tests/test_algo_exp_distance_transform.cxx
#include <iostream>
#include <cmath>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil3d/algo/vil3d_exp_distance_transform.h>
#include <vil3d/vil3d_crop.h>

static void test_algo_exp_distance_transform_float()
{
  std::cout << "**************************************\n"
           << " Testing vil3d_exp_distance_transform\n"
           << "*********************************** **\n";

  vil3d_image_view<float> image(11,11,11);

  image.fill(0.0f);
  image(5,5,5)=100.0f;

  vil3d_exp_distance_transform(image,1.0,2.0,4.0,2);

  TEST_NEAR("image(5,5,5)",image(5,5,5),100.0f,0.01f);
  TEST_NEAR("image(4,5,5)",image(4,5,5), 50.0f,0.01f);
  TEST_NEAR("image(6,5,5)",image(6,5,5), 50.0f,0.01f);
  TEST_NEAR("image(5,3,5)",image(5,3,5), 50.0f,0.01f);
  TEST_NEAR("image(5,7,5)",image(5,7,5), 50.0f,0.01f);
  TEST_NEAR("image(5,5,1)",image(5,5,1), 50.0f,0.01f);
  TEST_NEAR("image(5,5,9)",image(5,5,9), 50.0f,0.01f);

  TEST_NEAR("image(4,3,5)",image(4,3,5),
            100.0f*std::pow(0.5,std::sqrt(2.0)),0.01f);
  TEST_NEAR("image(4,3,3)",image(4,3,3),
            100.0f*std::pow(0.5,std::sqrt(2.25)),0.01f);
  TEST_NEAR("image(0,5,5)",image(0,5,5),
            100.0f*std::pow(0.5,5.0),0.01f);
  TEST_NEAR("image(10,5,5)",image(10,5,5),
            100.0f*std::pow(0.5,5.0),0.01f);
  TEST_NEAR("image(5,0,5)",image(5,0,5),
            100.0f*std::pow(0.5,2.5),0.01f);
  TEST_NEAR("image(5,10,5)",image(5,10,5),
            100.0f*std::pow(0.5,2.5),0.01f);
  TEST_NEAR("image(5,5,0)",image(5,5,0),
            100.0f*std::pow(0.5,1.25),0.01f);
  TEST_NEAR("image(5,5,10)",image(5,5,10),
            100.0f*std::pow(0.5,1.25),0.01f);
}

static void test_algo_exp_distance_transform()
{
  test_algo_exp_distance_transform_float();
}

TESTMAIN(test_algo_exp_distance_transform);
