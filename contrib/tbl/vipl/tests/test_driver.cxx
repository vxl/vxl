// This is tbl/vipl/tests/test_driver.cxx
#include <iostream>
#include <cmath>
#include "test_driver.h"
#include <testlib/testlib_register.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

DECLARE( vipl_test_histogram );
DECLARE( vipl_test_erode_disk );
DECLARE( vipl_test_threshold );
DECLARE( vipl_test_gaussian_convolution );

void
register_tests()
{
  REGISTER( vipl_test_histogram );
  REGISTER( vipl_test_erode_disk );
  REGISTER( vipl_test_threshold );
  REGISTER( vipl_test_gaussian_convolution );
}

DEFINE_MAIN;

// create an 8 bit test image
vil_image_view<vxl_byte> CreateTest8bitImage(int wd, int ht)
{
  vil_image_view<vxl_byte> image; image.set_size(wd, ht);
  for (int x = 0; x < wd; x++)
    for (int y = 0; y < ht; y++) {
      auto data = vxl_byte(((x-wd/2)*(y-ht/2)/16)%(1<<8));
      image(x,y) = data;
    }
  return image;
}

// create a 16 bit test image
vil_image_view<short> CreateTest16bitImage(int wd, int ht)
{
  vil_image_view<short> image; image.set_size(wd, ht);
  for (int x = 0; x < wd; x++)
    for (int y = 0; y < ht; y++) {
      auto data = short(((x-wd/2)*(y-ht/2)/16)%(1<<16));
      image(x,y) = data;
  }
  return image;
}

// create a float-pixel test image
vil_image_view<float> CreateTestfloatImage(int wd, int ht)
{
  vil_image_view<float> image; image.set_size(wd, ht);
  for (int x = 0; x < wd; x++)
    for (int y = 0; y < ht; y++) {
      float data = 0.01f * ((x-wd/2)*(y-ht/2)/16);
      image(x,y) = data;
    }
  return image;
}

template <class T>
bool difference(vil_image_view<T> const& a, vil_image_view<T> const& b, double v, std::string const& m, T)
{
  int sx = a.ni();
  int sy = a.nj();
  int wd = b.ni();
  int ht = b.nj();
  TEST("Size of images match", sx == wd && sy == ht, true);

  double val = 0.0;
  // run over all pixels except for an outer border of 1 pixel:
  for (int x=1; x+1<wd; ++x)
  for (int y=1; y+1<ht; ++y) {
    double d=(double)a(x,y) - (double)b(x,y);
    val += std::fabs(d);
  }
  std::cout<<m<<": expected "<<v<<", found "<<val<<std::endl;
  bool ret = (std::fabs(val - v) > 0.01*std::fabs(v));
  TEST(m.c_str(), ret, false);
  return ret;
}

template bool difference(vil_image_view<vxl_byte> const&, vil_image_view<vxl_byte> const&,
                         double, std::string const&, vxl_byte);
template bool difference(vil_image_view<short> const&, vil_image_view<short> const&,
                         double, std::string const&, short);
template bool difference(vil_image_view<float> const&, vil_image_view<float> const&,
                         double, std::string const&, float);
