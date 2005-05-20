// This is mul/vil3d/tests/test_math.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vcl_iomanip.h>
#include <vil3d/vil3d_math.h>


static void test_image_view_maths_float()
{
  vcl_cout << "********************************\n"
           << " Testing vil3d_image_view_maths\n"
           << "********************************\n";

  unsigned ni=5, nj=6, nk = 7;

  vil3d_image_view<float> imA(ni,nj,nk);

  double sum0 = 0,sum_sq0=0;
  for (unsigned int k=0;k<imA.nk();++k)
   for (unsigned int j=0;j<imA.nj();++j)
    for (unsigned int i=0;i<imA.ni();++i)
    {
      imA(i,j,k) = 1.f+i+j*ni+3*k; sum0+=imA(i,j,k); sum_sq0+= imA(i,j,k)*imA(i,j,k);
    }

  double sum;
  vil3d_math_sum(sum,imA,0);
  TEST_NEAR("Sum",sum,sum0,1e-8);

  double mean;
  vil3d_math_mean(mean,imA,0);
  TEST_NEAR("mean",mean,sum0/(ni*nj*nk),1e-8);

  double sum_sq;
  vil3d_math_sum_squares(sum,sum_sq,imA,0);
  TEST_NEAR("Sum",sum,sum0,1e-8);
  TEST_NEAR("Sum of squares",sum_sq,sum_sq0,1e-8);
}


static void test_math_value_range()
{
  vcl_cout << "********************************\n"
           << " Testing vil3d_math_value_range \n"
           << "********************************\n";

  // Create a test image with values 1...1000
  unsigned ni=10, nj=10, nk=10;
  vil3d_image_view<int> img(ni, nj, nk);
  for (unsigned i=0; i<ni; ++i)
  {
    for (unsigned j=0; j<nj; ++j)
    {
      for (unsigned k=0; k<nk; ++k)
      {
        int val = k*nj*ni + j*ni + i +1; // NB Data values not already sorted!
        img(i,j,k) = val;
      }
    }
  }

  // Check the min/max values
  int min=2000; 
  int max=-2000;
  vil3d_math_value_range(img, min, max);
  TEST("vil3d_math_value_range(): min", min==1, true);
  TEST("vil3d_math_value_range(): max", max==1000, true);

  int val=2000;

  // Test a likely pair of percentiles
  vil3d_math_value_range_percentile(img, 0.05, val);
  TEST("vil3d_math_value_range_percentile():  5  %", val==50, true);
  vil3d_math_value_range_percentile(img, 0.95, val);
  TEST("vil3d_math_value_range_percentile(): 95  %", val==950, true);  

  // Test a likely pair of percentiles
  vil3d_math_value_range_percentile(img, 0.10, val);
  TEST("vil3d_math_value_range_percentile(): 10  %", val==100, true);
  vil3d_math_value_range_percentile(img, 0.90, val);
  TEST("vil3d_math_value_range_percentile(): 90  %", val==900, true);  
  
  // Test an unlikely pair of percentiles
  vil3d_math_value_range_percentile(img, 0.311, val);
  TEST("vil3d_math_value_range_percentile(): 31.1 %", val==311, true);
  vil3d_math_value_range_percentile(img, 0.737, val);
  TEST("vil3d_math_value_range_percentile(): 73.7 %", val==737, true); 
  
  // Test several percentiles at once
  int nfrac = 9;
  vcl_vector<double> fraction(nfrac);
  vcl_vector<double> true_value(nfrac);
  fraction[0] = 0.000;  true_value[0] =    1;
  fraction[1] = 0.050;  true_value[1] =   50;
  fraction[2] = 0.100;  true_value[2] =  100;
  fraction[3] = 0.311;  true_value[3] =  311;
  fraction[4] = 0.500;  true_value[4] =  500;
  fraction[5] = 0.737;  true_value[5] =  737;
  fraction[6] = 0.900;  true_value[6] =  900;
  fraction[7] = 0.950;  true_value[7] =  950;
  fraction[8] = 1.000;  true_value[8] = 1000;
  vcl_vector<int> value;
  vil3d_math_value_range_percentiles(img, fraction, value);
  bool all_correct = true;
  for (unsigned f=0; f<nfrac; ++f)
  {
    if (value[f] != true_value[f]) 
    {
      all_correct = false;
    }
  }  
  TEST("vil3d_math_value_range_percentiles(): all correct", all_correct, true);
}


static void test_math()
{
  test_image_view_maths_float();
  test_math_value_range();
}

TESTMAIN(test_math);
