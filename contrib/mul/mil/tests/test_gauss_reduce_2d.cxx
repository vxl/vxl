// This is mul/mil/tests/test_gauss_reduce_2d.cxx
#include <vcl_iostream.h>
#include <mil/mil_image_2d_of.h>
#include <mil/mil_gauss_reduce_2d.h>
#include <vxl_config.h>
#include <testlib/testlib_test.h>

void test_gauss_reduce_2d_byte(int nx)
{
  vcl_cout << "******************************************\n"
           << " Testing mil_gauss_reduce_2d (byte)(nx="<<nx<<")\n"
           << "******************************************\n";

  mil_image_2d_of<vxl_byte> image0;
  image0.resize(nx,3);
  mil_image_2d_of<vxl_byte> reduced_x;
  reduced_x.resize((nx+1)/2,3);

  for (int y=0;y<image0.ny();++y)
    for (int x=0;x<image0.nx();++x)
      image0(x,y) = x+y*10;

  mil_gauss_reduce_2d(reduced_x.plane(0),reduced_x.xstep(),reduced_x.ystep(),
                      image0.plane(0),image0.nx(),image0.ny(),
                      image0.xstep(),image0.ystep());

  vcl_cout<<"Original: "; image0.print_all(vcl_cout);
  vcl_cout<<"reduced_x : "; reduced_x.print_all(vcl_cout);

  TEST("First element",reduced_x(0,1),image0(0,1));
  TEST("Next element", reduced_x(1,1),image0(2,1));
  int L = (nx-1)/2;
  TEST("Last element", reduced_x(L,1),image0(2*L,1));


  mil_image_2d_of<vxl_byte> test2;
  test2.resize(nx,3);
  test2.fill(222);
  mil_gauss_reduce_2d(test2.plane(0),test2.xstep(),test2.ystep(),
                      image0.plane(0),image0.nx(),image0.ny(),
                      image0.xstep(),image0.ystep());
  TEST("No overrun",test2(L+1,1),222);


  // Test it can be used to smooth in y by swapping ordinates
  mil_image_2d_of<vxl_byte> image1;
  image1.resize(3,nx);
  mil_image_2d_of<vxl_byte> reduced_y;
  reduced_y.resize(3,(nx+1)/2);

  for (int y=0;y<image1.ny();++y)
    for (int x=0;x<image1.nx();++x)
      image1(x,y) = x+y*10;

  mil_gauss_reduce_2d(reduced_y.plane(0),reduced_y.ystep(),reduced_y.xstep(),
                      image1.plane(0),image1.ny(),image1.nx(),
                      image1.ystep(),image1.xstep());

  vcl_cout<<"Original: "; image1.print_all(vcl_cout);
  vcl_cout<<"reduced_y : "; reduced_y.print_all(vcl_cout);

  TEST("First element",reduced_y(1,0),image1(1,0));
  TEST("Next element", reduced_y(1,1),image1(1,2));
  TEST("Last element", reduced_y(1,L),image1(1,2*L));
}

void test_gauss_reduce_2d_float(int nx)
{
  vcl_cout << "*******************************************\n"
           << " Testing mil_gauss_reduce_2d (float)(nx="<<nx<<")\n"
           << "*******************************************\n";

  mil_image_2d_of<float> image0;
  image0.resize(nx,3);
  mil_image_2d_of<float> reduced_x;
  reduced_x.resize((nx+1)/2,3);

  for (int y=0;y<image0.ny();++y)
    for (int x=0;x<image0.nx();++x)
      image0(x,y) = 0.1f*x+y;

  mil_gauss_reduce_2d(reduced_x.plane(0),reduced_x.xstep(),reduced_x.ystep(),
                      image0.plane(0),image0.nx(),image0.ny(),
                      image0.xstep(),image0.ystep());

  vcl_cout<<"Original: "; image0.print_all(vcl_cout);
  vcl_cout<<"reduced_x : "; reduced_x.print_all(vcl_cout);

  TEST_NEAR("First element",reduced_x(0,1),image0(0,1),1e-6);
  TEST_NEAR("Next element",reduced_x(1,1),image0(2,1),1e-6);
  int L = (nx-1)/2;
  TEST_NEAR("Last element",reduced_x(L,1),image0(2*L,1),1e-6);


  mil_image_2d_of<float> test2;
  test2.resize(nx,3);
  test2.fill(22.2f);
  mil_gauss_reduce_2d(test2.plane(0),test2.xstep(),test2.ystep(),
                      image0.plane(0),image0.nx(),image0.ny(),
                      image0.xstep(),image0.ystep());
  TEST_NEAR("No overrun",test2(L+1,1),22.2f,1e-6);
}

