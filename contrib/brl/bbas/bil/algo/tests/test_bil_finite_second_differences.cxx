#include <testlib/testlib_test.h>
#include <vil/vil_print.h>
#include <bil/algo/bil_finite_differences.h>
#include <bil/algo/bil_finite_second_differences.h>


static void test_bil_finite_second_differences()
{
  int dim = 5;
  vil_image_view<float> testim(dim,dim);
  vil_image_view<float> dxp,dxm,dxc,dyp,dym,dyc;
  vil_image_view<float> dxx,dxy,dyy;

  testim.fill(1);
  bil_finite_differences(testim, dxp, dxm, dxc, dyp, dym, dyc);
  bil_finite_second_differences(testim, dxp, dxm, dyp, dym,dxx,dyy,dxy);

  float dxxsum=0;
  float dyysum=0;
  float dxysum=0;
  for (unsigned j = 0 ; j < testim.nj(); j++) {
    for (unsigned i = 0 ; i < testim.ni(); i++) {
      dxxsum+=dxx(i,j);
      dyysum+=dyy(i,j);
      dxysum+=dxy(i,j);
    }
  }

  TEST_NEAR("D_xx constant image", dxxsum,0, 0.001);
  TEST_NEAR("D_yy constant image", dyysum,0, 0.001);
  TEST_NEAR("D_xy constant image", dxysum,0, 0.001);

  for (unsigned j = 0 ; j < testim.nj(); j++) {
    for (unsigned i = 0 ; i < testim.ni(); i++) {
      if (i < static_cast<unsigned>(dim/2)) {
        testim(i,j) = 1;
      }
      else {
        testim(i,j) = 0;
      }
    }
  }

  bil_finite_differences(testim, dxp, dxm, dxc, dyp, dym, dyc);
  bil_finite_second_differences(testim, dxp, dxm, dyp, dym,dxx,dyy,dxy);
  /*
  vil_print_all(std::cout,dxx);
  vil_print_all(std::cout,dyy);
  vil_print_all(std::cout,dxy);
  */
  std::cout << "testing half-one half-zero (along i-axis) image" << std::endl;
  for (unsigned j = 0 ; j < testim.nj(); j++) {
    TEST_NEAR("D_xx"  ,     dxx(0,j)    ,    0    ,     0.001);
    TEST_NEAR("D_yy"  ,     dyy(0,j)    ,    0    ,     0.001);
    TEST_NEAR("D_xy"  ,     dxy(0,j)    ,    0    ,     0.001);

    TEST_NEAR("D_xx"  ,     dxx(testim.ni()-1,j)    ,    0    ,     0.001);
    TEST_NEAR("D_yy"  ,     dyy(testim.ni()-1,j)    ,    0    ,     0.001);
    TEST_NEAR("D_xy"  ,     dxy(testim.ni()-1,j)    ,    0    ,     0.001);

    TEST_NEAR("D_xx"  ,     dxx(dim/2 - 1,j)    ,   -1  ,     0.001);
    TEST_NEAR("D_yy"  ,     dyy(dim/2 - 1,j)    ,    0  ,     0.001);
    TEST_NEAR("D_xy"  ,     dxy(dim/2 - 1,j)    ,    0  ,     0.001);

    TEST_NEAR("D_xx"  ,     dxx(dim/2,j)    ,    1  ,     0.001);
    TEST_NEAR("D_yy"  ,     dyy(dim/2,j)    ,    0  ,     0.001);
    TEST_NEAR("D_xy"  ,     dxy(dim/2,j)    ,    0  ,     0.001);
  }

  for (unsigned j = 0 ; j < testim.nj(); j++) {
    for (unsigned i = 0 ; i < testim.ni(); i++) {
      if (j < static_cast<unsigned>(dim/2)) {
        testim(i,j) = 1;
      }
      else {
        testim(i,j) = 0;
      }
    }
  }

  bil_finite_differences(testim, dxp, dxm, dxc, dyp, dym, dyc);
  bil_finite_second_differences(testim, dxp, dxm, dyp, dym,dxx,dyy,dxy);
  /*
  vil_print_all(std::cout,dxx);
  vil_print_all(std::cout,dxy);
  */

  vil_print_all(std::cout,testim);
  vil_print_all(std::cout,dyy);
  std::cout << "testing half-one half-zero (along j-axis) image" << std::endl;
  for (unsigned i = 0 ; i < testim.nj(); i++) {
    TEST_NEAR("D_xx"  ,     dxx(i,0)    ,    0    ,     0.001);
    TEST_NEAR("D_yy"  ,     dyy(i,0)    ,    0    ,     0.001);
    TEST_NEAR("D_xy"  ,     dxy(i,0)    ,    0    ,     0.001);

    TEST_NEAR("D_xx"  ,     dxx(i,testim.nj()-1)    ,    0    ,     0.001);
    TEST_NEAR("D_yy"  ,     dyy(i,testim.nj()-1)    ,    0    ,     0.001);
    TEST_NEAR("D_xy"  ,     dxy(i,testim.nj()-1)    ,    0    ,     0.001);

    TEST_NEAR("D_xx"  ,     dxx(i,dim/2 - 1)    ,   0 ,     0.001);
    TEST_NEAR("D_yy"  ,     dyy(i,dim/2 - 1)    ,   -1  ,     0.001);
    TEST_NEAR("D_xy"  ,     dxy(i,dim/2 - 1)    ,   0  ,     0.001);

    TEST_NEAR("D_xx"  ,     dxx(i,dim/2)    ,    0  ,     0.001);
    TEST_NEAR("D_yy"  ,     dyy(i,dim/2)    ,    1  ,     0.001);
    TEST_NEAR("D_xy"  ,     dxy(i,dim/2)    ,    0  ,     0.001);
  }

  for (unsigned j = 0 ; j < testim.nj(); j++) {
    for (unsigned i = 0 ; i < testim.ni(); i++) {
      if (i==j) {
        testim(i,j) = 1;
      }
      else {
        testim(i,j) = 0;
      }
    }
  }

  bil_finite_differences(testim, dxp, dxm, dxc, dyp, dym, dyc);
  bil_finite_second_differences(testim, dxp, dxm, dyp, dym,dxx,dyy,dxy);

  vil_print_all(std::cout,dxx);
  vil_print_all(std::cout,dyy);

  std::cout << "testing diagonal image" << std::endl;
  for (unsigned j = 0 ; j < testim.nj(); ++j) {
    for (unsigned i = 0 ; i < testim.ni(); ++i) {
      if ((i==0 && j==0) || (i+1 == testim.ni() && j+1 == testim.nj())) {
        std::cout << "corners on diagonal " << std::endl;
        TEST_NEAR("D_xx"  ,     dxx(i,j)    ,    -1    ,     0.001);
        TEST_NEAR("D_yy"  ,     dyy(i,j)    ,    -1    ,     0.001);
      }
      else if (i == j) {
        std::cout << "non-corner on diagonal " << std::endl;
        TEST_NEAR("D_xx"  ,     dxx(i,j)    ,    -2    ,     0.001);
        TEST_NEAR("D_yy"  ,     dyy(i,j)    ,    -2    ,     0.001);
      }
      else if (i == j + 1 || i == j-1 ) {
        std::cout << "super/sub diagonal " << std::endl;
        TEST_NEAR("D_xx"  ,     dxx(i,j)    ,     1    ,     0.001);
        TEST_NEAR("D_yy"  ,     dyy(i,j)    ,     1    ,     0.001);
      }
      else {
        std::cout << "elsewhere " << std::endl;
        TEST_NEAR("D_xx"  ,     dxx(i,j)    ,     0    ,     0.001);
        TEST_NEAR("D_yy"  ,     dyy(i,j)    ,     0    ,     0.001);
      }
    }
  }

  vil_print_all(std::cout,dxy);
  for (unsigned j = 0 ; j < testim.nj(); j++)
  {
    for (unsigned i = 0 ; i < testim.ni(); i++)
    {
      if (i == j) {
        std::cout << "corner on diagonal\n"
                 << '(' << i << ',' << j << ')' << std::endl;
        TEST_NEAR("D_xy"  ,     dxy(i,j)    ,    0.5   ,     0.001);
      }
      else if (i == j - 2 || j == i- 2 ) {
        std::cout << "second super/sub diagonals\n"
                 << '(' << i << ',' << j << ')' << std::endl;
        TEST_NEAR("D_xy"  ,     dxy(i,j)    ,    -0.25   ,     0.001);
      }
      else if (i + j == 1 || i + j == testim.ni() + testim.nj() - 3 ) {
        std::cout << "i + j == 1 || i + j == testim.ni() + testim.nj() - 1\n"
                 << '(' << i << ',' << j << ')' << std::endl;;
        TEST_NEAR("D_xy"  ,     dxy(i,j)    ,    0.25   ,     0.001);
      }
      else if (i + j == 2 || i + j == testim.ni() + testim.nj() - 4 ) {
        std::cout << "i + j == 1 || i + j == testim.ni() + testim.nj() - 2\n"
                 << '(' << i << ',' << j << ')' << std::endl;
        TEST_NEAR("D_xy"  ,     dxy(i,j)    ,    -0.25   ,     0.001);
      }
      else {
        std::cout << "elsewhere\n"
                 << '(' << i << ',' << j << ')' << std::endl;
        TEST_NEAR("D_xy"  ,     dxy(i,j)    ,    0   ,     0.001);
      }
    }
  }
}

TESTMAIN(test_bil_finite_second_differences);
