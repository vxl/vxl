// This is core/vil/algo/tests/test_algo_exp_filter_1d.cxx
#include <testlib/testlib_test.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte
#include <vil/algo/vil_exp_filter_1d.h>

static void test_algo_exp_filter_1d_byte_float()
{
  vcl_cout << "*******************************************\n"
           << " Testing vil_algo_exp_filter_1d byte-float\n"
           << "*******************************************\n";

  const int n = 100;
  vcl_vector<vxl_byte> src(n);
  for (int i=0;i<n;++i) src[i]=0;
  src[50] = 100;

  double k = 0.25;
  vcl_vector<float> dest(n);
  vil_exp_filter_1d(&src[0],1,&dest[0],1,n,float(k));

  double r0 = 100*(1-k)/(1+k);
  TEST_NEAR("Central value",dest[50],r0,1e-4);
  double r1 = k*r0;
  TEST_NEAR("Left value" ,dest[49],r1,1e-6);
  TEST_NEAR("Right value",dest[51],r1,1e-6);
  TEST_NEAR("Neighbours",dest[54]*k,dest[55],1e-6);
  TEST_NEAR("Neighbours",dest[47]*k,dest[46],1e-6);

  double sum = 0;
  for (int i=0;i<n;++i) sum+=dest[i];
  TEST_NEAR("Sum unchanged",sum,100,1e-4);

  src[55]=100;
  vil_exp_filter_1d(&src[0],1,&dest[0],1,n,float(k));
  sum = 0;
  for (int i=0;i<n;++i) sum+=dest[i];
  TEST_NEAR("Sum unchanged (more complex data)", sum, 200, 1e-4);

  for (int i=30;i<=70;++i) src[i]=100;
  vil_exp_filter_1d(&src[0],1,&dest[0],1,n,float(k));
  for (int i=48;i<=52;++i)
  {
    TEST_NEAR("Flat regions remain flat",dest[i],100,1e-4);
  }

  // Test application to whole images
  vil_image_view<vxl_byte> src_im(10,10);
  vil_image_view<float> dest_im;
  for (unsigned j=0;j<10;++j)
    for (unsigned i=0;i<10;++i)
      src_im(i,j) = i+10*j;
  vil_exp_filter_i(src_im,dest_im,double(0.1));

  TEST("Width", dest_im.ni(), src_im.ni());
  TEST("Height",dest_im.nj(), src_im.nj());
  TEST_NEAR("dest_im(5,5)", dest_im(5,5), 55, 1e-2);

  vil_exp_filter_j(src_im,dest_im,double(0.1));

  TEST("Width", dest_im.ni(), src_im.ni());
  TEST("Height",dest_im.nj(), src_im.nj());
  TEST_NEAR("dest_im(5,5)", dest_im(5,5), 55, 1e-2);
}

static void test_algo_exp_filter_1d_float_float()
{
  vcl_cout << "********************************************\n"
           << " Testing vil_algo_exp_filter_1d float-float\n"
           << "********************************************\n";

  int n = 100;
  vcl_vector<float> src(n);
  for (int i=0;i<n;++i) src[i]=0;
  src[50] = 100;

  double k = 0.25;
  vcl_vector<float> dest(n);
  vil_exp_filter_1d(&src[0],1,&dest[0],1,n,float(k));

  double r0 = 100*(1-k)/(1+k);
  TEST_NEAR("Central value",dest[50],r0,1e-4);
  double r1 = k*r0;
  TEST_NEAR("Left value" ,dest[49],r1,1e-6);
  TEST_NEAR("Right value",dest[51],r1,1e-6);
  TEST_NEAR("Neighbours",dest[54]*k,dest[55],1e-6);
  TEST_NEAR("Neighbours",dest[47]*k,dest[46],1e-6);

  double sum = 0;
  for (int i=0;i<n;++i) sum+=dest[i];
  TEST_NEAR("Sum unchanged",sum,100,1e-4);
}

static void test_algo_exp_filter_1d()
{
  test_algo_exp_filter_1d_byte_float();
  test_algo_exp_filter_1d_float_float();
}

TESTMAIN(test_algo_exp_filter_1d);
