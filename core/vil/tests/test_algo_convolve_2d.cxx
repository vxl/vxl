// This is mul/vil2/tests/test_algo_convolve_2d.cxx
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <testlib/testlib_test.h>
#include <vil2/algo/vil2_algo_convolve_2d.h>

void test_algo_convolve_2d_byte()
{
  vcl_cout << "*****************************\n";
  vcl_cout << " Testing vil2_algo_convolve_2d\n";
  vcl_cout << "*****************************\n";

  int n=10, m=5;

  vil2_image_view<vxl_byte> src_im(n,n,1);

  for (int y=0;y<src_im.nj();++y)
    for (int x=0;x<src_im.ni();++x)
      src_im(x,y) = 1+x+y*10;

			// Create kernel1 (all ones in a single row)
  vil2_image_view<float> kernel1(m,1,1);
  for (int y=0;y<kernel1.nj();++y)
    for (int x=0;x<kernel1.ni();++x)
      kernel1(x,y) = 1;

  vil2_image_view<double> dest_im;

	vil2_algo_convolve_2d(src_im,dest_im,kernel1,double());

	vcl_cout<<"Kernel - single row of ones"<<vcl_endl;
	TEST("Destination size",dest_im.ni(),1+n-m);

	TEST_NEAR("dest_im(0,0)",dest_im(0,0),0.5*m*(m+1),1e-6);
	TEST_NEAR("dest_im(1,0)",dest_im(1,0),0.5*m*(m+1)+m,1e-6);
	TEST_NEAR("dest_im(0,1)",dest_im(0,1),0.5*m*(m+1)+10*m,1e-6);

	vcl_cout<<"Kernel - three rows of ones"<<vcl_endl;
  vil2_image_view<float> kernel2(m,3,1);
  for (int y=0;y<kernel2.nj();++y)
    for (int x=0;x<kernel2.ni();++x)
      kernel2(x,y) = 1;

	vil2_algo_convolve_2d(src_im,dest_im,kernel2,double());

	TEST("Destination size",dest_im.ni(),1+n-m);
	TEST("Destination size",dest_im.nj(),1+n-3);

	TEST_NEAR("dest_im(0,0)",dest_im(0,0),0.5*m*(m+1)*3+30*m,1e-6);
	TEST_NEAR("dest_im(1,0)",dest_im(1,0),0.5*m*(m+1)*3+33*m,1e-6);
	TEST_NEAR("dest_im(0,1)",dest_im(0,1),0.5*m*(m+1)*3+60*m,1e-6);

	vcl_cout<<"Kernel - three rows of 1s, 2s 3s"<<vcl_endl;
  vil2_image_view<float> kernel3(m,3,1);
  for (int y=0;y<kernel3.nj();++y)
    for (int x=0;x<kernel3.ni();++x)
      kernel3(x,y) = 1+y;

	vil2_algo_convolve_2d(src_im,dest_im,kernel3,double());

	TEST("Destination size",dest_im.ni(),1+n-m);
	TEST("Destination size",dest_im.nj(),1+n-3);

	TEST_NEAR("dest_im(0,0)",dest_im(0,0),0.5*m*(m+1)*6+80*m,1e-6);
	TEST_NEAR("dest_im(1,0)",dest_im(1,0),0.5*m*(m+1)*6+86*m,1e-6);
	TEST_NEAR("dest_im(0,1)",dest_im(0,1),0.5*m*(m+1)*6+140*m,1e-6);

}

MAIN( test_algo_convolve_2d )
{
  START( "vil2_algo_convolve_2d" );

  test_algo_convolve_2d_byte();

  SUMMARY();
}

