// This is core/vil/tests/test_algo_gauss_filter.cxx
#include <vcl_iostream.h>
#include <vcl_algorithm.h>
#include <vcl_vector.h>
#include <vcl_cstdlib.h> // for abs(int)
#include <vcl_cmath.h> // for fabs()

#include <vil/vil_image_view.h>
#include <vil/vil_print.h>
#include <vil/algo/vil_gauss_filter.h>
#include <vxl_config.h>
#include <testlib/testlib_test.h>

static void test_algo_gaussian_filter_5tap_byte_float()
{
  vcl_cout << "******************************************\n"
           << " Testing vil_gauss_filter_5tap byte float\n"
           << "******************************************\n";

  const int n = 10;

  vil_image_view<vxl_byte> src(n,n);
  for (int i=0;i<n;++i)
    for (int j=0;j<n;++j)
      src(i,j)=j*10+i;
  vil_image_view<float> dest;
  vil_gauss_filter_5tap_params params(1.0);
  vil_gauss_filter_5tap(src, dest, params);
  vcl_cout << "Source\n";
  vil_print_all(vcl_cout,  src);
  vcl_cout << "Destination\n";
  vil_print_all(vcl_cout,  dest);


  bool fail=false;
  for (int i=2;i<n-2;++i)
    for (int j=2;j<n-2;++j)
      if (vcl_fabs(dest(i,j)-src(i,j)) > 0.5) fail = true;
  TEST("Central areas close to original", fail, false);

  fail=false;
  for (int i=0;i<n;++i)
    for (int j=0;j<n;++j)
      if (vcl_fabs(dest(i,j)-src(i,j)) > n/2)
        fail = true;
  TEST("Central areas not so close to original", fail, false);

  src.fill(0);
  src(n/2,n/2) = 255;

  vil_gauss_filter_5tap(src,dest, params);
  vcl_cout << "Source\n";
  vil_print_all(vcl_cout,  src);
  vcl_cout << "Destination\n";
  vil_print_all(vcl_cout,  dest);


  float sum = 0;
  bool fail1=false, fail2=false;
  for (int i=0;i<n;++i)
    for (int j=0;j<n;++j)
    {
      sum += dest(i,j);
      if (vcl_max(vcl_abs(i-n/2), vcl_abs(j-n/2)) > 2 && dest(i,j) != 0)
        fail1 = true;
      if (dest(i,j) > dest(n/2,n/2)) fail2=true;
    }
  TEST("impulse response is contained", fail1, false);
  TEST("response is centre maximum", fail2, false);
  TEST_NEAR("impulse response sums to input", sum, 255.0f, 1e-3);


  vcl_cout << "**********Awkward cases***************" << vcl_endl;

  src.set_size(n,1);
  for (int i=0;i<n;++i)
    src(i,0)=(i%2)*10+i;
  vil_gauss_filter_5tap(src,dest, params);
  vcl_cout << "Source\n";
  vil_print_all(vcl_cout,  src);
  vcl_cout << "Destination\n";
  vil_print_all(vcl_cout,  dest);

  src.set_size(n,2);
  for (int i=0;i<n;++i)
  {
    src(i,0)=(i%2)*10+i;
    src(i,1)=((i+1)%2)*10+i;
  }
  vil_gauss_filter_5tap(src,dest, params);
  vcl_cout << "Source\n";
  vil_print_all(vcl_cout,  src);
  vcl_cout << "Destination\n";
  vil_print_all(vcl_cout,  dest);

  src.set_size(3,n);
  for (int i=0;i<n;++i)
  {
    src(0,i)=(i%3)*10+i;
    src(1,i)=((i+1)%3)*10+i;
    src(2,i)=((i+2)%3)*10+i;
  }
  vil_gauss_filter_5tap(src,dest, params);
  vcl_cout << "Source\n";
  vil_print_all(vcl_cout,  src);
  vcl_cout << "Destination\n";
  vil_print_all(vcl_cout,  dest);

  src.set_size(n,2);
  for (int i=0;i<n;++i)
  {
    src(i,0)=(i%2)*10+i;
    src(i,1)=((i+1)%2)*10+i;
  }
  vil_gauss_filter_5tap(src,dest, params);
  vcl_cout << "Source\n";
  vil_print_all(vcl_cout,  src);
  vcl_cout << "Destination\n";
  vil_print_all(vcl_cout,  dest);

  src.set_size(3,3);
  for (int j=0;j<3;++j)
    for (int i=0;i<3;++i)
      src(i,j)=i*10+j;
  vil_gauss_filter_5tap(src,dest, params);
  vcl_cout << "Source\n";
  vil_print_all(vcl_cout,  src);
  vcl_cout << "Destination\n";
  vil_print_all(vcl_cout,  dest);

  src.set_size(4,4);
  for (int j=0;j<4;++j)
    for (int i=0;i<4;++i)
      src(i,j)=i*10+j;
  vil_gauss_filter_5tap(src,dest, params);
  vcl_cout << "Source\n";
  vil_print_all(vcl_cout,  src);
  vcl_cout << "Destination\n";
  vil_print_all(vcl_cout,  dest);

}


