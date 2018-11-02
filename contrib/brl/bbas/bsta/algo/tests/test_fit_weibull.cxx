#include <iostream>
#include <string>
#include <testlib/testlib_test.h>
#include <bsta/algo/bsta_fit_weibull.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T>
void test_fit_weibull_type(T epsilon, const std::string& type_name)
{
  T sample_mean = T(0.404), sample_std_dev = T(0.33);
  bsta_weibull_cost_function wcf(sample_mean, sample_std_dev);
  bsta_fit_weibull<T> fw(&wcf);
  T k = T(0);
  fw.init(k);
  fw.solve(k);
  T residual = fw.residual();
  TEST("residual", residual, residual);
  T lambda = fw.lambda(k);

  TEST_NEAR(("shape param k <"+type_name+">").c_str(),
            k, static_cast<T>(1.2309439), epsilon);

  TEST_NEAR(("shape param lambda <"+type_name+">").c_str(),
            lambda, static_cast<T>(0.43221056), epsilon);
}


static void test_fit_weibull()
{
  test_fit_weibull_type(1e-5f,"float");
  test_fit_weibull_type(1e-8 ,"double");
}

TESTMAIN(test_fit_weibull);
