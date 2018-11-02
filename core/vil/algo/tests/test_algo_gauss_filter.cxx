// This is core/vil/algo/tests/test_algo_gauss_filter.cxx
#include <iostream>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <cmath>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vil/vil_image_view.h>
#include <vil/vil_print.h>
#include <vil/algo/vil_gauss_filter.h>
#include <vxl_config.h>
#include <testlib/testlib_test.h>

static void test_algo_gaussian_filter_5tap_byte_float()
{
  std::cout << "******************************************\n"
           << " Testing vil_gauss_filter_5tap byte float\n"
           << "******************************************\n";

  constexpr int n = 10;

  vil_image_view<vxl_byte> src(n,n);
  for (int i=0;i<n;++i)
    for (int j=0;j<n;++j)
      src(i,j) = vxl_byte(j*10+i);
  vil_image_view<float> dest;
  vil_gauss_filter_5tap_params params(1.0);
  vil_gauss_filter_5tap(src, dest, params);
  std::cout << "Source\n";
  vil_print_all(std::cout,  src);
  std::cout << "Destination\n";
  vil_print_all(std::cout,  dest);

  bool fail=false;
  for (int i=2;i<n-2;++i)
    for (int j=2;j<n-2;++j)
      if (std::fabs(dest(i,j)-src(i,j)) > 0.5)
        fail = true;
  TEST("Central areas close to original", fail, false);

  fail=false;
  for (int i=0;i<n;++i)
    for (int j=0;j<n;++j)
      if (std::fabs(dest(i,j)-src(i,j)) > n/2)
        fail = true;
  TEST("Central areas not so close to original", fail, false);

  src.fill(0);
  src(n/2,n/2) = 255;

  vil_gauss_filter_5tap(src,dest, params);
  std::cout << "Source\n";
  vil_print_all(std::cout,  src);
  std::cout << "Destination\n";
  vil_print_all(std::cout,  dest);

  float sum = 0;
  bool fail1=false, fail2=false;
  for (int i=0;i<n;++i)
    for (int j=0;j<n;++j)
    {
      sum += dest(i,j);
      if (std::max(std::abs(i-n/2), std::abs(j-n/2)) > 2 && dest(i,j) != 0)
        fail1 = true;
      if (dest(i,j) > dest(n/2,n/2))
        fail2=true;
    }
  TEST("impulse response is contained", fail1, false);
  TEST("response is centre maximum", fail2, false);
  TEST_NEAR("impulse response sums to input", sum, 255.0f, 1e-3);

  std::cout << "**********Awkward cases***************\n";

  src.set_size(n,1);
  for (int i=0;i<n;++i)
    src(i,0) = vxl_byte((i%2)*10+i);
  vil_gauss_filter_5tap(src,dest, params);
  std::cout << "Source\n";
  vil_print_all(std::cout,  src);
  std::cout << "Destination\n";
  vil_print_all(std::cout,  dest);

  src.set_size(1,n);
  for (int i=0;i<n;++i)
    src(0,i) = vxl_byte((i%2)*10+i);
  vil_gauss_filter_5tap(src,dest, params);
  std::cout << "Source\n";
  vil_print_all(std::cout,  src);
  std::cout << "Destination\n";
  vil_print_all(std::cout,  dest);

  src.set_size(n,2);
  for (int i=0;i<n;++i)
  {
    src(i,0) = vxl_byte((i%2)*10+i);
    src(i,1) = vxl_byte(((i+1)%2)*10+i);
  }
  vil_gauss_filter_5tap(src,dest, params);
  std::cout << "Source\n";
  vil_print_all(std::cout,  src);
  std::cout << "Destination\n";
  vil_print_all(std::cout,  dest);

  src.set_size(2,n);
  for (int i=0;i<n;++i)
  {
    src(0,i) = vxl_byte((i%2)*10+i);
    src(1,i) = vxl_byte(((i+1)%2)*10+i);
  }
  vil_gauss_filter_5tap(src,dest, params);
  std::cout << "Source\n";
  vil_print_all(std::cout,  src);
  std::cout << "Destination\n";
  vil_print_all(std::cout,  dest);

  src.set_size(3,n);
  for (int i=0;i<n;++i)
  {
    src(0,i) = vxl_byte((i%3)*10+i);
    src(1,i) = vxl_byte(((i+1)%3)*10+i);
    src(2,i) = vxl_byte(((i+2)%3)*10+i);
  }
  vil_gauss_filter_5tap(src,dest, params);
  std::cout << "Source\n";
  vil_print_all(std::cout,  src);
  std::cout << "Destination\n";
  vil_print_all(std::cout,  dest);

  src.set_size(n,2);
  for (int i=0;i<n;++i)
  {
    src(i,0) = vxl_byte((i%2)*10+i);
    src(i,1) = vxl_byte(((i+1)%2)*10+i);
  }
  vil_gauss_filter_5tap(src,dest, params);
  std::cout << "Source\n";
  vil_print_all(std::cout,  src);
  std::cout << "Destination\n";
  vil_print_all(std::cout,  dest);

  src.set_size(3,3);
  for (int j=0;j<3;++j)
    for (int i=0;i<3;++i)
      src(i,j) = vxl_byte(i*10+j);
  vil_gauss_filter_5tap(src,dest, params);
  std::cout << "Source\n";
  vil_print_all(std::cout,  src);
  std::cout << "Destination\n";
  vil_print_all(std::cout,  dest);

  src.set_size(4,4);
  for (int j=0;j<4;++j)
    for (int i=0;i<4;++i)
      src(i,j) = vxl_byte(i*10+j);
  vil_gauss_filter_5tap(src,dest, params);
  std::cout << "Source\n";
  vil_print_all(std::cout,  src);
  std::cout << "Destination\n";
  vil_print_all(std::cout,  dest);
}


