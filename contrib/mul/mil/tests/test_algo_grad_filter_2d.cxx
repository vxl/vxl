// This is mul/mil/tests/test_algo_grad_filter_2d.cxx
#include <vcl_iostream.h>
#include <mil/algo/mil_algo_grad_filter_2d.h>
#include <testlib/testlib_test.h>

#define DEBUG 1

void test_algo_grad_filter_2d_byte_float()
{
  vcl_cout << "********************************************\n";
  vcl_cout << " Testing mil_algo_grad_filter_2d byte-float\n";
  vcl_cout << "********************************************\n";

  mil_image_2d_of<unsigned char> src;
  mil_image_2d_of<float> gx,gy;

  int n = 10;
  src.resize(n,n);
  src.fill(1);
  // Create square in middle
  for (int y=3;y<=7;++y)
    for (int x=3;x<=7;++x) src(x,y)=9;

//  src.print_all(vcl_cout);

  mil_algo_grad_filter_2d<unsigned char, float>::filter_xy_3x3(gx,gy,src);
#ifdef DEBUG
  gx.print_all(vcl_cout);
  gy.print_all(vcl_cout);
#endif

  TEST_NEAR("Left edge of gx is zero",gx(0,5),0.f,1e-6);
  TEST_NEAR("Left edge of gy is zero",gy(0,5),0.f,1e-6);
  TEST_NEAR("Right edge of gx is zero",gx(n-1,5),0.f,1e-6);
  TEST_NEAR("Right edge of gy is zero",gy(n-1,5),0.f,1e-6);
  TEST_NEAR("Bottom edge of gx is zero",gx(5,0),0.f,1e-6);
  TEST_NEAR("Bottom edge of gy is zero",gy(5,0),0.f,1e-6);
  TEST_NEAR("Top edge of gx is zero",gx(5,n-1),0.f,1e-6);
  TEST_NEAR("Top edge of gy is zero",gy(5,n-1),0.f,1e-6);

  TEST_NEAR("Centre of gx image is zero",gx(5,5),0.f,1e-6);
  TEST_NEAR("Centre of gy image is zero",gy(5,5),0.f,1e-6);

  TEST_NEAR("Left of square in gx image",gx(2,5),4.f,1e-6);
  TEST_NEAR("Right of square in gx image",gx(7,5),-4.f,1e-6);
  TEST_NEAR("Top of square in gx image",gx(5,7),0.f,1e-6);
  TEST_NEAR("Bottom of square in gx image",gx(5,2),0.f,1e-6);

  TEST_NEAR("Left of square in gy image",gy(2,5),0.f,1e-6);
  TEST_NEAR("Right of square in gy image",gy(7,5),0.f,1e-6);
  TEST_NEAR("Top of square in gy image",gy(5,7),-4.f,1e-6);
  TEST_NEAR("Bottom of square in gy image",gy(5,2),4.f,1e-6);

  vcl_cout << "Testing central differences filter\n";

  mil_algo_grad_filter_2d<unsigned char, float>::filter_xy_1x3(gx,gy,src);
#ifdef DEBUG
  gx.print_all(vcl_cout);
  gy.print_all(vcl_cout);
#endif

  TEST_NEAR("Left edge of gx is zero",gx(0,5),0.f,1e-6);
  TEST_NEAR("Left edge of gy is zero",gy(0,5),0.f,1e-6);
  TEST_NEAR("Right edge of gx is zero",gx(n-1,5),0.f,1e-6);
  TEST_NEAR("Right edge of gy is zero",gy(n-1,5),0.f,1e-6);
  TEST_NEAR("Bottom edge of gx is zero",gx(5,0),0.f,1e-6);
  TEST_NEAR("Bottom edge of gy is zero",gy(5,0),0.f,1e-6);
  TEST_NEAR("Top edge of gx is zero",gx(5,n-1),0.f,1e-6);
  TEST_NEAR("Top edge of gy is zero",gy(5,n-1),0.f,1e-6);

  TEST_NEAR("Centre of gx image is zero",gx(5,5),0.f,1e-6);
  TEST_NEAR("Centre of gy image is zero",gy(5,5),0.f,1e-6);

  TEST_NEAR("Left of square in gx image",gx(2,5),4.f,1e-6);
  TEST_NEAR("Right of square in gx image",gx(7,5),-4.f,1e-6);
  TEST_NEAR("Top of square in gx image",gx(5,7),0.f,1e-6);
  TEST_NEAR("Bottom of square in gx image",gx(5,2),0.f,1e-6);

  TEST_NEAR("Left of square in gy image",gy(2,5),0.f,1e-6);
  TEST_NEAR("Right of square in gy image",gy(7,5),0.f,1e-6);
  TEST_NEAR("Top of square in gy image",gy(5,7),-4.f,1e-6);
  TEST_NEAR("Bottom of square in gy image",gy(5,2),4.f,1e-6);
}

