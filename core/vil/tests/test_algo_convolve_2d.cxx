// This is mul/vil2/tests/test_algo_convolve_2d.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte
#include <vil2/algo/vil2_algo_convolve_2d.h>

void test_algo_convolve_2d_byte()
{
  vcl_cout << "*******************************\n"
           << " Testing vil2_algo_convolve_2d\n"
           << "*******************************\n";

  int n=10, m=5;

  vil2_image_view<vxl_byte> src_im(n,n,1);

  for (unsigned int j=0;j<src_im.nj();++j)
    for (unsigned int i=0;i<src_im.ni();++i)
      src_im(i,j) = 1+i+j*n;

      // Create kernel1 (all ones in a single row)
  vil2_image_view<float> kernel1(m,1,1);
  for (unsigned int j=0;j<kernel1.nj();++j)
    for (unsigned int i=0;i<kernel1.ni();++i)
      kernel1(i,j) = 1;

  vil2_image_view<double> dest_im;

  vil2_algo_convolve_2d(src_im,dest_im,kernel1,double());

  vcl_cout<<"Kernel - single row of ones\n";
  TEST("Destination size",dest_im.ni(),1+n-m);

  TEST_NEAR("dest_im(0,0)",dest_im(0,0),0.5*m*(m+1),1e-6);
  TEST_NEAR("dest_im(1,0)",dest_im(1,0),0.5*m*(m+1)+m,1e-6);
  TEST_NEAR("dest_im(0,1)",dest_im(0,1),0.5*m*(m+1)+n*m,1e-6);

  vcl_cout<<"Kernel - three rows of ones\n";
  vil2_image_view<float> kernel2(m,3,1);
  for (unsigned int j=0;j<kernel2.nj();++j)
    for (unsigned int i=0;i<kernel2.ni();++i)
      kernel2(i,j) = 1;

  vil2_algo_convolve_2d(src_im,dest_im,kernel2,double());

  TEST("Destination size",dest_im.ni(),1+n-m);
  TEST("Destination size",dest_im.nj(),1+n-3);

  TEST_NEAR("dest_im(0,0)",dest_im(0,0),0.5*m*(m+1)*3+30*m,1e-6);
  TEST_NEAR("dest_im(1,0)",dest_im(1,0),0.5*m*(m+1)*3+33*m,1e-6);
  TEST_NEAR("dest_im(0,1)",dest_im(0,1),0.5*m*(m+1)*3+60*m,1e-6);

  vcl_cout<<"Kernel - three rows of 1s, 2s 3s\n";
  vil2_image_view<float> kernel3(m,3,1);
  for (unsigned int j=0;j<kernel3.nj();++j)
    for (unsigned int i=0;i<kernel3.ni();++i)
      kernel3(i,j) = 1+j;

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
