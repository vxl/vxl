// This is mul/mil/tests/test_algo_exp_filter_1d.cxx
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <mil/algo/mil_algo_exp_filter_1d.h>
#include <vxl_config.h>
#include <testlib/testlib_test.h>

void test_algo_exp_filter_1d_byte_float()
{
  vcl_cout << "*******************************************\n"
           << " Testing mil_algo_exp_filter_1d byte-float\n"
           << "*******************************************\n";

  int n = 100;
  vcl_vector<vxl_byte> src(n);
  for (int i=0;i<n;++i) src[i]=0;
  src[50] = 100;

  double k = 0.25;
  vcl_vector<float> dest(n);
  mil_algo_exp_filter_1d(&dest[0],1,&src[0],1,n,k);

  double r0 = 100*(1-k)/(1+k);
  TEST_NEAR("Central value",dest[50],r0,1e-6);
  double r1 = k*r0;
  TEST_NEAR("Left value" ,dest[49],r1,1e-6);
  TEST_NEAR("Right value",dest[51],r1,1e-6);
  TEST_NEAR("Neighbours",dest[54]*k,dest[55],1e-6);
  TEST_NEAR("Neighbours",dest[47]*k,dest[46],1e-6);

  double sum = 0;
  for (int i=0;i<n;++i) sum+=dest[i];
  TEST_NEAR("Sum unchanged",sum,100,1e-6);

  src[55]=100;
  mil_algo_exp_filter_1d(&dest[0],1,&src[0],1,n,k);
  sum = 0;
  for (int i=0;i<n;++i) sum+=dest[i];
  TEST_NEAR("Sum unchanged (more complex data)",sum,200,1e-6);

  for (int i=30;i<=70;++i) src[i]=100;
  mil_algo_exp_filter_1d(&dest[0],1,&src[0],1,n,k);
  for (int i=48;i<=52;++i)
    TEST_NEAR("Flat regions remain flat",dest[i],100,1e-4);
}

void test_algo_exp_filter_1d_float_float()
{
  vcl_cout << "********************************************\n"
           << " Testing mil_algo_exp_filter_1d float-float\n"
           << "********************************************\n";

  int n = 100;
  vcl_vector<float> src(n);
  for (int i=0;i<n;++i) src[i]=0;
  src[50] = 100;

  double k = 0.25;
  vcl_vector<float> dest(n);
  mil_algo_exp_filter_1d(&dest[0],1,&src[0],1,n,k);

  double r0 = 100*(1-k)/(1+k);
  TEST_NEAR("Central value",dest[50],r0,1e-6);
  double r1 = k*r0;
  TEST_NEAR("Left value" ,dest[49],r1,1e-6);
  TEST_NEAR("Right value",dest[51],r1,1e-6);
  TEST_NEAR("Neighbours",dest[54]*k,dest[55],1e-6);
  TEST_NEAR("Neighbours",dest[47]*k,dest[46],1e-6);

  double sum = 0;
  for (int i=0;i<n;++i) sum+=dest[i];
  TEST_NEAR("Sum unchanged",sum,100,1e-6);
}

void test_algo_exp_filter_1d()
{
  test_algo_exp_filter_1d_byte_float();
  test_algo_exp_filter_1d_float_float();
}


TESTLIB_DEFINE_MAIN(test_algo_exp_filter_1d);
