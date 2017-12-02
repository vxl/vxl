// This is mul/vil3d/tests/test_algo_grad_1x3.cxx
#include <iostream>
#include <testlib/testlib_test.h>
#include <vcl_compiler.h>
#include <vxl_config.h> // for vxl_byte
#include <vil3d/algo/vil3d_grad_1x3.h>
#include <vil3d/vil3d_crop.h>
#include <vil3d/vil3d_print.h>

static void test_grad_1x3_1plane_byte(vil3d_image_view<vxl_byte>& image)
{
  std::cout << "*******************************\n"
           << " Testing vil3d_grad_1x3_1plane\n"
           << "*******************************\n";

  image.fill(vxl_byte(0));
  image(5,5,5)=vxl_byte(100);

  unsigned ni = image.ni();
  unsigned nj = image.nj();
  unsigned nk = image.nk();
  vil3d_image_view<float> gradi(ni,nj,nk), gradj(ni,nj,nk), gradk(ni,nj,nk);
  vil3d_grad_1x3_1plane(image,gradi,gradj,gradk);


  TEST_NEAR("gradi(5,5,5)", gradi(5,5,5),  0.0f, 0.0001f);
  TEST_NEAR("gradj(5,5,5)", gradj(5,5,5),  0.0f, 0.0001f);
  TEST_NEAR("gradk(5,5,5)", gradk(5,5,5),  0.0f, 0.0001f);
  TEST_NEAR("gradi(4,5,5)", gradi(4,5,5), 50.0f, 0.0001f);
  TEST_NEAR("gradj(4,5,5)", gradj(4,5,5),  0.0f, 0.0001f);
  TEST_NEAR("gradk(4,5,5)", gradk(4,5,5),  0.0f, 0.0001f);
  TEST_NEAR("gradi(6,5,5)", gradi(6,5,5),-50.0f, 0.0001f);
  TEST_NEAR("gradj(6,5,5)", gradj(6,5,5),  0.0f, 0.0001f);
  TEST_NEAR("gradk(6,5,5)", gradk(6,5,5),  0.0f, 0.0001f);
  TEST_NEAR("gradi(5,4,5)", gradi(5,4,5),  0.0f, 0.0001f);
  TEST_NEAR("gradj(5,4,5)", gradj(5,4,5), 50.0f, 0.0001f);
  TEST_NEAR("gradk(5,4,5)", gradk(5,4,5),  0.0f, 0.0001f);
  TEST_NEAR("gradi(5,5,4)", gradi(5,5,4),  0.0f, 0.0001f);
  TEST_NEAR("gradj(5,5,4)", gradj(5,5,4),  0.0f, 0.0001f);
  TEST_NEAR("gradk(5,5,4)", gradk(5,5,4), 50.0f, 0.0001f);
}

static void test_grad_1x3_byte(vil3d_image_view<vxl_byte>& image)
{
  std::cout << "************************\n"
           << " Testing vil3d_grad_1x3\n"
           << "************************\n";

  image.fill(vxl_byte(0));
  image(5,5,5)=vxl_byte(100);

  vil3d_image_view<float> gradi, gradj, gradk;
  vil3d_grad_1x3(image,gradi,gradj,gradk);

  std::cout << " ********SRC***********\n";
  vil3d_print_all(std::cout, image);

  std::cout << " ********gradi***********\n";
  vil3d_print_all(std::cout, gradi);
  std::cout << " ********gradj***********\n";
  vil3d_print_all(std::cout, gradj);
  std::cout << " ********gradk***********\n";
  vil3d_print_all(std::cout, gradk);


  TEST_NEAR("gradi(5,5,5)", gradi(5,5,5),  0.0f, 0.0001f);
  TEST_NEAR("gradj(5,5,5)", gradj(5,5,5),  0.0f, 0.0001f);
  TEST_NEAR("gradk(5,5,5)", gradk(5,5,5),  0.0f, 0.0001f);
  TEST_NEAR("gradi(4,5,5)", gradi(4,5,5), 50.0f, 0.0001f);
  TEST_NEAR("gradj(4,5,5)", gradj(4,5,5),  0.0f, 0.0001f);
  TEST_NEAR("gradk(4,5,5)", gradk(4,5,5),  0.0f, 0.0001f);
  TEST_NEAR("gradi(6,5,5)", gradi(6,5,5),-50.0f, 0.0001f);
  TEST_NEAR("gradj(6,5,5)", gradj(6,5,5),  0.0f, 0.0001f);
  TEST_NEAR("gradk(6,5,5)", gradk(6,5,5),  0.0f, 0.0001f);
  TEST_NEAR("gradi(5,4,5)", gradi(5,4,5),  0.0f, 0.0001f);
  TEST_NEAR("gradj(5,4,5)", gradj(5,4,5), 50.0f, 0.0001f);
  TEST_NEAR("gradk(5,4,5)", gradk(5,4,5),  0.0f, 0.0001f);
  TEST_NEAR("gradi(5,5,4)", gradi(5,5,4),  0.0f, 0.0001f);
  TEST_NEAR("gradj(5,5,4)", gradj(5,5,4),  0.0f, 0.0001f);
  TEST_NEAR("gradk(5,5,4)", gradk(5,5,4), 50.0f, 0.0001f);
}

