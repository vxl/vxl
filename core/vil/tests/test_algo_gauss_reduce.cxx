// This is mul/vil2/tests/test_algo_gauss_reduce.cxx
#include <vcl_iostream.h>
#include <vil2/vil2_image_view.h>
#include <vil2/vil2_image_view_functions.h>
#include <vil2/algo/vil2_algo_gauss_reduce.h>
#include <vil/vil_byte.h>
#include <testlib/testlib_test.h>

void test_algo_gauss_reduce_byte(int nx)
{
  vcl_cout << "***********************************************\n";
  vcl_cout << " Testing vil2_algo_gauss_reduce (byte)(nx="<<nx<<")\n";
  vcl_cout << "***********************************************\n";

  vil2_image_view<vil_byte> image0;
  image0.resize(nx,3);
  vil2_image_view<vil_byte> reduced_x;
  reduced_x.resize((nx+1)/2,3);

  for (int y=0;y<image0.nj();++y)
    for (int x=0;x<image0.ni();++x)
      image0(x,y) = x+y*10;

  vil2_algo_gauss_reduce(reduced_x.top_left_ptr(),reduced_x.istep(),reduced_x.jstep(),
                         image0.top_left_ptr(),image0.ni(),image0.nj(),
                         image0.istep(),image0.jstep());

  vcl_cout<<"Original: "; vil2_print_all(vcl_cout,image0); vcl_cout<<vcl_endl;
  vcl_cout<<"reduced_x : "; vil2_print_all(vcl_cout,reduced_x); vcl_cout<<vcl_endl;

  TEST("First element",reduced_x(0,1),image0(0,1));
  TEST("Next element", reduced_x(1,1),image0(2,1));
  int L = (nx-1)/2;
  TEST("Last element", reduced_x(L,1),image0(2*L,1));


  vil2_image_view<vil_byte> test2;
  test2.resize(nx,3);
  test2.fill(222);
  vil2_algo_gauss_reduce(test2.top_left_ptr(),test2.istep(),test2.jstep(),
                         image0.top_left_ptr(),image0.ni(),image0.nj(),
                         image0.istep(),image0.jstep());
  TEST("No overrun",test2(L+1,1),222);


  // Test it can be used to smooth in y by swapping ordinates
  vil2_image_view<vil_byte> image1;
  image1.resize(3,nx);
  vil2_image_view<vil_byte> reduced_y;
  reduced_y.resize(3,(nx+1)/2);

  for (int y=0;y<image1.nj();++y)
    for (int x=0;x<image1.ni();++x)
      image1(x,y) = x+y*10;

  vil2_algo_gauss_reduce(reduced_y.top_left_ptr(),reduced_y.jstep(),reduced_y.istep(),
                         image1.top_left_ptr(),image1.nj(),image1.ni(),
                         image1.jstep(),image1.istep());

  vcl_cout<<"Original: "; vil2_print_all(vcl_cout,image1); vcl_cout<<vcl_endl;
  vcl_cout<<"reduced_y : "; vil2_print_all(vcl_cout,reduced_y); vcl_cout<<vcl_endl;

  TEST("First element",reduced_y(1,0),image1(1,0));
  TEST("Next element", reduced_y(1,1),image1(1,2));
  TEST("Last element", reduced_y(1,L),image1(1,2*L));
}

void test_algo_gauss_reduce_float(int nx)
{
  vcl_cout << "*******************************************\n";
  vcl_cout << " Testing vil2_algo_gauss_reduce (float)(nx="<<nx<<")\n";
  vcl_cout << "*******************************************\n";

  vil2_image_view<float> image0;
  image0.resize(nx,3);
  vil2_image_view<float> reduced_x;
  reduced_x.resize((nx+1)/2,3);

  for (int y=0;y<image0.nj();++y)
    for (int x=0;x<image0.ni();++x)
      image0(x,y) = 0.1f*x+y;

  vil2_algo_gauss_reduce(reduced_x.top_left_ptr(),reduced_x.istep(),reduced_x.jstep(),
                         image0.top_left_ptr(),image0.ni(),image0.nj(),
                         image0.istep(),image0.jstep());

  vcl_cout<<"Original: "; vil2_print_all(vcl_cout,image0); vcl_cout<<vcl_endl;
  vcl_cout<<"reduced_x : "; vil2_print_all(vcl_cout,reduced_x); vcl_cout<<vcl_endl;

  TEST_NEAR("First element",reduced_x(0,1),image0(0,1),1e-6);
  TEST_NEAR("Next element",reduced_x(1,1),image0(2,1),1e-6);
  int L = (nx-1)/2;
  TEST_NEAR("Last element",reduced_x(L,1),image0(2*L,1),1e-6);


  vil2_image_view<float> test2;
  test2.resize(nx,3);
  test2.fill(22.2f);
  vil2_algo_gauss_reduce(test2.top_left_ptr(),test2.istep(),test2.jstep(),
                         image0.top_left_ptr(),image0.ni(),image0.nj(),
                         image0.istep(),image0.jstep());
  TEST_NEAR("No overrun",test2(L+1,1),22.2f,1e-6);
}

