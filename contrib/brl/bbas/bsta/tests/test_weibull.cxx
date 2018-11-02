#include <iostream>
#include <string>
#include <testlib/testlib_test.h>
#include <bsta/bsta_weibull.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T>
void test_weibull_type(T epsilon, const std::string& type_name)
{
  //test default constructor
  bsta_weibull<T> wb;
  T mean = wb.mean();
  T var = wb.var();
  T cvar = wb.covar();
  TEST_NEAR(("default constructor <"+type_name+">").c_str(),
            mean*var*cvar, static_cast<T>(1.0), epsilon);
  //test full constructor, mean, variance
  T lambda = static_cast<T>(1.0);
  T k = static_cast<T>(2.0);
  T mu = static_cast<T>(1.0);
  bsta_weibull<T> wbf(lambda, k, mu);
  mean = wbf.mean();
  var = wbf.var();
  TEST_NEAR(("mean <"+type_name+">").c_str(),
            mean, static_cast<T>(1.886226925452758), epsilon);

  TEST_NEAR(("var <"+type_name+">").c_str(),
            var, static_cast<T>(0.21460183660255172), epsilon);
  //test probability density
  T x = static_cast<T>(2.0);
  T pd = wbf.prob_density(x);
  TEST_NEAR(("prob density <"+type_name+">").c_str(),
            pd, static_cast<T>(0.7357588823428847), epsilon);

  T xl = static_cast<T>(1.8);
  T xh = static_cast<T>(2.2);
  T p = wbf.probability(xl, xh);
  TEST_NEAR(("probability <"+type_name+">").c_str(),
            p, static_cast<T>(0.29036466536092675), epsilon);
}


static void test_weibull()
{
  test_weibull_type(1e-5f,"float");
  test_weibull_type(1e-13,"double");
}

TESTMAIN(test_weibull);
