#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_utility.h>
#include <vcl_vector.h>

#include <vnl/vnl_test.h>
#include <mil/algo/mil_algo_exp_filter_1d.h>
#include <vil/vil_byte.h>
#include <vcl_cmath.h> // for fabs()

void test_algo_exp_filter_1d_byte_float()
{
  vcl_cout << "***********************" << vcl_endl;
  vcl_cout << " Testing mil_algo_exp_filter_1d byte-float" << vcl_endl;
  vcl_cout << "***********************" << vcl_endl;

  int n = 100;
  vcl_vector<vil_byte> src(n);
  for (int i=0;i<n;++i) src[i]=0;
  src[50] = 100;

  double k = 0.25;
  vcl_vector<float> dest(n);
  mil_algo_exp_filter_1d(&dest[0],1,&src[0],1,n,k);

  double r0 = 100*(1-k)/(1+k);
  TEST("Central value",vcl_fabs(dest[50]-r0)<1e-6,true);
  double r1 = k*r0;
  TEST("Left value" ,vcl_fabs(dest[49]-r1)<1e-6,true);
  TEST("Right value",vcl_fabs(dest[51]-r1)<1e-6,true);
  TEST("Neighbours",vcl_fabs(dest[54]*k-dest[55])<1e-6,true);
  TEST("Neighbours",vcl_fabs(dest[47]*k-dest[46])<1e-6,true);

  double sum = 0;
  for (int i=0;i<n;++i) sum+=dest[i];
  TEST("Sum unchanged",vcl_fabs(sum-100)<1e-6,true);

  src[55]=100;
  mil_algo_exp_filter_1d(&dest[0],1,&src[0],1,n,k);
  sum = 0;
  for (int i=0;i<n;++i) sum+=dest[i];
  TEST("Sum unchanged (more complex data)",vcl_fabs(sum-200)<1e-6,true);

  for (int i=30;i<=70;++i) src[i]=100;
  mil_algo_exp_filter_1d(&dest[0],1,&src[0],1,n,k);
  for (int i=48;i<=52;++i)
    TEST("Flat regions remain flat",vcl_fabs(dest[i]-100)<1e-4,true);
}

void test_algo_exp_filter_1d_float_float()
{
  vcl_cout << "***********************" << vcl_endl;
  vcl_cout << " Testing mil_algo_exp_filter_1d float-float" << vcl_endl;
  vcl_cout << "***********************" << vcl_endl;

  int n = 100;
  vcl_vector<float> src(n);
  for (int i=0;i<n;++i) src[i]=0;
  src[50] = 100;

  double k = 0.25;
  vcl_vector<float> dest(n);
  mil_algo_exp_filter_1d(&dest[0],1,&src[0],1,n,k);

  double r0 = 100*(1-k)/(1+k);
  TEST("Central value",vcl_fabs(dest[50]-r0)<1e-6,true);
  double r1 = k*r0;
  TEST("Left value" ,vcl_fabs(dest[49]-r1)<1e-6,true);
  TEST("Right value",vcl_fabs(dest[51]-r1)<1e-6,true);
  TEST("Neighbours",vcl_fabs(dest[54]*k-dest[55])<1e-6,true);
  TEST("Neighbours",vcl_fabs(dest[47]*k-dest[46])<1e-6,true);

  double sum = 0;
  for (int i=0;i<n;++i) sum+=dest[i];
  TEST("Sum unchanged",vcl_fabs(sum-100)<1e-6,true);
}

void test_algo_exp_filter_1d()
{
  test_algo_exp_filter_1d_byte_float();
  test_algo_exp_filter_1d_float_float();
}


TESTMAIN(test_algo_exp_filter_1d);