void test_algo_gauss_reduce_121_byte(int nx, int ny)
{
  vcl_cout << "**********************************************\n";
  vcl_cout << " Testing vil2_algo_gauss_reduce_121 (byte)(nx="<<nx<<")\n";
  vcl_cout << "**********************************************\n";

  vil2_image_view<vil_byte> image0;
  image0.resize(nx,ny);
  vil2_image_view<vil_byte> reduced_x;
  reduced_x.resize((nx+1)/2,(ny+1)/2);

  for (int y=0;y<image0.nj();++y)
    for (int x=0;x<image0.ni();++x)
      image0(x,y) = x+y*10;

  vil2_algo_gauss_reduce_121(reduced_x.top_left_ptr(),reduced_x.istep(),reduced_x.jstep(),
                             image0.top_left_ptr(),image0.ni(),image0.nj(),
                             image0.istep(),image0.jstep());

  vcl_cout<<"Original: "; vil2_print_all(vcl_cout,image0); vcl_cout<<vcl_endl;
  vcl_cout<<"reduced_x : "; vil2_print_all(vcl_cout,reduced_x); vcl_cout<<vcl_endl;

  TEST("First element",reduced_x(0,1),image0(0,2));
  TEST("Next element",reduced_x(1,1),image0(2,2));
  int Lx = (nx+1)/2;
  int Ly = (ny+1)/2;
  TEST("Last element in x",reduced_x(Lx-1,1),image0(2*(Lx-1),2));
  TEST("Last element in y",reduced_x(1,Ly-1),image0(2,2*(Ly-1)));

  vil2_image_view<vil_byte> test2;
  test2.resize(nx,ny);
  test2.fill(222);
  vil2_algo_gauss_reduce_121(test2.top_left_ptr(),test2.istep(),test2.jstep(),
                             image0.top_left_ptr(),image0.ni(),image0.nj(),
                             image0.istep(),image0.jstep());
  TEST("No overrun in x",test2(Lx,1), 222);
  TEST("No overrun in y",test2(1,Ly), 222);

  image0.fill(17);
  vil2_algo_gauss_reduce_121(test2.top_left_ptr(),test2.istep(),test2.jstep(),
                             image0.top_left_ptr(),image0.ni(),image0.nj(),
                             image0.istep(),image0.jstep());
  TEST("Smoothing correct",test2(1,1), 17);
  vcl_cout<<"Value at (1,1):"<<int(test2(1,1))<<vcl_endl;
}

void test_algo_gauss_reduce_121_float(int nx, int ny)
{
  vcl_cout << "***********************************************\n";
  vcl_cout << " Testing vil2_algo_gauss_reduce_121 (float)(nx="<<nx<<")\n";
  vcl_cout << "***********************************************\n";

  vil2_image_view<float> image0;
  image0.resize(nx,ny);
  vil2_image_view<float> reduced_x;
  reduced_x.resize((nx+1)/2,(ny+1)/2);

  for (int y=0;y<image0.nj();++y)
    for (int x=0;x<image0.ni();++x)
      image0(x,y) = 0.1f*x+y;

  vil2_algo_gauss_reduce_121(reduced_x.top_left_ptr(),reduced_x.istep(),reduced_x.jstep(),
                             image0.top_left_ptr(),image0.ni(),image0.nj(),
                             image0.istep(),image0.jstep());

  vcl_cout<<"Original: "; vil2_print_all(vcl_cout,image0); vcl_cout<<vcl_endl;
  vcl_cout<<"reduced_x : "; vil2_print_all(vcl_cout,reduced_x); vcl_cout<<vcl_endl;


  TEST_NEAR("First element",reduced_x(0,1),image0(0,2),1e-6);
  TEST_NEAR("Next element",reduced_x(1,1),image0(2,2),1e-6);
  int Lx = (nx+1)/2;
  int Ly = (ny+1)/2;
  TEST_NEAR("Last element in x",reduced_x(Lx-1,1),image0(2*(Lx-1),2),1e-6);
  TEST_NEAR("Last element in y",reduced_x(1,Ly-1),image0(2,2*(Ly-1)),1e-6);

  vil2_image_view<float> test2;
  test2.resize(nx,ny);
  test2.fill(22.2f);
  vil2_algo_gauss_reduce_121(test2.top_left_ptr(),test2.istep(),test2.jstep(),
                             image0.top_left_ptr(),image0.ni(),image0.nj(),
                             image0.istep(),image0.jstep());
  TEST_NEAR("No overrun in x",test2(Lx,1),22.2f,1e-6);
  TEST_NEAR("No overrun in y",test2(1,Ly),22.2f,1e-6);

  image0.fill(1.7f);
  vil2_algo_gauss_reduce_121(test2.top_left_ptr(),test2.istep(),test2.jstep(),
                             image0.top_left_ptr(),image0.ni(),image0.nj(),
                             image0.istep(),image0.jstep());
  TEST_NEAR("Smoothing correct",test2(1,1),1.7f,1e-6);
  vcl_cout<<"Value at (1,1):"<<float(test2(1,1))<<vcl_endl;
}

MAIN( test_algo_gauss_reduce )
{
  START( "vil2_algo_gauss_reduce" );

  test_algo_gauss_reduce_byte(7);
  test_algo_gauss_reduce_byte(6);
  test_algo_gauss_reduce_float(7);
  test_algo_gauss_reduce_float(6);

  test_algo_gauss_reduce_121_byte(6,6);
  test_algo_gauss_reduce_121_byte(7,7);
  test_algo_gauss_reduce_121_float(6,6);
  test_algo_gauss_reduce_121_float(7,7);

  SUMMARY();
}