static void test_grad_1x3_mag_sq_byte(vil3d_image_view<vxl_byte>& image)
{
  std::cout << "*******************************\n"
           << " Testing vil3d_grad_1x3_mag_sq\n"
           << "*******************************\n";

  image.fill(vxl_byte(0));
  image(5,5,5)=vxl_byte(100);

  vil3d_image_view<float> grad_mag2;
  vil3d_grad_1x3_mag_sq(image,grad_mag2);

  TEST_NEAR("grad_mag2(5,5,5)", grad_mag2(5,5,5),    0.0f, 0.0001f);
  TEST_NEAR("grad_mag2(4,5,5)", grad_mag2(4,5,5), 2500.0f, 0.0001f);
  TEST_NEAR("grad_mag2(6,5,5)", grad_mag2(6,5,5), 2500.0f, 0.0001f);
  TEST_NEAR("grad_mag2(5,4,5)", grad_mag2(5,4,5), 2500.0f, 0.0001f);
  TEST_NEAR("grad_mag2(5,5,4)", grad_mag2(5,5,4), 2500.0f, 0.0001f);
}

static void test_grad_1x3_i_byte()
{
  std::cout << "*************************\n"
           << " Testing vil3d_grad_1x3_i\n"
           << "*************************\n";

  vil3d_image_view<vxl_byte> image(10,10,10);
  image.fill(vxl_byte(0));
  image(5,5,5)=vxl_byte(100);

  vil3d_image_view<float> grad_im;
  vil3d_grad_1x3_i(image,grad_im);

  TEST_NEAR("grad_im(4,5,5)", grad_im(4,5,5),  50.0f, 0.0001f);
  TEST_NEAR("grad_im(5,5,5)", grad_im(5,5,5),   0.0f, 0.0001f);
  TEST_NEAR("grad_im(6,5,5)", grad_im(6,5,5), -50.0f, 0.0001f);
  TEST_NEAR("grad_im(5,4,5)", grad_im(5,4,5),   0.0f, 0.0001f);
  TEST_NEAR("grad_im(5,6,5)", grad_im(5,6,5),   0.0f, 0.0001f);
  TEST_NEAR("grad_im(5,5,4)", grad_im(5,5,4),   0.0f, 0.0001f);
  TEST_NEAR("grad_im(5,5,6)", grad_im(5,5,6),   0.0f, 0.0001f);
}

static void test_grad_1x3_j_byte()
{
  std::cout << "*************************\n"
           << " Testing vil3d_grad_1x3_j\n"
           << "*************************\n";

  vil3d_image_view<vxl_byte> image(10,10,10);
  image.fill(vxl_byte(0));
  image(5,5,5)=vxl_byte(100);

  vil3d_image_view<float> grad_im;
  vil3d_grad_1x3_j(image,grad_im);

  TEST_NEAR("grad_im(4,5,5)", grad_im(4,5,5),   0.0f, 0.0001f);
  TEST_NEAR("grad_im(5,5,5)", grad_im(5,5,5),   0.0f, 0.0001f);
  TEST_NEAR("grad_im(6,5,5)", grad_im(6,5,5),   0.0f, 0.0001f);
  TEST_NEAR("grad_im(5,4,5)", grad_im(5,4,5),  50.0f, 0.0001f);
  TEST_NEAR("grad_im(5,6,5)", grad_im(5,6,5), -50.0f, 0.0001f);
  TEST_NEAR("grad_im(5,5,4)", grad_im(5,5,4),   0.0f, 0.0001f);
  TEST_NEAR("grad_im(5,5,6)", grad_im(5,5,6),   0.0f, 0.0001f);
}

static void test_grad_1x3_k_byte()
{
  std::cout << "*************************\n"
           << " Testing vil3d_grad_1x3_k\n"
           << "*************************\n";

  vil3d_image_view<vxl_byte> image(10,10,10);
  image.fill(vxl_byte(0));
  image(5,5,5)=vxl_byte(100);

  vil3d_image_view<float> grad_im;
  vil3d_grad_1x3_k(image,grad_im);

  TEST_NEAR("grad_im(4,5,5)", grad_im(4,5,5),   0.0f, 0.0001f);
  TEST_NEAR("grad_im(5,5,5)", grad_im(5,5,5),   0.0f, 0.0001f);
  TEST_NEAR("grad_im(6,5,5)", grad_im(6,5,5),   0.0f, 0.0001f);
  TEST_NEAR("grad_im(5,4,5)", grad_im(5,4,5),   0.0f, 0.0001f);
  TEST_NEAR("grad_im(5,6,5)", grad_im(5,6,5),   0.0f, 0.0001f);
  TEST_NEAR("grad_im(5,5,4)", grad_im(5,5,4),  50.0f, 0.0001f);
  TEST_NEAR("grad_im(5,5,6)", grad_im(5,5,6), -50.0f, 0.0001f);
}

static void test_grad_1x3_byte()
{
  vil3d_image_view<vxl_byte> image(15,15,15);

  test_grad_1x3_1plane_byte(image);
  test_grad_1x3_byte(image);
  test_grad_1x3_mag_sq_byte(image);

  std::cout<<"Test non-contiguous image\n";
  vil3d_image_view<vxl_byte> crop_image = vil3d_crop(image,2,10,3,10,4,10);

  test_grad_1x3_byte(crop_image);

  test_grad_1x3_i_byte();
  test_grad_1x3_j_byte();
  test_grad_1x3_k_byte();
}

static void test_algo_grad_1x3()
{
  test_grad_1x3_byte();
}

TESTMAIN(test_algo_grad_1x3);
