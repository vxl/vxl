#include <image/vbl_array_to_vil1.h>
#include <image/vil1_to_vbl_array.h>

#include <vcl_cstring.h> // for memcmp()
#include <vcl_fstream.h>
#include <vpl/vpl.h> // for vpl_unlink()
#include <vil1/vil1_load.h>
#include <testlib/testlib_test.h>
#include <vul/vul_temp_filename.h>

static void create_image(const char* name)
{
  // create a PGM image, just a very simple one (3x2)
  vcl_ofstream f(name);
  f << "P5\n3 2\n255\n123456";
  f.close();
}

static void test_vil1_vbl_conversions()
{
  vcl_string filename = vul_temp_filename();
  if (filename == "") filename = "vil1_vbl_test.pgm";
  create_image(filename.c_str());
  vil1_image im1 = vil1_load(filename.c_str());
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
  vpl_unlink(filename.c_str());

  vbl_array_2d<unsigned char> im2 = vil1_to_vbl_array(im1);
  TEST("vil1_to_vbl_array width", wd, im2.columns());
  TEST("vil1_to_vbl_array height", ht, im2.rows());
  const unsigned char* buf2 = im2.begin();
  TEST("vil1_to_vbl_array data", 0, vcl_memcmp(buf1, buf2, size));

  vil1_image im3 = vbl_array_to_vil1(im2);
  TEST("vbl_array_to_vil1 width", im3.width(), im2.columns());
  TEST("vbl_array_to_vil1 height", im3.height(), im2.rows());
  im3.get_section(buf1, 0, 0, wd, ht);
  TEST("vbl_array_to_vil1 data", 0, vcl_memcmp(buf1, buf2, size));

  delete[] buf1;
}

TESTMAIN(test_vil1_vbl_conversions);