static void test_algo_vil_gauss_filter_gen_ntaps()
{
  std::cout << "***********************************\n"
           << " Testing vil_gauss_filter_gen_ntap\n"
           << "***********************************\n\n";
  std::vector<double> filter1(7);
  std::cout << "filter1 = 7 tap gaussian(sd=1.5)\n";
  vil_gauss_filter_gen_ntap(1.5, 0, filter1);
#ifdef DEBUG
  for (unsigned i=0; i<7; ++i)
    std::cout << ' ' << filter1[i];
  std::cout << std::endl;
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
  std::cout << "filter2 = 7 tap (d/dx)^2 gaussian(sd=1.5)\n";
  std::vector<double> filter2(7);
  vil_gauss_filter_gen_ntap(1.5, 2, filter2); // This is not a useful filter - it has significant truncation.
  for (unsigned i=0; i<7; ++i)
  {
    TEST_NEAR("filter2", filter2[i], correct_filter2[i], 1e-7);
  }

  const double correct_filter3[] = {0.00000000000003,  0.00000028665154,
    0.00620937867420,  0.49379033467422, 0.49379033467422,
    0.00620937867420,  0.00000028665154, 0.00000000000003 };
  std::cout << "filter3 = 8 tap  gaussian(sd=0.4)\n";
  std::vector<double> filter3(8);
  vil_gauss_filter_gen_ntap(0.4, 0, filter3);
  for (unsigned i=0; i<8; ++i)
  {
    // use log to get even tolerances
    TEST_NEAR("filter3", std::log(filter3[i]), std::log(correct_filter3[i]), 0.3);
  }

  const double correct_filter4[] = {0.18846314959135, 0.16222013702815,
    0.11021993712191, 0.03909677625858, -0.03909677625858, -0.11021993712191,
    -0.16222013702815,  -0.18846314959135 };
  std::cout << "filter4 = 8 tap (d/dx)^5 gaussian(sd=4.0)\n";
  std::vector<double> filter4(8);
  vil_gauss_filter_gen_ntap(4.0, 1, filter4); // This is not a useful filter - it has extreme truncation.
  for (unsigned i=0; i<8; ++i)
  {
    TEST_NEAR("filter4", filter4[i], correct_filter4[i], 1e-7);
  }
}

void test_algo_gauss_filter_1d()
{
  std::cout << "*****************************\n"
           << " Testing vil_gauss_filter_1d\n"
           << "*****************************\n\n";

  unsigned nx=15,ny=17;
  vil_image_view<vxl_byte> src_im(2*nx+1,2*ny+1);
  src_im.fill(vxl_byte(0));
  src_im(nx,ny)=100;  // Centre pixel
  vil_image_view<float> dest_im;

  double sd = 2.5;
  unsigned half_width = 8;
  vil_gauss_filter_1d(src_im,dest_im,sd,half_width);
  TEST("Output image ni",dest_im.ni(),src_im.ni());
  TEST("Output image nj",dest_im.nj(),src_im.nj());

  // Generate filter
  std::vector<double> filter(2*half_width+1);
  vil_gauss_filter_gen_ntap(sd,0,filter);
  TEST_NEAR("Value beyond filter",dest_im(nx-(half_width+1),ny),0,1e-6);
  TEST_NEAR("Value at limit of filter",dest_im(nx-half_width,ny),100*filter[0],1e-6);
  TEST_NEAR("Value at limit of filter",dest_im(nx+half_width,ny),100*filter[0],1e-6);
  TEST_NEAR("Value at centre of filter",dest_im(nx,ny),100*filter[half_width],1e-6);
}

void test_algo_gauss_filter_2d()
{
  std::cout << "*****************************\n"
           << " Testing vil_gauss_filter_2d\n"
           << "*****************************\n\n";

  unsigned nx=15,ny=17;
  vil_image_view<vxl_byte> src_im(2*nx+1,2*ny+1);
  src_im.fill(vxl_byte(0));
  src_im(nx,ny)=100;  // Centre pixel
  vil_image_view<float> dest_im;

  double sd = 2.5;
  unsigned half_width = 8;
  vil_gauss_filter_2d(src_im,dest_im,sd,half_width);
  TEST("Output image ni",dest_im.ni(),src_im.ni());
  TEST("Output image nj",dest_im.nj(),src_im.nj());

  // Generate filter
  std::vector<double> filter(2*half_width+1);
  vil_gauss_filter_gen_ntap(sd,0,filter);
  double f_c = filter[half_width];  // Centre value of filter
  double f_0 = filter[0];           // Value at end of filter
  TEST_NEAR("Value beyond filter",dest_im(nx-(half_width+1),0),0,1e-6);
  TEST_NEAR("Value at limit of filter",dest_im(nx-half_width,ny),100*f_0*f_c,1e-6);
  TEST_NEAR("Value at limit of filter",dest_im(nx+half_width,ny),100*f_0*f_c,1e-6);
  TEST_NEAR("Value at centre of filter",dest_im(nx,ny),100*f_c*f_c,1e-6);
  TEST_NEAR("Value at corner",dest_im(nx+half_width,ny+half_width),100*f_0*f_0,1e-6);
  TEST_NEAR("Value at corner",dest_im(nx-half_width,ny-half_width),100*f_0*f_0,1e-6);
}

static void test_algo_gauss_filter()
{
  test_algo_vil_gauss_filter_gen_ntaps();
  test_algo_gaussian_filter_5tap_byte_float();
  test_algo_gauss_filter_1d();
  test_algo_gauss_filter_2d();
}

TESTMAIN(test_algo_gauss_filter);