void test_gauss_reduce_121_2d_byte(int nx, int ny)
{
  vcl_cout << "**********************************************\n"
           << " Testing mil_gauss_reduce_121_2d (byte)(nx="<<nx<<")\n"
           << "**********************************************\n";

  mil_image_2d_of<vxl_byte> image0;
  image0.resize(nx,ny);
  mil_image_2d_of<vxl_byte> reduced_x;
  reduced_x.resize((nx+1)/2,(ny+1)/2);

  for (int y=0;y<image0.ny();++y)
    for (int x=0;x<image0.nx();++x)
      image0(x,y) = x+y*10;

  mil_gauss_reduce_121_2d(reduced_x.plane(0),reduced_x.xstep(),reduced_x.ystep(),
                          image0.plane(0),image0.nx(),image0.ny(),
                          image0.xstep(),image0.ystep());

  vcl_cout<<"Original: "; image0.print_all(vcl_cout);
  vcl_cout<<"reduced_x : "; reduced_x.print_all(vcl_cout);

  TEST("First element",reduced_x(0,1),image0(0,2));
  TEST("Next element",reduced_x(1,1),image0(2,2));
  int Lx = (nx+1)/2;
  int Ly = (ny+1)/2;
  TEST("Last element in x",reduced_x(Lx-1,1),image0(2*(Lx-1),2));
  TEST("Last element in y",reduced_x(1,Ly-1),image0(2,2*(Ly-1)));

  mil_image_2d_of<vxl_byte> test2;
  test2.resize(nx,ny);
  test2.fill(222);
  mil_gauss_reduce_121_2d(test2.plane(0),test2.xstep(),test2.ystep(),
                          image0.plane(0),image0.nx(),image0.ny(),
                          image0.xstep(),image0.ystep());
  TEST("No overrun in x",test2(Lx,1), 222);
  TEST("No overrun in y",test2(1,Ly), 222);

  image0.fill(17);
  mil_gauss_reduce_121_2d(test2.plane(0),test2.xstep(),test2.ystep(),
                          image0.plane(0),image0.nx(),image0.ny(),
                          image0.xstep(),image0.ystep());
  TEST("Smoothing correct",test2(1,1), 17);
  vcl_cout<<"Value at (1,1):"<<int(test2(1,1))<<vcl_endl;
}

void test_gauss_reduce_121_2d_float(int nx, int ny)
{
  vcl_cout << "***********************************************\n"
           << " Testing mil_gauss_reduce_121_2d (float)(nx="<<nx<<")\n"
           << "***********************************************\n";

  mil_image_2d_of<float> image0;
  image0.resize(nx,ny);
  mil_image_2d_of<float> reduced_x;
  reduced_x.resize((nx+1)/2,(ny+1)/2);

  for (int y=0;y<image0.ny();++y)
    for (int x=0;x<image0.nx();++x)
      image0(x,y) = 0.1f*x+y;

  mil_gauss_reduce_121_2d(reduced_x.plane(0),reduced_x.xstep(),reduced_x.ystep(),
                          image0.plane(0),image0.nx(),image0.ny(),
                          image0.xstep(),image0.ystep());

  vcl_cout<<"Original: "; image0.print_all(vcl_cout);
  vcl_cout<<"reduced_x : "; reduced_x.print_all(vcl_cout);


  TEST_NEAR("First element",reduced_x(0,1),image0(0,2),1e-6);
  TEST_NEAR("Next element",reduced_x(1,1),image0(2,2),1e-6);
  int Lx = (nx+1)/2;
  int Ly = (ny+1)/2;
  TEST_NEAR("Last element in x",reduced_x(Lx-1,1),image0(2*(Lx-1),2),1e-6);
  TEST_NEAR("Last element in y",reduced_x(1,Ly-1),image0(2,2*(Ly-1)),1e-6);

  mil_image_2d_of<float> test2;
  test2.resize(nx,ny);
  test2.fill(22.2f);
  mil_gauss_reduce_121_2d(test2.plane(0),test2.xstep(),test2.ystep(),
                          image0.plane(0),image0.nx(),image0.ny(),
                          image0.xstep(),image0.ystep());
  TEST_NEAR("No overrun in x",test2(Lx,1),22.2f,1e-6);
  TEST_NEAR("No overrun in y",test2(1,Ly),22.2f,1e-6);

  image0.fill(1.7f);
  mil_gauss_reduce_121_2d(test2.plane(0),test2.xstep(),test2.ystep(),
                          image0.plane(0),image0.nx(),image0.ny(),
                          image0.xstep(),image0.ystep());
  TEST_NEAR("Smoothing correct",test2(1,1),1.7f,1e-6);
  vcl_cout<<"Value at (1,1):"<<float(test2(1,1))<<vcl_endl;
}

void test_gauss_reduce_2d()
{
  test_gauss_reduce_2d_byte(7);
  test_gauss_reduce_2d_byte(6);
  test_gauss_reduce_2d_float(7);
  test_gauss_reduce_2d_float(6);

  test_gauss_reduce_121_2d_byte(6,6);
  test_gauss_reduce_121_2d_byte(7,7);
  test_gauss_reduce_121_2d_float(6,6);
  test_gauss_reduce_121_2d_float(7,7);
}


TESTLIB_DEFINE_MAIN(test_gauss_reduce_2d);
