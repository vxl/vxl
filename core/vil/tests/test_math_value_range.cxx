// This is core/vil/tests/test_math_value_range.cxx

#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vil/vil_math.h>


static void test_math_value_range()
{
  vcl_cout << "********************************\n"
           << " Testing vil_math_value_range \n"
           << "********************************\n";

  // Create a test image with values 1...100
  unsigned ni=10, nj=10;
  vil_image_view<int> img(ni, nj);
  for (unsigned i=0; i<ni; ++i)
  {
    for (unsigned j=0; j<nj; ++j)
    {
      int val = j*ni + i +1; // NB Data values not already sorted!
      img(i,j) = val;
      vcl_cout << val << "\n";
    }
  }

  // Check the min/max values
  int min=1e6; 
  int max=-1e6;
  vil_math_value_range(img, min, max);
  TEST("vil_math_value_range(): min", min==1, true);
  TEST("vil_math_value_range(): max", max==100, true);

  int val=1e6;

  // Test a likely pair of percentiles
  vil_math_value_range_percentile(img, 0.05, val);
  TEST("vil_math_value_range_percentile():  5 %", val==5, true);
  vil_math_value_range_percentile(img, 0.95, val);
  TEST("vil_math_value_range_percentile(): 95 %", val==95, true);  

  // Test a likely pair of percentiles
  vil_math_value_range_percentile(img, 0.10, val);
  TEST("vil_math_value_range_percentile(): 10 %", val==10, true);
  vil_math_value_range_percentile(img, 0.90, val);
  TEST("vil_math_value_range_percentile(): 90 %", val==90, true);  
  
  // Test an unlikely pair of percentiles
  vil_math_value_range_percentile(img, 0.31, val);
  TEST("vil_math_value_range_percentile(): 31 %", val==31, true);
  vil_math_value_range_percentile(img, 0.73, val);
  TEST("vil_math_value_range_percentile(): 73 %", val==73, true);  
}


TESTMAIN(test_math_value_range);