void test_algo_grad_filter_2d_float_float()
{
  vcl_cout << "*********************************************\n";
  vcl_cout << " Testing mil_algo_grad_filter_2d float-float\n";
  vcl_cout << "*********************************************\n";

  vcl_cout << " Testing sobel filter\n";


  mil_image_2d_of<float> src;
  mil_image_2d_of<float> gx,gy;

  int n = 10;
  src.resize(n,n);
  src.fill(1);
  // Create square in middle
  for (int y=3;y<=7;++y)
    for (int x=3;x<=7;++x) src(x,y)=9;

//  src.print_all(vcl_cout);

  mil_algo_grad_filter_2d<float, float>::filter_xy_3x3(gx,gy,src);
#ifdef DEBUG
  gx.print_all(vcl_cout);
  gy.print_all(vcl_cout);
#endif

  TEST_NEAR("Left edge of gx is zero",gx(0,5),0.f,1e-6);
  TEST_NEAR("Left edge of gy is zero",gy(0,5),0.f,1e-6);
  TEST_NEAR("Right edge of gx is zero",gx(n-1,5),0.f,1e-6);
  TEST_NEAR("Right edge of gy is zero",gy(n-1,5),0.f,1e-6);
  TEST_NEAR("Bottom edge of gx is zero",gx(5,0),0.f,1e-6);
  TEST_NEAR("Bottom edge of gy is zero",gy(5,0),0.f,1e-6);
  TEST_NEAR("Top edge of gx is zero",gx(5,n-1),0.f,1e-6);
  TEST_NEAR("Top edge of gy is zero",gy(5,n-1),0.f,1e-6);

  TEST_NEAR("Centre of gx image is zero",gx(5,5),0.f,1e-6);
  TEST_NEAR("Centre of gy image is zero",gy(5,5),0.f,1e-6);

  TEST_NEAR("Left of square in gx image",gx(2,5),4.f,1e-6);
  TEST_NEAR("Right of square in gx image",gx(7,5),-4.f,1e-6);
  TEST_NEAR("Top of square in gx image",gx(5,7),0.f,1e-6);
  TEST_NEAR("Bottom of square in gx image",gx(5,2),0.f,1e-6);

  TEST_NEAR("Left of square in gy image",gy(2,5),0.f,1e-6);
  TEST_NEAR("Right of square in gy image",gy(7,5),0.f,1e-6);
  TEST_NEAR("Top of square in gy image",gy(5,7),-4.f,1e-6);
  TEST_NEAR("Bottom of square in gy image",gy(5,2),4.f,1e-6);

  vcl_cout << "Testing central differences filter\n";

  mil_algo_grad_filter_2d<float, float>::filter_xy_1x3(gx,gy,src);
#ifdef DEBUG
  gx.print_all(vcl_cout);
  gy.print_all(vcl_cout);
#endif

  TEST_NEAR("Left edge of gx is zero",gx(0,5),0.f,1e-6);
  TEST_NEAR("Left edge of gy is zero",gy(0,5),0.f,1e-6);
  TEST_NEAR("Right edge of gx is zero",gx(n-1,5),0.f,1e-6);
  TEST_NEAR("Right edge of gy is zero",gy(n-1,5),0.f,1e-6);
  TEST_NEAR("Bottom edge of gx is zero",gx(5,0),0.f,1e-6);
  TEST_NEAR("Bottom edge of gy is zero",gy(5,0),0.f,1e-6);
  TEST_NEAR("Top edge of gx is zero",gx(5,n-1),0.f,1e-6);
  TEST_NEAR("Top edge of gy is zero",gy(5,n-1),0.f,1e-6);

  TEST_NEAR("Centre of gx image is zero",gx(5,5),0.f,1e-6);
  TEST_NEAR("Centre of gy image is zero",gy(5,5),0.f,1e-6);

  TEST_NEAR("Left of square in gx image",gx(2,5),4.f,1e-6);
  TEST_NEAR("Right of square in gx image",gx(7,5),-4.f,1e-6);
  TEST_NEAR("Top of square in gx image",gx(5,7),0.f,1e-6);
  TEST_NEAR("Bottom of square in gx image",gx(5,2),0.f,1e-6);

  TEST_NEAR("Left of square in gy image",gy(2,5),0.f,1e-6);
  TEST_NEAR("Right of square in gy image",gy(7,5),0.f,1e-6);
  TEST_NEAR("Top of square in gy image",gy(5,7),-4.f,1e-6);
  TEST_NEAR("Bottom of square in gy image",gy(5,2),4.f,1e-6);
}

void test_algo_grad_filter_2d()
{
  test_algo_grad_filter_2d_byte_float();
  test_algo_grad_filter_2d_float_float();
}


TESTLIB_DEFINE_MAIN(test_algo_grad_filter_2d);
