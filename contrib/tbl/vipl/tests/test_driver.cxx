// This is tbl/vipl/tests/test_driver.cxx
#include "test_driver.h"
#include <testlib/testlib_register.h>
#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_rgb_byte.h>
#include <vcl_cmath.h> // for vcl_fabs()

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
vil1_image CreateTest8bitImage(int wd, int ht)
{
  vil1_memory_image_of<unsigned char> image(wd, ht);
  for (int x = 0; x < wd; x++)
    for (int y = 0; y < ht; y++) {
      unsigned char data = ((x-wd/2)*(y-ht/2)/16) % (1<<8);
      image.put_section(&data, x, y, 1, 1);
    }
  return image;
}

// create a 16 bit test image
vil1_image CreateTest16bitImage(int wd, int ht)
{
  vil1_memory_image_of<unsigned short> image(wd, ht);
  for (int x = 0; x < wd; x++)
    for (int y = 0; y < ht; y++) {
      unsigned short data = ((x-wd/2)*(y-ht/2)/16) % (1<<16);
      image.put_section(&data, x, y, 1, 1);
  }
  return image;
}

// create a 24 bit color test image
vil1_image CreateTest24bitImage(int wd, int ht)
{
  vil1_memory_image_of<vil1_rgb_byte> image(wd, ht);
  for (int x = 0; x < wd; x++)
    for (int y = 0; y < ht; y++) {
      unsigned char data[3] = { x%(1<<8), ((x-wd/2)*(y-ht/2)/16) % (1<<8), ((y/3)%(1<<8)) };
      image.put_section(data, x, y, 1, 1);
    }
  return image;
}

// create a 24 bit color test image, with 3 planes
vil1_image CreateTest3planeImage(int wd, int ht)
{
  vil1_memory_image image(3, wd, ht, 1, 8, VIL1_COMPONENT_FORMAT_UNSIGNED_INT);
  for (int x = 0; x < wd; x++)
    for (int y = 0; y < ht; y++) {
      unsigned char data[3] = { x%(1<<8), ((x-wd/2)*(y-ht/2)/16) % (1<<8), ((y/3)%(1<<8)) };
      image.put_section(data, x, y, 1, 1);
    }
  return image;
}

// create a float-pixel test image
vil1_image CreateTestfloatImage(int wd, int ht)
{
  vil1_memory_image_of<float> image(wd, ht);
  for (int x = 0; x < wd; x++)
    for (int y = 0; y < ht; y++) {
      float data = 0.01f * ((x-wd/2)*(y-ht/2)/16);
      image.put_section(&data, x, y, 1, 1);
    }
  return image;
}

#if 0
// create an 8 bit test image
mil_image_2d_of<unsigned char> Create_mil8bitImage(int wd, int ht)
{
  mil_image_2d_of<unsigned char> image(wd, ht);
  for (int x = 0; x < wd; x++)
    for (int y = 0; y < ht; y++) {
      unsigned char data = ((x-wd/2)*(y-ht/2)/16) % (1<<8);
      image(x,y) = data;
    }
  return image;
}

// create a 16 bit test image
mil_image_2d_of<short> Create_mil16bitImage(int wd, int ht)
{
  mil_image_2d_of<short> image(wd, ht);
  for (int x = 0; x < wd; x++)
    for (int y = 0; y < ht; y++) {
      short data = ((x-wd/2)*(y-ht/2)/16) % (1<<16);
      image(x,y) = data;
  }
  return image;
}

// create a float-pixel test image
mil_image_2d_of<float> Create_milfloatImage(int wd, int ht)
{
  mil_image_2d_of<float> image(wd, ht);
  for (int x = 0; x < wd; x++)
    for (int y = 0; y < ht; y++) {
      float data = 0.01f * ((x-wd/2)*(y-ht/2)/16);
      image(x,y) = data;
    }
  return image;
}
#endif

// Compare two images and return true (=failure) if their difference is not v
template <class T>
bool difference(vil1_image const& a, vil1_image const& b, double v, vcl_string const& m, T)
{
  int sx = a.width();
  int sy = a.height();
  int wd = b.width();
  int ht = b.height();
  TEST("Size of images match", sx == wd && sy == ht, true);

  double val = 0.0;
  // run over all pixels except for an outer border of 1 pixel:
  int siz = (sx-2)*(sy-2)*a.planes()*a.components()*(a.bits_per_component()/8);
  char* v1 = new char[siz]; a.get_section(v1,1,1,sx-2,sy-2);
  char* v2 = new char[siz]; b.get_section(v2,1,1,sx-2,sy-2);
  for (int i=0; i<(sx-2)*(sy-2); ++i) {
    double d=(double)(((T*)v1)[i])-(double)(((T*)v2)[i]);
    val += vcl_fabs(d);
  }
  delete[] v1; delete[] v2;
  vcl_cout<<m<<": expected "<<v<<", found "<<val<<vcl_endl;
  bool ret = (vcl_fabs(val - v) > 0.01*vcl_fabs(v));
  TEST(m.c_str(), ret, false);
  return ret;
}

template bool difference(vil1_image const&, vil1_image const&, double, vcl_string const&, unsigned char);
template bool difference(vil1_image const&, vil1_image const&, double, vcl_string const&, unsigned short);
template bool difference(vil1_image const&, vil1_image const&, double, vcl_string const&, signed short);
template bool difference(vil1_image const&, vil1_image const&, double, vcl_string const&, float);

#if 0
template <class T>
bool difference(mil_image_2d_of<T> const& a, mil_image_2d_of<T> const& b, double v, vcl_string const& m, T)
{
  int sx = a.nx();
  int sy = a.ny();
  int wd = b.nx();
  int ht = b.ny();
  TEST("Size of images match", sx == wd && sy == ht, true);

  double val = 0.0;
  // run over all pixels except for an outer border of 1 pixel:
  for (int x=1; x+1<wd; ++x)
  for (int y=1; y+1<ht; ++y) {
    double d=(double)a(x,y) - (double)b(x,y);
    val += vcl_fabs(d);
  }
  vcl_cout<<m<<": expected "<<v<<", found "<<val<<vcl_endl;
  bool ret = (vcl_fabs(val - v) > 0.01*vcl_fabs(v));
  TEST(m.c_str(), ret, false);
  return ret;
}

template bool difference(mil_image_2d_of<unsigned char> const&, mil_image_2d_of<unsigned char> const&,
                         double, vcl_string const&, unsigned char);
template bool difference(mil_image_2d_of<short> const&, mil_image_2d_of<short> const&,
                         double, vcl_string const&, short);
template bool difference(mil_image_2d_of<float> const&, mil_image_2d_of<float> const&,
                         double, vcl_string const&, float);
#endif
