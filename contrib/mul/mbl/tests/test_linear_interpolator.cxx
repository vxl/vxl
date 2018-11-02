// This is mul/mbl/tests/test_linear_interpolator.cxx
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h>
#include <mbl/mbl_linear_interpolator.h>
#include <testlib/testlib_test.h>

double func(double x)
{
  return 3.2*x-6;
}

double diff_li(const mbl_linear_interpolator &li, double x)
{
  double d=li.y(x)-func(x);
  if (d<0) d=-d;
  return d;
}

void test_linear_interpolator()
{
  std::cout << "*********************************\n"
           << " Testing mbl_linear_interpolator\n"
           << "*********************************\n";

  mbl_linear_interpolator li;

  // tabulate line of y=3.2x-7
  std::vector<double> x;
  std::vector<double> y;
  double v1=-4;
  double v2=8;
  for (double i=v1;i<=v2;++i)
  {
    x.push_back(i);
    y.push_back(func(i));
  }

  TEST("No data so return an NaN",vnl_math::isnan(li.y(1.5)),true);

  li.set(x,y);

  // test some values inside the tabulated interval
  TEST("Y value interpolated correctly",bool(diff_li(li,1.5)<1e-8),true);
  TEST("Y value interpolated correctly",bool(diff_li(li,-2)<1e-8),true);
  TEST("Y value interpolated correctly",bool(diff_li(li,4.2)<1e-8),true);
  TEST("Y value interpolated correctly",bool(diff_li(li,6.9)<1e-8),true);
  TEST("Y value interpolated correctly",bool(diff_li(li,2.45)<1e-8),true);

  // test some values outside the tabulated interval
  TEST("Y value boundary condition correct",li.y(-10)==li.y(v1),true);
  TEST("Y value boundary condition correct",li.y(+20)==li.y(v2),true);

  li.clear();
  TEST("No data so return an NaN",vnl_math::isnan(li.y(1.5)),true);
}

TESTMAIN(test_linear_interpolator);