static void test_algo_vil_gauss_filter_gen_ntaps()
{
  vcl_cout << "***********************************\n"
           << " Testing vil_gauss_filter_gen_ntap\n"
           << "***********************************\n\n";
  vcl_vector<double> filter1(7);
  vcl_cout << "filter1 = 7 tap gaussian(sd=1.5)\n";
  vil_gauss_filter_gen_ntap(1.5, 0, filter1);
#ifdef DEBUG
  for (unsigned i=0; i<7; ++i)
    vcl_cout << ' ' << filter1[i];
  vcl_cout << vcl_endl;
#endif

  // golden data using matlab command
  // y=subs(int(diff('exp(-1/2*(x^2)/v)','x',xderiv),'a','b')); y/sum(abs(y))
  const double correct_filter1[] = {0.03873542550599,
    0.11308483122135, 0.21500681127034, 0.26634586400464,
    0.21500681127034, 0.11308483122135, 0.03873542550599};
  for (unsigned i=0; i<7; ++i)
  {
    TEST_NEAR("filter1", filter1[i], correct_filter1[i], 1e-7);
  }

  const double correct_filter2[] = {0.12372451934878,
     0.09009370132564, -0.13740317011788, -0.29755721841539,
    -0.13740317011788,  0.09009370132564,  0.12372451934878 };
  vcl_cout << "filter2 = 7 tap (d/dx)^2 gaussian(sd=1.5)\n";
  vcl_vector<double> filter2(7);
  vil_gauss_filter_gen_ntap(1.5, 2, filter2); // This is not a useful filter - it has significant truncation.
  for (unsigned i=0; i<7; ++i)
  {
    TEST_NEAR("filter2", filter2[i], correct_filter2[i], 1e-7);
  }

  const double correct_filter3[] = {0.00000000000003,  0.00000028665154,
    0.00620937867420,  0.49379033467422, 0.49379033467422,
    0.00620937867420,  0.00000028665154, 0.00000000000003 };
  vcl_cout << "filter3 = 8 tap  gaussian(sd=0.4)\n";
  vcl_vector<double> filter3(8);
  vil_gauss_filter_gen_ntap(0.4, 0, filter3);
  for (unsigned i=0; i<8; ++i)
  {
    // use log to get even tolerances
    TEST_NEAR("filter3", vcl_log(filter3[i]), vcl_log(correct_filter3[i]), 0.3);
  }


  const double correct_filter4[] = {0.18846314959135, 0.16222013702815,
    0.11021993712191, 0.03909677625858, -0.03909677625858, -0.11021993712191,
    -0.16222013702815,  -0.18846314959135 };
  vcl_cout << "filter4 = 8 tap (d/dx)^5 gaussian(sd=4.0)\n";
  vcl_vector<double> filter4(8);
  vil_gauss_filter_gen_ntap(4.0, 1, filter4); // This is not a useful filter - it has extreme truncation.
  for (unsigned i=0; i<8; ++i)
  {
    TEST_NEAR("filter4", filter4[i], correct_filter4[i], 1e-7);
  }



}


static void test_algo_gauss_filter()
{
  test_algo_vil_gauss_filter_gen_ntaps();
  test_algo_gaussian_filter_5tap_byte_float();
}

TESTMAIN(test_algo_gauss_filter);
