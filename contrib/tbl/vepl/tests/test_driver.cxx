// This is tbl/vepl/tests/test_driver.cxx
#include <testlib/testlib_register.h>
#include "test_driver.h"
#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_rgb_byte.h>
#include <vcl_string.h>
#include <vxl_config.h> // for vxl_byte

DECLARE( vepl_test_erode_disk );
DECLARE( vepl_test_threshold );

void
register_tests()
{
  REGISTER( vepl_test_erode_disk );
  REGISTER( vepl_test_threshold );
}

DEFINE_MAIN;

// create an 8 bit test image
vil1_image CreateTest8bitImage(int wd, int ht)
{
  vil1_memory_image_of<vxl_byte> image(wd, ht);
  for (int x = 0; x < wd; x++)
    for (int y = 0; y < ht; y++) {
      vxl_byte data = ((x-wd/2)*(y-ht/2)/16) % (1<<8);
      image.put_section(&data, x, y, 1, 1);
    }
  return image;
}

// create a 16 bit test image
vil1_image CreateTest16bitImage(int wd, int ht)
{
  vil1_memory_image_of<vxl_uint_16> image(wd, ht);
  for (int x = 0; x < wd; x++)
    for (int y = 0; y < ht; y++) {
      vxl_uint_16 data = ((x-wd/2)*(y-ht/2)/16) % (1<<16);
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
      vxl_byte data[3] = { x%(1<<8), ((x-wd/2)*(y-ht/2)/16) % (1<<8), ((y/3)%(1<<8)) };
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
      vxl_byte data[3] = { x%(1<<8), ((x-wd/2)*(y-ht/2)/16) % (1<<8), ((y/3)%(1<<8)) };
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

// Compare two images and return true if their difference is not v
bool difference(vil1_image const& a, vil1_image const& b, int v, vcl_string const& m)
{
  int sx = a.width(),  sy = a.height();
  TEST("Size of images match", sx == b.width() && sy == b.height(), true);
  TEST("# planes/components match", a.planes() == b.planes() && a.components() == b.components(), true);
  TEST("Image formats match", a.component_format(), b.component_format());
  TEST("Bits per component match", a.bits_per_component(), b.bits_per_component());

  int ret = 0;
  // run over all pixels except for an outer border of 1 pixel:
  int siz = (sx-2)*(sy-2)*a.planes()*a.components()*(a.bits_per_component()/8);
  char* v1 = new char[siz]; a.get_section(v1,1,1,sx-2,sy-2);
  char* v2 = new char[siz]; b.get_section(v2,1,1,sx-2,sy-2);
  for (int i=0; i<siz; ++i) {
    int d;
#define DIFF(T) d=((int)(((T*)(v1))[i]))-((int)(((T*)(v2))[i])); if (d<0) d = -d
    if (a.component_format() == VIL1_COMPONENT_FORMAT_IEEE_FLOAT)
      switch (a.bits_per_component()) {
        case sizeof(float): DIFF(float); break;
        case sizeof(double): DIFF(double); break;
        default: d = 0;
      }
    else
      switch (a.bits_per_component()) {
        case 1: DIFF(vxl_byte); break;
        case 2: DIFF(vxl_uint_16); break;
        case 3: DIFF(vxl_byte); break;
        case 4: DIFF(int); break;
        default: d = 0;
      }
    ret += d;
  }
  delete[] v1; delete[] v2;
  ret /= a.planes()*a.components();
  vcl_cout<<m<<": expected "<<v<<", found "<<ret<<vcl_endl;
  TEST(m.c_str(), ret, v);
  return v!=ret;
}
