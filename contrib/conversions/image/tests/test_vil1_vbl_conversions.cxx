#include <image/vbl_array_to_vil.h>
#include <image/vil_to_vbl_array.h>

#include <vcl_cstring.h> // for memcmp()
#include <vcl_cstdio.h> // for fopen() etc.
#include <vpl/vpl.h> // for vpl_unlink()
#include <vil/vil_load.h>
#include <vil/vil_test.h>

static void create_image(const char* name)
{
  // create a PGM image, just a very simple one (3x2)
  FILE* f = fopen(name, "wb");
  fprintf(f, "P5\n3 2\n255\n123456");
  fclose(f);
}

void test_image_conversions()
{
  char const* filename = tempnam(0,0);
  create_image(filename);
  vil_image im1 = vil_load(filename);
  TEST("image file", (bool)im1, true);
  if (!im1) { vcl_cerr << filename << " is not a valid image file\n"; return; }
  int size = im1.get_size_bytes();
  TEST("image size", size, 6);
  int wd = im1.width();
  TEST("image width", wd, 3);
  int ht = im1.height();
  TEST("image height", ht, 2);
  char* buf1 = new char[size];
  // load into memory
  im1.get_section(buf1, 0, 0, wd, ht);
  // now remove the file
  vpl_unlink(filename);

  vbl_array_2d<unsigned char> im2 = vil_to_vbl_array(im1);
  TEST("vil_to_vbl_array width", wd, im2.columns());
  TEST("vil_to_vbl_array height", ht, im2.rows());
  const unsigned char* buf2 = im2.begin();
  TEST("vil_to_vbl_array data", 0, memcmp(buf1, buf2, size));

  vil_image im3 = vbl_array_to_vil(im2);
  TEST("vbl_array_to_vil width", im3.width(), im2.columns())
  TEST("vbl_array_to_vil height", im3.height(), im2.rows())
  im3.get_section(buf1, 0, 0, wd, ht);
  TEST("vbl_array_to_vil data", 0, memcmp(buf1, buf2, size))

  delete[] buf1;
}

TESTMAIN(test_image_conversions);
