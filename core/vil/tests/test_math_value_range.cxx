// This is core/vil/tests/test_math_value_range.cxx

#include <iostream>
#include <limits>
#include "testlib/testlib_test.h"
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include "vil/vil_math.h"

template <class T>
void view_cout(const vil_image_view<T>& view)
{
  for (unsigned p = 0; p < view.nplanes(); ++p)
  {
    std::cout << "Plane " << p << "\n";
    for (unsigned j = 0; j < view.nj(); ++j)
    {
      for (unsigned i = 0; i < view.ni(); ++i)
      {
        std::cout << view(i, j, p) << ' ';
      }
      std::cout << '\n';
    }
  }
}


template <class T>
static void
_test_math_value_range(std::string type_name, bool test_nan=false)
{
  std::cout << "************************************************\n"
            << "test math_value_range vil_image_view<" << type_name << ">\n"
            << "************************************************\n";

  // Create a test image with values 1...100
  unsigned ni = 10, nj = 10;
  vil_image_view<T> img(ni, nj);
  for (unsigned i = 0; i < ni; ++i)
  {
    for (unsigned j = 0; j < nj; ++j)
    {
      T val = j * ni + i + 1; // NB Ensure that data values not already sorted!
      img(i, j) = val;
    }
  }

#ifdef DEBUG
  view_cout(img);
#endif

  // Check the min/max values
  T min, max;
  vil_math_value_range(img, min, max);
  TEST("vil_math_value_range(): min", min == 1, true);
  TEST("vil_math_value_range(): max", max == 100, true);

  T val = T(0); // initialised to avoid compiler warning

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
  std::vector<double> fraction = {0.00, 0.05, 0.10, 0.31, 0.50,
                                  0.73, 0.90, 0.95, 1.00};
  std::vector<T> true_values = {1, 5, 10, 31, 50, 73, 90, 95, 100};
  size_t nfrac = fraction.size();

  std::vector<T> values;
  vil_math_value_range_percentiles(img, fraction, values);
  std::cout << "true_value, value:\n";
  for (size_t f; f < nfrac; ++f)
  {
    std::cout << true_values[f] << "," << values[f] << "\n";
  }

  TEST("vil_math_value_range_percentiles()", values, true_values);

  // nan tests
  if (!test_nan)
    return;

  // pad img with nan values
  vil_image_view<T> img2(ni+2, nj+2);
  img2.fill(std::numeric_limits<T>::quiet_NaN());
  for (unsigned j = 0; j < nj; ++j)
  {
    for (unsigned i = 0; i < ni; ++i)
    {
      img2(i+1, j+1) = img(i, j);
    }
  }

#ifdef DEBUG
  view_cout(img2);
#endif

  std::vector<T> values2;
  vil_math_value_range_percentiles(img2, fraction, values2, true);
  std::cout << "true_value, value:\n";
  for (size_t f; f < nfrac; ++f)
  {
    std::cout << true_values[f] << "," << values2[f] << "\n";
  }

  TEST("vil_math_value_range_percentiles(ignore_nan)", values2, true_values);
}

static void
test_math_value_range()
{
  _test_math_value_range<vxl_byte>("vxl_byte");
  _test_math_value_range<int>("int");
  _test_math_value_range<float>("float", true);
  _test_math_value_range<double>("double", true);
}

TESTMAIN(test_math_value_range);
