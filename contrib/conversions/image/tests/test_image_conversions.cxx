#include <image/Image_to_vil.h>
#include <image/vil_to_Image.h>
//#include <image/vbl_array_to_vil.h>
//#include <image/vil_to_vbl_array.h>

#include <vcl_cstring.h> // for memcmp()
#include <vcl_cstdio.h> // for fopen() etc.
#include <vpl/vpl.h> // for vpl_unlink()
#include <vul/vul_temp_filename.h>
#include <vil/vil_load.h>
#include <vil/vil_test.h>

static void create_image(const char* name)
{
  // create a PPM image, just a very simple one (3x2)
  FILE* f = fopen(name, "wb");
  fprintf(f, "P6\n3 2\n255\n111222333456567678");
  fclose(f);
}

void test_image_conversions()
{
  char const* filename = vul_temp_filename().c_str();
  create_image(filename);
  vil_image im1 = vil_load(filename);
  TEST("image file", (bool)im1, true);
  if (!im1) { vcl_cerr << filename << " is not a valid image file\n"; return; }
  int size = im1.get_size_bytes();
  TEST("image size", size, 18);
  int wd = im1.width();
  TEST("image width", wd, 3);
  int ht = im1.height();
  TEST("image height", ht, 2);
  char* buf1 = new char[size];
  // load into memory
  im1.get_section(buf1, 0, 0, wd, ht);
  // now remove the file
  vpl_unlink(filename);

  Image* im2 = vil_to_Image(im1);
  TEST("vil_to_Image width", wd, im2->GetSizeX());
  TEST("vil_to_Image height", ht, im2->GetSizeY());
  char* buf2 = new char[size];
  im2->GetSection(buf2, 0, 0, wd, ht);
  TEST("vil_to_Image data", 0, memcmp(buf1, buf2, size));

  vil_image im3 = Image_to_vil(im2);
  TEST("Image_to_vil width", im3.width(), im2->GetSizeX())
  TEST("Image_to_vil height", im3.height(), im2->GetSizeY())
  im3.get_section(buf1, 0, 0, wd, ht);
  TEST("Image_to_vil data", 0, memcmp(buf1, buf2, size))

  delete[] buf1; delete[] buf2;
}

TESTMAIN(test_image_conversions);
