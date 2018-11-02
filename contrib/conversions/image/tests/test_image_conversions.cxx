// This is conversions/image/tests/test_image_conversions.cxx
#include <iostream>
#include <cstring>
#include <fstream>
#include <testlib/testlib_test.h>

#ifdef HAS_NO_IMAGE

void test_image_conversions()
{
  std::cerr << "test not run since you do not have the TargetJr Image package\n";
}

#else
#include <image/Image_to_vil1.h>
#include <image/vil1_to_Image.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpl/vpl.h> // for vpl_unlink()
#include <vul/vul_temp_filename.h>
#include <vil1/vil1_load.h>

static void create_image(const char* name)
{
  // create a PPM image, just a very simple one (3x2)
  std::ofstream f(name);
  f << "P6\n3 2\n255\n111222333456567678";
  f.close();
}

void test_image_conversions()
{
  std::string filename = vul_temp_filename();
  if (filename == "") filename = "temp_image.ppm";
  create_image(filename.c_str());
  vil1_image im1 = vil1_load(filename.c_str());
  TEST("image file", (bool)im1, true);
  if (!im1) { std::cerr << filename << " is not a valid image file\n"; return; }
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
  vpl_unlink(filename.c_str());

  Image* im2 = vil1_to_Image(im1);
  TEST("vil1_to_Image width", wd, im2->GetSizeX());
  TEST("vil1_to_Image height", ht, im2->GetSizeY());
  char* buf2 = new char[size];
  im2->GetSection(buf2, 0, 0, wd, ht);
  TEST("vil1_to_Image data", 0, std::memcmp(buf1, buf2, size));

  vil1_image im3 = Image_to_vil1(im2);
  TEST("Image_to_vil1 width", im3.width(), im2->GetSizeX());
  TEST("Image_to_vil1 height", im3.height(), im2->GetSizeY());
  im3.get_section(buf1, 0, 0, wd, ht);
  TEST("Image_to_vil1 data", 0, std::memcmp(buf1, buf2, size));

  delete[] buf1; delete[] buf2;
}

#endif // HAS_NO_IMAGE

TESTMAIN(test_image_conversions);
