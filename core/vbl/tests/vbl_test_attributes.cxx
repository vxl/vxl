// This is core/vbl/tests/vbl_test_attributes.cxx
#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/vbl_array_1d.h>
#include <vbl/vbl_array_2d.h>
#include <vbl/vbl_array_3d.h>
#include <vbl/vbl_attributes.h>

static void vbl_test_attributes_1d()
{
  std::cout << "\n Testing vbl_attributes 1d\n +++++++++++++++++++++++++\n\n";
  constexpr unsigned n = 10;
  vbl_array_1d<double> v(n,1.0);
  // set up minima at middle and two ends
  v[0]=0.06;   v[n/2]= 0.05;  v[n-1]=0.04; v[1]=10.0;
  double mnv = minval(v), mxv = maxval(v);
  double mnvb = 0.0, mxvb = 0.0;
  bounds(v, mnvb, mxvb);
  double mn = mean(v);
  TEST("min,max,bounds 1d", mnv  == 0.04 && mxv  == 10.0
                         && mnvb == 0.04 && mxvb == 10.0, true);
  TEST_NEAR("mean 1d", mn, 1.615, 1e-10);
  mn = median(v);
  TEST("median 1d", mn, 1.0);
}

static void vbl_test_attributes_2d()
{
  std::cout << "\n Testing vbl_attributes 2d\n +++++++++++++++++++++++++\n\n";
  const unsigned nr = 10, nc = 5;
  vbl_array_2d<double> v(nr, nc, 1.0);
  v[0][0]=0.06;   v[nr/2][0]= 0.05;  v[0][nc-1]=0.04; v[0][1]=10.0;
  double mnv = minval(v), mxv = maxval(v);
  double mnvb = 0.0, mxvb = 0.0;
  bounds(v, mnvb, mxvb);
  double mn = mean(v);
  TEST("min,max,bounds 2d", mnv  == 0.04 && mxv  == 10.0
                         && mnvb == 0.04 && mxvb == 10.0, true);
  TEST_NEAR("mean 2d", mn, 1.123, 1e-10);
  mn = median(v);
  TEST("median 2d", mn, 1.0);
}

static void vbl_test_attributes_3d()
{
  std::cout << "\n Testing vbl_attributes 3d\n +++++++++++++++++++++++++\n\n";
  const unsigned n1 = 5, n2 = 4, n3 = 10;
  vbl_array_3d<double> v(n1, n2, n3, 1.0);
   v[0][0][0]=0.06;   v[n1/2][0][3]= 0.05;  v[0][n2-1][4]=0.04; v[0][1][5]=10.0;
  double mnv = minval(v), mxv = maxval(v);
  double mnvb = 0.0, mxvb = 0.0;
  bounds(v, mnvb, mxvb);
  double mn = mean(v);
  TEST("min,max,bounds 3d", mnv  == 0.04 && mxv  == 10.0
                         && mnvb == 0.04 && mxvb == 10.0, true);
  TEST_NEAR("mean 3d", mn, 1.03075, 1e-10);
  mn = median(v);
  TEST("median 3d", mn, 1.0);
}

static void vbl_test_attributes()
{
  vbl_test_attributes_1d();
  vbl_test_attributes_2d();
  vbl_test_attributes_3d();
}

TESTMAIN(vbl_test_attributes);
