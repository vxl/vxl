// This is mul/vil2/tests/test_algo_convolve_1d.cxx
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vil2/algo/vil2_algo_convolve_1d.h>
#include <vil/vil_byte.h>
#include <testlib/testlib_test.h>

void test_algo_convolve_1d_byte()
{
  vcl_cout << "*****************************\n";
  vcl_cout << " Testing vil2_algo_convolve_1d\n";
  vcl_cout << "*****************************\n";


  int n = 10;
  vcl_vector<double> src(n),dest(n),kernel(3);
  for (int i=0;i<n;++i) src[i]=i+1;
  for (int i=0;i<3;++i) kernel[i]=(i+1);

  double ac;  // Indicates accumulator type
  vil2_algo_convolve_1d(&dest[0],1, &src[0],n,1,
                        &kernel[1],-1,1,ac,
						vil2_convolve_no_extend,vil2_convolve_no_extend);
  TEST_NEAR("First full value",dest[1],14.0,1e-6);
  TEST_NEAR("Last full value",dest[n-2],6*n-4,1e-6);

  vcl_cout<<"Testing vil2_convolve_no_extend end type"<<vcl_endl;
  vil2_algo_convolve_1d(&dest[0],1, &src[0],n,1,
                        &kernel[1],-1,1,ac,
						vil2_convolve_no_extend,vil2_convolve_no_extend);

  TEST_NEAR("Start",dest[0],0,1e-6);
  TEST_NEAR("First full value",dest[1],14.0,1e-6);
  TEST_NEAR("End",dest[n-1],0,1e-6);

  for (int i=0;i<n;++i) vcl_cout<<dest[i]<<" ";
  vcl_cout<<vcl_endl;

  vcl_cout<<"Testing vil2_convolve_zero_extend end type"<<vcl_endl;
  vil2_algo_convolve_1d(&dest[0],1, &src[0],n,1,
                        &kernel[1],-1,1,ac,
						vil2_convolve_zero_extend,vil2_convolve_zero_extend);

  TEST_NEAR("Start",dest[0],8,1e-6);
  TEST_NEAR("First full value",dest[1],14.0,1e-6);
  TEST_NEAR("End",dest[n-1],3*n-1,1e-6);

  for (int i=0;i<n;++i) vcl_cout<<dest[i]<<" ";
  vcl_cout<<vcl_endl;

	// *** Check for over-runs ****

}

MAIN( test_algo_convolve_1d )
{
  START( "vil2_algo_convolve_1d" );

  test_algo_convolve_1d_byte();

  SUMMARY();
}

