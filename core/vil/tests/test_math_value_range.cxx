// This is core/vil/tests/test_math_value_range.cxx

#include <iostream>
#include "testlib/testlib_test.h"
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include "vil/vil_math.h"


static void
test_math_value_range()
{
  std::cout << "******************************\n"
            << " Testing vil_math_value_range\n"
            << "******************************\n";

  // Create a test image with values 1...100
  unsigned ni = 10, nj = 10;
  vil_image_view<int> img(ni, nj);
  for (unsigned i = 0; i < ni; ++i)
  {
    for (unsigned j = 0; j < nj; ++j)
    {
      int val = j * ni + i + 1; // NB Ensure that data values not already sorted!
      img(i, j) = val;
#ifdef DEBUG
      std::cout << val << ' ';
#endif
    }
#ifdef DEBUG
    std::cout << val << '\n';
#endif
  }

  // Check the min/max values
  int min, max;
  vil_math_value_range(img, min, max);
  TEST("vil_math_value_range(): min", min == 1, true);
  TEST("vil_math_value_range(): max", max == 100, true);

  int val = 0; // initialised to avoid compiler warning

  // Test a likely pair of percentiles
  vil_math_value_range_percentile(img, 0.05, val);
  TEST("vil_math_value_range_percentile():  5 %", val == 5, true);
  vil_math_value_range_percentile(img, 0.95, val);
  TEST("vil_math_value_range_percentile(): 95 %", val == 95, true);

  // Test a likely pair of percentiles
  vil_math_value_range_percentile(img, 0.10, val);
  TEST("vil_math_value_range_percentile(): 10 %", val == 10, true);
  vil_math_value_range_percentile(img, 0.90, val);
  TEST("vil_math_value_range_percentile(): 90 %", val == 90, true);

  // Test an unlikely pair of percentiles
  vil_math_value_range_percentile(img, 0.31, val);
  TEST("vil_math_value_range_percentile(): 31 %", val == 31, true);
  vil_math_value_range_percentile(img, 0.73, val);
  TEST("vil_math_value_range_percentile(): 73 %", val == 73, true);

  // Test several percentiles at once
  unsigned int nfrac = 9;
  std::vector<double> fraction(nfrac);
  std::vector<double> true_value(nfrac);
  fraction[0] = 0.00;
  true_value[0] = 1;
  fraction[1] = 0.05;
  true_value[1] = 5;
  fraction[2] = 0.10;
  true_value[2] = 10;
  fraction[3] = 0.31;
  true_value[3] = 31;
  fraction[4] = 0.50;
  true_value[4] = 50;
  fraction[5] = 0.73;
  true_value[5] = 73;
  fraction[6] = 0.90;
  true_value[6] = 90;
  fraction[7] = 0.95;
  true_value[7] = 95;
  fraction[8] = 1.00;
  true_value[8] = 100;
  std::vector<int> value;
  vil_math_value_range_percentiles(img, fraction, value);
  bool all_correct = true;
  for (unsigned f = 0; f < nfrac; ++f)
  {
    if (value[f] != true_value[f])
    {
      all_correct = false;
    }
  }
  TEST("vil_math_value_range_percentiles(): all correct", all_correct, true);
}

TESTMAIN(test_math_value_range);
