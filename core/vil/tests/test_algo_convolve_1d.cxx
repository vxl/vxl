// This is mul/vil2/tests/test_algo_convolve_1d.cxx
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vil2/algo/vil2_algo_convolve_1d.h>
#include <testlib/testlib_test.h>

void test_algo_convolve_1d_double()
{
  vcl_cout << "*****************************\n";
  vcl_cout << " Testing vil2_algo_convolve_1d\n";
  vcl_cout << "*****************************\n";


  int n = 10;
  vcl_vector<double> src(n),dest(n+2),kernel(3);
  for (int i=0;i<n;++i) src[i]=i+1;
  for (int i=0;i<3;++i) kernel[i]=(i+1);

  // Note: In the following dest[1]..dest[n] should be valid.
  // dest[0] and dest[n+1] should be untouched.
  // They are included to test for over-runs.

  vil2_algo_convolve_1d(&src[0],n,1, &dest[1],1,
                        &kernel[1],-1,1,
                        double(), // indicates accumulator type
                        vil2_convolve_no_extend,vil2_convolve_no_extend);
  TEST_NEAR("First full value",dest[2],14.0,1e-6);
  TEST_NEAR("Last full value",dest[n-1],6*n-4,1e-6);

  vcl_cout<<"Testing vil2_convolve_ignore_edge end type"<<vcl_endl;
  for (int i=0;i<n+2;++i) dest[i]=999;

  vil2_algo_convolve_1d(&src[0],n,1, &dest[1],1,
                        &kernel[1],-1,1,
                        double(), // indicates accumulator type
                        vil2_convolve_ignore_edge,vil2_convolve_ignore_edge);

  TEST_NEAR("Start",dest[1],999,1e-6);
  TEST_NEAR("First full value",dest[2],14.0,1e-6);
  TEST_NEAR("Last full value",dest[n-1],6*n-4.0,1e-6);
  TEST_NEAR("End",dest[n],999,1e-6);
  TEST_NEAR("No overrun start",dest[0],999,1e-6);
  TEST_NEAR("No overrun end",dest[n+1],999,1e-6);

  vcl_cout<<"Testing vil2_convolve_no_extend end type"<<vcl_endl;
  for (int i=0;i<n+2;++i) dest[i]=999;

  vil2_algo_convolve_1d(&src[0],n,1, &dest[1],1,
                        &kernel[1],-1,1,
                        double(), // indicates accumulator type
                        vil2_convolve_no_extend,vil2_convolve_no_extend);

  TEST_NEAR("Start",dest[1],0,1e-6);
  TEST_NEAR("First full value",dest[2],14.0,1e-6);
  TEST_NEAR("Last full value",dest[n-1],6*n-4.0,1e-6);
  TEST_NEAR("End",dest[n],0,1e-6);
  TEST_NEAR("No overrun start",dest[0],999,1e-6);
  TEST_NEAR("No overrun end",dest[n+1],999,1e-6);


  vcl_cout<<"Testing vil2_convolve_zero_extend end type"<<vcl_endl;
  for (int i=0;i<n+2;++i) dest[i]=999;
  vil2_algo_convolve_1d(&src[0],n,1, &dest[1],1,
                        &kernel[1],-1,1,
                        double(), // indicates accumulator type
                        vil2_convolve_zero_extend,vil2_convolve_zero_extend);

  TEST_NEAR("Start",dest[1],8,1e-6);
  TEST_NEAR("First full value",dest[2],14.0,1e-6);
  TEST_NEAR("Last full value",dest[n-1],6*n-4.0,1e-6);
  TEST_NEAR("End",dest[n],3*n-1,1e-6);
  TEST_NEAR("No overrun start",dest[0],999,1e-6);
  TEST_NEAR("No overrun end",dest[n+1],999,1e-6);

  vcl_cout<<"Testing vil2_convolve_constant_extend end type"<<vcl_endl;
  for (int i=0;i<n+2;++i) dest[i]=999;
  vil2_algo_convolve_1d(&src[0],n,1, &dest[1],1,
                        &kernel[1],-1,1,
                        double(), // indicates accumulator type
                        vil2_convolve_constant_extend,vil2_convolve_constant_extend);

  TEST_NEAR("Start",dest[1],9,1e-6);
  TEST_NEAR("First full value",dest[2],14.0,1e-6);
  TEST_NEAR("Last full value",dest[n-1],6*n-4.0,1e-6);
  TEST_NEAR("End",dest[n],6*n-1,1e-6);
  TEST_NEAR("No overrun start",dest[0],999,1e-6);
  TEST_NEAR("No overrun end",dest[n+1],999,1e-6);

  vcl_cout<<"Testing vil2_convolve_reflect_extend end type"<<vcl_endl;
  for (int i=0;i<n+2;++i) dest[i]=999;
  vil2_algo_convolve_1d(&src[0],n,1, &dest[1],1,
                        &kernel[1],-1,1,
                        double(), // indicates accumulator type
                        vil2_convolve_reflect_extend,vil2_convolve_reflect_extend);

  TEST_NEAR("Start",dest[1],n+8,1e-6);
  TEST_NEAR("First full value",dest[2],14.0,1e-6);
  TEST_NEAR("Last full value",dest[n-1],6*n-4.0,1e-6);
  TEST_NEAR("End",dest[n],3*n+2,1e-6);
  TEST_NEAR("No overrun start",dest[0],999,1e-6);
  TEST_NEAR("No overrun end",dest[n+1],999,1e-6);

  vcl_cout<<"Testing vil2_convolve_trim end type"<<vcl_endl;
  for (int i=0;i<n+2;++i) dest[i]=999;
  vil2_algo_convolve_1d(&src[0],n,1, &dest[1],1,
                        &kernel[1],-1,1,
                        double(), // indicates accumulator type
                        vil2_convolve_trim,vil2_convolve_trim);

  TEST_NEAR("Start",dest[1],48.0/5.0,1e-6);
  TEST_NEAR("First full value",dest[2],14.0,1e-6);
  TEST_NEAR("Last full value",dest[n-1],6*n-4.0,1e-6);
  TEST_NEAR("End",dest[n],6*n-2.0,1e-6);
  TEST_NEAR("No overrun start",dest[0],999,1e-6);
  TEST_NEAR("No overrun end",dest[n+1],999,1e-6);
}

MAIN( test_algo_convolve_1d )
{
  START( "vil2_algo_convolve_1d" );

  test_algo_convolve_1d_double();

  SUMMARY();
}
