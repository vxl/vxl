// This is mul/vil2/tests/test_algo_exp_grad_filter_1d.cxx
#include <testlib/testlib_test.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte
#include <vil2/algo/vil2_exp_grad_filter_1d.h>
#include <vil2/vil2_print.h>

void test_algo_exp_grad_filter_1d_byte_float()
{
  vcl_cout << "********************************************\n"
           << " Testing vil2_algo_exp_grad_filter_1d byte-float\n"
           << "********************************************\n";

  int n = 100;
  vcl_vector<vxl_byte> src(n);
  for (int i=0;i<n;++i) src[i]=0;
  src[50] = 100;

  double k = 0.25;
  vcl_vector<float> dest_block(n+2);
  float *dest = &dest_block[1];
  dest[-1]=9876; dest[n]=9876;  // Marks to check for over-runs
  vil2_exp_grad_filter_1d(&src[0],1,&dest[0],1,n,k,float(0));

  double half_sum = k/(1-k);
  TEST_NEAR("Central value",dest[50],0,1e-6);
  TEST_NEAR("Left value" ,dest[49], 100*k/half_sum,1e-4);
  TEST_NEAR("Right value",dest[51],-100*k/half_sum,1e-4);
  TEST_NEAR("Neighbours",dest[54]*k,dest[55],1e-6);
  TEST_NEAR("Neighbours",dest[47]*k,dest[46],1e-6);
  TEST_NEAR("No start over-run",dest[-1],9876,1e-6);
  TEST_NEAR("No end over-run",dest[n],9876,1e-6);

  double sum = 0;
  for (int i=0;i<n;++i) sum+=dest[i];
  TEST_NEAR("Sum zero",sum,0,1e-6);

  for (int i=0;i<10;++i) src[i]=i;
  vil2_exp_grad_filter_1d(&src[0],1,&dest[0],1,10,k,float(0));
  vcl_cout<<"Applying to 0 1 2 3 .."<<vcl_endl;
  for (int i=0;i<10;++i) vcl_cout<<" "<<dest[i];
  vcl_cout<<vcl_endl;

  // Test application to whole images
  vil2_image_view<vxl_byte> src_im(10,10);
  vil2_image_view<float> dest_im;
  src_im.fill(10);
  for (unsigned j=0;j<10;++j)
    for (unsigned i=5;i<10;++i) src_im(i,j)=20;
  vil2_exp_grad_filter_1d(src_im,dest_im,0.25,double());

  TEST("Width",dest_im.ni(),src_im.ni());
  TEST("Height",dest_im.nj(),src_im.nj());
  TEST_NEAR("dest_im(5,5)",dest_im(5,5),10,1e-2);

  vil2_print_all(vcl_cout,dest_im);
}


MAIN( test_algo_exp_grad_filter_1d )
{
  START( "vil2_algo_exp_grad_filter_1d" );

  test_algo_exp_grad_filter_1d_byte_float();

  SUMMARY();
}
